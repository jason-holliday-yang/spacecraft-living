#include "task_runtime_internal.h"

#include "localization.h"

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

static const char *GetStage7RouteChoiceMessage(const TaskSystem *tasks) {
    if (IsCrossX3Ready(tasks)) {
        return Loc_PickLiteral("Loxi online. The west crew trail and the buried system record now resolve into the same truth. Heroic rescue forces the beacon alive. Peaceful rescue steadies the lattice. Settlement means staying to inherit the work. Choose here before you touch the tower, while this is still a decision and not a reaction. Settlement still needs explicit confirmation because it closes both rescue routes.",
                               "洛希已上线。西侧船员证词与地下系统记录如今指向同一个真相。强行救援意味着强行发射代价高昂的信标，和平救援意味着稳定整套格网，而定居则意味着接手这份工作。请在碰触塔楼之前先在这里作出选择。异星定居仍需额外确认，因为它会关闭两条救援路线。");
    }

    return Loc_PickLiteral("Loxi online. The archive is complete, the west and south investigations are closed, and this terminal is now the point of no return. Choose heroic rescue, peaceful rescue, or settlement here before you touch the tower and turn this ending into momentum. Settlement still needs explicit confirmation because it closes both rescue routes.",
                           "洛希已上线。主线档案已经拼合完成，西线与南线调查也都结束，这个终端如今就是无法回头的分叉点。请在碰触塔楼之前，于这里选择强行救援、和平救援或异星定居。异星定居仍需额外确认，因为它会关闭两条救援路线。");
}

static const char *GetStage7ArchiveReviewMessage(const TaskSystem *tasks) {
    if (IsCrossX3Ready(tasks)) {
        return Loc_PickLiteral("Archive review complete. Loxi has assembled the west testimony, the south system record, and the main archive into one final picture. Heroic rescue forces the beacon through. Peaceful rescue stabilizes the lattice. Settlement means taking over the work. Interact here again when you are ready to commit, and choose with the whole story in view.",
                               "档案复核完成。洛希已经把西线证词、南方系统记录与主线档案拼成同一张终局图景。强行救援意味着强行发射代价高昂的信标，和平救援意味着稳定整套格网，而定居则意味着接手这份工作。等你准备好正式作出路线承诺时，再次在这里交互。");
    }

    return Loc_PickLiteral("Archive review complete. Loxi has assembled the main archive and both investigation chains into one final comparison. Heroic rescue, peaceful rescue, and settlement are now clear answers to the same truth. Interact here again when you are ready to commit, and choose knowing what each answer costs.",
                           "档案复核完成。洛希已经把主线档案与两条调查线整理成同一份最终比较。强行救援、和平救援与异星定居，如今都成为了对同一真相的明确回应。等你准备好正式作出路线承诺时，再次在这里交互。");
}

static bool TryRepairAtOxygenConsole(TaskSystem *tasks, GameMap *map, Player *player, char *message, size_t messageSize) {
    if (tasks->stage == 1) {
        if (Player_HasResources(player, RESOURCE_WOOD, 3) && Player_HasResources(player, RESOURCE_METAL_SCRAP, 2)) {
            Player_SpendResource(player, RESOURCE_WOOD, 3);
            Player_SpendResource(player, RESOURCE_METAL_SCRAP, 2);
            tasks->oxygenRepairLevel = 1;
            TasksRuntime_UnlockStageIfNeeded(tasks, map, 2);
            Player_AddOxygen(player, 18.0f);
            TasksRuntime_WriteMessage(message, messageSize, Loc_PickLiteral("First oxygen module repaired in the lower deck. Craft light gear before going deeper.", "下层甲板的第一组氧气模块已经修好。继续深入前，先准备基础装备。"));
            return true;
        }

        TasksRuntime_WriteMessage(message, messageSize, Loc_PickLiteral("Missing materials for the first oxygen repair: 3 Wood and 2 Metal Scrap.", "第一次氧气修复缺少材料：3 份木材和 2 份金属残片。"));
        return true;
    }

    if (tasks->stage == 2) {
        if (player->hasGlowStick && Player_HasResources(player, RESOURCE_GLOW_MOSS, 1) && Player_HasResources(player, RESOURCE_ORE, 1)) {
            Player_SpendResource(player, RESOURCE_GLOW_MOSS, 1);
            Player_SpendResource(player, RESOURCE_ORE, 1);
            tasks->oxygenRepairLevel = 2;
            TasksRuntime_UnlockStageIfNeeded(tasks, map, 3);
            Player_AddOxygen(player, 35.0f);
            TasksRuntime_WriteMessage(message, messageSize, Loc_PickLiteral("Lower-deck oxygen system fully repaired. The outer swamp is now open.", "下层甲板的氧气系统已经完全修复。外层沼泽现已开放。"));
            return true;
        }

        TasksRuntime_WriteMessage(message, messageSize, Loc_PickLiteral("Final oxygen repair requires a Glow Stick, 1 Glow Moss, and 1 Ore.", "最终氧气修复需要一根荧光棒、1 份发光苔和 1 份矿石。"));
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
                                  Loc_PickLiteral("Base oxygen console reset complete: oxygen reserves are full, pressure is stable, and breathing alerts are cleared.",
                                                  "基地氧气控制台重置完成：氧气储备已补满，气压恢复稳定，呼吸类警报也已清除。"));
        return true;
    }

    return false;
}

