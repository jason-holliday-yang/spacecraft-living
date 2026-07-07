#include "ui_system.h"

#include "localization.h"
#include "ui_runtime_internal.h"

#include <cmath>
#include <cstdio>
#include <cstring>

static bool TextContainsNonAscii(const char *text) {
    const unsigned char *cursor;

    if (text == NULL) {
        return false;
    }

    cursor = (const unsigned char *)text;
    while (*cursor != '\0') {
        if ((*cursor & 0x80u) != 0) {
            return true;
        }
        cursor++;
    }

    return false;
}

static void DrawPixelTitleText(const AssetBundle *assets, const char *text, Vector2 center, float fontSize, float scale, Color fill) {
    Font font;
    float spacing;
    float outlineOffset;
    float shadowOffset;
    float highlightOffset;
    Vector2 textSize;
    Vector2 position;
    bool useDefaultFont;
    bool useCjkTitleStyle;

    useDefaultFont = assets == NULL || !assets->uiFontLoaded;
    font = useDefaultFont ? GetFontDefault() : assets->uiFont;
    if (font.texture.id == 0) {
        return;
    }

    useCjkTitleStyle = TextContainsNonAscii(text);

    if (useDefaultFont || useCjkTitleStyle) {
        SetTextureFilter(font.texture, TEXTURE_FILTER_POINT);
    }

    spacing = useCjkTitleStyle ? 0.0f : (useDefaultFont ? std::floor(fontSize * 0.12f) : fontSize * 0.05f);
    outlineOffset = std::round((useCjkTitleStyle ? 2.0f : 3.0f) * scale);
    shadowOffset = std::round((useCjkTitleStyle ? 4.0f : 7.0f) * scale);
    highlightOffset = std::round((useCjkTitleStyle ? 1.0f : 2.0f) * scale);
    textSize = MeasureTextEx(font, text, fontSize, spacing);
    position = Vector2{
        std::round(center.x - textSize.x * 0.5f),
        std::round(center.y - textSize.y * 0.5f)
    };

    DrawTextEx(font,
               text,
               Vector2{position.x + shadowOffset, position.y + shadowOffset},
               fontSize,
               spacing,
               Color{3, 7, 12, (unsigned char)(useCjkTitleStyle ? 150 : 180)});
    if (useCjkTitleStyle) {
        DrawTextEx(font, text, Vector2{position.x, position.y + outlineOffset}, fontSize, spacing, Color{8, 18, 30, 220});
        DrawTextEx(font, text, Vector2{position.x + outlineOffset, position.y}, fontSize, spacing, Color{8, 18, 30, 200});
    } else {
        DrawTextEx(font, text, Vector2{position.x - outlineOffset, position.y}, fontSize, spacing, Color{8, 18, 30, 255});
        DrawTextEx(font, text, Vector2{position.x + outlineOffset, position.y}, fontSize, spacing, Color{8, 18, 30, 255});
        DrawTextEx(font, text, Vector2{position.x, position.y - outlineOffset}, fontSize, spacing, Color{8, 18, 30, 255});
        DrawTextEx(font, text, Vector2{position.x, position.y + outlineOffset}, fontSize, spacing, Color{8, 18, 30, 255});
    }
    DrawTextEx(font, text, position, fontSize, spacing, fill);
    DrawTextEx(font,
               text,
               Vector2{position.x, position.y - highlightOffset},
               fontSize,
               spacing,
               Color{255, 255, 255, (unsigned char)(useCjkTitleStyle ? 76 : 96)});

    if (!useDefaultFont && useCjkTitleStyle) {
        SetTextureFilter(font.texture, TEXTURE_FILTER_BILINEAR);
    }
}

static void DrawMainMenuTitle(const AssetBundle *assets, int screenWidth, float scale) {
    const char *eyebrow;
    const char *englishTitle;
    const char *localizedSubtitle;
    Rectangle titlePanel;
    Vector2 eyebrowSize;
    Vector2 subtitleSize;
    Color subtitleTint;

    eyebrow = Loc_PickLiteral("Loxi archive // survival reconstruction", "洛希档案 // 生存复原");
    englishTitle = "SpaceCraft Living";
    localizedSubtitle = Loc_PickLiteral("Survival, investigation, and final choice", "异星求生");
    subtitleTint = Loc_GetLanguage() == GAME_LANGUAGE_ZH_CN
        ? Color{188, 233, 255, 255}
        : Color{176, 221, 246, 255};

    titlePanel = Rectangle{
        screenWidth * 0.5f - 360.0f * scale,
        86.0f * scale,
        720.0f * scale,
        190.0f * scale
    };

    DrawRectangleRounded(titlePanel, 0.18f, 10, Color{7, 16, 28, 158});
    DrawRectangleRoundedLinesEx(titlePanel, 0.18f, 10, 1.4f, Color{130, 192, 235, 65});
    DrawCircleGradient((int)(screenWidth * 0.5f), (int)(titlePanel.y + 82.0f * scale), 220.0f * scale, Color{98, 184, 235, 30}, Color{98, 184, 235, 0});

    eyebrowSize = UIRuntime_MeasureText(assets, eyebrow, 13.5f * scale);
    UIRuntime_DrawText(assets,
                       eyebrow,
                       Vector2{screenWidth * 0.5f - eyebrowSize.x * 0.5f, titlePanel.y + 26.0f * scale},
                       13.5f * scale,
                       Color{144, 206, 238, 255});

    DrawPixelTitleText(assets,
                       englishTitle,
                       Vector2{screenWidth * 0.5f, titlePanel.y + 90.0f * scale},
                       72.0f * scale,
                       scale,
                       Color{236, 246, 255, 255});

    subtitleSize = UIRuntime_MeasureText(assets, localizedSubtitle, 28.0f * scale);
    UIRuntime_DrawText(assets,
                       localizedSubtitle,
                       Vector2{screenWidth * 0.5f - subtitleSize.x * 0.5f, titlePanel.y + 130.0f * scale},
                       28.0f * scale,
                       subtitleTint);
}

