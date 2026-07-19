#ifndef TASK_PROGRESSION_INTERNAL_H
#define TASK_PROGRESSION_INTERNAL_H

#include "task_system.h"

SCL_EXTERN_C_BEGIN

void TasksRuntime_UnlockStageIfNeeded(TaskSystem *tasks, GameMap *map, int newStage);
void TasksRuntime_EnsureEnergyCoreNode(TaskSystem *tasks, const GameMap *map, const Player *player);

SCL_EXTERN_C_END
#endif
