#include "game_session_internal.h"

#include <cstring>

void Game_BeginScreenTransition(Game *game, ScreenTransitionAction action, int slotIndex) {
    if (game == NULL || action == SCREEN_TRANSITION_NONE || game->screenTransitionActive) {
        return;
    }

    game->screenTransitionActive = true;
    game->screenTransitionResolved = false;
    game->screenTransitionElapsed = 0.0f;
    game->screenTransitionAction = action;
    game->screenTransitionSlotIndex = slotIndex;
}

void Game_ApplyLanguage(Game *game, GameLanguage language) {
    GameLanguage normalized;

    if (game == NULL) {
        return;
    }

    normalized = Loc_NormalizeLanguage((int)language);
    game->pendingLanguage = normalized;
    if (game->settings.language == normalized) {
        return;
    }

    game->settings.language = normalized;
    game->settingsDirty = true;
    Loc_SetLanguage(normalized);
    Tasks_UpdateObjective(&game->tasks, &game->player);
}

void Game_BeginLanguageTransition(Game *game, GameLanguage language) {
    GameLanguage normalized;

    if (game == NULL) {
        return;
    }

    normalized = Loc_NormalizeLanguage((int)language);
    game->pendingLanguage = normalized;
    if (game->screenTransitionActive || game->settings.language == normalized) {
        return;
    }

    Game_BeginScreenTransition(game, SCREEN_TRANSITION_APPLY_LANGUAGE, -1);
}

static float ClampFloat(float value, float minValue, float maxValue) {
    if (value < minValue) {
        return minValue;
    }
    if (value > maxValue) {
        return maxValue;
    }
    return value;
}

static int AbsInt(int value) {
    return value < 0 ? -value : value;
}

static bool IsSafeLoadedPlayerTile(const Game *game, int gridX, int gridY) {
    return Map_IsWithinBounds(gridX, gridY)
        && Map_IsWalkable(&game->map, gridX, gridY)
        && !Tasks_IsBlockingActorTile(&game->tasks, gridX, gridY);
}

bool Game_FindNearestSafeLoadedPlayerTile(const Game *game, int originX, int originY, int *gridX, int *gridY) {
    int radius;
    int maxRadius;

    if (IsSafeLoadedPlayerTile(game, originX, originY)) {
        *gridX = originX;
        *gridY = originY;
        return true;
    }

    maxRadius = MAP_WIDTH + MAP_HEIGHT;
    for (radius = 1; radius <= maxRadius; radius++) {
        int offsetY;

        for (offsetY = -radius; offsetY <= radius; offsetY++) {
            int offsetX;
            int candidateX;
            int candidateY;

            offsetX = radius - AbsInt(offsetY);
            candidateY = originY + offsetY;

            candidateX = originX - offsetX;
            if (IsSafeLoadedPlayerTile(game, candidateX, candidateY)) {
                *gridX = candidateX;
                *gridY = candidateY;
                return true;
            }

            if (offsetX == 0) {
                continue;
            }

            candidateX = originX + offsetX;
            if (IsSafeLoadedPlayerTile(game, candidateX, candidateY)) {
                *gridX = candidateX;
                *gridY = candidateY;
                return true;
            }
        }
    }

    return false;
}

void Game_CloseTransientOverlays(Game *game) {
    game->pauseMenuOpen = false;
    game->settingsOpen = false;
    game->settingsSliderDragging = false;
    game->settingsSliderDragIndex = -1;
    game->infoOverlayOpen = false;
    game->infoOverlayTab = INFO_OVERLAY_TAB_MAP;
    game->craftOpen = false;
    game->communicatorLogDetailOpen = false;
    game->communicatorLogDetailVisibility = 0.0f;
    game->communicatorLogDetailScroll = 0.0f;
    game->helpOpen = false;
    game->logReaderOpen = false;
    game->savePanelOpen = false;
    game->showDeathPopup = false;
    game->deathPopupSelection = DEATH_POPUP_BUTTON_RESTART;
    game->settlementConfirmOpen = false;
    game->settlementConfirmSelection = SETTLEMENT_CONFIRM_BUTTON_HEROIC;
    game->endingRouteDoubleConfirmOpen = false;
    game->endingRouteDoubleConfirmSelection = 0;
    game->endingRoutePendingConfirm = ENDING_NONE;
    game->accountDeleteConfirmOpen = false;
    game->accountDeleteConfirmSelection = ACCOUNT_DELETE_CONFIRM_BUTTON_CANCEL;
    game->accountDeleteFromAuth = false;
    game->accountDeleteTargetName[0] = '\0';
}

