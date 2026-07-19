#include "task_interaction_internal.h"
#include "task_survival_internal.h"
#include "task_targeting_internal.h"
#include "task_crafting_internal.h"
#include "task_archive_internal.h"
#include "task_runtime_internal.h"
#include "localization.h"

void TasksRuntime_ReducePoisonAtRecovery(Player *player, float amount) {
    if (player == NULL || amount <= 0.0f) {
        return;
    }

    player->poison -= amount;
    if (player->poison <= 0.0f) {
        Player_ClearPoison(player);
        return;
    }

    if (player->poison > MAX_POISON) {
        player->poison = MAX_POISON;
    }

    Player_SyncPoisonStatus(player);
}

void TasksRuntime_DowngradeOxygenLeakAtRecovery(Player *player) {
    const PlayerStatusEffect *effect;

    if (player == NULL || !Player_HasStatus(player, PLAYER_STATUS_OXYGEN_LEAK)) {
        return;
    }

    effect = Player_GetStatusEffect(player, PLAYER_STATUS_OXYGEN_LEAK);
    if (effect == NULL || effect->level <= 1) {
        Player_ClearStatus(player, PLAYER_STATUS_OXYGEN_LEAK);
        return;
    }

    Player_DowngradeStatus(player,
                           PLAYER_STATUS_OXYGEN_LEAK,
                           effect->level - 1,
                           effect->remainingTime,
                           effect->magnitude * 0.70f);
}

void TasksRuntime_ClearNegativeSurvivalStatuses(Player *player) {
    Player_ClearStatus(player, PLAYER_STATUS_POISONED);
    Player_ClearStatus(player, PLAYER_STATUS_OXYGEN_LEAK);
    Player_ClearStatus(player, PLAYER_STATUS_LOW_OXYGEN);
    Player_ClearStatus(player, PLAYER_STATUS_SUFFOCATING);
    Player_ClearStatus(player, PLAYER_STATUS_CRITICAL_CONDITION);
}

static int GetExpandedRectInteractionScore(const Player *player,
                                           int gridX,
                                           int gridY,
                                           int width,
                                           int height,
                                           int extraRange) {
    if (extraRange <= 0) {
        return TasksRuntime_GetRectInteractionScore(player, gridX, gridY, width, height);
    }

    return TasksRuntime_GetRectInteractionScore(player,
                                                gridX - extraRange,
                                                gridY - extraRange,
                                                width + extraRange * 2,
                                                height + extraRange * 2);
}

static void GetMapAnchorOrFallback(const GameMap *map,
                                     MapAnchor anchor,
                                     int fallbackX,
                                     int fallbackY,
                                     int *gridX,
                                     int *gridY) {
    if (Map_GetAnchorPosition(map, anchor, gridX, gridY)) {
        return;
    }
    if (gridX != NULL) {
        *gridX = fallbackX;
    }
    if (gridY != NULL) {
        *gridY = fallbackY;
    }
}

static void ResolvePropInteractionBounds(const GameMap *map,
                                         TileType tile,
                                         int *gridX,
                                         int *gridY,
                                         int *width,
                                         int *height,
                                         int fallbackWidth,
                                         int fallbackHeight) {
    int originX;
    int originY;
    int resolvedWidth;
    int resolvedHeight;

    *width = fallbackWidth;
    *height = fallbackHeight;
    if (Map_GetMultiTilePropBounds(map,
                                           tile,
                                           *gridX,
                                           *gridY,
                                           &originX,
                                           &originY,
                                           &resolvedWidth,
                                           &resolvedHeight)) {
        *gridX = originX;
        *gridY = originY;
        *width = resolvedWidth;
        *height = resolvedHeight;
    }
}

