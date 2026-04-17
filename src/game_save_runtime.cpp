#include "game_session_internal.h"
#include "task_runtime_internal.h"

#include <cmath>
#include <cstring>

static float ClampFloat(float value, float minValue, float maxValue) {
    if (value < minValue) {
        return minValue;
    }
    if (value > maxValue) {
        return maxValue;
    }
    return value;
}

static bool IsMonsterFootprintValid(const Monster *monster, const GameMap *map, int originX, int originY) {
    int offsetY;
    int offsetX;

    if (monster == NULL || map == NULL) {
        return false;
    }

    for (offsetY = 0; offsetY < MONSTER_FOOTPRINT_SIZE; offsetY++) {
        for (offsetX = 0; offsetX < MONSTER_FOOTPRINT_SIZE; offsetX++) {
            const int gridX = originX + offsetX;
            const int gridY = originY + offsetY;

            if (!Map_IsWithinBounds(gridX, gridY)) {
                return false;
            }
            if (Map_GetAreaAt(gridX, gridY) != monster->area) {
                return false;
            }
            if (!Map_IsWalkable(map, gridX, gridY)) {
                return false;
            }
        }
    }

    return true;
}

static void SanitizeLoadedTaskRuntimeState(Game *game) {
    int index;

    for (index = 0; index < game->tasks.nodeCount; index++) {
        if (game->tasks.nodes[index].respawnsRemaining < 0) {
            game->tasks.nodes[index].respawnsRemaining = 0;
        }
    }

    for (index = 0; index < game->tasks.monsterCount; index++) {
        Monster *monster;
        bool resetToSpawn;

        monster = &game->tasks.monsters[index];
        resetToSpawn = !IsMonsterFootprintValid(monster, &game->map, monster->gridX, monster->gridY);
        if (resetToSpawn && IsMonsterFootprintValid(monster, &game->map, monster->spawnX, monster->spawnY)) {
            monster->gridX = monster->spawnX;
            monster->gridY = monster->spawnY;
            resetToSpawn = false;
        }
        if (resetToSpawn) {
            monster->active = false;
        }

        if (!std::isfinite(monster->health)) {
            monster->health = 0.0f;
        }

        monster->health = ClampFloat(monster->health, 0.0f, monster->maxHealth);
        if (monster->health <= 0.0f) {
            monster->active = false;
        }
    }
}

static bool IsTrackedDynamicTile(int gridX, int gridY) {
    if (gridY == LOXI_ROOM_DOOR_Y
        && gridX >= LOXI_ROOM_DOOR_X
        && gridX < LOXI_ROOM_DOOR_X + LOXI_ROOM_DOOR_WIDTH) {
        return true;
    }

    if (gridX == AIRLOCK_DOOR_X && gridY >= AIRLOCK_DOOR_TOP_Y && gridY < AIRLOCK_DOOR_TOP_Y + AIRLOCK_DOOR_HEIGHT) {
        return true;
    }

    if ((gridX == SWAMP_GATE_X || gridX == SWAMP_GATE_X + 1)
        && gridY >= SWAMP_GATE_TOP_Y && gridY < SWAMP_GATE_TOP_Y + SWAMP_GATE_HEIGHT) {
        return true;
    }

    if ((gridX == RUINS_GATE_X || gridX == RUINS_GATE_X + 1)
        && gridY >= RUINS_GATE_TOP_Y && gridY < RUINS_GATE_TOP_Y + RUINS_GATE_HEIGHT) {
        return true;
    }

    return (gridX == ROPE_BARRIER_A_X && gridY == ROPE_BARRIER_A_Y)
        || (gridX == ROPE_BARRIER_B_X && gridY == ROPE_BARRIER_B_Y)
        || (gridX == ROPE_BARRIER_C_X && gridY == ROPE_BARRIER_C_Y);
}

static void CaptureClearedDynamicTiles(const GameMap *map, SaveSnapshot *snapshot) {
    int gridY;
    int gridX;

    snapshot->clearedDynamicTileCount = 0;
    for (gridY = 0; gridY < MAP_HEIGHT; gridY++) {
        for (gridX = 0; gridX < MAP_WIDTH; gridX++) {
            if (!IsTrackedDynamicTile(gridX, gridY)) {
                continue;
            }

            if (Map_GetPropTileAt(map, gridX, gridY) != TILE_VOID) {
                continue;
            }

            if (snapshot->clearedDynamicTileCount >= SAVE_DYNAMIC_TILE_MAX) {
                return;
            }

            snapshot->clearedDynamicTileX[snapshot->clearedDynamicTileCount] = gridX;
            snapshot->clearedDynamicTileY[snapshot->clearedDynamicTileCount] = gridY;
            snapshot->clearedDynamicTileCount += 1;
        }
    }
}

