#include "ui_system.h"
#include "task_presentation.h"

#include "localization.h"
#include "task_runtime_internal.h"
#include "ui_story_internal.h"
#include "ui_runtime_internal.h"
#include "ui_components_internal.h"

#include <cmath>
#include <cstdio>
#include <cstring>

namespace {

constexpr int kCommunicatorTabLogs = 1;
constexpr int kCommunicatorTabStory = 2;
constexpr int kCommunicatorTabCount = 3;
constexpr int kCommunicatorCenteredTextMaxLines = 12;

float ClampUnit(float value) {
    if (value < 0.0f) {
        return 0.0f;
    }
    if (value > 1.0f) {
        return 1.0f;
    }
    return value;
}

float SmoothStep(float value) {
    value = ClampUnit(value);
    return value * value * (3.0f - 2.0f * value);
}

int GetShownMainStorySceneCount(const bool *storySceneShown) {
    int count = 0;

    if (storySceneShown == nullptr) {
        return 0;
    }

    for (int scene = STORY_SCENE_MAIN_AIR_FOR_ONE_MORE_DAY;
         scene < STORY_SCENE_MAIN_AIR_FOR_ONE_MORE_DAY + STORY_MAIN_SCENE_COUNT;
         ++scene) {
        if (storySceneShown[scene]) {
            count++;
        }
    }

    return count;
}

StoryScene GetShownMainStorySceneAt(const bool *storySceneShown, int index) {
    int currentIndex = 0;

    if (storySceneShown == nullptr || index < 0) {
        return STORY_SCENE_NONE;
    }

    for (int scene = STORY_SCENE_MAIN_AIR_FOR_ONE_MORE_DAY;
         scene < STORY_SCENE_MAIN_AIR_FOR_ONE_MORE_DAY + STORY_MAIN_SCENE_COUNT;
         ++scene) {
        if (!storySceneShown[scene]) {
            continue;
        }
        if (currentIndex == index) {
            return (StoryScene)scene;
        }
        currentIndex++;
    }

    return STORY_SCENE_NONE;
}

int GetUtf8CharLength(const char *text) {
    unsigned char value;

    if (text == nullptr || text[0] == '\0') {
        return 0;
    }

    value = (unsigned char)text[0];
    if ((value & 0x80u) == 0) {
        return 1;
    }
    if ((value & 0xE0u) == 0xC0u) {
        return text[1] != '\0' ? 2 : 1;
    }
    if ((value & 0xF0u) == 0xE0u) {
        if (text[1] != '\0' && text[2] != '\0') {
            return 3;
        }
        return text[1] != '\0' ? 2 : 1;
    }
    if ((value & 0xF8u) == 0xF0u) {
        if (text[1] != '\0' && text[2] != '\0' && text[3] != '\0') {
            return 4;
        }
        if (text[1] != '\0' && text[2] != '\0') {
            return 3;
        }
        return text[1] != '\0' ? 2 : 1;
    }

    return 1;
}

bool IsAsciiBreakCharacter(const char *text, int charLength) {
    unsigned char value;

    if (text == nullptr || charLength != 1) {
        return false;
    }

    value = (unsigned char)text[0];
    return value == ' ' || value == '-' || value == '/' || value == '\t';
}

int GetWrappedLineLength(const AssetBundle *assets, const char *text, float fontSize, float maxWidth) {
    char buffer[512];
    int byteOffset;
    int lastBreak;

    if (text == nullptr || text[0] == '\0') {
        return 0;
    }

    byteOffset = 0;
    lastBreak = -1;
    while (text[byteOffset] != '\0' && text[byteOffset] != '\n') {
        int charLength;
        int copyLength;

        charLength = GetUtf8CharLength(text + byteOffset);
        if (charLength <= 0) {
            break;
        }

        copyLength = byteOffset + charLength;
        if (copyLength >= (int)sizeof(buffer)) {
            break;
        }

        memcpy(buffer, text, (size_t)copyLength);
        buffer[copyLength] = '\0';

        if (UIRuntime_MeasureText(assets, buffer, fontSize).x > maxWidth) {
            if (lastBreak > 0) {
                return lastBreak;
            }
            return byteOffset > 0 ? byteOffset : charLength;
        }

        if (IsAsciiBreakCharacter(text + byteOffset, charLength)) {
            lastBreak = copyLength;
        }
        byteOffset = copyLength;
    }

    return byteOffset;
}

int TrimTrailingWhitespaceLength(const char *text, int length) {
    while (length > 0 && (text[length - 1] == ' ' || text[length - 1] == '\t' || text[length - 1] == '\r')) {
        length--;
    }

    return length;
}

int CountWrappedTextLinesExact(const AssetBundle *assets, const char *text, float fontSize, float maxWidth) {
    const char *cursor;
    int lineCount;

    if (text == nullptr || text[0] == '\0') {
        return 0;
    }

    cursor = text;
    lineCount = 0;
    while (*cursor != '\0') {
        int lineLength;

        while (*cursor == ' ') {
            cursor++;
        }

        if (*cursor == '\n') {
            cursor++;
            lineCount++;
            continue;
        }

        lineLength = GetWrappedLineLength(assets, cursor, fontSize, maxWidth);
        if (lineLength <= 0) {
            break;
        }

        cursor += lineLength;
        if (*cursor == '\n') {
            cursor++;
        }
        lineCount++;
    }

    return lineCount;
}

float GetWrappedTextHeight(const AssetBundle *assets, const char *text, float fontSize, float lineSpacing, float maxWidth) {
    const int lineCount = CountWrappedTextLinesExact(assets, text, fontSize, maxWidth);

    if (lineCount <= 0) {
        return 0.0f;
    }

    return fontSize + (float)(lineCount - 1) * lineSpacing;
}

float ClampScrollOffset(float offset, float maxOffset) {
    if (offset < 0.0f) {
        return 0.0f;
    }
    if (offset > maxOffset) {
        return maxOffset;
    }
    return offset;
}

Rectangle GetCommunicatorPreviewImageRect(Rectangle contentPanel, float scale) {
    return Rectangle{
        contentPanel.x + 6.0f * scale,
        contentPanel.y + 6.0f * scale,
        contentPanel.width - 12.0f * scale,
        contentPanel.height - 12.0f * scale
    };
}

void DrawScrolledWrappedText(const AssetBundle *assets,
                             const char *text,
                             Rectangle rect,
                             float fontSize,
                             float lineSpacing,
                             float scrollOffset,
                             Color tint) {
    char lineBuffer[512];
    const char *cursor;
    const float maxScroll = std::fmax(0.0f, GetWrappedTextHeight(assets, text, fontSize, lineSpacing, rect.width) - rect.height);
    float y;

    if (text == nullptr || text[0] == '\0') {
        return;
    }

    cursor = text;
    y = rect.y - ClampScrollOffset(scrollOffset, maxScroll);

    BeginScissorMode((int)rect.x, (int)rect.y, (int)rect.width, (int)rect.height);
    while (*cursor != '\0') {
        int lineLength;
        int drawLength;

        while (*cursor == ' ') {
            cursor++;
        }

        if (*cursor == '\n') {
            cursor++;
            y += lineSpacing;
            continue;
        }

        lineLength = GetWrappedLineLength(assets, cursor, fontSize, rect.width);
        if (lineLength <= 0) {
            break;
        }

        drawLength = TrimTrailingWhitespaceLength(cursor, lineLength);
        if (drawLength >= (int)sizeof(lineBuffer)) {
            drawLength = (int)sizeof(lineBuffer) - 1;
        }

        if (y + fontSize >= rect.y && y <= rect.y + rect.height) {
            memcpy(lineBuffer, cursor, (size_t)drawLength);
            lineBuffer[drawLength] = '\0';
            UIRuntime_DrawText(assets, lineBuffer, Vector2{rect.x, y}, fontSize, tint);
        }

        cursor += lineLength;
        if (*cursor == '\n') {
            cursor++;
        }
        y += lineSpacing;
    }
    EndScissorMode();
}

void DrawCommunicatorDetailHint(const AssetBundle *assets,
                                Rectangle rect,
                                float scale,
                                float detailProgress) {
    const char *hint = Loc_PickLiteral("Wheel / W S scroll   Enter or ESC close", "滚轮 / W S 滚动   Enter 或 ESC 关闭");
    const float fontSize = 15.0f * scale;
    const Vector2 hintSize = UIRuntime_MeasureText(assets, hint, fontSize);

    UIRuntime_DrawText(assets,
                       hint,
                       Vector2{rect.x + rect.width - hintSize.x, rect.y + rect.height + 10.0f * scale},
                       fontSize,
                       Fade(Color{170, 188, 206, 255}, detailProgress));
}

void DrawCenteredWrappedText(const AssetBundle *assets,
                             const char *text,
                             Rectangle rect,
                             float fontSize,
                             float lineSpacing,
                             Color tint) {
    char lineBuffer[512];
    const char *cursor;
    int lineCount;
    float totalHeight;
    float y;

    if (text == nullptr || text[0] == '\0') {
        return;
    }

    lineCount = UIRuntime_CountWrappedTextLines(assets, text, fontSize, rect.width, kCommunicatorCenteredTextMaxLines);
    totalHeight = lineCount > 0 ? fontSize + (float)(lineCount - 1) * lineSpacing : 0.0f;
    y = rect.y + (rect.height - totalHeight) * 0.5f;
    if (y < rect.y) {
        y = rect.y;
    }

    cursor = text;
    BeginScissorMode((int)rect.x, (int)rect.y, (int)rect.width, (int)rect.height);
    while (*cursor != '\0' && y + fontSize <= rect.y + rect.height) {
        int lineLength;
        int drawLength;
        float lineWidth;
        float x;

        while (*cursor == ' ') {
            cursor++;
        }

        if (*cursor == '\n') {
            cursor++;
            y += lineSpacing;
            continue;
        }

        lineLength = GetWrappedLineLength(assets, cursor, fontSize, rect.width);
        if (lineLength <= 0) {
            break;
        }

        drawLength = TrimTrailingWhitespaceLength(cursor, lineLength);
        if (drawLength >= (int)sizeof(lineBuffer)) {
            drawLength = (int)sizeof(lineBuffer) - 1;
        }

        memcpy(lineBuffer, cursor, (size_t)drawLength);
        lineBuffer[drawLength] = '\0';
        lineWidth = UIRuntime_MeasureText(assets, lineBuffer, fontSize).x;
        x = rect.x + (rect.width - lineWidth) * 0.5f;
        if (x < rect.x) {
            x = rect.x;
        }
        UIRuntime_DrawText(assets, lineBuffer, Vector2{x, y}, fontSize, tint);

        cursor += lineLength;
        if (*cursor == '\n') {
            cursor++;
        }
        y += lineSpacing;
    }
    EndScissorMode();
}

void DrawCommunicatorTabs(const AssetBundle *assets,
                          int selectedTab,
                          int screenWidth,
                          int screenHeight,
                          float scale) {
    const UITheme *theme = UITheme_Get();
    static const char *kTabLabels[kCommunicatorTabCount][2] = {
        {"Current Task", "当前任务"},
        {"Archive Logs", "档案日志"},
        {"Main Story", "主线剧情"}
    };

    for (int tabIndex = 0; tabIndex < kCommunicatorTabCount; ++tabIndex) {
        const Rectangle tabRect = UI_GetCommunicatorTabRect(screenWidth, screenHeight, tabIndex);
        const bool active = tabIndex == selectedTab;
        const char *label = Loc_PickLiteral(kTabLabels[tabIndex][0], kTabLabels[tabIndex][1]);

        UIComponents_DrawCompactButton(assets,
                                       tabRect,
                                       label,
                                       true,
                                       active,
                                       active ? UI_ACTION_PRIMARY : UI_ACTION_GHOST,
                                       scale);
        if (active) {
            DrawLineEx(Vector2{tabRect.x + 18.0f * scale, tabRect.y + tabRect.height - 5.0f * scale},
                       Vector2{tabRect.x + tabRect.width - 18.0f * scale, tabRect.y + tabRect.height - 5.0f * scale},
                       2.0f * scale,
                       theme->signalCyan);
        }
    }
}

void DrawCommunicatorTaskTab(const AssetBundle *assets,
                             const TaskSystem *tasks,
                             Rectangle contentPanel,
                             float scale) {
    const UITheme *theme = UITheme_Get();
    Rectangle stageRect;
    Rectangle objectivePanel;
    Rectangle objectiveRect;
    char stageBuffer[128];
    char sanitizedObjective[256];
    const char *objectiveTitle = Loc_PickLiteral("Tracked Objective", "当前追踪");

    stageRect = Rectangle{
        contentPanel.x + 34.0f * scale,
        contentPanel.y + 32.0f * scale,
        contentPanel.width - 68.0f * scale,
        70.0f * scale
    };
    objectivePanel = Rectangle{
        contentPanel.x + 34.0f * scale,
        stageRect.y + stageRect.height + 18.0f * scale,
        contentPanel.width - 68.0f * scale,
        contentPanel.height - 138.0f * scale
    };
    objectiveRect = Rectangle{
        objectivePanel.x + 24.0f * scale,
        objectivePanel.y + 62.0f * scale,
        objectivePanel.width - 48.0f * scale,
        objectivePanel.height - 86.0f * scale
    };

    std::snprintf(stageBuffer,
                  sizeof(stageBuffer),
                  "%s %d  /  %s",
                  Loc_PickLiteral("Stage", "阶段"),
                  tasks != nullptr ? tasks->stage : 0,
                  tasks != nullptr ? Tasks_GetStageName(tasks->stage) : "");
    TasksRuntime_SanitizeDisplayText(tasks != nullptr ? tasks->objective : "", sanitizedObjective, sizeof(sanitizedObjective));
    UIComponents_DrawCard(contentPanel, false, true, scale, theme->signalBlue);
    UIComponents_DrawCard(stageRect, false, true, scale, theme->signalCyan);
    UIRuntime_DrawText(assets,
                       Loc_PickLiteral("Current Stage", "当前阶段"),
                       Vector2{stageRect.x + 20.0f * scale, stageRect.y + 12.0f * scale},
                       18.0f * scale,
                       theme->archiveGold);
    UIRuntime_DrawText(assets,
                       stageBuffer,
                       Vector2{stageRect.x + 20.0f * scale, stageRect.y + 38.0f * scale},
                       22.0f * scale,
                       theme->textPrimary);
    UIComponents_DrawCard(objectivePanel, true, true, scale, theme->signalBlue);
    UIRuntime_DrawText(assets,
                       objectiveTitle,
                       Vector2{objectivePanel.x + objectivePanel.width * 0.5f - UIRuntime_MeasureText(assets, objectiveTitle, 26.0f * scale).x * 0.5f,
                               objectivePanel.y + 22.0f * scale},
                       26.0f * scale,
                       theme->archiveGold);
    DrawCenteredWrappedText(assets, sanitizedObjective, objectiveRect, 29.0f * scale, 36.0f * scale, theme->iceWhite);
}

void DrawCommunicatorLogTab(const AssetBundle *assets,
                            const TaskSystem *tasks,
                            int selectedLog,
                            int firstVisibleLog,
                            float detailVisibility,
                            float detailScroll,
                            Rectangle listPanel,
                            Rectangle contentPanel,
                            float scale,
                            int screenWidth,
                            int screenHeight) {
    Rectangle imageRect;
    Rectangle detailMaskRect;
    Rectangle detailBodyPanelRect;
    Rectangle detailBodyRect;
    int collectedCount = Tasks_GetCollectedLogCount(tasks);
    int visibleCount = 0;
    int drawCount = 0;
    int sceneIndex = -1;
    const ShipLog *log = nullptr;
    const char *detailText = "";
    const IntroSlideDef *sceneDef = nullptr;
    const TextureAsset *sceneTexture = nullptr;
    const float detailProgress = SmoothStep(detailVisibility);
    const bool showSelectedLogDetail = detailProgress > 0.001f;

    UIComponents_DrawCard(listPanel, false, true, scale, UITheme_Get()->signalBlue);
    UIComponents_DrawCard(contentPanel, false, true, scale, UITheme_Get()->signalBlue);

    if (collectedCount <= 0) {
        UIRuntime_DrawWrappedText(assets,
                                  Loc_PickLiteral("No logs recovered yet. Keep exploring, retrieve ship records, then return here to review them with Loxi.",
                                                  "还没有回收到任何日志。继续探索，找回飞船记录，然后回到这里与洛希一起复核。"),
                                  Rectangle{contentPanel.x + 20.0f * scale, contentPanel.y + 20.0f * scale, contentPanel.width - 40.0f * scale, contentPanel.height - 40.0f * scale},
                                  21.0f * scale,
                                  27.0f * scale,
                                  Color{227, 237, 245, 255});
        return;
    }

    if (selectedLog < 0) {
        selectedLog = 0;
    }
    if (selectedLog >= collectedCount) {
        selectedLog = collectedCount - 1;
    }
    log = Tasks_GetCollectedLogAt(tasks, selectedLog);
    firstVisibleLog = UI_ClampCommunicatorFirstVisibleLogIndex(screenWidth, screenHeight, firstVisibleLog, collectedCount);
    visibleCount = UI_GetCommunicatorVisibleLogCount(screenWidth, screenHeight);
    drawCount = collectedCount - firstVisibleLog;
    if (drawCount > visibleCount) {
        drawCount = visibleCount;
    }
    imageRect = GetCommunicatorPreviewImageRect(contentPanel, scale);
    detailMaskRect = Rectangle{
        listPanel.x,
        listPanel.y,
        contentPanel.x + contentPanel.width - listPanel.x,
        listPanel.height
    };
    detailBodyPanelRect = Rectangle{
        detailMaskRect.x + 26.0f * scale,
        detailMaskRect.y + 24.0f * scale,
        detailMaskRect.width - 52.0f * scale,
        detailMaskRect.height - 48.0f * scale
    };
    detailBodyRect = Rectangle{
        detailBodyPanelRect.x + 28.0f * scale,
        detailBodyPanelRect.y + 24.0f * scale,
        detailBodyPanelRect.width - 56.0f * scale,
        detailBodyPanelRect.height - 48.0f * scale
    };

    for (int visibleIndex = 0; visibleIndex < drawCount; ++visibleIndex) {
        int entryIndex = firstVisibleLog + visibleIndex;
        Rectangle entryRect = UI_GetCommunicatorVisibleLogEntryRect(screenWidth, screenHeight, visibleIndex);
        const ShipLog *entry = Tasks_GetCollectedLogAt(tasks, entryIndex);
        bool active = entryIndex == selectedLog;

        UIComponents_DrawCard(entryRect,
                              active,
                              true,
                              scale,
                              active ? UITheme_Get()->signalCyan : UITheme_Get()->border);
        UIRuntime_DrawWrappedText(assets,
                                  entry != nullptr ? Tasks_GetLogTitle(entry) : "",
                                  Rectangle{entryRect.x + 16.0f * scale, entryRect.y + 11.0f * scale, entryRect.width - 32.0f * scale, entryRect.height - 22.0f * scale},
                                  15.8f * scale,
                                  18.4f * scale,
                                  active ? WHITE : Color{217, 228, 237, 255});
    }

    if (log != nullptr) {
        sceneIndex = Tasks_GetLogSceneIndex(tasks, log);
        if (sceneIndex >= 0 && sceneIndex < STORY_LOG_SCENE_COUNT) {
            StoryScene storyScene = (StoryScene)(STORY_SCENE_LOG_THE_CRASH + sceneIndex);
            sceneDef = UIStory_GetStorySceneDef(storyScene);
            sceneTexture = UIStory_GetStorySceneTexture(assets, storyScene);
        }

        detailText = Tasks_GetLogDetailText(log);
        if (detailText[0] == '\0') {
            detailText = Tasks_GetLogStoryText(log);
        }
        if (sceneTexture != nullptr && sceneTexture->loaded) {
            UIRuntime_DrawTextureAssetFitted(sceneTexture, imageRect, WHITE);
        } else {
            DrawRectangleGradientV((int)imageRect.x,
                                   (int)imageRect.y,
                                   (int)imageRect.width,
                                   (int)imageRect.height,
                                   Color{20, 36, 58, 255},
                                   Color{9, 16, 28, 255});
            UIRuntime_DrawWrappedText(assets,
                                      sceneDef != nullptr ? Loc_PickText(sceneDef->title) : Tasks_GetLogTitle(log),
                                      Rectangle{imageRect.x + 28.0f * scale, imageRect.y + 24.0f * scale, imageRect.width - 56.0f * scale, imageRect.height - 48.0f * scale},
                                      26.0f * scale,
                                      32.0f * scale,
                                      WHITE);
        }
        if (!showSelectedLogDetail) {
            return;
        }

        if (showSelectedLogDetail) {
            DrawRectangleGradientV((int)detailMaskRect.x,
                                   (int)detailMaskRect.y,
                                   (int)detailMaskRect.width,
                                   (int)detailMaskRect.height,
                                   Fade(Color{7, 13, 22, 255}, detailProgress * 0.98f),
                                   Fade(Color{3, 6, 12, 255}, detailProgress));
            DrawRectangle((int)detailMaskRect.x,
                          (int)detailMaskRect.y,
                          (int)detailMaskRect.width,
                          (int)detailMaskRect.height,
                          Fade(Color{5, 10, 18, 255}, detailProgress * 0.80f));

            UIRuntime_DrawPanel(detailBodyPanelRect,
                                Fade(Color{10, 18, 30, 248}, detailProgress),
                                Fade(UITheme_Get()->signalBlue, detailProgress * 0.38f));
            if (detailText[0] != '\0') {
                DrawScrolledWrappedText(assets,
                                        detailText,
                                        detailBodyRect,
                                        25.4f * scale,
                                        37.0f * scale,
                                        detailScroll,
                                        Fade(Color{227, 237, 245, 255}, detailProgress));
                DrawCommunicatorDetailHint(assets, detailBodyRect, scale, detailProgress);
            } else {
                for (float y = detailBodyRect.y + 10.0f * scale; y < detailBodyRect.y + detailBodyRect.height - 6.0f * scale; y += 30.0f * scale) {
                    DrawRectangle((int)detailBodyRect.x,
                                  (int)y,
                                  (int)(detailBodyRect.width * 0.92f),
                                  1,
                                  Fade(Color{156, 184, 202, 255}, detailProgress * 0.13f));
                }
            }
        }
    }
}

void DrawCommunicatorStoryTab(const AssetBundle *assets,
                              const bool *storySceneShown,
                              int selectedStoryScene,
                              int firstVisibleStoryScene,
                              float detailVisibility,
                              float detailScroll,
                              Rectangle listPanel,
                              Rectangle contentPanel,
                              float scale,
                              int screenWidth,
                              int screenHeight) {
    Rectangle imageRect;
    Rectangle detailMaskRect;
    Rectangle detailBodyPanelRect;
    Rectangle detailBodyRect;
    int shownCount = GetShownMainStorySceneCount(storySceneShown);
    int visibleCount = 0;
    int drawCount = 0;
    StoryScene scene = STORY_SCENE_NONE;
    const IntroSlideDef *sceneDef = nullptr;
    const TextureAsset *sceneTexture = nullptr;
    const char *detailText = "";
    const float detailProgress = SmoothStep(detailVisibility);
    const bool showSelectedStoryDetail = detailProgress > 0.001f;

    UIComponents_DrawCard(listPanel, false, true, scale, UITheme_Get()->signalBlue);
    UIComponents_DrawCard(contentPanel, false, true, scale, UITheme_Get()->signalBlue);

    if (shownCount <= 0) {
        UIRuntime_DrawWrappedText(assets,
                                  Loc_PickLiteral("Main story cards will be archived here after you unlock them. Push the investigation forward, then come back to review how Loxi has pieced the whole story together.",
                                                  "解锁过的主线剧情卡会收录在这里。继续推进调查，之后再回来查看洛希是怎样把整件事一步步拼起来的。"),
                                  Rectangle{contentPanel.x + 20.0f * scale, contentPanel.y + 20.0f * scale, contentPanel.width - 40.0f * scale, contentPanel.height - 40.0f * scale},
                                  21.0f * scale,
                                  27.0f * scale,
                                  Color{227, 237, 245, 255});
        return;
    }

    if (selectedStoryScene < 0) {
        selectedStoryScene = 0;
    }
    if (selectedStoryScene >= shownCount) {
        selectedStoryScene = shownCount - 1;
    }

    firstVisibleStoryScene = UI_ClampCommunicatorFirstVisibleLogIndex(screenWidth, screenHeight, firstVisibleStoryScene, shownCount);
    visibleCount = UI_GetCommunicatorVisibleLogCount(screenWidth, screenHeight);
    drawCount = shownCount - firstVisibleStoryScene;
    if (drawCount > visibleCount) {
        drawCount = visibleCount;
    }

    scene = GetShownMainStorySceneAt(storySceneShown, selectedStoryScene);
    sceneDef = UIStory_GetStorySceneDef(scene);
    sceneTexture = UIStory_GetStorySceneTexture(assets, scene);
    detailText = UIStory_GetStorySceneDetailText(scene);
    imageRect = GetCommunicatorPreviewImageRect(contentPanel, scale);
    detailMaskRect = Rectangle{
        listPanel.x,
        listPanel.y,
        contentPanel.x + contentPanel.width - listPanel.x,
        listPanel.height
    };
    detailBodyPanelRect = Rectangle{
        detailMaskRect.x + 26.0f * scale,
        detailMaskRect.y + 24.0f * scale,
        detailMaskRect.width - 52.0f * scale,
        detailMaskRect.height - 48.0f * scale
    };
    detailBodyRect = Rectangle{
        detailBodyPanelRect.x + 28.0f * scale,
        detailBodyPanelRect.y + 24.0f * scale,
        detailBodyPanelRect.width - 56.0f * scale,
        detailBodyPanelRect.height - 48.0f * scale
    };

    for (int visibleIndex = 0; visibleIndex < drawCount; ++visibleIndex) {
        int entryIndex = firstVisibleStoryScene + visibleIndex;
        StoryScene entryScene = GetShownMainStorySceneAt(storySceneShown, entryIndex);
        const IntroSlideDef *entryDef = UIStory_GetStorySceneDef(entryScene);
        Rectangle entryRect = UI_GetCommunicatorVisibleLogEntryRect(screenWidth, screenHeight, visibleIndex);
        bool active = entryIndex == selectedStoryScene;

        UIComponents_DrawCard(entryRect,
                              active,
                              true,
                              scale,
                              active ? UITheme_Get()->signalCyan : UITheme_Get()->border);
        UIRuntime_DrawWrappedText(assets,
                                  entryDef != nullptr ? Loc_PickText(entryDef->title) : "",
                                  Rectangle{entryRect.x + 16.0f * scale, entryRect.y + 11.0f * scale, entryRect.width - 32.0f * scale, entryRect.height - 22.0f * scale},
                                  15.8f * scale,
                                  18.4f * scale,
                                  active ? WHITE : Color{217, 228, 237, 255});
    }

    if (sceneTexture != nullptr && sceneTexture->loaded) {
        UIRuntime_DrawTextureAssetFitted(sceneTexture, imageRect, WHITE);
    } else {
        DrawRectangleGradientV((int)imageRect.x,
                               (int)imageRect.y,
                               (int)imageRect.width,
                               (int)imageRect.height,
                               Color{20, 36, 58, 255},
                               Color{9, 16, 28, 255});
        UIRuntime_DrawWrappedText(assets,
                                  sceneDef != nullptr ? Loc_PickText(sceneDef->title) : "",
                                  Rectangle{imageRect.x + 28.0f * scale, imageRect.y + 24.0f * scale, imageRect.width - 56.0f * scale, imageRect.height - 48.0f * scale},
                                  26.0f * scale,
                                  32.0f * scale,
                                  WHITE);
    }

    if (!showSelectedStoryDetail) {
        return;
    }

    DrawRectangleGradientV((int)detailMaskRect.x,
                           (int)detailMaskRect.y,
                           (int)detailMaskRect.width,
                           (int)detailMaskRect.height,
                           Fade(Color{7, 13, 22, 255}, detailProgress * 0.98f),
                           Fade(Color{3, 6, 12, 255}, detailProgress));
    DrawRectangle((int)detailMaskRect.x,
                  (int)detailMaskRect.y,
                  (int)detailMaskRect.width,
                  (int)detailMaskRect.height,
                  Fade(Color{5, 10, 18, 255}, detailProgress * 0.80f));

    UIRuntime_DrawPanel(detailBodyPanelRect,
                        Fade(Color{10, 18, 30, 248}, detailProgress),
                        Fade(UITheme_Get()->signalBlue, detailProgress * 0.38f));
    if (detailText[0] != '\0') {
        DrawScrolledWrappedText(assets,
                                detailText,
                                detailBodyRect,
                                25.4f * scale,
                                37.0f * scale,
                                detailScroll,
                                Fade(Color{227, 237, 245, 255}, detailProgress));
        DrawCommunicatorDetailHint(assets, detailBodyRect, scale, detailProgress);
    } else {
        for (float y = detailBodyRect.y + 10.0f * scale; y < detailBodyRect.y + detailBodyRect.height - 6.0f * scale; y += 30.0f * scale) {
            DrawRectangle((int)detailBodyRect.x,
                          (int)y,
                          (int)(detailBodyRect.width * 0.92f),
                          1,
                          Fade(Color{156, 184, 202, 255}, detailProgress * 0.13f));
        }
    }
}

}  // namespace

