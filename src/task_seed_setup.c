#include "task_runtime_internal.h"

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

void Tasks_Init(TaskSystem *tasks, GameMap *map) {
    int seedIndex;

    memset(tasks, 0, sizeof(*tasks));
    tasks->stage = 1;
    tasks->phase = DAY_PHASE_DAY;
    tasks->currentEvent = EVENT_HARVEST;
    tasks->communicatorUnlocked = false;
    tasks->endingArchiveReviewed = false;
    tasks->westW1Started = false;
    tasks->westW1Completed = false;
    tasks->westW2Started = false;
    tasks->westW2Completed = false;
    tasks->westW3Started = false;
    tasks->westW3Completed = false;
    tasks->westW4Started = false;
    tasks->westW4Completed = false;
    tasks->westW5Started = false;
    tasks->westW5Completed = false;
    tasks->southS1Started = false;
    tasks->southS1Completed = false;
    tasks->southS2Started = false;
    tasks->southS2Completed = false;
    tasks->southS3Started = false;
    tasks->southS3Completed = false;
    tasks->southS4Started = false;
    tasks->southS4Completed = false;
    tasks->southS5Started = false;
    tasks->southS5Completed = false;

    Puzzle_Init(&tasks->monolithPuzzle);

    for (seedIndex = 0; seedIndex < TasksContent_GetNodeSeedCount(); seedIndex++) {
        TaskNodeSeed seed;

        if (!TasksContent_GetNodeSeed(seedIndex, &seed)) {
            continue;
        }

        AddNode(tasks, map, seed.type, seed.gridX, seed.gridY, seed.baseYield, seed.respawnsRemaining, seed.special);
    }

    for (seedIndex = 0; seedIndex < TasksContent_GetMonsterSeedCount(); seedIndex++) {
        TaskMonsterSeed seed;

        if (!TasksContent_GetMonsterSeed(seedIndex, &seed)) {
            continue;
        }

        TasksRuntime_AddMonsterSpawn(tasks, map, seed.type, seed.gridX, seed.gridY, seed.unlockStage);
    }

    for (seedIndex = 0; seedIndex < TasksContent_GetLogSeedCount(); seedIndex++) {
        TaskLogSeed seed;

        if (!TasksContent_GetLogSeed(seedIndex, &seed)) {
            continue;
        }

        AddLog(tasks,
               seed.gridX,
               seed.gridY,
               seed.category,
               seed.title,
               seed.storyText,
               seed.detailText,
               map);
    }

    TasksRuntime_SyncLogAvailability(tasks);

    tasks->stage = 1;
    tasks->objective[0] = '\0';
    tasks->communicator[0] = '\0';
}
