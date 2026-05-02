#ifndef TASK_RUNTIME_INTERNAL_H
#define TASK_RUNTIME_INTERNAL_H

#include "task_system.h"

/* Internal task-runtime helpers shared by the task C/C++ implementation split. */

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

typedef enum ResourceNodeInfoState {
    RESOURCE_NODE_INFO_READY = 0,
    RESOURCE_NODE_INFO_DEPLETED,
    RESOURCE_NODE_INFO_RESTORED,
    RESOURCE_NODE_INFO_SHIP_SUPPLY_READY,
    RESOURCE_NODE_INFO_SHIP_SUPPLY_EMPTY
} ResourceNodeInfoState;

bool TasksRuntime_IsNearRect(const Player *player, int gridX, int gridY, int width, int height);
bool TasksRuntime_IsNearTile(const Player *player, int gridX, int gridY);
int TasksRuntime_GetRectInteractionScore(const Player *player, int gridX, int gridY, int width, int height);
TaskInteractionTarget TasksRuntime_GetPreferredInteractionTarget(const Player *player);
void TasksRuntime_ReducePoisonAtRecovery(Player *player, float amount);
void TasksRuntime_DowngradeOxygenLeakAtRecovery(Player *player);
void TasksRuntime_ClearNegativeSurvivalStatuses(Player *player);
bool TasksRuntime_HandleShipInteraction(TaskSystem *tasks,
                                        GameMap *map,
                                        Player *player,
                                        TaskInteractionTarget target,
                                        char *message,
                                        size_t messageSize);
bool TasksRuntime_HandleWorldInteraction(TaskSystem *tasks,
                                         GameMap *map,
                                         Player *player,
                                         TaskInteractionTarget target,
                                         char *message,
                                         size_t messageSize);
void TasksRuntime_SanitizeDisplayText(const char *source, char *dest, size_t destSize);
void TasksRuntime_WriteMessage(char *message, size_t messageSize, const char *text);

bool TasksRuntime_SpendRecipeResources(TaskSystem *tasks,
                                       Player *player,
                                       RecipeType recipe,
                                       const char *advancedRequirementMessage,
                                       char *message,
                                       size_t messageSize);
bool TasksRuntime_IsRecipeCompleted(const TaskSystem *tasks,
                                    const GameMap *map,
                                    const Player *player,
                                    RecipeType recipe);

bool TasksRuntime_FindNodeSpawnTile(const TaskSystem *tasks,
                                    const GameMap *map,
                                    int preferredX,
                                    int preferredY,
                                    int *resolvedX,
                                    int *resolvedY);
bool TasksRuntime_FindLogSpawnTile(const TaskSystem *tasks,
                                   const GameMap *map,
                                   int preferredX,
                                   int preferredY,
                                   int *resolvedX,
                                   int *resolvedY);
bool TasksRuntime_FindMonsterSpawnTile(const TaskSystem *tasks,
                                       const GameMap *map,
                                       int preferredX,
                                       int preferredY,
                                       int *resolvedX,
                                       int *resolvedY);
bool TasksRuntime_AddMonsterSpawn(TaskSystem *tasks,
                                  const GameMap *map,
                                  MonsterType type,
                                  int gridX,
                                  int gridY,
                                  int unlockStage);
bool TasksRuntime_HasNearbyPickupPriority(const TaskSystem *tasks, const Player *player);

ResourceNode *TasksRuntime_FindNearbyNode(TaskSystem *tasks, const Player *player);
ResourceNode *TasksRuntime_FindNearbyNodeAnyState(TaskSystem *tasks, const Player *player);
ShipLog *TasksRuntime_FindNearbyLog(TaskSystem *tasks, const Player *player);
void TasksRuntime_GetMonsterFootprint(const Monster *monster,
                                      int *originX,
                                      int *originY,
                                      int *width,
                                      int *height);
bool TasksRuntime_IsMonsterOccupyingTile(const Monster *monster, int gridX, int gridY);
int TasksRuntime_GetDistanceToMonster(const Monster *monster, int gridX, int gridY);
Monster *TasksRuntime_FindMonsterAt(TaskSystem *tasks, int gridX, int gridY);
Monster *TasksRuntime_FindAttackTarget(TaskSystem *tasks, const Player *player);

bool TasksRuntime_IsShipInteriorNode(const ResourceNode *node);
ResourceNodeInfoState TasksRuntime_GetNodeInfoState(const ResourceNode *node);
void TasksRuntime_DescribeNodeStatus(const ResourceNode *node, char *message, size_t messageSize);
bool TasksRuntime_CollectNode(TaskSystem *tasks, Player *player, ResourceNode *node, char *message, size_t messageSize);
void TasksRuntime_UnlockStageIfNeeded(TaskSystem *tasks, GameMap *map, int newStage);
void TasksRuntime_SyncLogAvailability(TaskSystem *tasks);
void TasksRuntime_GrantLogReward(TaskSystem *tasks, Player *player, ShipLog *log, char *message, size_t messageSize);
void TasksRuntime_DropMonsterRewards(TaskSystem *tasks, Player *player, Monster *monster);
void TasksRuntime_UpdateDayCycle(TaskSystem *tasks, float deltaTime);
void TasksRuntime_RespawnNodes(TaskSystem *tasks, const Player *player, float deltaTime);
void TasksRuntime_EnsureEnergyCoreNode(TaskSystem *tasks, const GameMap *map, const Player *player);
void TasksRuntime_UpdateMonsters(TaskSystem *tasks, const GameMap *map, Player *player, float deltaTime);
bool TasksRuntime_IsOuterSwampLowerRoute(const Player *player);
bool TasksRuntime_IsDeepSwampCore(const Player *player);
bool TasksRuntime_IsTowerPlateau(const Player *player);
bool TasksRuntime_IsMonolithRing(const Player *player);
bool TasksRuntime_IsInSafeRecoveryZone(const GameMap *map, const Player *player, MapArea area);
float TasksRuntime_GetRuinsPreparationFactor(const TaskSystem *tasks, const Player *player);
float TasksRuntime_GetOxygenLeakRate(const TaskSystem *tasks);
float TasksRuntime_GetAreaOxygenDrain(MapArea area, const Player *player);
float TasksRuntime_GetPlayerFilterStrength(const Player *player);
int TasksRuntime_GetPoisonStatusLevel(float poison);
void TasksRuntime_GetLowOxygenThresholds(MapArea area,
                                         const Player *player,
                                         float *lowOxygenThreshold,
                                         float *criticalOxygenThreshold);
void TasksRuntime_ApplyOxygenLeakStatus(Player *player, int level, float duration, float magnitude);
void TasksRuntime_UpdatePlayerSurvival(TaskSystem *tasks, GameMap *map, Player *player, float deltaTime);

SCL_EXTERN_C_END

#endif
