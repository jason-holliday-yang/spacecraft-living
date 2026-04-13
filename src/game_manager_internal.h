#ifndef GAME_MANAGER_INTERNAL_H
#define GAME_MANAGER_INTERNAL_H

#include "game_manager.h"

/* Internal game-manager/session helpers shared across runtime coordinator slices. */

SCL_EXTERN_C_BEGIN

void Game_PostMessage(Game *game, const char *text, float duration);
void Game_ClearMessage(Game *game);
void Game_TrySaveSettings(Game *game);
bool Game_OpenStoryScene(Game *game, StoryScene scene);
void Game_CloseStoryScene(Game *game);
AudioScene Game_SelectAudioScene(const Game *game);
void Game_SyncTrackedLocation(Game *game);
void Game_MaybePostNorthRouteTransitionHint(Game *game);

void Game_ResetGameplayWorld(Game *game);
void Game_StartNewGame(Game *game);
void Game_CompleteOpeningCutscene(Game *game);
void Game_HandlePlayerDeath(Game *game);
void Game_HandleDeathRecovery(Game *game);
void Game_RefreshSaveSlots(Game *game);
void Game_CloseSavePanel(Game *game);
void Game_OpenSavePanel(Game *game, SavePanelMode mode);
bool Game_SaveCurrentGame(Game *game, int slotIndex);
bool Game_LoadSavedGame(Game *game, int slotIndex);
void Game_ReturnToMenu(Game *game);
void Game_MoveSavePanelSelection(Game *game, int delta);
void Game_ActivateSelectedSaveSlot(Game *game);
void Game_DeleteSelectedSaveSlot(Game *game);

bool Game_UpdateOverlayState(Game *game);
void Game_UpdatePlayingState(Game *game, float deltaTime);

SCL_EXTERN_C_END

#endif
