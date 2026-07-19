#ifndef TASK_ENDING_INTERNAL_H
#define TASK_ENDING_INTERNAL_H

#include "task_system.h"

SCL_EXTERN_C_BEGIN

bool TasksEnding_IsHeroicEvidenceReady(const TaskSystem *tasks);
bool TasksEnding_IsPeacefulEvidenceReady(const TaskSystem *tasks);
bool TasksEnding_IsPeacefulAmplifierReady(const TaskSystem *tasks);
bool TasksEnding_IsSettlementEvidenceReady(const TaskSystem *tasks);
bool TasksEnding_IsSettlementBossGateReady(const TaskSystem *tasks);

SCL_EXTERN_C_END
#endif
