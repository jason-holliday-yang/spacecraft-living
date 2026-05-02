#include "ui_system.h"

#include "localization.h"
#include "ui_runtime_internal.h"

#include <cmath>
#include <cstdio>

static float ClampFloatLocal(float value, float minValue, float maxValue) {
    if (value < minValue) {
        return minValue;
    }
    if (value > maxValue) {
        return maxValue;
    }
    return value;
}

static void DrawBar(const AssetBundle *assets, Rectangle rect, const char *label, float value, float maxValue, Color fill, Color bg) {
    float ratio;
    float localScale;
    float padding;
    float barY;
    float barHeight;
    float labelSize;
    float valueSize;
    char buffer[64];

    ratio = maxValue > 0.0f ? ClampFloatLocal(value / maxValue, 0.0f, 1.0f) : 0.0f;
    localScale = rect.height / 70.0f;
    padding = 14.0f * localScale;
    barY = rect.y + 39.0f * localScale;
    barHeight = 14.0f * localScale;
    labelSize = 19.0f * localScale;
    valueSize = 17.0f * localScale;
    UIRuntime_DrawPanel(rect, Color{8, 18, 30, 210}, Color{112, 168, 210, 65});
    UIRuntime_DrawText(assets, label, Vector2{rect.x + padding, rect.y + 8.0f * localScale}, labelSize, WHITE);
    DrawRectangle((int)(rect.x + padding), (int)barY, (int)(rect.width - padding * 2.0f), (int)barHeight, bg);
    DrawRectangle((int)(rect.x + padding), (int)barY, (int)((rect.width - padding * 2.0f) * ratio), (int)barHeight, fill);
    snprintf(buffer, sizeof(buffer), "%03d / %03d", (int)value, (int)maxValue);
    UIRuntime_DrawText(assets, buffer, Vector2{rect.x + rect.width - UIRuntime_MeasureText(assets, buffer, valueSize).x - padding, rect.y + 9.0f * localScale}, valueSize, Color{210, 225, 238, 255});
}

