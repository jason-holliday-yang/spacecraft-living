#include "ui_system.h"

#include "localization.h"
#include "ui_runtime_internal.h"

void UI_DrawCommunicatorOverlay(const AssetBundle *assets, const TaskSystem *tasks, int screenWidth, int screenHeight) {
    float scale;
    Rectangle panel;
    Rectangle taskPanel;
    Rectangle objectiveRect;

    scale = UIRuntime_GetScale(screenWidth, screenHeight);
    panel = UI_GetStandardOverlayRect(screenWidth, screenHeight);
    taskPanel = Rectangle{panel.x + 32.0f * scale, panel.y + 110.0f * scale, panel.width - 64.0f * scale, panel.height - 142.0f * scale};
    objectiveRect = Rectangle{taskPanel.x + 22.0f * scale, taskPanel.y + 110.0f * scale, taskPanel.width - 44.0f * scale, taskPanel.height - 132.0f * scale};

    DrawRectangle(0, 0, screenWidth, screenHeight, Color{5, 9, 16, 190});
    UIRuntime_DrawPanel(panel, Color{8, 18, 30, 245}, Color{99, 233, 195, 75});
    UIRuntime_DrawText(assets, "Loxi Interface", Vector2{panel.x + 30.0f * scale, panel.y + 24.0f * scale}, 34.0f * scale, WHITE);
    UIRuntime_DrawText(assets, "Press N or ESC to close.", Vector2{panel.x + panel.width - UIRuntime_MeasureText(assets, "Press N or ESC to close.", 17.5f * scale).x - 24.0f * scale, panel.y + 28.0f * scale}, 17.5f * scale, Color{182, 199, 214, 255});
    UIRuntime_DrawWrappedText(assets, "Current task from Loxi", Rectangle{panel.x + 30.0f * scale, panel.y + 66.0f * scale, panel.width - 60.0f * scale, 22.0f * scale}, 18.0f * scale, 18.0f * scale, Color{194, 224, 255, 255});
    UIRuntime_DrawPanel(taskPanel, Color{11, 20, 32, 230}, Color{104, 196, 222, 50});
    UIRuntime_DrawText(assets, "Mission Feed", Vector2{taskPanel.x + 22.0f * scale, taskPanel.y + 22.0f * scale}, 24.0f * scale, Color{255, 214, 154, 255});
    UIRuntime_DrawWrappedText(assets, Tasks_GetCommunicatorHint(tasks), objectiveRect, 17.0f * scale, 21.0f * scale, Color{227, 237, 245, 255});
}

void UI_DrawHelpOverlay(const AssetBundle *assets, int screenWidth, int screenHeight) {
    float scale;
    Rectangle panel;
    const char *lines[] = {
        "WASD / Arrow Keys: original grid movement",
        "F: interact, gather, repair, read logs, craft at workbench",
        "Space: attack, uses laser line if available",
        "B: open the backpack and inspect supplies",
        "N: open the Loxi interface",
        "M: open the area map overlay",
        "H: open this help panel",
        "Z: use food for quick health and oxygen recovery",
        "X: use antidote and filter items for poison or leak relief",
        "C: toggle crouch stealth",
        "ESC: pause",
        "Base and camp restore health, oxygen, and stabilize bad conditions"
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
        UIRuntime_DrawWrappedText(assets, lines[lineIndex], Rectangle{x + 16.0f * scale, y + 10.0f * scale, 402.0f * scale, 34.0f * scale}, 16.5f * scale, 18.0f * scale, Color{229, 238, 246, 255});
    }
}
