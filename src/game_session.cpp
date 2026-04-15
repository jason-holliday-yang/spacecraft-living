#include "game_session_internal.h"

#include <cstring>

static const int DEATH_FAILURE_THRESHOLD = 3;
static const float DEATH_RECOVERY_HEALTH_FACTOR = 0.55f;
static const float DEATH_RECOVERY_STAMINA_FACTOR = 0.55f;
static const float DEATH_RECOVERY_OXYGEN = 62.0f;
static const float DEATH_RECOVERY_POISON_FACTOR = 0.35f;

static int AbsInt(int value) {
    return value < 0 ? -value : value;
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

static bool FindDeathRecoveryTile(const Game *game, int *gridX, int *gridY, bool *usedCamp) {
    if (gridX == NULL || gridY == NULL) {
        return false;
    }

    if (usedCamp != NULL) {
        *usedCamp = false;
    }

    if (Map_GetAreaAt(game->player.gridX, game->player.gridY) == MAP_AREA_BOSS_ARENA
        && game->tasks.selectedEndingRoute == ENDING_HEROIC
        && !game->tasks.bossDefeated) {
        if (Game_FindNearestSafeLoadedPlayerTile(game, PLAYER_START_X, PLAYER_START_Y, gridX, gridY)) {
            return true;
        }
    }

    /* Recover at the field camp first, then fall back to the original base spawn. */
    if (game->map.campPlaced
        && Game_FindNearestSafeLoadedPlayerTile(game, game->map.campX, game->map.campY, gridX, gridY)) {
        if (usedCamp != NULL) {
            *usedCamp = true;
        }
        return true;
    }

    if (Game_FindNearestSafeLoadedPlayerTile(game, PLAYER_START_X, PLAYER_START_Y, gridX, gridY)) {
        return true;
    }

    *gridX = game->player.gridX;
    *gridY = game->player.gridY;
    return true;
}

void Game_CloseTransientOverlays(Game *game) {
    game->pauseMenuOpen = false;
    game->settingsOpen = false;
    game->settingsSliderDragging = false;
    game->backpackOpen = false;
    game->craftOpen = false;
    game->mapOpen = false;
    game->communicatorOpen = false;
    game->helpOpen = false;
    game->logReaderOpen = false;
    game->savePanelOpen = false;
    game->showDeathPopup = false;
    game->deathPopupSelection = DEATH_POPUP_BUTTON_RESTART;
    game->settlementConfirmOpen = false;
    game->settlementConfirmSelection = SETTLEMENT_CONFIRM_BUTTON_PEACEFUL;
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
    game->player.moveTimer = 0.0f;
    game->player.moveAnimElapsed = 0.0f;
    game->player.moveAnimDuration = 0.0f;
    game->player.safeRecoveryTimer = 0.0f;
}

static void EnterGameplayFromOpening(Game *game) {
    game->state = GAME_STATE_PLAYING;
    game->openingSlideIndex = 0;
    game->openingCutsceneElapsed = 0.0f;
    game->narrativeTransitionActive = false;
    game->narrativeTransitionElapsed = 0.0f;
    game->narrativeTransitionAction = NARRATIVE_TRANSITION_NONE;
    Game_CloseStoryScene(game);
    Audio_SetScene(&game->audio, AUDIO_SCENE_BASE);
    Game_PostMessage(game, Loc_PickLiteral("Start in the lower oxygen bay: restore the oxygen console. The terminal bay can sync Loxi when you need the link.", "从下层氧气舱开始：先修复氧气控制台。需要时可以去终端舱与洛希同步。"), 4.2f);
}

void Game_ResetGameplayWorld(Game *game) {
    Map_Init(&game->map);
    Player_Init(&game->player);
    Tasks_Init(&game->tasks, &game->map);
    Tasks_UpdateObjective(&game->tasks, &game->player);
    MiniMap_Init(&game->miniMap);
    game->elapsedSeconds = 0.0f;
    game->openingCutsceneElapsed = 0.0f;
    game->storySceneElapsed = 0.0f;
    game->narrativeTransitionActive = false;
    game->narrativeTransitionElapsed = 0.0f;
    game->narrativeTransitionAction = NARRATIVE_TRANSITION_NONE;
    game->hurtSoundCooldown = 0.0f;
    game->monsterCueCooldown = 0.0f;
    game->bufferedMoveX = 0;
    game->bufferedMoveY = 0;
    game->inputBufferTimer = 0.0f;
    game->heldMoveX = 0;
    game->heldMoveY = 0;
    game->holdRepeatTimer = 0.0f;
    Game_CloseTransientOverlays(game);
    game->selectedBackpackItem = 0;
    game->selectedCraftIndex = 0;
    game->savePanelMode = SAVE_PANEL_MODE_LOAD;
    game->selectedSaveSlot = 0;
    game->openingSlideIndex = 0;
    game->storySceneOpen = false;
    game->storyScene = STORY_SCENE_NONE;
    std::memset(game->storySceneShown, 0, sizeof(game->storySceneShown));
    game->selectedLogIndex = 0;
    game->requestClose = false;
    game->lastLocationName[0] = '\0';
    Game_SyncTrackedLocation(game);
    Game_ResetCameraToPlayer(game);
    Game_ClearMessage(game);
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
    game->narrativeTransitionActive = false;
    game->narrativeTransitionElapsed = 0.0f;
    game->narrativeTransitionAction = NARRATIVE_TRANSITION_NONE;
    Audio_SetScene(&game->audio, AUDIO_SCENE_MENU);
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
    game->showDeathPopup = false;
    game->deathPopupSelection = DEATH_POPUP_BUTTON_RESTART;
    Game_ResetTransientGameplayState(game);

    if (game->player.deathCount >= DEATH_FAILURE_THRESHOLD) {
        game->tasks.ending = ENDING_FAILURE;
        game->state = GAME_STATE_ENDING;
        Audio_SetScene(&game->audio, AUDIO_SCENE_ENDING);
        Audio_PlayCue(&game->audio, AUDIO_CUE_WARNING);
        return;
    }

    game->showDeathPopup = true;
    Audio_PlayCue(&game->audio, AUDIO_CUE_WARNING);
}

void Game_HandleDeathRecovery(Game *game) {
    int safeGridX;
    int safeGridY;
    float staminaCap;
    int remainingLives;
    const char *message;
    bool usedCampRecovery;

    if (game->tasks.ending != ENDING_NONE) {
        return;
    }

    safeGridX = game->player.gridX;
    safeGridY = game->player.gridY;
    usedCampRecovery = false;
    FindDeathRecoveryTile(game, &safeGridX, &safeGridY, &usedCampRecovery);

    game->player.gridX = safeGridX;
    game->player.gridY = safeGridY;
    game->player.facingX = 0;
    game->player.facingY = 1;
    Player_UpdateWorldPosition(&game->player);

    game->player.health = ClampFloat(Player_GetMaxHealth(&game->player) * DEATH_RECOVERY_HEALTH_FACTOR,
                                     22.0f,
                                     Player_GetMaxHealth(&game->player));
    staminaCap = Player_GetCurrentStaminaCap(&game->player);
    game->player.stamina = ClampFloat(Player_GetMaxStamina(&game->player) * DEATH_RECOVERY_STAMINA_FACTOR, 18.0f, staminaCap);
    game->player.oxygen = ClampFloat(DEATH_RECOVERY_OXYGEN, 0.0f, MAX_OXYGEN);
    game->player.pressure = INITIAL_PRESSURE;
    game->player.poison = ClampFloat(game->player.poison * DEATH_RECOVERY_POISON_FACTOR, 0.0f, MAX_POISON);
    Game_ResetTransientGameplayState(game);
    Game_TrySaveSettings(game);

    Game_CloseTransientOverlays(game);
    game->state = GAME_STATE_PLAYING;
    game->openingSlideIndex = 0;
    game->openingCutsceneElapsed = 0.0f;
    Game_CloseStoryScene(game);
    Game_ResetCameraToPlayer(game);
    MiniMap_Update(&game->miniMap, &game->player, &game->map);
    Tasks_UpdateObjective(&game->tasks, &game->player);
    Game_SyncTrackedLocation(game);
    Audio_SetScene(&game->audio, Game_SelectAudioScene(game));

    remainingLives = DEATH_FAILURE_THRESHOLD - game->player.deathCount;
    if (usedCampRecovery && remainingLives <= 1) {
        message = Loc_PickLiteral("Emergency recovery returned you to the field camp. Progress is intact, but one more collapse ends the run.", "紧急回收把你送回了野外营地。进度仍在，但再倒下一次就会结束本次生存。");
    } else if (usedCampRecovery) {
        message = Loc_PickLiteral("Emergency recovery returned you to the field camp. Progress is intact, but repeated collapses will end the run.", "紧急回收把你送回了野外营地。进度仍在，但持续崩溃会终结本次生存。");
    } else if (remainingLives <= 1) {
        message = Loc_PickLiteral("Emergency recovery returned you to base. Progress is intact, but one more collapse ends the run.", "紧急回收把你送回了基地。进度仍在，但再倒下一次就会结束本次生存。");
    } else {
        message = Loc_PickLiteral("Emergency recovery returned you to base. Progress is intact, but repeated collapses will end the run.", "紧急回收把你送回了基地。进度仍在，但持续崩溃会终结本次生存。");
    }
    Game_PostMessage(game, message, 4.2f);
}

void Game_ReturnToMenu(Game *game) {
    Game_TrySaveSettings(game);
    game->state = GAME_STATE_INTRO;
    game->openingSlideIndex = 0;
    game->openingCutsceneElapsed = 0.0f;
    Game_CloseStoryScene(game);
    Game_CloseTransientOverlays(game);
    Game_ClearMessage(game);
    Game_RefreshSaveSlots(game);
    Audio_SetScene(&game->audio, AUDIO_SCENE_MENU);
}
