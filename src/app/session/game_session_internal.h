#ifndef GAME_SESSION_INTERNAL_H
#define GAME_SESSION_INTERNAL_H

#include "game_manager_internal.h"

/* Internal session-flow helpers shared by gameplay lifecycle and save-slot slices. */

SCL_EXTERN_C_BEGIN

void Game_CloseTransientOverlays(Game *game);
void Game_ResetCameraToPlayer(Game *game);
void Game_ResetTransientGameplayState(Game *game);
bool Game_FindNearestSafeLoadedPlayerTile(const Game *game,
                                         int originX,
                                         int originY,
                                         int *gridX,
                                         int *gridY);
void Game_BuildSaveSnapshot(const Game *game, SaveSnapshot *snapshot);
bool Game_LoadSnapshotIntoSession(Game *game, const SaveSnapshot *snapshot);

SCL_EXTERN_C_END

#endif
