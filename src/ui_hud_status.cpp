#include "ui_runtime_internal.h"

#include <cstdio>

static Color GetStatusPrimaryColor(PlayerStatusType status) {
    switch (status) {
        case PLAYER_STATUS_POISONED:
            return Color{128, 214, 122, 255};
        case PLAYER_STATUS_OXYGEN_LEAK:
            return Color{120, 210, 255, 255};
        case PLAYER_STATUS_LOW_OXYGEN:
            return Color{255, 196, 104, 255};
        case PLAYER_STATUS_SUFFOCATING:
            return Color{255, 112, 112, 255};
        case PLAYER_STATUS_CRITICAL_CONDITION:
            return Color{255, 144, 104, 255};
        case PLAYER_STATUS_FILTERED:
            return Color{132, 239, 214, 255};
        case PLAYER_STATUS_OXYGEN_RESERVE:
            return Color{132, 198, 255, 255};
        case PLAYER_STATUS_CAMP_RECOVERY:
            return Color{255, 214, 154, 255};
        case PLAYER_STATUS_COUNT:
        default:
            return Color{188, 202, 222, 255};
    }
}

static Color GetStatusSecondaryColor(PlayerStatusType status) {
    switch (status) {
        case PLAYER_STATUS_POISONED:
            return Color{42, 72, 36, 255};
        case PLAYER_STATUS_OXYGEN_LEAK:
            return Color{32, 56, 86, 255};
        case PLAYER_STATUS_LOW_OXYGEN:
            return Color{72, 56, 22, 255};
        case PLAYER_STATUS_SUFFOCATING:
            return Color{74, 30, 34, 255};
        case PLAYER_STATUS_CRITICAL_CONDITION:
            return Color{86, 42, 24, 255};
        case PLAYER_STATUS_FILTERED:
            return Color{26, 62, 60, 255};
        case PLAYER_STATUS_OXYGEN_RESERVE:
            return Color{26, 46, 76, 255};
        case PLAYER_STATUS_CAMP_RECOVERY:
            return Color{70, 52, 26, 255};
        case PLAYER_STATUS_COUNT:
        default:
            return Color{36, 46, 60, 255};
    }
}

static const char *GetStatusIconLabel(PlayerStatusType status) {
    switch (status) {
        case PLAYER_STATUS_POISONED:
            return "PX";
        case PLAYER_STATUS_OXYGEN_LEAK:
            return "LK";
        case PLAYER_STATUS_LOW_OXYGEN:
            return "LO";
        case PLAYER_STATUS_SUFFOCATING:
            return "SU";
        case PLAYER_STATUS_CRITICAL_CONDITION:
            return "CR";
        case PLAYER_STATUS_FILTERED:
            return "FL";
        case PLAYER_STATUS_OXYGEN_RESERVE:
            return "O+";
        case PLAYER_STATUS_CAMP_RECOVERY:
            return "CP";
        case PLAYER_STATUS_COUNT:
        default:
            return "--";
    }
}

static void DrawStatusChip(const AssetBundle *assets,
                           Rectangle rect,
                           PlayerStatusType status,
                           int level,
                           bool hovered) {
    Color primary;
    Color secondary;
    char levelBuffer[16];

    primary = GetStatusPrimaryColor(status);
    secondary = GetStatusSecondaryColor(status);

    UIRuntime_DrawPanel(rect,
                        hovered ? Color{18, 30, 46, 248} : Color{12, 22, 34, 226},
                        hovered ? primary : Color{primary.r, primary.g, primary.b, 95});
    DrawRectangleRounded(Rectangle{rect.x + 6.0f, rect.y + 6.0f, rect.width - 12.0f, rect.height - 12.0f},
                         0.26f,
                         8,
                         secondary);
    DrawCircleV(Vector2{rect.x + rect.width * 0.5f, rect.y + rect.height * 0.5f - 1.0f},
                rect.width * 0.18f,
                Color{primary.r, primary.g, primary.b, (unsigned char)(hovered ? 240 : 210)});
    UIRuntime_DrawText(assets,
                       GetStatusIconLabel(status),
                       Vector2{rect.x + 7.0f, rect.y + 7.0f},
                       11.5f,
                       WHITE);
    if (level > 1) {
        std::snprintf(levelBuffer, sizeof(levelBuffer), "%d", level);
        UIRuntime_DrawText(assets,
                           levelBuffer,
                           Vector2{rect.x + rect.width - UIRuntime_MeasureText(assets, levelBuffer, 10.0f).x - 6.0f, rect.y + rect.height - 16.0f},
                           10.0f,
                           primary);
    }
}

static void DrawStatusOverflowChip(const AssetBundle *assets, Rectangle rect, int hiddenCount) {
    char buffer[16];

    UIRuntime_DrawPanel(rect, Color{12, 22, 34, 226}, Color{180, 194, 214, 90});
    DrawRectangleRounded(Rectangle{rect.x + 6.0f, rect.y + 6.0f, rect.width - 12.0f, rect.height - 12.0f},
                         0.26f,
                         8,
                         Color{34, 42, 56, 255});
    std::snprintf(buffer, sizeof(buffer), "+%d", hiddenCount);
    UIRuntime_DrawText(assets,
                       buffer,
                       Vector2{rect.x + rect.width * 0.5f - UIRuntime_MeasureText(assets, buffer, 12.0f).x * 0.5f, rect.y + rect.height * 0.5f - 8.0f},
                       12.0f,
                       Color{228, 235, 244, 255});
}