TaskInteractionTarget TasksRuntime_GetPreferredInteractionTarget(const GameMap *map, const Player *player) {
    struct Candidate {
        TaskInteractionTarget target;
        int score;
    };
    int oxygenConsoleX;
    int oxygenConsoleY;
    int loxiTerminalX;
    int loxiTerminalY;
    int workbenchX;
    int workbenchY;
    int airlockConsoleX;
    int airlockConsoleY;
    int energyConsoleX;
    int energyConsoleY;
    int commRelayX;
    int commRelayY;
    int commRelayWidth;
    int commRelayHeight;
    int crashClueX;
    int crashClueY;
    int crashClueWidth;
    int crashClueHeight;
    int signalTowerX;
    int signalTowerY;
    int signalTowerWidth;
    int signalTowerHeight;
    int monolithAX;
    int monolithAY;
    int monolithAWidth;
    int monolithAHeight;
    int monolithBX;
    int monolithBY;
    int monolithBWidth;
    int monolithBHeight;
    int monolithCX;
    int monolithCY;
    int monolithCWidth;
    int monolithCHeight;
    int energyConsoleWidth;
    int energyConsoleHeight;

    GetMapAnchorOrFallback(map, MAP_ANCHOR_OXYGEN_CONSOLE, OXYGEN_CONSOLE_X, OXYGEN_CONSOLE_Y, &oxygenConsoleX, &oxygenConsoleY);
    GetMapAnchorOrFallback(map, MAP_ANCHOR_LOXI_TERMINAL, LOXI_TERMINAL_X, LOXI_TERMINAL_Y, &loxiTerminalX, &loxiTerminalY);
    GetMapAnchorOrFallback(map, MAP_ANCHOR_WORKBENCH, WORKBENCH_X, WORKBENCH_Y, &workbenchX, &workbenchY);
    GetMapAnchorOrFallback(map, MAP_ANCHOR_AIRLOCK_CONSOLE, AIRLOCK_CONSOLE_X, AIRLOCK_CONSOLE_Y, &airlockConsoleX, &airlockConsoleY);
    GetMapAnchorOrFallback(map, MAP_ANCHOR_ENERGY_CONSOLE, ENERGY_CONSOLE_X, ENERGY_CONSOLE_Y, &energyConsoleX, &energyConsoleY);
    GetMapAnchorOrFallback(map, MAP_ANCHOR_COMM_RELAY, COMM_RELAY_X, COMM_RELAY_Y, &commRelayX, &commRelayY);
    GetMapAnchorOrFallback(map, MAP_ANCHOR_CRASH_CLUE, CRASH_CLUE_X, CRASH_CLUE_Y, &crashClueX, &crashClueY);
    GetMapAnchorOrFallback(map, MAP_ANCHOR_SIGNAL_TOWER, SIGNAL_TOWER_X, SIGNAL_TOWER_Y, &signalTowerX, &signalTowerY);
    GetMapAnchorOrFallback(map, MAP_ANCHOR_MONOLITH_A, MONOLITH_A_X, MONOLITH_A_Y, &monolithAX, &monolithAY);
    GetMapAnchorOrFallback(map, MAP_ANCHOR_MONOLITH_B, MONOLITH_B_X, MONOLITH_B_Y, &monolithBX, &monolithBY);
    GetMapAnchorOrFallback(map, MAP_ANCHOR_MONOLITH_C, MONOLITH_C_X, MONOLITH_C_Y, &monolithCX, &monolithCY);

    ResolvePropInteractionBounds(map, TILE_COMM_RELAY, &commRelayX, &commRelayY, &commRelayWidth, &commRelayHeight, WORLD_INTERACTIVE_FOOTPRINT_SIZE, WORLD_INTERACTIVE_FOOTPRINT_SIZE);
    ResolvePropInteractionBounds(map, TILE_CRASH_CLUE, &crashClueX, &crashClueY, &crashClueWidth, &crashClueHeight, CRASH_CLUE_FOOTPRINT_SIZE, CRASH_CLUE_FOOTPRINT_SIZE);
    ResolvePropInteractionBounds(map, TILE_ENERGY_CONSOLE, &energyConsoleX, &energyConsoleY, &energyConsoleWidth, &energyConsoleHeight, STATION_FOOTPRINT_WIDTH, STATION_FOOTPRINT_HEIGHT);
    ResolvePropInteractionBounds(map, TILE_SIGNAL_TOWER, &signalTowerX, &signalTowerY, &signalTowerWidth, &signalTowerHeight, WORLD_INTERACTIVE_FOOTPRINT_SIZE, WORLD_INTERACTIVE_FOOTPRINT_SIZE);
    ResolvePropInteractionBounds(map, TILE_MONOLITH, &monolithAX, &monolithAY, &monolithAWidth, &monolithAHeight, WORLD_INTERACTIVE_FOOTPRINT_SIZE, WORLD_INTERACTIVE_FOOTPRINT_SIZE);
    ResolvePropInteractionBounds(map, TILE_MONOLITH, &monolithBX, &monolithBY, &monolithBWidth, &monolithBHeight, WORLD_INTERACTIVE_FOOTPRINT_SIZE, WORLD_INTERACTIVE_FOOTPRINT_SIZE);
    ResolvePropInteractionBounds(map, TILE_MONOLITH, &monolithCX, &monolithCY, &monolithCWidth, &monolithCHeight, WORLD_INTERACTIVE_FOOTPRINT_SIZE, WORLD_INTERACTIVE_FOOTPRINT_SIZE);

    const Candidate candidates[] = {
        {TASK_INTERACTION_OXYGEN_CONSOLE, TasksRuntime_GetRectInteractionScore(player, oxygenConsoleX, oxygenConsoleY, STATION_FOOTPRINT_WIDTH, STATION_FOOTPRINT_HEIGHT)},
        {TASK_INTERACTION_LOXI_TERMINAL, GetExpandedRectInteractionScore(player, loxiTerminalX, loxiTerminalY, STATION_FOOTPRINT_WIDTH, STATION_FOOTPRINT_HEIGHT, 1)},
        {TASK_INTERACTION_WORKBENCH, TasksRuntime_GetRectInteractionScore(player, workbenchX, workbenchY, STATION_FOOTPRINT_WIDTH, STATION_FOOTPRINT_HEIGHT)},
        {TASK_INTERACTION_AIRLOCK_CONSOLE,
         Map_GetPropTileAt(map, airlockConsoleX, airlockConsoleY) == TILE_AIRLOCK_CONSOLE
             ? TasksRuntime_GetRectInteractionScore(player, airlockConsoleX, airlockConsoleY, STATION_FOOTPRINT_WIDTH, STATION_FOOTPRINT_HEIGHT)
             : 999},
        {TASK_INTERACTION_COMM_RELAY, TasksRuntime_GetRectInteractionScore(player, commRelayX, commRelayY, commRelayWidth, commRelayHeight)},
        {TASK_INTERACTION_CRASH_CLUE, TasksRuntime_GetRectInteractionScore(player, crashClueX, crashClueY, crashClueWidth, crashClueHeight)},
        {TASK_INTERACTION_ENERGY_CONSOLE, TasksRuntime_GetRectInteractionScore(player, energyConsoleX, energyConsoleY, energyConsoleWidth, energyConsoleHeight)},
        {TASK_INTERACTION_MONOLITH_A, GetExpandedRectInteractionScore(player, monolithAX, monolithAY, monolithAWidth, monolithAHeight, 1)},
        {TASK_INTERACTION_MONOLITH_B, GetExpandedRectInteractionScore(player, monolithBX, monolithBY, monolithBWidth, monolithBHeight, 1)},
        {TASK_INTERACTION_MONOLITH_C, GetExpandedRectInteractionScore(player, monolithCX, monolithCY, monolithCWidth, monolithCHeight, 1)},
        {TASK_INTERACTION_SIGNAL_TOWER, TasksRuntime_GetRectInteractionScore(player, signalTowerX, signalTowerY, signalTowerWidth, signalTowerHeight)}
    };
    TaskInteractionTarget bestTarget;
    int bestScore;
    int index;

    bestTarget = TASK_INTERACTION_NONE;
    bestScore = 999;
    for (index = 0; index < (int)(sizeof(candidates) / sizeof(candidates[0])); index++) {
        if (candidates[index].score < bestScore) {
            bestScore = candidates[index].score;
            bestTarget = candidates[index].target;
        }
    }

    return bestTarget;
}

