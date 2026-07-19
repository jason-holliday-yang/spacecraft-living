#include "game_overlay_gameplay_internal.h"

/* In-game overlay priority and dispatch only. */

bool Game_UpdateGameplayOverlayState(Game *game) {
    if (game->ui.showDeathPopup) {
        GameOverlay_UpdateDeathPopup(game);
        return true;
    }

    if (game->ui.craftOpen) {
        GameOverlay_UpdateCraft(game);
        return true;
    }

    if (game->ui.infoOverlayOpen) {
        GameOverlay_UpdateInfo(game);
        return true;
    }

    if (game->ui.helpOpen) {
        GameOverlay_UpdateHelp(game);
        return true;
    }

    return false;
}
