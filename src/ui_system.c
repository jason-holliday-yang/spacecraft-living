#include "ui_system.h"

#include "localization.h"
#include <math.h>
#include <stdio.h>

static float ClampFloat(float value, float minValue, float maxValue) {
    if (value < minValue) {
        return minValue;
    }
    if (value > maxValue) {
        return maxValue;
    }
    return value;
}

static Font GetUIFont(const AssetBundle *assets) {
    return assets->uiFontLoaded ? assets->uiFont : GetFontDefault();
}

static float GetUIScale(int screenWidth, int screenHeight) {
    float widthScale;
    float heightScale;

    widthScale = (float)screenWidth / 1440.0f;
    heightScale = (float)screenHeight / 900.0f;
    return ClampFloat(fminf(widthScale, heightScale), 0.84f, 1.25f);
}

static void DrawUIText(const AssetBundle *assets, const char *text, Vector2 position, float fontSize, Color tint) {
    Font font;
    float spacing;

    font = GetUIFont(assets);
    spacing = fontSize * 0.05f;
    DrawTextEx(font, text, position, fontSize, spacing, tint);
}

static Vector2 MeasureUIText(const AssetBundle *assets, const char *text, float fontSize) {
    Font font;
    float spacing;

    font = GetUIFont(assets);
    spacing = fontSize * 0.05f;
    return MeasureTextEx(font, text, fontSize, spacing);
}

static void DrawPanel(Rectangle rect, Color fill, Color outline) {
    DrawRectangleRounded(rect, 0.16f, 8, fill);
    DrawRectangleRoundedLinesEx(rect, 0.16f, 8, 1.2f, outline);
}

static void DrawBackdrop(int screenWidth, int screenHeight, float elapsedSeconds) {
    int index;

    DrawRectangleGradientV(0, 0, screenWidth, screenHeight, (Color){5, 10, 18, 255}, (Color){11, 23, 37, 255});
    DrawRectangleGradientH(0, screenHeight / 2, screenWidth, screenHeight / 2, (Color){8, 40, 53, 0}, (Color){38, 74, 102, 50});

    for (index = 0; index < 80; index++) {
        float seed;
        float x;
        float y;
        float radius;

        seed = (float)index * 21.0f;
        x = fmodf(seed * 29.0f, (float)screenWidth);
        y = fmodf(seed * 12.0f + elapsedSeconds * (4.0f + (float)(index % 5)), (float)screenHeight);
        radius = 1.0f + (float)(index % 3);
        DrawCircle((int)x, (int)y, radius, (Color){154, 202, 255, (unsigned char)(70 + (index % 4) * 25)});
    }
}

static void DrawButton(const AssetBundle *assets, Rectangle rect, const char *label, bool enabled) {
    bool hovered;
    Color fill;
    Color outline;
    Color textColor;
    Vector2 labelSize;

    hovered = enabled && CheckCollisionPointRec(GetMousePosition(), rect);
    fill = enabled
        ? (hovered ? (Color){36, 76, 114, 240} : (Color){18, 40, 66, 235})
        : (Color){28, 32, 44, 220};
    outline = enabled
        ? (hovered ? (Color){165, 230, 255, 150} : (Color){105, 168, 222, 80})
        : (Color){84, 96, 118, 50};
    textColor = enabled ? WHITE : (Color){138, 146, 165, 255};
    labelSize = MeasureUIText(assets, label, rect.height * 0.40f);
    DrawPanel(rect, fill, outline);
    DrawUIText(
        assets,
        label,
        (Vector2){rect.x + rect.width * 0.5f - labelSize.x * 0.5f, rect.y + rect.height * 0.5f - labelSize.y * 0.5f},
        rect.height * 0.40f,
        textColor
    );
}

Rectangle UI_GetMainMenuButtonRect(int screenWidth, int screenHeight, int buttonIndex) {
    float scale;
    float width;
    float height;
    float startY;

    scale = GetUIScale(screenWidth, screenHeight);
    width = 300.0f * scale;
    height = 58.0f * scale;
    startY = screenHeight * 0.54f;

    return (Rectangle){
        screenWidth * 0.5f - width * 0.5f,
        startY + buttonIndex * (height + 18.0f * scale),
        width,
        height
    };
}

