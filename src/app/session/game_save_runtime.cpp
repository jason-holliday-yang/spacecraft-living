#include "game_session_internal.h"
#include "task_progression_internal.h"
#include "task_presentation.h"
#include "task_content.h"

#include <cmath>
#include <cstdio>
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

static int CountShownMainStoryScenes(const bool *shownScenes) {
    int count = 0;

    if (shownScenes == NULL) {
        return 0;
    }

    for (int index = 0; index < STORY_MAIN_SCENE_COUNT; index++) {
        if (shownScenes[index]) {
            count += 1;
        }
    }

    return count;
}

static float GetLegacyMonsterMaxHealth(MonsterType type) {
    switch (type) {
        case MONSTER_THORN_LARVA:
            return 28.0f;
        case MONSTER_WING_BUG:
            return 18.0f;
        case MONSTER_RAPTOR:
            return 52.0f;
        case MONSTER_SWAMP_STALKER:
            return 48.0f;
        case MONSTER_SENTINEL_JELLY:
            return 72.0f;
        case MONSTER_FOG_WORM:
            return 64.0f;
        case MONSTER_RELIC_GUARD:
            return 92.0f;
        case MONSTER_FINAL_BOSS:
            return 220.0f;
        default:
            return 0.0f;
    }
}

static float MigrateFullHealthMonsterValue(MonsterType type, float savedHealth, float currentMaxHealth) {
    const float legacyMaxHealth = GetLegacyMonsterMaxHealth(type);

    if (legacyMaxHealth > 0.0f
        && currentMaxHealth > legacyMaxHealth
        && std::fabs(savedHealth - legacyMaxHealth) <= 0.01f) {
        return currentMaxHealth;
    }
    return savedHealth;
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

            if (!Map_IsWithinMapBounds(map, gridX, gridY)) {
                return false;
            }
            if (Map_GetAreaAtMap(map, gridX, gridY) != monster->area) {
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

    for (index = 0; index < game->runtime.tasks.nodeCount; index++) {
        if (game->runtime.tasks.nodes[index].respawnsRemaining < 0) {
            game->runtime.tasks.nodes[index].respawnsRemaining = 0;
        }
    }

    for (index = 0; index < game->runtime.tasks.monsterCount; index++) {
        Monster *monster;
        bool resetToSpawn;

        monster = &game->runtime.tasks.monsters[index];
        resetToSpawn = !IsMonsterFootprintValid(monster, &game->runtime.map, monster->gridX, monster->gridY);
        if (resetToSpawn && IsMonsterFootprintValid(monster, &game->runtime.map, monster->spawnX, monster->spawnY)) {
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

static bool IsLegacyTrackedDynamicTile(int gridX, int gridY) {
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

static bool IsTrackedDynamicTile(const GameMap *map, int gridX, int gridY) {
    if (Map_GetUnlockAt(map, gridX, gridY) != nullptr) {
        return true;
    }
    return IsLegacyTrackedDynamicTile(gridX, gridY);
}

static void CaptureClearedDynamicTiles(const GameMap *map, SaveSnapshot *snapshot) {
    int gridY;
    int gridX;

    snapshot->clearedDynamicTileCount = 0;
    for (gridY = 0; gridY < map->height; gridY++) {
        for (gridX = 0; gridX < map->width; gridX++) {
            if (!IsTrackedDynamicTile(map, gridX, gridY)) {
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
        if (!Map_IsWithinMapBounds(map, gridX, gridY) || !IsTrackedDynamicTile(map, gridX, gridY)) {
            continue;
        }

        map->propTiles[gridY][gridX] = TILE_VOID;
    }
}

static SavedMapStateSnapshot *EnsureSavedMapState(SaveSnapshot *snapshot, const char *mapId) {
    int index;
    if (snapshot == nullptr || mapId == nullptr || mapId[0] == '\0') return nullptr;
    for (index = 0; index < snapshot->mapStateCount; index++) {
        if (std::strcmp(snapshot->mapStates[index].mapId, mapId) == 0) return &snapshot->mapStates[index];
    }
    if (snapshot->mapStateCount >= SAVE_MAP_STATE_MAX) return nullptr;
    index = snapshot->mapStateCount++;
    std::memset(&snapshot->mapStates[index], 0, sizeof(snapshot->mapStates[index]));
    std::snprintf(snapshot->mapStates[index].mapId, MAP_ID_MAX, "%s", mapId);
    return &snapshot->mapStates[index];
}

static const SavedMapStateSnapshot *FindSavedMapState(const SaveSnapshot *snapshot, const char *mapId) {
    int index;
    if (snapshot == nullptr || mapId == nullptr) return nullptr;
    for (index = 0; index < snapshot->mapStateCount; index++) {
        if (std::strcmp(snapshot->mapStates[index].mapId, mapId) == 0) return &snapshot->mapStates[index];
    }
    return nullptr;
}

static void CopyRuntimeMapStateToSave(const MapRuntimeState *runtime, SavedMapStateSnapshot *saved) {
    int index;
    if (runtime == nullptr || saved == nullptr) return;
    saved->contentVersion = runtime->contentVersion;
    saved->mapWidth = runtime->mapWidth;
    saved->mapHeight = runtime->mapHeight;
    saved->unlockCount = runtime->unlockCount;
    for (index = 0; index < saved->unlockCount; index++) {
        std::snprintf(saved->unlocks[index].unlockId, MAP_UNLOCK_ID_MAX, "%s", runtime->unlocks[index].unlockId);
        saved->unlocks[index].open = runtime->unlocks[index].open;
    }
    saved->clearedDynamicTileCount = runtime->clearedDynamicTileCount;
    for (index = 0; index < saved->clearedDynamicTileCount; index++) {
        saved->clearedDynamicTileX[index] = runtime->clearedDynamicTileX[index];
        saved->clearedDynamicTileY[index] = runtime->clearedDynamicTileY[index];
    }
    saved->campPlaced = runtime->campPlaced;
    saved->campX = runtime->campX;
    saved->campY = runtime->campY;
}

static void CaptureActiveMapStateToSave(const GameMap *map, SavedMapStateSnapshot *saved) {
    int index;
    SaveSnapshot temporary{};
    if (map == nullptr || saved == nullptr) return;
    saved->contentVersion = map->contentVersion;
    saved->mapWidth = map->width;
    saved->mapHeight = map->height;
    saved->unlockCount = map->unlockCount;
    for (index = 0; index < saved->unlockCount; index++) {
        std::snprintf(saved->unlocks[index].unlockId, MAP_UNLOCK_ID_MAX, "%s", map->unlocks[index].id);
        saved->unlocks[index].open = Map_IsUnlockOpen(map, map->unlocks[index].id);
    }
    CaptureClearedDynamicTiles(map, &temporary);
    saved->clearedDynamicTileCount = temporary.clearedDynamicTileCount;
    for (index = 0; index < saved->clearedDynamicTileCount; index++) {
        saved->clearedDynamicTileX[index] = temporary.clearedDynamicTileX[index];
        saved->clearedDynamicTileY[index] = temporary.clearedDynamicTileY[index];
    }
    saved->campPlaced = map->campPlaced;
    saved->campX = map->campX;
    saved->campY = map->campY;
}

static const char *ResolveSnapshotMapId(Game *game, const SaveSnapshot *snapshot) {
    static const char *shipMapId = "ship_interior";
    static const char *surfaceMapId = "planet_surface_01";
    GameMap legacyMap{};
    MapArea legacyArea = MAP_AREA_UNKNOWN;

    if (snapshot->sourceVersion >= 16 && snapshot->currentMapId[0] != '\0'
        && MapCatalog_Find(&game->services.mapScene.catalog, snapshot->currentMapId) != nullptr) {
        return snapshot->currentMapId;
    }
    if (Map_LoadById(&legacyMap, &game->services.mapScene.catalog, "legacy_world")) {
        legacyArea = Map_GetAreaAtMap(&legacyMap, snapshot->gridX, snapshot->gridY);
    }
    Map_Destroy(&legacyMap);
    if (legacyArea == MAP_AREA_BASE) return shipMapId;
    if (MapCatalog_Find(&game->services.mapScene.catalog, surfaceMapId) != nullptr) return surfaceMapId;
    return "planet_surface_legacy";
}

static bool LoadSnapshotMap(Game *game, const char *mapId) {
    GameMap loaded{};
    if (game == nullptr || mapId == nullptr) return false;
    if (std::strcmp(game->runtime.map.mapId, mapId) == 0) return true;
    if (!Map_LoadById(&loaded, &game->services.mapScene.catalog, mapId)
        || (std::strcmp(mapId, "legacy_world") == 0 ? !Map_IsProductionReady(&loaded) : !Map_IsSceneReady(&loaded))) {
        Map_Destroy(&loaded);
        return false;
    }
    MapSceneManager_ApplyMapState(&game->services.mapScene, &loaded);
    if (std::strcmp(mapId, "legacy_world") == 0) {
        Tasks_Init(&game->runtime.tasks, &loaded);
    } else if (!Tasks_RegisterMapEntities(&game->runtime.tasks, &loaded)) {
        Map_Destroy(&loaded);
        return false;
    }
    Map_Destroy(&game->runtime.map);
    game->runtime.map = loaded;
    std::memset(&loaded, 0, sizeof(loaded));
    Map_ActivateRuntime(&game->runtime.map);
    MapSceneManager_BindActiveMap(&game->services.mapScene, &game->runtime.map);
    Tasks_SetActiveMap(&game->runtime.tasks, &game->runtime.map);
    MiniMap_Init(&game->runtime.miniMap, &game->runtime.map);
    return true;
}

static void ImportSavedMapRuntimeStates(Game *game, const SaveSnapshot *snapshot) {
    int mapIndex;
    game->services.mapScene.runtimeStateCount = 0;
    for (mapIndex = 0; mapIndex < snapshot->mapStateCount; mapIndex++) {
        const SavedMapStateSnapshot *saved = &snapshot->mapStates[mapIndex];
        MapRuntimeState *runtime = MapSceneManager_GetMapState(&game->services.mapScene, saved->mapId, true);
        int index;
        if (runtime == nullptr) continue;
        runtime->contentVersion = saved->contentVersion;
        runtime->mapWidth = saved->mapWidth;
        runtime->mapHeight = saved->mapHeight;
        runtime->unlockCount = saved->unlockCount;
        for (index = 0; index < runtime->unlockCount; index++) {
            std::snprintf(runtime->unlocks[index].unlockId, MAP_UNLOCK_ID_MAX, "%s", saved->unlocks[index].unlockId);
            runtime->unlocks[index].open = saved->unlocks[index].open;
        }
        runtime->clearedDynamicTileCount = saved->clearedDynamicTileCount;
        for (index = 0; index < runtime->clearedDynamicTileCount; index++) {
            runtime->clearedDynamicTileX[index] = saved->clearedDynamicTileX[index];
            runtime->clearedDynamicTileY[index] = saved->clearedDynamicTileY[index];
        }
        runtime->campPlaced = saved->campPlaced;
        runtime->campX = saved->campX;
        runtime->campY = saved->campY;
    }
}

static void ApplyV16EntityStates(Game *game, const SaveSnapshot *snapshot) {
    int mapIndex;
    for (mapIndex = 0; mapIndex < snapshot->mapStateCount; mapIndex++) {
        const SavedMapStateSnapshot *state = &snapshot->mapStates[mapIndex];
        int nodeLocal = 0;
        int monsterLocal = 0;
        int logLocal = 0;
        int index;
        for (index = 0; index < game->runtime.tasks.nodeCount; index++) {
            ResourceNode *node = &game->runtime.tasks.nodes[index];
            if (std::strcmp(node->mapId, state->mapId) != 0) continue;
            if (nodeLocal < state->nodeCount) {
                node->active = state->nodes[nodeLocal].active;
                node->respawnsRemaining = state->nodes[nodeLocal].respawnsRemaining;
            }
            nodeLocal++;
        }
        for (index = 0; index < game->runtime.tasks.monsterCount; index++) {
            Monster *monster = &game->runtime.tasks.monsters[index];
            if (std::strcmp(monster->mapId, state->mapId) != 0) continue;
            if (monsterLocal < state->monsterCount) {
                const SavedMonsterSnapshot *saved = &state->monsters[monsterLocal];
                monster->active = saved->active;
                monster->gridX = saved->gridX;
                monster->gridY = saved->gridY;
                monster->health = MigrateFullHealthMonsterValue(monster->type, saved->health, monster->maxHealth);
                monster->phaseTriggered = saved->phaseTriggered;
            }
            monsterLocal++;
        }
        for (index = 0; index < game->runtime.tasks.logCount; index++) {
            ShipLog *log = &game->runtime.tasks.logs[index];
            if (std::strcmp(log->mapId, state->mapId) != 0) continue;
            if (logLocal < state->logCount) log->collected = state->logs[logLocal].collected;
            logLocal++;
        }
    }
}

void Game_BuildSaveSnapshot(const Game *game, SaveSnapshot *snapshot) {
    int index;

    std::memset(snapshot, 0, sizeof(*snapshot));
    snapshot->sourceVersion = 16;
    std::snprintf(snapshot->currentMapId, MAP_ID_MAX, "%s", game->runtime.map.mapId);
    snapshot->gridX = game->runtime.player.gridX;
    snapshot->gridY = game->runtime.player.gridY;
    snapshot->facingX = game->runtime.player.facingX;
    snapshot->facingY = game->runtime.player.facingY;
    snapshot->mapWidth = game->runtime.map.width;
    snapshot->mapHeight = game->runtime.map.height;
    snapshot->health = game->runtime.player.health;
    snapshot->stamina = game->runtime.player.health;
    snapshot->pressure = LEGACY_INITIAL_PRESSURE;
    snapshot->oxygen = game->runtime.player.oxygen;
    snapshot->poison = game->runtime.player.poison;
    snapshot->maxHealthBonus = game->runtime.player.maxHealthBonus;
    snapshot->maxStaminaBonus = game->runtime.player.maxHealthBonus;
    snapshot->attackBonus = 0.0f;
    snapshot->deathCount = game->runtime.player.deathCount;
    snapshot->hasGlowStick = game->runtime.player.hasGlowStick;
    snapshot->hasRope = game->runtime.player.hasRope;
    snapshot->hasLaserGun = game->runtime.player.hasLaserGun;
    snapshot->hasProtectionSuit = game->runtime.player.hasProtectionSuit;
    snapshot->hasSignalAmplifier = game->runtime.player.hasSignalAmplifier;
    snapshot->hasFieldCamp = game->runtime.player.hasFieldCamp;
    for (index = 0; index < PLAYER_STATUS_COUNT; index++) {
        snapshot->statuses[index].active = game->runtime.player.statusEffects[index].active;
        snapshot->statuses[index].level = game->runtime.player.statusEffects[index].level;
        snapshot->statuses[index].remainingTime = game->runtime.player.statusEffects[index].remainingTime;
        snapshot->statuses[index].magnitude = game->runtime.player.statusEffects[index].magnitude;
    }
    for (index = 0; index < RESOURCE_COUNT; index++) {
        snapshot->resources[index] = game->runtime.player.resources[index];
    }

    snapshot->stage = game->runtime.tasks.stage;
    snapshot->dayCount = game->runtime.tasks.dayCount;
    snapshot->phase = game->runtime.tasks.phase;
    snapshot->currentEvent = game->runtime.tasks.currentEvent;
    snapshot->cycleTimer = game->runtime.tasks.cycleTimer;
    snapshot->elapsedSeconds = game->runtime.tasks.elapsedSeconds;
    snapshot->oxygenRepairLevel = game->runtime.tasks.oxygenRepairLevel;
    snapshot->commRepairLevel = game->runtime.tasks.commRepairLevel;
    snapshot->energyRepairLevel = game->runtime.tasks.energyRepairLevel;
    snapshot->crashClueFound = game->runtime.tasks.crashClueFound;
    snapshot->amplifierUnlocked = game->runtime.tasks.amplifierUnlocked;
    snapshot->bossDefeated = game->runtime.tasks.bossDefeated;
    snapshot->signalTowerActivated = game->runtime.tasks.signalTowerActivated;
    snapshot->selectedEndingRoute = game->runtime.tasks.selectedEndingRoute;
    snapshot->endingArchiveReviewed = game->runtime.tasks.endingArchiveReviewed;
    snapshot->westW1Started = game->runtime.tasks.westW1Started;
    snapshot->westW1Completed = game->runtime.tasks.westW1Completed;
    snapshot->westW2Started = game->runtime.tasks.westW2Started;
    snapshot->westW2Completed = game->runtime.tasks.westW2Completed;
    snapshot->westW3Started = game->runtime.tasks.westW3Started;
    snapshot->westW3Completed = game->runtime.tasks.westW3Completed;
    snapshot->westW4Started = game->runtime.tasks.westW4Started;
    snapshot->westW4Completed = game->runtime.tasks.westW4Completed;
    snapshot->westW5Started = game->runtime.tasks.westW5Started;
    snapshot->westW5Completed = game->runtime.tasks.westW5Completed;
    snapshot->southS1Started = game->runtime.tasks.southS1Started;
    snapshot->southS1Completed = game->runtime.tasks.southS1Completed;
    snapshot->southS2Started = game->runtime.tasks.southS2Started;
    snapshot->southS2Completed = game->runtime.tasks.southS2Completed;
    snapshot->southS3Started = game->runtime.tasks.southS3Started;
    snapshot->southS3Completed = game->runtime.tasks.southS3Completed;
    snapshot->southS4Started = game->runtime.tasks.southS4Started;
    snapshot->southS4Completed = game->runtime.tasks.southS4Completed;
    snapshot->southS5Started = game->runtime.tasks.southS5Started;
    snapshot->southS5Completed = game->runtime.tasks.southS5Completed;
    snapshot->monolithActivated[0] = game->runtime.tasks.monolithActivated[0];
    snapshot->monolithActivated[1] = game->runtime.tasks.monolithActivated[1];
    snapshot->monolithActivated[2] = game->runtime.tasks.monolithActivated[2];
    snapshot->monolithsLit = game->runtime.tasks.monolithsLit;
    snapshot->ending = game->runtime.tasks.ending;
    snapshot->campPlaced = game->runtime.map.campPlaced;
    snapshot->campX = game->runtime.map.campX;
    snapshot->campY = game->runtime.map.campY;
    snapshot->communicatorUnlocked = game->runtime.tasks.communicatorUnlocked;
    snapshot->clearedDynamicTileCount = 0;

    for (index = 0; index < game->runtime.tasks.nodeCount; index++) {
        snapshot->nodes[index].active = game->runtime.tasks.nodes[index].active;
        snapshot->nodes[index].respawnsRemaining = game->runtime.tasks.nodes[index].respawnsRemaining;
    }
    for (index = 0; index < game->runtime.tasks.monsterCount; index++) {
        snapshot->monsters[index].active = game->runtime.tasks.monsters[index].active;
        snapshot->monsters[index].gridX = game->runtime.tasks.monsters[index].gridX;
        snapshot->monsters[index].gridY = game->runtime.tasks.monsters[index].gridY;
        snapshot->monsters[index].health = game->runtime.tasks.monsters[index].health;
        snapshot->monsters[index].phaseTriggered = game->runtime.tasks.monsters[index].phaseTriggered;
    }
    for (index = 0; index < game->runtime.tasks.logCount; index++) {
        snapshot->logs[index].collected = game->runtime.tasks.logs[index].collected;
    }
    for (index = 0; index < STORY_MAIN_SCENE_COUNT; index++) {
        snapshot->storyMainSceneShown[index] =
            game->flow.storySceneShown[STORY_SCENE_MAIN_AIR_FOR_ONE_MORE_DAY + index];
    }

    CaptureClearedDynamicTiles(&game->runtime.map, snapshot);

    for (index = 0; index < game->services.mapScene.runtimeStateCount; index++) {
        const MapRuntimeState *runtime = &game->services.mapScene.runtimeStates[index];
        SavedMapStateSnapshot *savedState = EnsureSavedMapState(snapshot, runtime->mapId);
        CopyRuntimeMapStateToSave(runtime, savedState);
    }
    CaptureActiveMapStateToSave(&game->runtime.map, EnsureSavedMapState(snapshot, game->runtime.map.mapId));
    for (index = 0; index < game->runtime.tasks.nodeCount; index++) {
        const ResourceNode *node = &game->runtime.tasks.nodes[index];
        SavedMapStateSnapshot *state = EnsureSavedMapState(snapshot, node->mapId);
        if (state != nullptr && state->nodeCount < MAX_RESOURCE_NODES) {
            state->nodes[state->nodeCount].active = node->active;
            state->nodes[state->nodeCount].respawnsRemaining = node->respawnsRemaining;
            state->nodeCount++;
        }
    }
    for (index = 0; index < game->runtime.tasks.monsterCount; index++) {
        const Monster *monster = &game->runtime.tasks.monsters[index];
        SavedMapStateSnapshot *state = EnsureSavedMapState(snapshot, monster->mapId);
        if (state != nullptr && state->monsterCount < MAX_MONSTERS) {
            SavedMonsterSnapshot *savedMonster = &state->monsters[state->monsterCount++];
            savedMonster->active = monster->active;
            savedMonster->gridX = monster->gridX;
            savedMonster->gridY = monster->gridY;
            savedMonster->health = monster->health;
            savedMonster->phaseTriggered = monster->phaseTriggered;
        }
    }
    for (index = 0; index < game->runtime.tasks.logCount; index++) {
        const ShipLog *log = &game->runtime.tasks.logs[index];
        SavedMapStateSnapshot *state = EnsureSavedMapState(snapshot, log->mapId);
        if (state != nullptr && state->logCount < MAX_LOGS) {
            state->logs[state->logCount++].collected = log->collected;
        }
    }
}

bool Game_LoadSnapshotIntoSession(Game *game, const SaveSnapshot *snapshot) {
    int index;
    bool positionAdjusted;
    int safeGridX;
    int safeGridY;

    const char *targetMapId;

    Game_ResetGameplayWorld(game);
    if (snapshot->sourceVersion >= 16) ImportSavedMapRuntimeStates(game, snapshot);
    targetMapId = ResolveSnapshotMapId(game, snapshot);
    if (!LoadSnapshotMap(game, targetMapId)) {
        LoadSnapshotMap(game, "ship_interior");
        targetMapId = "ship_interior";
    }
    MapSceneManager_ApplyMapState(&game->services.mapScene, &game->runtime.map);

    game->runtime.player.gridX = snapshot->gridX;
    game->runtime.player.gridY = snapshot->gridY;
    game->runtime.player.facingX = snapshot->facingX;
    game->runtime.player.facingY = snapshot->facingY;
    Player_UpdateWorldPosition(&game->runtime.player);
    game->runtime.player.health = snapshot->health;
    game->runtime.player.oxygen = snapshot->oxygen;
    game->runtime.player.poison = snapshot->poison;
    game->runtime.player.maxHealthBonus = snapshot->maxHealthBonus;
    game->runtime.player.attackBonus = 0.0f;
    game->runtime.player.deathCount = snapshot->deathCount;
    game->runtime.player.hasGlowStick = snapshot->hasGlowStick;
    game->runtime.player.hasRope = snapshot->hasRope;
    game->runtime.player.hasLaserGun = snapshot->hasLaserGun;
    game->runtime.player.hasProtectionSuit = snapshot->hasProtectionSuit;
    game->runtime.player.hasSignalAmplifier = snapshot->hasSignalAmplifier;
    game->runtime.player.hasFieldCamp = snapshot->hasFieldCamp;
    for (index = 0; index < PLAYER_STATUS_COUNT; index++) {
        game->runtime.player.statusEffects[index].active = snapshot->statuses[index].active;
        game->runtime.player.statusEffects[index].level = snapshot->statuses[index].level;
        game->runtime.player.statusEffects[index].remainingTime = snapshot->statuses[index].remainingTime;
        game->runtime.player.statusEffects[index].magnitude = snapshot->statuses[index].magnitude;
    }
    for (index = 0; index < RESOURCE_COUNT; index++) {
        game->runtime.player.resources[index] = snapshot->resources[index];
    }

    game->runtime.tasks.stage = snapshot->stage;
    game->runtime.tasks.dayCount = snapshot->dayCount;
    game->runtime.tasks.phase = (DayPhase)snapshot->phase;
    game->runtime.tasks.currentEvent = (EventType)snapshot->currentEvent;
    game->runtime.tasks.cycleTimer = snapshot->cycleTimer;
    game->runtime.tasks.elapsedSeconds = snapshot->elapsedSeconds;
    {
        const float minElapsedForSavedDay = (float)game->runtime.tasks.dayCount * DAY_COUNT_DURATION_SECONDS;
        float dayProgress = game->runtime.tasks.elapsedSeconds - minElapsedForSavedDay;

        if (!(dayProgress >= 0.0f) || dayProgress >= DAY_COUNT_DURATION_SECONDS) {
            dayProgress = std::fmod(game->runtime.tasks.cycleTimer, DAY_COUNT_DURATION_SECONDS);
            if (!(dayProgress >= 0.0f)) {
                dayProgress = 0.0f;
            }
        }
        game->runtime.tasks.elapsedSeconds = minElapsedForSavedDay + dayProgress;
        game->runtime.tasks.currentEvent = TasksContent_GetDailyEvent(game->runtime.tasks.dayCount);
    }
    game->runtime.tasks.oxygenRepairLevel = snapshot->oxygenRepairLevel;
    game->runtime.tasks.commRepairLevel = snapshot->commRepairLevel;
    game->runtime.tasks.energyRepairLevel = snapshot->energyRepairLevel;
    game->runtime.tasks.crashClueFound = snapshot->crashClueFound;
    game->runtime.tasks.amplifierUnlocked = snapshot->amplifierUnlocked;
    game->runtime.tasks.signalAmplifierCrafted = snapshot->hasSignalAmplifier;
    game->runtime.tasks.communicatorUnlocked = snapshot->communicatorUnlocked;
    game->runtime.tasks.bossDefeated = snapshot->bossDefeated;
    game->runtime.tasks.signalTowerActivated = snapshot->signalTowerActivated;
    game->runtime.tasks.selectedEndingRoute = (GameEnding)snapshot->selectedEndingRoute;
    game->runtime.tasks.endingArchiveReviewed = snapshot->endingArchiveReviewed;
    game->runtime.tasks.westW1Started = snapshot->westW1Started;
    game->runtime.tasks.westW1Completed = snapshot->westW1Completed;
    game->runtime.tasks.westW2Started = snapshot->westW2Started;
    game->runtime.tasks.westW2Completed = snapshot->westW2Completed;
    game->runtime.tasks.westW3Started = snapshot->westW3Started;
    game->runtime.tasks.westW3Completed = snapshot->westW3Completed;
    game->runtime.tasks.westW4Started = snapshot->westW4Started;
    game->runtime.tasks.westW4Completed = snapshot->westW4Completed;
    game->runtime.tasks.westW5Started = snapshot->westW5Started;
    game->runtime.tasks.westW5Completed = snapshot->westW5Completed;
    game->runtime.tasks.southS1Started = snapshot->southS1Started;
    game->runtime.tasks.southS1Completed = snapshot->southS1Completed;
    game->runtime.tasks.southS2Started = snapshot->southS2Started;
    game->runtime.tasks.southS2Completed = snapshot->southS2Completed;
    game->runtime.tasks.southS3Started = snapshot->southS3Started;
    game->runtime.tasks.southS3Completed = snapshot->southS3Completed;
    game->runtime.tasks.southS4Started = snapshot->southS4Started;
    game->runtime.tasks.southS4Completed = snapshot->southS4Completed;
    game->runtime.tasks.southS5Started = snapshot->southS5Started;
    game->runtime.tasks.southS5Completed = snapshot->southS5Completed;
    game->runtime.tasks.monolithActivated[0] = snapshot->monolithActivated[0];
    game->runtime.tasks.monolithActivated[1] = snapshot->monolithActivated[1];
    game->runtime.tasks.monolithActivated[2] = snapshot->monolithActivated[2];
    game->runtime.tasks.monolithsLit = snapshot->monolithsLit;
    game->runtime.tasks.shownMainStorySceneCount = CountShownMainStoryScenes(snapshot->storyMainSceneShown);
    game->runtime.tasks.ending = (GameEnding)snapshot->ending;
    if (snapshot->sourceVersion >= 16) {
        ApplyV16EntityStates(game, snapshot);
    } else {
        for (index = 0; index < game->runtime.tasks.nodeCount; index++) {
            game->runtime.tasks.nodes[index].active = snapshot->nodes[index].active;
            game->runtime.tasks.nodes[index].respawnsRemaining = snapshot->nodes[index].respawnsRemaining;
        }
        for (index = 0; index < game->runtime.tasks.monsterCount; index++) {
            game->runtime.tasks.monsters[index].active = snapshot->monsters[index].active;
            game->runtime.tasks.monsters[index].gridX = snapshot->monsters[index].gridX;
            game->runtime.tasks.monsters[index].gridY = snapshot->monsters[index].gridY;
            game->runtime.tasks.monsters[index].health = MigrateFullHealthMonsterValue(game->runtime.tasks.monsters[index].type,
                                                                                snapshot->monsters[index].health,
                                                                                game->runtime.tasks.monsters[index].maxHealth);
            game->runtime.tasks.monsters[index].phaseTriggered = snapshot->monsters[index].phaseTriggered;
        }
        for (index = 0; index < game->runtime.tasks.logCount; index++) game->runtime.tasks.logs[index].collected = snapshot->logs[index].collected;
    }
    for (index = 0; index < STORY_MAIN_SCENE_COUNT; index++) {
        game->flow.storySceneShown[STORY_SCENE_MAIN_AIR_FOR_ONE_MORE_DAY + index] = snapshot->storyMainSceneShown[index];
    }
    SanitizeLoadedTaskRuntimeState(game);

    if (game->runtime.tasks.stage >= 5) {
        Map_UnlockSwampDeep(&game->runtime.map);
        TasksRuntime_EnsureEnergyCoreNode(&game->runtime.tasks, &game->runtime.map, &game->runtime.player);
    }
    if (game->runtime.tasks.stage >= 6) {
        Map_UnlockRuins(&game->runtime.map);
    }
    if (snapshot->sourceVersion < 16 && snapshot->campPlaced) {
        Map_SetFieldCamp(&game->runtime.map, snapshot->campX, snapshot->campY);
    }
    if (game->runtime.tasks.communicatorUnlocked) {
        Map_UnlockLoxiRoom(&game->runtime.map);
    }
    if (snapshot->sourceVersion < 16) {
        ApplySavedDynamicTiles(&game->runtime.map, snapshot);
    } else {
        const SavedMapStateSnapshot *activeState = FindSavedMapState(snapshot, game->runtime.map.mapId);
        MapSceneManager_ApplyMapState(&game->services.mapScene, &game->runtime.map);
        if (activeState != nullptr && activeState->campPlaced) {
            Map_SetFieldCamp(&game->runtime.map, activeState->campX, activeState->campY);
        }
    }
    if (game->runtime.tasks.selectedEndingRoute == ENDING_HEROIC && !game->runtime.tasks.bossDefeated) {
        Map_LockSwampOuter(&game->runtime.map);
    }

    positionAdjusted = false;
    safeGridX = game->runtime.player.gridX;
    safeGridY = game->runtime.player.gridY;
    if (!Game_FindNearestSafeLoadedPlayerTile(game, game->runtime.player.gridX, game->runtime.player.gridY, &safeGridX, &safeGridY)) {
        const MapCatalogEntry *entry = MapCatalog_Find(&game->services.mapScene.catalog, targetMapId);
        int anchorX;
        int anchorY;
        if (entry != nullptr
            && Map_GetAnchorPositionByName(&game->runtime.map, entry->respawnAnchor, &anchorX, &anchorY)
            && Game_FindNearestSafeLoadedPlayerTile(game, anchorX, anchorY, &safeGridX, &safeGridY)) {
            positionAdjusted = true;
        }
    } else if (safeGridX != game->runtime.player.gridX || safeGridY != game->runtime.player.gridY) {
        positionAdjusted = true;
    }

    if (positionAdjusted) {
        game->runtime.player.gridX = safeGridX;
        game->runtime.player.gridY = safeGridY;
        Player_UpdateWorldPosition(&game->runtime.player);
    }

    MiniMap_Update(&game->runtime.miniMap, &game->runtime.player, &game->runtime.map);
    Tasks_UpdateObjective(&game->runtime.tasks, &game->runtime.player);
    Game_SyncTrackedLocation(game);
    game->flow.state = game->runtime.tasks.ending == ENDING_NONE ? GAME_STATE_PLAYING : GAME_STATE_ENDING;
    game->flow.openingSlideIndex = 0;
    game->flow.openingCutsceneElapsed = 0.0f;
    game->flow.openingAwaitingFirstAdvance = false;
    Game_CloseStoryScene(game);
    Game_CloseTransientOverlays(game);
    Game_ResetCameraToPlayer(game);
    Audio_SetScene(&game->services.audio, Game_SelectAudioScene(game));
    Audio_SetMusicStage(&game->services.audio, Game_SelectMusicStage(game));

    return positionAdjusted;
}