static bool TryUseLoxiTerminal(TaskSystem *tasks, GameMap *map, Player *player, char *message, size_t messageSize) {
    bool activatedThisUse;
    bool settlementAvailable;
    GameEnding selectedRoute;

    activatedThisUse = false;
    settlementAvailable = Tasks_CanChooseSettlement(tasks);
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
            TasksRuntime_WriteMessage(message, messageSize, Loc_PickLiteral("Loxi finished reading the fragment set: west crew traces, purifier controls, and monolith routing all resolve into one failing maintenance lattice. The shards are now fully mapped, so the Signal Amplifier can be built later without losing this analysis. Finish the main archive, then choose with the full picture.", "洛希已经完成对整套碎片的解读：西线船员痕迹、净化控制与石碑路径最终都指向同一套正在失效的维护格网。这些碎片的数据已经被完整映射，之后依然可以用于制作信号放大器。请先补完整个主线档案，再带着完整理解作出选择。"));
        } else if (IsCrossX1Ready(tasks)) {
            TasksRuntime_WriteMessage(message, messageSize, Loc_PickLiteral("Loxi finished the fragment sync: west crew traces and south facility handovers now read as one shared survival timeline. The fragment set remains intact, so it can still anchor the Signal Amplifier once the final choice is made. Heroic rescue, peaceful rescue, and settlement are all visible now, but the main archive still has to be finished before the final choice opens.", "洛希完成了碎片同步：西线船员痕迹与南方设施交接记录如今能够拼成一条共同的生存时间线。这套碎片本身仍然完好，等最终路线确认后依然可以拿去制作信号放大器。强行救援、和平救援和异星定居都已经浮现，但在最终选择开放前，你仍需补完主线档案。"));
        } else {
            TasksRuntime_WriteMessage(message, messageSize, Loc_PickLiteral("Loxi finished the endgame analysis: the ruins now resolve into three real outcomes. The fragment set is fully mapped and can still be used to craft the Signal Amplifier for peaceful rescue. Face the guardian for heroic rescue, or keep the base and choose settlement deliberately.", "洛希完成了终局分析：遗迹如今清晰地分成三种真实结局。这套碎片已经被完整映射，之后仍可用于制作和平救援所需的信号放大器。你也可以直面守卫走强行救援，或者保留基地，主动选择异星定居。"));
        }
        return true;
    }

    if (tasks->stage == 6) {
        if (IsCrossX2Ready(tasks)) {
            TasksRuntime_WriteMessage(message, messageSize, Loc_PickLiteral("Loxi already has enough west and south evidence to redraw the late-game picture, but it still needs 3 Relic Fragments before the final rescue-versus-settlement choice can be explained cleanly.", "洛希已经拥有足够的西线与南线证据来重构终局图景，但它仍需要 3 枚遗迹碎片，才能把最终的救援与定居分歧讲清楚。"));
        } else if (IsCrossX1Ready(tasks)) {
            TasksRuntime_WriteMessage(message, messageSize, Loc_PickLiteral("Loxi can already align the west crew trail with the south facility record, but it still needs 3 Relic Fragments before the final choice becomes readable.", "洛希已经能把西线船员轨迹与南方设施记录对齐，但它仍需要 3 枚遗迹碎片，最终选择才能真正被读懂。"));
        } else {
            TasksRuntime_WriteMessage(message, messageSize, Loc_PickLiteral("Loxi needs 3 Relic Fragments at the upper terminal before the final options can be explained clearly. Bring the full set back here so the ruins stop feeling like chaos and start reading like a plan.", "在能够清楚解释最终选项之前，洛希需要你把 3 枚遗迹碎片带回上层终端。把整套碎片带回来，让这片遗迹不再像一团混乱，而开始显露出计划。"));
        }
        return true;
    }

    if (tasks->stage == 3) {
        TasksRuntime_WriteMessage(message, messageSize, Loc_PickLiteral("Loxi online. Next: repair east relay and return.", "洛希已上线。下一步：修复东侧中继，然后返回。"));
        return true;
    }

    if (tasks->stage == 4) {
        TasksRuntime_WriteMessage(message, messageSize, Loc_PickLiteral("Loxi online. Craft the Laser Gun and Protection Suit, then inspect the crash clue.", "洛希已上线。先制作激光枪和防护服，再去调查坠毁线索。"));
        return true;
    }

    if (tasks->stage == 5) {
        if (Player_HasResources(player, RESOURCE_ENERGY_CORE, 1)) {
            TasksRuntime_WriteMessage(message, messageSize, Loc_PickLiteral("Loxi confirms Energy Core acquired. Install it in Power Bay.", "洛希确认你已取得能源核心。请将它安装到动力舱。"));
        } else {
            TasksRuntime_WriteMessage(message, messageSize, Loc_PickLiteral("Loxi online. Prepare your gear and head east for the Energy Core.", "洛希已上线。整理好装备，向东出发寻找能源核心。"));
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
        } else if (selectedRoute == ENDING_HEROIC) {
            if (tasks->bossDefeated) {
                TasksRuntime_WriteMessage(message, messageSize, Loc_PickLiteral("Loxi online. Heroic route confirmed. The guardian is down. Nothing stands between you and the Signal Tower now except the choice to finish what you started.", "洛希已上线。强行救援路线已确认。守卫已经倒下，如今挡在你与信号塔之间的，只剩下把这条路走到底的决心。"));
            } else if (tasks->monolithsLit >= 3) {
                TasksRuntime_WriteMessage(message, messageSize, Loc_PickLiteral("Loxi online. Heroic route confirmed. The airlock is cycling toward an isolated guardian arena, and the completed monolith ring should steady the fight once you enter.", "洛希已上线。强行救援路线已确认。气闸正在切换到隔离守卫战场，而完整石碑环会在你进入后稳住那场战斗。"));
            } else {
                TasksRuntime_WriteMessage(message, messageSize, Loc_PickLiteral("Loxi online. Heroic route confirmed. The airlock now leads into an isolated guardian arena. Any monolith work you already finished will still carry into that fight.", "洛希已上线。强行救援路线已确认。气闸现在会通向一个隔离的守卫战场。你此前完成的任何石碑准备，都会被带进那场战斗。"));
            }
        } else if (selectedRoute == ENDING_PEACEFUL) {
            if (player->hasSignalAmplifier) {
                TasksRuntime_WriteMessage(message, messageSize, Loc_PickLiteral("Loxi online. Peaceful route confirmed. The Signal Amplifier is ready. Carry it to the tower and let the lattice settle instead of breaking it open.", "洛希已上线。和平救援路线已确认。信号放大器已经准备好，把它带去塔楼，让整套格网稳定下来，而不是被强行撕开。"));
            } else if (Player_HasResources(player, RESOURCE_RELIC_FRAGMENT, 3)) {
                TasksRuntime_WriteMessage(message, messageSize, Loc_PickLiteral("Loxi online. Peaceful route confirmed. The fragment set is ready. Craft the Signal Amplifier at the workshop, then carry that answer to the tower.", "洛希已上线。和平救援路线已确认。碎片已经齐备，先去工坊制作信号放大器，再把这份答案带去塔楼。"));
            } else {
                TasksRuntime_WriteMessage(message, messageSize, Loc_PickLiteral("Loxi online. Peaceful route confirmed. Recover 3 Relic Fragments, craft the Signal Amplifier at the workshop, then carry it to the tower before this route hardens into the harsher one.", "洛希已上线。和平救援路线已确认。先找回 3 枚遗迹碎片，在工坊制作信号放大器，再把它带去塔楼，别让这条路最终只剩下更强硬的做法。"));
            }
        } else if (settlementAvailable) {
            TasksRuntime_WriteMessage(message, messageSize, Loc_PickLiteral("Settlement is available. Confirm it only if you truly mean to close both rescue routes and stay to inherit the work here.", "异星定居路线已可选择。只有在你真的决定关闭两条救援路线，并留下来接手这里的一切时，才去确认。"));
        } else if (IsCrossX3Ready(tasks)) {
            TasksRuntime_WriteMessage(message, messageSize, Loc_PickLiteral("Loxi online. West crew testimony and south system truth are already synchronized, but the main archive is not complete yet. Recover the remaining mainline logs before choosing heroic rescue, peaceful rescue, or settlement here.", "洛希已上线。西线船员证词与南方系统真相已经同步，但主线档案仍未完整。请先补齐剩余主线日志，再回来选择强行救援、和平救援或异星定居。"));
        } else if (IsCrossX2Ready(tasks)) {
            TasksRuntime_WriteMessage(message, messageSize, Loc_PickLiteral("Loxi online. The late-game picture has changed: tower, purifier ring, and monolith network all read as one failing maintenance lattice. Finish the remaining mainline logs before the final choice opens.", "洛希已上线。终局图景已经改变：塔楼、净化环和石碑网络都指向同一套失效中的维护格网。请先补齐剩余主线日志，最终选择才会开启。"));
        } else if (IsCrossX1Ready(tasks)) {
            TasksRuntime_WriteMessage(message, messageSize, Loc_PickLiteral("Loxi online. West-side handoffs and south-side maintenance records now align into one survival timeline. Keep clearing archive work so the final choice has full context.", "洛希已上线。西侧交接记录与南侧维护记录如今已经对齐成同一条生存时间线。继续推进档案回收，让最终选择拥有完整背景。"));
        } else {
            TasksRuntime_WriteMessage(message, messageSize, Loc_PickLiteral("Loxi online. The ending is not open yet. Recover the remaining mainline logs and finish the west and south investigations before choosing an ending.", "洛希已上线。结局分支尚未开放。请先找回剩余主线日志，并完成西线与南线调查，再来选择结局。"));
        }
        return true;
    }

    if (settlementAvailable) {
        TasksRuntime_WriteMessage(message, messageSize, Loc_PickLiteral("Settlement is available. Confirm it only if you truly mean to close both rescue routes and stay to inherit the work here.", "异星定居路线已可选择。只有在你真的决定关闭两条救援路线，并留下来接手这里的一切时，才去确认。"));
        return true;
    }

    if (activatedThisUse) {
        TasksRuntime_WriteMessage(message, messageSize, Loc_PickLiteral("Upper cabin terminal synced. The room seal is gone, and N now opens Loxi guidance and recovered logs.", "上层舱室终端已同步，房门限制已经解除。随时按 N 查看洛希指引与档案日志。"));
        return true;
    }

    TasksRuntime_WriteMessage(message, messageSize, Loc_PickLiteral("Loxi uplink stable. Press N to review objectives and recovered logs.", "洛希上行链路稳定。按 N 查看当前任务。"));
    return true;
}

