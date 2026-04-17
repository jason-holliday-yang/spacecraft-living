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
            TasksRuntime_WriteMessage(message,
                                      messageSize,
                                      Loc_PickLiteral("The comm relay is restored. Loxi can now read the east route properly, and the west frontier archive has become recoverable as a real log trail instead of background debris.",
                                                      "通讯中继已经恢复。洛希现在可以正确读取东线路线，而西部前线档案也终于能作为真正的日志线索被回收，而不再只是背景残骸。"));
            return true;
        }

        TasksRuntime_WriteMessage(message,
                                  messageSize,
                                  Loc_PickLiteral("Comm relay repair still needs 2 Vines, 2 Shell Fruit, and 1 Special Fungus.",
                                                  "通讯中继修复仍需要 2 份藤蔓、2 份壳果和 1 份特殊菌株。"));
        return true;
    }

    TasksRuntime_WriteMessage(message, messageSize, Loc_PickLiteral("The comm relay is stable.", "通讯中继运行稳定。"));
    return true;
}

static bool TryInspectCrashClue(TaskSystem *tasks, GameMap *map, const Player *player, char *message, size_t messageSize) {
    if (tasks->crashClueFound) {
        TasksRuntime_WriteMessage(message,
                                  messageSize,
                                  Loc_PickLiteral("The wreck has already been fully explored. The residue trail is logged, and the eastern lead is now part of your route data.",
                                                  "这处残骸已经彻底探索完毕。残留物轨迹已被记录，东线线索也已经纳入路线数据。"));
        return true;
    }

    if (tasks->stage == 4 && player->hasLaserGun && player->hasProtectionSuit) {
        tasks->crashClueFound = true;
        TasksRuntime_UnlockStageIfNeeded(tasks, map, 5);
        TasksRuntime_WriteMessage(message,
                                  messageSize,
                                  Loc_PickLiteral("You found the crash clue. The residue points deeper east, the deep swamp entrance is now unlocked for the real qualification run, and the wreck's black-box log can finally be recovered.",
                                                  "你找到了坠毁线索。残留物指向更深的东侧区域，深层沼泽入口现已开放，而残骸中的黑匣子日志也终于可以回收。"));
        return true;
    }

    TasksRuntime_WriteMessage(message,
                              messageSize,
                              Loc_PickLiteral("The wreck is leaking dangerous residue. Better prepare a weapon and suit first if you want this clue to turn into a real lead.",
                                              "这处残骸仍在泄露危险残留物。如果你想把这条线索真正变成突破口，最好先准备好武器和防护服。"));
    return true;
}

