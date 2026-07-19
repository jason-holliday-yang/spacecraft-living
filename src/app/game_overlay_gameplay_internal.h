#ifndef GAME_OVERLAY_GAMEPLAY_INTERNAL_H
#define GAME_OVERLAY_GAMEPLAY_INTERNAL_H

#include "game_overlay_internal.h"

/* In-game overlay slices. The info shell owns tab routing; panels own their input. */

void GameOverlay_UpdateBackpack(Game *game, bool *open, KeyboardKey alternateKey);
void GameOverlay_UpdateCraft(Game *game);
void GameOverlay_UpdateCommunicator(Game *game, bool *open, KeyboardKey alternateKey);
void GameOverlay_ResetCommunicatorDetailState(Game *game);
void GameOverlay_UpdateMap(Game *game, bool *open, KeyboardKey alternateKey);
void GameOverlay_UpdateInfo(Game *game);
void GameOverlay_UpdateHelp(Game *game);
void GameOverlay_UpdateDeathPopup(Game *game);

#endif