static void DrawVolumePreviewBars(Rectangle rect, float volume) {
    int barIndex;

    for (barIndex = 0; barIndex < 12; ++barIndex) {
        float ratio;
        float barWidth;
        float barHeight;
        float x;
        float y;
        bool active;

        ratio = (float)(barIndex + 1) / 12.0f;
        barWidth = rect.width / 16.0f;
        barHeight = rect.height * (0.22f + ratio * 0.78f);
        x = rect.x + barIndex * (barWidth + rect.width / 44.0f);
        y = rect.y + rect.height - barHeight;
        active = volume >= ratio - (1.0f / 24.0f);
        DrawRectangleRounded(Rectangle{x, y, barWidth, barHeight},
                             0.35f,
                             6,
                             active ? Color{94, 217, 199, 255} : Color{39, 58, 77, 255});
    }
}

static float GetSettingsVolumeValue(const GameSettings *settings, int sliderIndex) {
    if (settings == NULL) {
        return 0.0f;
    }

    switch (sliderIndex) {
        case 0:
            return settings->masterVolume;
        case 1:
            return settings->musicVolume;
        case 2:
            return settings->sfxVolume;
        default:
            return 0.0f;
    }
}

static void DrawSettingsSliderRow(const AssetBundle *assets,
                                  const GameSettings *settings,
                                  int sliderIndex,
                                  const char *title,
                                  const char *description,
                                  int screenWidth,
                                  int screenHeight) {
    float scale;
    Rectangle rowRect;
    Rectangle sliderRect;
    Rectangle decreaseRect;
    Rectangle increaseRect;
    Rectangle handleRect;
    Rectangle previewRect;
    float filledWidth;
    float value;
    char valueBuffer[16];

    scale = UIRuntime_GetScale(screenWidth, screenHeight);
    rowRect = UI_GetSettingsRowRect(screenWidth, screenHeight, sliderIndex);
    sliderRect = UI_GetSettingsSliderRect(screenWidth, screenHeight, sliderIndex);
    decreaseRect = UI_GetSettingsDecreaseButtonRect(screenWidth, screenHeight, sliderIndex);
    increaseRect = UI_GetSettingsIncreaseButtonRect(screenWidth, screenHeight, sliderIndex);
    value = GetSettingsVolumeValue(settings, sliderIndex);
    filledWidth = sliderRect.width * value;
    handleRect = Rectangle{
        sliderRect.x + filledWidth - 14.0f * scale,
        sliderRect.y - 8.0f * scale,
        28.0f * scale,
        sliderRect.height + 16.0f * scale
    };
    previewRect = Rectangle{
        rowRect.x + 190.0f * scale,
        rowRect.y + 18.0f * scale,
        42.0f * scale,
        36.0f * scale
    };

    UIRuntime_DrawPanel(rowRect, Color{12, 24, 39, 225}, Color{104, 196, 222, 50});
    UIRuntime_DrawText(assets, title, Vector2{rowRect.x + 22.0f * scale, rowRect.y + 15.0f * scale}, 23.0f * scale, WHITE);
    UIRuntime_DrawWrappedText(assets,
                              description,
                              Rectangle{rowRect.x + 22.0f * scale, rowRect.y + 42.0f * scale, 220.0f * scale, 18.0f * scale},
                              12.5f * scale,
                              13.5f * scale,
                              Color{194, 224, 255, 255});
    DrawVolumePreviewBars(previewRect, value);
    UIRuntime_DrawPanel(sliderRect, Color{18, 30, 46, 235}, Color{104, 196, 222, 70});
    if (filledWidth > 6.0f * scale) {
        DrawRectangleRounded(Rectangle{sliderRect.x + 3.0f * scale, sliderRect.y + 3.0f * scale, filledWidth - 6.0f * scale, sliderRect.height - 6.0f * scale}, 0.35f, 8, Color{94, 217, 199, 255});
    }
    UIRuntime_DrawPanel(handleRect, Color{255, 214, 154, 245}, Color{255, 255, 255, 90});
    UIRuntime_DrawButton(assets, decreaseRect, "-", true);
    UIRuntime_DrawButton(assets, increaseRect, "+", true);

    std::snprintf(valueBuffer, sizeof(valueBuffer), "%d", (int)std::round(value * 100.0f));
    UIRuntime_DrawText(assets,
                       valueBuffer,
                       Vector2{rowRect.x + rowRect.width - 95.0f * scale, rowRect.y + 25.0f * scale},
                       21.0f * scale,
                       Color{255, 214, 154, 255});
    UIRuntime_DrawText(assets,
                       "%",
                       Vector2{rowRect.x + rowRect.width - 63.0f * scale, rowRect.y + 26.0f * scale},
                       18.0f * scale,
                       Color{194, 224, 255, 255});
}

static void DrawAuthInputField(const AssetBundle *assets,
                               const char *label,
                               const char *value,
                               Rectangle rect,
                               bool selected,
                               bool maskValue,
                               float scale) {
    char displayValue[SAVE_ACCOUNT_PASSWORD_MAX];
    size_t index;
    Color outline;

    std::memset(displayValue, 0, sizeof(displayValue));
    if (value != NULL) {
        if (maskValue) {
            size_t valueLength = std::strlen(value);

            if (valueLength >= sizeof(displayValue)) {
                valueLength = sizeof(displayValue) - 1;
            }
            for (index = 0; index < valueLength; ++index) {
                displayValue[index] = '*';
            }
        } else {
            std::snprintf(displayValue, sizeof(displayValue), "%s", value);
        }
    }

    outline = selected ? Color{255, 214, 154, 220} : Color{104, 196, 222, 70};
    UIRuntime_DrawPanel(rect, Color{14, 24, 40, 230}, outline);
    UIRuntime_DrawText(assets, label, Vector2{rect.x + 18.0f * scale, rect.y + 10.0f * scale}, 14.0f * scale, Color{166, 255, 226, 255});
    UIRuntime_DrawText(
        assets,
        displayValue[0] != '\0'
            ? displayValue
            : (maskValue ? Loc_PickLiteral("Enter password", "请输入密码") : Loc_PickLiteral("Enter username", "请输入用户名")),
        Vector2{rect.x + 18.0f * scale, rect.y + 30.0f * scale},
        22.0f * scale,
        displayValue[0] != '\0' ? WHITE : Color{130, 148, 171, 255}
    );
}

