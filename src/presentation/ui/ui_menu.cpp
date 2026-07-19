#include "ui_system.h"

#include "localization.h"
#include "ui_runtime_internal.h"
#include "ui_theme_internal.h"
#include "ui_components_internal.h"

#include <algorithm>
#include <cmath>
#include <cstdio>
#include <cstring>

/* Main-menu and pause-menu drawing only. */

static void DrawMainMenuTitle(const AssetBundle *assets, int screenWidth, float scale) {
    const UITheme *theme = UITheme_Get();
    const char *eyebrow;
    const char *localizedSubtitle;
    float centerX;
    float titleY;
    float lineWidth;
    Vector2 eyebrowSize;
    Vector2 subtitleSize;

    eyebrow = Loc_PickLiteral("LOXI ARCHIVE // EXPEDITION 07", "洛希档案 // 第 07 次远征");
    localizedSubtitle = Loc_PickLiteral("Survive the fall. Read the signal. Choose the ending.", "在坠落后求生 · 追踪信号 · 决定终局");
    centerX = screenWidth * 0.235f;
    titleY = 106.0f * scale;
    lineWidth = 380.0f * scale;

    DrawCircleGradient((int)centerX,
                       (int)(titleY + 90.0f * scale),
                       250.0f * scale,
                       Color{48, 121, 171, 78},
                       Color{7, 18, 32, 0});
    DrawTriangle(Vector2{centerX - 235.0f * scale, titleY + 24.0f * scale},
                 Vector2{centerX + 235.0f * scale, titleY + 156.0f * scale},
                 Vector2{centerX + 210.0f * scale, titleY - 4.0f * scale},
                 Color{4, 14, 25, 142});
    DrawTriangle(Vector2{centerX - 235.0f * scale, titleY + 24.0f * scale},
                 Vector2{centerX - 208.0f * scale, titleY + 174.0f * scale},
                 Vector2{centerX + 235.0f * scale, titleY + 156.0f * scale},
                 Color{4, 14, 25, 142});

    eyebrowSize = UIRuntime_MeasureText(assets, eyebrow, 13.5f * scale);
    UIRuntime_DrawText(assets,
                       eyebrow,
                       Vector2{centerX - eyebrowSize.x * 0.5f, titleY},
                       13.5f * scale,
                       theme->signalCyan);
    UIComponents_DrawDiamond(Vector2{centerX - eyebrowSize.x * 0.5f - 18.0f * scale, titleY + 7.0f * scale},
                             5.0f * scale,
                             Color{13, 49, 72, 255},
                             Color{107, 203, 228, 200});
    UIComponents_DrawDiamond(Vector2{centerX + eyebrowSize.x * 0.5f + 18.0f * scale, titleY + 7.0f * scale},
                             5.0f * scale,
                             Color{13, 49, 72, 255},
                             Color{107, 203, 228, 200});

    UITheme_DrawTitleText(assets,
                       "SPACECRAFT",
                       Vector2{centerX, titleY + 69.0f * scale},
                       60.0f * scale,
                       scale,
                       theme->iceWhite);
    UITheme_DrawTitleText(assets,
                       "LIVING",
                       Vector2{centerX, titleY + 126.0f * scale},
                       52.0f * scale,
                       scale,
                       Color{111, 194, 229, 255});

    DrawLineEx(Vector2{centerX - lineWidth * 0.5f, titleY + 165.0f * scale},
               Vector2{centerX - 18.0f * scale, titleY + 165.0f * scale},
               1.5f * scale,
               Color{58, 134, 200, 155});
    DrawLineEx(Vector2{centerX + 18.0f * scale, titleY + 165.0f * scale},
               Vector2{centerX + lineWidth * 0.5f, titleY + 165.0f * scale},
               1.5f * scale,
               Color{58, 134, 200, 155});
    UIComponents_DrawDiamond(Vector2{centerX, titleY + 165.0f * scale},
                             8.0f * scale,
                             Color{15, 58, 80, 255},
                             Color{107, 203, 228, 220});

    subtitleSize = UIRuntime_MeasureText(assets, localizedSubtitle, 16.5f * scale);
    UIRuntime_DrawText(assets,
                       localizedSubtitle,
                       Vector2{centerX - subtitleSize.x * 0.5f, titleY + 183.0f * scale},
                       16.5f * scale,
                       theme->textSecondary);
}

