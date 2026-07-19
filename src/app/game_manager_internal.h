#ifndef GAME_MANAGER_INTERNAL_H
#define GAME_MANAGER_INTERNAL_H

#include "game_manager.h"

/* Internal game-manager/session helpers shared across runtime coordinator slices. */

SCL_EXTERN_C_BEGIN

void Game_PostMessage(Game *game, const char *text, float duration);
void Game_ClearMessage(Game *game);
void Game_ClearMessageHistory(Game *game);
void Game_SanitizeGameplayState(Game *game, int screenWidth, int screenHeight);
float Game_GetScreenTransitionDuration(const Game *game);
float Game_GetNarrativeTransitionDuration(void);
float Game_GetScreenTransitionAlpha(const Game *game);
float Game_GetNarrativeOverlayAlpha(const Game *game);
void Game_AdvanceWorldClock(Game *game, float deltaTime);
void Game_MaybePostDayAdvanceMessage(Game *game, int previousDayCount);
void Game_TrySaveSettings(Game *game);
void Game_BeginScreenTransition(Game *game, ScreenTransitionAction action, int slotIndex);
void Game_BeginLanguageTransition(Game *game, GameLanguage language);
bool Game_ResolveMapTransition(Game *game);
void Game_FinishMapTransition(Game *game);
bool Game_LoadLegacyCompatibilityMap(Game *game);
void Game_ApplyLanguage(Game *game, GameLanguage language);
void Game_ToggleAuthMode(Game *game);
void Game_LogoutToAuthScreen(Game *game);
void Game_CompleteAuthSuccess(Game *game);
void Game_EnterEndingState(Game *game);
bool Game_OpenStoryScene(Game *game, StoryScene scene);
void Game_CloseStoryScene(Game *game);
AudioScene Game_SelectAudioScene(const Game *game);
AudioMusicStage Game_SelectMusicStage(const Game *game);
void Game_SyncTrackedLocation(Game *game);
void Game_MaybePostNorthRouteTransitionHint(Game *game);

void Game_ResetGameplayWorld(Game *game);
void Game_StartNewGame(Game *game);
void Game_CompleteOpeningCutscene(Game *game);
void Game_HandlePlayerDeath(Game *game);
void Game_HandleDeathRecovery(Game *game);
void Game_RecordActiveAccountScore(Game *game);
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