void UI_DrawAuthScreen(const AssetBundle *assets,
                       AuthScreenMode mode,
                       const char *username,
                       const char *password,
                       bool passwordVisible,
                       AuthField selectedField,
                       const char *message,
                       bool hasAccounts,
                       int screenWidth,
                       int screenHeight,
                       float elapsedSeconds) {
    float scale;
    Rectangle panel;
    Rectangle usernameRect;
    Rectangle passwordRect;
    Rectangle togglePasswordRect;
    Rectangle deleteRect;
    Rectangle submitRect;
    Rectangle switchRect;
    Rectangle exitRect;
    const char *title;
    const char *body;
    const char *submitLabel;
    const char *switchLabel;
    const char *accountsHint;
    bool deleteEnabled;
    Color messageColor;

    scale = UIRuntime_GetScale(screenWidth, screenHeight);
    panel = UI_GetAuthPanelRect(screenWidth, screenHeight);
    usernameRect = UI_GetAuthInputRect(screenWidth, screenHeight, 0);
    passwordRect = UI_GetAuthInputRect(screenWidth, screenHeight, 1);
    togglePasswordRect = UI_GetAuthPasswordToggleRect(screenWidth, screenHeight);
    deleteRect = UI_GetAuthDeleteAccountRect(screenWidth, screenHeight);
    submitRect = UI_GetAuthSubmitButtonRect(screenWidth, screenHeight);
    switchRect = UI_GetAuthSwitchModeRect(screenWidth, screenHeight);
    exitRect = UI_GetAuthExitButtonRect(screenWidth, screenHeight);
    title = mode == AUTH_SCREEN_MODE_REGISTER
        ? Loc_PickLiteral("Create Local Account", "创建本地账号")
        : Loc_PickLiteral("Local Sign-In", "账号登录");
    body = mode == AUTH_SCREEN_MODE_REGISTER
        ? Loc_PickLiteral("Create a local account to continue.", "创建本地账号以继续。")
        : Loc_PickLiteral("Sign in with a local account to continue.", "使用本地账号登录。");
    submitLabel = mode == AUTH_SCREEN_MODE_REGISTER ? Loc_PickLiteral("Create and Continue", "注册并进入") : Loc_PickLiteral("Sign In", "登录");
    switchLabel = mode == AUTH_SCREEN_MODE_REGISTER ? Loc_PickLiteral("Sign In Instead", "使用已有账号") : Loc_PickLiteral("Create Account", "创建新账号");
    accountsHint = hasAccounts ? Loc_PickLiteral("Local accounts are already available on this device.", "此设备上已存在本地账号。") : Loc_PickLiteral("No local account is saved on this device yet.", "当前还没有本地账号，请先注册。");
    deleteEnabled = mode == AUTH_SCREEN_MODE_LOGIN && hasAccounts;
    messageColor = Color{255, 214, 154, 255};
    if (message != NULL
        && (std::strstr(message, "Logged in") != NULL
            || std::strstr(message, "created") != NULL
            || std::strstr(message, "signed in") != NULL
            || std::strstr(message, "已登录") != NULL
            || std::strstr(message, "已创建并登录") != NULL)) {
        messageColor = Color{166, 255, 226, 255};
    }

    UIRuntime_DrawBackdrop(screenWidth, screenHeight, elapsedSeconds);
    DrawCircleGradient((int)(screenWidth * 0.5f - 260.0f * scale), (int)(screenHeight * 0.5f - 210.0f * scale), 180.0f * scale, Color{65, 164, 214, 48}, Color{65, 164, 214, 0});
    DrawCircleGradient((int)(screenWidth * 0.5f + 260.0f * scale), (int)(screenHeight * 0.5f + 210.0f * scale), 180.0f * scale, Color{110, 227, 196, 40}, Color{110, 227, 196, 0});

    UIRuntime_DrawPanel(panel, Color{8, 18, 30, 240}, Color{153, 226, 255, 78});
    UIRuntime_DrawText(assets, title, Vector2{panel.x + 52.0f * scale, panel.y + 34.0f * scale}, 36.0f * scale, WHITE);
    UIRuntime_DrawButton(assets, exitRect, Loc_PickLiteral("Exit", "退出"), true);
    if (selectedField == AUTH_FIELD_EXIT_GAME) {
        DrawRectangleRoundedLinesEx(exitRect, 0.16f, 8, 2.0f, Color{255, 214, 154, 220});
    }
    UIRuntime_DrawWrappedText(assets, body, Rectangle{panel.x + 52.0f * scale, panel.y + 82.0f * scale, panel.width - 104.0f * scale, 52.0f * scale}, 17.0f * scale, 19.0f * scale, Color{196, 214, 230, 255});
    UIRuntime_DrawText(assets, accountsHint, Vector2{panel.x + 52.0f * scale, panel.y + 136.0f * scale}, 15.0f * scale, Color{166, 255, 226, 255});

    DrawAuthInputField(assets, Loc_PickLiteral("Username", "用户名"), username, usernameRect, selectedField == AUTH_FIELD_USERNAME, false, scale);
    DrawAuthInputField(assets, Loc_PickLiteral("Password", "密码"), password, passwordRect, selectedField == AUTH_FIELD_PASSWORD, !passwordVisible, scale);
    UIRuntime_DrawButton(assets, togglePasswordRect, passwordVisible ? Loc_PickLiteral("Hide", "隐藏") : Loc_PickLiteral("Show", "显示"), true);
    if (selectedField == AUTH_FIELD_TOGGLE_PASSWORD) {
        DrawRectangleRoundedLinesEx(togglePasswordRect, 0.16f, 8, 2.0f, Color{255, 214, 154, 220});
    }

    UIRuntime_DrawButton(assets, submitRect, submitLabel, true);
    if (selectedField == AUTH_FIELD_SUBMIT) {
        DrawRectangleRoundedLinesEx(submitRect, 0.16f, 8, 2.0f, Color{255, 214, 154, 220});
    }
    UIRuntime_DrawButton(assets, deleteRect, Loc_PickLiteral("Delete Account", "删除账号"), deleteEnabled);
    if (deleteEnabled && selectedField == AUTH_FIELD_DELETE_ACCOUNT) {
        DrawRectangleRoundedLinesEx(deleteRect, 0.16f, 8, 2.0f, Color{255, 214, 154, 220});
    }
    UIRuntime_DrawButton(assets, switchRect, switchLabel, true);
    if (selectedField == AUTH_FIELD_SWITCH_MODE) {
        DrawRectangleRoundedLinesEx(switchRect, 0.16f, 8, 2.0f, Color{255, 214, 154, 220});
    }

    if (message != NULL && message[0] != '\0') {
        UIRuntime_DrawWrappedText(
            assets,
            message,
            Rectangle{panel.x + 52.0f * scale, panel.y + panel.height - 224.0f * scale, panel.width - 104.0f * scale, 40.0f * scale},
            15.0f * scale,
            17.0f * scale,
            messageColor
        );
    }
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
    float accountWidth;
    float bestScoreWidth;
    float saveWidth;
    float rightTextX;

    scale = UIRuntime_GetScale(screenWidth, screenHeight);

    UIRuntime_DrawBackdrop(screenWidth, screenHeight, elapsedSeconds);
    DrawCircleGradient((int)(screenWidth * 0.5f - 210.0f * scale), (int)(184.0f * scale), 156.0f * scale, Color{65, 164, 214, 56}, Color{65, 164, 214, 0});
    DrawCircleGradient((int)(screenWidth * 0.5f + 210.0f * scale), (int)(188.0f * scale), 156.0f * scale, Color{110, 227, 196, 44}, Color{110, 227, 196, 0});
    DrawMainMenuTitle(assets, screenWidth, scale);

    std::snprintf(accountBuffer, sizeof(accountBuffer), "%s", (accountName != NULL && accountName[0] != '\0') ? accountName : Loc_PickLiteral("Unknown", "未知"));
    if (hasBestScore) {
        std::snprintf(bestScoreBuffer, sizeof(bestScoreBuffer), "%s %d", Loc_PickLiteral("Best Score", "最高得分"), bestScore);
    } else {
        bestScoreBuffer[0] = '\0';
    }
    std::snprintf(saveCountBuffer, sizeof(saveCountBuffer), "%s %d", Loc_PickLiteral("Saves", "存档"), saveCount);
    accountWidth = UIRuntime_MeasureText(assets, accountBuffer, 18.5f * scale).x;
    bestScoreWidth = bestScoreBuffer[0] != '\0' ? UIRuntime_MeasureText(assets, bestScoreBuffer, 15.5f * scale).x : 0.0f;
    saveWidth = UIRuntime_MeasureText(assets, saveCountBuffer, 15.5f * scale).x;
    rightTextX = screenWidth - 36.0f * scale - accountWidth;
    if (bestScoreWidth > accountWidth && bestScoreWidth > saveWidth) {
        rightTextX = screenWidth - 36.0f * scale - bestScoreWidth;
    } else if (saveWidth > accountWidth && saveWidth > bestScoreWidth) {
        rightTextX = screenWidth - 36.0f * scale - saveWidth;
    }
    UIRuntime_DrawText(assets, accountBuffer, Vector2{rightTextX, 112.0f * scale}, 18.5f * scale, Color{166, 255, 226, 255});
    if (bestScoreBuffer[0] != '\0') {
        UIRuntime_DrawText(assets, bestScoreBuffer, Vector2{rightTextX, 136.0f * scale}, 15.5f * scale, Color{255, 214, 154, 255});
        UIRuntime_DrawText(assets, saveCountBuffer, Vector2{rightTextX, 158.0f * scale}, 15.5f * scale, Color{194, 224, 255, 255});
    } else {
        UIRuntime_DrawText(assets, saveCountBuffer, Vector2{rightTextX, 138.0f * scale}, 15.5f * scale, Color{194, 224, 255, 255});
    }

    for (buttonIndex = 0; buttonIndex < MAIN_MENU_BUTTON_COUNT; buttonIndex++) {
        bool enabled;

        enabled = buttonIndex != MAIN_MENU_BUTTON_LOAD || hasSave;
        UIRuntime_DrawButton(assets, UI_GetMainMenuButtonRect(screenWidth, screenHeight, buttonIndex), buttonLabels[buttonIndex], enabled);
    }
}

