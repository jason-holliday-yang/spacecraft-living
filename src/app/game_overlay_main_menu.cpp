#include "game_overlay_frontend_internal.h"
#include "game_session_internal.h"

#include "localization.h"
#include "ui_runtime_internal.h"

#include <cmath>
#include <cstdio>
#include <cstring>

void GameOverlay_UpdateMainMenu(Game *game) {
    Vector2 mouse;
    int buttonIndex;

    if (GameOverlay_IsConfirmPressed()) {
        Audio_PlayCue(&game->services.audio, AUDIO_CUE_CONFIRM);
        Game_StartNewGame(game);
        return;
    }

    if (!GameOverlay_TryGetPrimaryClickPosition(&mouse)) {
        return;
    }

    buttonIndex = UI_GetMainMenuButtonIndexAtPoint(GetScreenWidth(), GetScreenHeight(), mouse);
    switch (buttonIndex) {
        case MAIN_MENU_BUTTON_START:
            Audio_PlayCue(&game->services.audio, AUDIO_CUE_CONFIRM);
            Game_StartNewGame(game);
            return;
        case MAIN_MENU_BUTTON_LOAD:
            if (game->account.hasSaveFile) {
                Audio_PlayCue(&game->services.audio, AUDIO_CUE_OPEN);
                Game_OpenSavePanel(game, SAVE_PANEL_MODE_LOAD);
            }
            return;
        case MAIN_MENU_BUTTON_SETTINGS:
            game->ui.settingsOpen = true;
            Audio_PlayCue(&game->services.audio, AUDIO_CUE_OPEN);
            return;
        case MAIN_MENU_BUTTON_EXIT:
            game->flow.requestClose = true;
            return;
        default:
            break;
    }
}

void GameOverlay_UpdatePauseMenu(Game *game) {
    Vector2 mouse;
    int buttonIndex;

    if (GameOverlay_TryCloseOverlay(game, &game->ui.pauseMenuOpen, KEY_NULL)) {
        return;
    }

    if (!GameOverlay_TryGetPrimaryClickPosition(&mouse)) {
        return;
    }

    buttonIndex = GameOverlay_FindClickedIndexedRect(mouse, PAUSE_MENU_BUTTON_COUNT, UI_GetPauseMenuButtonRect);
    if (buttonIndex < 0) {
        return;
    }

    switch (buttonIndex) {
        case PAUSE_MENU_BUTTON_CONTINUE:
            game->ui.pauseMenuOpen = false;
            Audio_PlayCue(&game->services.audio, AUDIO_CUE_CONFIRM);
            return;
        case PAUSE_MENU_BUTTON_SAVE:
            Audio_PlayCue(&game->services.audio, AUDIO_CUE_OPEN);
            Game_OpenSavePanel(game, SAVE_PANEL_MODE_SAVE);
            return;
        case PAUSE_MENU_BUTTON_LOAD:
            if (game->account.hasSaveFile) {
                Audio_PlayCue(&game->services.audio, AUDIO_CUE_OPEN);
                Game_OpenSavePanel(game, SAVE_PANEL_MODE_LOAD);
            } else {
                Game_PostMessage(game, LOC_UI_SAVE_MISSING, 2.8f);
            }
            return;
        case PAUSE_MENU_BUTTON_SETTINGS:
            game->ui.pauseMenuOpen = false;
            game->ui.infoOverlayOpen = true;
            game->ui.infoOverlayTab = INFO_OVERLAY_TAB_SETTINGS;
            game->ui.settingsOpen = false;
            Audio_PlayCue(&game->services.audio, AUDIO_CUE_OPEN);
            return;
        case PAUSE_MENU_BUTTON_MENU:
            Audio_PlayCue(&game->services.audio, AUDIO_CUE_CLOSE);
            Game_BeginScreenTransition(game, SCREEN_TRANSITION_RETURN_TO_MENU, -1);
            return;
        default:
            break;
    }
}
