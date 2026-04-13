#include "task_runtime_internal.h"

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

static bool TryRepairAtOxygenConsole(TaskSystem *tasks, GameMap *map, Player *player, char *message, size_t messageSize) {
    if (tasks->stage == 1) {
        if (Player_HasResources(player, RESOURCE_WOOD, 3) && Player_HasResources(player, RESOURCE_METAL_SCRAP, 2)) {
            Player_SpendResource(player, RESOURCE_WOOD, 3);
            Player_SpendResource(player, RESOURCE_METAL_SCRAP, 2);
            tasks->oxygenRepairLevel = 1;
            TasksRuntime_UnlockStageIfNeeded(tasks, map, 2);
            Player_AddOxygen(player, 18.0f);
            TasksRuntime_WriteMessage(message, messageSize, "First oxygen module repaired in the lower deck. Craft light gear before going deeper.");
            return true;
        }

        TasksRuntime_WriteMessage(message, messageSize, "Missing materials for the first oxygen repair: 3 Wood and 2 Metal Scrap.");
        return true;
    }

    if (tasks->stage == 2) {
        if (player->hasGlowStick && Player_HasResources(player, RESOURCE_GLOW_MOSS, 1) && Player_HasResources(player, RESOURCE_ORE, 1)) {
            Player_SpendResource(player, RESOURCE_GLOW_MOSS, 1);
            Player_SpendResource(player, RESOURCE_ORE, 1);
            tasks->oxygenRepairLevel = 2;
            TasksRuntime_UnlockStageIfNeeded(tasks, map, 3);
            Player_AddOxygen(player, 35.0f);
            TasksRuntime_WriteMessage(message, messageSize, "Lower-deck oxygen system fully repaired. The outer swamp is now open.");
            return true;
        }

        TasksRuntime_WriteMessage(message, messageSize, "Final oxygen repair requires a Glow Stick, 1 Glow Moss, and 1 Ore.");
        return true;
    }

    if (Map_GetAreaAt(player->gridX, player->gridY) == MAP_AREA_BASE) {
        player->pressure = INITIAL_PRESSURE;
        player->health = Player_GetMaxHealth(player);
        player->oxygen = MAX_OXYGEN;
        player->stamina = Player_GetCurrentStaminaCap(player);
        Player_ClearPoison(player);
        TasksRuntime_ClearNegativeSurvivalStatuses(player);
        TasksRuntime_WriteMessage(message, messageSize, "Base oxygen console reset complete: health and oxygen fully restored, anomalies cleared, and your gear stabilized.");
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
        TasksRuntime_WriteMessage(message, messageSize, "Loxi finished the endgame analysis: the ruins now resolve into three real outcomes. Craft the Signal Amplifier for peaceful rescue, confront the guardian for heroic rescue, or keep the base as the place where settlement must be chosen on purpose.");
        return true;
    }

    if (tasks->stage == 6) {
        TasksRuntime_WriteMessage(message, messageSize, "Loxi needs 3 Relic Fragments at the upper terminal before the final routes can be explained cleanly. Bring the full set back here to turn the ruins into a readable endgame plan.");
        return true;
    }

    if (tasks->stage == 3) {
        TasksRuntime_WriteMessage(message, messageSize, "Loxi online. Next: repair east relay and return.");
        return true;
    }

    if (tasks->stage == 4) {
        TasksRuntime_WriteMessage(message, messageSize, "Loxi online. Craft Laser Gun + Protection Suit, then inspect crash clue.");
        return true;
    }

    if (tasks->stage == 5) {
        if (Player_HasResources(player, RESOURCE_ENERGY_CORE, 1)) {
            TasksRuntime_WriteMessage(message, messageSize, "Loxi confirms Energy Core acquired. Install it in Power Bay.");
        } else {
            TasksRuntime_WriteMessage(message, messageSize, "Loxi online. Prepare gear and head east for Energy Core.");
        }
        return true;
    }

    if (tasks->stage == 7) {
        if (Tasks_IsEndingBranchReady(tasks) && selectedRoute == ENDING_NONE) {
            TasksRuntime_WriteMessage(message, messageSize, "Loxi online. The main archive is assembled, west and south route work is complete, and this terminal is now the ending branch point. Choose heroic rescue, peaceful rescue, or settlement here on purpose before touching the tower. Settlement still requires explicit confirmation because it will close both rescue routes.");
        } else if (selectedRoute == ENDING_HEROIC) {
            if (tasks->bossDefeated) {
                TasksRuntime_WriteMessage(message, messageSize, "Loxi online. Heroic route locked. The guardian is down, so your next step is the Signal Tower.");
            } else if (tasks->monolithsLit >= 3) {
                TasksRuntime_WriteMessage(message, messageSize, "Loxi online. Heroic route locked. The monolith ring is complete, so finish the guardian and then use the Signal Tower.");
            } else {
                TasksRuntime_WriteMessage(message, messageSize, "Loxi online. Heroic route locked. Finish monolith prep if you want a safer climb, defeat the guardian, then return to the tower.");
            }
        } else if (selectedRoute == ENDING_PEACEFUL) {
            if (player->hasSignalAmplifier) {
                TasksRuntime_WriteMessage(message, messageSize, "Loxi online. Peaceful route locked. The Signal Amplifier is ready, so take it to the tower and stabilize the lattice.");
            } else {
                TasksRuntime_WriteMessage(message, messageSize, "Loxi online. Peaceful route locked. Craft the Signal Amplifier, then carry it to the tower.");
            }
        } else if (settlementAvailable) {
            TasksRuntime_WriteMessage(message, messageSize, "Settlement is available. Confirm only if you want to close both rescue routes.");
        } else if (IsCrossX3Ready(tasks)) {
            TasksRuntime_WriteMessage(message, messageSize, "Loxi online. West and south conclusions are synchronized, but the main archive is not complete yet. Recover the remaining mainline logs before choosing the ending route here.");
        } else if (IsCrossX2Ready(tasks)) {
            TasksRuntime_WriteMessage(message, messageSize, "Loxi online. The late-route analysis is stable. Finish the remaining archive work and bring back the missing mainline logs before the final branch opens.");
        } else if (IsCrossX1Ready(tasks)) {
            TasksRuntime_WriteMessage(message, messageSize, "Loxi online. Shared route alignment has started. Keep clearing west and south tasks and recover the missing mainline logs.");
        } else {
            TasksRuntime_WriteMessage(message, messageSize, "Loxi online. Final act is not open yet. Recover the remaining mainline logs and finish the west/south field work before choosing an ending.");
        }
        return true;
    }

    if (settlementAvailable) {
        TasksRuntime_WriteMessage(message, messageSize, "Settlement is available. Confirm only if you want to close both rescue routes.");
        return true;
    }

    if (activatedThisUse) {
        TasksRuntime_WriteMessage(message, messageSize, "Upper cabin terminal synced. Press N any time to review Loxi guidance.");
        return true;
    }

    TasksRuntime_WriteMessage(message, messageSize, "Loxi uplink is stable. Press N to review the current mission.");
    return true;
}

