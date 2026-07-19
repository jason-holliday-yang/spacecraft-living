#ifndef TASK_PRESENTATION_H
#define TASK_PRESENTATION_H

#include "c_compat.h"
#include "task_system.h"

typedef struct AssetBundle AssetBundle;

/* Task/world rendering and localized task/ending presentation queries. */

SCL_EXTERN_C_BEGIN

void Tasks_DrawWorld(const TaskSystem *tasks, const AssetBundle *assets, float elapsedSeconds);
void Tasks_UpdateObjective(TaskSystem *tasks, const Player *player);
const char *Tasks_GetLogTitle(const ShipLog *log);
const char *Tasks_GetLogStoryText(const ShipLog *log);
const char *Tasks_GetLogDetailText(const ShipLog *log);
const char *Tasks_GetStageName(int stage);
const char *Tasks_GetPhaseName(DayPhase phase);
const char *Tasks_GetEventName(EventType eventType);
const char *Tasks_GetCommunicatorHint(const TaskSystem *tasks);
const char *Tasks_GetEndingTitle(GameEnding ending);
const char *Tasks_GetEndingBody(GameEnding ending);
const char *Tasks_GetEndingScoreRank(int score);
const char *Tasks_GetCombatEncounterName(CombatEncounterId encounter);

SCL_EXTERN_C_END

#endif