void UI_DrawCommunicatorOverlay(const AssetBundle *assets,
                                const TaskSystem *tasks,
                                const bool *storySceneShown,
                                int selectedTab,
                                int selectedLog,
                                int firstVisibleLog,
                                int selectedStoryScene,
                                int firstVisibleStoryScene,
                                float detailVisibility,
                                float detailScroll,
                                int screenWidth,
                                int screenHeight) {
    const UITheme *theme = UITheme_Get();
    float scale;
    Rectangle panel;
    Rectangle listPanel;
    Rectangle contentPanel;
    const char *closeHint;

    scale = UIRuntime_GetScale(screenWidth, screenHeight);
    panel = UI_GetCommunicatorOverlayRect(screenWidth, screenHeight);
    listPanel = UI_GetCommunicatorLogListRect(screenWidth, screenHeight);
    contentPanel = UI_GetCommunicatorLogContentRect(screenWidth, screenHeight);
    closeHint = Loc_PickLiteral("Press N or ESC to close.", "按 N 或 ESC 关闭。");

    UIComponents_DrawScrim(screenWidth, screenHeight, 198);
    UIComponents_DrawFrame(panel, scale, UI_FRAME_STANDARD, true);
    UIComponents_DrawPanelHeader(assets,
                                 panel,
                                 Loc_PickLiteral("LOXI SIGNAL ARCHIVE // SYNCHRONIZED", "洛希信号档案 // 已同步"),
                                 Loc_PickLiteral("LOXI TERMINAL", "洛希终端"),
                                 closeHint,
                                 scale,
                                 theme->signalCyan);
    DrawCommunicatorTabs(assets, selectedTab, screenWidth, screenHeight, scale);

    if (selectedTab == kCommunicatorTabLogs) {
        DrawCommunicatorLogTab(assets,
                               tasks,
                               selectedLog,
                               firstVisibleLog,
                               detailVisibility,
                               detailScroll,
                               listPanel,
                               contentPanel,
                               scale,
                               screenWidth,
                               screenHeight);
        return;
    }

    if (selectedTab == kCommunicatorTabStory) {
        DrawCommunicatorStoryTab(assets,
                                 storySceneShown,
                                 selectedStoryScene,
                                 firstVisibleStoryScene,
                                 detailVisibility,
                                 detailScroll,
                                 listPanel,
                                 contentPanel,
                                 scale,
                                 screenWidth,
                                 screenHeight);
        return;
    }

    DrawCommunicatorTaskTab(assets,
                            tasks,
                            Rectangle{
                                panel.x + 20.0f * scale,
                                panel.y + 146.0f * scale,
                                panel.width - 40.0f * scale,
                                panel.height - 178.0f * scale
                            },
                            scale);
}