bool UIRuntime_DrawHudStatusBar(const AssetBundle *assets,
                                const Player *player,
                                Rectangle rect,
                                int screenWidth,
                                int screenHeight,
                                PlayerStatusType *hoveredStatus) {
    const int maxVisibleIcons = 6;
    PlayerStatusType activeStatuses[PLAYER_STATUS_COUNT];
    Vector2 mouse;
    float padding;
    float slotSize;
    float gap;
    int activeCount;
    int iconCount;
    int drawCount;
    int index;

    if (hoveredStatus != NULL) {
        *hoveredStatus = PLAYER_STATUS_COUNT;
    }

    UIRuntime_DrawPanel(rect, Color{8, 18, 30, 210}, Color{112, 168, 210, 65});
    UIRuntime_DrawText(assets, "Status Bar", Vector2{rect.x + 12.0f, rect.y + 7.0f}, 14.0f, WHITE);

    activeCount = Player_CollectActiveStatuses(player, activeStatuses, PLAYER_STATUS_COUNT);
    if (activeCount <= 0) {
        UIRuntime_DrawText(assets,
                           "Stable",
                           Vector2{rect.x + 14.0f, rect.y + rect.height - 20.0f},
                           14.0f,
                           Color{170, 198, 220, 255});
        return false;
    }

    mouse = GetMousePosition();
    padding = 12.0f;
    gap = 6.0f;
    slotSize = rect.height - 22.0f;
    if (slotSize > 34.0f) {
        slotSize = 34.0f;
    }

    iconCount = activeCount > maxVisibleIcons ? maxVisibleIcons - 1 : activeCount;
    drawCount = activeCount > maxVisibleIcons ? maxVisibleIcons : activeCount;
    for (index = 0; index < drawCount; index++) {
        Rectangle slotRect;

        slotRect = Rectangle{
            rect.x + padding + index * (slotSize + gap),
            rect.y + rect.height - slotSize - 8.0f,
            slotSize,
            slotSize
        };

        if (activeCount > maxVisibleIcons && index == drawCount - 1) {
            DrawStatusOverflowChip(assets, slotRect, activeCount - iconCount);
            continue;
        }

        DrawStatusChip(assets,
                       slotRect,
                       activeStatuses[index],
                       player->statusEffects[activeStatuses[index]].level,
                       CheckCollisionPointRec(mouse, slotRect));
        if (hoveredStatus != NULL && CheckCollisionPointRec(mouse, slotRect)) {
            *hoveredStatus = activeStatuses[index];
        }
    }

    if (activeCount > maxVisibleIcons) {
        char buffer[32];

        std::snprintf(buffer, sizeof(buffer), "%d active", activeCount);
        UIRuntime_DrawText(assets,
                           buffer,
                           Vector2{rect.x + rect.width - UIRuntime_MeasureText(assets, buffer, 12.0f).x - 12.0f, rect.y + 8.0f},
                           12.0f,
                           Color{170, 198, 220, 255});
    }

    (void)screenWidth;
    (void)screenHeight;
    return hoveredStatus != NULL && *hoveredStatus != PLAYER_STATUS_COUNT;
}

void UIRuntime_DrawHudStatusTooltip(const AssetBundle *assets,
                                    const Player *player,
                                    PlayerStatusType status,
                                    Rectangle anchorRect,
                                    int screenWidth,
                                    int screenHeight) {
    char tooltip[640];
    Rectangle panel;
    Rectangle textRect;
    float scale;
    float tooltipWidth;
    float lineHeight;
    int lineCount;
    float tooltipHeight;

    if (status == PLAYER_STATUS_COUNT) {
        return;
    }

    Player_GetStatusTooltip(player, status, tooltip, (int)sizeof(tooltip));
    scale = UIRuntime_GetScale(screenWidth, screenHeight);
    tooltipWidth = 292.0f * scale;
    lineHeight = 16.0f * scale;
    lineCount = UIRuntime_CountWrappedTextLines(assets, tooltip, 13.5f * scale, tooltipWidth - 28.0f * scale, 8);
    if (lineCount < 1) {
        lineCount = 1;
    }
    tooltipHeight = 22.0f * scale + lineCount * lineHeight + 18.0f * scale;
    panel = Rectangle{
        anchorRect.x,
        anchorRect.y - tooltipHeight - 10.0f * scale,
        tooltipWidth,
        tooltipHeight
    };

    if (panel.x + panel.width > screenWidth - 18.0f * scale) {
        panel.x = screenWidth - 18.0f * scale - panel.width;
    }
    if (panel.x < 18.0f * scale) {
        panel.x = 18.0f * scale;
    }
    if (panel.y < 18.0f * scale) {
        panel.y = anchorRect.y + anchorRect.height + 10.0f * scale;
    }

    textRect = Rectangle{panel.x + 14.0f * scale, panel.y + 12.0f * scale, panel.width - 28.0f * scale, panel.height - 18.0f * scale};
    UIRuntime_DrawPanel(panel, Color{9, 16, 26, 245}, Color{255, 214, 154, 88});
    UIRuntime_DrawWrappedText(assets, tooltip, textRect, 13.5f * scale, lineHeight, Color{235, 241, 247, 255});
}