void Game_ResetCameraToPlayer(Game *game) {
    game->camera.target = game->player.worldPos;
    game->camera.offset = Vector2{GetScreenWidth() * 0.5f, GetScreenHeight() * 0.5f};
    game->camera.rotation = 0.0f;
    game->camera.zoom = CAMERA_ZOOM;
}

void Game_ResetTransientGameplayState(Game *game) {
    game->bufferedMoveX = 0;
    game->bufferedMoveY = 0;
    game->inputBufferTimer = 0.0f;
    game->heldMoveX = 0;
    game->heldMoveY = 0;
    game->holdRepeatTimer = 0.0f;
    game->laserEffectTimer = 0.0f;
    game->laserEffectHit = false;
    game->laserEffectStart = Vector2{0.0f, 0.0f};
    game->laserEffectEnd = Vector2{0.0f, 0.0f};
    game->player.moveTimer = 0.0f;
    game->player.moveAnimElapsed = 0.0f;
    game->player.moveAnimDuration = 0.0f;
    game->player.safeRecoveryTimer = 0.0f;
}

static void EnterGameplayFromOpening(Game *game) {
    game->state = GAME_STATE_PLAYING;
    game->openingSlideIndex = 0;
    game->openingCutsceneElapsed = 0.0f;
    game->openingAwaitingFirstAdvance = false;
    game->narrativeTransitionActive = false;
    game->narrativeTransitionElapsed = 0.0f;
    game->narrativeTransitionAction = NARRATIVE_TRANSITION_NONE;
    Game_CloseStoryScene(game);
    Audio_SetScene(&game->audio, Game_SelectAudioScene(game));
    Audio_SetMusicStage(&game->audio, Game_SelectMusicStage(game));
    Game_PostMessage(game, Loc_PickLiteral("Wake in Loxi's cabin. Sync the uplink, then restore the lower oxygen console.", "先在洛希舱室醒来并完成同步，再去修复下层氧气控制台。"), 4.2f);
}

static void RestartGameplayAfterDeath(Game *game) {
    Game_ResetGameplayWorld(game);
    Game_RefreshSaveSlots(game);
    MiniMap_Update(&game->miniMap, &game->player, &game->map);
    EnterGameplayFromOpening(game);
}

