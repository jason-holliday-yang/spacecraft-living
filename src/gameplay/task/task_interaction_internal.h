#ifndef TASK_INTERACTION_INTERNAL_H
#define TASK_INTERACTION_INTERNAL_H

#include "task_system.h"

SCL_EXTERN_C_BEGIN

typedef enum TaskInteractionTarget {
    TASK_INTERACTION_NONE = 0,
    TASK_INTERACTION_OXYGEN_CONSOLE,
    TASK_INTERACTION_LOXI_TERMINAL,
    TASK_INTERACTION_WORKBENCH,
    TASK_INTERACTION_AIRLOCK_CONSOLE,
    TASK_INTERACTION_COMM_RELAY,
    TASK_INTERACTION_CRASH_CLUE,
    TASK_INTERACTION_ENERGY_CONSOLE,
    TASK_INTERACTION_MONOLITH_A,
    TASK_INTERACTION_MONOLITH_B,
    TASK_INTERACTION_MONOLITH_C,
    TASK_INTERACTION_SIGNAL_TOWER
} TaskInteractionTarget;

bool TasksRuntime_IsNearRect(const Player *player, int gridX, int gridY, int width, int height);
bool TasksRuntime_IsNearTile(const Player *player, int gridX, int gridY);
int TasksRuntime_GetRectInteractionScore(const Player *player, int gridX, int gridY, int width, int height);
TaskInteractionTarget TasksRuntime_GetPreferredInteractionTarget(const GameMap *map, const Player *player);
bool TasksRuntime_HandleShipInteraction(TaskSystem *tasks, GameMap *map, Player *player, TaskInteractionTarget target, char *message, size_t messageSize);
bool TasksRuntime_HandleWorldInteraction(TaskSystem *tasks, GameMap *map, Player *player, TaskInteractionTarget target, char *message, size_t messageSize);

SCL_EXTERN_C_END
#endif
