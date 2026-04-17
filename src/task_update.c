#include "task_runtime_internal.h"

static ResourceNode *FindEnergyCoreNode(TaskSystem *tasks) {
    int index;

    if (tasks == NULL) {
        return NULL;
    }

    for (index = 0; index < tasks->nodeCount; index++) {
        if (tasks->nodes[index].type == RESOURCE_ENERGY_CORE) {
            return &tasks->nodes[index];
        }
    }

    return NULL;
}

static bool ShouldEnsureEnergyCoreNode(const TaskSystem *tasks, const Player *player) {
    if (tasks == NULL || tasks->stage < 5 || tasks->energyRepairLevel >= 1) {
        return false;
    }

    return player == NULL || player->resources[RESOURCE_ENERGY_CORE] <= 0;
}

void TasksRuntime_EnsureEnergyCoreNode(TaskSystem *tasks, const GameMap *map, const Player *player) {
    ResourceNode *existingNode;
    int resolvedX;
    int resolvedY;
    ResourceNode *node;

    if (!ShouldEnsureEnergyCoreNode(tasks, player) || map == NULL) {
        return;
    }

    existingNode = FindEnergyCoreNode(tasks);
    if (existingNode != NULL) {
        existingNode->active = true;
        existingNode->respawnsRemaining = 0;
        existingNode->initialRespawnsRemaining = 0;
        existingNode->special = true;
        existingNode->awayTimer = 0.0f;
        return;
    }

    if (tasks->nodeCount >= MAX_RESOURCE_NODES
        || !TasksRuntime_FindNodeSpawnTile(tasks,
                                           map,
                                           ENERGY_CORE_NODE_X,
                                           ENERGY_CORE_NODE_Y,
                                           &resolvedX,
                                           &resolvedY)) {
        return;
    }

    node = &tasks->nodes[tasks->nodeCount++];
    node->active = true;
    node->type = RESOURCE_ENERGY_CORE;
    node->gridX = resolvedX;
    node->gridY = resolvedY;
    node->baseYield = 1;
    node->respawnsRemaining = 0;
    node->initialRespawnsRemaining = 0;
    node->special = true;
    node->area = Map_GetAreaAt(resolvedX, resolvedY);
    node->awayTimer = 0.0f;
}

void Tasks_Update(TaskSystem *tasks, GameMap *map, Player *player, float deltaTime) {
    TasksRuntime_EnsureEnergyCoreNode(tasks, map, player);
    TasksRuntime_UpdateDayCycle(tasks, deltaTime);
    TasksRuntime_RespawnNodes(tasks, player, deltaTime);
    TasksRuntime_UpdateMonsters(tasks, map, player, deltaTime);
    TasksRuntime_UpdatePlayerSurvival(tasks, map, player, deltaTime);
    Tasks_UpdateObjective(tasks, player);
}