static bool TryRoomInteraction(TaskSystem *tasks, Player *player, char *message, size_t messageSize) {
    const char *roomName;

    roomName = Map_GetRoomNameAt(player->gridX, player->gridY);

    if (std::strcmp(roomName, "Central Corridor") == 0) {
        if (tasks->stage == 3) {
            TasksRuntime_WriteMessage(message, messageSize, Loc_PickLiteral("Central Corridor links terminal, airlock, and life support.", "中央走廊连接终端、气闸与生命维持舱。"));
        } else if (tasks->stage == 5) {
            TasksRuntime_WriteMessage(message, messageSize, Loc_PickLiteral("Central Corridor is the prep route for east runs.", "中央走廊是东线出发前的准备通道。"));
        } else if (tasks->stage >= 7) {
            TasksRuntime_WriteMessage(message, messageSize, Loc_PickLiteral("Central Corridor is now the decision corridor: commit to rescue or confirm settlement here on purpose.", "中央走廊现在成了抉择走廊：你将在这里正式确认救援或定居。"));
        } else {
            TasksRuntime_WriteMessage(message, messageSize, Loc_PickLiteral("Central Corridor links recovery, planning, and crafting rooms.", "中央走廊连接恢复、规划与制作区域。"));
        }
        return true;
    }

    if (std::strcmp(roomName, "Cargo Hold") == 0) {
        TasksRuntime_WriteMessage(message,
                                  messageSize,
                                  Loc_PickLiteral("Cargo Hold: secured supplies and spare parts. This bay is storage, not a recovery station.",
                                                  "货舱：这里整理着补给与备用零件，但它只是储备区，不提供恢复。"));
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
                                  Loc_PickLiteral("Crew Quarters: you lie down long enough to fully recover your health. Stamina and breathing ease here too, but a full oxygen refill still belongs to the proper console.",
                                                  "船员舱：你躺下休息到生命值完全恢复。这里也会缓解体力与呼吸压力，但氧气补满仍要去对应控制台。"));
        return true;
    }

    if (std::strcmp(roomName, "Diagnostics") == 0) {
        if (tasks->stage == 4) {
            TasksRuntime_WriteMessage(message, messageSize, Loc_PickLiteral("Diagnostics: residue forecasts suggest bringing the suit and laser before you inspect the wreck.", "诊断舱：残留物预测建议你带上防护服与激光枪，再去检查残骸。"));
        } else if (tasks->stage == 5) {
            TasksRuntime_WriteMessage(message, messageSize, Loc_PickLiteral("Diagnostics: the deep east route still reads unstable. Use this bay to plan a Deep Gate fallback before you commit.", "诊断舱：深层东线仍然被判定为不稳定区域。正式深入前，先在这里规划一条以深层入口为基准的回撤方案。"));
        } else if (tasks->stage >= 7) {
            TasksRuntime_WriteMessage(message, messageSize, Loc_PickLiteral("Diagnostics: final-route scans are archived. Rest in the cabin or use the oxygen console before the last push.", "诊断舱：终局路线扫描已经归档。最后出发前，请回船员舱休息或去氧气控制台补氧。"));
        } else {
            TasksRuntime_WriteMessage(message, messageSize, Loc_PickLiteral("Diagnostics: ship status is readable here, but recovery now happens elsewhere.", "诊断舱：这里适合查看状态诊断，但恢复功能现在交给了别处。"));
        }
        return true;
    }

    if (std::strcmp(roomName, "Terminal Bay") == 0) {
        if (tasks->stage == 3) {
            TasksRuntime_WriteMessage(message, messageSize, Loc_PickLiteral("Terminal Bay: debrief with Loxi after the first east relay sortie so field results convert into route guidance.", "终端舱：完成第一次东侧中继行动后，请回到这里向洛希汇报，把现场结果转化为路线指引。"));
        } else if (tasks->stage == 5 && Player_HasResources(player, RESOURCE_ENERGY_CORE, 1)) {
            TasksRuntime_WriteMessage(message, messageSize, Loc_PickLiteral("Terminal Bay: Energy Core ready. Install in Power Bay.", "终端舱：能源核心已就位。请安装到动力舱。"));
        } else if (tasks->stage == 6 && Player_HasResources(player, RESOURCE_RELIC_FRAGMENT, 3)) {
            TasksRuntime_WriteMessage(message, messageSize, Loc_PickLiteral("Terminal Bay: full fragments collected. Sync with Loxi.", "终端舱：碎片已全部收齐。与洛希同步吧。"));
        } else if (tasks->stage == 6) {
            TasksRuntime_WriteMessage(message, messageSize, Loc_PickLiteral("Terminal Bay: collect all fragments before sync.", "终端舱：请先收齐全部碎片，再进行同步。"));
        } else if (tasks->stage >= 7) {
            TasksRuntime_WriteMessage(message, messageSize, Loc_PickLiteral("Terminal Bay: confirm your final route choice with Loxi.", "终端舱：在这里和洛希确认你的最终路线选择。"));
        } else {
            TasksRuntime_WriteMessage(message, messageSize, Loc_PickLiteral("Terminal Bay: debrief and mission guidance.", "终端舱：用于汇报与接收任务指引。"));
        }
        return true;
    }

    if (std::strcmp(roomName, "Life Support") == 0) {
        if (tasks->stage <= 2) {
            TasksRuntime_WriteMessage(message, messageSize, Loc_PickLiteral("Life Support is the ship's breathing backbone. Route refill requests through the oxygen console.", "生命维持舱是整艘飞船的呼吸骨架。需要补氧时，请操作氧气控制台。"));
        } else if (tasks->stage == 5) {
            TasksRuntime_WriteMessage(message, messageSize, Loc_PickLiteral("Life Support: the east run will strain your reserves. Refill at the oxygen console before you leave.", "生命维持舱：东线推进会重压你的储备。出发前请先去氧气控制台补满氧气。"));
        } else if (tasks->stage >= 7) {
            TasksRuntime_WriteMessage(message, messageSize, Loc_PickLiteral("Life Support: tower pressure will bite hard. Use the oxygen console before the final climb.", "生命维持舱：塔楼区域会带来更强的压力。最终攀登前，请先去氧气控制台补氧。"));
        } else {
            TasksRuntime_WriteMessage(message, messageSize, Loc_PickLiteral("Life Support is stable. Sleep in the cabin to recover, and use the oxygen console when your tank runs low.", "生命维持舱已经稳定。需要休整就去船员舱睡下，需要补氧就去氧气控制台。"));
        }
        return true;
    }

    if (std::strcmp(roomName, "Workshop") == 0) {
        if (!player->hasLaserGun && tasks->stage >= 4) {
            TasksRuntime_WriteMessage(message, messageSize, Loc_PickLiteral("Workshop: craft Laser Gun and Protection Suit.", "工坊：请制作激光枪和防护服。"));
        } else if (tasks->stage == 5) {
            TasksRuntime_WriteMessage(message, messageSize, Loc_PickLiteral("Workshop: prepare camp supplies, oxygen backup, and combat gear for the eastern push.", "工坊：为东线推进准备营地补给、补氧手段和战斗装备。"));
        } else if (tasks->stage >= 7 && tasks->amplifierUnlocked && !player->hasSignalAmplifier && Player_HasResources(player, RESOURCE_RELIC_FRAGMENT, 3)) {
            TasksRuntime_WriteMessage(message, messageSize, Loc_PickLiteral("Workshop: the mapped fragment set is ready. Craft the Signal Amplifier here for the peaceful route.", "工坊：已映射完成的碎片已经齐备。在这里制作和平路线所需的信号放大器。"));
        } else if (tasks->stage >= 7 && tasks->amplifierUnlocked && !player->hasSignalAmplifier) {
            TasksRuntime_WriteMessage(message, messageSize, Loc_PickLiteral("Workshop: peaceful rescue still needs 3 Relic Fragments before the Signal Amplifier can be assembled here.", "工坊：和平救援还需要先找回 3 枚遗迹碎片，才能在这里组装信号放大器。"));
        } else if (player->hasLaserGun) {
            TasksRuntime_WriteMessage(message, messageSize, Loc_PickLiteral("The workshop racks hold spare cells and training notes for field combat.", "工坊架子上还留着备用电池和野外战斗训练笔记。"));
        } else {
            TasksRuntime_WriteMessage(message, messageSize, Loc_PickLiteral("The workshop is still locked down. Loxi suggests restoring more ship systems first.", "工坊仍处于锁定状态。洛希建议先恢复更多飞船系统。"));
        }
        return true;
    }

    if (std::strcmp(roomName, "Power Bay") == 0) {
        if (tasks->stage == 5 && Player_HasResources(player, RESOURCE_ENERGY_CORE, 1)) {
            TasksRuntime_WriteMessage(message, messageSize, Loc_PickLiteral("Power Bay: install the Energy Core now. This handoff opens the north route and turns eastern proof into mainline progress.", "动力舱：现在就安装能源核心。这次交接将开启北线路线，并把东线证据转化为主线推进。"));
        } else if (tasks->stage == 5) {
            TasksRuntime_WriteMessage(message, messageSize, Loc_PickLiteral("Power Bay ready. Energy Core still missing.", "动力舱已准备就绪，但能源核心仍未找到。"));
        } else if (tasks->stage >= 6) {
            TasksRuntime_WriteMessage(message, messageSize, Loc_PickLiteral("Power Bay restored. North ruins route is open.", "动力舱已恢复，北方遗迹路线已经开放。"));
        } else {
            TasksRuntime_WriteMessage(message, messageSize, Loc_PickLiteral("Power Bay is still one of the ship's dormant systems. Later progression will have to wake it cleanly.", "动力舱仍是飞船中沉睡的系统之一，后续推进必须把它彻底唤醒。"));
        }
        return true;
    }

    return false;
}