void UI_DrawPauseMenu(const AssetBundle *assets, int screenWidth, int screenHeight) {
    Rectangle panel;
    int buttonIndex;
    const char *buttonLabels[PAUSE_MENU_BUTTON_COUNT] = {
        LOC_UI_PAUSE_CONTINUE,
        LOC_UI_PAUSE_SAVE,
        LOC_UI_PAUSE_LOAD,
        LOC_UI_PAUSE_SETTINGS,
        LOC_UI_PAUSE_MENU
    };

    panel = UI_GetPauseMenuPanelRect(screenWidth, screenHeight);

    DrawRectangle(0, 0, screenWidth, screenHeight, Color{6, 10, 18, 160});
    UIRuntime_DrawPanel(panel, Color{8, 18, 30, 235}, Color{153, 226, 255, 70});

    for (buttonIndex = 0; buttonIndex < PAUSE_MENU_BUTTON_COUNT; buttonIndex++) {
        UIRuntime_DrawButton(assets, UI_GetPauseMenuButtonRect(screenWidth, screenHeight, buttonIndex), buttonLabels[buttonIndex], true);
    }
}

void UI_DrawSettingsOverlay(const AssetBundle *assets,
                            const GameSettings *settings,
                            const char *accountName,
                            int saveCount,
                            bool accountActionsEnabled,
                            int screenWidth,
                            int screenHeight) {
    float scale;
    Rectangle panel;
    Rectangle closeRect;
    Rectangle languageRowRect;
    Rectangle accountRowRect;
    Rectangle languageEnglishRect;
    Rectangle languageChineseRect;
    Rectangle accountSwitchRect;
    Rectangle accountDeleteRect;
    char accountBuffer[128];
    const char *accountHint;

    scale = UIRuntime_GetScale(screenWidth, screenHeight);
    panel = UI_GetStandardOverlayRect(screenWidth, screenHeight);
    closeRect = UI_GetSettingsCloseButtonRect(screenWidth, screenHeight);
    languageRowRect = UI_GetSettingsRowRect(screenWidth, screenHeight, 3);
    accountRowRect = UI_GetSettingsRowRect(screenWidth, screenHeight, 4);
    languageEnglishRect = UI_GetSettingsLanguageButtonRect(screenWidth, screenHeight, 0);
    languageChineseRect = UI_GetSettingsLanguageButtonRect(screenWidth, screenHeight, 1);
    accountSwitchRect = UI_GetSettingsAccountButtonRect(screenWidth, screenHeight, 0);
    accountDeleteRect = UI_GetSettingsAccountButtonRect(screenWidth, screenHeight, 1);
    std::snprintf(accountBuffer,
                  sizeof(accountBuffer),
                  "%s  |  %s %d",
                  (accountName != NULL && accountName[0] != '\0') ? accountName : Loc_PickLiteral("Unknown", "未知"),
                  Loc_PickLiteral("Saves", "存档"),
                  saveCount);
    accountHint = accountActionsEnabled
        ? Loc_PickLiteral("Manage the active local account here.", "在这里管理当前本地账号。")
        : Loc_PickLiteral("Account actions are available from the main menu settings.", "账号操作仅在主界面的设置页中可用。");

    DrawRectangle(0, 0, screenWidth, screenHeight, Color{5, 9, 16, 208});
    UIRuntime_DrawPanel(panel, Color{8, 18, 30, 245}, Color{153, 226, 255, 75});
    UIRuntime_DrawText(assets, LOC_UI_SETTINGS_TITLE, Vector2{panel.x + 34.0f * scale, panel.y + 28.0f * scale}, 34.0f * scale, WHITE);
    UIRuntime_DrawButton(assets, closeRect, LOC_UI_CLOSE, true);
    UIRuntime_DrawText(assets,
                       Loc_PickLiteral("Audio, language, and account options all live here now.", "音频、语言与账号选项现在统一放在这里。"),
                       Vector2{panel.x + 34.0f * scale, panel.y + 72.0f * scale},
                       16.0f * scale,
                       Color{194, 224, 255, 255});
    UIRuntime_DrawText(
        assets,
        LOC_UI_PRESS_ESC_CLOSE,
        Vector2{
            closeRect.x - UIRuntime_MeasureText(assets, LOC_UI_PRESS_ESC_CLOSE, 16.0f * scale).x - 18.0f * scale,
            closeRect.y + closeRect.height * 0.5f - UIRuntime_MeasureText(assets, LOC_UI_PRESS_ESC_CLOSE, 16.0f * scale).y * 0.5f
        },
        16.0f * scale,
        Color{182, 199, 214, 255}
    );

    DrawSettingsSliderRow(assets,
                          settings,
                          0,
                          Loc_PickLiteral("Master Volume", "总音量"),
                          Loc_PickLiteral("Controls the full game mix.", "控制整个游戏的总体音量。"),
                          screenWidth,
                          screenHeight);
    DrawSettingsSliderRow(assets,
                          settings,
                          1,
                          Loc_PickLiteral("Music Volume", "音乐音量"),
                          Loc_PickLiteral("Adjusts background music only.", "只调整背景音乐。"),
                          screenWidth,
                          screenHeight);
    DrawSettingsSliderRow(assets,
                          settings,
                          2,
                          Loc_PickLiteral("SFX Volume", "音效音量"),
                          Loc_PickLiteral("Adjusts UI, combat, and ambient cues.", "调整界面、战斗与环境音效。"),
                          screenWidth,
                          screenHeight);

    UIRuntime_DrawPanel(languageRowRect, Color{12, 24, 39, 225}, Color{104, 196, 222, 50});
    UIRuntime_DrawText(assets, LOC_UI_LANGUAGE, Vector2{languageRowRect.x + 22.0f * scale, languageRowRect.y + 15.0f * scale}, 23.0f * scale, WHITE);
    UIRuntime_DrawWrappedText(assets,
                              Loc_PickLiteral("Switch between English and Simplified Chinese.", "在英文和简体中文之间切换。"),
                              Rectangle{languageRowRect.x + 22.0f * scale, languageRowRect.y + 42.0f * scale, 230.0f * scale, 18.0f * scale},
                              12.5f * scale,
                              13.5f * scale,
                              Color{194, 224, 255, 255});
    UIRuntime_DrawButton(assets, languageEnglishRect, Loc_GetLanguageNativeName(GAME_LANGUAGE_EN), true);
    UIRuntime_DrawButton(assets, languageChineseRect, "简体中文", true);
    if (Loc_NormalizeLanguage((int)settings->language) == GAME_LANGUAGE_EN) {
        DrawRectangleRoundedLinesEx(languageEnglishRect, 0.16f, 8, 2.0f, Color{255, 214, 154, 220});
    } else {
        DrawRectangleRoundedLinesEx(languageChineseRect, 0.16f, 8, 2.0f, Color{255, 214, 154, 220});
    }

    UIRuntime_DrawPanel(accountRowRect, Color{12, 24, 39, 225}, Color{104, 196, 222, 50});
    UIRuntime_DrawText(assets, Loc_PickLiteral("Account", "账号"), Vector2{accountRowRect.x + 22.0f * scale, accountRowRect.y + 15.0f * scale}, 23.0f * scale, WHITE);
    UIRuntime_DrawText(assets, accountBuffer, Vector2{accountRowRect.x + 22.0f * scale, accountRowRect.y + 42.0f * scale}, 13.5f * scale, Color{166, 255, 226, 255});
    UIRuntime_DrawButton(assets, accountSwitchRect, Loc_PickLiteral("Switch Account", "切换账号"), accountActionsEnabled);
    UIRuntime_DrawButton(assets, accountDeleteRect, Loc_PickLiteral("Delete Account", "删除账号"), accountActionsEnabled);
    UIRuntime_DrawWrappedText(assets,
                              accountHint,
                              Rectangle{accountRowRect.x + 248.0f * scale, accountRowRect.y + 52.0f * scale, accountRowRect.width - 618.0f * scale, 14.0f * scale},
                              11.6f * scale,
                              13.0f * scale,
                              accountActionsEnabled ? Color{194, 224, 255, 255} : Color{150, 166, 184, 255});
}

