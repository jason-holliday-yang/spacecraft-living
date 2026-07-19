#include "task_ending_internal.h"
#include "task_archive_internal.h"

static bool IsEndingRouteSelectionReady(const TaskSystem *tasks) {
    return tasks != NULL
        && Tasks_IsEndingBranchReady(tasks);
}


bool TasksEnding_IsHeroicEvidenceReady(const TaskSystem *tasks) {
    return TasksArchive_IsCollectedLogIndex(tasks, LOG_INDEX_CANOPY_HANDOFF_RECORD);
}


bool TasksEnding_IsPeacefulEvidenceReady(const TaskSystem *tasks) {
    return TasksArchive_IsCollectedLogIndex(tasks, LOG_INDEX_VENT_CALIBRATION_HANDOVER)
        || TasksArchive_IsCollectedLogIndex(tasks, LOG_INDEX_PURIFIER_RING_CONTROL_BRIEF);
}


bool TasksEnding_IsPeacefulAmplifierReady(const TaskSystem *tasks) {
    return tasks != NULL && tasks->signalAmplifierCrafted;
}


bool TasksEnding_IsSettlementEvidenceReady(const TaskSystem *tasks) {
    return TasksArchive_IsCollectedLogIndex(tasks, LOG_INDEX_LAST_CAMP_TESTAMENT);
}


bool TasksEnding_IsSettlementBossGateReady(const TaskSystem *tasks) {
    return tasks != NULL && tasks->bossDefeated;
}


bool Tasks_IsEndingPreCheckReady(const TaskSystem *tasks) {
    return tasks != NULL
        && TasksArchive_AreFinalTasksComplete(tasks)
        && TasksArchive_CountCollectedLogsForCategory(tasks, SHIP_LOG_MAINLINE)
            >= TasksArchive_CountTotalLogsForCategory(tasks, SHIP_LOG_MAINLINE);
}


bool Tasks_IsEndingBranchReady(const TaskSystem *tasks) {
    return Tasks_IsEndingPreCheckReady(tasks)
        && tasks->endingArchiveReviewed;
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
            return TasksEnding_IsHeroicEvidenceReady(tasks);
        case ENDING_PEACEFUL:
            return TasksEnding_IsPeacefulEvidenceReady(tasks)
                && TasksEnding_IsPeacefulAmplifierReady(tasks);
        case ENDING_SETTLEMENT:
            return TasksEnding_IsSettlementEvidenceReady(tasks)
                && TasksEnding_IsSettlementBossGateReady(tasks);
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

    mainlineLogs = TasksArchive_CountCollectedLogsForCategory(tasks, SHIP_LOG_MAINLINE);
    supplementalLogs = TasksArchive_CountCollectedLogsForCategory(tasks, SHIP_LOG_SUPPLEMENTAL);
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
    score += TasksArchive_AreFinalTasksComplete(tasks) ? 60 : 0;
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
