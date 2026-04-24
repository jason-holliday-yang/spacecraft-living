#include "task_runtime_internal.h"

void TasksRuntime_EnsureEnergyCoreNode(TaskSystem *tasks, const GameMap *map, const Player *player) {
    int index;

    (void)map;
    (void)player;

    if (tasks == NULL) {
        return;
    }

    for (index = 0; index < tasks->nodeCount; index++) {
        if (tasks->nodes[index].type == RESOURCE_ENERGY_CORE) {
            tasks->nodes[index].active = false;
            tasks->nodes[index].respawnsRemaining = 0;
            tasks->nodes[index].initialRespawnsRemaining = 0;
            tasks->nodes[index].awayTimer = 0.0f;
        }
    }
}

void Tasks_Update(TaskSystem *tasks, GameMap *map, Player *player, float deltaTime) {
    TasksRuntime_EnsureEnergyCoreNode(tasks, map, player);
    TasksRuntime_UpdateDayCycle(tasks, deltaTime);
    TasksRuntime_RespawnNodes(tasks, player, deltaTime);
    TasksRuntime_UpdateMonsters(tasks, map, player, deltaTime);
    TasksRuntime_UpdatePlayerSurvival(tasks, map, player, deltaTime);
    Tasks_UpdateObjective(tasks, player);
}
