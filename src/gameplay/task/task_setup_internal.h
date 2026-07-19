#ifndef TASK_SETUP_INTERNAL_H
#define TASK_SETUP_INTERNAL_H

#include "task_system.h"

SCL_EXTERN_C_BEGIN

bool TasksRuntime_FindNodeSpawnTile(const TaskSystem *tasks, const GameMap *map, int preferredX, int preferredY, int *resolvedX, int *resolvedY);
bool TasksRuntime_FindLogSpawnTile(const TaskSystem *tasks, const GameMap *map, int preferredX, int preferredY, int *resolvedX, int *resolvedY);
bool TasksRuntime_FindMonsterSpawnTile(const TaskSystem *tasks, const GameMap *map, int preferredX, int preferredY, int *resolvedX, int *resolvedY);
bool TasksRuntime_AddMonsterSpawn(TaskSystem *tasks, const GameMap *map, MonsterType type, int gridX, int gridY, int unlockStage);

SCL_EXTERN_C_END
#endif
