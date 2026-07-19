#ifndef TASK_ENCOUNTER_INTERNAL_H
#define TASK_ENCOUNTER_INTERNAL_H

#include "task_system.h"

SCL_EXTERN_C_BEGIN

void TasksRuntime_GetMonsterFootprint(const Monster *monster, int *originX, int *originY, int *width, int *height);
bool TasksRuntime_IsMonsterOccupyingTile(const Monster *monster, int gridX, int gridY);
int TasksRuntime_GetDistanceToMonster(const Monster *monster, int gridX, int gridY);
Monster *TasksRuntime_FindMonsterAt(TaskSystem *tasks, int gridX, int gridY);
Monster *TasksRuntime_FindAttackTarget(TaskSystem *tasks, const Player *player);
void TasksRuntime_DropMonsterRewards(TaskSystem *tasks, Player *player, Monster *monster);
void TasksRuntime_UpdateMonsters(TaskSystem *tasks, const GameMap *map, Player *player, float deltaTime);

SCL_EXTERN_C_END
#endif