Rectangle UI_GetPauseMenuButtonRect(int screenWidth, int screenHeight, int buttonIndex) {
    float scale;
    float width;
    float height;
    float startY;

    scale = GetUIScale(screenWidth, screenHeight);
    width = 280.0f * scale;
    height = 54.0f * scale;
    startY = screenHeight * 0.42f;

    return (Rectangle){
        screenWidth * 0.5f - width * 0.5f,
        startY + buttonIndex * (height + 16.0f * scale),
        width,
        height
    };
}

static void DrawBar(const AssetBundle *assets, Rectangle rect, const char *label, float value, float maxValue, Color fill, Color bg) {
    float ratio;
    char buffer[64];

    ratio = maxValue > 0.0f ? ClampFloat(value / maxValue, 0.0f, 1.0f) : 0.0f;
    DrawPanel(rect, (Color){8, 18, 30, 210}, (Color){112, 168, 210, 65});
    DrawUIText(assets, label, (Vector2){rect.x + 14.0f, rect.y + 9.0f}, 20.0f, WHITE);
    DrawRectangle((int)rect.x + 14, (int)rect.y + 41, (int)rect.width - 28, 16, bg);
    DrawRectangle((int)rect.x + 14, (int)rect.y + 41, (int)((rect.width - 28) * ratio), 16, fill);
    snprintf(buffer, sizeof(buffer), "%03d / %03d", (int)value, (int)maxValue);
    DrawUIText(assets, buffer, (Vector2){rect.x + rect.width - MeasureUIText(assets, buffer, 18.0f).x - 14.0f, rect.y + 10.0f}, 18.0f, (Color){210, 225, 238, 255});
}

