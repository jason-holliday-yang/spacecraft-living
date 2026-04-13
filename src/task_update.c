#include "task_runtime_internal.h"

void Tasks_Update(TaskSystem *tasks, GameMap *map, Player *player, float deltaTime) {
    TasksRuntime_UpdateDayCycle(tasks, deltaTime);
    TasksRuntime_RespawnNodes(tasks, player, deltaTime);
    TasksRuntime_UpdateMonsters(tasks, map, player, deltaTime);
    TasksRuntime_UpdatePlayerSurvival(tasks, map, player, deltaTime);
    Tasks_UpdateObjective(tasks, player);
}
