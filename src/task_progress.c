#include "task_runtime_internal.h"

#include "task_content.h"
#include "localization.h"

#include <stdio.h>

static int CountCollectedLogsForCategory(const TaskSystem *tasks, ShipLogCategory category) {
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

static int CountTotalLogsForCategory(const TaskSystem *tasks, ShipLogCategory category) {
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

static bool AreFinalArchiveTasksComplete(const TaskSystem *tasks) {
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

static bool IsEndingRouteSelectionReady(const TaskSystem *tasks) {
    return Tasks_IsEndingBranchReady(tasks)
        && tasks != NULL
        && tasks->endingArchiveReviewed;
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

static void SetRuinsApproachMarker(int *gridX, int *gridY) {
    *gridX = EXTERIOR_X(64);
    *gridY = EXTERIOR_Y(27);
}

static void SetDeepSwampMarker(int *gridX, int *gridY) {
    *gridX = EXTERIOR_X(112);
    *gridY = EXTERIOR_Y(56);
}

static void SetNextMonolithMarker(const TaskSystem *tasks, int *gridX, int *gridY) {
    if (!tasks->monolithActivated[1]) {
        *gridX = MONOLITH_B_X;
        *gridY = MONOLITH_B_Y;
        return;
    }
    if (!tasks->monolithActivated[0]) {
        *gridX = MONOLITH_A_X;
        *gridY = MONOLITH_A_Y;
        return;
    }
    if (!tasks->monolithActivated[2]) {
        *gridX = MONOLITH_C_X;
        *gridY = MONOLITH_C_Y;
        return;
    }

    *gridX = SIGNAL_TOWER_X;
    *gridY = SIGNAL_TOWER_Y;
}

static void SetBossArenaMarker(const TaskSystem *tasks, int *gridX, int *gridY) {
    int index;

    if (tasks != NULL) {
        for (index = 0; index < tasks->monsterCount; index++) {
            const Monster *monster;

            monster = &tasks->monsters[index];
            if (monster->active && monster->type == MONSTER_FINAL_BOSS) {
                *gridX = monster->gridX;
                *gridY = monster->gridY;
                return;
            }
        }
    }

    *gridX = BOSS_ARENA_BOSS_X;
    *gridY = BOSS_ARENA_BOSS_Y;
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

static void UpdateCommunicatorText(TaskSystem *tasks, const Player *player) {
    const char *areaName;
    const char *locationName;
    const char *guidance;
    const char *fieldNote;
    char guidanceBuffer[512];
    char fieldNoteBuffer[512];

    if (tasks == NULL || player == NULL) {
        return;
    }

    areaName = Loc_GetAreaNameText(Map_GetAreaName(Map_GetAreaAt(player->gridX, player->gridY)));
    locationName = Loc_GetLocationNameText(Map_GetLocationNameAt(player->gridX, player->gridY));
    guidance = TasksContent_GetStageGuidance(tasks, player);
    fieldNote = TasksContent_GetFieldNote(tasks, player);
    TasksRuntime_SanitizeDisplayText(guidance, guidanceBuffer, sizeof(guidanceBuffer));
    TasksRuntime_SanitizeDisplayText(fieldNote, fieldNoteBuffer, sizeof(fieldNoteBuffer));
    snprintf(tasks->communicator,
             sizeof(tasks->communicator),
             "%s: %s\n%s: %s\n%s: %s\n%s: %s\n%s: %s",
             Loc_PickLiteral("Current Area", "当前区域"),
             areaName,
             Loc_PickLiteral("Location", "位置"),
             locationName,
             Loc_PickLiteral("Task Brief", "任务简报"),
             tasks->objective,
             Loc_PickLiteral("Loxi Tip", "洛希提示"),
             guidanceBuffer,
             Loc_PickLiteral("Field Note", "现场备注"),
             fieldNoteBuffer);
}

const char *Tasks_GetLogTitle(const ShipLog *log) {
    if (log == NULL) {
        return "";
    }

    return Loc_PickLiteral(log->titleEn, log->titleZh);
}

const char *Tasks_GetLogStoryText(const ShipLog *log) {
    if (log == NULL) {
        return "";
    }

    return Loc_PickLiteral(log->storyTextEn, log->storyTextZh);
}

const char *Tasks_GetLogRewardDescription(const ShipLog *log) {
    if (log == NULL) {
        return "";
    }

    return Loc_PickLiteral(log->rewardDescEn, log->rewardDescZh);
}

void TasksRuntime_UnlockStageIfNeeded(TaskSystem *tasks, GameMap *map, int newStage) {
    if (newStage <= tasks->stage) {
        return;
    }

    tasks->stage = newStage;
    if (tasks->stage >= 5) {
        Map_UnlockSwampDeep(map);
    }
    if (tasks->stage >= 6) {
        Map_UnlockRuins(map);
    }
}

void TasksRuntime_GrantLogReward(TaskSystem *tasks, Player *player, ShipLog *log, char *message, size_t messageSize) {
    TaskLogRewardSpec rewardSpec;
    int resourceIndex;

    log->collected = true;

    if (TasksContent_GetLogRewardSpec(log->rewardKind, &rewardSpec)) {
        player->maxHealthBonus += rewardSpec.maxHealthBonus;
        player->attackBonus += rewardSpec.attackBonus;
        if (rewardSpec.oxygen > 0.0f) {
            Player_AddOxygen(player, rewardSpec.oxygen);
        }
        if (rewardSpec.pressureRelief > 0.0f) {
            Player_RelievePressure(player, rewardSpec.pressureRelief);
        }
        for (resourceIndex = 0; resourceIndex < RESOURCE_COUNT; resourceIndex++) {
            if (rewardSpec.resourceCounts[resourceIndex] > 0) {
                Player_AddResource(player, (ResourceType)resourceIndex, rewardSpec.resourceCounts[resourceIndex]);
            }
        }
    }

    snprintf(message,
             messageSize,
             "%s %s.\n%s\n%s: %s",
             Loc_PickLiteral("Recovered", "已回收"),
             Tasks_GetLogTitle(log),
             Loc_PickLiteral("Open the Ship Log Archive with L to read it.", "按 L 打开飞船日志档案查看详情。"),
             Loc_PickLiteral("Reward", "奖励"),
             Tasks_GetLogRewardDescription(log));
    Tasks_UpdateObjective(tasks, player);
}

void Tasks_UpdateObjective(TaskSystem *tasks, const Player *player) {
    char objectiveBuffer[sizeof(tasks->objective)];

    if (tasks == NULL) {
        return;
    }

    TasksRuntime_SyncLogAvailability(tasks);
    TasksRuntime_SanitizeDisplayText(TasksContent_GetStageObjective(tasks, player), objectiveBuffer, sizeof(objectiveBuffer));
    snprintf(tasks->objective, sizeof(tasks->objective), "%s", objectiveBuffer);
    UpdateCommunicatorText(tasks, player);
}

void TasksRuntime_DropMonsterRewards(TaskSystem *tasks, Player *player, Monster *monster) {
    TaskMonsterSpec monsterSpec;
    int resourceIndex;

    if (!TasksContent_GetMonsterSpec(monster->type, &monsterSpec)) {
        return;
    }

    for (resourceIndex = 0; resourceIndex < RESOURCE_COUNT; resourceIndex++) {
        if (monsterSpec.rewardCounts[resourceIndex] > 0) {
            Player_AddResource(player, (ResourceType)resourceIndex, monsterSpec.rewardCounts[resourceIndex]);
        }
    }

    if (monsterSpec.defeatsBossOnKill) {
        tasks->bossDefeated = true;
    }
}

bool Tasks_GetObjectiveMarker(const TaskSystem *tasks, const Player *player, int *gridX, int *gridY) {
    switch (tasks->stage) {
        case 1:
        case 2:
            *gridX = OXYGEN_CONSOLE_X;
            *gridY = OXYGEN_CONSOLE_Y;
            return true;
        case 6:
            if (player != NULL && player->resources[RESOURCE_RELIC_FRAGMENT] >= 3) {
                *gridX = LOXI_TERMINAL_X;
                *gridY = LOXI_TERMINAL_Y;
            } else {
                SetRuinsApproachMarker(gridX, gridY);
            }
            return true;
        case 3:
            if (Map_GetAreaAt(player->gridX, player->gridY) == MAP_AREA_BASE) {
                *gridX = AIRLOCK_CONSOLE_X;
                *gridY = AIRLOCK_CONSOLE_Y;
            } else {
                *gridX = COMM_RELAY_X;
                *gridY = COMM_RELAY_Y;
            }
            return true;
        case 4:
            *gridX = CRASH_CLUE_X;
            *gridY = CRASH_CLUE_Y;
            return true;
        case 5:
            if (player != NULL && player->resources[RESOURCE_ENERGY_CORE] > 0) {
                *gridX = ENERGY_CONSOLE_X;
                *gridY = ENERGY_CONSOLE_Y;
            } else {
                SetDeepSwampMarker(gridX, gridY);
            }
            return true;
        case 7:
            if (Tasks_IsEndingBranchReady(tasks) && tasks->selectedEndingRoute == ENDING_NONE) {
                *gridX = LOXI_TERMINAL_X;
                *gridY = LOXI_TERMINAL_Y;
            } else if (tasks->selectedEndingRoute == ENDING_PEACEFUL) {
                if (player != NULL && player->hasSignalAmplifier) {
                    *gridX = SIGNAL_TOWER_X;
                    *gridY = SIGNAL_TOWER_Y;
                } else {
                    *gridX = WORKBENCH_X;
                    *gridY = WORKBENCH_Y;
                }
            } else if (tasks->selectedEndingRoute == ENDING_HEROIC) {
                if (tasks->bossDefeated) {
                    *gridX = SIGNAL_TOWER_X;
                    *gridY = SIGNAL_TOWER_Y;
                } else if (player != NULL && Map_GetAreaAt(player->gridX, player->gridY) == MAP_AREA_BOSS_ARENA) {
                    SetBossArenaMarker(tasks, gridX, gridY);
                } else {
                    *gridX = AIRLOCK_CONSOLE_X;
                    *gridY = AIRLOCK_CONSOLE_Y;
                }
            } else {
                SetNextMonolithMarker(tasks, gridX, gridY);
            }
            return true;
        default:
            return false;
    }
}

const char *Tasks_GetStageName(int stage) {
    return TasksContent_GetStageNameText(stage);
}

const char *Tasks_GetPhaseName(DayPhase phase) {
    return TasksContent_GetPhaseNameText(phase);
}

const char *Tasks_GetEventName(EventType eventType) {
    return TasksContent_GetEventNameText(eventType);
}

bool Tasks_IsCommunicatorUnlocked(const TaskSystem *tasks) {
    return tasks->communicatorUnlocked;
}

const char *Tasks_GetCommunicatorHint(const TaskSystem *tasks) {
    return tasks->communicator;
}

bool Tasks_IsEndingBranchReady(const TaskSystem *tasks) {
    return AreFinalArchiveTasksComplete(tasks)
        && tasks != NULL
        && CountCollectedLogsForCategory(tasks, SHIP_LOG_MAINLINE)
            >= CountTotalLogsForCategory(tasks, SHIP_LOG_MAINLINE);
}

GameEnding Tasks_GetSelectedEndingRoute(const TaskSystem *tasks) {
    if (tasks == NULL) {
        return ENDING_NONE;
    }

    return tasks->selectedEndingRoute;
}

bool Tasks_SelectEndingRoute(TaskSystem *tasks, GameEnding ending) {
    if (tasks == NULL
        || tasks->ending != ENDING_NONE
        || tasks->selectedEndingRoute != ENDING_NONE
        || !IsEndingRouteSelectionReady(tasks)) {
        return false;
    }

    if (ending != ENDING_HEROIC && ending != ENDING_PEACEFUL && ending != ENDING_SETTLEMENT) {
        return false;
    }

    tasks->selectedEndingRoute = ending;
    if (ending == ENDING_SETTLEMENT) {
        tasks->ending = ENDING_SETTLEMENT;
    }

    return true;
}

bool Tasks_CanChooseSettlement(const TaskSystem *tasks) {
    return tasks != NULL
        && IsEndingRouteSelectionReady(tasks)
        && tasks->selectedEndingRoute == ENDING_NONE
        && tasks->ending == ENDING_NONE;
}

void Tasks_CommitSettlement(TaskSystem *tasks) {
    if (tasks == NULL || !Tasks_SelectEndingRoute(tasks, ENDING_SETTLEMENT)) {
        return;
    }
}

GameEnding Tasks_GetEnding(const TaskSystem *tasks) {
    return tasks->ending;
}

const char *Tasks_GetEndingTitle(GameEnding ending) {
    return TasksContent_GetEndingTitleText(ending);
}

const char *Tasks_GetEndingBody(GameEnding ending) {
    return TasksContent_GetEndingBodyText(ending);
}
