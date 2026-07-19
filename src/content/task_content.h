#ifndef TASK_CONTENT_H
#define TASK_CONTENT_H

#include "task_system.h"
#include "localization.h"

/* Internal content tables and text lookup APIs owned by the task content module. */

SCL_EXTERN_C_BEGIN

typedef struct TaskNodeSeed {
    ResourceType type;
    int gridX;
    int gridY;
    int baseYield;
    int respawnsRemaining;
    bool special;
} TaskNodeSeed;

typedef struct TaskMonsterSeed {
    MonsterType type;
    int gridX;
    int gridY;
    int unlockStage;
} TaskMonsterSeed;

typedef struct TaskLogSeed {
    int gridX;
    int gridY;
    ShipLogCategory category;
    LocalizedText title;
    LocalizedText storyText;
    LocalizedText detailText;
} TaskLogSeed;

typedef struct TaskMonsterSpec {
    float maxHealth;
    float attackDamage;
    int rewardCounts[RESOURCE_COUNT];
    bool defeatsBossOnKill;
} TaskMonsterSpec;

int TasksContent_GetNodeSeedCount(void);
bool TasksContent_GetNodeSeed(int index, TaskNodeSeed *seed);

int TasksContent_GetMonsterSeedCount(void);
bool TasksContent_GetMonsterSeed(int index, TaskMonsterSeed *seed);

int TasksContent_GetLogSeedCount(void);
bool TasksContent_GetLogSeed(int index, TaskLogSeed *seed);

bool TasksContent_GetMonsterSpec(MonsterType type, TaskMonsterSpec *spec);
EventType TasksContent_GetDailyEvent(int dayCount);

const char *TasksContent_GetStageObjective(const TaskSystem *tasks, const Player *player);
const char *TasksContent_GetStageGuidance(const TaskSystem *tasks, const Player *player);
const char *TasksContent_GetFieldNote(const TaskSystem *tasks, const Player *player);
const char *TasksContent_GetStageNameText(int stage);
const char *TasksContent_GetPhaseNameText(DayPhase phase);
const char *TasksContent_GetEventNameText(EventType eventType);
const char *TasksContent_GetEndingTitleText(GameEnding ending);
const char *TasksContent_GetEndingBodyText(GameEnding ending);

SCL_EXTERN_C_END

#endif