static bool ShouldPrioritizeLoxiTerminal(const TaskSystem *tasks,
                                         const Player *player,
                                         TaskInteractionTarget target) {
    if (tasks == NULL || player == NULL || target != TASK_INTERACTION_LOXI_TERMINAL) {
        return false;
    }

    return true;
}

bool Tasks_HandleInteraction(TaskSystem *tasks, GameMap *map, Player *player, char *message, size_t messageSize) {
    ResourceNode *node;
    ShipLog *log;
    TaskInteractionTarget target;

    if (tasks == NULL || map == NULL || player == NULL) {
        TasksRuntime_WriteMessage(message, messageSize, Loc_PickLiteral("Interaction unavailable.", "当前无法进行交互。"));
        return false;
    }

    target = TasksRuntime_GetPreferredInteractionTarget(map, player);

    if (ShouldPrioritizeLoxiTerminal(tasks, player, target)
        && TasksRuntime_HandleShipInteraction(tasks, map, player, target, message, messageSize)) {
        return true;
    }

    node = TasksRuntime_FindNearbyNode(tasks, player);
    if (node != NULL) {
        return TasksRuntime_CollectNode(tasks, player, node, message, messageSize);
    }

    log = TasksRuntime_FindNearbyLog(tasks, player);
    if (log != NULL) {
        TasksRuntime_GrantLogReward(tasks, player, log, message, messageSize);
        return true;
    }
    if (TasksRuntime_HandleShipInteraction(tasks, map, player, target, message, messageSize)) {
        return true;
    }

    if (TasksRuntime_HandleWorldInteraction(tasks, map, player, target, message, messageSize)) {
        return true;
    }

    if (message != NULL && messageSize > 0) {
        message[0] = '\0';
    }
    return false;
}