void UI_DrawHud(const Player *player, const TaskSystem *tasks, const HudMessage *message, const AssetBundle *assets, int screenWidth, int screenHeight) {
    float scale;
    Rectangle leftPanel;
    Rectangle objectivePanel;
    Rectangle resourcePanel;
    Rectangle hintPanel;
    Rectangle messagePanel;
    char smallBuffer[128];
    char cycleBuffer[128];

    scale = GetUIScale(screenWidth, screenHeight);
    leftPanel = (Rectangle){18.0f * scale, 18.0f * scale, 300.0f * scale, 248.0f * scale};
    objectivePanel = (Rectangle){screenWidth - 330.0f * scale, 18.0f * scale, 312.0f * scale, 120.0f * scale};
    resourcePanel = (Rectangle){screenWidth - 392.0f * scale, 214.0f * scale, 374.0f * scale, 200.0f * scale};
    hintPanel = (Rectangle){18.0f * scale, screenHeight - 102.0f * scale, 620.0f * scale, 76.0f * scale};
    messagePanel = (Rectangle){18.0f * scale, screenHeight - 194.0f * scale, screenWidth - 36.0f * scale, 64.0f * scale};

    DrawBar(assets, (Rectangle){leftPanel.x, leftPanel.y, leftPanel.width, 70.0f * scale}, LOC_UI_STAMINA, player->stamina, Player_GetCurrentStaminaCap(player), (Color){239, 107, 98, 255}, (Color){44, 24, 30, 255});
    DrawBar(assets, (Rectangle){leftPanel.x, leftPanel.y + 82.0f * scale, leftPanel.width, 70.0f * scale}, LOC_UI_PRESSURE, player->pressure, MAX_PRESSURE, (Color){255, 190, 86, 255}, (Color){54, 42, 20, 255});
    DrawBar(assets, (Rectangle){leftPanel.x, leftPanel.y + 164.0f * scale, leftPanel.width, 70.0f * scale}, LOC_UI_OXYGEN, player->oxygen, MAX_OXYGEN, (Color){92, 218, 255, 255}, (Color){20, 41, 52, 255});

    DrawPanel(objectivePanel, (Color){8, 18, 30, 214}, (Color){99, 221, 194, 75});
    DrawUIText(assets, "Status", (Vector2){objectivePanel.x + 18.0f * scale, objectivePanel.y + 14.0f * scale}, 25.0f * scale, WHITE);
    snprintf(smallBuffer, sizeof(smallBuffer), "%s: %s", LOC_UI_STAGE, Tasks_GetStageName(tasks->stage));
    DrawUIText(assets, smallBuffer, (Vector2){objectivePanel.x + 18.0f * scale, objectivePanel.y + 50.0f * scale}, 18.5f * scale, (Color){166, 255, 226, 255});
    snprintf(cycleBuffer, sizeof(cycleBuffer), "Cycle: Day %d - %s", tasks->dayCount + 1, Tasks_GetPhaseName(tasks->phase));
    DrawUIText(assets, cycleBuffer, (Vector2){objectivePanel.x + 18.0f * scale, objectivePanel.y + 80.0f * scale}, 18.0f * scale, (Color){194, 224, 255, 255});

    DrawPanel(resourcePanel, (Color){8, 18, 30, 214}, (Color){124, 166, 214, 65});
    DrawUIText(assets, "Key Resources", (Vector2){resourcePanel.x + 18.0f * scale, resourcePanel.y + 14.0f * scale}, 24.0f * scale, WHITE);
    snprintf(smallBuffer, sizeof(smallBuffer), "Wood %d  Ore %d  Scrap %d", player->resources[RESOURCE_WOOD], player->resources[RESOURCE_ORE], player->resources[RESOURCE_METAL_SCRAP]);
    DrawUIText(assets, smallBuffer, (Vector2){resourcePanel.x + 18.0f * scale, resourcePanel.y + 56.0f * scale}, 17.0f * scale, (Color){221, 233, 244, 255});
    snprintf(smallBuffer, sizeof(smallBuffer), "Vines %d  Moss %d  Fiber %d", player->resources[RESOURCE_ALIEN_VINE], player->resources[RESOURCE_GLOW_MOSS], player->resources[RESOURCE_PROTECTIVE_FIBER]);
    DrawUIText(assets, smallBuffer, (Vector2){resourcePanel.x + 18.0f * scale, resourcePanel.y + 86.0f * scale}, 17.0f * scale, (Color){221, 233, 244, 255});
    snprintf(smallBuffer, sizeof(smallBuffer), "Core %d  Relics %d  Boss Scales %d", player->resources[RESOURCE_ENERGY_CORE], player->resources[RESOURCE_RELIC_FRAGMENT], player->resources[RESOURCE_BOSS_SCALE]);
    DrawUIText(assets, smallBuffer, (Vector2){resourcePanel.x + 18.0f * scale, resourcePanel.y + 116.0f * scale}, 17.0f * scale, (Color){221, 233, 244, 255});
    snprintf(smallBuffer, sizeof(smallBuffer), "Gear: Glow Stick %s  Rope %s  Laser %s  Suit %s", player->hasGlowStick ? "Yes" : "No", player->hasRope ? "Yes" : "No", player->hasLaserGun ? "Yes" : "No", player->hasProtectionSuit ? "Yes" : "No");
    DrawUIText(assets, smallBuffer, (Vector2){resourcePanel.x + 18.0f * scale, resourcePanel.y + 152.0f * scale}, 16.0f * scale, (Color){195, 212, 228, 255});

    DrawPanel(hintPanel, (Color){8, 18, 30, 220}, (Color){255, 255, 255, 40});
    DrawUIText(assets, "Move WASD/Arrows  Interact E  Attack Space  Backpack B  Craft Q  Loxi N  Help H", (Vector2){hintPanel.x + 18.0f * scale, hintPanel.y + 16.0f * scale}, 16.5f * scale, (Color){211, 223, 234, 255});
    DrawUIText(assets, "Original grid movement, camera follow, and tile collision are preserved.", (Vector2){hintPanel.x + 18.0f * scale, hintPanel.y + 43.0f * scale}, 15.0f * scale, (Color){147, 193, 220, 255});

    if (message->timer > 0.0f && message->text[0] != '\0') {
        DrawPanel(messagePanel, (Color){11, 15, 26, 220}, (Color){255, 192, 129, 80});
        DrawUIText(assets, message->text, (Vector2){messagePanel.x + 18.0f * scale, messagePanel.y + 16.0f * scale}, 19.5f * scale, (Color){255, 236, 214, 255});
    }

    if (player->pressure >= 80.0f) {
        DrawRectangle(0, 0, screenWidth, screenHeight, (Color){255, 240, 220, (unsigned char)(12 + 14 * (sinf(player->blurPulse * 6.0f) * 0.5f + 0.5f))});
    }
}

