#include "task_runtime_internal.h"

#include "localization.h"

#include <cstdio>
#include <cstring>

static bool IsCrossX1Ready(const TaskSystem *tasks) {
    return tasks != NULL
        && tasks->westW3Completed
        && tasks->southS2Completed;
}

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

static bool HasEnergyCoreExtractionMaterials(const Player *player) {
    return player != NULL
        && Player_HasResources(player, RESOURCE_JUNK_METAL, 1)
        && Player_HasResources(player, RESOURCE_PROTECTIVE_FIBER, 1)
        && Player_HasResources(player, RESOURCE_ENERGY_CRYSTAL, 1);
}

enum {
    LOG_INDEX_SHIP_IMPACT_PROTOCOL = 0,
    LOG_INDEX_SHIP_SPLIT_ROSTER = 1,
    LOG_INDEX_SHIP_PATTERN_NOT_WILDERNESS = 2,
    LOG_INDEX_VENT_CALIBRATION_HANDOVER = 10,
    LOG_INDEX_PURIFIER_RING_CONTROL_BRIEF = 12
};

static bool IsCollectedLogIndex(const TaskSystem *tasks, int logIndex) {
    return tasks != NULL
        && logIndex >= 0
        && logIndex < tasks->logCount
        && tasks->logs[logIndex].collected;
}

static int CountCollectedShipIntroLogs(const TaskSystem *tasks) {
    int count = 0;

    count += IsCollectedLogIndex(tasks, LOG_INDEX_SHIP_IMPACT_PROTOCOL) ? 1 : 0;
    count += IsCollectedLogIndex(tasks, LOG_INDEX_SHIP_SPLIT_ROSTER) ? 1 : 0;
    count += IsCollectedLogIndex(tasks, LOG_INDEX_SHIP_PATTERN_NOT_WILDERNESS) ? 1 : 0;
    return count;
}

static bool IsPeacefulEndingEvidenceReady(const TaskSystem *tasks) {
    return IsCollectedLogIndex(tasks, LOG_INDEX_VENT_CALIBRATION_HANDOVER)
        || IsCollectedLogIndex(tasks, LOG_INDEX_PURIFIER_RING_CONTROL_BRIEF);
}

static const char *FormatSingleEndingTerminalMessage(const TaskSystem *tasks,
                                                     const char *englishFormat,
                                                     const char *simplifiedChineseFormat) {
    static char message[512];
    const char *endingTitle;

    endingTitle = Tasks_GetEndingTitle(Tasks_GetAvailableEndingAt(tasks, 0));
    std::snprintf(message,
                  sizeof(message),
                  Loc_PickLiteral(englishFormat, simplifiedChineseFormat),
                  endingTitle);
    return message;
}

static const char *GetStage7RouteChoiceMessage(const TaskSystem *tasks) {
    const int availableEndingCount = Tasks_GetAvailableEndingCount(tasks);
    const bool settlementEvidenceReady = tasks != NULL
        && tasks->logCount > 7
        && tasks->logs[7].collected;
    const bool peacefulEvidenceReady = IsPeacefulEndingEvidenceReady(tasks);
    const bool bossDefeated = tasks != NULL && tasks->bossDefeated;
    const bool amplifierCrafted = tasks != NULL && tasks->signalAmplifierCrafted;

    if (availableEndingCount <= 0) {
        if (peacefulEvidenceReady && !amplifierCrafted) {
            return Loc_PickLiteral("Loxi online. The south-line control records already support peaceful rescue, but you still have not built the Signal Amplifier. Craft it at the workshop first, then come back here if you want that ending to become selectable.",
                                   "洛希已上线。南线控制记录已经足以支撑和平救援，但你还没有制作信号放大器。先去工坊把它做出来，再回来，这个结局才会真正变成可选项。");
        }
        if (settlementEvidenceReady && !bossDefeated) {
            return Loc_PickLiteral("Loxi online. The Last Camp testament already supports settlement, but the guardian still controls the northwest ruins. Defeat it first, then return here if you want staying to become a real option.",
                                   "洛希已上线。最后营地遗言已经支撑起定居路线，但守卫仍在控制西北侧遗迹。先击败它，再回来，留下来才会真正成为一个可选答案。");
        }
        return Loc_PickLiteral("Loxi online. The main archive is assembled, but none of the ending lines is fully locked yet. Recover the Canopy Handoff record for heroic rescue, a south-line control record for peaceful rescue, or the Last Camp testament plus the guardian kill for settlement.",
                               "洛希已上线。主线档案虽然已经拼齐，但三条终局线里还没有哪一条被彻底锁定。想开启强行救援，请补回林冠交接记录；想开启和平救援，请补回南线控制记录；想开启异星定居，请补回最后营地遗言并击败守卫。");
    }
    if (availableEndingCount == 1) {
        switch (Tasks_GetAvailableEndingAt(tasks, 0)) {
            case ENDING_HEROIC:
                return Loc_PickLiteral("Loxi online. The recovered archive now supports only heroic rescue: accept the cost, defeat the guardian in the northwest ruins, then force the beacon alive and leave.",
                                       "洛希已上线。当前被回收档案真正支撑起来的，只有强行救援：接受代价，先在西北遗迹击败守卫，再强行点亮信标并离开。");
            case ENDING_PEACEFUL:
                return Loc_PickLiteral("Loxi online. The recovered archive now supports only peaceful rescue: steady the tower system and leave without forcing it open any further. Confirm the route here, then carry the amplifier to the tower.",
                                       "洛希已上线。当前被回收档案真正支撑起来的，只有和平救援：先稳住塔楼系统，再在不继续强行撕开它的前提下离开。在这里确认路线后，把放大器带去塔楼。");
            case ENDING_SETTLEMENT:
                return Loc_PickLiteral("Loxi online. The recovered archive now supports only settlement: the guardian is gone, and staying has become a real long-term choice. Confirm the route here, then return once more when you are ready to end the run as a settler.",
                                       "洛希已上线。当前被回收档案真正支撑起来的，只有异星定居：守卫已经清除，留下来如今成了真正可承担的长期选择。在这里确认路线后，等你准备好时再回来一次，以定居者身份结束本轮。");
            case ENDING_FAILURE:
            case ENDING_NONE:
            default:
                break;
        }
    }

    return Loc_PickLiteral("Loxi online. Multiple endings are now supported by the archive you actually recovered. Choose a route here, then carry it through in the world before the run truly ends.",
                           "洛希已上线。你实际回收回来的档案，如今已经支撑起多个可成立结局。请先在这里选定路线，再把它在世界里真正执行到底，本轮才会结束。");
}

