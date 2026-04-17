#include "ui_system.h"

#include "localization.h"
#include "task_runtime_internal.h"
#include "ui_story_internal.h"
#include "ui_runtime_internal.h"

#include <cstdio>
#include <cstring>

namespace {

constexpr int kCommunicatorTabLogs = 1;
constexpr int kCommunicatorTabCount = 2;
constexpr int kCommunicatorCenteredTextMaxLines = 12;

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

int GetCollectedMainArchiveCount(const TaskSystem *tasks) {
    int count = 0;

    if (tasks == nullptr) {
        return 0;
    }

    for (int index = 0; index < tasks->logCount; ++index) {
        if (tasks->logs[index].category == SHIP_LOG_MAINLINE
            && tasks->logs[index].active
            && tasks->logs[index].collected) {
            count += 1;
        }
    }

    return count;
}

int GetCollectedSupplementalArchiveCount(const TaskSystem *tasks) {
    int count = 0;

    if (tasks == nullptr) {
        return 0;
    }

    for (int index = 0; index < tasks->logCount; ++index) {
        if (tasks->logs[index].category == SHIP_LOG_SUPPLEMENTAL
            && tasks->logs[index].active
            && tasks->logs[index].collected) {
            count += 1;
        }
    }

    return count;
}

void DrawCommunicatorTabs(const AssetBundle *assets,
                          int selectedTab,
                          int screenWidth,
                          int screenHeight,
                          float scale) {
    static const char *kTabLabels[kCommunicatorTabCount][2] = {
        {"Current Task", "当前任务"},
        {"Archive Logs", "档案日志"}
    };

    for (int tabIndex = 0; tabIndex < kCommunicatorTabCount; ++tabIndex) {
        Rectangle tabRect = UI_GetCommunicatorTabRect(screenWidth, screenHeight, tabIndex);
        bool active = tabIndex == (int)selectedTab;
        const char *label = Loc_PickLiteral(kTabLabels[tabIndex][0], kTabLabels[tabIndex][1]);
        Vector2 labelSize = UIRuntime_MeasureText(assets, label, 21.0f * scale);

        UIRuntime_DrawPanel(tabRect,
                            active ? Color{20, 46, 56, 245} : Color{11, 20, 32, 228},
                            active ? Color{99, 233, 195, 130} : Color{104, 196, 222, 50});
        UIRuntime_DrawText(assets,
                           label,
                           Vector2{tabRect.x + (tabRect.width - labelSize.x) * 0.5f, tabRect.y + (tabRect.height - labelSize.y) * 0.5f - 1.0f * scale},
                           21.0f * scale,
                           active ? WHITE : Color{192, 208, 222, 255});
    }
}

void DrawCommunicatorTaskTab(const AssetBundle *assets,
                             const TaskSystem *tasks,
                             Rectangle contentPanel,
                             float scale) {
    Rectangle objectiveRect;
    char sanitizedObjective[256];
    objectiveRect = Rectangle{
        contentPanel.x + 34.0f * scale,
        contentPanel.y + 82.0f * scale,
        contentPanel.width - 68.0f * scale,
        contentPanel.height - 126.0f * scale
    };

    TasksRuntime_SanitizeDisplayText(tasks != nullptr ? tasks->objective : "", sanitizedObjective, sizeof(sanitizedObjective));
    UIRuntime_DrawPanel(contentPanel, Color{11, 20, 32, 230}, Color{104, 196, 222, 50});
    UIRuntime_DrawText(assets,
                       Loc_PickLiteral("Current Objective", "当前目标"),
                       Vector2{contentPanel.x + contentPanel.width * 0.5f - UIRuntime_MeasureText(assets, Loc_PickLiteral("Current Objective", "当前目标"), 28.0f * scale).x * 0.5f,
                               contentPanel.y + 22.0f * scale},
                       28.0f * scale,
                       Color{255, 214, 154, 255});
    DrawCenteredWrappedText(assets, sanitizedObjective, objectiveRect, 29.0f * scale, 35.0f * scale, Color{227, 237, 245, 255});
}

void DrawCommunicatorLogTab(const AssetBundle *assets,
                            const TaskSystem *tasks,
                            int selectedLog,
                            int firstVisibleLog,
                            Rectangle listPanel,
                            Rectangle contentPanel,
                            float scale,
                            int screenWidth,
                            int screenHeight) {
    Rectangle imagePanel;
    Rectangle imageRect;
    Rectangle detailPanel;
    Rectangle detailRect;
    Rectangle rewardRect;
    Rectangle transcriptRect;
    char buffer[128];
    char locationBuffer[128];
    char detailBuffer[4096];
    char sanitizedDetail[4096];
    int collectedCount = Tasks_GetCollectedLogCount(tasks);
    int mainCollectedCount = GetCollectedMainArchiveCount(tasks);
    int supplementalCollectedCount = GetCollectedSupplementalArchiveCount(tasks);
    int visibleCount = 0;
    int drawCount = 0;
    int sceneIndex = -1;
    const ShipLog *log = nullptr;
    const IntroSlideDef *sceneDef = nullptr;
    const TextureAsset *sceneTexture = nullptr;

    UIRuntime_DrawPanel(listPanel, Color{11, 20, 32, 230}, Color{104, 196, 222, 50});
    UIRuntime_DrawPanel(contentPanel, Color{11, 20, 32, 230}, Color{104, 196, 222, 50});

    std::snprintf(buffer,
                  sizeof(buffer),
                  "%s %d   %s %d",
                  Loc_PickLiteral("Main", "主线"),
                  mainCollectedCount,
                  Loc_PickLiteral("Supplemental", "补充"),
                  supplementalCollectedCount);
    UIRuntime_DrawText(assets, Loc_PickLiteral("Archive Index", "档案目录"), Vector2{listPanel.x + 18.0f * scale, listPanel.y + 18.0f * scale}, 24.0f * scale, Color{255, 214, 154, 255});
    UIRuntime_DrawText(assets, buffer, Vector2{listPanel.x + 18.0f * scale, listPanel.y + 48.0f * scale}, 16.0f * scale, Color{194, 224, 255, 255});

    if (collectedCount <= 0) {
        UIRuntime_DrawText(assets, Loc_PickLiteral("Archive Reader", "档案阅读器"), Vector2{contentPanel.x + 20.0f * scale, contentPanel.y + 18.0f * scale}, 24.0f * scale, Color{255, 214, 154, 255});
        UIRuntime_DrawWrappedText(assets,
                                  Loc_PickLiteral("No logs recovered yet. Keep exploring, retrieve ship records, then return here to review them with Loxi.",
                                                  "还没有回收到任何日志。继续探索，找回飞船记录，然后回到这里与洛希一起复核。"),
                                  Rectangle{contentPanel.x + 20.0f * scale, contentPanel.y + 62.0f * scale, contentPanel.width - 40.0f * scale, contentPanel.height - 82.0f * scale},
                                  18.0f * scale,
                                  22.0f * scale,
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
    std::snprintf(buffer,
                  sizeof(buffer),
                  "%s %d-%d / %d",
                  Loc_PickLiteral("Viewing", "显示"),
                  drawCount > 0 ? firstVisibleLog + 1 : 0,
                  drawCount > 0 ? firstVisibleLog + drawCount : 0,
                  collectedCount);
    UIRuntime_DrawText(assets,
                       Loc_PickLiteral("Scroll to browse, click to open", "滚轮浏览，点击打开"),
                       Vector2{listPanel.x + 18.0f * scale, listPanel.y + listPanel.height - 62.0f * scale},
                       12.5f * scale,
                       Color{184, 214, 240, 255});
    UIRuntime_DrawText(assets,
                       buffer,
                       Vector2{listPanel.x + 18.0f * scale, listPanel.y + listPanel.height - 42.0f * scale},
                       14.0f * scale,
                       Color{166, 255, 226, 255});
    imagePanel = Rectangle{
        contentPanel.x,
        contentPanel.y + 84.0f * scale,
        contentPanel.width,
        292.0f * scale
    };
    imageRect = Rectangle{
        imagePanel.x + 12.0f * scale,
        imagePanel.y + 12.0f * scale,
        imagePanel.width - 24.0f * scale,
        imagePanel.height - 24.0f * scale
    };
    detailPanel = Rectangle{
        contentPanel.x,
        imagePanel.y + imagePanel.height + 14.0f * scale,
        contentPanel.width,
        contentPanel.height - imagePanel.height - 98.0f * scale
    };
    detailRect = Rectangle{
        detailPanel.x + 16.0f * scale,
        detailPanel.y + 52.0f * scale,
        detailPanel.width - 32.0f * scale,
        detailPanel.height - 64.0f * scale
    };
    rewardRect = Rectangle{
        contentPanel.x + 16.0f * scale,
        contentPanel.y + contentPanel.height - 34.0f * scale,
        contentPanel.width - 32.0f * scale,
        20.0f * scale
    };
    transcriptRect = Rectangle{
        imageRect.x + 16.0f * scale,
        imageRect.y + imageRect.height - 42.0f * scale,
        imageRect.width - 32.0f * scale,
        24.0f * scale
    };

    for (int visibleIndex = 0; visibleIndex < drawCount; ++visibleIndex) {
        int entryIndex = firstVisibleLog + visibleIndex;
        Rectangle entryRect = UI_GetCommunicatorVisibleLogEntryRect(screenWidth, screenHeight, visibleIndex);
        const ShipLog *entry = Tasks_GetCollectedLogAt(tasks, entryIndex);
        bool active = entryIndex == selectedLog;

        std::snprintf(buffer, sizeof(buffer), "%02d", entryIndex + 1);

        UIRuntime_DrawPanel(entryRect,
                            active ? Color{20, 46, 56, 245} : Color{11, 20, 32, 228},
                            active ? Color{99, 233, 195, 110} : Color{255, 255, 255, 20});
        UIRuntime_DrawText(assets,
                           buffer,
                           Vector2{entryRect.x + 12.0f * scale, entryRect.y + 8.0f * scale},
                           13.5f * scale,
                           active ? Color{166, 255, 226, 255} : Color{168, 191, 212, 255});
        UIRuntime_DrawWrappedText(assets,
                                  entry != nullptr ? Tasks_GetLogTitle(entry) : "",
                                  Rectangle{entryRect.x + 46.0f * scale, entryRect.y + 9.0f * scale, entryRect.width - 58.0f * scale, 42.0f * scale},
                                  14.8f * scale,
                                  16.8f * scale,
                                  active ? WHITE : Color{217, 228, 237, 255});
    }

    UIRuntime_DrawText(assets, Loc_PickLiteral("Archive Reader", "档案阅读器"), Vector2{contentPanel.x + 20.0f * scale, contentPanel.y + 18.0f * scale}, 24.0f * scale, Color{255, 214, 154, 255});
    if (log != nullptr) {
        sceneIndex = Tasks_GetLogSceneIndex(tasks, log);
        if (sceneIndex >= 0 && sceneIndex < STORY_LOG_SCENE_COUNT) {
            StoryScene storyScene = (StoryScene)(STORY_SCENE_LOG_THE_CRASH + sceneIndex);
            sceneDef = UIStory_GetStorySceneDef(storyScene);
            sceneTexture = UIStory_GetStorySceneTexture(assets, storyScene);
        }

        std::snprintf(buffer, sizeof(buffer), "%s", Tasks_GetLogRewardDescription(log));
        std::snprintf(locationBuffer,
                      sizeof(locationBuffer),
                      "%s  |  %s  |  %s",
                      Tasks_GetLogTitle(log),
                      log->category == SHIP_LOG_MAINLINE
                          ? Loc_PickLiteral("Main Archive", "主线档案")
                          : Loc_PickLiteral("Supplemental Archive", "补充档案"),
                      Loc_GetLocationNameText(Map_GetLocationNameAt(log->gridX, log->gridY)));
        std::snprintf(detailBuffer,
                      sizeof(detailBuffer),
                      "%s\n%s\n\n%s\n%s",
                      Loc_PickLiteral("Loxi Reconstruction", "洛希复核"),
                      sceneDef != nullptr ? Loc_PickText(sceneDef->body) : Loc_PickLiteral("Recovered archive frame is stable. Cross-check the transcript below to rebuild the full context.", "档案画面已经稳定，可结合下方原始记录重建完整语境。"),
                      Loc_PickLiteral("Recovered Transcript", "原始记录"),
                      Tasks_GetLogStoryText(log));
        TasksRuntime_SanitizeDisplayText(detailBuffer, sanitizedDetail, sizeof(sanitizedDetail));

        UIRuntime_DrawText(assets, locationBuffer, Vector2{contentPanel.x + 20.0f * scale, contentPanel.y + 50.0f * scale}, 15.0f * scale, Color{194, 224, 255, 255});
        UIRuntime_DrawPanel(imagePanel, Color{8, 16, 28, 218}, Color{104, 196, 222, 45});
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
        UIRuntime_DrawPanel(detailPanel, Color{8, 16, 28, 218}, Color{104, 196, 222, 45});
        UIRuntime_DrawText(assets, Loc_PickLiteral("Recovered Visual", "回收画面"), Vector2{imagePanel.x + 16.0f * scale, imagePanel.y + 14.0f * scale}, 18.0f * scale, Color{255, 214, 154, 255});
        UIRuntime_DrawWrappedText(assets,
                                  Loc_PickLiteral("Recovered frame linked to this archive.", "该档案关联的回收画面。"),
                                  transcriptRect,
                                  13.5f * scale,
                                  15.0f * scale,
                                  Color{222, 232, 240, 220});
        UIRuntime_DrawText(assets, Loc_PickLiteral("Detailed Record", "详细记录"), Vector2{detailPanel.x + 16.0f * scale, detailPanel.y + 16.0f * scale}, 18.0f * scale, Color{255, 214, 154, 255});
        UIRuntime_DrawWrappedText(assets, sanitizedDetail, detailRect, 15.5f * scale, 19.0f * scale, Color{227, 237, 245, 255});
        UIRuntime_DrawWrappedText(assets, buffer, rewardRect, 14.0f * scale, 16.0f * scale, Color{166, 255, 226, 255});
    }
}

}  // namespace

void UI_DrawCommunicatorOverlay(const AssetBundle *assets,
                                const TaskSystem *tasks,
                                int selectedTab,
                                int selectedLog,
                                int firstVisibleLog,
                                int screenWidth,
                                int screenHeight) {
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

    DrawRectangle(0, 0, screenWidth, screenHeight, Color{5, 9, 16, 190});
    UIRuntime_DrawPanel(panel, Color{8, 18, 30, 245}, Color{99, 233, 195, 75});
    UIRuntime_DrawText(assets, Loc_PickLiteral("Loxi Terminal", "洛希终端"), Vector2{panel.x + 30.0f * scale, panel.y + 20.0f * scale}, 34.0f * scale, WHITE);
    UIRuntime_DrawText(assets, closeHint, Vector2{panel.x + panel.width - UIRuntime_MeasureText(assets, closeHint, 17.5f * scale).x - 24.0f * scale, panel.y + 28.0f * scale}, 17.5f * scale, Color{182, 199, 214, 255});
    DrawCommunicatorTabs(assets, selectedTab, screenWidth, screenHeight, scale);

    if (selectedTab == kCommunicatorTabLogs) {
        DrawCommunicatorLogTab(assets, tasks, selectedLog, firstVisibleLog, listPanel, contentPanel, scale, screenWidth, screenHeight);
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
        {"B: open the backpack and inspect supplies", "B：打开背包并查看补给"},
        {"N: open the Loxi terminal for current task and archive logs", "N：打开洛希终端，查看当前任务与档案日志"},
        {"M: open the area map overlay", "M：打开区域地图"},
        {"H: open this help panel", "H：打开本帮助面板"},
        {"Z: use food for quick health and oxygen recovery", "Z：使用食物快速恢复生命与氧气"},
        {"X: use antidote and filter items for poison or leak relief", "X：使用解毒或过滤类物品缓解中毒与漏氧"},
        {"C: toggle crouch stealth", "C：切换蹲伏潜行"},
        {"ESC: pause", "ESC：暂停"},
        {"Sleep in Crew Quarters to recover. Use the oxygen console to refill oxygen, and camps only provide partial field recovery", "在船员舱睡觉可恢复状态；氧气控制台负责补氧；野外营地只提供有限恢复"}
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

        column = lineIndex / 6;
        row = lineIndex % 6;
        x = panel.x + 34.0f * scale + column * 468.0f * scale;
        y = panel.y + 108.0f * scale + row * 66.0f * scale;
        UIRuntime_DrawPanel(Rectangle{x, y, 434.0f * scale, 52.0f * scale}, Color{11, 20, 32, 228}, Color{255, 255, 255, 20});
        UIRuntime_DrawWrappedText(assets, Loc_PickText(lines[lineIndex]), Rectangle{x + 16.0f * scale, y + 10.0f * scale, 402.0f * scale, 34.0f * scale}, 16.5f * scale, 18.0f * scale, Color{229, 238, 246, 255});
    }
}
