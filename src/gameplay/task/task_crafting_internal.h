#ifndef TASK_CRAFTING_INTERNAL_H
#define TASK_CRAFTING_INTERNAL_H

#include "task_system.h"

SCL_EXTERN_C_BEGIN

typedef enum ResourceNodeInfoState {
    RESOURCE_NODE_INFO_READY = 0,
    RESOURCE_NODE_INFO_DEPLETED,
    RESOURCE_NODE_INFO_RESTORED,
    RESOURCE_NODE_INFO_SHIP_SUPPLY_READY,
    RESOURCE_NODE_INFO_SHIP_SUPPLY_EMPTY
} ResourceNodeInfoState;

bool TasksRuntime_SpendRecipeResources(TaskSystem *tasks, Player *player, RecipeType recipe, const char *advancedRequirementMessage, char *message, size_t messageSize);
bool TasksRuntime_IsRecipeCompleted(const TaskSystem *tasks, const GameMap *map, const Player *player, RecipeType recipe);
bool TasksRuntime_IsShipInteriorNode(const ResourceNode *node);
ResourceNodeInfoState TasksRuntime_GetNodeInfoState(const ResourceNode *node);
void TasksRuntime_DescribeNodeStatus(const ResourceNode *node, char *message, size_t messageSize);
bool TasksRuntime_CollectNode(TaskSystem *tasks, Player *player, ResourceNode *node, char *message, size_t messageSize);

SCL_EXTERN_C_END
#endif
