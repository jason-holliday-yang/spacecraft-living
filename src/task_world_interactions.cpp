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
                                      Loc_PickLiteral("The comm relay is alive again. Loxi can finally read the east route cleanly, and the West Frontier archive now resolves into a real trail instead of ruined noise.",
                                                      "通讯中继重新活过来了。洛希终于能清楚读懂东线路线，而西部前线档案也不再只是残骸里的噪声，终于显出一条真正的线索。"));
            return true;
        }

        TasksRuntime_WriteMessage(message,
                                  messageSize,
                                  Loc_PickLiteral("The comm relay still needs 2 Vines, 2 Shell Fruit, and 1 Special Fungus before its voice comes back.", "通讯中继还需要 2 份藤蔓、2 份壳果和 1 份特殊菌株，才能重新把声音送回来。"));
        return true;
    }

    TasksRuntime_WriteMessage(message, messageSize, Loc_PickLiteral("The comm relay is stable, humming through the fog like it never wanted to go silent.", "通讯中继运行稳定，在雾里低低作响，像它从未甘心真正沉默过。"));
    return true;
}

static bool TryInspectCrashClue(TaskSystem *tasks, GameMap *map, Player *player, char *message, size_t messageSize) {
    if (tasks->stage == 4 && !tasks->crashClueFound && player->hasLaserGun && player->hasProtectionSuit) {
        tasks->crashClueFound = true;
        TasksRuntime_UnlockStageIfNeeded(tasks, map, 5);
        TasksRuntime_WriteMessage(message,
                                  messageSize,
                                  Loc_PickLiteral("You found the real wreck lead. The residue drags farther east, the black-box trail can now be recovered, and an emergency Energy Core is still fused inside the hull. Bring back 1 Junk Metal, 1 Protective Fiber, and 1 Energy Crystal so you can brace the frame, insulate the channel, and wake the housing before extraction.", "你找到了真正的坠毁线索。残留物一路把方向拖向更深的东侧，黑匣子轨迹已经能被回收，而残骸内部还卡着一枚应急能源核心。带回 1 份废旧金属、1 份防护纤维和 1 份能量晶体，才能先撑住结构、做绝缘，再唤醒核心仓，把它安全拆出来。"));
        return true;
    }

    if (tasks->stage == 5 && tasks->crashClueFound) {
        if (player->resources[RESOURCE_ENERGY_CORE] > 0) {
            TasksRuntime_WriteMessage(message,
                                      messageSize,
                                      Loc_PickLiteral("The wreck cavity is empty now. The black box is logged, the emergency core is gone, and the shell has become nothing but evidence.", "残骸核心仓现在已经空了。黑匣子已经归档，应急能源核心也已取走，剩下的外壳只剩下作为证据存在的意义。"));
            return true;
        }

        if (Player_HasResources(player, RESOURCE_JUNK_METAL, 1)
            && Player_HasResources(player, RESOURCE_PROTECTIVE_FIBER, 1)
            && Player_HasResources(player, RESOURCE_ENERGY_CRYSTAL, 1)) {
            Player_SpendResource(player, RESOURCE_JUNK_METAL, 1);
            Player_SpendResource(player, RESOURCE_PROTECTIVE_FIBER, 1);
            Player_SpendResource(player, RESOURCE_ENERGY_CRYSTAL, 1);
            Player_AddResource(player, RESOURCE_ENERGY_CORE, 1);
            TasksRuntime_WriteMessage(message,
                                      messageSize,
                                      Loc_PickLiteral("You braced the wreck with junk metal, wrapped the live channel in protective fiber, and used the crystal pulse to wake the housing just long enough to tear the Energy Core free. Bring it back to Power Bay and finish the repair there.", "你用废旧金属撑住残骸，用防护纤维裹住带电通道，再借助能量晶体的脉冲短暂唤醒核心仓，终于把能源核心硬生生拆了出来。把它带回动力舱，完成后续修复。"));
            return true;
        }

        TasksRuntime_WriteMessage(message,
                                  messageSize,
                                  Loc_PickLiteral("The wreck core is still fused in place. You still need 1 Junk Metal for a brace, 1 Protective Fiber for insulation, and 1 Energy Crystal to wake the housing before extraction is even possible.", "残骸里的核心仍然卡死在原位。你还需要 1 份废旧金属做支架、1 份防护纤维做绝缘，以及 1 份能量晶体来短暂唤醒核心仓，否则根本无从拆取。"));
        return true;
    }

    if (tasks->crashClueFound) {
        TasksRuntime_WriteMessage(message,
                                  messageSize,
                                  Loc_PickLiteral("The wreck is already logged. Follow the eastern trail, gather what the extraction needs, then come back when you are ready to cut the core loose.", "这处残骸已经完成初步记录。继续沿着东线搜集提取所需的东西，准备好之后再回来把核心拆出来。"));
        return true;
    }

    TasksRuntime_WriteMessage(message,
                              messageSize,
                              Loc_PickLiteral("The wreck is still sweating dangerous residue. Bring a weapon and a sealed suit first if you want this clue to become anything more than a warning.", "这处残骸还在不断渗出危险残留物。如果你想让这条线索不只是警告，最好先带上武器和防护服。"));
    return true;
}

