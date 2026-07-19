#include "game_overlay_gameplay_internal.h"

#include "localization.h"
#include "ui_inventory_internal.h"
#include "ui_story_internal.h"
#include "ui_runtime_internal.h"

#include <cmath>
#include <cstdio>

void GameOverlay_UpdateDeathPopup(Game *game) {
    Vector2 mouse;
    int buttonIndex;
    bool activateSelection;
    const bool canLoad = game != NULL && game->account.hasSaveFile;

    activateSelection = false;
    if (!canLoad) {
        if (game->ui.deathPopupSelection == DEATH_POPUP_BUTTON_LOAD) {
            game->ui.deathPopupSelection = DEATH_POPUP_BUTTON_RESTART;
        }
    }

    if (IsKeyPressed(KEY_ESCAPE)) {
        game->ui.deathPopupSelection = DEATH_POPUP_BUTTON_MENU;
        activateSelection = true;
    }

    if (GameOverlay_IsBackwardNavigationPressed()) {
        do {
            game->ui.deathPopupSelection = (game->ui.deathPopupSelection + DEATH_POPUP_BUTTON_COUNT - 1) % DEATH_POPUP_BUTTON_COUNT;
        } while (!canLoad && game->ui.deathPopupSelection == DEATH_POPUP_BUTTON_LOAD);
        Audio_PlayCue(&game->services.audio, AUDIO_CUE_OPEN);
    }

    if (GameOverlay_IsForwardNavigationPressed()) {
        do {
            game->ui.deathPopupSelection = (game->ui.deathPopupSelection + 1) % DEATH_POPUP_BUTTON_COUNT;
        } while (!canLoad && game->ui.deathPopupSelection == DEATH_POPUP_BUTTON_LOAD);
        Audio_PlayCue(&game->services.audio, AUDIO_CUE_OPEN);
    }

    if (GameOverlay_TryGetPrimaryClickPosition(&mouse)) {
        buttonIndex = GameOverlay_FindClickedIndexedRect(mouse, DEATH_POPUP_BUTTON_COUNT, UI_GetDeathPopupButtonRect);
        if (buttonIndex >= 0) {
            game->ui.deathPopupSelection = buttonIndex;
            activateSelection = true;
        }
    }

    if (GameOverlay_IsConfirmPressed()) {
        activateSelection = true;
    }

    if (!activateSelection) {
        return;
    }

    if (game->ui.deathPopupSelection == DEATH_POPUP_BUTTON_RESTART) {
        Game_HandleDeathRecovery(game);
    } else if (game->ui.deathPopupSelection == DEATH_POPUP_BUTTON_LOAD && canLoad) {
        Audio_PlayCue(&game->services.audio, AUDIO_CUE_OPEN);
        Game_OpenSavePanel(game, SAVE_PANEL_MODE_LOAD);
    } else if (game->ui.deathPopupSelection == DEATH_POPUP_BUTTON_MENU) {
        Audio_PlayCue(&game->services.audio, AUDIO_CUE_CLOSE);
        Game_BeginScreenTransition(game, SCREEN_TRANSITION_RETURN_TO_MENU, -1);
    }
}
