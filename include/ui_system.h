#ifndef UI_SYSTEM_H
#define UI_SYSTEM_H

#include "assets.h"
#include "map.h"
#include "player.h"
#include "save_system.h"
#include "task_system.h"

typedef struct HudMessage {
    char text[256];
    float timer;
} HudMessage;

typedef enum MainMenuButton {
    MAIN_MENU_BUTTON_START = 0,
    MAIN_MENU_BUTTON_CONTINUE,
    MAIN_MENU_BUTTON_EXIT,
    MAIN_MENU_BUTTON_COUNT
} MainMenuButton;

typedef enum PauseMenuButton {
    PAUSE_MENU_BUTTON_CONTINUE = 0,
    PAUSE_MENU_BUTTON_SAVE,
    PAUSE_MENU_BUTTON_MENU,
    PAUSE_MENU_BUTTON_COUNT
} PauseMenuButton;

Rectangle UI_GetMainMenuButtonRect(int screenWidth, int screenHeight, int buttonIndex);
Rectangle UI_GetPauseMenuButtonRect(int screenWidth, int screenHeight, int buttonIndex);
void UI_DrawHud(const Player *player, const TaskSystem *tasks, const HudMessage *message, const AssetBundle *assets, int screenWidth, int screenHeight);
void UI_DrawMainMenu(const AssetBundle *assets, bool hasSave, int screenWidth, int screenHeight, float elapsedSeconds);
void UI_DrawPauseMenu(const AssetBundle *assets, int screenWidth, int screenHeight);
void UI_DrawBackpackOverlay(const AssetBundle *assets, const Player *player, int screenWidth, int screenHeight);
void UI_DrawCraftOverlay(const AssetBundle *assets, const TaskSystem *tasks, int screenWidth, int screenHeight);
void UI_DrawCommunicatorOverlay(const AssetBundle *assets, const TaskSystem *tasks, int screenWidth, int screenHeight);
void UI_DrawHelpOverlay(const AssetBundle *assets, int screenWidth, int screenHeight);
void UI_DrawEnding(GameEnding ending, const Player *player, const TaskSystem *tasks, const AssetBundle *assets, int screenWidth, int screenHeight, float elapsedSeconds);
void UI_DrawDownedOverlay(const Player *player, const AssetBundle *assets, int screenWidth, int screenHeight);
void UI_DrawDeathPopup(const Player *player, const AssetBundle *assets, int screenWidth, int screenHeight);
void UI_DrawLogReader(const TaskSystem *tasks, int selectedLog, const AssetBundle *assets, int screenWidth, int screenHeight);

#endif