void UI_DrawMainMenu(const AssetBundle *assets,
                     bool hasSave,
                     int saveCount,
                     const char *accountName,
                     bool hasBestScore,
                     int bestScore,
                     int screenWidth,
                     int screenHeight,
                     float elapsedSeconds) {
    const UITheme *theme = UITheme_Get();
    int buttonIndex;
    const char *buttonLabels[MAIN_MENU_BUTTON_COUNT] = {
        LOC_UI_MENU_START,
        LOC_UI_MENU_LOAD,
        LOC_UI_MENU_SETTINGS,
        LOC_UI_MENU_EXIT
    };
    float scale;
    char accountBuffer[96];
    char bestScoreBuffer[64];
    char saveCountBuffer[64];
    Rectangle firstButton;
    Rectangle lastButton;
    Rectangle menuFrame;
    Rectangle accountPlaque;
    Rectangle accountInner;
    Vector2 accountSize;
    Vector2 headerSize;
    float plaqueRight;

    scale = UIRuntime_GetScale(screenWidth, screenHeight);

    UIRuntime_DrawBackdrop(screenWidth, screenHeight, elapsedSeconds);
    DrawMainMenuTitle(assets, screenWidth, scale);

    firstButton = UI_GetMainMenuButtonRect(screenWidth, screenHeight, 0);
    lastButton = UI_GetMainMenuButtonRect(screenWidth, screenHeight, MAIN_MENU_BUTTON_COUNT - 1);
    menuFrame = Rectangle{
        firstButton.x - 28.0f * scale,
        firstButton.y - 50.0f * scale,
        firstButton.width + 56.0f * scale,
        lastButton.y + lastButton.height - firstButton.y + 82.0f * scale
    };
    UIComponents_DrawFrame(menuFrame, scale, UI_FRAME_STANDARD, true);
    headerSize = UIRuntime_MeasureText(assets, Loc_PickLiteral("CHOOSE YOUR DESCENT", "选择你的远征"), 14.0f * scale);
    UIRuntime_DrawText(assets,
                       Loc_PickLiteral("CHOOSE YOUR DESCENT", "选择你的远征"),
                       Vector2{menuFrame.x + menuFrame.width * 0.5f - headerSize.x * 0.5f, menuFrame.y + 19.0f * scale},
                       14.0f * scale,
                       theme->signalCyan);
    DrawLineEx(Vector2{menuFrame.x + 46.0f * scale, menuFrame.y + 42.0f * scale},
               Vector2{menuFrame.x + menuFrame.width - 46.0f * scale, menuFrame.y + 42.0f * scale},
               1.0f,
               Color{58, 134, 200, 115});

    for (buttonIndex = 0; buttonIndex < MAIN_MENU_BUTTON_COUNT; buttonIndex++) {
        bool enabled;

        enabled = buttonIndex != MAIN_MENU_BUTTON_LOAD || hasSave;
        UIComponents_DrawFrontEndButton(assets,
                           UI_GetMainMenuButtonRect(screenWidth, screenHeight, buttonIndex),
                           buttonLabels[buttonIndex],
                           enabled,
                           false,
                           buttonIndex == MAIN_MENU_BUTTON_START,
                           scale);
    }

    std::snprintf(accountBuffer,
                  sizeof(accountBuffer),
                  "%s",
                  (accountName != NULL && accountName[0] != '\0') ? accountName : Loc_PickLiteral("Unknown", "未知身份"));
    if (hasBestScore) {
        std::snprintf(bestScoreBuffer, sizeof(bestScoreBuffer), "%s  %d", Loc_PickLiteral("BEST", "最高得分"), bestScore);
    } else {
        std::snprintf(bestScoreBuffer, sizeof(bestScoreBuffer), "%s", Loc_PickLiteral("BEST  --", "最高得分  --"));
    }
    std::snprintf(saveCountBuffer, sizeof(saveCountBuffer), "%s  %d", Loc_PickLiteral("ARCHIVES", "存档"), saveCount);

    accountPlaque = Rectangle{
        screenWidth - 318.0f * scale,
        42.0f * scale,
        272.0f * scale,
        132.0f * scale
    };
    plaqueRight = accountPlaque.x + accountPlaque.width;
    UIComponents_DrawFrame(accountPlaque, scale, UI_FRAME_COMPACT, true);
    accountInner = Rectangle{accountPlaque.x + 22.0f * scale, accountPlaque.y + 20.0f * scale, accountPlaque.width - 44.0f * scale, accountPlaque.height - 40.0f * scale};
    UIRuntime_DrawText(assets,
                       Loc_PickLiteral("ACTIVE FIELD ID", "当前远征身份"),
                       Vector2{accountInner.x, accountInner.y},
                       12.5f * scale,
                       theme->signalCyan);
    accountSize = UIRuntime_MeasureText(assets, accountBuffer, 21.0f * scale);
    UIRuntime_DrawText(assets,
                       accountBuffer,
                       Vector2{plaqueRight - 22.0f * scale - accountSize.x, accountInner.y + 23.0f * scale},
                       21.0f * scale,
                       theme->iceWhite);
    DrawLineEx(Vector2{accountInner.x, accountInner.y + 55.0f * scale},
               Vector2{accountInner.x + accountInner.width, accountInner.y + 55.0f * scale},
               1.0f,
               Color{58, 134, 200, 120});
    UIRuntime_DrawText(assets,
                       bestScoreBuffer,
                       Vector2{accountInner.x, accountInner.y + 67.0f * scale},
                       13.5f * scale,
                       theme->archiveGold);
    {
        Vector2 savesSize;
        savesSize = UIRuntime_MeasureText(assets, saveCountBuffer, 13.5f * scale);
        UIRuntime_DrawText(assets,
                           saveCountBuffer,
                           Vector2{accountInner.x + accountInner.width - savesSize.x, accountInner.y + 67.0f * scale},
                           13.5f * scale,
                           theme->textSecondary);
    }

    UIRuntime_DrawText(assets,
                       Loc_PickLiteral("A quiet signal is waiting beyond the ridge.", "山脊之后，一束沉默的信号仍在等待。"),
                       Vector2{screenWidth - 486.0f * scale, screenHeight - 52.0f * scale},
                       15.0f * scale,
                       Color{151, 181, 202, 225});
}

