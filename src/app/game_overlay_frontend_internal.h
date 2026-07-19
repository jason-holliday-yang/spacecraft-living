#ifndef GAME_OVERLAY_FRONTEND_INTERNAL_H
#define GAME_OVERLAY_FRONTEND_INTERNAL_H

#include "game_overlay_internal.h"

/* Frontend overlay slices. Keep state ordering in game_overlay_frontend.cpp. */

void GameOverlay_UpdateAccountDeleteConfirm(Game *game);
void GameOverlay_UpdateAuthScreen(Game *game);
void GameOverlay_UpdateMainMenu(Game *game);
void GameOverlay_UpdatePauseMenu(Game *game);
void GameOverlay_UpdateSettingsOverlay(Game *game);
void GameOverlay_UpdateSavePanel(Game *game);
void GameOverlay_UpdateOpeningCutscene(Game *game);
void GameOverlay_UpdateStoryScene(Game *game);
void GameOverlay_UpdateEndingMenu(Game *game);
void GameOverlay_UpdateSettlementConfirm(Game *game);
void GameOverlay_UpdateEndingRouteDoubleConfirm(Game *game);

#endif