static bool TryRoomInteraction(TaskSystem *tasks, Player *player, char *message, size_t messageSize) {
    const char *roomName;

    roomName = Map_GetRoomNameAt(player->gridX, player->gridY);

    if (std::strcmp(roomName, "Central Corridor") == 0) {
        if (tasks->stage == 3) {
            TasksRuntime_WriteMessage(message, messageSize, "Central Corridor links terminal, airlock, and life support.");
        } else if (tasks->stage == 5) {
            TasksRuntime_WriteMessage(message, messageSize, "Central Corridor is the prep route for east runs.");
        } else if (tasks->stage >= 7) {
            TasksRuntime_WriteMessage(message, messageSize, "Central Corridor is now the route-choice spine: commit to rescue or confirm settlement here on purpose.");
        } else {
            TasksRuntime_WriteMessage(message, messageSize, "Central Corridor links recovery, planning, and crafting rooms.");
        }
        return true;
    }

    if (std::strcmp(roomName, "Cargo Hold") == 0) {
        Player_RecoverHealth(player, 8.0f);
        Player_AddOxygen(player, 8.0f);
        Player_RecoverStamina(player, 20.0f);
        Player_RelievePressure(player, 12.0f);
        TasksRuntime_WriteMessage(message, messageSize, "Cargo Hold: recovered a small amount of oxygen and condition.");
        return true;
    }

    if (std::strcmp(roomName, "Crew Quarters") == 0) {
        Player_RecoverHealth(player, 14.0f);
        Player_RecoverStamina(player, 22.0f);
        Player_RelievePressure(player, 8.0f);
        TasksRuntime_WriteMessage(message, messageSize, "Crew Quarters: recovered health and condition.");
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
            TasksRuntime_WriteMessage(message, messageSize, "Diagnostics: toxins cleared, filtered breathing active. Good for crash clue prep.");
        } else if (tasks->stage == 5) {
            TasksRuntime_WriteMessage(message, messageSize, "Diagnostics: toxins cleared, filtered breathing active. Use this bay to plan Deep Gate turnback checkpoints before deep-swamp commitment.");
        } else if (tasks->stage >= 7) {
            TasksRuntime_WriteMessage(message, messageSize, "Diagnostics: stabilize before final north push.");
        } else {
            TasksRuntime_WriteMessage(message, messageSize, "Diagnostics: toxins cleared and condition stabilized.");
        }
        return true;
    }

    if (std::strcmp(roomName, "Terminal Bay") == 0) {
        if (tasks->stage == 3) {
            TasksRuntime_WriteMessage(message, messageSize, "Terminal Bay: debrief with Loxi after the first east relay sortie so field results convert into route guidance.");
        } else if (tasks->stage == 5 && Player_HasResources(player, RESOURCE_ENERGY_CORE, 1)) {
            TasksRuntime_WriteMessage(message, messageSize, "Terminal Bay: Energy Core ready. Install in Power Bay.");
        } else if (tasks->stage == 6 && Player_HasResources(player, RESOURCE_RELIC_FRAGMENT, 3)) {
            TasksRuntime_WriteMessage(message, messageSize, "Terminal Bay: full fragments collected. Sync with Loxi.");
        } else if (tasks->stage == 6) {
            TasksRuntime_WriteMessage(message, messageSize, "Terminal Bay: collect all fragments before sync.");
        } else if (tasks->stage >= 7) {
            TasksRuntime_WriteMessage(message, messageSize, "Terminal Bay: confirm your final route choice with Loxi.");
        } else {
            TasksRuntime_WriteMessage(message, messageSize, "Terminal Bay: debrief and mission guidance.");
        }
        return true;
    }

    if (std::strcmp(roomName, "Life Support") == 0) {
        if (tasks->stage <= 2) {
            TasksRuntime_WriteMessage(message, messageSize, "Life Support is priority until oxygen is stable.");
        } else if (tasks->stage == 5) {
            TasksRuntime_WriteMessage(message, messageSize, "Life Support: refill before deep east attempts.");
        } else if (tasks->stage >= 7) {
            TasksRuntime_WriteMessage(message, messageSize, "Life Support: top off before final tower push.");
        } else {
            TasksRuntime_WriteMessage(message, messageSize, "Life Support is stable. Use it as your reset point.");
        }
        return true;
    }

    if (std::strcmp(roomName, "Workshop") == 0) {
        if (!player->hasLaserGun && tasks->stage >= 4) {
            TasksRuntime_WriteMessage(message, messageSize, "Workshop: craft Laser Gun and Protection Suit.");
        } else if (tasks->stage == 5) {
            TasksRuntime_WriteMessage(message, messageSize, "Workshop: prepare Rope, Camp, and combat gear for east run.");
        } else if (tasks->stage >= 7 && tasks->amplifierUnlocked && !player->hasSignalAmplifier) {
            TasksRuntime_WriteMessage(message, messageSize, "Workshop: craft Signal Amplifier for peaceful rescue.");
        } else if (player->hasLaserGun) {
            TasksRuntime_WriteMessage(message, messageSize, "The workshop racks hold spare cells and training notes for field combat.");
        } else {
            TasksRuntime_WriteMessage(message, messageSize, "The workshop is still locked down. Loxi suggests restoring more ship systems first.");
        }
        return true;
    }

    if (std::strcmp(roomName, "Power Bay") == 0) {
        if (tasks->stage == 5 && Player_HasResources(player, RESOURCE_ENERGY_CORE, 1)) {
            TasksRuntime_WriteMessage(message, messageSize, "Power Bay: install Energy Core now. This handoff is the north-route unlock that converts east proof into mainline progression.");
        } else if (tasks->stage == 5) {
            TasksRuntime_WriteMessage(message, messageSize, "Power Bay ready. Energy Core still missing.");
        } else if (tasks->stage >= 6) {
            TasksRuntime_WriteMessage(message, messageSize, "Power Bay restored. North ruins route is open.");
        } else {
            TasksRuntime_WriteMessage(message, messageSize, "Power Bay is still one of the ship's dormant systems. Later progression will have to wake it cleanly.");
        }
        return true;
    }

    return false;
}

