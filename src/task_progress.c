#include "task_runtime_internal.h"

#include "task_content.h"
#include "localization.h"

#include <stdio.h>
#include <stdlib.h>

enum {
    LOG_INDEX_SHIP_IMPACT_PROTOCOL = 0,
    LOG_INDEX_SHIP_SPLIT_ROSTER = 1,
    LOG_INDEX_SHIP_PATTERN_NOT_WILDERNESS = 2,
    LOG_INDEX_CANOPY_HANDOFF_RECORD = 5,
    LOG_INDEX_LAST_CAMP_TESTAMENT = 7,
    LOG_INDEX_VENT_CALIBRATION_HANDOVER = 10,
    LOG_INDEX_PURIFIER_RING_CONTROL_BRIEF = 12
};

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

static bool IsCollectedLogIndex(const TaskSystem *tasks, int logIndex) {
    return tasks != NULL
        && logIndex >= 0
        && logIndex < tasks->logCount
        && tasks->logs[logIndex].collected;
}

static bool IsHeroicEndingEvidenceReady(const TaskSystem *tasks) {
    return IsCollectedLogIndex(tasks, LOG_INDEX_CANOPY_HANDOFF_RECORD);
}

static bool IsPeacefulEndingEvidenceReady(const TaskSystem *tasks) {
    return IsCollectedLogIndex(tasks, LOG_INDEX_VENT_CALIBRATION_HANDOVER)
        || IsCollectedLogIndex(tasks, LOG_INDEX_PURIFIER_RING_CONTROL_BRIEF);
}

static bool IsPeacefulEndingAmplifierReady(const TaskSystem *tasks) {
    return tasks != NULL && tasks->signalAmplifierCrafted;
}

static bool IsSettlementEndingEvidenceReady(const TaskSystem *tasks) {
    return IsCollectedLogIndex(tasks, LOG_INDEX_LAST_CAMP_TESTAMENT);
}

static bool IsSettlementEndingBossGateReady(const TaskSystem *tasks) {
    return tasks != NULL && tasks->bossDefeated;
}

static bool FindNearestActiveNodeOfType(const TaskSystem *tasks,
                                        ResourceType resourceType,
                                        const Player *player,
                                        int *gridX,
                                        int *gridY) {
    int bestIndex;
    int bestDistance;
    int index;

    if (tasks == NULL || gridX == NULL || gridY == NULL) {
        return false;
    }

    bestIndex = -1;
    bestDistance = MAP_WIDTH + MAP_HEIGHT + 1;
    for (index = 0; index < tasks->nodeCount; index++) {
        int distance;

        if (!tasks->nodes[index].active || tasks->nodes[index].type != resourceType) {
            continue;
        }

        if (player == NULL) {
            bestIndex = index;
            break;
        }

        distance = abs(tasks->nodes[index].gridX - player->gridX)
            + abs(tasks->nodes[index].gridY - player->gridY);
        if (bestIndex < 0 || distance < bestDistance) {
            bestIndex = index;
            bestDistance = distance;
        }
    }

    if (bestIndex < 0) {
        return false;
    }

    *gridX = tasks->nodes[bestIndex].gridX;
    *gridY = tasks->nodes[bestIndex].gridY;
    return true;
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
    *gridX = ENERGY_CORE_NODE_X;
    *gridY = ENERGY_CORE_NODE_Y;
}

static void SetNextRelicFragmentMarker(const TaskSystem *tasks,
                                       const Player *player,
                                       int *gridX,
                                       int *gridY) {
    if (FindNearestActiveNodeOfType(tasks, RESOURCE_RELIC_FRAGMENT, player, gridX, gridY)) {
        return;
    }

    SetRuinsApproachMarker(gridX, gridY);
}

