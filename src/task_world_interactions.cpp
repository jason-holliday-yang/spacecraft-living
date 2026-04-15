#include "task_runtime_internal.h"

#include <cstdio>
#include <cstdlib>

static bool IsCrossX2Ready(const TaskSystem *tasks) {
    return tasks != NULL
        && tasks->westW4Completed
        && tasks->southS4Completed;
}

static bool IsCrossX3Ready(const TaskSystem *tasks) {
    return tasks != NULL
        && tasks->westW5Completed
        && tasks->southS5Completed;
}

static int ClampMonolithIndex(int value) {
    if (value < 0) {
        return 0;
    }
    if (value >= MAX_PUZZLE_STEPS) {
        return MAX_PUZZLE_STEPS - 1;
    }
    return value;
}

static int GetNextMonolithIndex(const TaskSystem *tasks) {
    int stepIndex;

    if (tasks == NULL) {
        return 0;
    }

    stepIndex = ClampMonolithIndex(tasks->monolithPuzzle.currentStep);
    return tasks->monolithPuzzle.correctOrder[stepIndex];
}

static void ActivateMonolith(TaskSystem *tasks, int monolithIndex) {
    if (tasks == NULL || monolithIndex < 0 || monolithIndex >= 3) {
        return;
    }

    if (!tasks->monolithActivated[monolithIndex]) {
        tasks->monolithActivated[monolithIndex] = true;
        tasks->monolithsLit += 1;
    }
}

static bool TryUseCommRelay(TaskSystem *tasks, GameMap *map, Player *player, char *message, size_t messageSize) {
    if (tasks->stage == 3) {
        if (Player_HasResources(player, RESOURCE_ALIEN_VINE, 2)
            && Player_HasResources(player, RESOURCE_SHELL_FRUIT, 2)
            && Player_HasResources(player, RESOURCE_SPECIAL_FUNGUS, 1)) {
            Player_SpendResource(player, RESOURCE_ALIEN_VINE, 2);
            Player_SpendResource(player, RESOURCE_SHELL_FRUIT, 2);
            Player_SpendResource(player, RESOURCE_SPECIAL_FUNGUS, 1);
            tasks->commRepairLevel = 1;
            TasksRuntime_UnlockStageIfNeeded(tasks, map, 4);
            TasksRuntime_WriteMessage(message, messageSize, "The comm relay is restored. Loxi can now read the east route properly, and the west frontier archive has become recoverable as a real log trail instead of background debris.");
            return true;
        }

        TasksRuntime_WriteMessage(message, messageSize, "Comm relay repair still needs 2 Vines, 2 Shell Fruit, and 1 Special Fungus.");
        return true;
    }

    TasksRuntime_WriteMessage(message, messageSize, "The comm relay is stable.");
    return true;
}

static bool TryInspectCrashClue(TaskSystem *tasks, GameMap *map, const Player *player, char *message, size_t messageSize) {
    if (tasks->stage == 4 && player->hasLaserGun && player->hasProtectionSuit) {
        tasks->crashClueFound = true;
        TasksRuntime_UnlockStageIfNeeded(tasks, map, 5);
        TasksRuntime_WriteMessage(message, messageSize, "You found the crash clue. The residue points deeper east, the deep swamp entrance is now unlocked for the real qualification run, and the wreck's black-box log can finally be recovered.");
        return true;
    }

    TasksRuntime_WriteMessage(message, messageSize, "The wreck is leaking dangerous residue. Better prepare a weapon and suit first if you want this clue to turn into a real lead.");
    return true;
}