void UI_DrawMainMenu(const AssetBundle *assets, bool hasSave, int screenWidth, int screenHeight, float elapsedSeconds) {
    int buttonIndex;
    const char *buttonLabels[MAIN_MENU_BUTTON_COUNT] = {
        LOC_UI_MENU_START,
        LOC_UI_MENU_CONTINUE,
        LOC_UI_MENU_EXIT
    };

    DrawBackdrop(screenWidth, screenHeight, elapsedSeconds);
    DrawUIText(assets, "SpaceCraft Living", (Vector2){screenWidth * 0.5f - MeasureUIText(assets, "SpaceCraft Living", 66.0f).x * 0.5f, 100.0f}, 66.0f, WHITE);
    DrawUIText(assets, "Survive, call for rescue, or choose to stay.", (Vector2){screenWidth * 0.5f - MeasureUIText(assets, "Survive, call for rescue, or choose to stay.", 27.0f).x * 0.5f, 176.0f}, 27.0f, (Color){196, 224, 240, 255});
    DrawUIText(assets, "Rebuilt to match the design document while keeping the original movement feel.", (Vector2){screenWidth * 0.5f - MeasureUIText(assets, "Rebuilt to match the design document while keeping the original movement feel.", 20.0f).x * 0.5f, 224.0f}, 20.0f, (Color){153, 190, 214, 255});
    DrawUIText(assets, hasSave ? "A local survival save was found." : "No local survival save was found.", (Vector2){screenWidth * 0.5f - MeasureUIText(assets, hasSave ? "A local survival save was found." : "No local survival save was found.", 19.0f).x * 0.5f, 292.0f}, 19.0f, hasSave ? (Color){152, 244, 206, 255} : (Color){183, 195, 208, 255});

    for (buttonIndex = 0; buttonIndex < MAIN_MENU_BUTTON_COUNT; buttonIndex++) {
        bool enabled;

        enabled = !(buttonIndex == MAIN_MENU_BUTTON_CONTINUE && !hasSave);
        DrawButton(assets, UI_GetMainMenuButtonRect(screenWidth, screenHeight, buttonIndex), buttonLabels[buttonIndex], enabled);
    }

    DrawUIText(assets, "Use Q / N / H in game for crafting, Loxi, and help.", (Vector2){screenWidth * 0.5f - MeasureUIText(assets, "Use Q / N / H in game for crafting, Loxi, and help.", 20.0f).x * 0.5f, screenHeight - 60.0f}, 20.0f, (Color){255, 202, 154, 255});
}

void UI_DrawPauseMenu(const AssetBundle *assets, int screenWidth, int screenHeight) {
    int buttonIndex;
    const char *buttonLabels[PAUSE_MENU_BUTTON_COUNT] = {
        LOC_UI_PAUSE_CONTINUE,
        LOC_UI_PAUSE_SAVE,
        LOC_UI_PAUSE_MENU
    };

    DrawRectangle(0, 0, screenWidth, screenHeight, (Color){6, 10, 18, 160});
    DrawPanel((Rectangle){screenWidth * 0.5f - 200.0f, screenHeight * 0.5f - 160.0f, 400.0f, 300.0f}, (Color){8, 18, 30, 235}, (Color){153, 226, 255, 70});
    DrawUIText(assets, LOC_UI_PAUSE_TITLE, (Vector2){screenWidth * 0.5f - MeasureUIText(assets, LOC_UI_PAUSE_TITLE, 34.0f).x * 0.5f, screenHeight * 0.5f - 122.0f}, 34.0f, WHITE);

    for (buttonIndex = 0; buttonIndex < PAUSE_MENU_BUTTON_COUNT; buttonIndex++) {
        DrawButton(assets, UI_GetPauseMenuButtonRect(screenWidth, screenHeight, buttonIndex), buttonLabels[buttonIndex], true);
    }
}

