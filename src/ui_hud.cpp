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
    float sidePanelWidth;
    float topPanelHeight;
    float shortcutsPanelHeight;
    float vitalsPanelHeight;
    Rectangle vitalsPanel;
    Rectangle statusBarRect;
    Rectangle healthBarRect;
    Rectangle oxygenBarRect;
    Rectangle objectivePanel;
    Rectangle statusPanel;
    Rectangle shortcutsPanel;
    Rectangle messagePanel;
    Rectangle messageTextRect;
    char smallBuffer[128];
    char cycleBuffer[128];
    char reserveBuffer[128];
    float messageFontSize;
    float messageLineSpacing;
    int messageLines;
    float messageHeight;
    float messageWidth;
    float messageCenterY;
    bool showLoxiGuidance;
    PlayerStatusType hoveredStatus;
    bool statusHovered;

    scale = UIRuntime_GetScale(screenWidth, screenHeight);
    edgeInset = 18.0f * scale;
    sidePanelWidth = 316.0f * scale;
    topPanelHeight = 128.0f * scale;
    shortcutsPanelHeight = 106.0f * scale;
    vitalsPanelHeight = 214.0f * scale;
    vitalsPanel = Rectangle{edgeInset, screenHeight - edgeInset - vitalsPanelHeight, sidePanelWidth, vitalsPanelHeight};
    statusBarRect = Rectangle{vitalsPanel.x + 10.0f * scale, vitalsPanel.y + 38.0f * scale, vitalsPanel.width - 20.0f * scale, 54.0f * scale};
    healthBarRect = Rectangle{vitalsPanel.x + 10.0f * scale, vitalsPanel.y + 98.0f * scale, vitalsPanel.width - 20.0f * scale, 56.0f * scale};
    oxygenBarRect = Rectangle{vitalsPanel.x + 10.0f * scale, vitalsPanel.y + 158.0f * scale, vitalsPanel.width - 20.0f * scale, 46.0f * scale};
    objectivePanel = Rectangle{edgeInset, edgeInset, sidePanelWidth, topPanelHeight};
    statusPanel = Rectangle{screenWidth - edgeInset - sidePanelWidth, edgeInset, sidePanelWidth, topPanelHeight};
    shortcutsPanel = Rectangle{screenWidth - edgeInset - sidePanelWidth, screenHeight - edgeInset - shortcutsPanelHeight, sidePanelWidth, shortcutsPanelHeight};
    showLoxiGuidance = Tasks_IsCommunicatorUnlocked(tasks);
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

    UIRuntime_DrawPanel(vitalsPanel, Color{8, 18, 30, 220}, Color{124, 166, 214, 65});
    UIRuntime_DrawText(assets, Loc_PickLiteral("Survival", "生存状态"), Vector2{vitalsPanel.x + 16.0f * scale, vitalsPanel.y + 12.0f * scale}, 24.0f * scale, WHITE);
    statusHovered = UIRuntime_DrawHudStatusBar(assets, player, statusBarRect, screenWidth, screenHeight, &hoveredStatus);
    DrawBar(assets, healthBarRect, LOC_UI_HEALTH, player->health, Player_GetMaxHealth(player), Color{239, 107, 98, 255}, Color{44, 24, 30, 255});
    DrawBar(assets, oxygenBarRect, LOC_UI_OXYGEN, player->oxygen, MAX_OXYGEN, Color{92, 218, 255, 255}, Color{20, 41, 52, 255});

    UIRuntime_DrawPanel(statusPanel, Color{8, 18, 30, 214}, Color{99, 221, 194, 75});
    UIRuntime_DrawText(assets, Loc_PickLiteral("Mission Status", "任务状态"), Vector2{statusPanel.x + 18.0f * scale, statusPanel.y + 14.0f * scale}, 25.0f * scale, WHITE);
    snprintf(smallBuffer, sizeof(smallBuffer), "%s: %s", LOC_UI_STAGE, Tasks_GetStageName(tasks->stage));
    UIRuntime_DrawWrappedText(assets, smallBuffer, Rectangle{statusPanel.x + 18.0f * scale, statusPanel.y + 48.0f * scale, statusPanel.width - 36.0f * scale, 18.0f * scale}, 16.0f * scale, 17.0f * scale, Color{166, 255, 226, 255});
    snprintf(cycleBuffer, sizeof(cycleBuffer), "%s: %s %d - %s", LOC_UI_DAY, Loc_PickLiteral("Day", "第"), tasks->dayCount + 1, Tasks_GetPhaseName(tasks->phase));
    UIRuntime_DrawWrappedText(assets, cycleBuffer, Rectangle{statusPanel.x + 18.0f * scale, statusPanel.y + 68.0f * scale, statusPanel.width - 36.0f * scale, 18.0f * scale}, 14.5f * scale, 16.0f * scale, Color{194, 224, 255, 255});
    snprintf(reserveBuffer, sizeof(reserveBuffer), "%s: %d", Loc_PickLiteral("Active Effects", "当前效果"), Player_GetActiveStatusCount(player));
    UIRuntime_DrawWrappedText(assets, reserveBuffer, Rectangle{statusPanel.x + 18.0f * scale, statusPanel.y + 88.0f * scale, statusPanel.width - 36.0f * scale, 18.0f * scale}, 14.5f * scale, 16.0f * scale, Color{255, 214, 154, 255});
    if (showLoxiGuidance) {
        UIRuntime_DrawPanel(objectivePanel, Color{8, 18, 30, 214}, Color{99, 221, 194, 75});
        UIRuntime_DrawText(assets, Loc_PickLiteral("Loxi Guidance", "洛希指引"), Vector2{objectivePanel.x + 18.0f * scale, objectivePanel.y + 14.0f * scale}, 25.0f * scale, WHITE);
        UIRuntime_DrawText(assets, LOC_UI_OBJECTIVE, Vector2{objectivePanel.x + 18.0f * scale, objectivePanel.y + 48.0f * scale}, 15.0f * scale, Color{255, 214, 154, 255});
        UIRuntime_DrawWrappedText(assets,
                                  tasks->objective,
                                  Rectangle{objectivePanel.x + 18.0f * scale, objectivePanel.y + 66.0f * scale, objectivePanel.width - 36.0f * scale, objectivePanel.height - 80.0f * scale},
                                  14.8f * scale,
                                  18.0f * scale,
                                  Color{214, 226, 238, 255});
    }
    UIRuntime_DrawPanel(shortcutsPanel, Color{8, 18, 30, 220}, Color{255, 255, 255, 36});
    UIRuntime_DrawText(assets, Loc_PickLiteral("Quick Access", "快捷功能"), Vector2{shortcutsPanel.x + 12.0f * scale, shortcutsPanel.y - 22.0f * scale}, 15.0f * scale, Color{182, 199, 214, 255});
    UIRuntime_DrawHudShortcut(assets, UI_GetHudShortcutRect(screenWidth, screenHeight, 0), Loc_PickLiteral("Map", "地图"), Loc_PickLiteral("Scout", "侦察"), "M", 0, Color{110, 201, 255, 255}, Color{62, 118, 192, 255});
    UIRuntime_DrawHudShortcut(assets, UI_GetHudShortcutRect(screenWidth, screenHeight, 1), Loc_PickLiteral("Bag", "背包"), Loc_PickLiteral("Supplies", "补给"), "B", 1, Color{232, 180, 113, 255}, Color{127, 84, 44, 255});

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