static void ApplySavedDynamicTiles(GameMap *map, const SaveSnapshot *snapshot) {
    int index;

    for (index = 0; index < snapshot->clearedDynamicTileCount; index++) {
        int gridX;
        int gridY;

        gridX = snapshot->clearedDynamicTileX[index];
        gridY = snapshot->clearedDynamicTileY[index];
        if (!Map_IsWithinBounds(gridX, gridY) || !IsTrackedDynamicTile(gridX, gridY)) {
            continue;
        }

        map->propTiles[gridY][gridX] = TILE_VOID;
    }
}

void Game_BuildSaveSnapshot(const Game *game, SaveSnapshot *snapshot) {
    int index;

    std::memset(snapshot, 0, sizeof(*snapshot));
    snapshot->gridX = game->player.gridX;
    snapshot->gridY = game->player.gridY;
    snapshot->facingX = game->player.facingX;
    snapshot->facingY = game->player.facingY;
    snapshot->health = game->player.health;
    snapshot->stamina = game->player.stamina;
    snapshot->pressure = INITIAL_PRESSURE;
    snapshot->oxygen = game->player.oxygen;
    snapshot->poison = game->player.poison;
    snapshot->maxHealthBonus = game->player.maxHealthBonus;
    snapshot->maxStaminaBonus = game->player.maxStaminaBonus;
    snapshot->attackBonus = game->player.attackBonus;
    snapshot->deathCount = game->player.deathCount;
    snapshot->crouching = game->player.crouching;
    snapshot->hasGlowStick = game->player.hasGlowStick;
    snapshot->hasRope = game->player.hasRope;
    snapshot->hasLaserGun = game->player.hasLaserGun;
    snapshot->hasProtectionSuit = game->player.hasProtectionSuit;
    snapshot->hasSignalAmplifier = game->player.hasSignalAmplifier;
    snapshot->hasFieldCamp = game->player.hasFieldCamp;
    for (index = 0; index < PLAYER_STATUS_COUNT; index++) {
        snapshot->statuses[index].active = game->player.statusEffects[index].active;
        snapshot->statuses[index].level = game->player.statusEffects[index].level;
        snapshot->statuses[index].remainingTime = game->player.statusEffects[index].remainingTime;
        snapshot->statuses[index].magnitude = game->player.statusEffects[index].magnitude;
    }
    for (index = 0; index < RESOURCE_COUNT; index++) {
        snapshot->resources[index] = game->player.resources[index];
    }

    snapshot->stage = game->tasks.stage;
    snapshot->dayCount = game->tasks.dayCount;
    snapshot->phase = game->tasks.phase;
    snapshot->currentEvent = game->tasks.currentEvent;
    snapshot->cycleTimer = game->tasks.cycleTimer;
    snapshot->elapsedSeconds = game->tasks.elapsedSeconds;
    snapshot->oxygenRepairLevel = game->tasks.oxygenRepairLevel;
    snapshot->commRepairLevel = game->tasks.commRepairLevel;
    snapshot->energyRepairLevel = game->tasks.energyRepairLevel;
    snapshot->crashClueFound = game->tasks.crashClueFound;
    snapshot->amplifierUnlocked = game->tasks.amplifierUnlocked;
    snapshot->bossDefeated = game->tasks.bossDefeated;
    snapshot->signalTowerActivated = game->tasks.signalTowerActivated;
    snapshot->selectedEndingRoute = game->tasks.selectedEndingRoute;
    snapshot->endingArchiveReviewed = game->tasks.endingArchiveReviewed;
    snapshot->westW1Started = game->tasks.westW1Started;
    snapshot->westW1Completed = game->tasks.westW1Completed;
    snapshot->westW2Started = game->tasks.westW2Started;
    snapshot->westW2Completed = game->tasks.westW2Completed;
    snapshot->westW3Started = game->tasks.westW3Started;
    snapshot->westW3Completed = game->tasks.westW3Completed;
    snapshot->westW4Started = game->tasks.westW4Started;
    snapshot->westW4Completed = game->tasks.westW4Completed;
    snapshot->westW5Started = game->tasks.westW5Started;
    snapshot->westW5Completed = game->tasks.westW5Completed;
    snapshot->southS1Started = game->tasks.southS1Started;
    snapshot->southS1Completed = game->tasks.southS1Completed;
    snapshot->southS2Started = game->tasks.southS2Started;
    snapshot->southS2Completed = game->tasks.southS2Completed;
    snapshot->southS3Started = game->tasks.southS3Started;
    snapshot->southS3Completed = game->tasks.southS3Completed;
    snapshot->southS4Started = game->tasks.southS4Started;
    snapshot->southS4Completed = game->tasks.southS4Completed;
    snapshot->southS5Started = game->tasks.southS5Started;
    snapshot->southS5Completed = game->tasks.southS5Completed;
    snapshot->monolithActivated[0] = game->tasks.monolithActivated[0];
    snapshot->monolithActivated[1] = game->tasks.monolithActivated[1];
    snapshot->monolithActivated[2] = game->tasks.monolithActivated[2];
    snapshot->monolithsLit = game->tasks.monolithsLit;
    snapshot->ending = game->tasks.ending;
    snapshot->campPlaced = game->map.campPlaced;
    snapshot->campX = game->map.campX;
    snapshot->campY = game->map.campY;
    snapshot->communicatorUnlocked = game->tasks.communicatorUnlocked;
    snapshot->clearedDynamicTileCount = 0;

    for (index = 0; index < game->tasks.nodeCount; index++) {
        snapshot->nodes[index].active = game->tasks.nodes[index].active;
        snapshot->nodes[index].respawnsRemaining = game->tasks.nodes[index].respawnsRemaining;
    }
    for (index = 0; index < game->tasks.monsterCount; index++) {
        snapshot->monsters[index].active = game->tasks.monsters[index].active;
        snapshot->monsters[index].gridX = game->tasks.monsters[index].gridX;
        snapshot->monsters[index].gridY = game->tasks.monsters[index].gridY;
        snapshot->monsters[index].health = game->tasks.monsters[index].health;
        snapshot->monsters[index].phaseTriggered = game->tasks.monsters[index].phaseTriggered;
    }
    for (index = 0; index < game->tasks.logCount; index++) {
        snapshot->logs[index].collected = game->tasks.logs[index].collected;
    }

    CaptureClearedDynamicTiles(&game->map, snapshot);
}