void UI_DrawDeathPopup(const Player *player, bool hasSave, const AssetBundle *assets, int screenWidth, int screenHeight, int selectedButton) {
    float scale;
    Rectangle panel;
    Rectangle bodyRect;
    Rectangle hintRect;
    char deathBuffer[128];
    const char *buttonLabels[DEATH_POPUP_BUTTON_COUNT] = {
        Loc_PickLiteral("Restart", "重新开始"),
        Loc_PickLiteral("Load", "读档"),
        Loc_PickLiteral("Main Menu", "主菜单")
    };
    int buttonIndex;

    scale = UIRuntime_GetScale(screenWidth, screenHeight);
    panel = Rectangle{screenWidth * 0.5f - 350.0f * scale, screenHeight * 0.5f - 170.0f * scale, 700.0f * scale, 340.0f * scale};
    bodyRect = Rectangle{panel.x + 54.0f * scale, panel.y + 132.0f * scale, panel.width - 108.0f * scale, 28.0f * scale};
    hintRect = Rectangle{panel.x + 54.0f * scale, panel.y + 166.0f * scale, panel.width - 108.0f * scale, 44.0f * scale};

    DrawRectangle(0, 0, screenWidth, screenHeight, Color{10, 5, 5, 220});

    UIRuntime_DrawPanel(panel, Color{30, 10, 10, 245}, Color{255, 80, 80, 90});

    UIRuntime_DrawText(assets, Loc_PickLiteral("YOU DIED", "你已死亡"), Vector2{panel.x + panel.width * 0.5f - UIRuntime_MeasureText(assets, Loc_PickLiteral("YOU DIED", "你已死亡"), 38.0f * scale).x * 0.5f, panel.y + 35.0f * scale}, 38.0f * scale, Color{255, 80, 80, 255});

    std::snprintf(deathBuffer, sizeof(deathBuffer), "%s %d", Loc_PickLiteral("Deaths", "死亡次数"), player->deathCount);
    UIRuntime_DrawText(assets, deathBuffer, Vector2{panel.x + panel.width * 0.5f - UIRuntime_MeasureText(assets, deathBuffer, 22.0f * scale).x * 0.5f, panel.y + 95.0f * scale}, 22.0f * scale, Color{200, 200, 200, 255});

    UIRuntime_DrawWrappedText(assets,
                              Loc_PickLiteral("Suit vital systems have failed. This run is over as soon as your health is depleted.",
                                              "防护服生命体征系统已经崩溃。生命值归零后，本轮会立刻结束。"),
                              bodyRect,
                              19.0f * scale,
                              22.0f * scale,
                              Color{180, 180, 180, 255});
    UIRuntime_DrawWrappedText(assets,
                              hasSave
                                ? Loc_PickLiteral("Restart begins a fresh run immediately from Loxi's room and skips the opening crash recap. Load lets you return to an existing save.",
                                                  "重新开始会直接从洛希房间重新开局，并跳过开场坠毁剧情。读档可以返回已有存档。")
                                : Loc_PickLiteral("Restart begins a fresh run immediately from Loxi's room and skips the opening crash recap. No save is available to load for this account.",
                                                  "重新开始会直接从洛希房间重新开局，并跳过开场坠毁剧情。当前账号没有可读取的存档。"),
                              hintRect,
                              16.0f * scale,
                              18.0f * scale,
                              Color{196, 184, 184, 255});

    DrawLine(panel.x + 40.0f * scale, panel.y + 212.0f * scale, panel.x + panel.width - 40.0f * scale, panel.y + 212.0f * scale, Color{255, 100, 100, 120});

    for (buttonIndex = 0; buttonIndex < DEATH_POPUP_BUTTON_COUNT; buttonIndex++) {
        Rectangle buttonRect;
        bool enabled;

        buttonRect = UI_GetDeathPopupButtonRect(screenWidth, screenHeight, buttonIndex);
        enabled = buttonIndex != DEATH_POPUP_BUTTON_LOAD || hasSave;
        UIRuntime_DrawButton(assets, buttonRect, buttonLabels[buttonIndex], enabled);
        if (buttonIndex == selectedButton) {
            DrawRectangleRoundedLinesEx(buttonRect, 0.16f, 8, 2.0f, Color{255, 214, 154, 220});
        }
    }
}