static const char *GetStage7ArchiveReviewMessage(const TaskSystem *tasks) {
    const int availableEndingCount = Tasks_GetAvailableEndingCount(tasks);
    const bool peacefulEvidenceReady = IsPeacefulEndingEvidenceReady(tasks);
    const bool amplifierCrafted = tasks != NULL && tasks->signalAmplifierCrafted;

    if (availableEndingCount <= 0) {
        if (peacefulEvidenceReady && !amplifierCrafted) {
            return Loc_PickLiteral("Archive review complete. The peaceful-rescue evidence is already in place, but the Signal Amplifier still has to be built before that ending can be confirmed here.",
                                   "档案复核完成。和平救援所需的证据已经齐了，但信号放大器仍必须先制作出来，这个结局才能在这里被确认。");
        }
        return Loc_PickLiteral("Archive review complete. The main truth is assembled, but no ending line is fully locked yet. Recover the route-specific records that point toward heroic rescue, peaceful rescue, or settlement, and remember that settlement also requires the guardian to fall.",
                               "档案复核完成。主线真相已经拼齐，但三条终局线还没有任何一条被真正锁死。请继续补回那些分别指向强行救援、和平救援或异星定居的路线记录，并记住定居路线还需要守卫倒下。");
    }
    if (availableEndingCount == 1) {
        return FormatSingleEndingTerminalMessage(tasks,
                                                 "Archive review complete. The recovered route evidence now supports only %s. Confirm that route here, then carry out the final step it still requires.",
                                                 "档案复核完成。当前被你回收回来的路线证据，如今只支撑起“%s”这一个可成立结局。请先在这里确认路线，再完成它仍然要求的最后一步。");
    }

    return Loc_PickLiteral("Archive review complete. More than one ending is now supported by the route evidence you recovered. Interact here again when you are ready to choose which route you will actually carry through.",
                           "档案复核完成。当前被你回收回来的路线证据，已经支撑起不止一个可成立结局。等你准备好决定要真正执行哪条路线时，再次在这里交互。");
}

static bool TryRepairAtOxygenConsole(TaskSystem *tasks, GameMap *map, Player *player, char *message, size_t messageSize) {
    if (tasks->stage == 1) {
        if (Player_HasResources(player, RESOURCE_WOOD, 3) && Player_HasResources(player, RESOURCE_METAL_SCRAP, 2)) {
            Player_SpendResource(player, RESOURCE_WOOD, 3);
            Player_SpendResource(player, RESOURCE_METAL_SCRAP, 2);
            tasks->oxygenRepairLevel = 1;
            TasksRuntime_UnlockStageIfNeeded(tasks, map, 2);
            Player_AddOxygen(player, 18.0f);
            TasksRuntime_WriteMessage(message, messageSize, Loc_PickLiteral("The first lower-deck oxygen module is breathing again. Before you push deeper, make sure you carry light and something reliable in your hands.", "下层甲板的第一组氧气模块重新喘起来了。继续往深处走之前，先把照明和基础装备准备稳。"));
            return true;
        }

        TasksRuntime_WriteMessage(message, messageSize, Loc_PickLiteral("The first oxygen repair still needs 3 Wood and 2 Metal Scrap before this deck stops bleeding air.", "这次首次氧气修复还缺 3 份木材和 2 份金属残片，下层甲板的漏气还没法止住。"));
        return true;
    }

    if (tasks->stage == 2) {
        if (player->hasGlowStick && Player_HasResources(player, RESOURCE_GLOW_MOSS, 1) && Player_HasResources(player, RESOURCE_ORE, 1)) {
            Player_SpendResource(player, RESOURCE_GLOW_MOSS, 1);
            Player_SpendResource(player, RESOURCE_ORE, 1);
            tasks->oxygenRepairLevel = 2;
            TasksRuntime_UnlockStageIfNeeded(tasks, map, 3);
            Player_AddOxygen(player, 35.0f);
            TasksRuntime_WriteMessage(message, messageSize, Loc_PickLiteral("The lower-deck oxygen system is fully restored. The ship can finally spare enough breath for the outer swamp route.", "下层甲板的氧气系统已经完全恢复。飞船终于能挤出足够的呼吸余量，支撑你打开外层沼泽路线。"));
            return true;
        }

        TasksRuntime_WriteMessage(message, messageSize, Loc_PickLiteral("The final oxygen repair still needs a Glow Stick, 1 Glow Moss, and 1 Ore. Do not open the ship wider without them.", "最终氧气修复还需要一根荧光棒、1 份发光苔和 1 份矿石。在这些东西齐备之前，别急着把飞船和外界彻底打通。"));
        return true;
    }

    if (Map_GetAreaAt(player->gridX, player->gridY) == MAP_AREA_BASE) {
        player->pressure = INITIAL_PRESSURE;
        player->oxygen = MAX_OXYGEN;
        Player_ClearStatus(player, PLAYER_STATUS_LOW_OXYGEN);
        Player_ClearStatus(player, PLAYER_STATUS_SUFFOCATING);
        Player_ClearStatus(player, PLAYER_STATUS_OXYGEN_LEAK);
        Player_ClearStatus(player, PLAYER_STATUS_CRITICAL_CONDITION);
        TasksRuntime_WriteMessage(message,
                                  messageSize,
                                  Loc_PickLiteral("Base oxygen reset complete. Your oxygen reserves are full, pressure has settled, and the breathing alerts finally shut up.",
                                                  "基地氧气控制台重置完成：氧气储备已补满，气压重新稳定，恼人的呼吸警报也总算停了。"));
        return true;
    }

    return false;
}

