#ifndef TASK_ARCHIVE_INTERNAL_H
#define TASK_ARCHIVE_INTERNAL_H

#include "task_system.h"

SCL_EXTERN_C_BEGIN

enum {
    LOG_INDEX_SHIP_IMPACT_PROTOCOL = 0,
    LOG_INDEX_SHIP_SPLIT_ROSTER = 1,
    LOG_INDEX_SHIP_PATTERN_NOT_WILDERNESS = 2,
    LOG_INDEX_CANOPY_HANDOFF_RECORD = 5,
    LOG_INDEX_LAST_CAMP_TESTAMENT = 7,
    LOG_INDEX_VENT_CALIBRATION_HANDOVER = 10,
    LOG_INDEX_PURIFIER_RING_CONTROL_BRIEF = 12
};

bool TasksArchive_IsCollectedLogIndex(const TaskSystem *tasks, int logIndex);
int TasksArchive_CountCollectedLogsForCategory(const TaskSystem *tasks, ShipLogCategory category);
int TasksArchive_CountTotalLogsForCategory(const TaskSystem *tasks, ShipLogCategory category);
bool TasksArchive_AreFinalTasksComplete(const TaskSystem *tasks);
void TasksRuntime_SyncLogAvailability(TaskSystem *tasks);
void TasksRuntime_GrantLogReward(TaskSystem *tasks, Player *player, ShipLog *log, char *message, size_t messageSize);

SCL_EXTERN_C_END
#endif
