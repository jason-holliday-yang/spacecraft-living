#include "ui_system.h"

#include "localization.h"
#include "ui_runtime_internal.h"
#include "ui_components_internal.h"

#include <algorithm>
#include <cmath>
#include <cstdio>
#include <cstring>

static void DrawArchiveSeal(float centerX, float centerY, float radius, float scale, float elapsedSeconds) {
    const UITheme *theme = UITheme_Get();
    int rayIndex;
    float pulse;

    pulse = 0.5f + 0.5f * std::sin(elapsedSeconds * 1.2f);
    DrawCircleGradient((int)centerX,
                       (int)centerY,
                       radius * 1.42f,
                       Color{58, 134, 200, (unsigned char)(48 + pulse * 24.0f)},
                       Color{7, 18, 32, 0});
    DrawCircle((int)centerX, (int)centerY, radius, Color{8, 27, 44, 244});
    DrawCircleLines((int)centerX, (int)centerY, radius, Color{107, 203, 228, 205});
    DrawCircleLines((int)centerX, (int)centerY, radius * 0.80f, Color{58, 134, 200, 175});
    DrawCircleLines((int)centerX, (int)centerY, radius * 0.48f, Color{210, 170, 109, 105});

    for (rayIndex = 0; rayIndex < 12; ++rayIndex) {
        float angle;
        Vector2 a;
        Vector2 b;
        angle = (float)rayIndex * 3.14159265f / 6.0f + elapsedSeconds * 0.025f;
        a = Vector2{centerX + std::cos(angle) * radius * 0.83f, centerY + std::sin(angle) * radius * 0.83f};
        b = Vector2{centerX + std::cos(angle) * radius * 1.02f, centerY + std::sin(angle) * radius * 1.02f};
        DrawLineEx(a, b, 1.5f * scale, Color{107, 203, 228, 120});
    }

    DrawTriangle(Vector2{centerX, centerY - radius * 0.55f},
                 Vector2{centerX - radius * 0.34f, centerY + radius * 0.42f},
                 Vector2{centerX + radius * 0.30f, centerY + radius * 0.42f},
                 Color{13, 49, 72, 255});
    DrawLineEx(Vector2{centerX, centerY - radius * 0.55f},
               Vector2{centerX - radius * 0.34f, centerY + radius * 0.42f},
               2.0f * scale,
               Color{107, 203, 228, 210});
    DrawLineEx(Vector2{centerX, centerY - radius * 0.55f},
               Vector2{centerX + radius * 0.30f, centerY + radius * 0.42f},
               2.0f * scale,
               Color{58, 134, 200, 205});
    DrawCircle((int)centerX, (int)(centerY + radius * 0.02f), radius * 0.10f, theme->iceWhite);
    DrawCircleLines((int)centerX, (int)(centerY + radius * 0.02f), radius * (0.14f + pulse * 0.025f), Color{107, 203, 228, 130});
}