static bool TryUseLoxiTerminal(TaskSystem *tasks, GameMap *map, Player *player, char *message, size_t messageSize) {
    bool activatedThisUse;
    int availableEndingCount;
    GameEnding selectedRoute;

    activatedThisUse = false;
    availableEndingCount = Tasks_GetAvailableEndingCount(tasks);
    selectedRoute = Tasks_GetSelectedEndingRoute(tasks);
    if (!tasks->communicatorUnlocked) {
        tasks->communicatorUnlocked = true;
        if (map != NULL) {
            Map_UnlockLoxiRoom(map);
        }
        activatedThisUse = true;
    }

    if (tasks->stage == 6 && Player_HasResources(player, RESOURCE_RELIC_FRAGMENT, 3)) {
        tasks->amplifierUnlocked = true;
        TasksRuntime_UnlockStageIfNeeded(tasks, map, 7);
        if (IsCrossX2Ready(tasks)) {
            TasksRuntime_WriteMessage(message, messageSize, Loc_PickLiteral("Loxi finished reading the fragment set: west crew traces, purifier controls, and monolith routing now point back to the same failing old system. The shards are fully mapped, so the Signal Amplifier can still be built later. Finish the main archive, then come back and decide with the whole situation in view.", "洛希已经完成对整套碎片的解读：西线船员痕迹、净化控制与石碑路径如今都指回同一套正在失效的旧系统。这些碎片已经被完整映射，之后依然可以拿去制作信号放大器。请先补完整个主线档案，再回来，在看清整体局势后作出决定。"));
        } else if (IsCrossX1Ready(tasks)) {
            TasksRuntime_WriteMessage(message, messageSize, Loc_PickLiteral("Loxi finished the fragment sync: west crew traces and south facility handovers can finally be pieced together. The fragment set remains intact, so it can still anchor the Signal Amplifier once the final choice is made. Heroic rescue, peaceful rescue, and settlement are all visible now, but the main archive still has to be finished before the final choice opens.", "洛希完成了碎片同步：西线船员痕迹与南方设施交接记录终于能够拼到一起。这套碎片本身仍然完好，等最终路线确认后依然可以拿去制作信号放大器。强行救援、和平救援和异星定居都已经浮现，但在最终选择开放前，你仍需补完主线档案。"));
        } else {
            TasksRuntime_WriteMessage(message, messageSize, Loc_PickLiteral("Loxi finished the endgame analysis: the ruins now resolve into three real outcomes. The fragment set is fully mapped and can still be used to craft the Signal Amplifier for peaceful rescue. Face the guardian for heroic rescue, or keep the base and choose settlement deliberately.", "洛希完成了终局分析：遗迹如今清晰地分成三种真实结局。这套碎片已经被完整映射，之后仍可用于制作和平救援所需的信号放大器。你也可以直面守卫走强行救援，或者保留基地，主动选择异星定居。"));
        }
        return true;
    }

    if (tasks->stage == 6) {
        if (IsCrossX2Ready(tasks)) {
            TasksRuntime_WriteMessage(message, messageSize, Loc_PickLiteral("Loxi already has enough west and south evidence to see how the tower system failed, but it still needs 3 Relic Fragments before the final rescue-versus-settlement choice can be explained cleanly.", "洛希已经拥有足够的西线与南线证据来弄清塔楼系统是怎样失效的，但它仍需要 3 枚遗迹碎片，才能把最终的救援与定居分歧讲清楚。"));
        } else if (IsCrossX1Ready(tasks)) {
            TasksRuntime_WriteMessage(message, messageSize, Loc_PickLiteral("Loxi can already connect the west crew trail with the south facility record, but it still needs 3 Relic Fragments before the final choice can be laid out clearly.", "洛希已经能把西线船员轨迹与南方设施记录接起来，但它仍需要 3 枚遗迹碎片，最终选择才能真正被讲清楚。"));
        } else {
            TasksRuntime_WriteMessage(message, messageSize, Loc_PickLiteral("Loxi needs 3 Relic Fragments at the upper terminal before the final options can be explained clearly. Bring the full set back here so the ruins stop feeling like chaos and start reading like a plan.", "在能够清楚解释最终选项之前，洛希需要你把 3 枚遗迹碎片带回上层终端。把整套碎片带回来，让这片遗迹不再像一团混乱，而开始显露出计划。"));
        }
        return true;
    }

    if (tasks->stage == 3) {
        TasksRuntime_WriteMessage(message,
                                  messageSize,
                                  Loc_PickLiteral("Loxi online. Cycle the east airlock, gather Vine, Shell Fruit, and Special Fungus, then restore the comm relay. Once that link is clean, I can turn West Frontier from static into a recoverable archive trail.",
                                                  "洛希已上线。先打开东侧气闸，收集藤蔓、壳果和特殊菌株，再把通讯中继修好。链路一旦恢复，西部前线就会从噪声变成真正可回收的档案路线。"));
        return true;
    }

    if (tasks->stage == 4) {
        TasksRuntime_WriteMessage(message,
                                  messageSize,
                                  Loc_PickLiteral("Loxi online. Build the Laser Gun and Protection Suit first. Then inspect the eastern wreck clue and begin pulling in the West Frontier archive line now that it can finally be read cleanly.",
                                                  "洛希已上线。先把激光枪和防护服做出来，再去确认东侧残骸线索，并开始回收那条终于能被清晰读出的西部前线档案线。"));
        return true;
    }

    if (tasks->stage == 5) {
        if (Player_HasResources(player, RESOURCE_ENERGY_CORE, 1)) {
            TasksRuntime_WriteMessage(message,
                                      messageSize,
                                      Loc_PickLiteral("Loxi confirms the Energy Core is free. Install it in Power Bay, finish the ore-side bracing there, and you will reopen the south facility archive line while clearing the north ruins approach.",
                                                      "洛希确认能源核心已经成功拆出。把它送回动力舱，并在那里完成矿石侧的加固修复；这样既能重新打开南线设施档案，也能清出北线遗迹入口。"));
        } else if (HasEnergyCoreExtractionMaterials(player)) {
            TasksRuntime_WriteMessage(message,
                                      messageSize,
                                      Loc_PickLiteral("Loxi online. Your extraction kit is complete. Go back to the wreck, pull the Energy Core free, and bring it home so the south archive route and north ruins access can both open cleanly.",
                                                      "洛希已上线。你的提取材料已经齐了。回到残骸，把能源核心完整拆出来并带回飞船，这样南线档案路线和北线遗迹入口才能一起恢复。"));
        } else {
            TasksRuntime_WriteMessage(message,
                                      messageSize,
                                      Loc_PickLiteral("Loxi online. The core is still fused inside the wreck. Gather 1 Junk Metal, 1 Protective Fiber, and 1 Energy Crystal along the east route, then return for extraction. That handoff is what reopens the south archive trail and clears the north ruins approach.",
                                                      "洛希已上线。能源核心本体仍卡死在残骸里。先去东线收集 1 份废旧金属、1 份防护纤维和 1 份能量晶体，再回去做拆取。正是这次回收交接，会重新打开南线档案路线并打通北线遗迹入口。"));
        }
        return true;
    }

    if (tasks->stage == 2) {
        if (CountCollectedShipIntroLogs(tasks) < 3) {
            TasksRuntime_WriteMessage(message,
                                      messageSize,
                                      Loc_PickLiteral("Loxi online. Do not rush outside yet. Recover the ship archive set in Cargo Hold, Crew Quarters, and Diagnostics, then complete the final oxygen repair that opens the east airlock.",
                                                      "洛希已上线。先别急着出舱。去货舱、船员舱和诊断舱补齐船内档案，再完成那次会开启东侧气闸的最终氧气修复。"));
        } else {
            TasksRuntime_WriteMessage(message,
                                      messageSize,
                                      Loc_PickLiteral("Loxi online. The opening ship archive is stable now. Craft a Glow Stick, use 1 Glow Moss and 1 Ore for the final oxygen repair, then open the east airlock for your first real field archive run.",
                                                      "洛希已上线。开场船内档案已经足够稳定。先制作荧光棒，再用 1 份发光苔和 1 份矿石完成最终氧气修复，然后打开东侧气闸，开始第一趟真正的外勤档案回收。"));
        }
        return true;
    }

    if (tasks->stage == 1) {
        if (CountCollectedShipIntroLogs(tasks) < 3) {
            TasksRuntime_WriteMessage(message,
                                      messageSize,
                                      Loc_PickLiteral("Loxi online. Recover the first ship archives from Cargo Hold, Crew Quarters, and Diagnostics, then repair the first oxygen module. Before we open anything wider, we need to understand what happened at the start.", "洛希已上线。先回收货舱、船员舱和诊断舱里的第一批飞船档案，再修好第一组氧气模块。在扩大路线之前，我们得先弄清楚开头到底发生了什么。"));
        } else {
            TasksRuntime_WriteMessage(message,
                                      messageSize,
                                      Loc_PickLiteral("Loxi online. The opening ship archive now tells the beginning clearly enough. Finish the first oxygen repair before the ship loses breathing margin faster than we can keep up with it.",
                                                      "洛希已上线。开场飞船档案现在已经足够把事情的开头说清楚。现在就把第一组氧气修复做完，别让飞船的呼吸余量掉得比我们的整理速度还快。"));
        }
        return true;
    }

    if (tasks->stage == 7) {
        if (Tasks_IsEndingBranchReady(tasks) && selectedRoute == ENDING_NONE) {
            if (!tasks->endingArchiveReviewed) {
                tasks->endingArchiveReviewed = true;
                Tasks_UpdateObjective(tasks, player);
                TasksRuntime_WriteMessage(message, messageSize, GetStage7ArchiveReviewMessage(tasks));
            } else {
                TasksRuntime_WriteMessage(message, messageSize, GetStage7RouteChoiceMessage(tasks));
            }
        } else if (selectedRoute == ENDING_SETTLEMENT) {
            if (!tasks->bossDefeated) {
                TasksRuntime_WriteMessage(message,
                                          messageSize,
                                          Loc_PickLiteral("Loxi online. Settlement route marked, but it is still unsafe to stay while the guardian controls the northwest ruins. Finish that fight first, then return here to confirm settlement.",
                                                          "洛希已上线。定居路线已经标记，但在守卫仍控制西北遗迹时，留下来依然不安全。先解决那场战斗，再回来确认定居。"));
            } else {
                Tasks_CommitSettlement(tasks);
                Tasks_UpdateObjective(tasks, player);
                TasksRuntime_WriteMessage(message,
                                          messageSize,
                                          Loc_PickLiteral("Loxi online. Settlement route confirmed. The guardian is gone, the archive is closed, and this run now ends with you choosing to stay and inherit the work here.",
                                                          "洛希已上线。定居路线已确认。守卫已经清除，档案也已闭合，而本轮会以你选择留下并继承这里的工作作为终点。"));
            }
        } else if (selectedRoute == ENDING_HEROIC) {
            if (tasks->bossDefeated) {
                TasksRuntime_WriteMessage(message, messageSize, Loc_PickLiteral("Loxi online. Heroic route confirmed. The guardian is down. Nothing stands between you and the Signal Tower now except the choice to finish what you started.", "洛希已上线。强行救援路线已确认。守卫已经倒下，如今挡在你与信号塔之间的，只剩下把这条路走到底的决心。"));
            } else if (tasks->monolithsLit >= 3) {
                TasksRuntime_WriteMessage(message, messageSize, Loc_PickLiteral("Loxi online. Heroic route confirmed. The guardian is waiting in the northwest ruins, and the completed monolith ring should steady that fight once you push in.", "洛希已上线。强行救援路线已确认。守卫正在西北遗迹等着你，而完整石碑环会在你真正推进那场战斗时帮你稳住局面。"));
            } else {
                TasksRuntime_WriteMessage(message, messageSize, Loc_PickLiteral("Loxi online. Heroic route confirmed. Hunt the guardian in the northwest ruins first. Any monolith work you already finished will still carry into that fight.", "洛希已上线。强行救援路线已确认。先去西北遗迹猎杀守卫。你此前完成的任何石碑准备，都会继续带进那场战斗。"));
            }
        } else if (selectedRoute == ENDING_PEACEFUL) {
            if (player->hasSignalAmplifier) {
                TasksRuntime_WriteMessage(message, messageSize, Loc_PickLiteral("Loxi online. Peaceful route confirmed. The Signal Amplifier is ready. Carry it to the tower and steady the system instead of breaking it open.", "洛希已上线。和平救援路线已确认。信号放大器已经准备好，把它带去塔楼，稳住整套系统，而不是再把它强行撕开。"));
            } else if (Player_HasResources(player, RESOURCE_RELIC_FRAGMENT, 3)) {
                TasksRuntime_WriteMessage(message, messageSize, Loc_PickLiteral("Loxi online. Peaceful route confirmed. The fragment set is ready. Craft the Signal Amplifier at the workshop, then carry that answer to the tower.", "洛希已上线。和平救援路线已确认。碎片已经齐备，先去工坊制作信号放大器，再把这份答案带去塔楼。"));
            } else {
                TasksRuntime_WriteMessage(message, messageSize, Loc_PickLiteral("Loxi online. Peaceful route confirmed. Recover 3 Relic Fragments, craft the Signal Amplifier at the workshop, then carry it to the tower before this route hardens into the harsher one.", "洛希已上线。和平救援路线已确认。先找回 3 枚遗迹碎片，在工坊制作信号放大器，再把它带去塔楼，别让这条路最终只剩下更强硬的做法。"));
            }
        } else if (Tasks_IsEndingAvailable(tasks, ENDING_SETTLEMENT)) {
            TasksRuntime_WriteMessage(message, messageSize, GetStage7RouteChoiceMessage(tasks));
        } else if (IsCrossX3Ready(tasks)) {
            if (availableEndingCount <= 0) {
                TasksRuntime_WriteMessage(message, messageSize, Loc_PickLiteral("Loxi online. West crew testimony and south system truth are already synchronized, but none of the route-ending records is fully locked yet. Recover the Canopy Handoff record, a south-line control record, or the Last Camp testament plus the guardian kill to decide how this run can end.", "洛希已上线。西线船员证词与南方系统真相已经同步，但终局路线记录还没有哪一条真正锁死。请补回林冠交接记录、南线控制记录，或补回最后营地遗言并击败守卫，才能决定本轮可以如何结束。"));
            } else {
                TasksRuntime_WriteMessage(message, messageSize, GetStage7RouteChoiceMessage(tasks));
            }
        } else if (IsCrossX2Ready(tasks)) {
            TasksRuntime_WriteMessage(message, messageSize, Loc_PickLiteral("Loxi online. The late findings have changed the picture: tower, purifier ring, and monolith network all point back to the same failing old system. Finish the remaining mainline logs before the final choice opens.", "洛希已上线。后段调查已经改变了局势判断：塔楼、净化环和石碑网络都指向同一套正在失效的旧系统。请先补齐剩余主线日志，最终选择才会开启。"));
        } else if (IsCrossX1Ready(tasks)) {
            TasksRuntime_WriteMessage(message, messageSize, Loc_PickLiteral("Loxi online. West-side handoffs and south-side records can finally be read together. Keep clearing archive work so the final choice rests on the whole story, not fragments.", "洛希已上线。西侧交接记录与南侧记录终于能够放在一起读了。继续推进档案回收，让最终选择建立在完整来龙去脉上，而不是零碎片段。"));
        } else {
            TasksRuntime_WriteMessage(message, messageSize, Loc_PickLiteral("Loxi online. The ending is not open yet. Recover the remaining mainline logs and finish the west and south investigations before choosing an ending.", "洛希已上线。结局分支尚未开放。请先找回剩余主线日志，并完成西线与南线调查，再来选择结局。"));
        }
        return true;
    }

    if (Tasks_IsEndingAvailable(tasks, ENDING_SETTLEMENT)) {
        TasksRuntime_WriteMessage(message, messageSize, GetStage7RouteChoiceMessage(tasks));
        return true;
    }

    if (activatedThisUse) {
        TasksRuntime_WriteMessage(message, messageSize, Loc_PickLiteral("Upper cabin terminal synced. The room seal is gone, and N now opens Loxi guidance plus recovered logs. Start with the ship archive in Cargo Hold, Crew Quarters, and Diagnostics.", "上层舱室终端已同步，房门限制已经解除。随时按 N 查看洛希指引与档案日志。先从货舱、船员舱和诊断舱的船内档案开始。"));
        return true;
    }

    TasksRuntime_WriteMessage(message, messageSize, Loc_PickLiteral("Loxi uplink stable. Press N to review objectives, route guidance, and recovered logs.", "洛希上行链路稳定。按 N 查看当前目标、路线指引和已回收日志。"));
    return true;
}

