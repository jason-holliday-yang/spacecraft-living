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

TaskInteractionTarget TasksRuntime_GetPreferredInteractionTarget(const Player *player) {
    struct Candidate {
        TaskInteractionTarget target;
        int score;
    };
    const Candidate candidates[] = {
        {TASK_INTERACTION_OXYGEN_CONSOLE, TasksRuntime_GetRectInteractionScore(player, OXYGEN_CONSOLE_X, OXYGEN_CONSOLE_Y, STATION_FOOTPRINT_WIDTH, STATION_FOOTPRINT_HEIGHT)},
        {TASK_INTERACTION_LOXI_TERMINAL, GetExpandedRectInteractionScore(player, LOXI_TERMINAL_X, LOXI_TERMINAL_Y, STATION_FOOTPRINT_WIDTH, STATION_FOOTPRINT_HEIGHT, 1)},
        {TASK_INTERACTION_WORKBENCH, TasksRuntime_GetRectInteractionScore(player, WORKBENCH_X, WORKBENCH_Y, STATION_FOOTPRINT_WIDTH, STATION_FOOTPRINT_HEIGHT)},
        {TASK_INTERACTION_AIRLOCK_CONSOLE, TasksRuntime_GetRectInteractionScore(player, AIRLOCK_CONSOLE_X, AIRLOCK_CONSOLE_Y, STATION_FOOTPRINT_WIDTH, STATION_FOOTPRINT_HEIGHT)},
        {TASK_INTERACTION_COMM_RELAY, TasksRuntime_GetRectInteractionScore(player, COMM_RELAY_X, COMM_RELAY_Y, WORLD_INTERACTIVE_FOOTPRINT_SIZE, WORLD_INTERACTIVE_FOOTPRINT_SIZE)},
        {TASK_INTERACTION_CRASH_CLUE, TasksRuntime_GetRectInteractionScore(player, CRASH_CLUE_X, CRASH_CLUE_Y, CRASH_CLUE_FOOTPRINT_SIZE, CRASH_CLUE_FOOTPRINT_SIZE)},
        {TASK_INTERACTION_ENERGY_CONSOLE, TasksRuntime_GetRectInteractionScore(player, ENERGY_CONSOLE_X, ENERGY_CONSOLE_Y, STATION_FOOTPRINT_WIDTH, STATION_FOOTPRINT_HEIGHT)},
        {TASK_INTERACTION_MONOLITH_A, GetExpandedRectInteractionScore(player, MONOLITH_A_X, MONOLITH_A_Y, WORLD_INTERACTIVE_FOOTPRINT_SIZE, WORLD_INTERACTIVE_FOOTPRINT_SIZE, 1)},
        {TASK_INTERACTION_MONOLITH_B, GetExpandedRectInteractionScore(player, MONOLITH_B_X, MONOLITH_B_Y, WORLD_INTERACTIVE_FOOTPRINT_SIZE, WORLD_INTERACTIVE_FOOTPRINT_SIZE, 1)},
        {TASK_INTERACTION_MONOLITH_C, GetExpandedRectInteractionScore(player, MONOLITH_C_X, MONOLITH_C_Y, WORLD_INTERACTIVE_FOOTPRINT_SIZE, WORLD_INTERACTIVE_FOOTPRINT_SIZE, 1)},
        {TASK_INTERACTION_SIGNAL_TOWER, TasksRuntime_GetRectInteractionScore(player, SIGNAL_TOWER_X, SIGNAL_TOWER_Y, WORLD_INTERACTIVE_FOOTPRINT_SIZE, WORLD_INTERACTIVE_FOOTPRINT_SIZE)}
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

    target = TasksRuntime_GetPreferredInteractionTarget(player);

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
