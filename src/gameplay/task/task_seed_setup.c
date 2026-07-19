#include "task_setup_internal.h"
#include "task_archive_internal.h"
#include "task_content.h"

#include <stdio.h>
#include <string.h>

static void AddNode(TaskSystem *tasks,
                    const GameMap *map,
                    ResourceType type,
                    int gridX,
                    int gridY,
                    int yieldCount,
                    int respawns,
                    bool special) {
    ResourceNode *node;
    int resolvedX;
    int resolvedY;

    if (tasks->nodeCount >= MAX_RESOURCE_NODES
        || !TasksRuntime_FindNodeSpawnTile(tasks, map, gridX, gridY, &resolvedX, &resolvedY)) {
        return;
    }

    node = &tasks->nodes[tasks->nodeCount++];
    memset(node, 0, sizeof(*node));
    snprintf(node->mapId, sizeof(node->mapId), "%s", map != NULL ? map->mapId : "legacy_world");
    node->active = true;
    node->type = type;
    node->gridX = resolvedX;
    node->gridY = resolvedY;
    node->baseYield = yieldCount;
    node->respawnsRemaining = respawns;
    node->initialRespawnsRemaining = respawns;
    node->special = special;
    node->area = Map_GetAreaAt(resolvedX, resolvedY);
    node->awayTimer = 0.0f;
}

static void AddLog(TaskSystem *tasks,
                   int gridX,
                   int gridY,
                   int sourceIndex,
                   ShipLogCategory category,
                   LocalizedText title,
                   LocalizedText story,
                   LocalizedText detailText,
                   const GameMap *map) {
    ShipLog *log;
    int resolvedX;
    int resolvedY;

    if (tasks->logCount >= MAX_LOGS
        || map == NULL
        || !TasksRuntime_FindLogSpawnTile(tasks, map, gridX, gridY, &resolvedX, &resolvedY)) {
        return;
    }

    log = &tasks->logs[tasks->logCount++];
    memset(log, 0, sizeof(*log));
    snprintf(log->mapId, sizeof(log->mapId), "%s", map->mapId);
    log->sourceIndex = sourceIndex;
    log->active = false;
    log->collected = false;
    log->gridX = resolvedX;
    log->gridY = resolvedY;
    log->category = category;

    snprintf(log->titleEn, sizeof(log->titleEn), "%s", title.english != NULL ? title.english : "Unknown Log");
    snprintf(log->titleZh, sizeof(log->titleZh), "%s", title.simplifiedChinese != NULL ? title.simplifiedChinese : log->titleEn);
    snprintf(log->storyTextEn, sizeof(log->storyTextEn), "%s", story.english != NULL ? story.english : "");
    snprintf(log->storyTextZh, sizeof(log->storyTextZh), "%s", story.simplifiedChinese != NULL ? story.simplifiedChinese : log->storyTextEn);
    snprintf(log->detailTextEn, sizeof(log->detailTextEn), "%s", detailText.english != NULL ? detailText.english : "");
    snprintf(log->detailTextZh, sizeof(log->detailTextZh), "%s", detailText.simplifiedChinese != NULL ? detailText.simplifiedChinese : log->detailTextEn);
}

static bool IsMapRegistered(const TaskSystem *tasks, const char *mapId) {
    int index;

    for (index = 0; index < tasks->registeredMapCount; index++) {
        if (strcmp(tasks->registeredMapIds[index], mapId) == 0) {
            return true;
        }
    }
    return false;
}

void Tasks_SetActiveMap(TaskSystem *tasks, const GameMap *map) {
    if (tasks == NULL || map == NULL) {
        return;
    }
    snprintf(tasks->activeMapId, sizeof(tasks->activeMapId), "%s", map->mapId);
    tasks->activeMapKind = map->mapKind;
}

bool Tasks_IsEntityOnActiveMap(const TaskSystem *tasks, const char *mapId) {
    if (tasks == NULL || mapId == NULL) {
        return false;
    }
    if (strcmp(tasks->activeMapId, "legacy_world") == 0) {
        return true;
    }
    return strcmp(tasks->activeMapId, mapId) == 0;
}