static bool TryUseAirlockConsole(TaskSystem *tasks, GameMap *map, char *message, size_t messageSize) {
    if (tasks->stage < 3) {
        TasksRuntime_WriteMessage(message, messageSize, "The airlock is still sealed. Restore the oxygen system first.");
        return true;
    }

    if (!Map_IsSwampOuterUnlocked(map)) {
        Map_UnlockSwampOuter(map);
        TasksRuntime_WriteMessage(message, messageSize, "Airlock cycling complete. The outer blast door is now open.");
        return true;
    }

    Map_LockSwampOuter(map);
    TasksRuntime_WriteMessage(message, messageSize, "Airlock cycle reversed. The outer blast door is now closed.");
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
            TasksRuntime_WriteMessage(message, messageSize, "The power module is fully restored. The east-route qualification has paid off, the ruins entrance is now open, and Loxi can begin reading the north route as a real endgame structure instead of raw hazard.");
            return true;
        }

        TasksRuntime_WriteMessage(message, messageSize, "Power repair still needs 1 Energy Core, 2 Ore, and 1 Energy Crystal.");
        return true;
    }

    TasksRuntime_WriteMessage(message, messageSize, "The power console is operating normally.");
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
            TasksRuntime_WriteMessage(message, messageSize, "This is the workbench. Press F to open crafting.");
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
