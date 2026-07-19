#include "game_session_internal.h"
#include "task_presentation.h"

#include <cstring>

void Game_BeginScreenTransition(Game *game, ScreenTransitionAction action, int slotIndex) {
    if (game == NULL || action == SCREEN_TRANSITION_NONE || game->flow.screenTransitionActive) {
        return;
    }

    game->flow.screenTransitionActive = true;
    game->flow.screenTransitionResolved = false;
    game->flow.screenTransitionElapsed = 0.0f;
    game->flow.screenTransitionAction = action;
    game->flow.screenTransitionSlotIndex = slotIndex;
}

void Game_ApplyLanguage(Game *game, GameLanguage language) {
    GameLanguage normalized;

    if (game == NULL) {
        return;
    }

    normalized = Loc_NormalizeLanguage((int)language);
    game->flow.pendingLanguage = normalized;
    if (game->services.settings.language == normalized) {
        return;
    }

    game->services.settings.language = normalized;
    game->ui.settingsDirty = true;
    Loc_SetLanguage(normalized);
    Tasks_UpdateObjective(&game->runtime.tasks, &game->runtime.player);
}

void Game_BeginLanguageTransition(Game *game, GameLanguage language) {
    GameLanguage normalized;

    if (game == NULL) {
        return;
    }

    normalized = Loc_NormalizeLanguage((int)language);
    game->flow.pendingLanguage = normalized;
    if (game->flow.screenTransitionActive || game->services.settings.language == normalized) {
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
    return Map_IsWithinMapBounds(&game->runtime.map, gridX, gridY)
        && Map_IsWalkable(&game->runtime.map, gridX, gridY)
        && !Tasks_IsBlockingActorTile(&game->runtime.tasks, gridX, gridY);
}

bool Game_FindNearestSafeLoadedPlayerTile(const Game *game, int originX, int originY, int *gridX, int *gridY) {
    int radius;
    int maxRadius;

    if (IsSafeLoadedPlayerTile(game, originX, originY)) {
        *gridX = originX;
        *gridY = originY;
        return true;
    }

    maxRadius = game->runtime.map.width + game->runtime.map.height;
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
    game->ui.pauseMenuOpen = false;
    game->ui.settingsOpen = false;
    game->ui.settingsSliderDragging = false;
    game->ui.settingsSliderDragIndex = -1;
    game->ui.infoOverlayOpen = false;
    game->ui.infoOverlayTab = INFO_OVERLAY_TAB_MAP;
    game->ui.craftOpen = false;
    game->ui.communicatorLogDetailOpen = false;
    game->ui.communicatorLogDetailVisibility = 0.0f;
    game->ui.communicatorLogDetailScroll = 0.0f;
    game->ui.helpOpen = false;
    game->ui.logReaderOpen = false;
    game->ui.savePanelOpen = false;
    game->ui.showDeathPopup = false;
    game->ui.deathPopupSelection = DEATH_POPUP_BUTTON_RESTART;
    game->ui.settlementConfirmOpen = false;
    game->ui.settlementConfirmSelection = SETTLEMENT_CONFIRM_BUTTON_HEROIC;
    game->ui.endingRouteDoubleConfirmOpen = false;
    game->ui.endingRouteDoubleConfirmSelection = 0;
    game->ui.endingRoutePendingConfirm = ENDING_NONE;
    game->account.accountDeleteConfirmOpen = false;
    game->account.accountDeleteConfirmSelection = ACCOUNT_DELETE_CONFIRM_BUTTON_CANCEL;
    game->account.accountDeleteFromAuth = false;
    game->account.accountDeleteTargetName[0] = '\0';
}

void Game_ResetCameraToPlayer(Game *game) {
    game->runtime.camera.target = game->runtime.player.worldPos;
    game->runtime.camera.offset = Vector2{GetScreenWidth() * 0.5f, GetScreenHeight() * 0.5f};
    game->runtime.camera.rotation = 0.0f;
    game->runtime.camera.zoom = CAMERA_ZOOM;
}

void Game_ResetTransientGameplayState(Game *game) {
    game->input.bufferedMoveX = 0;
    game->input.bufferedMoveY = 0;
    game->input.inputBufferTimer = 0.0f;
    game->input.heldMoveX = 0;
    game->input.heldMoveY = 0;
    game->input.holdRepeatTimer = 0.0f;
    game->runtime.laserEffectTimer = 0.0f;
    game->runtime.laserEffectHit = false;
    game->runtime.laserEffectStart = Vector2{0.0f, 0.0f};
    game->runtime.laserEffectEnd = Vector2{0.0f, 0.0f};
    game->runtime.player.moveTimer = 0.0f;
    game->runtime.player.moveAnimElapsed = 0.0f;
    game->runtime.player.moveAnimDuration = 0.0f;
    game->runtime.player.safeRecoveryTimer = 0.0f;
}

static void EnterGameplayFromOpening(Game *game) {
    game->flow.state = GAME_STATE_PLAYING;
    game->flow.openingSlideIndex = 0;
    game->flow.openingCutsceneElapsed = 0.0f;
    game->flow.openingAwaitingFirstAdvance = false;
    game->flow.narrativeTransitionActive = false;
    game->flow.narrativeTransitionElapsed = 0.0f;
    game->flow.narrativeTransitionAction = NARRATIVE_TRANSITION_NONE;
    Game_CloseStoryScene(game);
    Audio_SetScene(&game->services.audio, Game_SelectAudioScene(game));
    Audio_SetMusicStage(&game->services.audio, Game_SelectMusicStage(game));
    Game_PostMessage(game, Loc_PickLiteral("Wake in Loxi's cabin. Sync the uplink, then restore the lower oxygen console.", "先在洛希舱室醒来并完成同步，再去修复下层氧气控制台。"), 4.2f);
}

static void RestartGameplayAfterDeath(Game *game) {
    Game_ResetGameplayWorld(game);
    Game_RefreshSaveSlots(game);
    MiniMap_Update(&game->runtime.miniMap, &game->runtime.player, &game->runtime.map);
    EnterGameplayFromOpening(game);
}

void Game_ResetGameplayWorld(Game *game) {
    GameMap initialMap = {};
    GameMap surfaceMap = {};

    MapSceneManager_Destroy(&game->services.mapScene);
    Map_Destroy(&game->runtime.map);
    Map_Init(&game->runtime.map);
    MapSceneManager_Init(&game->services.mapScene, &game->runtime.map, "maps/map_catalog.json");
    if (game->services.mapScene.catalog.loaded
        && Map_LoadById(&initialMap, &game->services.mapScene.catalog, "ship_interior")
        && Map_IsSceneReady(&initialMap)) {
        Map_Destroy(&game->runtime.map);
        game->runtime.map = initialMap;
        std::memset(&initialMap, 0, sizeof(initialMap));
        Map_ActivateRuntime(&game->runtime.map);
        MapSceneManager_BindActiveMap(&game->services.mapScene, &game->runtime.map);
    }

    Player_Init(&game->runtime.player);
    Tasks_Init(&game->runtime.tasks, &game->runtime.map);
    if (game->services.mapScene.catalog.loaded
        && std::strcmp(game->runtime.map.mapId, "ship_interior") == 0
        && Map_LoadById(&surfaceMap, &game->services.mapScene.catalog, "planet_surface_01")) {
        Tasks_RegisterMapEntities(&game->runtime.tasks, &surfaceMap);
    }
    Map_Destroy(&surfaceMap);
    Tasks_SetActiveMap(&game->runtime.tasks, &game->runtime.map);
    Tasks_UpdateObjective(&game->runtime.tasks, &game->runtime.player);
    MiniMap_Init(&game->runtime.miniMap, &game->runtime.map);
    game->runtime.elapsedSeconds = 0.0f;
    game->flow.openingCutsceneElapsed = 0.0f;
    game->flow.openingAwaitingFirstAdvance = false;
    game->flow.storySceneElapsed = 0.0f;
    game->flow.narrativeTransitionActive = false;
    game->flow.narrativeTransitionElapsed = 0.0f;
    game->flow.narrativeTransitionAction = NARRATIVE_TRANSITION_NONE;
    game->flow.screenTransitionActive = false;
    game->flow.screenTransitionResolved = false;
    game->flow.screenTransitionElapsed = 0.0f;
    game->flow.screenTransitionAction = SCREEN_TRANSITION_NONE;
    game->flow.screenTransitionSlotIndex = -1;
    game->runtime.hurtSoundCooldown = 0.0f;
    game->runtime.monsterCueCooldown = 0.0f;
    game->runtime.laserEffectTimer = 0.0f;
    game->runtime.laserEffectHit = false;
    game->runtime.laserEffectStart = Vector2{0.0f, 0.0f};
    game->runtime.laserEffectEnd = Vector2{0.0f, 0.0f};
    game->input.bufferedMoveX = 0;
    game->input.bufferedMoveY = 0;
    game->input.inputBufferTimer = 0.0f;
    game->input.heldMoveX = 0;
    game->input.heldMoveY = 0;
    game->input.holdRepeatTimer = 0.0f;
    Game_CloseTransientOverlays(game);
    game->ui.infoOverlayTab = INFO_OVERLAY_TAB_MAP;
    game->ui.communicatorTab = COMMUNICATOR_TAB_TASKS;
    game->ui.selectedBackpackItem = 0;
    game->ui.selectedCraftIndex = 0;
    game->ui.savePanelMode = SAVE_PANEL_MODE_LOAD;
    game->ui.selectedSaveSlot = 0;
    game->flow.openingSlideIndex = 0;
    game->flow.storySceneOpen = false;
    game->flow.storyScene = STORY_SCENE_NONE;
    std::memset(game->flow.storySceneShown, 0, sizeof(game->flow.storySceneShown));
    game->ui.selectedLogIndex = 0;
    game->ui.communicatorFirstVisibleLogIndex = 0;
    game->ui.selectedStorySceneIndex = 0;
    game->ui.communicatorFirstVisibleStorySceneIndex = 0;
    game->ui.communicatorLogDetailOpen = false;
    game->ui.communicatorLogDetailVisibility = 0.0f;
    game->ui.communicatorLogDetailScroll = 0.0f;
    game->flow.pendingLanguage = game->services.settings.language;
    game->flow.requestClose = false;
    game->runtime.lastLocationName[0] = '\0';
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
    MiniMap_Update(&game->runtime.miniMap, &game->runtime.player, &game->runtime.map);
    game->flow.state = GAME_STATE_OPENING;
    game->flow.openingSlideIndex = 0;
    game->flow.openingCutsceneElapsed = 0.0f;
    game->flow.openingAwaitingFirstAdvance = true;
    game->flow.narrativeTransitionActive = false;
    game->flow.narrativeTransitionElapsed = 0.0f;
    game->flow.narrativeTransitionAction = NARRATIVE_TRANSITION_NONE;
    Audio_SetScene(&game->services.audio, AUDIO_SCENE_MENU);
    Audio_SetMusicStage(&game->services.audio, Game_SelectMusicStage(game));
}

void Game_CompleteOpeningCutscene(Game *game) {
    if (game == NULL) {
        return;
    }

    EnterGameplayFromOpening(game);
}

void Game_HandlePlayerDeath(Game *game) {
    if (game->ui.showDeathPopup || game->flow.state != GAME_STATE_PLAYING || game->runtime.tasks.ending != ENDING_NONE) {
        return;
    }

    game->runtime.player.deathCount += 1;
    game->runtime.player.health = 0.0f;
    game->runtime.player.oxygen = ClampFloat(game->runtime.player.oxygen, 0.0f, MAX_OXYGEN);
    game->ui.showDeathPopup = true;
    game->ui.deathPopupSelection = DEATH_POPUP_BUTTON_RESTART;
    game->flow.screenTransitionActive = false;
    game->flow.screenTransitionAction = SCREEN_TRANSITION_NONE;
    Game_ResetTransientGameplayState(game);
    Game_RecordActiveAccountScore(game);
    Audio_PlayCue(&game->services.audio, AUDIO_CUE_WARNING);
}

void Game_HandleDeathRecovery(Game *game) {
    if (game == NULL) {
        return;
    }

    RestartGameplayAfterDeath(game);
}

void Game_ReturnToMenu(Game *game) {
    Game_TrySaveSettings(game);
    game->flow.state = GAME_STATE_INTRO;
    game->flow.openingSlideIndex = 0;
    game->flow.openingCutsceneElapsed = 0.0f;
    game->flow.openingAwaitingFirstAdvance = false;
    Game_CloseStoryScene(game);
    Game_CloseTransientOverlays(game);
    Game_ClearMessage(game);
    Game_ClearMessageHistory(game);
    Game_RefreshSaveSlots(game);
    Audio_SetScene(&game->services.audio, AUDIO_SCENE_MENU);
    Audio_SetMusicStage(&game->services.audio, AUDIO_MUSIC_MENU);
}