void UI_DrawBackpackOverlay(const AssetBundle *assets, const Player *player, int screenWidth, int screenHeight) {
    float scale;
    Rectangle panel;
    Rectangle leftColumn;
    Rectangle rightColumn;
    char buffer[160];

    scale = GetUIScale(screenWidth, screenHeight);
    panel = (Rectangle){screenWidth * 0.5f - 430.0f * scale, screenHeight * 0.5f - 250.0f * scale, 860.0f * scale, 500.0f * scale};
    leftColumn = (Rectangle){panel.x + 22.0f * scale, panel.y + 78.0f * scale, panel.width * 0.53f, panel.height - 110.0f * scale};
    rightColumn = (Rectangle){panel.x + panel.width * 0.60f, panel.y + 78.0f * scale, panel.width * 0.33f, panel.height - 110.0f * scale};

    DrawRectangle(0, 0, screenWidth, screenHeight, (Color){5, 9, 16, 190});
    DrawPanel(panel, (Color){8, 18, 30, 245}, (Color){124, 166, 214, 75});
    DrawUIText(assets, "Backpack", (Vector2){panel.x + 26.0f * scale, panel.y + 22.0f * scale}, 33.0f * scale, WHITE);
    DrawUIText(assets, "Press B or ESC to close.", (Vector2){panel.x + panel.width - MeasureUIText(assets, "Press B or ESC to close.", 17.5f * scale).x - 26.0f * scale, panel.y + 28.0f * scale}, 17.5f * scale, (Color){182, 199, 214, 255});

    DrawPanel(leftColumn, (Color){14, 26, 42, 220}, (Color){255, 255, 255, 22});
    DrawUIText(assets, "Supplies", (Vector2){leftColumn.x + 16.0f * scale, leftColumn.y + 16.0f * scale}, 24.0f * scale, WHITE);
    snprintf(buffer, sizeof(buffer), "Wood %d   Ore %d   Metal Scrap %d", player->resources[RESOURCE_WOOD], player->resources[RESOURCE_ORE], player->resources[RESOURCE_METAL_SCRAP]);
    DrawUIText(assets, buffer, (Vector2){leftColumn.x + 16.0f * scale, leftColumn.y + 58.0f * scale}, 18.5f * scale, (Color){226, 236, 245, 255});
    snprintf(buffer, sizeof(buffer), "Fruit %d   Shell Fruit %d   Fungus %d", player->resources[RESOURCE_FRUIT], player->resources[RESOURCE_SHELL_FRUIT], player->resources[RESOURCE_SPECIAL_FUNGUS]);
    DrawUIText(assets, buffer, (Vector2){leftColumn.x + 16.0f * scale, leftColumn.y + 92.0f * scale}, 18.5f * scale, (Color){226, 236, 245, 255});
    snprintf(buffer, sizeof(buffer), "Glow Moss %d   Alien Vine %d   Calm Mushroom %d", player->resources[RESOURCE_GLOW_MOSS], player->resources[RESOURCE_ALIEN_VINE], player->resources[RESOURCE_CALM_MUSHROOM]);
    DrawUIText(assets, buffer, (Vector2){leftColumn.x + 16.0f * scale, leftColumn.y + 126.0f * scale}, 18.5f * scale, (Color){226, 236, 245, 255});
    snprintf(buffer, sizeof(buffer), "Fiber %d   Crystal %d   Energy Core %d", player->resources[RESOURCE_PROTECTIVE_FIBER], player->resources[RESOURCE_ENERGY_CRYSTAL], player->resources[RESOURCE_ENERGY_CORE]);
    DrawUIText(assets, buffer, (Vector2){leftColumn.x + 16.0f * scale, leftColumn.y + 160.0f * scale}, 18.5f * scale, (Color){226, 236, 245, 255});
    snprintf(buffer, sizeof(buffer), "Relic Fragment %d   Boss Scale %d   Slime %d", player->resources[RESOURCE_RELIC_FRAGMENT], player->resources[RESOURCE_BOSS_SCALE], player->resources[RESOURCE_ALIEN_SLIME]);
    DrawUIText(assets, buffer, (Vector2){leftColumn.x + 16.0f * scale, leftColumn.y + 194.0f * scale}, 18.5f * scale, (Color){226, 236, 245, 255});
    DrawUIText(assets, "Quick use: Z consumes food combos, X uses calming items.", (Vector2){leftColumn.x + 16.0f * scale, leftColumn.y + 246.0f * scale}, 17.0f * scale, (Color){181, 205, 225, 255});
    DrawUIText(assets, "Craft stronger tools at the workbench in the Fabrication Bay.", (Vector2){leftColumn.x + 16.0f * scale, leftColumn.y + 278.0f * scale}, 17.0f * scale, (Color){181, 205, 225, 255});

    DrawPanel(rightColumn, (Color){14, 26, 42, 220}, (Color){255, 255, 255, 22});
    DrawUIText(assets, "Equipment", (Vector2){rightColumn.x + 16.0f * scale, rightColumn.y + 16.0f * scale}, 24.0f * scale, WHITE);
    DrawUIText(assets, player->hasGlowStick ? "Glow Stick: Equipped" : "Glow Stick: Missing", (Vector2){rightColumn.x + 16.0f * scale, rightColumn.y + 58.0f * scale}, 18.5f * scale, player->hasGlowStick ? (Color){170, 245, 215, 255} : (Color){226, 236, 245, 255});
    DrawUIText(assets, player->hasRope ? "Simple Rope: Packed" : "Simple Rope: Missing", (Vector2){rightColumn.x + 16.0f * scale, rightColumn.y + 92.0f * scale}, 18.5f * scale, player->hasRope ? (Color){170, 245, 215, 255} : (Color){226, 236, 245, 255});
    DrawUIText(assets, player->hasLaserGun ? "Laser Gun: Equipped" : "Laser Gun: Missing", (Vector2){rightColumn.x + 16.0f * scale, rightColumn.y + 126.0f * scale}, 18.5f * scale, player->hasLaserGun ? (Color){170, 245, 215, 255} : (Color){226, 236, 245, 255});
    DrawUIText(assets, player->hasProtectionSuit ? "Protection Suit: Equipped" : "Protection Suit: Missing", (Vector2){rightColumn.x + 16.0f * scale, rightColumn.y + 160.0f * scale}, 18.5f * scale, player->hasProtectionSuit ? (Color){170, 245, 215, 255} : (Color){226, 236, 245, 255});
    DrawUIText(assets, player->hasSignalAmplifier ? "Signal Amplifier: Ready" : "Signal Amplifier: Missing", (Vector2){rightColumn.x + 16.0f * scale, rightColumn.y + 194.0f * scale}, 18.5f * scale, player->hasSignalAmplifier ? (Color){170, 245, 215, 255} : (Color){226, 236, 245, 255});
    DrawUIText(assets, player->hasFieldCamp ? "Field Camp Kit: Deployed" : "Field Camp Kit: Missing", (Vector2){rightColumn.x + 16.0f * scale, rightColumn.y + 228.0f * scale}, 18.5f * scale, player->hasFieldCamp ? (Color){170, 245, 215, 255} : (Color){226, 236, 245, 255});
    snprintf(buffer, sizeof(buffer), "Oxygen %.0f   Stress %.0f   Poison %.0f", player->oxygen, player->pressure, player->poison);
    DrawUIText(assets, buffer, (Vector2){rightColumn.x + 16.0f * scale, rightColumn.y + 282.0f * scale}, 17.5f * scale, (Color){196, 214, 230, 255});
    snprintf(buffer, sizeof(buffer), "Stamina %.0f / %.0f", player->stamina, Player_GetCurrentStaminaCap(player));
    DrawUIText(assets, buffer, (Vector2){rightColumn.x + 16.0f * scale, rightColumn.y + 314.0f * scale}, 17.5f * scale, (Color){196, 214, 230, 255});
}

