#include "ui_system.h"

#include "localization.h"
#include "ui_runtime_internal.h"

#include <cmath>
#include <cstdio>
#include <cstring>

static void DrawPixelTitleText(const AssetBundle *assets, const char *text, Vector2 center, float fontSize, float scale, Color fill) {
    Font font;
    float spacing;
    float outlineOffset;
    float shadowOffset;
    Vector2 textSize;
    Vector2 position;
    bool useDefaultFont;

    useDefaultFont = assets == NULL || !assets->uiFontLoaded;
    font = useDefaultFont ? GetFontDefault() : assets->uiFont;
    if (font.texture.id == 0) {
        return;
    }

    if (useDefaultFont) {
        SetTextureFilter(font.texture, TEXTURE_FILTER_POINT);
    }

    spacing = useDefaultFont ? std::floor(fontSize * 0.12f) : fontSize * 0.05f;
    outlineOffset = std::round(3.0f * scale);
    shadowOffset = std::round(7.0f * scale);
    textSize = MeasureTextEx(font, text, fontSize, spacing);
    position = Vector2{
        std::round(center.x - textSize.x * 0.5f),
        std::round(center.y - textSize.y * 0.5f)
    };

    DrawTextEx(font, text, Vector2{position.x + shadowOffset, position.y + shadowOffset}, fontSize, spacing, Color{3, 7, 12, 180});
    DrawTextEx(font, text, Vector2{position.x - outlineOffset, position.y}, fontSize, spacing, Color{8, 18, 30, 255});
    DrawTextEx(font, text, Vector2{position.x + outlineOffset, position.y}, fontSize, spacing, Color{8, 18, 30, 255});
    DrawTextEx(font, text, Vector2{position.x, position.y - outlineOffset}, fontSize, spacing, Color{8, 18, 30, 255});
    DrawTextEx(font, text, Vector2{position.x, position.y + outlineOffset}, fontSize, spacing, Color{8, 18, 30, 255});
    DrawTextEx(font, text, position, fontSize, spacing, fill);
    DrawTextEx(font, text, Vector2{position.x, position.y - std::round(2.0f * scale)}, fontSize, spacing, Color{255, 255, 255, 96});
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

static void DrawSettingsSlider(const AssetBundle *assets, const GameSettings *settings, int screenWidth, int screenHeight) {
    float scale;
    Rectangle sliderRect;
    Rectangle handleRect;
    float filledWidth;
    char volumeBuffer[64];

    scale = UIRuntime_GetScale(screenWidth, screenHeight);
    sliderRect = UI_GetSettingsSliderRect(screenWidth, screenHeight);
    filledWidth = sliderRect.width * settings->masterVolume;
    handleRect = Rectangle{
        sliderRect.x + filledWidth - 14.0f * scale,
        sliderRect.y - 8.0f * scale,
        28.0f * scale,
        sliderRect.height + 16.0f * scale
    };

    UIRuntime_DrawPanel(sliderRect, Color{18, 30, 46, 235}, Color{104, 196, 222, 70});
    if (filledWidth > 6.0f * scale) {
        DrawRectangleRounded(Rectangle{sliderRect.x + 3.0f * scale, sliderRect.y + 3.0f * scale, filledWidth - 6.0f * scale, sliderRect.height - 6.0f * scale}, 0.35f, 8, Color{94, 217, 199, 255});
    }
    UIRuntime_DrawPanel(handleRect, Color{255, 214, 154, 245}, Color{255, 255, 255, 90});

    std::snprintf(volumeBuffer, sizeof(volumeBuffer), "%d%%", (int)std::round(settings->masterVolume * 100.0f));
    UIRuntime_DrawText(
        assets,
        volumeBuffer,
        Vector2{
            sliderRect.x + sliderRect.width - UIRuntime_MeasureText(assets, volumeBuffer, 18.0f * scale).x,
            sliderRect.y - 36.0f * scale
        },
        18.0f * scale,
        Color{255, 214, 154, 255}
    );
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
    title = mode == AUTH_SCREEN_MODE_REGISTER
        ? Loc_PickLiteral("Create Local Account", "创建本地账号")
        : Loc_PickLiteral("Account Login", "账号登录");
    body = mode == AUTH_SCREEN_MODE_REGISTER
        ? Loc_PickLiteral("Create a local account to continue.", "创建本地账号以继续。")
        : Loc_PickLiteral("Sign in with your local account.", "使用本地账号登录。");
    submitLabel = mode == AUTH_SCREEN_MODE_REGISTER ? Loc_PickLiteral("Register and Enter", "注册并进入") : Loc_PickLiteral("Login", "登录");
    switchLabel = mode == AUTH_SCREEN_MODE_REGISTER ? Loc_PickLiteral("Use Existing Account", "使用已有账号") : Loc_PickLiteral("Create New Account", "创建新账号");
    accountsHint = hasAccounts ? Loc_PickLiteral("Local accounts found on this device.", "此设备上已存在本地账号。") : Loc_PickLiteral("No local account yet. Register one to continue.", "当前还没有本地账号，请先注册。");
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
    UIRuntime_DrawWrappedText(
        assets,
        Loc_PickLiteral("TAB switch field. ENTER confirm.", "按 TAB 切换输入项，按 ENTER 确认。"),
        Rectangle{panel.x + 52.0f * scale, panel.y + panel.height - 176.0f * scale, panel.width - 104.0f * scale, 38.0f * scale},
        14.0f * scale,
        16.0f * scale,
        Color{181, 198, 216, 255}
    );
}

void UI_DrawMainMenu(const AssetBundle *assets,
                     bool hasSave,
                     int saveCount,
                     const char *accountName,
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
    Rectangle switchAccountRect;
    Rectangle deleteAccountRect;
    char accountBuffer[96];
    char saveCountBuffer[64];

    scale = UIRuntime_GetScale(screenWidth, screenHeight);
    switchAccountRect = UI_GetMainMenuSwitchAccountRect(screenWidth, screenHeight);
    deleteAccountRect = UI_GetMainMenuDeleteAccountRect(screenWidth, screenHeight);

    UIRuntime_DrawBackdrop(screenWidth, screenHeight, elapsedSeconds);
    DrawCircleGradient((int)(screenWidth * 0.5f - 210.0f * scale), (int)(184.0f * scale), 156.0f * scale, Color{65, 164, 214, 56}, Color{65, 164, 214, 0});
    DrawCircleGradient((int)(screenWidth * 0.5f + 210.0f * scale), (int)(188.0f * scale), 156.0f * scale, Color{110, 227, 196, 44}, Color{110, 227, 196, 0});
    DrawPixelTitleText(assets, Loc_PickLiteral("SPACECRAFT LIVING", "飞船求生"), Vector2{screenWidth * 0.5f, 214.0f * scale}, 96.0f * scale, scale, Color{232, 248, 255, 255});

    std::snprintf(accountBuffer, sizeof(accountBuffer), "%s: %s", Loc_PickLiteral("Account", "账号"), (accountName != NULL && accountName[0] != '\0') ? accountName : Loc_PickLiteral("Unknown", "未知"));
    std::snprintf(saveCountBuffer, sizeof(saveCountBuffer), "%s: %d", Loc_PickLiteral("Visible saves", "可见存档"), saveCount);
    UIRuntime_DrawText(assets, accountBuffer, Vector2{screenWidth * 0.5f - 154.0f * scale, 126.0f * scale}, 18.0f * scale, Color{166, 255, 226, 255});
    UIRuntime_DrawText(assets, saveCountBuffer, Vector2{screenWidth * 0.5f - 154.0f * scale, 151.0f * scale}, 15.0f * scale, Color{194, 224, 255, 255});
    UIRuntime_DrawButton(assets, switchAccountRect, Loc_PickLiteral("Switch Account", "切换账号"), true);
    UIRuntime_DrawButton(assets, deleteAccountRect, Loc_PickLiteral("Delete Account", "删除账号"), true);

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

void UI_DrawSettingsOverlay(const AssetBundle *assets, const GameSettings *settings, int screenWidth, int screenHeight) {
    float scale;
    Rectangle panel;
    Rectangle volumeCard;
    Rectangle sliderRect;
    Rectangle decreaseRect;
    Rectangle increaseRect;
    Rectangle closeRect;
    Rectangle languageEnglishRect;
    Rectangle languageChineseRect;
    Rectangle previewPanel;
    Rectangle previewBars;
    Rectangle notesPanel;
    float cardGap;
    float innerPadding;
    float sectionWidth;

    scale = UIRuntime_GetScale(screenWidth, screenHeight);
    panel = UI_GetStandardOverlayRect(screenWidth, screenHeight);
    volumeCard = Rectangle{panel.x + 34.0f * scale, panel.y + 118.0f * scale, panel.width - 68.0f * scale, 212.0f * scale};
    sliderRect = UI_GetSettingsSliderRect(screenWidth, screenHeight);
    decreaseRect = UI_GetSettingsDecreaseButtonRect(screenWidth, screenHeight);
    increaseRect = UI_GetSettingsIncreaseButtonRect(screenWidth, screenHeight);
    closeRect = UI_GetSettingsCloseButtonRect(screenWidth, screenHeight);
    languageEnglishRect = UI_GetSettingsLanguageButtonRect(screenWidth, screenHeight, 0);
    languageChineseRect = UI_GetSettingsLanguageButtonRect(screenWidth, screenHeight, 1);
    cardGap = 18.0f * scale;
    innerPadding = 22.0f * scale;
    sectionWidth = (panel.width - 68.0f * scale - cardGap) * 0.5f;
    previewPanel = Rectangle{panel.x + 34.0f * scale, panel.y + 356.0f * scale, sectionWidth, 152.0f * scale};
    notesPanel = Rectangle{previewPanel.x + previewPanel.width + cardGap, previewPanel.y, sectionWidth, previewPanel.height};
    previewBars = Rectangle{previewPanel.x + innerPadding, previewPanel.y + 52.0f * scale, previewPanel.width - innerPadding * 2.0f, 60.0f * scale};

    DrawRectangle(0, 0, screenWidth, screenHeight, Color{5, 9, 16, 208});
    UIRuntime_DrawPanel(panel, Color{8, 18, 30, 245}, Color{153, 226, 255, 75});
    UIRuntime_DrawText(assets, LOC_UI_SETTINGS_TITLE, Vector2{panel.x + 34.0f * scale, panel.y + 28.0f * scale}, 34.0f * scale, WHITE);
    UIRuntime_DrawButton(assets, closeRect, LOC_UI_CLOSE, true);
    UIRuntime_DrawText(
        assets,
        Loc_PickLiteral("Volume applies immediately.", "音量会立即生效。"),
        Vector2{panel.x + 34.0f * scale, panel.y + 72.0f * scale},
        17.0f * scale,
        Color{194, 224, 255, 255}
    );
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

    UIRuntime_DrawPanel(volumeCard, Color{12, 24, 39, 225}, Color{104, 196, 222, 50});
    UIRuntime_DrawText(assets, Loc_PickLiteral("Master Volume", "主音量"), Vector2{volumeCard.x + 20.0f * scale, volumeCard.y + 22.0f * scale}, 28.0f * scale, WHITE);
    UIRuntime_DrawWrappedText(
        assets,
        Loc_PickLiteral("Use slider or +/- buttons.", "可使用滑杆或 +/- 按钮调节。"),
        Rectangle{volumeCard.x + 20.0f * scale, volumeCard.y + 62.0f * scale, volumeCard.width - 40.0f * scale, 40.0f * scale},
        15.0f * scale,
        17.0f * scale,
        Color{190, 207, 222, 255}
    );

    DrawSettingsSlider(assets, settings, screenWidth, screenHeight);
    UIRuntime_DrawButton(assets, decreaseRect, "-", true);
    UIRuntime_DrawButton(assets, increaseRect, "+", true);
    UIRuntime_DrawWrappedText(assets, Loc_PickLiteral("Arrow keys / A-D: 5% step.", "方向键 / A-D：每次调整 5%。"), Rectangle{sliderRect.x, sliderRect.y + 96.0f * scale, sliderRect.width, 18.0f * scale}, 14.0f * scale, 15.0f * scale, Color{166, 255, 226, 255});
    UIRuntime_DrawText(assets, "0%", Vector2{sliderRect.x, sliderRect.y - 34.0f * scale}, 14.0f * scale, Color{168, 180, 196, 255});
    UIRuntime_DrawText(assets, "100%", Vector2{sliderRect.x + sliderRect.width - UIRuntime_MeasureText(assets, "100%", 14.0f * scale).x, sliderRect.y - 34.0f * scale}, 14.0f * scale, Color{168, 180, 196, 255});

    UIRuntime_DrawPanel(previewPanel, Color{12, 24, 39, 225}, Color{255, 255, 255, 22});
    UIRuntime_DrawPanel(notesPanel, Color{12, 24, 39, 225}, Color{255, 255, 255, 22});
    UIRuntime_DrawText(assets, Loc_PickLiteral("Live Meter", "实时电平"), Vector2{previewPanel.x + innerPadding, previewPanel.y + 18.0f * scale}, 22.0f * scale, WHITE);
    DrawVolumePreviewBars(previewBars, settings->masterVolume);
    UIRuntime_DrawText(assets, settings->masterVolume > 0.65f ? Loc_PickLiteral("High output", "高输出") : (settings->masterVolume > 0.30f ? Loc_PickLiteral("Balanced", "均衡") : Loc_PickLiteral("Quiet mode", "安静模式")), Vector2{previewPanel.x + innerPadding, previewPanel.y + previewPanel.height - 34.0f * scale}, 15.0f * scale, Color{194, 224, 255, 255});

    UIRuntime_DrawText(assets, LOC_UI_LANGUAGE, Vector2{notesPanel.x + innerPadding, notesPanel.y + 18.0f * scale}, 22.0f * scale, WHITE);
    UIRuntime_DrawWrappedText(assets, Loc_PickLiteral("English stays the source text. Update both options in one place when editing copy.", "英文仍然是源文本。后续改文案时，请在同一处同时更新两种语言。"), Rectangle{notesPanel.x + innerPadding, notesPanel.y + 54.0f * scale, notesPanel.width - innerPadding * 2.0f, 58.0f * scale}, 15.0f * scale, 16.0f * scale, Color{194, 224, 255, 255});
    UIRuntime_DrawButton(assets, languageEnglishRect, Loc_GetLanguageNativeName(GAME_LANGUAGE_EN), true);
    UIRuntime_DrawButton(assets, languageChineseRect, "简体中文", true);
    if (Loc_NormalizeLanguage((int)settings->language) == GAME_LANGUAGE_EN) {
        DrawRectangleRoundedLinesEx(languageEnglishRect, 0.16f, 8, 2.0f, Color{255, 214, 154, 220});
    } else {
        DrawRectangleRoundedLinesEx(languageChineseRect, 0.16f, 8, 2.0f, Color{255, 214, 154, 220});
    }
    UIRuntime_DrawWrappedText(assets, Loc_PickLiteral("Saved automatically on close. Press TAB here to switch language quickly.", "关闭时会自动保存。也可以按 TAB 快速切换语言。"), Rectangle{notesPanel.x + innerPadding, notesPanel.y + 118.0f * scale, notesPanel.width - innerPadding * 2.0f, 44.0f * scale}, 14.0f * scale, 15.0f * scale, Color{166, 255, 226, 255});
}

void UI_DrawDeathPopup(const Player *player, const AssetBundle *assets, int screenWidth, int screenHeight, int selectedButton) {
    const int deathFailureThreshold = 3;
    float scale;
    Rectangle panel;
    Rectangle bodyRect;
    Rectangle hintRect;
    char deathBuffer[128];
    char hintBuffer[160];
    const char *buttonLabels[DEATH_POPUP_BUTTON_COUNT] = {
        Loc_PickLiteral("Restart", "重新开始"),
        Loc_PickLiteral("Exit", "退出")
    };
    int buttonIndex;

    scale = UIRuntime_GetScale(screenWidth, screenHeight);
    panel = Rectangle{screenWidth * 0.5f - 350.0f * scale, screenHeight * 0.5f - 170.0f * scale, 700.0f * scale, 340.0f * scale};
    bodyRect = Rectangle{panel.x + 54.0f * scale, panel.y + 132.0f * scale, panel.width - 108.0f * scale, 28.0f * scale};
    hintRect = Rectangle{panel.x + 54.0f * scale, panel.y + 166.0f * scale, panel.width - 108.0f * scale, 44.0f * scale};

    DrawRectangle(0, 0, screenWidth, screenHeight, Color{10, 5, 5, 220});

    UIRuntime_DrawPanel(panel, Color{30, 10, 10, 245}, Color{255, 80, 80, 90});

    UIRuntime_DrawText(assets, Loc_PickLiteral("YOU DIED", "你已死亡"), Vector2{panel.x + panel.width * 0.5f - UIRuntime_MeasureText(assets, Loc_PickLiteral("YOU DIED", "你已死亡"), 38.0f * scale).x * 0.5f, panel.y + 35.0f * scale}, 38.0f * scale, Color{255, 80, 80, 255});

    std::snprintf(deathBuffer, sizeof(deathBuffer), "%s %d / %d", Loc_PickLiteral("Deaths", "死亡次数"), player->deathCount, deathFailureThreshold);
    UIRuntime_DrawText(assets, deathBuffer, Vector2{panel.x + panel.width * 0.5f - UIRuntime_MeasureText(assets, deathBuffer, 22.0f * scale).x * 0.5f, panel.y + 95.0f * scale}, 22.0f * scale, Color{200, 200, 200, 255});

    if (player->deathCount >= deathFailureThreshold - 1) {
        std::snprintf(hintBuffer, sizeof(hintBuffer), "%s", Loc_PickLiteral("Restart at recovery point. One more death ends the run.", "将在恢复点重来。再死一次，本轮就会结束。"));
    } else {
        std::snprintf(hintBuffer, sizeof(hintBuffer), "%s", Loc_PickLiteral("Restart at recovery point. Repeated deaths end the run.", "将在恢复点重来。反复死亡会导致本轮结束。"));
    }

    UIRuntime_DrawWrappedText(assets, Loc_PickLiteral("Suit vitals collapsed.", "防护服生命体征系统已经崩溃。"), bodyRect, 19.0f * scale, 22.0f * scale, Color{180, 180, 180, 255});
    UIRuntime_DrawWrappedText(assets, hintBuffer, hintRect, 16.0f * scale, 18.0f * scale, Color{196, 184, 184, 255});

    DrawLine(panel.x + 40.0f * scale, panel.y + 212.0f * scale, panel.x + panel.width - 40.0f * scale, panel.y + 212.0f * scale, Color{255, 100, 100, 120});

    for (buttonIndex = 0; buttonIndex < DEATH_POPUP_BUTTON_COUNT; buttonIndex++) {
        Rectangle buttonRect;

        buttonRect = UI_GetDeathPopupButtonRect(screenWidth, screenHeight, buttonIndex);
        UIRuntime_DrawButton(assets, buttonRect, buttonLabels[buttonIndex], true);
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
                                   int selectedButton) {
    float scale;
    Rectangle panel;
    Rectangle bodyRect;
    Rectangle noteRect;
    const char *titleText;
    const char *bodyText;
    const char *noteText;
    const char *buttonLabels[SETTLEMENT_CONFIRM_BUTTON_COUNT] = {
        Loc_PickLiteral("Heroic", "强行救援"),
        Loc_PickLiteral("Peaceful", "和平救援"),
        Loc_PickLiteral("Settlement", "异星定居"),
        Loc_PickLiteral("Cancel", "取消")
    };
    int buttonIndex;

    scale = UIRuntime_GetScale(screenWidth, screenHeight);
    panel = UI_GetSettlementConfirmPanelRect(screenWidth, screenHeight);
    bodyRect = Rectangle{panel.x + 34.0f * scale, panel.y + 96.0f * scale, panel.width - 68.0f * scale, 62.0f * scale};
    noteRect = Rectangle{panel.x + 34.0f * scale, panel.y + 166.0f * scale, panel.width - 68.0f * scale, 50.0f * scale};
    titleText = Loc_PickLiteral("Confirm Final Route", "确认最终路线");
    bodyText = Loc_PickLiteral("Loxi has finished the final archive review. Commit to the route you want before the tower or airlock turns that choice into a point of no return.",
                               "洛希已经完成最终档案复核。请先在这里确认你要走的路线，再去触发塔楼或气闸里的不可回头步骤。");
    if (tasks != NULL && tasks->bossDefeated) {
        noteText = Loc_PickLiteral("Heroic can finish immediately at the Signal Tower. Peaceful still requires the Signal Amplifier. Settlement still ends the run here at the ship.",
                                   "强行救援现在可以直接去信号塔完成。和平救援仍需要信号放大器。异星定居仍会在飞船处结束本轮。");
    } else if (tasks != NULL && tasks->selectedEndingRoute == ENDING_HEROIC) {
        noteText = Loc_PickLiteral("Heroic is already locked toward the guardian arena. Peaceful requires the Signal Amplifier. Settlement still closes both rescue routes.",
                                   "强行救援已经锁定为守卫战场路线。和平救援仍需要信号放大器。异星定居仍会关闭两条救援路线。");
    } else if (player != NULL && player->hasSignalAmplifier) {
        noteText = Loc_PickLiteral("Heroic requires the guardian arena. Peaceful can now finish at the tower with the Signal Amplifier. Settlement still closes both rescue routes.",
                                   "强行救援需要进入守卫战场。和平救援现在可以带着信号放大器去塔楼完成。异星定居仍会关闭两条救援路线。");
    } else {
        noteText = Loc_PickLiteral("Heroic requires the guardian arena. Peaceful requires the Signal Amplifier. Settlement still closes both rescue routes and ends the run at the ship.",
                                   "强行救援需要进入守卫战场。和平救援需要信号放大器。异星定居仍会关闭两条救援路线，并在飞船处结束本轮。");
    }

    DrawRectangle(0, 0, screenWidth, screenHeight, Color{6, 10, 18, 200});
    UIRuntime_DrawPanel(panel, Color{16, 22, 30, 245}, Color{255, 214, 154, 80});
    UIRuntime_DrawText(assets, titleText, Vector2{panel.x + 34.0f * scale, panel.y + 28.0f * scale}, 32.0f * scale, WHITE);
    UIRuntime_DrawText(assets, Loc_PickLiteral("ESC cancels", "按 ESC 取消"), Vector2{panel.x + panel.width - UIRuntime_MeasureText(assets, Loc_PickLiteral("ESC cancels", "按 ESC 取消"), 16.0f * scale).x - 30.0f * scale, panel.y + 34.0f * scale}, 16.0f * scale, Color{182, 199, 214, 255});
    UIRuntime_DrawWrappedText(assets, bodyText, bodyRect, 18.0f * scale, 22.0f * scale, Color{226, 235, 244, 255});
    UIRuntime_DrawWrappedText(assets, noteText, noteRect, 16.0f * scale, 19.0f * scale, Color{255, 214, 154, 255});

    for (buttonIndex = 0; buttonIndex < SETTLEMENT_CONFIRM_BUTTON_COUNT; buttonIndex++) {
        Rectangle buttonRect;

        buttonRect = UI_GetSettlementConfirmButtonRect(screenWidth, screenHeight, buttonIndex);
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
                              Loc_PickLiteral("Delete this account and all saves.", "删除这个账号及其全部存档。"),
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