bool Game_LoadSnapshotIntoSession(Game *game, const SaveSnapshot *snapshot) {
    int index;
    bool positionAdjusted;
    int safeGridX;
    int safeGridY;

    Game_ResetGameplayWorld(game);

    game->player.gridX = snapshot->gridX;
    game->player.gridY = snapshot->gridY;
    game->player.facingX = snapshot->facingX;
    game->player.facingY = snapshot->facingY;
    Player_UpdateWorldPosition(&game->player);
    game->player.health = snapshot->health;
    game->player.stamina = snapshot->stamina;
    game->player.pressure = INITIAL_PRESSURE;
    game->player.oxygen = snapshot->oxygen;
    game->player.poison = snapshot->poison;
    game->player.maxHealthBonus = snapshot->maxHealthBonus;
    game->player.maxStaminaBonus = snapshot->maxStaminaBonus;
    game->player.attackBonus = snapshot->attackBonus;
    game->player.deathCount = snapshot->deathCount;
    game->player.crouching = snapshot->crouching;
    game->player.hasGlowStick = snapshot->hasGlowStick;
    game->player.hasRope = snapshot->hasRope;
    game->player.hasLaserGun = snapshot->hasLaserGun;
    game->player.hasProtectionSuit = snapshot->hasProtectionSuit;
    game->player.hasSignalAmplifier = snapshot->hasSignalAmplifier;
    game->player.hasFieldCamp = snapshot->hasFieldCamp;
    for (index = 0; index < PLAYER_STATUS_COUNT; index++) {
        game->player.statusEffects[index].active = snapshot->statuses[index].active;
        game->player.statusEffects[index].level = snapshot->statuses[index].level;
        game->player.statusEffects[index].remainingTime = snapshot->statuses[index].remainingTime;
        game->player.statusEffects[index].magnitude = snapshot->statuses[index].magnitude;
    }
    for (index = 0; index < RESOURCE_COUNT; index++) {
        game->player.resources[index] = snapshot->resources[index];
    }

    game->tasks.stage = snapshot->stage;
    game->tasks.dayCount = snapshot->dayCount;
    game->tasks.phase = (DayPhase)snapshot->phase;
    game->tasks.currentEvent = (EventType)snapshot->currentEvent;
    game->tasks.cycleTimer = snapshot->cycleTimer;
    game->tasks.elapsedSeconds = snapshot->elapsedSeconds;
    game->tasks.oxygenRepairLevel = snapshot->oxygenRepairLevel;
    game->tasks.commRepairLevel = snapshot->commRepairLevel;
    game->tasks.energyRepairLevel = snapshot->energyRepairLevel;
    game->tasks.crashClueFound = snapshot->crashClueFound;
    game->tasks.amplifierUnlocked = snapshot->amplifierUnlocked;
    game->tasks.communicatorUnlocked = snapshot->communicatorUnlocked;
    game->tasks.bossDefeated = snapshot->bossDefeated;
    game->tasks.signalTowerActivated = snapshot->signalTowerActivated;
    game->tasks.selectedEndingRoute = (GameEnding)snapshot->selectedEndingRoute;
    game->tasks.endingArchiveReviewed = snapshot->endingArchiveReviewed;
    game->tasks.westW1Started = snapshot->westW1Started;
    game->tasks.westW1Completed = snapshot->westW1Completed;
    game->tasks.westW2Started = snapshot->westW2Started;
    game->tasks.westW2Completed = snapshot->westW2Completed;
    game->tasks.westW3Started = snapshot->westW3Started;
    game->tasks.westW3Completed = snapshot->westW3Completed;
    game->tasks.westW4Started = snapshot->westW4Started;
    game->tasks.westW4Completed = snapshot->westW4Completed;
    game->tasks.westW5Started = snapshot->westW5Started;
    game->tasks.westW5Completed = snapshot->westW5Completed;
    game->tasks.southS1Started = snapshot->southS1Started;
    game->tasks.southS1Completed = snapshot->southS1Completed;
    game->tasks.southS2Started = snapshot->southS2Started;
    game->tasks.southS2Completed = snapshot->southS2Completed;
    game->tasks.southS3Started = snapshot->southS3Started;
    game->tasks.southS3Completed = snapshot->southS3Completed;
    game->tasks.southS4Started = snapshot->southS4Started;
    game->tasks.southS4Completed = snapshot->southS4Completed;
    game->tasks.southS5Started = snapshot->southS5Started;
    game->tasks.southS5Completed = snapshot->southS5Completed;
    game->tasks.monolithActivated[0] = snapshot->monolithActivated[0];
    game->tasks.monolithActivated[1] = snapshot->monolithActivated[1];
    game->tasks.monolithActivated[2] = snapshot->monolithActivated[2];
    game->tasks.monolithsLit = snapshot->monolithsLit;
    game->tasks.ending = (GameEnding)snapshot->ending;
    for (index = 0; index < game->tasks.nodeCount; index++) {
        game->tasks.nodes[index].active = snapshot->nodes[index].active;
        game->tasks.nodes[index].respawnsRemaining = snapshot->nodes[index].respawnsRemaining;
    }
    for (index = 0; index < game->tasks.monsterCount; index++) {
        game->tasks.monsters[index].active = snapshot->monsters[index].active;
        game->tasks.monsters[index].gridX = snapshot->monsters[index].gridX;
        game->tasks.monsters[index].gridY = snapshot->monsters[index].gridY;
        game->tasks.monsters[index].health = snapshot->monsters[index].health;
        game->tasks.monsters[index].phaseTriggered = snapshot->monsters[index].phaseTriggered;
    }
    for (index = 0; index < game->tasks.logCount; index++) {
        game->tasks.logs[index].collected = snapshot->logs[index].collected;
    }
    SanitizeLoadedTaskRuntimeState(game);

    if (game->tasks.stage >= 5) {
        Map_UnlockSwampDeep(&game->map);
        TasksRuntime_EnsureEnergyCoreNode(&game->tasks, &game->map, &game->player);
    }
    if (game->tasks.stage >= 6) {
        Map_UnlockRuins(&game->map);
    }
    if (snapshot->campPlaced) {
        Map_SetFieldCamp(&game->map, snapshot->campX, snapshot->campY);
    }
    if (game->tasks.communicatorUnlocked) {
        Map_UnlockLoxiRoom(&game->map);
    }
    ApplySavedDynamicTiles(&game->map, snapshot);
    if (game->tasks.selectedEndingRoute == ENDING_HEROIC && !game->tasks.bossDefeated) {
        Map_LockSwampOuter(&game->map);
    }

    positionAdjusted = false;
    safeGridX = game->player.gridX;
    safeGridY = game->player.gridY;
    if (!Game_FindNearestSafeLoadedPlayerTile(game, game->player.gridX, game->player.gridY, &safeGridX, &safeGridY)
        && Game_FindNearestSafeLoadedPlayerTile(game, PLAYER_RESPAWN_X, PLAYER_RESPAWN_Y, &safeGridX, &safeGridY)) {
        positionAdjusted = true;
    } else if (safeGridX != game->player.gridX || safeGridY != game->player.gridY) {
        positionAdjusted = true;
    }

    if (positionAdjusted) {
        game->player.gridX = safeGridX;
        game->player.gridY = safeGridY;
        Player_UpdateWorldPosition(&game->player);
    }

    MiniMap_Update(&game->miniMap, &game->player, &game->map);
    Tasks_UpdateObjective(&game->tasks, &game->player);
    Game_SyncTrackedLocation(game);
    game->state = game->tasks.ending == ENDING_NONE ? GAME_STATE_PLAYING : GAME_STATE_ENDING;
    game->openingSlideIndex = 0;
    game->openingCutsceneElapsed = 0.0f;
    Game_CloseStoryScene(game);
    Game_CloseTransientOverlays(game);
    Game_ResetCameraToPlayer(game);
    Audio_SetScene(&game->audio, Game_SelectAudioScene(game));

    return positionAdjusted;
}
