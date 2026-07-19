#ifndef TASK_TARGETING_INTERNAL_H
#define TASK_TARGETING_INTERNAL_H

#include "task_system.h"

SCL_EXTERN_C_BEGIN

bool TasksRuntime_HasNearbyPickupPriority(const TaskSystem *tasks, const Player *player);
ResourceNode *TasksRuntime_FindNearbyNode(TaskSystem *tasks, const Player *player);
ResourceNode *TasksRuntime_FindNearbyNodeAnyState(TaskSystem *tasks, const Player *player);
ShipLog *TasksRuntime_FindNearbyLog(TaskSystem *tasks, const Player *player);

SCL_EXTERN_C_END
#endif