bool Tasks_RegisterMapEntities(TaskSystem *tasks, GameMap *map) {
    int seedIndex;
    int initialNodeCount;
    int initialMonsterCount;
    int initialLogCount;
    char previousActiveMapId[MAP_ID_MAX];
    MapKind previousActiveMapKind;

    if (tasks == NULL || map == NULL || map->mapId[0] == '\0') {
        return false;
    }
    if (IsMapRegistered(tasks, map->mapId)) {
        return true;
    }
    if (tasks->registeredMapCount >= MAX_MAP_CATALOG_ENTRIES
        || tasks->nodeCount + map->resourceSeedCount > MAX_RESOURCE_NODES
        || tasks->monsterCount + map->monsterSeedCount > MAX_MONSTERS
        || tasks->logCount + map->logSeedCount > MAX_LOGS) {
        return false;
    }

    initialNodeCount = tasks->nodeCount;
    initialMonsterCount = tasks->monsterCount;
    initialLogCount = tasks->logCount;
    snprintf(previousActiveMapId, sizeof(previousActiveMapId), "%s", tasks->activeMapId);
    previousActiveMapKind = tasks->activeMapKind;
    Tasks_SetActiveMap(tasks, map);

    for (seedIndex = 0; seedIndex < map->resourceSeedCount; seedIndex++) {
        const MapResourceSeed *seed = &map->resourceSeeds[seedIndex];
        if (seed->resourceType < 0 || seed->resourceType >= RESOURCE_COUNT) {
            continue;
        }
        AddNode(tasks, map, (ResourceType)seed->resourceType, seed->gridX, seed->gridY,
                seed->baseYield, seed->respawnsRemaining, seed->special);
    }

    for (seedIndex = 0; seedIndex < map->monsterSeedCount; seedIndex++) {
        const MapMonsterSeed *seed = &map->monsterSeeds[seedIndex];
        if (seed->monsterType < MONSTER_THORN_LARVA || seed->monsterType > MONSTER_FINAL_BOSS) {
            continue;
        }
        TasksRuntime_AddMonsterSpawn(tasks, map, (MonsterType)seed->monsterType,
                                     seed->gridX, seed->gridY, seed->unlockStage);
    }

    for (seedIndex = 0; seedIndex < map->logSeedCount; seedIndex++) {
        const MapLogSeed *mapSeed = &map->logSeeds[seedIndex];
        TaskLogSeed contentSeed;
        if (!TasksContent_GetLogSeed(mapSeed->sourceIndex, &contentSeed)
            || mapSeed->logCategory < SHIP_LOG_MAINLINE
            || mapSeed->logCategory > SHIP_LOG_SUPPLEMENTAL) {
            continue;
        }
        AddLog(tasks, mapSeed->gridX, mapSeed->gridY, mapSeed->sourceIndex,
               (ShipLogCategory)mapSeed->logCategory, contentSeed.title,
               contentSeed.storyText, contentSeed.detailText, map);
    }

    if (tasks->nodeCount - initialNodeCount != map->resourceSeedCount
        || tasks->monsterCount - initialMonsterCount != map->monsterSeedCount
        || tasks->logCount - initialLogCount != map->logSeedCount) {
        tasks->nodeCount = initialNodeCount;
        tasks->monsterCount = initialMonsterCount;
        tasks->logCount = initialLogCount;
        snprintf(tasks->activeMapId, sizeof(tasks->activeMapId), "%s", previousActiveMapId);
        tasks->activeMapKind = previousActiveMapKind;
        return false;
    }

    snprintf(tasks->registeredMapIds[tasks->registeredMapCount++], MAP_ID_MAX, "%s", map->mapId);
    snprintf(tasks->activeMapId, sizeof(tasks->activeMapId), "%s", previousActiveMapId);
    tasks->activeMapKind = previousActiveMapKind;
    TasksRuntime_SyncLogAvailability(tasks);
    return true;
}

void Tasks_Init(TaskSystem *tasks, GameMap *map) {
    memset(tasks, 0, sizeof(*tasks));
    tasks->stage = 1;
    tasks->phase = DAY_PHASE_DAY;
    tasks->currentEvent = EVENT_HARVEST;
    Puzzle_Init(&tasks->monolithPuzzle);
    Tasks_SetActiveMap(tasks, map);

    if (map != NULL && map->hasResourceLayer && map->hasMonsterLayer && map->hasLogLayer) {
        Tasks_RegisterMapEntities(tasks, map);
    } else {
        int seedIndex;
        GameMap fallback = {0};
        snprintf(fallback.mapId, sizeof(fallback.mapId), "legacy_world");
        fallback.width = map != NULL ? map->width : MAP_WIDTH;
        fallback.height = map != NULL ? map->height : MAP_HEIGHT;
        fallback.groundTiles = map != NULL ? map->groundTiles : NULL;
        fallback.propTiles = map != NULL ? map->propTiles : NULL;
        fallback.areaTiles = map != NULL ? map->areaTiles : NULL;
        fallback.hasAreaLayer = map != NULL && map->hasAreaLayer;
        for (seedIndex = 0; seedIndex < TasksContent_GetNodeSeedCount(); seedIndex++) {
            TaskNodeSeed seed;
            if (TasksContent_GetNodeSeed(seedIndex, &seed)) {
                AddNode(tasks, map, seed.type, seed.gridX, seed.gridY,
                        seed.baseYield, seed.respawnsRemaining, seed.special);
            }
        }
        for (seedIndex = 0; seedIndex < TasksContent_GetMonsterSeedCount(); seedIndex++) {
            TaskMonsterSeed seed;
            if (TasksContent_GetMonsterSeed(seedIndex, &seed)) {
                TasksRuntime_AddMonsterSpawn(tasks, map, seed.type, seed.gridX, seed.gridY, seed.unlockStage);
            }
        }
        for (seedIndex = 0; seedIndex < TasksContent_GetLogSeedCount(); seedIndex++) {
            TaskLogSeed seed;
            if (TasksContent_GetLogSeed(seedIndex, &seed)) {
                AddLog(tasks, seed.gridX, seed.gridY, seedIndex, seed.category,
                       seed.title, seed.storyText, seed.detailText, map);
            }
        }
    }

    TasksRuntime_SyncLogAvailability(tasks);
    tasks->objective[0] = '\0';
    tasks->communicator[0] = '\0';
}