static bool TryRoomInteraction(TaskSystem *tasks, Player *player, char *message, size_t messageSize) {
    const char *roomName;

    roomName = Map_GetRoomNameAt(player->gridX, player->gridY);

    if (std::strcmp(roomName, "Central Corridor") == 0) {
        if (tasks->stage == 3) {
            TasksRuntime_WriteMessage(message, messageSize, Loc_PickLiteral("Central Corridor: every first sortie passes through here, between the terminal glow, the airlock seal, and the ship's last clean air.", "中央走廊：第一次外出行动都会经过这里，一侧是终端的冷光，一侧是气闸密封，再往里就是飞船仅剩的稳定空气。"));
        } else if (tasks->stage == 5) {
            TasksRuntime_WriteMessage(message, messageSize, Loc_PickLiteral("Central Corridor: the last stretch of dry metal before another eastern push into wet ground and wreck light.", "中央走廊：在你再次踏入东线湿地和残骸冷光前，这里是最后一段干燥而安静的金属通道。"));
        } else if (tasks->stage >= 7) {
            TasksRuntime_WriteMessage(message, messageSize, Loc_PickLiteral("Central Corridor: it has become the ship's decision corridor. Rescue, repair, or settlement; one of those choices leaves this hall with you.", "中央走廊：这里已经变成整艘船的抉择走廊。救援、修复，还是定居，总有一个决定会从这里被你真正带走。"));
        } else {
            TasksRuntime_WriteMessage(message, messageSize, Loc_PickLiteral("Central Corridor: the ship's working spine, linking sleep, planning, repairs, and whatever comes next.", "中央走廊：整艘船仍在运作的脊骨，把休整、规划、修理和下一步行动全都串在一起。"));
        }
        return true;
    }

    if (std::strcmp(roomName, "Cargo Hold") == 0) {
        TasksRuntime_WriteMessage(message,
                                  messageSize,
                                  Loc_PickLiteral("Cargo Hold: tied-down crates, spare parts, and the smell of old impact foam. Useful stock, but no comfort lives here.",
                                                  "货舱：捆牢的箱体、备用零件和撞击缓冲材料的陈味还留在这里。它能提供物资，却给不了半点安稳。"));
        return true;
    }

    if (std::strcmp(roomName, "Crew Quarters") == 0) {
        player->pressure = INITIAL_PRESSURE;
        player->health = Player_GetMaxHealth(player);
        Player_AddOxygen(player, 18.0f);
        Player_RecoverStamina(player, 34.0f);
        TasksRuntime_ReducePoisonAtRecovery(player, 12.0f);
        TasksRuntime_WriteMessage(message,
                                  messageSize,
                                  Loc_PickLiteral("Crew Quarters: the bunk finally lets your body unclench. You fully recover your health here, your pulse drops, and the ship almost feels inhabited again for a minute.", "船员舱：躺回床铺后，身体总算能短暂松开。你会在这里把生命值完全恢复，心率也会回落，整艘船在这一刻几乎像是又有人真正住在这里。"));
        return true;
    }

    if (std::strcmp(roomName, "Diagnostics") == 0) {
        if (tasks->stage == 4) {
            TasksRuntime_WriteMessage(message, messageSize, Loc_PickLiteral("Diagnostics: residue forecasts say the wreck should be approached with the suit sealed and the laser ready.", "诊断舱：残留物预测显示，靠近那处残骸时最好先封好防护服，并把激光枪准备在手。"));
        } else if (tasks->stage == 5) {
            TasksRuntime_WriteMessage(message, messageSize, Loc_PickLiteral("Diagnostics: the deep east route is still unstable. Treat it like a salvage run for extractor parts, not a casual detour, and plan a Deep Gate fallback before you go in.", "诊断舱：深层东线依旧被判定为不稳定区域。把这次行动当成一趟为提取核心而做的打捞任务，而不是顺路查看；深入前先以深层入口为基准，把回撤方案想清楚。"));
        } else if (tasks->stage >= 7) {
            TasksRuntime_WriteMessage(message, messageSize, Loc_PickLiteral("Diagnostics: final-route scans are archived and ugly in all the expected places. Rest first, top off oxygen second, then make the last push.", "诊断舱：终局路线扫描已经归档，该难看的地方一个不少。先休息，再补满氧气，然后再去走最后一程。"));
        } else {
            TasksRuntime_WriteMessage(message, messageSize, Loc_PickLiteral("Diagnostics: this room still reads the ship honestly, even if healing and resupply now happen elsewhere.", "诊断舱：这里仍会如实读出飞船的状态，只不过恢复和补给已经不在这里进行了。"));
        }
        return true;
    }

    if (std::strcmp(roomName, "Terminal Bay") == 0) {
        if (tasks->stage == 3) {
            TasksRuntime_WriteMessage(message, messageSize, Loc_PickLiteral("Terminal Bay: come back here after the first relay sortie so Loxi can turn raw field noise into a route you can actually follow.", "终端舱：完成第一次中继行动后回到这里，让洛希把现场带回来的噪声整理成真正能执行的路线。"));
        } else if (tasks->stage == 5 && Player_HasResources(player, RESOURCE_ENERGY_CORE, 1)) {
            TasksRuntime_WriteMessage(message, messageSize, Loc_PickLiteral("Terminal Bay: the Energy Core is in hand. Take it to Power Bay and finish the remaining ore bracing there before anything else slips again.", "终端舱：能源核心已经到手。先把它送去动力舱，并在那里完成剩余的矿石加固，别让系统再次掉回停摆。"));
        } else if (tasks->stage == 5 && HasEnergyCoreExtractionMaterials(player)) {
            TasksRuntime_WriteMessage(message, messageSize, Loc_PickLiteral("Terminal Bay: the extraction kit is ready. Go back to the wreck, free the Energy Core, then bring it straight to Power Bay.", "终端舱：提取材料已经备齐。回到残骸，把能源核心完整拆出，再直接送去动力舱。"));
        } else if (tasks->stage == 6 && Player_HasResources(player, RESOURCE_RELIC_FRAGMENT, 3)) {
            TasksRuntime_WriteMessage(message, messageSize, Loc_PickLiteral("Terminal Bay: all fragments are in. Sync with Loxi and let the bigger pattern come into focus.", "终端舱：碎片已经收齐。和洛希同步，让更大的图景真正显出来。"));
        } else if (tasks->stage == 6) {
            TasksRuntime_WriteMessage(message, messageSize, Loc_PickLiteral("Terminal Bay: the sync will not mean much until every fragment is on the table.", "终端舱：在所有碎片都带回来之前，这里的同步还无法得出完整结论。"));
        } else if (tasks->stage >= 7) {
            TasksRuntime_WriteMessage(message,
                                      messageSize,
                                      Loc_PickLiteral("Terminal Bay: go over the assembled archive with Loxi here, then decide which route this run is really going to carry through.", "终端舱：在这里和洛希把拼齐的档案重新过一遍，然后决定本轮最终到底要把哪条路线真正执行到底。"));
        } else {
            TasksRuntime_WriteMessage(message, messageSize, Loc_PickLiteral("Terminal Bay: this is where reports stop being memories and become direction.", "终端舱：这里会把汇报从零散记忆变成下一步真正可执行的方向。"));
        }
        return true;
    }

    if (std::strcmp(roomName, "Life Support") == 0) {
        if (tasks->stage <= 2) {
            TasksRuntime_WriteMessage(message, messageSize, Loc_PickLiteral("Life Support: the ship's lungs, stripped down to what still works. Use the oxygen console when you need those lungs to work for you.", "生命维持舱：这是飞船仅剩还能运转的肺。需要它替你继续呼吸时，就去操作旁边的氧气控制台。"));
        } else if (tasks->stage == 5) {
            TasksRuntime_WriteMessage(message, messageSize, Loc_PickLiteral("Life Support: the east salvage run will chew through your reserves. Refill at the oxygen console before you step out.", "生命维持舱：东线打捞会狠狠干掉你的储备。出发前先去氧气控制台把氧气补满。"));
        } else if (tasks->stage >= 7) {
            TasksRuntime_WriteMessage(message, messageSize, Loc_PickLiteral("Life Support: the tower route bites harder than the ship ever did. Fill your tank before the final climb.", "生命维持舱：塔楼路线的压力比飞船内部更狠。最终攀登前，先把氧气储备补满。"));
        } else {
            TasksRuntime_WriteMessage(message, messageSize, Loc_PickLiteral("Life Support is stable for now. Sleep in the cabin when your body gives out, and use the oxygen console when your tank does.", "生命维持舱目前还算稳定。身体撑不住就去船员舱休息，氧气见底了就去控制台补。"));
        }
        return true;
    }

    if (std::strcmp(roomName, "Workshop") == 0) {
        if (!player->hasLaserGun && tasks->stage >= 4) {
            TasksRuntime_WriteMessage(message, messageSize, Loc_PickLiteral("Workshop: if you are heading back outside now, you should not do it without the Laser Gun and Protection Suit.", "工坊：如果你现在准备再出舱，那就别在没做出激光枪和防护服的情况下硬走。"));
        } else if (tasks->stage == 5) {
            TasksRuntime_WriteMessage(message, messageSize, Loc_PickLiteral("Workshop: this phase is salvage preparation now. Bring camp support, breathing backup, and enough space to haul junk metal, fiber, and a live crystal back intact.", "工坊：这一阶段的重点已经变成打捞准备。记得带上营地支援、补氧手段，并留够空间，把废旧金属、防护纤维和带电晶体完整带回来。"));
        } else if (tasks->stage >= 7
                   && tasks->amplifierUnlocked
                   && !player->hasSignalAmplifier
                   && (tasks->selectedEndingRoute == ENDING_NONE || tasks->selectedEndingRoute == ENDING_PEACEFUL)
                   && Player_HasResources(player, RESOURCE_RELIC_FRAGMENT, 3)) {
            TasksRuntime_WriteMessage(message, messageSize, Loc_PickLiteral("Workshop: the fragment map is complete. Build the Signal Amplifier here if you want the peaceful route to stay real.", "工坊：碎片映射已经完成。如果你想让和平路线真正成立，就在这里把信号放大器做出来。"));
        } else if (tasks->stage >= 7
                   && tasks->amplifierUnlocked
                   && !player->hasSignalAmplifier
                   && (tasks->selectedEndingRoute == ENDING_NONE || tasks->selectedEndingRoute == ENDING_PEACEFUL)) {
            TasksRuntime_WriteMessage(message, messageSize, Loc_PickLiteral("Workshop: peaceful rescue still needs 3 Relic Fragments before the Signal Amplifier can be built here.", "工坊：和平救援还得先找回 3 枚遗迹碎片，才能在这里把信号放大器真正组装出来。"));
        } else if (player->hasLaserGun) {
            TasksRuntime_WriteMessage(message, messageSize, Loc_PickLiteral("The workshop racks still hold spare cells, scorched tools, and old combat notes no one expected to matter this much.", "工坊架子上还留着备用电池、烧灼过的工具和旧战斗笔记，原本谁也没想到它们会在这里变得这么重要。"));
        } else {
            TasksRuntime_WriteMessage(message, messageSize, Loc_PickLiteral("The workshop is still locked down behind dead systems. Restore more of the ship before expecting it to help you.", "工坊还被停摆系统锁着。先把飞船再救回来一点，它才有可能开始帮你。"));
        }
        return true;
    }

    if (std::strcmp(roomName, "Power Bay") == 0) {
        if (tasks->stage == 5 && Player_HasResources(player, RESOURCE_ENERGY_CORE, 1)) {
            TasksRuntime_WriteMessage(message, messageSize, Loc_PickLiteral("Power Bay: slot the extracted Energy Core in now and finish the ore bracing. That handoff reopens South Collapse, clears the north route, and turns eastern salvage into real progress.", "动力舱：现在就把拆出的能源核心装进去，并把矿石支架加固做完。这次交接会重新打开南部塌陷区、打通北线路线，并把东线打捞真正转化成主线推进。"));
        } else if (tasks->stage == 5 && HasEnergyCoreExtractionMaterials(player)) {
            TasksRuntime_WriteMessage(message, messageSize, Loc_PickLiteral("Power Bay: the socket is ready, but the core is still trapped in the wreck. Go free it, then come back for final installation.", "动力舱：安装位已经准备好了，但核心还卡在残骸里。先把它拆出来，再回来做最终安装。"));
        } else if (tasks->stage == 5) {
            TasksRuntime_WriteMessage(message, messageSize, Loc_PickLiteral("Power Bay is waiting, but the core is still trapped in the wreck. Final ore bracing cannot begin until you bring it home.", "动力舱已经待命，但能源核心仍被困在残骸里；在你把它带回来之前，最后的矿石加固根本没法开始。"));
        } else if (tasks->stage >= 6) {
            TasksRuntime_WriteMessage(message, messageSize, Loc_PickLiteral("Power Bay restored. The ship's heart is beating properly again, South Collapse is back online, and the north ruins route has opened.", "动力舱已恢复。飞船的动力心脏重新跳了起来，南部塌陷区重新可用，北方遗迹路线也已经打开。"));
        } else {
            TasksRuntime_WriteMessage(message, messageSize, Loc_PickLiteral("Power Bay is still one of the ship's sleeping systems. Sooner or later, you will have to wake it cleanly.", "动力舱仍是飞船里沉睡的系统之一。迟早，你都得把它彻底唤醒。"));
        }
        return true;
    }

    return false;
}