void UI_DrawCraftOverlay(const AssetBundle *assets, const TaskSystem *tasks, int screenWidth, int screenHeight) {
    float scale;
    Rectangle panel;
    int recipeCount;
    int recipeIndex;

    scale = GetUIScale(screenWidth, screenHeight);
    panel = (Rectangle){screenWidth * 0.5f - 410.0f * scale, screenHeight * 0.5f - 240.0f * scale, 820.0f * scale, 480.0f * scale};
    recipeCount = Tasks_GetVisibleRecipeCount(tasks);

    DrawRectangle(0, 0, screenWidth, screenHeight, (Color){5, 9, 16, 190});
    DrawPanel(panel, (Color){8, 18, 30, 245}, (Color){153, 226, 255, 75});
    DrawUIText(assets, LOC_UI_CRAFT_TITLE, (Vector2){panel.x + 26.0f * scale, panel.y + 22.0f * scale}, 33.0f * scale, WHITE);
    DrawUIText(assets, "Press 1-9 to craft. ESC or Q closes.", (Vector2){panel.x + panel.width - MeasureUIText(assets, "Press 1-9 to craft. ESC or Q closes.", 17.5f * scale).x - 26.0f * scale, panel.y + 28.0f * scale}, 17.5f * scale, (Color){182, 199, 214, 255});

    for (recipeIndex = 0; recipeIndex < recipeCount; recipeIndex++) {
        Rectangle rowRect;
        RecipeType recipe;
        char title[256];

        rowRect = (Rectangle){panel.x + 22.0f * scale, panel.y + 76.0f * scale + recipeIndex * 52.0f * scale, panel.width - 44.0f * scale, 42.0f * scale};
        recipe = Tasks_GetVisibleRecipeAt(tasks, recipeIndex);
        DrawPanel(rowRect, (Color){14, 26, 42, 220}, (Color){255, 255, 255, 24});
        snprintf(title, sizeof(title), "%d. %s", recipeIndex + 1, Player_GetRecipeName(recipe));
        DrawUIText(assets, title, (Vector2){rowRect.x + 14.0f * scale, rowRect.y + 8.0f * scale}, 19.5f * scale, WHITE);
        DrawUIText(assets, Player_GetRecipeSummary(recipe), (Vector2){rowRect.x + 220.0f * scale, rowRect.y + 10.0f * scale}, 16.5f * scale, (Color){208, 221, 232, 255});
    }
}