static void SetNextShipIntroLogMarker(const TaskSystem *tasks, int *gridX, int *gridY) {
    static const int kShipIntroLogIndices[] = {
        LOG_INDEX_SHIP_IMPACT_PROTOCOL,
        LOG_INDEX_SHIP_SPLIT_ROSTER,
        LOG_INDEX_SHIP_PATTERN_NOT_WILDERNESS
    };
    int index;

    if (tasks == NULL) {
        *gridX = OXYGEN_CONSOLE_X;
        *gridY = OXYGEN_CONSOLE_Y;
        return;
    }

    for (index = 0; index < (int)(sizeof(kShipIntroLogIndices) / sizeof(kShipIntroLogIndices[0])); index++) {
        const int logIndex = kShipIntroLogIndices[index];

        if (!IsCollectedLogIndex(tasks, logIndex)) {
            *gridX = tasks->logs[logIndex].gridX;
            *gridY = tasks->logs[logIndex].gridY;
            return;
        }
    }

    *gridX = OXYGEN_CONSOLE_X;
    *gridY = OXYGEN_CONSOLE_Y;
}

static bool HasEnergyCoreExtractionMaterials(const Player *player) {
    return player != NULL
        && player->resources[RESOURCE_JUNK_METAL] >= 1
        && player->resources[RESOURCE_PROTECTIVE_FIBER] >= 1
        && player->resources[RESOURCE_ENERGY_CRYSTAL] >= 1;
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

static void SetStage7UnlockMarker(const TaskSystem *tasks,
                                  const Player *player,
                                  int *gridX,
                                  int *gridY) {
    if (tasks == NULL) {
        *gridX = LOXI_TERMINAL_X;
        *gridY = LOXI_TERMINAL_Y;
        return;
    }

    if (!tasks->endingArchiveReviewed || Tasks_GetAvailableEndingCount(tasks) > 0) {
        *gridX = LOXI_TERMINAL_X;
        *gridY = LOXI_TERMINAL_Y;
        return;
    }

    if (IsPeacefulEndingEvidenceReady(tasks) && !IsPeacefulEndingAmplifierReady(tasks)) {
        if (player != NULL && player->resources[RESOURCE_RELIC_FRAGMENT] >= 3) {
            *gridX = WORKBENCH_X;
            *gridY = WORKBENCH_Y;
        } else {
            SetNextRelicFragmentMarker(tasks, player, gridX, gridY);
        }
        return;
    }

    if (!IsHeroicEndingEvidenceReady(tasks)) {
        *gridX = tasks->logs[LOG_INDEX_CANOPY_HANDOFF_RECORD].gridX;
        *gridY = tasks->logs[LOG_INDEX_CANOPY_HANDOFF_RECORD].gridY;
        return;
    }

    if (!IsPeacefulEndingEvidenceReady(tasks)) {
        const int logIndex = !IsCollectedLogIndex(tasks, LOG_INDEX_VENT_CALIBRATION_HANDOVER)
            ? LOG_INDEX_VENT_CALIBRATION_HANDOVER
            : LOG_INDEX_PURIFIER_RING_CONTROL_BRIEF;

        *gridX = tasks->logs[logIndex].gridX;
        *gridY = tasks->logs[logIndex].gridY;
        return;
    }

    if (!IsSettlementEndingEvidenceReady(tasks)) {
        *gridX = tasks->logs[LOG_INDEX_LAST_CAMP_TESTAMENT].gridX;
        *gridY = tasks->logs[LOG_INDEX_LAST_CAMP_TESTAMENT].gridY;
        return;
    }

    if (!IsSettlementEndingBossGateReady(tasks)) {
        SetBossArenaMarker(tasks, gridX, gridY);
        return;
    }

    *gridX = LOXI_TERMINAL_X;
    *gridY = LOXI_TERMINAL_Y;
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

const char *Tasks_GetLogDetailText(const ShipLog *log) {
    if (log == NULL) {
        return "";
    }

    return Loc_PickLiteral(log->detailTextEn, log->detailTextZh);
}

void TasksRuntime_UnlockStageIfNeeded(TaskSystem *tasks, GameMap *map, int newStage) {
    if (newStage <= tasks->stage) {
        return;
    }

    tasks->stage = newStage;
    if (tasks->stage >= 5) {
        Map_UnlockSwampDeep(map);
        TasksRuntime_EnsureEnergyCoreNode(tasks, map, NULL);
    }
    if (tasks->stage >= 6) {
        Map_UnlockRuins(map);
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

void Tasks_UpdateObjective(TaskSystem *tasks, const Player *player) {
    char objectiveBuffer[sizeof(tasks->objective)];

    if (tasks == NULL) {
        return;
    }

    tasks->signalAmplifierCrafted = player != NULL && player->hasSignalAmplifier;
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
            if (!IsCollectedLogIndex(tasks, LOG_INDEX_SHIP_IMPACT_PROTOCOL)
                || !IsCollectedLogIndex(tasks, LOG_INDEX_SHIP_SPLIT_ROSTER)
                || !IsCollectedLogIndex(tasks, LOG_INDEX_SHIP_PATTERN_NOT_WILDERNESS)) {
                SetNextShipIntroLogMarker(tasks, gridX, gridY);
            } else {
                *gridX = OXYGEN_CONSOLE_X;
                *gridY = OXYGEN_CONSOLE_Y;
            }
            return true;
        case 2:
            if (!IsCollectedLogIndex(tasks, LOG_INDEX_SHIP_IMPACT_PROTOCOL)
                || !IsCollectedLogIndex(tasks, LOG_INDEX_SHIP_SPLIT_ROSTER)
                || !IsCollectedLogIndex(tasks, LOG_INDEX_SHIP_PATTERN_NOT_WILDERNESS)) {
                SetNextShipIntroLogMarker(tasks, gridX, gridY);
            } else if (player != NULL && !player->hasGlowStick) {
                *gridX = WORKBENCH_X;
                *gridY = WORKBENCH_Y;
            } else {
                *gridX = OXYGEN_CONSOLE_X;
                *gridY = OXYGEN_CONSOLE_Y;
            }
            return true;
        case 6:
            if (player != NULL && player->resources[RESOURCE_RELIC_FRAGMENT] >= 3) {
                *gridX = LOXI_TERMINAL_X;
                *gridY = LOXI_TERMINAL_Y;
            } else {
                SetNextRelicFragmentMarker(tasks, player, gridX, gridY);
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
            if (player != NULL && (!player->hasLaserGun || !player->hasProtectionSuit)) {
                *gridX = WORKBENCH_X;
                *gridY = WORKBENCH_Y;
            } else {
                *gridX = CRASH_CLUE_X;
                *gridY = CRASH_CLUE_Y;
            }
            return true;
        case 5:
            if (player != NULL && player->resources[RESOURCE_ENERGY_CORE] > 0) {
                *gridX = ENERGY_CONSOLE_X;
                *gridY = ENERGY_CONSOLE_Y;
            } else if (HasEnergyCoreExtractionMaterials(player)) {
                *gridX = CRASH_CLUE_X;
                *gridY = CRASH_CLUE_Y;
            } else {
                SetDeepSwampMarker(gridX, gridY);
            }
            return true;
        case 7:
            if (Tasks_IsEndingBranchReady(tasks) && tasks->selectedEndingRoute == ENDING_NONE) {
                SetStage7UnlockMarker(tasks, player, gridX, gridY);
            } else if (tasks->selectedEndingRoute == ENDING_PEACEFUL) {
                if (player != NULL && player->hasSignalAmplifier) {
                    *gridX = SIGNAL_TOWER_X;
                    *gridY = SIGNAL_TOWER_Y;
                } else if (player != NULL && player->resources[RESOURCE_RELIC_FRAGMENT] >= 3) {
                    *gridX = WORKBENCH_X;
                    *gridY = WORKBENCH_Y;
                } else {
                    SetRuinsApproachMarker(gridX, gridY);
                }
            } else if (tasks->selectedEndingRoute == ENDING_HEROIC) {
                if (tasks->bossDefeated) {
                    *gridX = SIGNAL_TOWER_X;
                    *gridY = SIGNAL_TOWER_Y;
                } else {
                    SetBossArenaMarker(tasks, gridX, gridY);
                }
            } else if (tasks->selectedEndingRoute == ENDING_SETTLEMENT) {
                if (tasks->bossDefeated) {
                    *gridX = LOXI_TERMINAL_X;
                    *gridY = LOXI_TERMINAL_Y;
                } else {
                    SetBossArenaMarker(tasks, gridX, gridY);
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
    if (!Tasks_IsEndingAvailable(tasks, ending)) {
        return false;
    }

    tasks->selectedEndingRoute = ending;
    return true;
}

bool Tasks_IsEndingAvailable(const TaskSystem *tasks, GameEnding ending) {
    if (!IsEndingRouteSelectionReady(tasks)) {
        return false;
    }

    switch (ending) {
        case ENDING_HEROIC:
            return IsHeroicEndingEvidenceReady(tasks);
        case ENDING_PEACEFUL:
            return IsPeacefulEndingEvidenceReady(tasks)
                && IsPeacefulEndingAmplifierReady(tasks);
        case ENDING_SETTLEMENT:
            return IsSettlementEndingEvidenceReady(tasks)
                && IsSettlementEndingBossGateReady(tasks);
        case ENDING_FAILURE:
        case ENDING_NONE:
        default:
            return false;
    }
}

int Tasks_GetAvailableEndingCount(const TaskSystem *tasks) {
    int count = 0;

    count += Tasks_IsEndingAvailable(tasks, ENDING_HEROIC) ? 1 : 0;
    count += Tasks_IsEndingAvailable(tasks, ENDING_PEACEFUL) ? 1 : 0;
    count += Tasks_IsEndingAvailable(tasks, ENDING_SETTLEMENT) ? 1 : 0;
    return count;
}

GameEnding Tasks_GetAvailableEndingAt(const TaskSystem *tasks, int index) {
    int currentIndex = 0;

    if (index < 0) {
        return ENDING_NONE;
    }
    if (Tasks_IsEndingAvailable(tasks, ENDING_HEROIC)) {
        if (currentIndex == index) {
            return ENDING_HEROIC;
        }
        currentIndex += 1;
    }
    if (Tasks_IsEndingAvailable(tasks, ENDING_PEACEFUL)) {
        if (currentIndex == index) {
            return ENDING_PEACEFUL;
        }
        currentIndex += 1;
    }
    if (Tasks_IsEndingAvailable(tasks, ENDING_SETTLEMENT)) {
        if (currentIndex == index) {
            return ENDING_SETTLEMENT;
        }
    }

    return ENDING_NONE;
}

bool Tasks_CanChooseSettlement(const TaskSystem *tasks) {
    return tasks != NULL
        && Tasks_IsEndingAvailable(tasks, ENDING_SETTLEMENT)
        && tasks->selectedEndingRoute == ENDING_NONE
        && tasks->ending == ENDING_NONE;
}

void Tasks_CommitSettlement(TaskSystem *tasks) {
    if (tasks == NULL) {
        return;
    }

    if (tasks->selectedEndingRoute == ENDING_NONE) {
        if (!Tasks_SelectEndingRoute(tasks, ENDING_SETTLEMENT)) {
            return;
        }
    }

    if (tasks->selectedEndingRoute == ENDING_SETTLEMENT && tasks->bossDefeated) {
        tasks->ending = ENDING_SETTLEMENT;
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

static int CountCompletedInvestigationSegments(const TaskSystem *tasks) {
    int completedCount;

    if (tasks == NULL) {
        return 0;
    }

    completedCount = 0;
    completedCount += tasks->westW1Completed ? 1 : 0;
    completedCount += tasks->westW2Completed ? 1 : 0;
    completedCount += tasks->westW3Completed ? 1 : 0;
    completedCount += tasks->westW4Completed ? 1 : 0;
    completedCount += tasks->westW5Completed ? 1 : 0;
    completedCount += tasks->southS1Completed ? 1 : 0;
    completedCount += tasks->southS2Completed ? 1 : 0;
    completedCount += tasks->southS3Completed ? 1 : 0;
    completedCount += tasks->southS4Completed ? 1 : 0;
    completedCount += tasks->southS5Completed ? 1 : 0;
    return completedCount;
}

int Tasks_GetArchiveScore(const TaskSystem *tasks) {
    int mainlineLogs;
    int supplementalLogs;
    int shownMainStoryScenes;
    int score;

    if (tasks == NULL) {
        return 0;
    }

    mainlineLogs = CountCollectedLogsForCategory(tasks, SHIP_LOG_MAINLINE);
    supplementalLogs = CountCollectedLogsForCategory(tasks, SHIP_LOG_SUPPLEMENTAL);
    shownMainStoryScenes = tasks->shownMainStorySceneCount;
    if (shownMainStoryScenes < 0) {
        shownMainStoryScenes = 0;
    }
    if (shownMainStoryScenes > STORY_MAIN_SCENE_COUNT) {
        shownMainStoryScenes = STORY_MAIN_SCENE_COUNT;
    }

    score = 0;
    score += mainlineLogs * 35;
    score += supplementalLogs * 15;
    score += shownMainStoryScenes * 2;
    score += shownMainStoryScenes >= STORY_MAIN_SCENE_COUNT ? 20 : 0;
    return score;
}

int Tasks_GetInvestigationScore(const TaskSystem *tasks) {
    int score;

    if (tasks == NULL) {
        return 0;
    }

    score = 0;
    score += CountCompletedInvestigationSegments(tasks) * 10;
    score += tasks->monolithsLit * 20;
    score += tasks->endingArchiveReviewed ? 25 : 0;
    score += AreFinalArchiveTasksComplete(tasks) ? 60 : 0;
    return score;
}

int Tasks_GetSurvivalScore(const TaskSystem *tasks, const Player *player) {
    int deathBonus;
    int score;

    (void)tasks;

    if (player == NULL) {
        return 0;
    }

    score = 0;
    if (player->hasSignalAmplifier) {
        score += 45;
    }

    deathBonus = 120 - player->deathCount * 20;
    if (deathBonus > 0) {
        score += deathBonus;
    }
    if (player->health > 0.0f) {
        score += 20;
    }
    if (player->oxygen > 0.0f) {
        score += 20;
    }
    return score;
}

int Tasks_GetEndingCompletionScore(const TaskSystem *tasks, const Player *player) {
    (void)player;

    if (tasks == NULL) {
        return 0;
    }

    switch (tasks->ending) {
        case ENDING_HEROIC:
        case ENDING_PEACEFUL:
        case ENDING_SETTLEMENT:
            return 140;
        case ENDING_FAILURE:
        case ENDING_NONE:
        default:
            return 0;
    }
}

int Tasks_GetCombatEncounterCount(void) {
    return COMBAT_ENCOUNTER_COUNT - 1;
}

int Tasks_GetCombatEncounterScore(CombatEncounterId encounter) {
    switch (encounter) {
        case COMBAT_ENCOUNTER_WEST_FRONTIER:
        case COMBAT_ENCOUNTER_CANOPY_HOLLOW:
        case COMBAT_ENCOUNTER_ECHO_BASIN:
        case COMBAT_ENCOUNTER_DEEP_BASIN:
        case COMBAT_ENCOUNTER_SOUTH_COLLAPSE:
        case COMBAT_ENCOUNTER_ROOT_VAULT:
            return 10;
        case COMBAT_ENCOUNTER_RELIC_GUARD:
            return 20;
        case COMBAT_ENCOUNTER_FINAL_BOSS:
            return 40;
        case COMBAT_ENCOUNTER_NONE:
        case COMBAT_ENCOUNTER_COUNT:
        default:
            return 0;
    }
}

const char *Tasks_GetCombatEncounterName(CombatEncounterId encounter) {
    switch (encounter) {
        case COMBAT_ENCOUNTER_WEST_FRONTIER:
            return Loc_PickLiteral("West Frontier", "西部前线");
        case COMBAT_ENCOUNTER_CANOPY_HOLLOW:
            return Loc_PickLiteral("Canopy Hollow", "林冠洼地");
        case COMBAT_ENCOUNTER_ECHO_BASIN:
            return Loc_PickLiteral("Echo Basin", "回声盆地");
        case COMBAT_ENCOUNTER_DEEP_BASIN:
            return Loc_PickLiteral("Deep Basin", "深层盆地");
        case COMBAT_ENCOUNTER_SOUTH_COLLAPSE:
            return Loc_PickLiteral("South Collapse", "南部塌陷口");
        case COMBAT_ENCOUNTER_ROOT_VAULT:
            return Loc_PickLiteral("Root Vault", "根脉核心库");
        case COMBAT_ENCOUNTER_RELIC_GUARD:
            return Loc_PickLiteral("Relic Guard", "遗迹守卫");
        case COMBAT_ENCOUNTER_FINAL_BOSS:
            return Loc_PickLiteral("Final Boss", "最终守卫");
        case COMBAT_ENCOUNTER_NONE:
        case COMBAT_ENCOUNTER_COUNT:
        default:
            return "";
    }
}

bool Tasks_IsCombatEncounterCompleted(const TaskSystem *tasks, CombatEncounterId encounter) {
    int index;

    if (tasks == NULL || encounter <= COMBAT_ENCOUNTER_NONE || encounter >= COMBAT_ENCOUNTER_COUNT) {
        return false;
    }
    if (encounter == COMBAT_ENCOUNTER_FINAL_BOSS && tasks->bossDefeated) {
        return true;
    }

    for (index = 0; index < tasks->monsterCount; index++) {
        const Monster *monster;

        monster = &tasks->monsters[index];
        if (monster->encounterId != encounter) {
            continue;
        }
        return !monster->active || monster->health <= 0.0f;
    }

    return false;
}

int Tasks_GetCombatScore(const TaskSystem *tasks) {
    int score;
    int encounter;

    if (tasks == NULL) {
        return 0;
    }

    score = 0;
    for (encounter = COMBAT_ENCOUNTER_WEST_FRONTIER; encounter < COMBAT_ENCOUNTER_COUNT; encounter++) {
        const CombatEncounterId encounterId = (CombatEncounterId)encounter;

        if (Tasks_IsCombatEncounterCompleted(tasks, encounterId)) {
            score += Tasks_GetCombatEncounterScore(encounterId);
        }
    }

    return score;
}

int Tasks_GetCombatScoreMax(void) {
    int score;
    int encounter;

    score = 0;
    for (encounter = COMBAT_ENCOUNTER_WEST_FRONTIER; encounter < COMBAT_ENCOUNTER_COUNT; encounter++) {
        score += Tasks_GetCombatEncounterScore((CombatEncounterId)encounter);
    }

    return score;
}

int Tasks_CalculateEndingScore(const TaskSystem *tasks, const Player *player) {
    int score;

    if (tasks == NULL) {
        return 0;
    }

    score = 0;
    score += Tasks_GetArchiveScore(tasks);
    score += Tasks_GetInvestigationScore(tasks);
    score += Tasks_GetCombatScore(tasks);
    score += Tasks_GetSurvivalScore(tasks, player);
    score += Tasks_GetEndingCompletionScore(tasks, player);

    return score;
}

const char *Tasks_GetEndingScoreRank(int score) {
    if (score >= 760) {
        return "A";
    }
    if (score >= 620) {
        return "B";
    }
    if (score >= 480) {
        return "C";
    }
    if (score >= 340) {
        return "D";
    }
    return "E";
}
