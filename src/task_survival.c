#include "task_runtime_internal.h"

#include "task_content.h"

#include <math.h>

void TasksRuntime_RespawnNodes(TaskSystem *tasks, const Player *player, float deltaTime) {
    int index;
    MapArea playerArea;

    playerArea = Map_GetAreaAt(player->gridX, player->gridY);

    for (index = 0; index < tasks->nodeCount; index++) {
        ResourceNode *node;
        float threshold;

        node = &tasks->nodes[index];
        if (node->active || TasksRuntime_IsShipInteriorNode(node) || node->respawnsRemaining <= 0) {
            continue;
        }

        if (playerArea != node->area) {
            node->awayTimer += deltaTime;
        } else {
            node->awayTimer = 0.0f;
        }

        if (node->special) {
            threshold = 240.0f;
        } else if (node->baseYield >= 2) {
            threshold = 95.0f;
        } else {
            threshold = 135.0f;
        }
        if (node->awayTimer >= threshold) {
            node->active = true;
            node->awayTimer = 0.0f;
        }
    }
}

void TasksRuntime_UpdateDayCycle(TaskSystem *tasks, float deltaTime) {
    float previousTimer;
    int previousDay;
    float cycleTime;

    previousTimer = tasks->cycleTimer;
    previousDay = tasks->dayCount;
    tasks->cycleTimer += deltaTime;
    tasks->elapsedSeconds += deltaTime;

    while (tasks->cycleTimer >= FULL_CYCLE_SECONDS) {
        tasks->cycleTimer -= FULL_CYCLE_SECONDS;
        tasks->dayCount += 1;
    }

    cycleTime = tasks->cycleTimer;
    if (cycleTime < DAY_DURATION_SECONDS) {
        tasks->phase = DAY_PHASE_DAY;
    } else if (cycleTime < DAY_DURATION_SECONDS + DUSK_DURATION_SECONDS) {
        tasks->phase = DAY_PHASE_DUSK;
    } else {
        tasks->phase = DAY_PHASE_NIGHT;
    }

    if (tasks->dayCount != previousDay || (previousTimer > tasks->cycleTimer && tasks->cycleTimer < 1.0f)) {
        tasks->currentEvent = TasksContent_GetDailyEvent(tasks->dayCount);
    }
}
