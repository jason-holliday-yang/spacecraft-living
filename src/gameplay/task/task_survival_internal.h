#ifndef TASK_SURVIVAL_INTERNAL_H
#define TASK_SURVIVAL_INTERNAL_H

#include "task_system.h"

SCL_EXTERN_C_BEGIN

void TasksRuntime_ReducePoisonAtRecovery(Player *player, float amount);
void TasksRuntime_DowngradeOxygenLeakAtRecovery(Player *player);
void TasksRuntime_ClearNegativeSurvivalStatuses(Player *player);
void TasksRuntime_UpdateDayCycle(TaskSystem *tasks, float deltaTime);
void TasksRuntime_RespawnNodes(TaskSystem *tasks, const Player *player, float deltaTime);
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
void TasksRuntime_GetLowOxygenThresholds(MapArea area, const Player *player, float *lowOxygenThreshold, float *criticalOxygenThreshold);
void TasksRuntime_ApplyOxygenLeakStatus(Player *player, int level, float duration, float magnitude);
void TasksRuntime_UpdatePlayerSurvival(TaskSystem *tasks, GameMap *map, Player *player, float deltaTime);

SCL_EXTERN_C_END
#endif
