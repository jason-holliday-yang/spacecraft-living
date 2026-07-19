#include "game_overlay_internal.h"

bool Game_UpdateOverlayState(Game *game) {
    if (Game_UpdateFrontEndOverlayState(game)) {
        return true;
    }

    return Game_UpdateGameplayOverlayState(game);
}
