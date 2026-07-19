#include "game_overlay_gameplay_internal.h"

#include "localization.h"
#include "ui_inventory_internal.h"
#include "ui_story_internal.h"
#include "ui_runtime_internal.h"

#include <cmath>
#include <cstdio>

void GameOverlay_UpdateMap(Game *game, bool *open, KeyboardKey alternateKey) {
    GameOverlay_TryCloseOverlay(game, open, alternateKey);
}