void UI_DrawSettlementConfirmPopup(const AssetBundle *assets,
                                   const Player *player,
                                   const TaskSystem *tasks,
                                   int screenWidth,
                                   int screenHeight,
                                   int buttonCount,
                                   int selectedButton) {
    (void)player;
    float scale;
    Rectangle panel;
    Rectangle bodyRect;
    Rectangle noteRect;
    const char *titleText;
    const char *bodyText;
    const char *noteText;
    const char *buttonLabels[SETTLEMENT_CONFIRM_BUTTON_COUNT] = {0};
    const int availableEndingCount = tasks != NULL ? Tasks_GetAvailableEndingCount(tasks) : 0;
    char singleEndingBody[320];
    char singleEndingNote[160];
    int buttonIndex;

    scale = UIRuntime_GetScale(screenWidth, screenHeight);
    panel = UI_GetSettlementConfirmPanelRect(screenWidth, screenHeight);
    bodyRect = Rectangle{panel.x + 34.0f * scale, panel.y + 96.0f * scale, panel.width - 68.0f * scale, 62.0f * scale};
    noteRect = Rectangle{panel.x + 34.0f * scale, panel.y + 166.0f * scale, panel.width - 68.0f * scale, 50.0f * scale};
    if (availableEndingCount <= 1) {
        GameEnding onlyEnding = tasks != NULL ? Tasks_GetAvailableEndingAt(tasks, 0) : ENDING_NONE;
        const char *onlyEndingTitle = Tasks_GetEndingTitle(onlyEnding);

        titleText = Loc_PickLiteral("Confirm Ending", "确认结局");
        std::snprintf(singleEndingBody,
                      sizeof(singleEndingBody),
                      Loc_PickLiteral("Loxi's archive review now supports only one final answer: %s. Confirming here means you are no longer choosing in uncertainty; you are accepting the route the full record now makes possible. The run will still continue until you carry out its final in-world step.",
                                      "洛希完成档案复核后，目前只剩下一个被完整证据支撑的终局回答：%s。在这里确认，不再只是挑选路线，而是正式接受整套记录如今所指向的结论；但本轮仍不会立刻结束，你还需要在世界里把它最后一步真正做完。"),
                      onlyEndingTitle);
        std::snprintf(singleEndingNote,
                      sizeof(singleEndingNote),
                      Loc_PickLiteral("Final route under review: %s", "当前进入最终复核的路线：%s"),
                      onlyEndingTitle);
        bodyText = singleEndingBody;
        noteText = singleEndingNote;
        buttonLabels[0] = onlyEnding != ENDING_NONE ? onlyEndingTitle : Loc_PickLiteral("Confirm", "确认");
        buttonLabels[1] = Loc_PickLiteral("Cancel", "取消");
    } else {
        titleText = Loc_PickLiteral("Choose Final Ending", "选择最终结局");
        bodyText = Loc_PickLiteral("Loxi has assembled enough evidence for more than one honest ending. Choose the answer you are willing to put your name on here, then return to the world and carry that decision through before the run truly ends.",
                                   "洛希已经把证据拼到足以支撑不止一种诚实结局。请在这里选出你愿意亲自承担的回答，然后回到世界里把这个决定真正执行到底，本轮才会结束。");
        noteText = Loc_PickLiteral("Only routes supported by recovered logs, archived investigations, and any required world-state conditions are shown here.",
                                   "这里出现的每条路线，都已经被你亲自回收的日志、完成归档的调查线以及对应世界状态共同支撑。");
        for (buttonIndex = 0; buttonIndex < availableEndingCount && buttonIndex < SETTLEMENT_CONFIRM_BUTTON_COUNT - 1; buttonIndex++) {
            buttonLabels[buttonIndex] = Tasks_GetEndingTitle(Tasks_GetAvailableEndingAt(tasks, buttonIndex));
        }
        buttonLabels[availableEndingCount] = Loc_PickLiteral("Cancel", "取消");
    }

    for (buttonIndex = 0; buttonIndex < buttonCount && buttonIndex < SETTLEMENT_CONFIRM_BUTTON_COUNT; buttonIndex++) {
        if (buttonLabels[buttonIndex] == NULL) {
            buttonLabels[buttonIndex] = buttonIndex == buttonCount - 1
                ? Loc_PickLiteral("Cancel", "取消")
                : Loc_PickLiteral("Unavailable", "不可用");
        }
    }

    DrawRectangle(0, 0, screenWidth, screenHeight, Color{6, 10, 18, 200});
    UIRuntime_DrawPanel(panel, Color{16, 22, 30, 245}, Color{255, 214, 154, 80});
    UIRuntime_DrawText(assets, titleText, Vector2{panel.x + 34.0f * scale, panel.y + 28.0f * scale}, 32.0f * scale, WHITE);
    UIRuntime_DrawText(assets, Loc_PickLiteral("ESC cancels", "按 ESC 取消"), Vector2{panel.x + panel.width - UIRuntime_MeasureText(assets, Loc_PickLiteral("ESC cancels", "按 ESC 取消"), 16.0f * scale).x - 30.0f * scale, panel.y + 34.0f * scale}, 16.0f * scale, Color{182, 199, 214, 255});
    UIRuntime_DrawWrappedText(assets, bodyText, bodyRect, 18.0f * scale, 22.0f * scale, Color{226, 235, 244, 255});
    UIRuntime_DrawWrappedText(assets, noteText, noteRect, 16.0f * scale, 19.0f * scale, Color{255, 214, 154, 255});

    for (buttonIndex = 0; buttonIndex < buttonCount && buttonIndex < SETTLEMENT_CONFIRM_BUTTON_COUNT; buttonIndex++) {
        Rectangle buttonRect;

        buttonRect = UI_GetSettlementConfirmButtonRect(screenWidth, screenHeight, buttonIndex, buttonCount);
        UIRuntime_DrawButton(assets, buttonRect, buttonLabels[buttonIndex], true);
        if (buttonIndex == selectedButton) {
            DrawRectangleRoundedLinesEx(buttonRect, 0.16f, 8, 2.0f, Color{255, 214, 154, 220});
        }
    }
}