static bool TryUseMonolith(TaskSystem *tasks, const Player *player, int monolithIndex, char *message, size_t messageSize) {
    char hint[160];
    int nextIndex;

    if (tasks->stage < 7) {
        TasksRuntime_WriteMessage(message, messageSize, "The monolith is still dormant. Loxi suggests finishing the main prep first.");
        return true;
    }

    if (!tasks->monolithPuzzle.active) {
        tasks->monolithPuzzle.active = true;
        tasks->monolithPuzzle.correctOrder[0] = 1;
        tasks->monolithPuzzle.correctOrder[1] = 0;
        tasks->monolithPuzzle.correctOrder[2] = 2;
        tasks->monolithPuzzle.currentStep = tasks->monolithsLit;
        Puzzle_GetHint(&tasks->monolithPuzzle, hint, sizeof(hint));
        std::snprintf(message,
                      messageSize,
                      "%s %s",
                      Loc_PickLiteral("The monolith hums to life. Solve the sequence to strengthen your boss damage, soften the tower approach, and let the ring explain how the guardian and Signal Tower are tied together before the final choice.",
                                      "石碑开始低鸣。解开这段顺序能强化你对最终首领的伤害、减轻塔楼路径的压力，并让石碑环在最终抉择前解释守卫与信号塔之间的联系。"),
                      hint);
        return true;
    }

    if (tasks->monolithPuzzle.solved) {
        TasksRuntime_WriteMessage(message, messageSize, "This monolith is already active. The full set is empowering your attacks against the final boss and easing the final tower climb.");
        return true;
    }

    if (tasks->monolithActivated[monolithIndex]) {
        nextIndex = GetNextMonolithIndex(tasks);
        if (tasks->monolithsLit >= 3) {
            TasksRuntime_WriteMessage(message, messageSize, "This monolith is already active. The full set is empowering your attacks against the final boss and easing the final tower climb.");
        } else if (nextIndex == monolithIndex) {
            TasksRuntime_WriteMessage(message, messageSize, "This monolith is already resonating at the front of the sequence. Follow the ring to the next silent stone.");
        } else {
            TasksRuntime_WriteMessage(message, messageSize, "This monolith is already active. Follow the resonance to the next silent stone in the ring.");
        }
        return true;
    }

    nextIndex = GetNextMonolithIndex(tasks);
    if (monolithIndex != nextIndex) {
        TasksRuntime_WriteMessage(message, messageSize, "The ring rejects that order. Another silent stone should answer before this one.");
        return true;
    }

    ActivateMonolith(tasks, monolithIndex);
    tasks->monolithPuzzle.currentStep = tasks->monolithsLit;
    if (tasks->monolithsLit >= MAX_PUZZLE_STEPS) {
        tasks->monolithPuzzle.solved = true;
        TasksRuntime_WriteMessage(message,
                                  messageSize,
                                  Loc_PickLiteral("All three monoliths resonate in harmony! Your attacks against the final boss now deal 30% more damage, the tower route loses part of its oxygen-leak strain, and the ring has made the heroic path fully legible. This is the cleanest heroic timing you have had so far.",
                                                  "三座石碑已经完全共鸣！你对最终首领的攻击现在会额外造成 30% 伤害，塔楼路线的漏氧压力也有所减轻，而石碑环终于让强行救援路线变得清晰可读。这是你目前最理想的一次强攻时机。"));
    } else if (tasks->monolithsLit == 1) {
        TasksRuntime_WriteMessage(message,
                                  messageSize,
                                  Loc_PickLiteral("The first monolith locks into resonance. The guardian's grip weakens slightly, the tower climb leaks a little less oxygen, and the ruins are starting to explain what the ring is really doing. 2 silent stones remain.",
                                                  "第一座石碑已锁定共鸣。守卫的压制略有减弱，攀塔过程中的漏氧也稍微缓和，遗迹开始解释石碑环真正的作用。还剩 2 座沉默石碑。"));
    } else {
        TasksRuntime_WriteMessage(message,
                                  messageSize,
                                  Loc_PickLiteral("The second monolith joins the resonance. The guardian weakens further, the tower climb steadies again, and only 1 silent stone remains before the ring fully turns the heroic route into a clear execution path.",
                                                  "第二座石碑加入了共鸣。守卫进一步削弱，攀塔路线也再次稳定下来；只要再点亮 1 座沉默石碑，石碑环就会把强行救援路线彻底转变为清晰的执行路径。"));
    }

    Tasks_UpdateObjective(tasks, player);
    return true;
}