static bool TryUseAirlockConsole(TaskSystem *tasks, GameMap *map, Player *player, char *message, size_t messageSize) {
    (void)player;

    if (tasks->stage < 3) {
        TasksRuntime_WriteMessage(message, messageSize, Loc_PickLiteral("The airlock is still sealed. Restore the oxygen system before you ask the ship to face the swamp.", "气闸仍然封着。在让飞船真正面对外面那片沼泽之前，先把氧气系统修好。"));
        return true;
    }

    if (!Map_IsSwampOuterUnlocked(map)) {
        Map_UnlockSwampOuter(map);
        TasksRuntime_WriteMessage(message, messageSize, Loc_PickLiteral("Airlock cycle complete. The outer blast door groans open, and the swamp is now on the other side of a single threshold.", "气闸循环完成。外层防爆门低沉地打开，外面的沼泽如今只隔着最后一道门槛。"));
        return true;
    }

    Map_LockSwampOuter(map);
    TasksRuntime_WriteMessage(message, messageSize, Loc_PickLiteral("Airlock cycle reversed. The outer blast door seals shut again.", "气闸循环已反转。外层防爆门重新严丝合缝地闭合。"));
    return true;
}

static bool TryUseEnergyConsole(TaskSystem *tasks, GameMap *map, Player *player, char *message, size_t messageSize) {
    if (tasks->stage == 5) {
        if (Player_HasResources(player, RESOURCE_ENERGY_CORE, 1)
            && Player_HasResources(player, RESOURCE_ORE, 2)) {
            Player_SpendResource(player, RESOURCE_ENERGY_CORE, 1);
            Player_SpendResource(player, RESOURCE_ORE, 2);
            tasks->energyRepairLevel = 1;
            TasksRuntime_UnlockStageIfNeeded(tasks, map, 6);
            TasksRuntime_WriteMessage(message, messageSize, Loc_PickLiteral("The power module is fully restored. Your eastern salvage paid off, the ruins entrance is open, and Loxi can finally read the north route as a deliberate endgame path instead of raw hazard.", "动力模块已完全恢复。你在东线的打捞准备终于有了回报，遗迹入口现已开启，洛希也总算能把北线路线读作真正的终局路径，而不再只是原始危险区。"));
            return true;
        }

        TasksRuntime_WriteMessage(message, messageSize, Loc_PickLiteral("Power repair still needs 1 Energy Core and 2 Ore. The live crystal was already burned stabilizing the wreck extraction.", "动力修复仍需要 1 个能源核心和 2 份矿石。那枚带电晶体已经在残骸提取阶段被消耗掉，用来稳定核心仓了。"));
        return true;
    }

    if (tasks->energyRepairLevel >= 1 || tasks->stage >= 6) {
        TasksRuntime_WriteMessage(message, messageSize, Loc_PickLiteral("The power console is operating normally, almost like nothing ever failed here.", "动力控制台运转正常，几乎像这里从未经历过停摆。"));
        return true;
    }

    TasksRuntime_WriteMessage(message,
                              messageSize,
                              Loc_PickLiteral("The power console is still on standby. The bay has not truly woken up yet.",
                                              "动力控制台仍处于待机状态，动力舱还远没有真正醒过来。"));
    return true;
}