void UI_DrawPauseMenu(const AssetBundle *assets, int screenWidth, int screenHeight) {
    const UITheme *theme = UITheme_Get();
    const float scale = UIRuntime_GetScale(screenWidth, screenHeight);
    const Rectangle panel = UI_GetPauseMenuPanelRect(screenWidth, screenHeight);
    const char *buttonLabels[PAUSE_MENU_BUTTON_COUNT] = {
        LOC_UI_PAUSE_CONTINUE,
        LOC_UI_PAUSE_SAVE,
        LOC_UI_PAUSE_LOAD,
        LOC_UI_PAUSE_SETTINGS,
        LOC_UI_PAUSE_MENU
    };

    UIComponents_DrawScrim(screenWidth, screenHeight, 174);
    DrawCircleGradient((int)(panel.x + panel.width * 0.5f),
                       (int)(panel.y + panel.height * 0.45f),
                       panel.width * 0.82f,
                       Color{42, 116, 169, 52},
                       Color{7, 18, 32, 0});
    UIComponents_DrawFrame(panel, scale, UI_FRAME_STANDARD, true);
    UIComponents_DrawPanelHeader(assets,
                                 panel,
                                 Loc_PickLiteral("LOXI FIELD LINK // SUSPENDED", "洛希现场连接 // 已暂停"),
                                 Loc_PickLiteral("EXPEDITION PAUSED", "远征已暂停"),
                                 "",
                                 scale,
                                 theme->signalCyan);
    UIRuntime_DrawText(assets,
                       Loc_PickLiteral("Suit telemetry remains stable while the expedition is suspended.", "远征暂停期间，防护服遥测与当前状态保持不变。"),
                       Vector2{panel.x + 34.0f * scale, panel.y + 96.0f * scale},
                       14.5f * scale,
                       theme->textSecondary);

    for (int buttonIndex = 0; buttonIndex < PAUSE_MENU_BUTTON_COUNT; ++buttonIndex) {
        UIActionStyle style = UI_ACTION_SECONDARY;
        if (buttonIndex == PAUSE_MENU_BUTTON_CONTINUE) {
            style = UI_ACTION_PRIMARY;
        } else if (buttonIndex == PAUSE_MENU_BUTTON_MENU) {
            style = UI_ACTION_WARNING;
        }
        UIComponents_DrawActionButton(assets,
                                      UI_GetPauseMenuButtonRect(screenWidth, screenHeight, buttonIndex),
                                      buttonLabels[buttonIndex],
                                      true,
                                      false,
                                      style,
                                      scale);
    }
}