static bool TryUseAirlockConsole(TaskSystem *tasks, GameMap *map, Player *player, char *message, size_t messageSize) {
    if (tasks->stage < 3) {
        TasksRuntime_WriteMessage(message, messageSize, Loc_PickLiteral("The airlock is still sealed. Restore the oxygen system first.", "气闸仍然处于封闭状态。请先恢复氧气系统。"));
        return true;
    }

    if (tasks->selectedEndingRoute == ENDING_HEROIC && !tasks->bossDefeated) {
        Map_LockSwampOuter(map);
        player->gridX = BOSS_ARENA_PLAYER_ENTRY_X;
        player->gridY = BOSS_ARENA_PLAYER_ENTRY_Y;
        player->facingX = 1;
        player->facingY = 0;
        Player_UpdateWorldPosition(player);
        Tasks_UpdateObjective(tasks, player);
        TasksRuntime_WriteMessage(message,
                                  messageSize,
                                  Loc_PickLiteral("Airlock cycling into isolated breach mode. The outer door will not reopen into the world route yet; it will throw you straight into the guardian arena. Cross that threshold only when you are ready to win there and come back for the tower.",
                                                  "气闸正在切换到隔离突破模式。外门暂时不会重新连回常规地图，而是会直接把你送进守卫战场。赢下那边的战斗，再回来完成塔楼。"));
        return true;
    }

    if (!Map_IsSwampOuterUnlocked(map)) {
        Map_UnlockSwampOuter(map);
        TasksRuntime_WriteMessage(message, messageSize, Loc_PickLiteral("Airlock cycling complete. The outer blast door is now open.", "气闸循环完成，外层防爆门现已开启。"));
        return true;
    }

    Map_LockSwampOuter(map);
    TasksRuntime_WriteMessage(message, messageSize, Loc_PickLiteral("Airlock cycle reversed. The outer blast door is now closed.", "气闸循环已反转，外层防爆门现已关闭。"));
    return true;
}

