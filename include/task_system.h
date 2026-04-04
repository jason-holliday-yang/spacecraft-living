#ifndef TASK_SYSTEM_H
#define TASK_SYSTEM_H

#include <stdbool.h>
#include <stddef.h>
#include "assets.h"
#include "player.h"
#include "puzzle.h"

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
    bool special;
    MapArea area;
    float awayTimer;
} ResourceNode;

typedef struct Monster {
    bool active;
    MonsterType type;
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
    bool phaseTriggered;
} Monster;

typedef struct ShipLog {
    bool active;
    bool collected;
    int gridX;
    int gridY;
    int rewardKind;
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
    bool communicatorUnlocked;
    bool bossDefeated;
    bool signalTowerActivated;
    bool monolithActivated[3];
    int monolithsLit;
    MonolithPuzzle monolithPuzzle;
    GameEnding ending;
    ResourceNode nodes[MAX_RESOURCE_NODES];
    int nodeCount;
    Monster monsters[MAX_MONSTERS];
    int monsterCount;
    ShipLog logs[MAX_LOGS];
    int logCount;
    char objective[200];
    char communicator[320];
} TaskSystem;

void Tasks_Init(TaskSystem *tasks, GameMap *map);
void Tasks_UpdateObjective(TaskSystem *tasks, const Player *player);
void Tasks_Update(TaskSystem *tasks, GameMap *map, Player *player, float deltaTime);
bool Tasks_HandleInteraction(TaskSystem *tasks, GameMap *map, Player *player, char *message, size_t messageSize);
bool Tasks_HandleAttack(TaskSystem *tasks, GameMap *map, Player *player, char *message, size_t messageSize);
bool Tasks_TryCraft(TaskSystem *tasks, GameMap *map, Player *player, RecipeType recipe, char *message, size_t messageSize);
bool Tasks_IsBlockingActorTile(const TaskSystem *tasks, int gridX, int gridY);
bool Tasks_GetObjectiveMarker(const TaskSystem *tasks, const Player *player, int *gridX, int *gridY);
void Tasks_DrawWorld(const TaskSystem *tasks, const AssetBundle *assets, float elapsedSeconds);
int Tasks_GetVisibleRecipeCount(const TaskSystem *tasks);
RecipeType Tasks_GetVisibleRecipeAt(const TaskSystem *tasks, int index);
const char *Tasks_GetStageName(int stage);
const char *Tasks_GetPhaseName(DayPhase phase);
const char *Tasks_GetEventName(EventType eventType);
bool Tasks_IsCommunicatorUnlocked(const TaskSystem *tasks);
const char *Tasks_GetCommunicatorHint(const TaskSystem *tasks);
GameEnding Tasks_GetEnding(const TaskSystem *tasks);
const char *Tasks_GetEndingTitle(GameEnding ending);
const char *Tasks_GetEndingBody(GameEnding ending);

#endif