static bool TryUseSignalTower(TaskSystem *tasks, const Player *player, char *message, size_t messageSize) {
    GameEnding selectedRoute;

    if (tasks->stage < 7) {
        TasksRuntime_WriteMessage(message, messageSize, "The Signal Tower is still offline. Loxi suggests finishing the endgame prep first.");
        return true;
    }

    if (!Tasks_IsEndingBranchReady(tasks)) {
        TasksRuntime_WriteMessage(message, messageSize, "The tower remains unreadable. Recover the remaining mainline logs, finish the route archive work, then return to Loxi before committing to an ending.");
        return true;
    }

    selectedRoute = Tasks_GetSelectedEndingRoute(tasks);
    if (selectedRoute == ENDING_NONE) {
        TasksRuntime_WriteMessage(message, messageSize, "Do not commit at the tower blind. Return to Loxi, review the complete archive, and choose the ending route at the ship first.");
        return true;
    }

    if (selectedRoute == ENDING_SETTLEMENT) {
        TasksRuntime_WriteMessage(message, messageSize, "The settlement route has already been chosen. The tower is no longer part of this ending.");
        return true;
    }

    if (selectedRoute == ENDING_HEROIC && tasks->bossDefeated) {
        tasks->signalTowerActivated = true;
        tasks->ending = ENDING_HEROIC;
        if (IsCrossX3Ready(tasks) && tasks->monolithsLit >= 3) {
            TasksRuntime_WriteMessage(message, messageSize, "The guardian is down, the full monolith ring is stable, and west/south archive evidence confirms what this beacon costs. You followed the route chosen with Loxi, committed to the heroic ending, and sent the rescue beacon.");
        } else if (IsCrossX3Ready(tasks)) {
            TasksRuntime_WriteMessage(message, messageSize, "The guardian is down. West and south archive findings now frame this as a deliberate heroic commitment, and you sent the rescue beacon after confirming the route with Loxi.");
        } else if (tasks->monolithsLit >= 3) {
            TasksRuntime_WriteMessage(message, messageSize, "The guardian is down and the fully lit ring holds the route steady. You followed the chosen heroic route, manually activated the Signal Tower, and sent the rescue beacon.");
        } else {
            TasksRuntime_WriteMessage(message, messageSize, "The guardian is down. You followed the chosen heroic route, manually activated the Signal Tower, and sent the rescue beacon.");
        }
        Tasks_UpdateObjective(tasks, player);
        return true;
    }

    if (selectedRoute == ENDING_PEACEFUL && player->hasSignalAmplifier) {
        tasks->signalTowerActivated = true;
        tasks->ending = ENDING_PEACEFUL;
        if (IsCrossX3Ready(tasks)) {
            TasksRuntime_WriteMessage(message, messageSize, "The Signal Amplifier stabilized the tower without another fight. With west and south records aligned, this peaceful rescue now lands as a full-system repair choice chosen with full context back at the ship.");
        } else if (IsCrossX2Ready(tasks)) {
            TasksRuntime_WriteMessage(message, messageSize, "The Signal Amplifier stabilized the tower without another fight. The peaceful route chosen with Loxi still carries the lowest-risk rescue profile.");
        } else {
            TasksRuntime_WriteMessage(message, messageSize, "The Signal Amplifier stabilized the tower without another fight. You followed the peaceful route chosen with Loxi, and the final plateau stayed calmer than the forced heroic climb.");
        }
        Tasks_UpdateObjective(tasks, player);
        return true;
    }

    if (selectedRoute == ENDING_HEROIC) {
        if (tasks->monolithsLit >= 3) {
            TasksRuntime_WriteMessage(message, messageSize, "The heroic route is locked in, but the tower is waiting on the isolated guardian breach. Open the ship airlock, clear the arena, then return here.");
        } else if (tasks->monolithsLit > 0) {
            TasksRuntime_WriteMessage(message, messageSize, "The heroic route is locked in. The arena breach is ready at the ship airlock, and your lit monoliths will still help once the guardian engages.");
        } else {
            TasksRuntime_WriteMessage(message, messageSize, "The heroic route is locked in. The ship airlock now leads into the guardian arena. Clear that fight before trying to launch the tower.");
        }
    } else if (tasks->monolithsLit >= 3) {
        TasksRuntime_WriteMessage(message, messageSize, "The peaceful route is locked in, but the tower still needs the Signal Amplifier. The fully lit ring is helping keep the climb stable while you prepare it.");
    } else if (tasks->monolithsLit > 0) {
        TasksRuntime_WriteMessage(message, messageSize, "The peaceful route is locked in, but the tower still needs the Signal Amplifier. Lit monoliths are helping while you finish preparations.");
    } else {
        TasksRuntime_WriteMessage(message, messageSize, "The peaceful route is locked in, but the tower still needs the Signal Amplifier before activation.");
    }
    return true;
}