static void DrawAuthInputField(const AssetBundle *assets,
                               const char *label,
                               const char *value,
                               Rectangle rect,
                               bool selected,
                               bool maskValue,
                               bool selectionOnly,
                               float scale) {
    const UITheme *theme = UITheme_Get();
    char displayValue[SAVE_ACCOUNT_PASSWORD_MAX];
    size_t index;
    bool hovered;
    Color outline;
    Color fillTop;
    Rectangle labelPlate;

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

    hovered = CheckCollisionPointRec(GetMousePosition(), rect);
    outline = selected ? theme->signalCyan : (hovered ? Color{92, 169, 215, 220} : theme->borderSoft);
    fillTop = selected ? Color{18, 53, 77, 250} : theme->surfaceRaised;

    DrawRectangleRounded(Rectangle{rect.x + 6.0f * scale, rect.y + 7.0f * scale, rect.width, rect.height}, 0.10f, 8, Color{1, 5, 10, 150});
    DrawRectangleRounded(rect, 0.10f, 8, theme->surface);
    DrawRectangleGradientV((int)(rect.x + 3.0f * scale),
                           (int)(rect.y + 3.0f * scale),
                           (int)(rect.width - 6.0f * scale),
                           (int)(rect.height - 6.0f * scale),
                           fillTop,
                           Color{7, 21, 34, 250});
    DrawRectangleRoundedLinesEx(rect, 0.10f, 8, selected ? 2.0f * scale : 1.25f * scale, outline);
    DrawLineEx(Vector2{rect.x + 14.0f * scale, rect.y + rect.height - 7.0f * scale},
               Vector2{rect.x + rect.width - 14.0f * scale, rect.y + rect.height - 7.0f * scale},
               1.0f * scale,
               selected ? Color{107, 203, 228, 135} : Color{58, 134, 200, 42});

    labelPlate = Rectangle{rect.x + 16.0f * scale, rect.y - 9.0f * scale, 116.0f * scale, 23.0f * scale};
    DrawRectangleRounded(labelPlate, 0.24f, 8, Color{13, 49, 72, 255});
    DrawRectangleRoundedLinesEx(labelPlate, 0.24f, 8, 1.0f, Color{58, 134, 200, 175});
    UIRuntime_DrawText(assets, label, Vector2{labelPlate.x + 10.0f * scale, labelPlate.y + 4.0f * scale}, 13.0f * scale, theme->signalCyan);

    UIComponents_DrawDiamond(Vector2{rect.x + 18.0f * scale, rect.y + rect.height * 0.60f},
                5.0f * scale,
                selected ? Color{24, 91, 127, 255} : Color{18, 56, 73, 255},
                outline);
    UIRuntime_DrawText(
        assets,
        displayValue[0] != '\0'
            ? displayValue
            : (maskValue
                ? Loc_PickLiteral("Enter password", "请输入密码")
                : (selectionOnly ? Loc_PickLiteral("Choose a local account", "请选择本地账号") : Loc_PickLiteral("Enter username", "请输入用户名"))),
        Vector2{rect.x + 34.0f * scale, rect.y + 27.0f * scale},
        21.0f * scale,
        displayValue[0] != '\0' ? theme->iceWhite : theme->textMuted
    );

    if (selectionOnly) {
        const char *chooseLabel = Loc_PickLiteral("CHOOSE", "选择");
        const Vector2 chooseSize = UIRuntime_MeasureText(assets, chooseLabel, 13.0f * scale);

        UIRuntime_DrawText(assets,
                           chooseLabel,
                           Vector2{rect.x + rect.width - chooseSize.x - 18.0f * scale, rect.y + 28.0f * scale},
                           13.0f * scale,
                           selected ? theme->signalCyan : theme->textMuted);
    }
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
    const UITheme *theme = UITheme_Get();
    float scale;
    Rectangle panel;
    Rectangle usernameRect;
    Rectangle passwordRect;
    Rectangle togglePasswordRect;
    Rectangle deleteRect;
    Rectangle submitRect;
    Rectangle switchRect;
    Rectangle exitRect;
    Rectangle loreRect;
    const char *title;
    const char *body;
    const char *submitLabel;
    const char *switchLabel;
    const char *accountsHint;
    const char *archiveLabel;
    bool deleteEnabled;
    Color messageColor;
    Vector2 titleSize;
    float dividerX;

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
        ? Loc_PickLiteral("CREATE FIELD IDENTITY", "建立远征身份")
        : Loc_PickLiteral("RETURN TO THE ARCHIVE", "返回洛希档案");
    body = mode == AUTH_SCREEN_MODE_REGISTER
        ? Loc_PickLiteral("Create a local identity. Your expedition records and endings remain on this device.", "创建一个本地身份。远征记录、存档与结局都会保存在这台设备上。")
        : Loc_PickLiteral("Unlock your local expedition archive and continue the signal reconstruction.", "解锁本地远征档案，继续追踪尚未完成的信号复原。\n");
    submitLabel = mode == AUTH_SCREEN_MODE_REGISTER ? Loc_PickLiteral("CREATE & DESCEND", "创建并进入") : Loc_PickLiteral("ENTER ARCHIVE", "进入档案");
    switchLabel = mode == AUTH_SCREEN_MODE_REGISTER ? Loc_PickLiteral("USE EXISTING ID", "使用已有身份") : Loc_PickLiteral("CREATE NEW ID", "建立新身份");
    accountsHint = hasAccounts
        ? (mode == AUTH_SCREEN_MODE_LOGIN
            ? Loc_PickLiteral("CLICK FIELD ID TO CHOOSE", "点击远征身份选择账号")
            : Loc_PickLiteral("LOCAL ARCHIVE DETECTED", "已检测到本地档案"))
        : Loc_PickLiteral("NO LOCAL ARCHIVE DETECTED", "尚未检测到本地档案");
    archiveLabel = Loc_PickLiteral("LOXI MEMORY VAULT", "洛希记忆库");
    deleteEnabled = mode == AUTH_SCREEN_MODE_LOGIN && hasAccounts;
    messageColor = theme->warning;
    if (message != NULL
        && (std::strstr(message, "Logged in") != NULL
            || std::strstr(message, "created") != NULL
            || std::strstr(message, "signed in") != NULL
            || std::strstr(message, "已登录") != NULL
            || std::strstr(message, "已创建并登录") != NULL)) {
        messageColor = theme->positive;
    }

    UIRuntime_DrawBackdrop(screenWidth, screenHeight, elapsedSeconds);
    DrawRectangle(0, 0, screenWidth, screenHeight, Color{3, 11, 21, 66});
    DrawCircleGradient((int)(panel.x + 196.0f * scale),
                       (int)(panel.y + 252.0f * scale),
                       260.0f * scale,
                       Color{42, 116, 169, 92},
                       Color{7, 18, 32, 0});
    UIComponents_DrawFrame(panel, scale, UI_FRAME_STANDARD, true);

    dividerX = panel.x + 390.0f * scale;
    DrawRectangleGradientH((int)(panel.x + 15.0f * scale),
                           (int)(panel.y + 15.0f * scale),
                           (int)(365.0f * scale),
                           (int)(panel.height - 30.0f * scale),
                           Color{12, 53, 82, 145},
                           Color{10, 35, 53, 24});
    DrawLineEx(Vector2{dividerX, panel.y + 40.0f * scale},
               Vector2{dividerX, panel.y + panel.height - 40.0f * scale},
               1.6f * scale,
               Color{58, 134, 200, 145});
    DrawLineEx(Vector2{dividerX + 5.0f * scale, panel.y + 72.0f * scale},
               Vector2{dividerX + 5.0f * scale, panel.y + panel.height - 72.0f * scale},
               1.0f * scale,
               Color{107, 203, 228, 90});
    UIComponents_DrawDiamond(Vector2{dividerX + 2.5f * scale, panel.y + 52.0f * scale}, 8.0f * scale, Color{13, 49, 72, 255}, Color{107, 203, 228, 205});
    UIComponents_DrawDiamond(Vector2{dividerX + 2.5f * scale, panel.y + panel.height - 52.0f * scale}, 8.0f * scale, Color{16, 57, 76, 255}, Color{58, 134, 200, 175});

    DrawArchiveSeal(panel.x + 202.0f * scale,
                    panel.y + 220.0f * scale,
                    102.0f * scale,
                    scale,
                    elapsedSeconds);
    UIRuntime_DrawText(assets,
                       archiveLabel,
                       Vector2{panel.x + 72.0f * scale, panel.y + 344.0f * scale},
                       17.0f * scale,
                       theme->signalCyan);
    loreRect = Rectangle{panel.x + 54.0f * scale, panel.y + 380.0f * scale, 294.0f * scale, 92.0f * scale};
    UIRuntime_DrawWrappedText(assets,
                              Loc_PickLiteral("A sealed record of every landing, every failure, and every choice made beneath the silent moon.",
                                              "每一次降落、失败与抉择，都会被封存于寂静卫星之下。"),
                              loreRect,
                              16.0f * scale,
                              19.0f * scale,
                              theme->textSecondary);
    UIRuntime_DrawText(assets,
                       Loc_PickLiteral("LOCAL LINK  //  ENCRYPTED", "本地连接  //  已加密"),
                       Vector2{panel.x + 74.0f * scale, panel.y + panel.height - 62.0f * scale},
                       13.0f * scale,
                       theme->positive);

    titleSize = UIRuntime_MeasureText(assets, title, 31.0f * scale);
    UIRuntime_DrawText(assets,
                       title,
                       Vector2{panel.x + 430.0f * scale, panel.y + 42.0f * scale},
                       31.0f * scale,
                       theme->iceWhite);
    DrawLineEx(Vector2{panel.x + 430.0f * scale, panel.y + 84.0f * scale},
               Vector2{panel.x + 430.0f * scale + fminf(titleSize.x, 430.0f * scale), panel.y + 84.0f * scale},
               1.4f * scale,
               Color{58, 134, 200, 165});
    UIComponents_DrawFrontEndButton(assets,
                       exitRect,
                       Loc_PickLiteral("EXIT", "退出"),
                       true,
                       selectedField == AUTH_FIELD_EXIT_GAME,
                       false,
                       scale);

    UIRuntime_DrawWrappedText(assets,
                              body,
                              Rectangle{panel.x + 430.0f * scale, panel.y + 100.0f * scale, panel.width - 480.0f * scale, 54.0f * scale},
                              16.0f * scale,
                              20.0f * scale,
                              theme->textSecondary);
    DrawRectangleRounded(Rectangle{panel.x + 430.0f * scale, panel.y + 153.0f * scale, 280.0f * scale, 25.0f * scale},
                         0.28f,
                         8,
                         Color{13, 58, 80, 230});
    DrawRectangleRoundedLinesEx(Rectangle{panel.x + 430.0f * scale, panel.y + 153.0f * scale, 280.0f * scale, 25.0f * scale},
                                0.28f,
                                8,
                                1.0f,
                                Color{107, 203, 228, 125});
    UIRuntime_DrawText(assets,
                       accountsHint,
                       Vector2{panel.x + 443.0f * scale, panel.y + 158.0f * scale},
                       12.5f * scale,
                       theme->signalCyan);

    DrawAuthInputField(assets, Loc_PickLiteral("FIELD ID", "远征身份"), username, usernameRect, selectedField == AUTH_FIELD_USERNAME, false, mode == AUTH_SCREEN_MODE_LOGIN && hasAccounts, scale);
    DrawAuthInputField(assets, Loc_PickLiteral("ACCESS KEY", "访问密钥"), password, passwordRect, selectedField == AUTH_FIELD_PASSWORD, !passwordVisible, false, scale);
    UIComponents_DrawFrontEndButton(assets,
                       togglePasswordRect,
                       passwordVisible ? Loc_PickLiteral("HIDE", "隐藏") : Loc_PickLiteral("SHOW", "显示"),
                       true,
                       selectedField == AUTH_FIELD_TOGGLE_PASSWORD,
                       false,
                       scale);

    if (message != NULL && message[0] != '\0') {
        UIComponents_DrawDiamond(Vector2{panel.x + 438.0f * scale, panel.y + 390.0f * scale},
                    5.0f * scale,
                    messageColor,
                    Color{107, 203, 228, 145});
        UIRuntime_DrawWrappedText(
            assets,
            message,
            Rectangle{panel.x + 453.0f * scale, panel.y + 378.0f * scale, panel.width - 505.0f * scale, 54.0f * scale},
            14.0f * scale,
            17.0f * scale,
            messageColor
        );
    } else {
        UIRuntime_DrawText(assets,
                           Loc_PickLiteral("Credentials never leave this device.", "身份信息仅保存在当前设备。"),
                           Vector2{panel.x + 430.0f * scale, panel.y + 384.0f * scale},
                           14.0f * scale,
                           theme->textMuted);
    }

    UIComponents_DrawFrontEndButton(assets,
                       submitRect,
                       submitLabel,
                       true,
                       selectedField == AUTH_FIELD_SUBMIT,
                       true,
                       scale);
    UIComponents_DrawFrontEndButton(assets,
                       switchRect,
                       switchLabel,
                       true,
                       selectedField == AUTH_FIELD_SWITCH_MODE,
                       false,
                       scale);
    UIComponents_DrawActionButton(assets,
                                  deleteRect,
                                  Loc_PickLiteral("DELETE LOCAL ID", "删除本地身份"),
                                  deleteEnabled,
                                  deleteEnabled && selectedField == AUTH_FIELD_DELETE_ACCOUNT,
                                  UI_ACTION_DANGER,
                                  scale);
}