static bool TryUseMonolith(TaskSystem *tasks, const Player *player, int monolithIndex, char *message, size_t messageSize) {
    char hint[160];
    int nextIndex;

    if (tasks->stage < 7) {
        TasksRuntime_WriteMessage(message, messageSize, Loc_PickLiteral("The monolith is still dormant. Loxi suggests finishing the main prep first.", "石碑仍处于沉寂状态。洛希建议先完成主要准备工作。"));
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
        TasksRuntime_WriteMessage(message,
                                  messageSize,
                                  Loc_PickLiteral("This monolith is already active. The full set is empowering your attacks against the final boss and easing the final tower climb.",
                                                  "这座石碑已经激活。整组石碑的共鸣正在强化你对最终首领的攻击，并减轻最后登塔的压力。"));
        return true;
    }

    if (tasks->monolithActivated[monolithIndex]) {
        nextIndex = GetNextMonolithIndex(tasks);
        if (tasks->monolithsLit >= 3) {
            TasksRuntime_WriteMessage(message,
                                      messageSize,
                                      Loc_PickLiteral("This monolith is already active. The full set is empowering your attacks against the final boss and easing the final tower climb.",
                                                      "这座石碑已经激活。整组石碑的共鸣正在强化你对最终首领的攻击，并减轻最后登塔的压力。"));
        } else if (nextIndex == monolithIndex) {
            TasksRuntime_WriteMessage(message,
                                      messageSize,
                                      Loc_PickLiteral("This monolith is already resonating at the front of the sequence. Follow the ring to the next silent stone.",
                                                      "这座石碑已经处在当前序列的起点共鸣中。沿着石碑环去寻找下一座沉默石碑吧。"));
        } else {
            TasksRuntime_WriteMessage(message,
                                      messageSize,
                                      Loc_PickLiteral("This monolith is already active. Follow the resonance to the next silent stone in the ring.",
                                                      "这座石碑已经激活。顺着共鸣去寻找石碑环中下一座沉默石碑。"));
        }
        return true;
    }

    nextIndex = GetNextMonolithIndex(tasks);
    if (monolithIndex != nextIndex) {
        TasksRuntime_WriteMessage(message,
                                  messageSize,
                                  Loc_PickLiteral("The ring rejects that order. Another silent stone should answer before this one.",
                                                  "石碑环拒绝了这个顺序。应该先由另一座沉默石碑回应。"));
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
        TasksRuntime_WriteMessage(message, messageSize, Loc_PickLiteral("The Signal Tower is still offline. Loxi suggests finishing the endgame prep first.", "信号塔仍未上线。洛希建议先完成终局准备。"));
        return true;
    }

    if (!Tasks_IsEndingBranchReady(tasks)) {
        TasksRuntime_WriteMessage(message,
                                  messageSize,
                                  Loc_PickLiteral("The tower still reads like noise. Recover the remaining mainline logs, finish the route archive work, then return to Loxi before you turn any ending into a commitment.",
                                                  "塔楼的读数仍然像一团噪音。请先找回剩余主线日志，完成路线档案工作，再回到洛希那里，别让任何结局在你准备好之前变成既定承诺。"));
        return true;
    }

    selectedRoute = Tasks_GetSelectedEndingRoute(tasks);
    if (selectedRoute == ENDING_NONE) {
        TasksRuntime_WriteMessage(message,
                                  messageSize,
                                  Loc_PickLiteral("Do not commit at the tower blind. Return to Loxi, review the complete archive, and choose your ending route at the ship before this place makes the choice for you.",
                                                  "不要在毫无准备的情况下于塔楼仓促作出承诺。先回到洛希那里复核完整档案，在飞船内确认你的结局路线，别让这里替你做决定。"));
        return true;
    }

    if (selectedRoute == ENDING_SETTLEMENT) {
        TasksRuntime_WriteMessage(message,
                                  messageSize,
                                  Loc_PickLiteral("The settlement route has already been chosen. The tower is no longer part of this ending.",
                                                  "你已经选择了定居路线，信号塔不再属于这个结局的一部分。"));
        return true;
    }

    if (selectedRoute == ENDING_HEROIC && tasks->bossDefeated) {
        tasks->signalTowerActivated = true;
        tasks->ending = ENDING_HEROIC;
        if (IsCrossX3Ready(tasks) && tasks->monolithsLit >= 3) {
            TasksRuntime_WriteMessage(message,
                                      messageSize,
                                      Loc_PickLiteral("The guardian is down, the full monolith ring is stable, and west/south archive evidence confirms what this beacon costs. You followed the route chosen with Loxi, committed to the heroic ending, and sent the rescue beacon anyway.",
                                                      "守卫已经倒下，完整石碑环也保持稳定，而西线与南线档案共同证明了这道信标的代价。你依照与洛希确认过的路线，依然选择了承担这一切并发出求救信标。"));
        } else if (IsCrossX3Ready(tasks)) {
            TasksRuntime_WriteMessage(message,
                                      messageSize,
                                      Loc_PickLiteral("The guardian is down. West and south archive findings frame this as a deliberate heroic commitment, and you sent the rescue beacon after confirming the route with Loxi.",
                                                      "守卫已经倒下。西线与南线档案让这次行动成为一次明确而自觉的强行救援，而你也在与洛希确认路线后发出了求救信标。"));
        } else if (tasks->monolithsLit >= 3) {
            TasksRuntime_WriteMessage(message,
                                      messageSize,
                                      Loc_PickLiteral("The guardian is down and the fully lit ring holds the route steady. You followed the chosen heroic route, manually activated the Signal Tower, and sent the rescue beacon into a sky that had to be forced open.",
                                                      "守卫已经倒下，而完整点亮的石碑环也稳住了这条路线。你遵循已选定的强行救援路线，手动启动了信号塔，并把求救信标送入了那片必须被强行撕开的天空。"));
        } else {
            TasksRuntime_WriteMessage(message,
                                      messageSize,
                                      Loc_PickLiteral("The guardian is down. You followed the chosen heroic route, manually activated the Signal Tower, and sent the rescue beacon.",
                                                      "守卫已经倒下。你遵循已选定的强行救援路线，手动启动了信号塔，并发出了求救信标。"));
        }
        Tasks_UpdateObjective(tasks, player);
        return true;
    }

    if (selectedRoute == ENDING_PEACEFUL && player->hasSignalAmplifier) {
        tasks->signalTowerActivated = true;
        tasks->ending = ENDING_PEACEFUL;
        if (IsCrossX3Ready(tasks)) {
            TasksRuntime_WriteMessage(message,
                                      messageSize,
                                      Loc_PickLiteral("The Signal Amplifier stabilized the tower without another fight. With west and south records aligned, this peaceful rescue lands as a full-system repair choice chosen with full context back at the ship.",
                                                      "信号放大器在没有再度战斗的情况下稳定了塔楼。当西线与南线记录拼合之后，这次和平救援更像是在飞船内作出的、基于完整背景的整套系统修复选择。"));
        } else if (IsCrossX2Ready(tasks)) {
            TasksRuntime_WriteMessage(message,
                                      messageSize,
                                      Loc_PickLiteral("The Signal Amplifier stabilized the tower without another fight. The peaceful route chosen with Loxi still carries the lowest-risk rescue profile, and the plateau finally quieted instead of tearing wider.",
                                                      "信号放大器在没有再度战斗的情况下稳定了塔楼。与洛希共同确认的和平路线依旧是风险最低的救援方案，而高台终于安静下来，没有继续撕裂。"));
        } else {
            TasksRuntime_WriteMessage(message,
                                      messageSize,
                                      Loc_PickLiteral("The Signal Amplifier stabilized the tower without another fight. You followed the peaceful route chosen with Loxi, and the final plateau stayed calmer than the forced heroic climb ever could have.",
                                                      "信号放大器在没有再度战斗的情况下稳定了塔楼。你遵循与洛希确认的和平路线，而最终高台也保持着比强攻路线安稳得多的状态。"));
        }
        Tasks_UpdateObjective(tasks, player);
        return true;
    }

    if (selectedRoute == ENDING_HEROIC) {
        if (tasks->monolithsLit >= 3) {
            TasksRuntime_WriteMessage(message,
                                      messageSize,
                                      Loc_PickLiteral("The heroic route is locked in, but the tower is waiting on the isolated guardian breach. Open the ship airlock, clear the arena, then return here.",
                                                      "强行救援路线已经锁定，但塔楼仍在等待那场隔离守卫战。打开飞船气闸，清空战场，然后再回到这里。"));
        } else if (tasks->monolithsLit > 0) {
            TasksRuntime_WriteMessage(message,
                                      messageSize,
                                      Loc_PickLiteral("The heroic route is locked in. The arena breach is ready at the ship airlock, and your lit monoliths will still help once the guardian engages.",
                                                      "强行救援路线已经锁定。守卫战场入口已在飞船气闸处待命，而你已点亮的石碑仍会在战斗开始后继续发挥作用。"));
        } else {
            TasksRuntime_WriteMessage(message,
                                      messageSize,
                                      Loc_PickLiteral("The heroic route is locked in. The ship airlock now leads into the guardian arena. Clear that fight before trying to launch the tower.",
                                                      "强行救援路线已经锁定。飞船气闸现在会直接通向守卫战场。先赢下那场战斗，再尝试启动塔楼。"));
        }
    } else if (player->resources[RESOURCE_RELIC_FRAGMENT] >= 3) {
        if (tasks->monolithsLit >= 3) {
            TasksRuntime_WriteMessage(message,
                                      messageSize,
                                      Loc_PickLiteral("The peaceful route is locked in, but the tower still needs the Signal Amplifier. Return to the workshop, build it from the mapped fragment set, then come back here.",
                                                      "和平路线已经锁定，但塔楼仍需要信号放大器。回到工坊，用已映射的碎片组装它，然后再回到这里。"));
        } else if (tasks->monolithsLit > 0) {
            TasksRuntime_WriteMessage(message,
                                      messageSize,
                                      Loc_PickLiteral("The peaceful route is locked in, but the tower still needs the Signal Amplifier. Return to the workshop, build it from the fragment set, then come back here.",
                                                      "和平路线已经锁定，但塔楼仍需要信号放大器。回到工坊，用碎片组装它，然后再回到这里。"));
        } else {
            TasksRuntime_WriteMessage(message,
                                      messageSize,
                                      Loc_PickLiteral("The peaceful route is locked in, but the tower still needs the Signal Amplifier. Build it at the workshop from the mapped fragment set before activation.",
                                                      "和平路线已经锁定，但塔楼仍需要信号放大器。在启动前，先去工坊用已映射的碎片把它制作出来。"));
        }
    } else if (tasks->monolithsLit >= 3) {
        TasksRuntime_WriteMessage(message,
                                  messageSize,
                                  Loc_PickLiteral("The peaceful route is locked in, but the tower still needs the Signal Amplifier. Recover 3 Relic Fragments, build it at the workshop, and use the fully lit ring to keep the climb stable meanwhile.",
                                                  "和平路线已经锁定，但塔楼仍需要信号放大器。先找回 3 枚遗迹碎片，在工坊制作它，同时利用完整点亮的石碑环维持攀登稳定。"));
    } else if (tasks->monolithsLit > 0) {
        TasksRuntime_WriteMessage(message,
                                  messageSize,
                                  Loc_PickLiteral("The peaceful route is locked in, but the tower still needs the Signal Amplifier. Recover 3 Relic Fragments and build it at the workshop while the lit monoliths help steady the route.",
                                                  "和平路线已经锁定，但塔楼仍需要信号放大器。先找回 3 枚遗迹碎片并在工坊制作它，已点亮的石碑会在此期间帮助稳定路线。"));
    } else {
        TasksRuntime_WriteMessage(message,
                                  messageSize,
                                  Loc_PickLiteral("The peaceful route is locked in, but the tower still needs the Signal Amplifier. Recover 3 Relic Fragments and build it at the workshop before activation.",
                                                  "和平路线已经锁定，但塔楼仍需要信号放大器。先找回 3 枚遗迹碎片，并在启动前去工坊把它制作出来。"));
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
    TasksRuntime_WriteMessage(message,
                              messageSize,
                              Loc_PickLiteral("You rested at the field camp and recovered part of your health and oxygen. It is a safe outdoor fallback, not a full reset, and it leaves a short Camp Recovery boost.",
                                              "你在野外营地休息了一次，恢复了部分生命与氧气。这里是安全的户外回撤点，但并非完全重置，同时还会留下短暂的营地恢复增益。"));
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
    TasksRuntime_WriteMessage(message,
                              messageSize,
                              Loc_PickLiteral("You used the rope to secure a shortcut across the hazardous terrain.",
                                              "你用绳索固定出了一条穿越危险地形的捷径。"));
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