static bool TryUseMonolith(TaskSystem *tasks, const Player *player, int monolithIndex, char *message, size_t messageSize) {
    char hint[160];
    int nextIndex;

    if (tasks->stage < 7) {
        TasksRuntime_WriteMessage(message, messageSize, Loc_PickLiteral("The monolith is still dormant, like it is listening but refusing to answer. Loxi suggests finishing the main preparation first.", "石碑仍处于沉寂状态，像是在听，却不肯回应。洛希建议先把主要准备工作做完。"));
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
                      Loc_PickLiteral("The monolith hums to life. Solve the sequence to harden your strikes against the guardian, take some strain off the tower climb, and let the ring explain what binds the two together before the final choice.",
                                      "石碑开始低鸣。解开这段顺序能让你对守卫的攻击更有力，也会替攀塔路线卸掉一部分压力，并在最终抉择前解释清楚守卫与信号塔之间究竟被什么联系着。"),
                      hint);
        return true;
    }

    if (tasks->monolithPuzzle.solved) {
        TasksRuntime_WriteMessage(message,
                                  messageSize,
                                  Loc_PickLiteral("This monolith is already active. The ring's full resonance is still strengthening your attacks and easing the tower climb.", "这座石碑已经激活。整组石碑的共鸣仍在强化你的攻击，并减轻最后攀塔时的压力。"));
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
                                      Loc_PickLiteral("This monolith is already resonating at the front of the sequence. Follow the ring until the next silent stone answers.", "这座石碑已经处在当前序列的起点共鸣中。顺着石碑环继续走，直到下一座沉默石碑回应。"));
        } else {
            TasksRuntime_WriteMessage(message,
                                      messageSize,
                                      Loc_PickLiteral("This monolith is already active. Follow the resonance through the ring to the next silent stone.", "这座石碑已经激活。沿着石碑环中的共鸣去找下一座沉默石碑。"));
        }
        return true;
    }

    nextIndex = GetNextMonolithIndex(tasks);
    if (monolithIndex != nextIndex) {
        TasksRuntime_WriteMessage(message,
                                  messageSize,
                                  Loc_PickLiteral("The ring rejects that order. Another silent stone is supposed to answer before this one.", "石碑环拒绝了这个顺序。应该先由另一座沉默石碑回应，而不是它。"));
        return true;
    }

    ActivateMonolith(tasks, monolithIndex);
    tasks->monolithPuzzle.currentStep = tasks->monolithsLit;
    if (tasks->monolithsLit >= MAX_PUZZLE_STEPS) {
        tasks->monolithPuzzle.solved = true;
        TasksRuntime_WriteMessage(message,
                                  messageSize,
                                  Loc_PickLiteral("All three monoliths resonate in harmony. Your attacks against the guardian now hit harder, the tower route bleeds less oxygen, and the heroic path finally reads like a plan instead of a gamble.", "三座石碑已经完全共鸣。你对守卫的攻击会更重，塔楼路线的漏氧压力也减轻了，而强行救援这条路终于更像一套计划，而不只是一次豪赌。"));
    } else if (tasks->monolithsLit == 1) {
        TasksRuntime_WriteMessage(message,
                                  messageSize,
                                  Loc_PickLiteral("The first monolith locks into resonance. The guardian's hold loosens slightly, the tower climb bleeds a little less oxygen, and the ring begins to explain itself. 2 silent stones remain.", "第一座石碑已锁定共鸣。守卫的压制稍微松了一点，攀塔时的漏氧也略有缓和，而石碑环终于开始解释自己的用途。还剩 2 座沉默石碑。"));
    } else {
        TasksRuntime_WriteMessage(message,
                                  messageSize,
                                  Loc_PickLiteral("The second monolith joins the resonance. The guardian weakens further, the tower climb steadies again, and only 1 silent stone remains before the ring turns the heroic route into a clear execution path.", "第二座石碑加入共鸣。守卫进一步削弱，攀塔路线也再次稳定下来；只要再点亮 1 座沉默石碑，石碑环就能把强行救援彻底变成一条清晰可执行的路线。"));
    }

    Tasks_UpdateObjective(tasks, player);
    return true;
}

