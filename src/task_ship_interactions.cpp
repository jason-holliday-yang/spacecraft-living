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
        return Loc_PickLiteral("Loxi online. West crew testimony and the buried system record now point to the same truth. Heroic rescue means forcing a costly beacon launch, peaceful rescue means stabilizing the lattice, and settlement means inheriting the work. Choose here before touching the tower. Settlement still requires explicit confirmation because it closes both rescue routes.",
                               "洛西已上线。西侧船员证词与地下系统记录如今指向同一个真相。强行救援意味着强行发射代价高昂的信标，和平救援意味着稳定整套格网，而定居则意味着接手这份工作。请在碰触塔楼之前先在这里作出选择。异星定居仍需额外确认，因为它会关闭两条救援路线。");
    }

    return Loc_PickLiteral("Loxi online. The main archive is assembled, the west and south investigations are complete, and this terminal is now the point of no return. Choose heroic rescue, peaceful rescue, or settlement here before touching the tower. Settlement still requires explicit confirmation because it closes both rescue routes.",
                           "洛西已上线。主线档案已经拼合完成，西线与南线调查也都结束，这个终端如今就是无法回头的分叉点。请在碰触塔楼之前，于这里选择强行救援、和平救援或异星定居。异星定居仍需额外确认，因为它会关闭两条救援路线。");
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
        player->health = Player_GetMaxHealth(player);
        player->oxygen = MAX_OXYGEN;
        player->stamina = Player_GetCurrentStaminaCap(player);
        Player_ClearPoison(player);
        TasksRuntime_ClearNegativeSurvivalStatuses(player);
        TasksRuntime_WriteMessage(message, messageSize, Loc_PickLiteral("Base oxygen console reset complete: health and oxygen fully restored, anomalies cleared, and your gear stabilized.", "基地氧气控制台重置完成：生命与氧气已全部恢复，异常状态已清除，装备状态也已稳定。"));
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
        activatedThisUse = true;
    }

    if (tasks->stage == 6 && Player_HasResources(player, RESOURCE_RELIC_FRAGMENT, 3)) {
        Player_SpendResource(player, RESOURCE_RELIC_FRAGMENT, 3);
        tasks->amplifierUnlocked = true;
        TasksRuntime_UnlockStageIfNeeded(tasks, map, 7);
        if (IsCrossX2Ready(tasks)) {
            TasksRuntime_WriteMessage(message, messageSize, Loc_PickLiteral("Loxi finished the endgame analysis: west crew traces, purifier controls, and monolith routing all resolve into one failing maintenance lattice. Heroic rescue forces a beacon through, peaceful rescue steadies the lattice, and settlement inherits the burden. Finish the main archive, then choose with the full picture.", "洛西完成了终局分析：西线船员痕迹、净化控制与石碑路径最终都指向同一套正在失效的维护格网。强行救援是强行点亮信标，和平救援是稳定格网，而定居则意味着接手这份负担。请先补完整个主线档案，再带着完整理解作出选择。"));
        } else if (IsCrossX1Ready(tasks)) {
            TasksRuntime_WriteMessage(message, messageSize, Loc_PickLiteral("Loxi finished the fragment sync: west crew traces and south facility handovers now read as one shared survival timeline. Heroic rescue, peaceful rescue, and settlement are all visible now, but the main archive still has to be finished before the final choice opens.", "洛西完成了碎片同步：西线船员痕迹与南方设施交接记录如今能够拼成一条共同的生存时间线。强行救援、和平救援和异星定居都已经浮现，但在最终选择开放前，你仍需补完主线档案。"));
        } else {
            TasksRuntime_WriteMessage(message, messageSize, Loc_PickLiteral("Loxi finished the endgame analysis: the ruins now resolve into three real outcomes. Craft the Signal Amplifier for peaceful rescue, face the guardian for heroic rescue, or keep the base and choose settlement deliberately.", "洛西完成了终局分析：遗迹如今清晰地分成三种真实结局。制作信号放大器可走和平救援，直面守卫可走强行救援，或者保留基地，主动选择异星定居。"));
        }
        return true;
    }

    if (tasks->stage == 6) {
        if (IsCrossX2Ready(tasks)) {
            TasksRuntime_WriteMessage(message, messageSize, Loc_PickLiteral("Loxi already has enough west and south evidence to redraw the late-game picture, but it still needs 3 Relic Fragments before the final rescue-versus-settlement choice can be explained cleanly.", "洛西已经拥有足够的西线与南线证据来重构终局图景，但它仍需要 3 枚遗迹碎片，才能把最终的救援与定居分歧讲清楚。"));
        } else if (IsCrossX1Ready(tasks)) {
            TasksRuntime_WriteMessage(message, messageSize, Loc_PickLiteral("Loxi can already align the west crew trail with the south facility record, but it still needs 3 Relic Fragments before the final choice becomes readable.", "洛西已经能把西线船员轨迹与南方设施记录对齐，但它仍需要 3 枚遗迹碎片，最终选择才能真正被读懂。"));
        } else {
            TasksRuntime_WriteMessage(message, messageSize, Loc_PickLiteral("Loxi needs 3 Relic Fragments at the upper terminal before the final options can be explained clearly. Bring the full set back here so the ruins stop feeling like chaos and start reading like a plan.", "在能够清楚解释最终选项之前，洛西需要你把 3 枚遗迹碎片带回上层终端。把整套碎片带回来，让这片遗迹不再像一团混乱，而开始显露出计划。"));
        }
        return true;
    }

    if (tasks->stage == 3) {
        TasksRuntime_WriteMessage(message, messageSize, Loc_PickLiteral("Loxi online. Next: repair east relay and return.", "洛西已上线。下一步：修复东侧中继，然后返回。"));
        return true;
    }

    if (tasks->stage == 4) {
        TasksRuntime_WriteMessage(message, messageSize, Loc_PickLiteral("Loxi online. Craft the Laser Gun and Protection Suit, then inspect the crash clue.", "洛西已上线。先制作激光枪和防护服，再去调查坠毁线索。"));
        return true;
    }

    if (tasks->stage == 5) {
        if (Player_HasResources(player, RESOURCE_ENERGY_CORE, 1)) {
            TasksRuntime_WriteMessage(message, messageSize, Loc_PickLiteral("Loxi confirms Energy Core acquired. Install it in Power Bay.", "洛西确认你已取得能源核心。请将它安装到动力舱。"));
        } else {
            TasksRuntime_WriteMessage(message, messageSize, Loc_PickLiteral("Loxi online. Prepare your gear and head east for the Energy Core.", "洛西已上线。整理好装备，向东出发寻找能源核心。"));
        }
        return true;
    }

    if (tasks->stage == 7) {
        if (Tasks_IsEndingBranchReady(tasks) && selectedRoute == ENDING_NONE) {
            TasksRuntime_WriteMessage(message, messageSize, GetStage7RouteChoiceMessage(tasks));
        } else if (selectedRoute == ENDING_HEROIC) {
            if (tasks->bossDefeated) {
                TasksRuntime_WriteMessage(message, messageSize, Loc_PickLiteral("Loxi online. Heroic route confirmed. The guardian is down, so your next step is the Signal Tower.", "洛西已上线。强行救援路线已确认。守卫已经倒下，你的下一步就是前往信号塔。"));
            } else if (tasks->monolithsLit >= 3) {
                TasksRuntime_WriteMessage(message, messageSize, Loc_PickLiteral("Loxi online. Heroic route confirmed. The monolith ring is complete, so finish the guardian and then use the Signal Tower.", "洛西已上线。强行救援路线已确认。石碑环已完整点亮，先解决守卫，再启用信号塔。"));
            } else {
                TasksRuntime_WriteMessage(message, messageSize, Loc_PickLiteral("Loxi online. Heroic route confirmed. Finish the monolith prep if you want a safer climb, defeat the guardian, then return to the tower.", "洛西已上线。强行救援路线已确认。如果你想让攀登更安全，就先完成石碑准备，击败守卫后再回到塔楼。"));
            }
        } else if (selectedRoute == ENDING_PEACEFUL) {
            if (player->hasSignalAmplifier) {
                TasksRuntime_WriteMessage(message, messageSize, Loc_PickLiteral("Loxi online. Peaceful route confirmed. The Signal Amplifier is ready, so take it to the tower and stabilize the lattice.", "洛西已上线。和平救援路线已确认。信号放大器已经准备好，把它带到塔楼去稳定格网。"));
            } else {
                TasksRuntime_WriteMessage(message, messageSize, Loc_PickLiteral("Loxi online. Peaceful route confirmed. Craft the Signal Amplifier, then carry it to the tower.", "洛西已上线。和平救援路线已确认。先制作信号放大器，再把它带去塔楼。"));
            }
        } else if (settlementAvailable) {
            TasksRuntime_WriteMessage(message, messageSize, Loc_PickLiteral("Settlement is available. Confirm only if you truly want to close both rescue routes.", "异星定居路线已可选择。只有在你确实想关闭两条救援路线时，才进行确认。"));
        } else if (IsCrossX3Ready(tasks)) {
            TasksRuntime_WriteMessage(message, messageSize, Loc_PickLiteral("Loxi online. West crew testimony and south system truth are already synchronized, but the main archive is not complete yet. Recover the remaining mainline logs before choosing heroic rescue, peaceful rescue, or settlement here.", "洛西已上线。西线船员证词与南方系统真相已经同步，但主线档案仍未完整。请先补齐剩余主线日志，再回来选择强行救援、和平救援或异星定居。"));
        } else if (IsCrossX2Ready(tasks)) {
            TasksRuntime_WriteMessage(message, messageSize, Loc_PickLiteral("Loxi online. The late-game picture has changed: tower, purifier ring, and monolith network all read as one failing maintenance lattice. Finish the remaining mainline logs before the final choice opens.", "洛西已上线。终局图景已经改变：塔楼、净化环和石碑网络都指向同一套失效中的维护格网。请先补齐剩余主线日志，最终选择才会开启。"));
        } else if (IsCrossX1Ready(tasks)) {
            TasksRuntime_WriteMessage(message, messageSize, Loc_PickLiteral("Loxi online. West-side handoffs and south-side maintenance records now align into one survival timeline. Keep clearing archive work so the final choice has full context.", "洛西已上线。西侧交接记录与南侧维护记录如今已经对齐成同一条生存时间线。继续推进档案回收，让最终选择拥有完整背景。"));
        } else {
            TasksRuntime_WriteMessage(message, messageSize, Loc_PickLiteral("Loxi online. The ending is not open yet. Recover the remaining mainline logs and finish the west and south investigations before choosing an ending.", "洛西已上线。结局分支尚未开放。请先找回剩余主线日志，并完成西线与南线调查，再来选择结局。"));
        }
        return true;
    }

    if (settlementAvailable) {
        TasksRuntime_WriteMessage(message, messageSize, Loc_PickLiteral("Settlement is available. Confirm only if you truly want to close both rescue routes.", "异星定居路线已可选择。只有在你确实想关闭两条救援路线时，才进行确认。"));
        return true;
    }

    if (activatedThisUse) {
        TasksRuntime_WriteMessage(message, messageSize, Loc_PickLiteral("Upper cabin terminal synced. Press N any time to review Loxi guidance.", "上层舱室终端已同步。随时按 N 查看洛西指引。"));
        return true;
    }

    TasksRuntime_WriteMessage(message, messageSize, Loc_PickLiteral("Loxi uplink is stable. Press N to review the current mission.", "洛西上行链路稳定。按 N 查看当前任务。"));
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
        Player_RecoverHealth(player, 8.0f);
        Player_AddOxygen(player, 8.0f);
        Player_RecoverStamina(player, 20.0f);
        Player_RelievePressure(player, 12.0f);
        TasksRuntime_WriteMessage(message, messageSize, Loc_PickLiteral("Cargo Hold: recovered a small amount of oxygen and condition.", "货舱：恢复了少量氧气与状态。"));
        return true;
    }

    if (std::strcmp(roomName, "Crew Quarters") == 0) {
        Player_RecoverHealth(player, 14.0f);
        Player_RecoverStamina(player, 22.0f);
        Player_RelievePressure(player, 8.0f);
        TasksRuntime_WriteMessage(message, messageSize, Loc_PickLiteral("Crew Quarters: recovered health and condition.", "船员舱：恢复了生命与状态。"));
        return true;
    }

    if (std::strcmp(roomName, "Diagnostics") == 0) {
        Player_ClearPoison(player);
        TasksRuntime_DowngradeOxygenLeakAtRecovery(player);
        Player_RecoverHealth(player, 12.0f);
        Player_AddOxygen(player, 10.0f);
        Player_RelievePressure(player, 18.0f);
        Player_SetStatus(player, PLAYER_STATUS_FILTERED, 1, 36.0f, 0.24f);
        if (tasks->stage == 4) {
            TasksRuntime_WriteMessage(message, messageSize, Loc_PickLiteral("Diagnostics: toxins cleared, filtered breathing active. Good for crash clue prep.", "诊断舱：毒素已清除，过滤呼吸生效。适合为坠毁线索调查做准备。"));
        } else if (tasks->stage == 5) {
            TasksRuntime_WriteMessage(message, messageSize, Loc_PickLiteral("Diagnostics: toxins cleared, filtered breathing active. Use this bay to plan Deep Gate fallback checkpoints before committing to the deep swamp.", "诊断舱：毒素已清除，过滤呼吸生效。深入深层沼泽前，可在这里规划深层入口的回撤检查点。"));
        } else if (tasks->stage >= 7) {
            TasksRuntime_WriteMessage(message, messageSize, Loc_PickLiteral("Diagnostics: stabilize before final north push.", "诊断舱：在最后一次北上推进前先把状态稳定下来。"));
        } else {
            TasksRuntime_WriteMessage(message, messageSize, Loc_PickLiteral("Diagnostics: toxins cleared and condition stabilized.", "诊断舱：毒素已清除，整体状态已稳定。"));
        }
        return true;
    }

    if (std::strcmp(roomName, "Terminal Bay") == 0) {
        if (tasks->stage == 3) {
            TasksRuntime_WriteMessage(message, messageSize, Loc_PickLiteral("Terminal Bay: debrief with Loxi after the first east relay sortie so field results convert into route guidance.", "终端舱：完成第一次东侧中继行动后，请回到这里向洛西汇报，把现场结果转化为路线指引。"));
        } else if (tasks->stage == 5 && Player_HasResources(player, RESOURCE_ENERGY_CORE, 1)) {
            TasksRuntime_WriteMessage(message, messageSize, Loc_PickLiteral("Terminal Bay: Energy Core ready. Install in Power Bay.", "终端舱：能源核心已就位。请安装到动力舱。"));
        } else if (tasks->stage == 6 && Player_HasResources(player, RESOURCE_RELIC_FRAGMENT, 3)) {
            TasksRuntime_WriteMessage(message, messageSize, Loc_PickLiteral("Terminal Bay: full fragments collected. Sync with Loxi.", "终端舱：碎片已全部收齐。与洛西同步吧。"));
        } else if (tasks->stage == 6) {
            TasksRuntime_WriteMessage(message, messageSize, Loc_PickLiteral("Terminal Bay: collect all fragments before sync.", "终端舱：请先收齐全部碎片，再进行同步。"));
        } else if (tasks->stage >= 7) {
            TasksRuntime_WriteMessage(message, messageSize, Loc_PickLiteral("Terminal Bay: confirm your final route choice with Loxi.", "终端舱：在这里和洛西确认你的最终路线选择。"));
        } else {
            TasksRuntime_WriteMessage(message, messageSize, Loc_PickLiteral("Terminal Bay: debrief and mission guidance.", "终端舱：用于汇报与接收任务指引。"));
        }
        return true;
    }

    if (std::strcmp(roomName, "Life Support") == 0) {
        if (tasks->stage <= 2) {
            TasksRuntime_WriteMessage(message, messageSize, Loc_PickLiteral("Life Support is priority until oxygen is stable.", "在氧气稳定之前，生命维持舱始终是第一优先级。"));
        } else if (tasks->stage == 5) {
            TasksRuntime_WriteMessage(message, messageSize, Loc_PickLiteral("Life Support: refill before deep east attempts.", "生命维持舱：深入东侧前请先补满状态。"));
        } else if (tasks->stage >= 7) {
            TasksRuntime_WriteMessage(message, messageSize, Loc_PickLiteral("Life Support: top off before final tower push.", "生命维持舱：在最后冲塔之前先把状态补满。"));
        } else {
            TasksRuntime_WriteMessage(message, messageSize, Loc_PickLiteral("Life Support is stable. Use it as your reset point.", "生命维持舱已经稳定，可把这里当作重整状态的据点。"));
        }
        return true;
    }

    if (std::strcmp(roomName, "Workshop") == 0) {
        if (!player->hasLaserGun && tasks->stage >= 4) {
            TasksRuntime_WriteMessage(message, messageSize, Loc_PickLiteral("Workshop: craft Laser Gun and Protection Suit.", "工坊：请制作激光枪和防护服。"));
        } else if (tasks->stage == 5) {
            TasksRuntime_WriteMessage(message, messageSize, Loc_PickLiteral("Workshop: prepare Rope, Camp, and combat gear for the eastern push.", "工坊：为东线推进准备绳索、营地和战斗装备。"));
        } else if (tasks->stage >= 7 && tasks->amplifierUnlocked && !player->hasSignalAmplifier) {
            TasksRuntime_WriteMessage(message, messageSize, Loc_PickLiteral("Workshop: craft Signal Amplifier for peaceful rescue.", "工坊：为和平救援制作信号放大器。"));
        } else if (player->hasLaserGun) {
            TasksRuntime_WriteMessage(message, messageSize, Loc_PickLiteral("The workshop racks hold spare cells and training notes for field combat.", "工坊架子上还留着备用电池和野外战斗训练笔记。"));
        } else {
            TasksRuntime_WriteMessage(message, messageSize, Loc_PickLiteral("The workshop is still locked down. Loxi suggests restoring more ship systems first.", "工坊仍处于锁定状态。洛西建议先恢复更多飞船系统。"));
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

static bool TryUseAirlockConsole(TaskSystem *tasks, GameMap *map, char *message, size_t messageSize) {
    if (tasks->stage < 3) {
        TasksRuntime_WriteMessage(message, messageSize, Loc_PickLiteral("The airlock is still sealed. Restore the oxygen system first.", "气闸仍然处于封闭状态。请先恢复氧气系统。"));
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
            TasksRuntime_WriteMessage(message, messageSize, Loc_PickLiteral("The power module is fully restored. Your eastern preparation has paid off, the ruins entrance is now open, and Loxi can finally read the north route as a real endgame path instead of raw hazard.", "动力模块已完全恢复。你在东线的准备得到了回报，遗迹入口现已开启，洛西也终于能把北线路线读作真正的终局路径，而不只是原始危险区。"));
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
            return TryUseAirlockConsole(tasks, map, message, messageSize);
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