bool TasksRuntime_HandleShipInteraction(TaskSystem *tasks,
                                        GameMap *map,
                                        Player *player,
                                        TaskInteractionTarget target,
                                        char *message,
                                        size_t messageSize) {
    switch (target) {
        case TASK_INTERACTION_OXYGEN_CONSOLE:
            return TryRepairAtOxygenConsole(tasks, map, player, message, messageSize);
        case TASK_INTERACTION_LOXI_TERMINAL:
            return TryUseLoxiTerminal(tasks, map, player, message, messageSize);
        case TASK_INTERACTION_WORKBENCH:
            TasksRuntime_WriteMessage(message, messageSize, Loc_PickLiteral("Workbench ready. Press F to turn salvage into something that can keep you alive.", "工作台已就绪。按 F，把回收来的材料变成真正能帮你活下去的东西。"));
            return true;
        case TASK_INTERACTION_AIRLOCK_CONSOLE:
            return TryUseAirlockConsole(tasks, map, player, message, messageSize);
        case TASK_INTERACTION_ENERGY_CONSOLE:
            return TryUseEnergyConsole(tasks, map, player, message, messageSize);
        case TASK_INTERACTION_NONE:
        case TASK_INTERACTION_COMM_RELAY:
        case TASK_INTERACTION_CRASH_CLUE:
        case TASK_INTERACTION_MONOLITH_A:
        case TASK_INTERACTION_MONOLITH_B:
        case TASK_INTERACTION_MONOLITH_C:
        case TASK_INTERACTION_SIGNAL_TOWER:
        default:
            break;
    }

    if (Map_GetAreaAt(player->gridX, player->gridY) == MAP_AREA_BASE) {
        return TryRoomInteraction(tasks, player, message, messageSize);
    }

    return false;
}