void UI_DrawEndingRouteConfirmPopup(const AssetBundle *assets,
                                    GameEnding pendingEnding,
                                    int screenWidth,
                                    int screenHeight,
                                    int selectedButton) {
    float scale;
    Rectangle panel;
    Rectangle bodyRect;
    const char *titleText;
    char bodyTextBuffer[320];
    const char *endingTitle;
    const char *buttonLabels[2];
    int buttonIndex;

    scale = UIRuntime_GetScale(screenWidth, screenHeight);
    panel = UI_GetEndingRouteConfirmPanelRect(screenWidth, screenHeight);
    bodyRect = Rectangle{panel.x + 34.0f * scale, panel.y + 68.0f * scale, panel.width - 68.0f * scale, 64.0f * scale};
    endingTitle = Tasks_GetEndingTitle(pendingEnding);
    titleText = Loc_PickLiteral("Confirm Your Choice", "确认你的选择");
    std::snprintf(bodyTextBuffer,
                  sizeof(bodyTextBuffer),
                  Loc_PickLiteral("Are you sure you want to commit to %s? Once locked in, winding paths collapse into a single ending, and you cannot change it for the remainder of this run.",
                                  "你确定要锁定 %s 路线吗？一旦确认，所有支路将收敛为唯一的结局走向，本轮剩余时间里不可再更改。"),
                  endingTitle);
    buttonLabels[0] = Loc_PickLiteral("Lock In", "确定锁定");
    buttonLabels[1] = Loc_PickLiteral("Cancel", "取消");

    DrawRectangle(0, 0, screenWidth, screenHeight, Color{6, 10, 18, 200});
    UIRuntime_DrawPanel(panel, Color{16, 22, 30, 245}, Color{255, 214, 154, 80});
    UIRuntime_DrawText(assets, titleText, Vector2{panel.x + 34.0f * scale, panel.y + 28.0f * scale}, 28.0f * scale, WHITE);
    UIRuntime_DrawText(assets, Loc_PickLiteral("ESC cancels", "按 ESC 取消"), Vector2{panel.x + panel.width - UIRuntime_MeasureText(assets, Loc_PickLiteral("ESC cancels", "按 ESC 取消"), 16.0f * scale).x - 30.0f * scale, panel.y + 32.0f * scale}, 16.0f * scale, Color{182, 199, 214, 255});
    UIRuntime_DrawWrappedText(assets, bodyTextBuffer, bodyRect, 17.0f * scale, 20.0f * scale, Color{226, 235, 244, 255});

    for (buttonIndex = 0; buttonIndex < 2; buttonIndex++) {
        Rectangle buttonRect;

        buttonRect = UI_GetEndingRouteConfirmButtonRect(screenWidth, screenHeight, buttonIndex, 2);
        UIRuntime_DrawButton(assets, buttonRect, buttonLabels[buttonIndex], true);
        if (buttonIndex == selectedButton) {
            DrawRectangleRoundedLinesEx(buttonRect, 0.16f, 8, 2.0f, Color{255, 214, 154, 220});
        }
    }
}