void UI_DrawCommunicatorOverlay(const AssetBundle *assets, const TaskSystem *tasks, int screenWidth, int screenHeight) {
    float scale;
    Rectangle panel;
    Rectangle section;
    const char *linkState;
    char stageBuffer[128];
    char cycleBuffer[128];
    scale = GetUIScale(screenWidth, screenHeight);
    panel = (Rectangle){screenWidth * 0.5f - 430.0f * scale, screenHeight * 0.5f - 250.0f * scale, 860.0f * scale, 500.0f * scale};
    section = (Rectangle){panel.x + 28.0f * scale, panel.y + 92.0f * scale, panel.width - 56.0f * scale, panel.height - 126.0f * scale};
    linkState = Tasks_IsCommunicatorUnlocked(tasks) ? "Online" : "Standby";
    snprintf(stageBuffer, sizeof(stageBuffer), "Current Stage: %s", Tasks_GetStageName(tasks->stage));
    snprintf(cycleBuffer, sizeof(cycleBuffer), "Current Cycle: Day %d - %s", tasks->dayCount + 1, Tasks_GetPhaseName(tasks->phase));

    DrawRectangle(0, 0, screenWidth, screenHeight, (Color){5, 9, 16, 190});
    DrawPanel(panel, (Color){8, 18, 30, 245}, (Color){99, 233, 195, 75});
    DrawUIText(assets, "Loxi Interface", (Vector2){panel.x + 26.0f * scale, panel.y + 22.0f * scale}, 33.0f * scale, WHITE);
    DrawUIText(assets, "Press N or ESC to close.", (Vector2){panel.x + panel.width - MeasureUIText(assets, "Press N or ESC to close.", 17.5f * scale).x - 24.0f * scale, panel.y + 28.0f * scale}, 17.5f * scale, (Color){182, 199, 214, 255});
    DrawUIText(assets, linkState, (Vector2){panel.x + 28.0f * scale, panel.y + 58.0f * scale}, 19.0f * scale, (Color){166, 255, 226, 255});
    DrawPanel(section, (Color){14, 26, 42, 220}, (Color){255, 255, 255, 22});
    DrawUIText(assets, stageBuffer, (Vector2){section.x + 18.0f * scale, section.y + 18.0f * scale}, 22.0f * scale, WHITE);
    DrawUIText(assets, cycleBuffer, (Vector2){section.x + 18.0f * scale, section.y + 56.0f * scale}, 20.0f * scale, (Color){194, 224, 255, 255});
    DrawUIText(assets, Tasks_GetCommunicatorHint(tasks), (Vector2){section.x + 18.0f * scale, section.y + 104.0f * scale}, 20.0f * scale, (Color){227, 237, 245, 255});
}