void UI_DrawHelpOverlay(const AssetBundle *assets, int screenWidth, int screenHeight) {
    float scale;
    Rectangle panel;
    const LocalizedText lines[] = {
        {"WASD / Arrow Keys: original grid movement", "WASD / 方向键：原始网格移动"},
        {"F: interact, gather, repair, read logs, craft at workbench", "F：交互、采集、修理、读取日志，或在工作台制作"},
        {"Space: attack, uses laser line if available", "Space：攻击，拥有激光枪时会发射激光"},
        {"B: open the unified info page directly on Backpack", "B：直接以背包页打开统一信息界面"},
        {"N: open the unified info page directly on Loxi", "N：直接以洛希页打开统一信息界面"},
        {"M: open the unified info page directly on Map", "M：直接以地图页打开统一信息界面"},
        {"H: open help, O: open the unified info page directly on Settings", "H：打开帮助，O：直接以设置页打开统一信息界面"},
        {"X: use a crafted Recovery Ration for health, poison relief, and oxygen recovery", "X：使用加工后的复苏口粮，同时恢复生命、解除中毒并补回氧气"},
        {"ESC: pause", "ESC：暂停"},
        {"Sleep in Crew Quarters to recover. Raw forage must be processed at the workbench, and camps only provide partial field recovery", "在船员舱睡觉可恢复状态；野外食材必须先在工作台加工；野外营地只提供有限恢复"}
    };
    int lineIndex;

    scale = UIRuntime_GetScale(screenWidth, screenHeight);
    panel = UI_GetStandardOverlayRect(screenWidth, screenHeight);

    DrawRectangle(0, 0, screenWidth, screenHeight, Color{5, 9, 16, 190});
    UIRuntime_DrawPanel(panel, Color{8, 18, 30, 245}, Color{153, 226, 255, 75});
    UIRuntime_DrawText(assets, LOC_UI_HELP_TITLE, Vector2{panel.x + 26.0f * scale, panel.y + 22.0f * scale}, 33.0f * scale, WHITE);
    UIRuntime_DrawText(assets, LOC_UI_HELP_HINT, Vector2{panel.x + panel.width - UIRuntime_MeasureText(assets, LOC_UI_HELP_HINT, 17.0f * scale).x - 24.0f * scale, panel.y + 28.0f * scale}, 17.0f * scale, Color{182, 199, 214, 255});

    for (lineIndex = 0; lineIndex < (int)(sizeof(lines) / sizeof(lines[0])); lineIndex++) {
        int column;
        int row;
        float x;
        float y;
        const TextureAsset *shortcutIcon;
        int shortcutIconKind;
        Color shortcutPrimary;
        Color shortcutSecondary;
        Rectangle rowRect;
        Rectangle textRect;

        column = lineIndex / 6;
        row = lineIndex % 6;
        x = panel.x + 34.0f * scale + column * 468.0f * scale;
        y = panel.y + 108.0f * scale + row * 66.0f * scale;
        shortcutIcon = nullptr;
        shortcutIconKind = -1;
        shortcutPrimary = Color{255, 255, 255, 255};
        shortcutSecondary = Color{255, 255, 255, 255};
        rowRect = Rectangle{x, y, 434.0f * scale, 52.0f * scale};
        textRect = Rectangle{x + 16.0f * scale, y + 10.0f * scale, 402.0f * scale, 34.0f * scale};

        if (lineIndex == 3) {
            shortcutIcon = &assets->iconInventoryButton;
            shortcutIconKind = 1;
            shortcutPrimary = Color{232, 180, 113, 255};
            shortcutSecondary = Color{127, 84, 44, 255};
        } else if (lineIndex == 4) {
            shortcutIcon = nullptr;
            shortcutIconKind = 2;
            shortcutPrimary = Color{118, 226, 255, 255};
            shortcutSecondary = Color{60, 120, 188, 255};
        } else if (lineIndex == 5) {
            shortcutIcon = &assets->iconMapButton;
            shortcutIconKind = 0;
            shortcutPrimary = Color{110, 201, 255, 255};
            shortcutSecondary = Color{62, 118, 192, 255};
        } else if (lineIndex == 6) {
            shortcutIcon = &assets->iconRecoveryRation;
            shortcutIconKind = 16;
            shortcutPrimary = Color{126, 212, 255, 255};
            shortcutSecondary = Color{57, 112, 172, 255};
        }

        UIRuntime_DrawPanel(rowRect, Color{11, 20, 32, 228}, Color{255, 255, 255, 20});
        if (shortcutIconKind >= 0) {
            Rectangle iconSlot = Rectangle{x + 10.0f * scale, y + 8.0f * scale, 36.0f * scale, 36.0f * scale};

            UIRuntime_DrawPanel(iconSlot, Color{18, 34, 52, 235}, Color{shortcutPrimary.r, shortcutPrimary.g, shortcutPrimary.b, 65});
            UIRuntime_DrawShortcutIcon(iconSlot, shortcutIcon, shortcutIconKind, shortcutPrimary, shortcutSecondary);
            textRect = Rectangle{x + 56.0f * scale, y + 10.0f * scale, 362.0f * scale, 34.0f * scale};
        }
        UIRuntime_DrawWrappedText(assets, Loc_PickText(lines[lineIndex]), textRect, 16.5f * scale, 18.0f * scale, Color{229, 238, 246, 255});
    }
}