void UI_DrawAccountDeleteConfirmPopup(const AssetBundle *assets,
                                      const char *accountName,
                                      int screenWidth,
                                      int screenHeight,
                                      int selectedButton) {
    float scale;
    Rectangle panel;
    Rectangle bodyRect;
    Rectangle noteRect;
    char titleBuffer[96];
    const char *buttonLabels[ACCOUNT_DELETE_CONFIRM_BUTTON_COUNT] = {
        Loc_PickLiteral("Keep Account", "保留账号"),
        Loc_PickLiteral("Delete Account", "删除账号")
    };
    int buttonIndex;

    scale = UIRuntime_GetScale(screenWidth, screenHeight);
    panel = UI_GetAccountDeleteConfirmPanelRect(screenWidth, screenHeight);
    bodyRect = Rectangle{panel.x + 34.0f * scale, panel.y + 98.0f * scale, panel.width - 68.0f * scale, 66.0f * scale};
    noteRect = Rectangle{panel.x + 34.0f * scale, panel.y + 172.0f * scale, panel.width - 68.0f * scale, 56.0f * scale};

    DrawRectangle(0, 0, screenWidth, screenHeight, Color{8, 8, 12, 214});
    UIRuntime_DrawPanel(panel, Color{30, 10, 10, 246}, Color{255, 108, 108, 92});
    std::snprintf(titleBuffer, sizeof(titleBuffer), "%s %s?", Loc_PickLiteral("Delete", "删除"), (accountName != NULL && accountName[0] != '\0') ? accountName : Loc_PickLiteral("this account", "此账号"));
    UIRuntime_DrawText(assets, titleBuffer, Vector2{panel.x + 34.0f * scale, panel.y + 28.0f * scale}, 32.0f * scale, WHITE);
    UIRuntime_DrawText(assets, Loc_PickLiteral("ESC cancels", "按 ESC 取消"), Vector2{panel.x + panel.width - UIRuntime_MeasureText(assets, Loc_PickLiteral("ESC cancels", "按 ESC 取消"), 16.0f * scale).x - 30.0f * scale, panel.y + 34.0f * scale}, 16.0f * scale, Color{205, 188, 188, 255});
    UIRuntime_DrawWrappedText(assets,
                              Loc_PickLiteral("Delete this account and every save tied to it.", "删除这个账号及其全部存档。"),
                              bodyRect,
                              18.0f * scale,
                              22.0f * scale,
                              Color{236, 224, 224, 255});
    UIRuntime_DrawWrappedText(assets,
                              Loc_PickLiteral("This action cannot be undone.", "该操作无法撤销。"),
                              noteRect,
                              16.0f * scale,
                              19.0f * scale,
                              Color{255, 214, 154, 255});

    for (buttonIndex = 0; buttonIndex < ACCOUNT_DELETE_CONFIRM_BUTTON_COUNT; ++buttonIndex) {
        Rectangle buttonRect;

        buttonRect = UI_GetAccountDeleteConfirmButtonRect(screenWidth, screenHeight, buttonIndex);
        UIRuntime_DrawButton(assets, buttonRect, buttonLabels[buttonIndex], true);
        if (buttonIndex == selectedButton) {
            DrawRectangleRoundedLinesEx(buttonRect, 0.16f, 8, 2.0f, Color{255, 214, 154, 220});
        }
    }
}
