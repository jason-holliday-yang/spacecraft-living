#include "game_overlay_frontend_internal.h"

/* Frontend overlay priority and dispatch only. */

bool Game_UpdateFrontEndOverlayState(Game *game) {
    if (game->flow.state == GAME_STATE_INTRO) {
        if (game->account.accountDeleteConfirmOpen) {
            GameOverlay_UpdateAccountDeleteConfirm(game);
        } else if (!game->account.authenticated) {
            GameOverlay_UpdateAuthScreen(game);
        } else if (game->ui.settingsOpen) {
            GameOverlay_UpdateSettingsOverlay(game);
        } else if (game->ui.savePanelOpen) {
            GameOverlay_UpdateSavePanel(game);
        } else {
            GameOverlay_UpdateMainMenu(game);
        }
        return true;
    }

    if (game->flow.state == GAME_STATE_OPENING) {
        GameOverlay_UpdateOpeningCutscene(game);
        return true;
    }

    if (game->flow.storySceneOpen) {
        GameOverlay_UpdateStoryScene(game);
        return true;
    }

    if (game->flow.state == GAME_STATE_ENDING) {
        GameOverlay_UpdateEndingMenu(game);
        return true;
    }

    if (game->ui.settlementConfirmOpen) {
        GameOverlay_UpdateSettlementConfirm(game);
        return true;
    }

    if (game->ui.endingRouteDoubleConfirmOpen) {
        GameOverlay_UpdateEndingRouteDoubleConfirm(game);
        return true;
    }

    if (game->ui.savePanelOpen) {
        GameOverlay_UpdateSavePanel(game);
        return true;
    }

    if (game->ui.pauseMenuOpen) {
        GameOverlay_UpdatePauseMenu(game);
        return true;
    }

    return false;
}