static bool TryUseEnergyConsole(TaskSystem *tasks, GameMap *map, Player *player, char *message, size_t messageSize) {
    if (tasks->stage == 5) {
        if (Player_HasResources(player, RESOURCE_ENERGY_CORE, 1)
            && Player_HasResources(player, RESOURCE_ORE, 2)
            && Player_HasResources(player, RESOURCE_ENERGY_CRYSTAL, 1)) {
            Player_SpendResource(player, RESOURCE_ENERGY_CORE, 1);
            Player_SpendResource(player, RESOURCE_ORE, 2);
            Player_SpendResource(player, RESOURCE_ENERGY_CRYSTAL, 1);
            tasks->energyRepairLevel = 1;
            TasksRuntime_UnlockStageIfNeeded(tasks, map, 6);
            TasksRuntime_WriteMessage(message, messageSize, Loc_PickLiteral("The power module is fully restored. Your eastern preparation has paid off, the ruins entrance is now open, and Loxi can finally read the north route as a real endgame path instead of raw hazard.", "动力模块已完全恢复。你在东线的准备得到了回报，遗迹入口现已开启，洛希也终于能把北线路线读作真正的终局路径，而不只是原始危险区。"));
            return true;
        }

        TasksRuntime_WriteMessage(message, messageSize, Loc_PickLiteral("Power repair still needs 1 Energy Core, 2 Ore, and 1 Energy Crystal.", "动力修复仍需要 1 个能源核心、2 份矿石和 1 份能量晶体。"));
        return true;
    }

    TasksRuntime_WriteMessage(message, messageSize, Loc_PickLiteral("The power console is operating normally.", "动力控制台运转正常。"));
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
            TasksRuntime_WriteMessage(message, messageSize, Loc_PickLiteral("This is the workbench. Press F to open crafting.", "这里是工作台。按 F 打开制作界面。"));
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