void Game_ResetGameplayWorld(Game *game) {
    Map_Init(&game->map);
    Player_Init(&game->player);
    Tasks_Init(&game->tasks, &game->map);
    Tasks_UpdateObjective(&game->tasks, &game->player);
    MiniMap_Init(&game->miniMap);
    game->elapsedSeconds = 0.0f;
    game->openingCutsceneElapsed = 0.0f;
    game->openingAwaitingFirstAdvance = false;
    game->storySceneElapsed = 0.0f;
    game->narrativeTransitionActive = false;
    game->narrativeTransitionElapsed = 0.0f;
    game->narrativeTransitionAction = NARRATIVE_TRANSITION_NONE;
    game->screenTransitionActive = false;
    game->screenTransitionResolved = false;
    game->screenTransitionElapsed = 0.0f;
    game->screenTransitionAction = SCREEN_TRANSITION_NONE;
    game->screenTransitionSlotIndex = -1;
    game->hurtSoundCooldown = 0.0f;
    game->monsterCueCooldown = 0.0f;
    game->laserEffectTimer = 0.0f;
    game->laserEffectHit = false;
    game->laserEffectStart = Vector2{0.0f, 0.0f};
    game->laserEffectEnd = Vector2{0.0f, 0.0f};
    game->bufferedMoveX = 0;
    game->bufferedMoveY = 0;
    game->inputBufferTimer = 0.0f;
    game->heldMoveX = 0;
    game->heldMoveY = 0;
    game->holdRepeatTimer = 0.0f;
    Game_CloseTransientOverlays(game);
    game->infoOverlayTab = INFO_OVERLAY_TAB_MAP;
    game->communicatorTab = COMMUNICATOR_TAB_TASKS;
    game->selectedBackpackItem = 0;
    game->selectedCraftIndex = 0;
    game->savePanelMode = SAVE_PANEL_MODE_LOAD;
    game->selectedSaveSlot = 0;
    game->openingSlideIndex = 0;
    game->storySceneOpen = false;
    game->storyScene = STORY_SCENE_NONE;
    std::memset(game->storySceneShown, 0, sizeof(game->storySceneShown));
    game->selectedLogIndex = 0;
    game->communicatorFirstVisibleLogIndex = 0;
    game->selectedStorySceneIndex = 0;
    game->communicatorFirstVisibleStorySceneIndex = 0;
    game->communicatorLogDetailOpen = false;
    game->communicatorLogDetailVisibility = 0.0f;
    game->communicatorLogDetailScroll = 0.0f;
    game->pendingLanguage = game->settings.language;
    game->requestClose = false;
    game->lastLocationName[0] = '\0';
    Game_SyncTrackedLocation(game);
    Game_ResetCameraToPlayer(game);
    Game_ClearMessage(game);
    Game_ClearMessageHistory(game);
}

void Game_StartNewGame(Game *game) {
    if (game == NULL) {
        return;
    }

    Game_ResetGameplayWorld(game);
    Game_RefreshSaveSlots(game);
    MiniMap_Update(&game->miniMap, &game->player, &game->map);
    game->state = GAME_STATE_OPENING;
    game->openingSlideIndex = 0;
    game->openingCutsceneElapsed = 0.0f;
    game->openingAwaitingFirstAdvance = true;
    game->narrativeTransitionActive = false;
    game->narrativeTransitionElapsed = 0.0f;
    game->narrativeTransitionAction = NARRATIVE_TRANSITION_NONE;
    Audio_SetScene(&game->audio, AUDIO_SCENE_MENU);
    Audio_SetMusicStage(&game->audio, Game_SelectMusicStage(game));
}

void Game_CompleteOpeningCutscene(Game *game) {
    if (game == NULL) {
        return;
    }

    EnterGameplayFromOpening(game);
}

void Game_HandlePlayerDeath(Game *game) {
    if (game->showDeathPopup || game->state != GAME_STATE_PLAYING || game->tasks.ending != ENDING_NONE) {
        return;
    }

    game->player.deathCount += 1;
    game->player.health = 0.0f;
    game->player.oxygen = ClampFloat(game->player.oxygen, 0.0f, MAX_OXYGEN);
    game->showDeathPopup = true;
    game->deathPopupSelection = DEATH_POPUP_BUTTON_RESTART;
    game->screenTransitionActive = false;
    game->screenTransitionAction = SCREEN_TRANSITION_NONE;
    Game_ResetTransientGameplayState(game);
    Game_RecordActiveAccountScore(game);
    Audio_PlayCue(&game->audio, AUDIO_CUE_WARNING);
}

void Game_HandleDeathRecovery(Game *game) {
    if (game == NULL) {
        return;
    }

    RestartGameplayAfterDeath(game);
}

void Game_ReturnToMenu(Game *game) {
    Game_TrySaveSettings(game);
    game->state = GAME_STATE_INTRO;
    game->openingSlideIndex = 0;
    game->openingCutsceneElapsed = 0.0f;
    game->openingAwaitingFirstAdvance = false;
    Game_CloseStoryScene(game);
    Game_CloseTransientOverlays(game);
    Game_ClearMessage(game);
    Game_ClearMessageHistory(game);
    Game_RefreshSaveSlots(game);
    Audio_SetScene(&game->audio, AUDIO_SCENE_MENU);
    Audio_SetMusicStage(&game->audio, AUDIO_MUSIC_MENU);
}