void UI_DrawHelpOverlay(const AssetBundle *assets, int screenWidth, int screenHeight) {
    float scale;
    Rectangle panel;
    const char *lines[] = {
        "WASD / Arrow Keys: original grid movement",
        "E: interact, gather, repair, read logs",
        "Space: attack, uses laser line if available",
        "B: open the backpack and inspect supplies",
        "Q: open crafting",
        "N: open the Loxi interface",
        "H: open this help panel",
        "Z: use food for quick stamina recovery",
        "X: use calm items for quick stress relief",
        "C: toggle crouch stealth",
        "ESC: pause",
        "Stamina slowly recovers near base or camp"
    };
    int lineIndex;

    scale = GetUIScale(screenWidth, screenHeight);
    panel = (Rectangle){screenWidth * 0.5f - 420.0f * scale, screenHeight * 0.5f - 260.0f * scale, 840.0f * scale, 520.0f * scale};

    DrawRectangle(0, 0, screenWidth, screenHeight, (Color){5, 9, 16, 190});
    DrawPanel(panel, (Color){8, 18, 30, 245}, (Color){153, 226, 255, 75});
    DrawUIText(assets, LOC_UI_HELP_TITLE, (Vector2){panel.x + 26.0f * scale, panel.y + 22.0f * scale}, 33.0f * scale, WHITE);
    DrawUIText(assets, LOC_UI_HELP_HINT, (Vector2){panel.x + panel.width - MeasureUIText(assets, LOC_UI_HELP_HINT, 17.0f * scale).x - 24.0f * scale, panel.y + 28.0f * scale}, 17.0f * scale, (Color){182, 199, 214, 255});

    for (lineIndex = 0; lineIndex < (int)(sizeof(lines) / sizeof(lines[0])); lineIndex++) {
        DrawUIText(assets, lines[lineIndex], (Vector2){panel.x + 30.0f * scale, panel.y + 92.0f * scale + lineIndex * 35.0f * scale}, 22.0f * scale, (Color){229, 238, 246, 255});
    }
}

void UI_DrawEnding(GameEnding ending, const Player *player, const TaskSystem *tasks, const AssetBundle *assets, int screenWidth, int screenHeight, float elapsedSeconds) {
    char detail[256];

    (void)elapsedSeconds;
    (void)tasks;

    DrawBackdrop(screenWidth, screenHeight, elapsedSeconds);
    DrawRectangle(0, 0, screenWidth, screenHeight, (Color){5, 9, 16, 120});
    DrawUIText(assets, Tasks_GetEndingTitle(ending), (Vector2){screenWidth * 0.5f - MeasureUIText(assets, Tasks_GetEndingTitle(ending), 50.0f).x * 0.5f, 148.0f}, 50.0f, WHITE);
    DrawUIText(assets, Tasks_GetEndingBody(ending), (Vector2){screenWidth * 0.5f - MeasureUIText(assets, Tasks_GetEndingBody(ending), 24.0f).x * 0.5f, 240.0f}, 24.0f, (Color){211, 225, 236, 255});
    snprintf(detail, sizeof(detail), "Oxygen Left %.0f  Stress %.0f  Total Collapses %d", player->oxygen, player->pressure, player->deathCount);
    DrawUIText(assets, detail, (Vector2){screenWidth * 0.5f - MeasureUIText(assets, detail, 20.0f).x * 0.5f, 320.0f}, 20.0f, (Color){255, 205, 151, 255});
    DrawUIText(assets, LOC_UI_END_EXIT, (Vector2){screenWidth * 0.5f - MeasureUIText(assets, LOC_UI_END_EXIT, 20.0f).x * 0.5f, screenHeight - 90.0f}, 20.0f, (Color){196, 213, 225, 255});
}