void UI_DrawAuthAccountPickerPopup(const AssetBundle *assets,
                                   const char accountNames[][SAVE_ACCOUNT_NAME_MAX],
                                   int accountCount,
                                   int selectedAccount,
                                   int firstVisibleAccount,
                                   int screenWidth,
                                   int screenHeight) {
    const UITheme *theme = UITheme_Get();
    const float scale = UIRuntime_GetScale(screenWidth, screenHeight);
    const Rectangle panel = UI_GetAuthAccountPickerPanelRect(screenWidth, screenHeight);
    const int visibleRowCount = UI_GetAuthAccountPickerVisibleRowCount();
    int visibleIndex;
    char countBuffer[64];

    DrawRectangle(0, 0, screenWidth, screenHeight, Color{3, 8, 15, 218});
    DrawCircleGradient((int)(panel.x + panel.width * 0.5f),
                       (int)(panel.y + panel.height * 0.45f),
                       panel.width * 0.62f,
                       Color{42, 116, 169, 64},
                       Color{7, 18, 32, 0});
    UIComponents_DrawFrame(panel, scale, UI_FRAME_STANDARD, true);

    UIComponents_DrawDiamond(Vector2{panel.x + 42.0f * scale, panel.y + 47.0f * scale},
                             11.0f * scale,
                             Color{13, 61, 83, 255},
                             theme->signalCyan);
    UIRuntime_DrawText(assets,
                       Loc_PickLiteral("CHOOSE LOCAL IDENTITY", "选择本地账号"),
                       Vector2{panel.x + 66.0f * scale, panel.y + 27.0f * scale},
                       29.0f * scale,
                       theme->iceWhite);
    UIRuntime_DrawText(assets,
                       Loc_PickLiteral("ENTER selects  //  ESC cancels", "回车选择  //  ESC 取消"),
                       Vector2{panel.x + 38.0f * scale, panel.y + 72.0f * scale},
                       14.0f * scale,
                       theme->textMuted);

    for (visibleIndex = 0; visibleIndex < visibleRowCount; ++visibleIndex) {
        const int accountIndex = firstVisibleAccount + visibleIndex;
        Rectangle rowRect;
        bool selected;
        bool hovered;
        Color outline;

        if (accountIndex < 0 || accountIndex >= accountCount) {
            break;
        }

        rowRect = UI_GetAuthAccountPickerRowRect(screenWidth, screenHeight, visibleIndex);
        selected = accountIndex == selectedAccount;
        hovered = CheckCollisionPointRec(GetMousePosition(), rowRect);
        outline = selected ? theme->signalCyan : (hovered ? theme->signalBlue : theme->borderSoft);

        DrawRectangleRounded(rowRect,
                             0.12f,
                             8,
                             selected ? Color{18, 58, 81, 250} : (hovered ? Color{14, 43, 62, 250} : theme->surfaceRaised));
        DrawRectangleRoundedLinesEx(rowRect, 0.12f, 8, selected ? 2.0f * scale : 1.0f * scale, outline);
        UIComponents_DrawDiamond(Vector2{rowRect.x + 21.0f * scale, rowRect.y + rowRect.height * 0.5f},
                                 5.0f * scale,
                                 selected ? Color{24, 91, 127, 255} : Color{18, 56, 73, 255},
                                 outline);
        UIRuntime_DrawText(assets,
                           accountNames[accountIndex],
                           Vector2{rowRect.x + 40.0f * scale, rowRect.y + 11.0f * scale},
                           19.0f * scale,
                           selected ? theme->iceWhite : theme->textSecondary);
        if (selected) {
            const char *selectedLabel = Loc_PickLiteral("SELECTED", "已选中");
            const Vector2 selectedSize = UIRuntime_MeasureText(assets, selectedLabel, 12.0f * scale);

            UIRuntime_DrawText(assets,
                               selectedLabel,
                               Vector2{rowRect.x + rowRect.width - selectedSize.x - 16.0f * scale, rowRect.y + 14.0f * scale},
                               12.0f * scale,
                               theme->signalCyan);
        }
    }

    std::snprintf(countBuffer,
                  sizeof(countBuffer),
                  Loc_PickLiteral("%d local identities", "共 %d 个本地账号"),
                  accountCount);
    UIRuntime_DrawText(assets,
                       countBuffer,
                       Vector2{panel.x + 38.0f * scale, panel.y + panel.height - 42.0f * scale},
                       14.0f * scale,
                       theme->textMuted);
    if (accountCount > visibleRowCount) {
        UIRuntime_DrawText(assets,
                           Loc_PickLiteral("Use arrows or mouse wheel to browse", "使用方向键或滚轮浏览"),
                           Vector2{panel.x + panel.width - 282.0f * scale, panel.y + panel.height - 42.0f * scale},
                           14.0f * scale,
                           theme->signalCyan);
    }
}