static bool TryUseSignalTower(TaskSystem *tasks, const Player *player, char *message, size_t messageSize) {
    GameEnding selectedRoute;

    if (tasks->stage < 7) {
        TasksRuntime_WriteMessage(message, messageSize, Loc_PickLiteral("The Signal Tower is still offline, its silhouette doing nothing but watch. Loxi suggests finishing the endgame preparation first.", "信号塔仍未上线，只剩轮廓立在那里无声俯视。洛希建议先完成终局准备。"));
        return true;
    }

    if (!Tasks_IsEndingBranchReady(tasks)) {
        TasksRuntime_WriteMessage(message,
                                  messageSize,
                                  Loc_PickLiteral("The tower still reads like noise. Recover the remaining mainline logs, finish the route archive work, and return to Loxi before this place turns uncertainty into commitment.", "塔楼的读数仍然像一团噪音。先找回剩余主线日志，完成路线档案工作，再回洛希那里，别让这里在你准备好之前把犹豫变成既定承诺。"));
        return true;
    }

    selectedRoute = Tasks_GetSelectedEndingRoute(tasks);
    if (selectedRoute == ENDING_NONE) {
        TasksRuntime_WriteMessage(message,
                                  messageSize,
                                  Loc_PickLiteral("Do not commit at the tower blind. Go back to Loxi, review the assembled archive, and choose your route aboard the ship before this place chooses the mood for you.", "不要在一知半解的情况下在塔楼这里仓促下决定。先回洛希那里把拼齐的档案再过一遍，在飞船里确认路线，别让这里替你把气氛推成既成事实。"));
        return true;
    }

    if (selectedRoute == ENDING_SETTLEMENT) {
        if (tasks->bossDefeated) {
            TasksRuntime_WriteMessage(message,
                                      messageSize,
                                      Loc_PickLiteral("The settlement route has already been chosen. The tower no longer belongs to that ending, so return to Loxi and confirm your decision to stay.", "你已经选择了定居路线。信号塔已经不属于这个结局的一部分，请回到洛希那里，确认你留下的决定。"));
        } else {
            TasksRuntime_WriteMessage(message,
                                      messageSize,
                                      Loc_PickLiteral("Settlement cannot be finalized here. Clear the guardian in the northwest ruins first, then return to Loxi if staying is still what you mean.", "定居路线不能在这里完成。先去清除西北遗迹里的守卫，如果那之后你仍然决定留下，再回洛希那里确认。"));
        }
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
                                      Loc_PickLiteral("The Signal Amplifier stabilized the tower without another fight. With west and south records finally fitting together, this peaceful rescue lands as a deliberate repair choice that was confirmed back at the ship.",
                                                      "信号放大器在没有再度战斗的情况下稳定了塔楼。当西线与南线记录终于拼到一起后，这次和平救援更像是一种在飞船里确认过的、主动承担的修复选择。"));
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
                                      Loc_PickLiteral("The heroic route is locked in, but the tower is still waiting on the guardian kill in the northwest ruins. The full monolith ring will help once you push that fight, then return here.",
                                                      "强行救援路线已经锁定，但塔楼仍在等待你清掉西北遗迹里的守卫。完整石碑环会在你推进那场战斗时提供帮助，然后再回到这里。"));
        } else if (tasks->monolithsLit > 0) {
            TasksRuntime_WriteMessage(message,
                                      messageSize,
                                      Loc_PickLiteral("The heroic route is locked in. The guardian is still holding the northwest ruins, and your lit monoliths will keep helping once it engages.",
                                                      "强行救援路线已经锁定。守卫仍在把守西北遗迹，而你已点亮的石碑会在战斗开始后继续发挥作用。"));
        } else {
            TasksRuntime_WriteMessage(message,
                                      messageSize,
                                      Loc_PickLiteral("The heroic route is locked in. Hunt the guardian in the northwest ruins before trying to launch the tower.",
                                                      "强行救援路线已经锁定。先去西北遗迹猎杀守卫，再尝试启动塔楼。"));
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
                              Loc_PickLiteral("You rest at the field camp long enough to steady your hands and breathing. It is a safe outdoor fallback, not true safety, but it restores part of your health and oxygen and leaves a short Camp Recovery boost for one more push.", "你在野外营地休息了一阵，直到手不再发抖、呼吸重新稳住。这里是一个安全的户外回撤点，却算不上真正的安全，但它会恢复部分生命与氧气，并留下短暂的营地恢复增益，帮你再推进一次。"));
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
                              Loc_PickLiteral("You cinch the rope tight and force a crossing through the hazard, turning dead ground into a usable shortcut.", "你把绳索狠狠勒紧，硬是在险地上拽出一条通路，把原本的死地变成了可用捷径。"));
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