static bool TryUseFieldCamp(const GameMap *map, Player *player, char *message, size_t messageSize) {
    if (!map->campPlaced || std::abs(player->gridX - map->campX) + std::abs(player->gridY - map->campY) > 1) {
        return false;
    }

    player->pressure = INITIAL_PRESSURE;
    Player_RecoverHealth(player, 22.0f);
    Player_AddOxygen(player, 24.0f);
    Player_RecoverStamina(player, 18.0f);
    TasksRuntime_ReducePoisonAtRecovery(player, 20.0f);
    TasksRuntime_DowngradeOxygenLeakAtRecovery(player);
    Player_SetStatus(player, PLAYER_STATUS_CAMP_RECOVERY, 1, 45.0f, SAFE_RECOVERY_AMOUNT);
    TasksRuntime_WriteMessage(message, messageSize, "You rested at the field camp and recovered part of your health and oxygen. It is a safe outdoor fallback, not a full reset, and it leaves a short Camp Recovery boost.");
    return true;
}

static bool TryUseRopeBridge(GameMap *map, const Player *player, char *message, size_t messageSize) {
    int targetX;
    int targetY;

    if (!player->hasRope) {
        return false;
    }

    targetX = player->gridX + player->facingX;
    targetY = player->gridY + player->facingY;
    if (!Map_CanCrossWithRope(map, player->gridX, player->gridY, targetX, targetY)) {
        return false;
    }

    Map_CreateRopeBridge(map, targetX, targetY);
    TasksRuntime_WriteMessage(message, messageSize, "You used the rope to secure a shortcut across the hazardous terrain.");
    return true;
}

bool TasksRuntime_HandleWorldInteraction(TaskSystem *tasks,
                                         GameMap *map,
                                         Player *player,
                                         TaskInteractionTarget target,
                                         char *message,
                                         size_t messageSize) {
    switch (target) {
        case TASK_INTERACTION_COMM_RELAY:
            return TryUseCommRelay(tasks, map, player, message, messageSize);
        case TASK_INTERACTION_CRASH_CLUE:
            return TryInspectCrashClue(tasks, map, player, message, messageSize);
        case TASK_INTERACTION_MONOLITH_A:
            return TryUseMonolith(tasks, player, 0, message, messageSize);
        case TASK_INTERACTION_MONOLITH_B:
            return TryUseMonolith(tasks, player, 1, message, messageSize);
        case TASK_INTERACTION_MONOLITH_C:
            return TryUseMonolith(tasks, player, 2, message, messageSize);
        case TASK_INTERACTION_SIGNAL_TOWER:
            return TryUseSignalTower(tasks, player, message, messageSize);
        case TASK_INTERACTION_NONE:
        case TASK_INTERACTION_OXYGEN_CONSOLE:
        case TASK_INTERACTION_LOXI_TERMINAL:
        case TASK_INTERACTION_WORKBENCH:
        case TASK_INTERACTION_AIRLOCK_CONSOLE:
        case TASK_INTERACTION_ENERGY_CONSOLE:
        default:
            break;
    }

    if (TryUseFieldCamp(map, player, message, messageSize)) {
        return true;
    }

    return TryUseRopeBridge(map, player, message, messageSize);
}
