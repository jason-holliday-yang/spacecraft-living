#include "task_archive_internal.h"
#include "task_runtime_internal.h"
#include "task_presentation.h"

#include "localization.h"

#include <stdio.h>

int TasksArchive_CountCollectedLogsForCategory(const TaskSystem *tasks, ShipLogCategory category) {
    int count;
    int index;

    if (tasks == NULL) {
        return 0;
    }

    count = 0;
    for (index = 0; index < tasks->logCount; index++) {
        if (tasks->logs[index].category == category && tasks->logs[index].collected) {
            count += 1;
        }
    }

    return count;
}


int TasksArchive_CountTotalLogsForCategory(const TaskSystem *tasks, ShipLogCategory category) {
    int count;
    int index;

    if (tasks == NULL) {
        return 0;
    }

    count = 0;
    for (index = 0; index < tasks->logCount; index++) {
        if (tasks->logs[index].category == category) {
            count += 1;
        }
    }

    return count;
}


bool TasksArchive_AreFinalTasksComplete(const TaskSystem *tasks) {
    return tasks != NULL
        && tasks->stage >= 7
        && tasks->oxygenRepairLevel >= 2
        && tasks->commRepairLevel >= 1
        && tasks->energyRepairLevel >= 1
        && tasks->crashClueFound
        && tasks->amplifierUnlocked
        && tasks->westW5Completed
        && tasks->southS5Completed;
}


bool TasksArchive_IsCollectedLogIndex(const TaskSystem *tasks, int logIndex) {
    return tasks != NULL
        && logIndex >= 0
        && logIndex < tasks->logCount
        && tasks->logs[logIndex].collected;
}


static bool IsLogUnlockedByIndex(const TaskSystem *tasks, int logIndex) {
    switch (logIndex) {
        case 0:
        case 1:
        case 2:
            return true;
        case 3:
            return tasks->commRepairLevel >= 1;
        case 4:
            return tasks->westW1Completed;
        case 5:
            return tasks->westW2Completed;
        case 6:
            return tasks->westW3Completed;
        case 7:
            return tasks->westW4Completed;
        case 8:
            return tasks->crashClueFound;
        case 9:
            return tasks->energyRepairLevel >= 1;
        case 10:
            return tasks->southS1Completed;
        case 11:
            return tasks->southS2Completed;
        case 12:
            return tasks->southS3Completed;
        case 13:
            return tasks->southS4Completed;
        default:
            return true;
    }
}


void TasksRuntime_SyncLogAvailability(TaskSystem *tasks) {
    int index;

    if (tasks == NULL) {
        return;
    }

    for (index = 0; index < tasks->logCount; index++) {
        ShipLog *log;

        log = &tasks->logs[index];
        log->active = log->collected || IsLogUnlockedByIndex(tasks, index);
    }
}


void TasksRuntime_GrantLogReward(TaskSystem *tasks, Player *player, ShipLog *log, char *message, size_t messageSize) {
    log->collected = true;

    snprintf(message,
             messageSize,
             "%s %s.\n%s",
             Loc_PickLiteral("Recovered", "已回收"),
             Tasks_GetLogTitle(log),
             Loc_PickLiteral("Press N to review the recovered log, its story context, and your current objective. Log recovery now advances the archive without granting extra stats or item rewards.",
                             "按 N 查看这份日志、对应背景以及当前目标。日志回收现在只推进档案本身，不再额外给予属性或物资奖励。"));
    Tasks_UpdateObjective(tasks, player);
}
