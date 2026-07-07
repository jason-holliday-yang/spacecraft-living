#ifndef TASK_SYSTEM_H
#define TASK_SYSTEM_H

#include <stdbool.h>
#include <stddef.h>
#include "c_compat.h"
#include "assets.h"
#include "localization.h"
#include "player.h"
#include "puzzle.h"

/* Public task/runtime state, progression enums, and gameplay-side task APIs. */

SCL_EXTERN_C_BEGIN

typedef enum DayPhase {
    DAY_PHASE_DAY = 0,
    DAY_PHASE_DUSK,
    DAY_PHASE_NIGHT
} DayPhase;

typedef enum EventType {
    EVENT_HARVEST = 0,
    EVENT_CALM_BEASTS,
    EVENT_CLEAR_SKY,
    EVENT_SPORE_STORM,
    EVENT_MONSTER_FRENZY,
    EVENT_DEVICE_FAULT,
    EVENT_COUNT
} EventType;

typedef enum MonsterType {
    MONSTER_THORN_LARVA = 0,
    MONSTER_WING_BUG,
    MONSTER_RAPTOR,
    MONSTER_SWAMP_STALKER,
    MONSTER_SENTINEL_JELLY,
    MONSTER_FOG_WORM,
    MONSTER_RELIC_GUARD,
    MONSTER_FINAL_BOSS
} MonsterType;

typedef enum CombatEncounterId {
    COMBAT_ENCOUNTER_NONE = 0,
    COMBAT_ENCOUNTER_WEST_FRONTIER,
    COMBAT_ENCOUNTER_CANOPY_HOLLOW,
    COMBAT_ENCOUNTER_ECHO_BASIN,
    COMBAT_ENCOUNTER_DEEP_BASIN,
    COMBAT_ENCOUNTER_SOUTH_COLLAPSE,
    COMBAT_ENCOUNTER_ROOT_VAULT,
    COMBAT_ENCOUNTER_RELIC_GUARD,
    COMBAT_ENCOUNTER_FINAL_BOSS,
    COMBAT_ENCOUNTER_COUNT
} CombatEncounterId;

typedef enum GameEnding {
    ENDING_NONE = 0,
    ENDING_HEROIC,
    ENDING_PEACEFUL,
    ENDING_SETTLEMENT,
    ENDING_FAILURE
} GameEnding;

typedef struct ResourceNode {
    bool active;
    ResourceType type;
    int gridX;
    int gridY;
    int baseYield;
    int respawnsRemaining;
    int initialRespawnsRemaining;
    bool special;
    MapArea area;
    float awayTimer;
} ResourceNode;

typedef enum BossAttackType {
    BOSS_ATTACK_NONE = 0,
    BOSS_ATTACK_MELEE,
    BOSS_ATTACK_CHARGE,
    BOSS_ATTACK_SPAWN,
    BOSS_ATTACK_AOE
} BossAttackType;

typedef struct Monster {
    bool active;
    MonsterType type;
    CombatEncounterId encounterId;
    int gridX;
    int gridY;
    int spawnX;
    int spawnY;
    int unlockStage;
    MapArea area;
    float health;
    float maxHealth;
    float moveTimer;
    float attackTimer;
    float recoverTimer;
    float disengageTimer;
    bool phaseTriggered;
    BossAttackType currentAttack;
    float attackTelegraph;
    float weakPointTimer;
    int targetX;
    int targetY;
} Monster;

typedef enum ShipLogCategory {
    SHIP_LOG_MAINLINE = 0,
    SHIP_LOG_SUPPLEMENTAL
} ShipLogCategory;

typedef struct ShipLog {
    bool active;
    bool collected;
    int gridX;
    int gridY;
    ShipLogCategory category;
    char titleEn[64];
    char titleZh[64];
    char storyTextEn[512];
    char storyTextZh[512];
    char detailTextEn[1024];
    char detailTextZh[1024];
} ShipLog;

typedef struct TaskSystem {
    int stage;
    int dayCount;
    DayPhase phase;
    EventType currentEvent;
    float cycleTimer;
    float elapsedSeconds;
    int oxygenRepairLevel;
    int commRepairLevel;
    int energyRepairLevel;
    bool crashClueFound;
    bool amplifierUnlocked;
    bool signalAmplifierCrafted;
    bool communicatorUnlocked;
    bool endingArchiveReviewed;
    int shownMainStorySceneCount;
    bool bossDefeated;
    bool signalTowerActivated;
    bool westW1Started;
    bool westW1Completed;
    bool westW2Started;
    bool westW2Completed;
    bool westW3Started;
    bool westW3Completed;
    bool westW4Started;
    bool westW4Completed;
    bool westW5Started;
    bool westW5Completed;
    bool southS1Started;
    bool southS1Completed;
    bool southS2Started;
    bool southS2Completed;
    bool southS3Started;
    bool southS3Completed;
    bool southS4Started;
    bool southS4Completed;
    bool southS5Started;
    bool southS5Completed;
    bool monolithActivated[3];
    int monolithsLit;
    MonolithPuzzle monolithPuzzle;
    GameEnding ending;
    GameEnding selectedEndingRoute;
    ResourceNode nodes[MAX_RESOURCE_NODES];
    int nodeCount;
    Monster monsters[MAX_MONSTERS];
    int monsterCount;
    ShipLog logs[MAX_LOGS];
    int logCount;
    char objective[200];
    char communicator[2048];
} TaskSystem;

