#include "task_progression_internal.h"
#include "task_survival_internal.h"
#include "task_encounter_internal.h"
#include "task_presentation.h"

void Tasks_Update(TaskSystem *tasks, GameMap *map, Player *player, float deltaTime) {
    TasksRuntime_EnsureEnergyCoreNode(tasks, map, player);
    TasksRuntime_UpdateDayCycle(tasks, deltaTime);
    TasksRuntime_RespawnNodes(tasks, player, deltaTime);
    TasksRuntime_UpdateMonsters(tasks, map, player, deltaTime);
    TasksRuntime_UpdatePlayerSurvival(tasks, map, player, deltaTime);
    Tasks_UpdateObjective(tasks, player);
}