void UI_DrawHud(const Player *player, const TaskSystem *tasks, const HudMessage *message, const AssetBundle *assets, int screenWidth, int screenHeight) {
    float scale;
    float edgeInset;
    float bottomLift;
    float sidePanelWidth;
    float vitalsPanelHeight;
    Rectangle vitalsPanel;
    Rectangle statusBarRect;
    Rectangle healthBarRect;
    Rectangle oxygenBarRect;
    Rectangle messagePanel;
    Rectangle messageTextRect;
    char quickUseCount[16];
    float messageFontSize;
    float messageLineSpacing;
    int messageLines;
    float messageHeight;
    float messageWidth;
    float messageCenterY;
    PlayerStatusType hoveredStatus;
    bool statusHovered;
    int recoveryRationCount;
    Rectangle quickUseRect;
    Color quickUsePrimary;
    Color quickUseSecondary;

    scale = UIRuntime_GetScale(screenWidth, screenHeight);
    (void)tasks;
    edgeInset = 18.0f * scale;
    bottomLift = 22.0f * scale;
    sidePanelWidth = 316.0f * scale;
    vitalsPanelHeight = 214.0f * scale;
    vitalsPanel = Rectangle{edgeInset, screenHeight - edgeInset - vitalsPanelHeight - bottomLift, sidePanelWidth, vitalsPanelHeight};
    statusBarRect = Rectangle{vitalsPanel.x + 10.0f * scale, vitalsPanel.y + 38.0f * scale, vitalsPanel.width - 20.0f * scale, 54.0f * scale};
    healthBarRect = Rectangle{vitalsPanel.x + 10.0f * scale, vitalsPanel.y + 98.0f * scale, vitalsPanel.width - 20.0f * scale, 56.0f * scale};
    oxygenBarRect = Rectangle{vitalsPanel.x + 10.0f * scale, vitalsPanel.y + 158.0f * scale, vitalsPanel.width - 20.0f * scale, 46.0f * scale};
    messageFontSize = 25.0f * scale;
    messageLineSpacing = 30.0f * scale;
    messageWidth = screenWidth - sidePanelWidth * 2.0f - 140.0f * scale;
    if (messageWidth < 420.0f * scale) {
        messageWidth = 420.0f * scale;
    }
    messageLines = UIRuntime_CountWrappedTextLines(assets,
                                                   message->text,
                                                   messageFontSize,
                                                   messageWidth - 40.0f * scale,
                                                   4);
    if (messageLines < 1) {
        messageLines = 1;
    }
    messageHeight = 30.0f * scale + messageLines * messageLineSpacing + 18.0f * scale;
    messageCenterY = screenHeight * 0.79f;
    messagePanel = Rectangle{
        screenWidth * 0.5f - messageWidth * 0.5f,
        ClampFloatLocal(messageCenterY - messageHeight * 0.5f,
                        edgeInset + 12.0f * scale,
                        screenHeight - edgeInset - messageHeight - 18.0f * scale),
        messageWidth,
        messageHeight
    };
    messageTextRect = Rectangle{messagePanel.x + 20.0f * scale, messagePanel.y + 16.0f * scale, messagePanel.width - 40.0f * scale, messagePanel.height - 28.0f * scale};
    hoveredStatus = PLAYER_STATUS_COUNT;
    statusHovered = false;
    recoveryRationCount = player->resources[RESOURCE_RECOVERY_RATION];
    quickUseRect = UI_GetHudShortcutRect(screenWidth, screenHeight, 3);
    quickUsePrimary = recoveryRationCount > 0 ? Color{126, 212, 255, 255} : Color{119, 131, 145, 255};
    quickUseSecondary = recoveryRationCount > 0 ? Color{57, 112, 172, 255} : Color{68, 79, 92, 255};

    UIRuntime_DrawPanel(vitalsPanel, Color{8, 18, 30, 220}, Color{124, 166, 214, 65});
    UIRuntime_DrawText(assets, Loc_PickLiteral("Survival", "生存状态"), Vector2{vitalsPanel.x + 16.0f * scale, vitalsPanel.y + 12.0f * scale}, 24.0f * scale, WHITE);
    statusHovered = UIRuntime_DrawHudStatusBar(assets, player, statusBarRect, screenWidth, screenHeight, &hoveredStatus);
    DrawBar(assets, healthBarRect, LOC_UI_HEALTH, player->health, Player_GetMaxHealth(player), Color{239, 107, 98, 255}, Color{44, 24, 30, 255});
    DrawBar(assets, oxygenBarRect, LOC_UI_OXYGEN, player->oxygen, MAX_OXYGEN, Color{92, 218, 255, 255}, Color{20, 41, 52, 255});

    UIRuntime_DrawHudShortcut(assets,
                              UI_GetHudShortcutRect(screenWidth, screenHeight, 0),
                              "",
                              "",
                              &assets->iconMapButton,
                              0,
                              Color{110, 201, 255, 255},
                              Color{62, 118, 192, 255});
    UIRuntime_DrawHudShortcut(assets,
                              UI_GetHudShortcutRect(screenWidth, screenHeight, 1),
                              "",
                              "",
                              &assets->iconInventoryButton,
                              1,
                              Color{232, 180, 113, 255},
                              Color{127, 84, 44, 255});
    UIRuntime_DrawHudShortcut(assets,
                              UI_GetHudShortcutRect(screenWidth, screenHeight, 2),
                              "",
                              "",
                              nullptr,
                              2,
                              Color{118, 226, 255, 255},
                              Color{60, 120, 188, 255});
    UIRuntime_DrawHudShortcut(assets,
                              quickUseRect,
                              "",
                              "X",
                              &assets->iconRecoveryRation,
                              16,
                              quickUsePrimary,
                              quickUseSecondary);
    if (recoveryRationCount > 0) {
        Rectangle countBadge = Rectangle{
            quickUseRect.x + quickUseRect.width - 22.0f * scale,
            quickUseRect.y + quickUseRect.height - 18.0f * scale,
            18.0f * scale,
            14.0f * scale
        };

        std::snprintf(quickUseCount, sizeof(quickUseCount), "%d", recoveryRationCount);
        UIRuntime_DrawPanel(countBadge, Color{22, 39, 58, 245}, Color{166, 255, 226, 120});
        UIRuntime_DrawText(assets,
                           quickUseCount,
                           Vector2{countBadge.x + 4.0f * scale, countBadge.y + 1.0f * scale},
                           9.5f * scale,
                           WHITE);
    }

    if (message->timer > 0.0f && message->text[0] != '\0') {
        UIRuntime_DrawPanel(messagePanel, Color{11, 15, 26, 220}, Color{255, 192, 129, 80});
        UIRuntime_DrawWrappedText(assets, message->text, messageTextRect, messageFontSize, messageLineSpacing, Color{255, 236, 214, 255});
    }

    if (statusHovered) {
        UIRuntime_DrawHudStatusTooltip(assets, player, hoveredStatus, statusBarRect, screenWidth, screenHeight);
    }

    if (Player_HasStatus(player, PLAYER_STATUS_SUFFOCATING)
        || Player_HasStatus(player, PLAYER_STATUS_LOW_OXYGEN)
        || Player_HasStatus(player, PLAYER_STATUS_CRITICAL_CONDITION)) {
        Color pulseTint;

        pulseTint = Player_HasStatus(player, PLAYER_STATUS_SUFFOCATING)
            || Player_HasStatus(player, PLAYER_STATUS_LOW_OXYGEN)
            ? Color{126, 212, 255, (unsigned char)(14 + 16 * (std::sin(player->blurPulse * 6.0f) * 0.5f + 0.5f))}
            : Color{255, 182, 144, (unsigned char)(12 + 14 * (std::sin(player->blurPulse * 6.0f) * 0.5f + 0.5f))};
        DrawRectangle(0, 0, screenWidth, screenHeight, pulseTint);
    }
}