void Tasks_Init(TaskSystem *tasks, GameMap *map);
void Tasks_UpdateObjective(TaskSystem *tasks, const Player *player);
void Tasks_Update(TaskSystem *tasks, GameMap *map, Player *player, float deltaTime);
bool Tasks_HandleInteraction(TaskSystem *tasks, GameMap *map, Player *player, char *message, size_t messageSize);
bool Tasks_HandleAttack(TaskSystem *tasks, GameMap *map, Player *player, char *message, size_t messageSize);
bool Tasks_TryCraft(TaskSystem *tasks, GameMap *map, Player *player, RecipeType recipe, char *message, size_t messageSize);
bool Tasks_IsNearWorkbench(const Player *player);
bool Tasks_IsBlockingActorTile(const TaskSystem *tasks, int gridX, int gridY);
bool Tasks_GetObjectiveMarker(const TaskSystem *tasks, const Player *player, int *gridX, int *gridY);
void Tasks_DrawWorld(const TaskSystem *tasks, const AssetBundle *assets, float elapsedSeconds);
int Tasks_GetCollectedLogCount(const TaskSystem *tasks);
const ShipLog *Tasks_GetCollectedLogAt(const TaskSystem *tasks, int index);
int Tasks_GetLogSceneIndex(const TaskSystem *tasks, const ShipLog *log);
const char *Tasks_GetLogTitle(const ShipLog *log);
const char *Tasks_GetLogStoryText(const ShipLog *log);
const char *Tasks_GetLogDetailText(const ShipLog *log);
bool Tasks_IsRecipeVisible(const TaskSystem *tasks, RecipeType recipe);
bool Tasks_CanCraftRecipe(const TaskSystem *tasks, const Player *player, RecipeType recipe);
int Tasks_GetVisibleRecipeCount(const TaskSystem *tasks);
RecipeType Tasks_GetVisibleRecipeAt(const TaskSystem *tasks, int index);
const char *Tasks_GetStageName(int stage);
const char *Tasks_GetPhaseName(DayPhase phase);
const char *Tasks_GetEventName(EventType eventType);
bool Tasks_IsCommunicatorUnlocked(const TaskSystem *tasks);
const char *Tasks_GetCommunicatorHint(const TaskSystem *tasks);
bool Tasks_IsEndingPreCheckReady(const TaskSystem *tasks);
bool Tasks_IsEndingBranchReady(const TaskSystem *tasks);
GameEnding Tasks_GetSelectedEndingRoute(const TaskSystem *tasks);
bool Tasks_SelectEndingRoute(TaskSystem *tasks, GameEnding ending);
bool Tasks_IsEndingAvailable(const TaskSystem *tasks, GameEnding ending);
int Tasks_GetAvailableEndingCount(const TaskSystem *tasks);
GameEnding Tasks_GetAvailableEndingAt(const TaskSystem *tasks, int index);
bool Tasks_CanChooseSettlement(const TaskSystem *tasks);
void Tasks_CommitSettlement(TaskSystem *tasks);
GameEnding Tasks_GetEnding(const TaskSystem *tasks);
const char *Tasks_GetEndingTitle(GameEnding ending);
const char *Tasks_GetEndingBody(GameEnding ending);
int Tasks_CalculateEndingScore(const TaskSystem *tasks, const Player *player);
const char *Tasks_GetEndingScoreRank(int score);
int Tasks_GetArchiveScore(const TaskSystem *tasks);
int Tasks_GetInvestigationScore(const TaskSystem *tasks);
int Tasks_GetSurvivalScore(const TaskSystem *tasks, const Player *player);
int Tasks_GetEndingCompletionScore(const TaskSystem *tasks, const Player *player);
int Tasks_GetCombatScore(const TaskSystem *tasks);
int Tasks_GetCombatScoreMax(void);
int Tasks_GetCombatEncounterCount(void);
bool Tasks_IsCombatEncounterCompleted(const TaskSystem *tasks, CombatEncounterId encounter);
int Tasks_GetCombatEncounterScore(CombatEncounterId encounter);
const char *Tasks_GetCombatEncounterName(CombatEncounterId encounter);

SCL_EXTERN_C_END

#endif
