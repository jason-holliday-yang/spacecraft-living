#include "task_archive_internal.h"
#include "task_runtime_internal.h"
#include "task_presentation.h"

#include "localization.h"
#include "task_content.h"

#include <stdio.h>

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

const char *Tasks_GetStageName(int stage) {
    return TasksContent_GetStageNameText(stage);
}

const char *Tasks_GetPhaseName(DayPhase phase) {
    return TasksContent_GetPhaseNameText(phase);
}

const char *Tasks_GetEventName(EventType eventType) {
    return TasksContent_GetEventNameText(eventType);
}

const char *Tasks_GetCommunicatorHint(const TaskSystem *tasks) {
    return tasks->communicator;
}

const char *Tasks_GetEndingTitle(GameEnding ending) {
    return TasksContent_GetEndingTitleText(ending);
}

const char *Tasks_GetEndingBody(GameEnding ending) {
    return TasksContent_GetEndingBodyText(ending);
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
