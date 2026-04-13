#include "task_content.h"

#include <array>
#include <cstring>

namespace {

typedef struct StageTextDef {
    int stage;
    const char *stageName;
    const char *defaultObjective;
} StageTextDef;

typedef struct EndingTextDef {
    GameEnding ending;
    const char *title;
    const char *body;
} EndingTextDef;

const std::array<StageTextDef, 7> kStageTextDefs = {{
    {1, "Stage 1: Wake Up", "Gather 3 Wood + 2 Metal Scrap, then repair the oxygen console."},
    {2, "Stage 2: First Steps", "Gather Glow Moss and Ore, craft a Glow Stick, finish oxygen repair."},
    {3, "Stage 3: Into the Wild", "Open airlock, gather Vine/Fruit/Fungus, repair comm relay."},
    {4, "Stage 4: Rising Risk", "Craft Laser Gun and Suit, then inspect the crash clue."},
    {5, "Stage 5: Power Breakthrough", "Secure an Energy Core and restore the power bay."},
    {6, "Stage 6: Final Preparation", "Collect 3 Relic Fragments and sync with Loxi terminal."},
    {7, "Stage 7: Final Choice", "Recover the final logs, return to Loxi, and choose the ending route."}
}};

const std::array<EndingTextDef, 4> kEndingTextDefs = {{
    {ENDING_HEROIC, "Heroic Rescue", "You defeated the final threat and manually activated the Signal Tower. Loxi confirmed the rescue beacon was sent."},
    {ENDING_PEACEFUL, "Peaceful Rescue", "You avoided the final battle and stabilized the tower with the Signal Amplifier, leaving the alien ecosystem undisturbed."},
    {ENDING_SETTLEMENT, "Alien Settlement", "You gave up on uncertain rescue and transformed the ship base into a long-term home."},
    {ENDING_FAILURE, "Failed Survival", "Repeated collapses and failing life-support left the expedition unsustainable. The ship could no longer keep you alive on this world."}
}};

bool IsTowerPlateau(const Player *player) {
    return player != NULL && player->gridY <= SIGNAL_TOWER_Y + 6;
}

bool IsWestRouteAvailable(const TaskSystem *tasks) {
    return tasks != NULL
        && tasks->stage >= 4
        && tasks->commRepairLevel >= 1;
}

bool IsSouthRouteAvailable(const TaskSystem *tasks) {
    return tasks != NULL
        && tasks->stage >= 5
        && tasks->energyRepairLevel >= 1;
}

bool IsCrossX1Ready(const TaskSystem *tasks) {
    return tasks != NULL
        && tasks->westW3Completed
        && tasks->southS2Completed;
}

bool IsCrossX2Ready(const TaskSystem *tasks) {
    return tasks != NULL
        && tasks->westW4Completed
        && tasks->southS4Completed;
}

bool IsCrossX3Ready(const TaskSystem *tasks) {
    return tasks != NULL
        && tasks->westW5Completed
        && tasks->southS5Completed;
}

const StageTextDef *FindStageTextDef(int stage) {
    for (const StageTextDef &entry : kStageTextDefs) {
        if (entry.stage == stage) {
            return &entry;
        }
    }

    return NULL;
}

const EndingTextDef *FindEndingTextDef(GameEnding ending) {
    for (const EndingTextDef &entry : kEndingTextDefs) {
        if (entry.ending == ending) {
            return &entry;
        }
    }

    return NULL;
}

}  // namespace

const char *TasksContent_GetStageObjective(const TaskSystem *tasks, const Player *player) {
    const StageTextDef *stageText;

    if (tasks == NULL) {
        return "Follow Loxi and keep exploring.";
    }

    stageText = FindStageTextDef(tasks->stage);
    if (tasks->stage == 6) {
        if (player != NULL && player->resources[RESOURCE_RELIC_FRAGMENT] >= 3) {
            if (IsCrossX2Ready(tasks)) {
                return "Sync fragments at Loxi and review the shared late-route analysis before the final route commit.";
            }
            if (IsCrossX1Ready(tasks)) {
                return "Sync fragments at Loxi and finish aligning the west and south evidence.";
            }
            return "Sync fragments at Loxi terminal.";
        }
        if (IsCrossX2Ready(tasks)) {
            return "Collect 3 Relic Fragments and confirm Loxi's late-route analysis at the terminal.";
        }
        if (IsCrossX1Ready(tasks)) {
            return "Collect 3 Relic Fragments and prepare the west-south evidence alignment at Loxi.";
        }
    }
    if (tasks->stage == 7) {
        if (Tasks_IsEndingBranchReady(tasks) && tasks->selectedEndingRoute == ENDING_NONE) {
            return "Return to Loxi and choose the final route.";
        }
        if (tasks->selectedEndingRoute == ENDING_HEROIC) {
            if (tasks->bossDefeated) {
                return "Heroic route chosen. Reach the Signal Tower and launch the rescue beacon.";
            }
            if (tasks->monolithsLit >= 3) {
                return "Heroic route chosen. The ring is complete. Defeat the guardian, then reach the Signal Tower.";
            }
            if (tasks->monolithsLit > 0) {
                return "Heroic route chosen. Light remaining monoliths or defeat the guardian, then reach the Signal Tower.";
            }
            return "Heroic route chosen. Light monoliths or defeat the guardian, then reach the Signal Tower.";
        }
        if (tasks->selectedEndingRoute == ENDING_PEACEFUL) {
            if (player != NULL && player->hasSignalAmplifier) {
                return "Peaceful route chosen. Carry the Signal Amplifier to the Signal Tower.";
            }
            return "Peaceful route chosen. Craft the Signal Amplifier, then carry it to the Signal Tower.";
        }
        if (!Tasks_IsEndingBranchReady(tasks)) {
            return "Recover the remaining mainline logs and finish west/south archive tasks before choosing an ending with Loxi.";
        }
        return "Return to Loxi and choose the final route.";
    }

    if (stageText != NULL) {
        return stageText->defaultObjective;
    }

    return "Follow Loxi and keep exploring.";
}

const char *TasksContent_GetStageGuidance(const TaskSystem *tasks, const Player *player) {
    MapArea area;
    const char *locationName;

    if (tasks == NULL) {
        return "Recover vitals and move to the next objective.";
    }

    area = player != NULL ? Map_GetAreaAt(player->gridX, player->gridY) : MAP_AREA_UNKNOWN;
    locationName = player != NULL ? Map_GetLocationNameAt(player->gridX, player->gridY) : "Unknown Area";
    if (std::strcmp(locationName, "West Frontier") == 0) {
        if (!IsWestRouteAvailable(tasks)) {
            return "West Frontier locked. Restore comm relay first.";
        }
        if (tasks->westW5Completed) {
            return "West investigation complete.";
        }
        if (tasks->westW5Started) {
            return "Finish the Last Camp pass and return to base.";
        }
        if (tasks->westW4Completed) {
            return "Echo Basin archive complete. Start the Last Camp pass.";
        }
        if (tasks->westW4Started) {
            return "Finish the Echo Basin pass and return to base.";
        }
        if (tasks->westW3Completed) {
            return "Canopy Hollow archive complete. Start the Echo Basin pass.";
        }
        if (tasks->westW3Started) {
            return "Finish the Canopy Hollow pass and return to base.";
        }
        if (tasks->westW2Completed) {
            return "Survey Break archive complete. Start the Canopy Hollow pass.";
        }
        if (tasks->westW2Started) {
            return "Finish the Survey Break pass and return to base.";
        }
        if (tasks->westW1Completed) {
            return "West Frontier archive complete. Start Survey Break.";
        }
        if (tasks->westW1Started) {
            return "Finish the West Frontier pass and return to base.";
        }
        return "The west investigation is open. Complete the first West Frontier pass and report to base.";
    }
    if (std::strcmp(locationName, "Survey Break") == 0) {
        if (!IsWestRouteAvailable(tasks)) {
            return "Survey Break locked. Restore comm relay first.";
        }
        if (tasks->westW3Completed) {
            return "West route archived through Echo Basin.";
        }
        if (tasks->westW3Started) {
            return "The Canopy Hollow pass is active. Complete it and return.";
        }
        if (tasks->westW2Completed) {
            return "Survey Break archive complete. Proceed to Canopy Hollow.";
        }
        if (tasks->westW2Started) {
            return "Finish the Survey Break objectives and return to base.";
        }
        if (tasks->westW1Completed) {
            return "The Survey Break pass is active here. Complete it and return.";
        }
        return "Complete the West Frontier pass first.";
    }
    if (std::strcmp(locationName, "Canopy Hollow") == 0) {
        if (!IsWestRouteAvailable(tasks)) {
            return "Canopy Hollow locked. Restore comm relay first.";
        }
        if (tasks->westW5Completed) {
            return "West route fully archived.";
        }
        if (tasks->westW5Started) {
            return "The Last Camp pass is active. Complete it and return.";
        }
        if (tasks->westW4Completed) {
            return "Echo Basin archive complete. Proceed to Last Camp.";
        }
        if (tasks->westW4Started) {
            return "The Echo Basin pass is active. Complete it and return.";
        }
        if (tasks->westW3Completed) {
            return "Canopy Hollow archive complete. Proceed to Echo Basin.";
        }
        if (tasks->westW3Started) {
            return "Finish the Canopy Hollow pass here and return to base.";
        }
        if (tasks->westW2Completed) {
            return "The Canopy Hollow pass is active here. Complete it and return.";
        }
        return "Complete the Survey Break pass first.";
    }
    if (std::strcmp(locationName, "Echo Basin") == 0) {
        if (!IsWestRouteAvailable(tasks)) {
            return "Echo Basin locked. Restore comm relay first.";
        }
        if (tasks->westW5Completed) {
            return "West route fully archived.";
        }
        if (tasks->westW5Started) {
            return "The Last Camp pass is active. Complete it and return.";
        }
        if (tasks->westW4Completed) {
            return "Echo Basin archive complete. Proceed to Last Camp.";
        }
        if (tasks->westW4Started) {
            return "Finish the Echo Basin pass here and return to base.";
        }
        if (tasks->westW3Completed) {
            return "The Echo Basin pass is active here. Complete it and return.";
        }
        return "Complete the Canopy Hollow pass first.";
    }
    if (std::strcmp(locationName, "Last Camp") == 0) {
        if (!IsWestRouteAvailable(tasks)) {
            return "Last Camp locked. Restore comm relay first.";
        }
        if (tasks->westW5Completed) {
            return "Last Camp archive complete.";
        }
        if (tasks->westW5Started) {
            return "Finish the Last Camp pass here and return to base.";
        }
        if (tasks->westW4Completed) {
            return "The Last Camp pass is active here. Complete it and return.";
        }
        return "Complete the Echo Basin pass first.";
    }
    if (std::strcmp(locationName, "South Collapse") == 0) {
        if (!IsSouthRouteAvailable(tasks)) {
            return "South Collapse locked. Restore power bay first.";
        }
        if (tasks->southS5Completed) {
            return "South investigation complete.";
        }
        if (tasks->southS5Started) {
            return "Finish the Root Vault pass and return to base.";
        }
        if (tasks->southS4Completed) {
            return "Purifier Ring archive complete. Start the Root Vault pass.";
        }
        if (tasks->southS4Started) {
            return "Finish the Purifier Ring pass and return to base.";
        }
        if (tasks->southS3Completed) {
            return "Service Shaft archive complete. Start the Purifier Ring pass.";
        }
        if (tasks->southS3Started) {
            return "Finish the Service Shaft pass and return to base.";
        }
        if (tasks->southS2Completed) {
            return "Vent Galleries archive complete. Start the Service Shaft pass.";
        }
        if (tasks->southS2Started) {
            return "Finish the Vent Galleries pass and return to base.";
        }
        if (tasks->southS1Completed) {
            return "South Collapse archive complete. Start Vent Galleries.";
        }
        if (tasks->southS1Started) {
            return "Finish the South Collapse pass and return to base.";
        }
        return "The south investigation is open. Complete the first South Collapse pass and report to base.";
    }
    if (std::strcmp(locationName, "Vent Galleries") == 0) {
        if (!IsSouthRouteAvailable(tasks)) {
            return "Vent Galleries locked. Restore power bay first.";
        }
        if (tasks->southS3Completed) {
            return "South route archived through Service Shafts.";
        }
        if (tasks->southS3Started) {
            return "The Service Shaft pass is active. Complete it and return.";
        }
        if (tasks->southS2Completed) {
            return "Vent Galleries archive complete. Proceed to Service Shafts.";
        }
        if (tasks->southS2Started) {
            return "Finish the Vent Galleries pass here and return to base.";
        }
        if (tasks->southS1Completed) {
            return "The Vent Galleries pass is active here. Complete it and return.";
        }
        return "Complete the South Collapse pass first.";
    }
    if (std::strcmp(locationName, "Service Shafts") == 0) {
        if (!IsSouthRouteAvailable(tasks)) {
            return "Service Shafts locked. Restore power bay first.";
        }
        if (tasks->southS5Completed) {
            return "South route fully archived.";
        }
        if (tasks->southS5Started) {
            return "The Root Vault pass is active. Complete it and return.";
        }
        if (tasks->southS4Completed) {
            return "Purifier Ring archive complete. Proceed to Root Vault.";
        }
        if (tasks->southS4Started) {
            return "The Purifier Ring pass is active. Complete it and return.";
        }
        if (tasks->southS3Completed) {
            return "Service Shaft archive complete. Proceed to Purifier Ring.";
        }
        if (tasks->southS3Started) {
            return "Finish the Service Shaft pass here and return to base.";
        }
        if (tasks->southS2Completed) {
            return "The Service Shaft pass is active here. Complete it and return.";
        }
        return "Complete the Vent Galleries pass first.";
    }
    if (std::strcmp(locationName, "Purifier Ring") == 0) {
        if (!IsSouthRouteAvailable(tasks)) {
            return "Purifier Ring locked. Restore power bay first.";
        }
        if (tasks->southS5Completed) {
            return "South route fully archived.";
        }
        if (tasks->southS5Started) {
            return "The Root Vault pass is active. Complete it and return.";
        }
        if (tasks->southS4Completed) {
            return "Purifier Ring archive complete. Proceed to Root Vault.";
        }
        if (tasks->southS4Started) {
            return "Finish the Purifier Ring pass here and return to base.";
        }
        if (tasks->southS3Completed) {
            return "The Purifier Ring pass is active here. Complete it and return.";
        }
        return "Complete the Service Shaft pass first.";
    }
    if (std::strcmp(locationName, "Root Vault") == 0) {
        if (!IsSouthRouteAvailable(tasks)) {
            return "Root Vault locked. Restore power bay first.";
        }
        if (tasks->southS5Completed) {
            return "Root Vault archive complete.";
        }
        if (tasks->southS5Started) {
            return "Finish the Root Vault pass here and return to base.";
        }
        if (tasks->southS4Completed) {
            return "The Root Vault pass is active here. Complete it and return.";
        }
        return "Complete the Purifier Ring pass first.";
    }

    switch (tasks->stage) {
        case 1:
            return "Gather Wood and Scrap, then repair oxygen console.";
        case 2:
            return "Gather Glow Moss and Ore, craft Glow Stick, finish repair.";
        case 3:
            return area == MAP_AREA_BASE
                ? "Use east airlock and start comm relay repair."
                : (std::strcmp(locationName, "Outer Swamp Rim") == 0
                    ? "Gather Vine, Shell Fruit, and Fungus, then return."
                    : "Return to relay route and avoid deep detours now.");
        case 4:
            return area == MAP_AREA_BASE
                ? "Craft Laser Gun and Protection Suit at workbench."
                : "Inspect crash clue with both upgrades equipped.";
        case 5:
            if (area == MAP_AREA_BASE) {
                if (player != NULL && player->resources[RESOURCE_ENERGY_CORE] > 0) {
                    return "Install Energy Core in power bay.";
                }
                return "Prepare gear, then head east for Energy Core.";
            }
            if (area == MAP_AREA_SWAMP_DEEP) {
                return "Secure Energy Core in deep swamp and return safely. Use Rope and Camp as your return fallback.";
            }
            return "Use Rope/Camp and oxygen items for east route runs.";
        case 6:
            if (area == MAP_AREA_BASE) {
                if (player != NULL && player->resources[RESOURCE_RELIC_FRAGMENT] >= 3) {
                    if (IsCrossX2Ready(tasks)) {
                        return "Shared late-route analysis is ready. Sync fragments at Loxi before the final route commit.";
                    }
                    if (IsCrossX1Ready(tasks)) {
                        return "Shared route evidence is ready. Sync fragments at Loxi before the final route commit.";
                    }
                    return "Sync fragments at Loxi terminal.";
                }
                if (IsCrossX2Ready(tasks)) {
                    return "Shared late-route analysis is unlocked. Gather 3 fragments, then return to Loxi.";
                }
                if (IsCrossX1Ready(tasks)) {
                    return "Shared route evidence is unlocked. Gather 3 fragments, then return to Loxi.";
                }
                return "Collect 3 Relic Fragments in north ruins.";
            }
            if (area == MAP_AREA_RUINS) {
                if (IsCrossX2Ready(tasks)) {
                    return "Ruins prep now supports Loxi's late-route analysis. Finish the fragment set and return to Loxi.";
                }
                return "Collect fragments in ruins and return to base.";
            }
            return "Gather fragments, then sync with Loxi.";
        case 7:
            if (area == MAP_AREA_BASE) {
                if (Tasks_IsEndingBranchReady(tasks) && tasks->selectedEndingRoute == ENDING_NONE) {
                    return "All evidence is assembled. Loxi is now the ending branch point.";
                }
                if (tasks->selectedEndingRoute == ENDING_HEROIC) {
                    if (tasks->bossDefeated) {
                        return "Heroic route chosen. Head to the Signal Tower.";
                    }
                    return "Heroic route chosen. Finish monolith prep if needed, then defeat the guardian.";
                }
                if (tasks->selectedEndingRoute == ENDING_PEACEFUL) {
                    if (player != NULL && player->hasSignalAmplifier) {
                        return "Peaceful route chosen. Take the Signal Amplifier to the tower.";
                    }
                    return "Peaceful route chosen. Craft the Signal Amplifier before the tower run.";
                }
                if (IsCrossX3Ready(tasks)) {
                    return "West and south conclusions are synchronized. Finish the main archive and return here to choose.";
                }
                if (IsCrossX2Ready(tasks)) {
                    return "Late-route analysis is stable. Finish the main archive work before the final branch opens.";
                }
                return "Final stage is now an archive phase: recover every remaining mainline log, then choose your ending here.";
            }
            if (tasks->selectedEndingRoute == ENDING_HEROIC) {
                if (tasks->bossDefeated) {
                    return "The tower is ready for the chosen heroic ending.";
                }
                if (tasks->monolithsLit >= 3) {
                    return "Heroic route selected. The guardian is weakened.";
                }
                return "Heroic route selected. Finish monolith prep or hunt the guardian.";
            }
            if (tasks->selectedEndingRoute == ENDING_PEACEFUL) {
                if (player != NULL && player->hasSignalAmplifier) {
                    return "Peaceful route selected. Bring the Signal Amplifier to the tower.";
                }
                return "Peaceful route selected. Craft the Signal Amplifier first.";
            }
            if (Tasks_IsEndingBranchReady(tasks)) {
                return "The main archive is complete. Return to Loxi to choose the ending route before pushing farther.";
            }
            return "Recover the remaining mainline logs and finish archive tasks before the final branch opens.";
        default:
            return "Recover vitals and follow current objective.";
    }
}

const char *TasksContent_GetFieldNote(const TaskSystem *tasks, const Player *player) {
    MapArea area;
    const char *locationName;

    if (tasks == NULL || player == NULL) {
        return "Watch oxygen, health, and retreat route.";
    }

    if (player->oxygen < 30.0f) {
        return "Low oxygen. Retreat or recover now.";
    }

    if (player->poison >= 40.0f) {
        return "High poison. Use Calm Mushroom or retreat.";
    }

    if (Player_HasStatus(player, PLAYER_STATUS_OXYGEN_LEAK)) {
        return "Oxygen leak active. Repair suit soon.";
    }

    if (!Player_CanCraftAdvanced(player)) {
        return "Vitals unstable for advanced crafting.";
    }

    area = Map_GetAreaAt(player->gridX, player->gridY);
    locationName = Map_GetLocationNameAt(player->gridX, player->gridY);
    if (std::strcmp(locationName, "West Frontier") == 0) {
        if (!IsWestRouteAvailable(tasks)) {
            return "West route locked until comm relay restore.";
        }
        if (tasks->westW5Completed) {
            return "West investigation complete.";
        }
        if (tasks->westW5Started) {
            return "Last Camp pass in progress. Finish and return.";
        }
        if (tasks->westW4Completed) {
            return "Echo Basin archive complete. Move to Last Camp.";
        }
        if (tasks->westW4Started) {
            return "Echo Basin pass in progress. Finish and return.";
        }
        if (tasks->westW3Completed) {
            return "Canopy Hollow archive complete. Move to Echo Basin.";
        }
        if (tasks->westW3Started) {
            return "Canopy Hollow pass in progress. Finish and return.";
        }
        if (tasks->westW2Completed) {
            return "Survey Break archive complete. Move to Canopy Hollow.";
        }
        if (tasks->westW2Started) {
            return "Survey Break pass in progress. Finish and return.";
        }
        if (tasks->westW1Completed) {
            return "West Frontier archive complete. Start Survey Break.";
        }
        return "West Frontier pass in progress. Complete and return.";
    }
    if (std::strcmp(locationName, "Survey Break") == 0) {
        if (!IsWestRouteAvailable(tasks)) {
            return "Survey Break locked until comm relay restore.";
        }
        if (tasks->westW3Completed) {
            return "West route archived through Echo Basin.";
        }
        if (tasks->westW3Started) {
            return "The Canopy Hollow pass is active. Finish it and return.";
        }
        if (tasks->westW2Completed) {
            return "Survey Break archive complete. Proceed to Canopy Hollow.";
        }
        if (tasks->westW2Started) {
            return "Survey Break pass in progress.";
        }
        if (tasks->westW1Completed) {
            return "The Survey Break pass is active here after the West Frontier archive is filed.";
        }
        return "Complete the West Frontier pass first.";
    }
    if (std::strcmp(locationName, "Canopy Hollow") == 0) {
        if (!IsWestRouteAvailable(tasks)) {
            return "Canopy Hollow locked until comm relay restore.";
        }
        if (tasks->westW5Completed) {
            return "West route fully archived.";
        }
        if (tasks->westW5Started) {
            return "The Last Camp pass is active. Finish it and return.";
        }
        if (tasks->westW4Completed) {
            return "Canopy Hollow archive complete. Move to Last Camp.";
        }
        if (tasks->westW4Started) {
            return "The Echo Basin pass is active. Finish it and return.";
        }
        if (tasks->westW3Completed) {
            return "Canopy Hollow archive complete. Proceed to Echo Basin.";
        }
        if (tasks->westW3Started) {
            return "Canopy Hollow pass in progress.";
        }
        if (tasks->westW2Completed) {
            return "The Canopy Hollow pass is active here after the Survey Break archive is filed.";
        }
        return "Complete the Survey Break pass first.";
    }
    if (std::strcmp(locationName, "Echo Basin") == 0) {
        if (!IsWestRouteAvailable(tasks)) {
            return "Echo Basin locked until comm relay restore.";
        }
        if (tasks->westW5Completed) {
            return "West route fully archived.";
        }
        if (tasks->westW5Started) {
            return "The Last Camp pass is active. Finish it and return.";
        }
        if (tasks->westW4Completed) {
            return "Echo Basin archive complete. Proceed to Last Camp.";
        }
        if (tasks->westW4Started) {
            return "Echo Basin pass in progress.";
        }
        if (tasks->westW3Completed) {
            return "The Echo Basin pass is active here after the Canopy Hollow archive is filed.";
        }
        return "Complete the Canopy Hollow pass first.";
    }
    if (std::strcmp(locationName, "Last Camp") == 0) {
        if (!IsWestRouteAvailable(tasks)) {
            return "Last Camp locked until comm relay restore.";
        }
        if (tasks->westW5Completed) {
            return "Last Camp archive complete.";
        }
        if (tasks->westW5Started) {
            return "Last Camp pass in progress.";
        }
        if (tasks->westW4Completed) {
            return "The Last Camp pass is active here after the Echo Basin archive is filed.";
        }
        return "Complete the Echo Basin pass first.";
    }
    if (std::strcmp(locationName, "South Collapse") == 0) {
        if (!IsSouthRouteAvailable(tasks)) {
            return "South route locked until power bay restore.";
        }
        if (tasks->southS5Completed) {
            return "South investigation complete.";
        }
        if (tasks->southS5Started) {
            return "Root Vault pass in progress. Finish and return.";
        }
        if (tasks->southS4Completed) {
            return "Purifier Ring archive complete. Move to Root Vault.";
        }
        if (tasks->southS4Started) {
            return "Purifier Ring pass in progress. Finish and return.";
        }
        if (tasks->southS3Completed) {
            return "Service Shaft archive complete. Move to Purifier Ring.";
        }
        if (tasks->southS3Started) {
            return "Service Shaft pass in progress. Finish and return.";
        }
        if (tasks->southS2Completed) {
            return "Vent Galleries archive complete. Move to Service Shafts.";
        }
        if (tasks->southS2Started) {
            return "Vent Galleries pass in progress. Finish and return.";
        }
        if (tasks->southS1Completed) {
            return "South Collapse archive complete. Start Vent Galleries.";
        }
        return "South Collapse pass in progress. Complete and return.";
    }
    if (std::strcmp(locationName, "Vent Galleries") == 0) {
        if (!IsSouthRouteAvailable(tasks)) {
            return "Vent Galleries locked until power bay restore.";
        }
        if (tasks->southS3Completed) {
            return "South route archived through Service Shafts.";
        }
        if (tasks->southS3Started) {
            return "The Service Shaft pass is active. Finish it and return.";
        }
        if (tasks->southS2Completed) {
            return "Vent Galleries archive complete. Proceed to Service Shafts.";
        }
        if (tasks->southS2Started) {
            return "Vent Galleries pass in progress.";
        }
        if (tasks->southS1Completed) {
            return "The Vent Galleries pass is active here after the South Collapse archive is filed.";
        }
        return "Complete the South Collapse pass first.";
    }
    if (std::strcmp(locationName, "Service Shafts") == 0) {
        if (!IsSouthRouteAvailable(tasks)) {
            return "Service Shafts locked until power bay restore.";
        }
        if (tasks->southS5Completed) {
            return "South route fully archived.";
        }
        if (tasks->southS5Started) {
            return "The Root Vault pass is active. Finish it and return.";
        }
        if (tasks->southS4Completed) {
            return "Service Shaft archive complete. Move to Root Vault.";
        }
        if (tasks->southS4Started) {
            return "The Purifier Ring pass is active. Finish it and return.";
        }
        if (tasks->southS3Completed) {
            return "Service Shaft archive complete. Proceed to Purifier Ring.";
        }
        if (tasks->southS3Started) {
            return "Service Shaft pass in progress.";
        }
        if (tasks->southS2Completed) {
            return "The Service Shaft pass is active here after the Vent Galleries archive is filed.";
        }
        return "Complete the Vent Galleries pass first.";
    }
    if (std::strcmp(locationName, "Purifier Ring") == 0) {
        if (!IsSouthRouteAvailable(tasks)) {
            return "Purifier Ring locked until power bay restore.";
        }
        if (tasks->southS5Completed) {
            return "South route fully archived.";
        }
        if (tasks->southS5Started) {
            return "The Root Vault pass is active. Finish it and return.";
        }
        if (tasks->southS4Completed) {
            return "Purifier Ring archive complete. Proceed to Root Vault.";
        }
        if (tasks->southS4Started) {
            return "Purifier Ring pass in progress.";
        }
        if (tasks->southS3Completed) {
            return "The Purifier Ring pass is active here after the Service Shaft archive is filed.";
        }
        return "Complete the Service Shaft pass first.";
    }
    if (std::strcmp(locationName, "Root Vault") == 0) {
        if (!IsSouthRouteAvailable(tasks)) {
            return "Root Vault locked until power bay restore.";
        }
        if (tasks->southS5Completed) {
            return "Root Vault archive complete.";
        }
        if (tasks->southS5Started) {
            return "Root Vault pass in progress.";
        }
        if (tasks->southS4Completed) {
            return "The Root Vault pass is active here after the Purifier Ring archive is filed.";
        }
        return "Complete the Purifier Ring pass first.";
    }

    switch (area) {
        case MAP_AREA_BASE:
            if (tasks->stage == 3) {
                return "Base is your relay-run hub. Prepare and return safely.";
            }
            if (tasks->stage == 4) {
                if (!player->hasLaserGun || !player->hasProtectionSuit) {
                    return "Craft Laser Gun and Protection Suit before heading out.";
                }
                return "Gear ready. Inspect crash clue.";
            }
            if (tasks->stage == 5) {
                if (player->resources[RESOURCE_ENERGY_CORE] > 0) {
                    return "Install Energy Core in power bay.";
                }
                return "Prepare Rope/Camp/Suit before deep swamp run.";
            }
            if (tasks->stage == 6) {
                if (player->resources[RESOURCE_RELIC_FRAGMENT] >= 3) {
                    if (IsCrossX2Ready(tasks)) {
                        return "Shared late-route analysis is unlocked. Sync fragments at Loxi before the final route choice.";
                    }
                    if (IsCrossX1Ready(tasks)) {
                        return "Shared route evidence is unlocked. Return fragments to Loxi and align the west/south record.";
                    }
                    return "Return fragments to Loxi terminal.";
                }
                if (IsCrossX2Ready(tasks)) {
                    return "Shared late-route analysis is ready. Finish fragment runs and return to Loxi.";
                }
                if (IsCrossX1Ready(tasks)) {
                    return "Shared route evidence is ready. Finish fragment runs and return to Loxi.";
                }
                return "Collect 3 Relic Fragments in north ruins.";
            }
            if (tasks->stage == 7) {
                if (Tasks_IsEndingBranchReady(tasks) && tasks->selectedEndingRoute == ENDING_NONE) {
                    return "All mainline logs recovered. Return to Loxi and choose the ending route.";
                }
                if (tasks->selectedEndingRoute == ENDING_HEROIC) {
                    if (tasks->bossDefeated) {
                        return "Heroic route locked. Finish at the Signal Tower.";
                    }
                    return "Heroic route locked. Push the guardian when ready.";
                }
                if (tasks->selectedEndingRoute == ENDING_PEACEFUL) {
                    if (player->hasSignalAmplifier) {
                        return "Peaceful route locked. Carry the amplifier to the tower.";
                    }
                    return "Peaceful route locked. Craft the amplifier before leaving base.";
                }
                if (IsCrossX3Ready(tasks)) {
                    return "Archive almost complete. One final sweep should unlock the ending choice.";
                }
                return "Final stage archive phase: collect truth first, choose an ending second.";
            }
            return "Base is the safest place to recover, craft, and sync.";
        case MAP_AREA_FOREST:
            if (player->crouching) {
                return "Crouching in forest reduces detection.";
            }
            return "Use Crouch in forest for stealth.";
        case MAP_AREA_SWAMP_OUTER:
            if (std::strcmp(locationName, "Outer Swamp Rim") == 0) {
                return "Outer Swamp Rim is safer; practice short runs and returns.";
            }
            if (std::strcmp(locationName, "Flooded Detour") == 0) {
                return "Flooded Detour is longer. Rope and Camp help return.";
            }
            if (player->hasRope && player->hasFieldCamp) {
                return "Use Rope shortcuts and Field Camp fallback.";
            }
            if (player->hasRope) {
                return "Rope reduces route length and oxygen pressure.";
            }
            if (player->hasFieldCamp) {
                return "Field Camp gives a safe fallback point.";
            }
            return "Outer swamp drains oxygen steadily. Plan return route.";
        case MAP_AREA_SWAMP_DEEP:
            if (std::strcmp(locationName, "Deep Gate") == 0) {
                return "Deep Gate is a good turn-back checkpoint.";
            }
            if (std::strcmp(locationName, "Deep Basin") == 0) {
                return "Deep Basin is high risk. Keep suit and oxygen support ready.";
            }
            if (player->hasProtectionSuit) {
                return "Protection Suit helps, but deep swamp is still dangerous.";
            }
            return "Deep swamp needs Protection Suit and poison control.";
        case MAP_AREA_RUINS:
            if (tasks->stage >= 7) {
                if (std::strcmp(locationName, "Signal Tower Plateau") == 0) {
                    if (tasks->selectedEndingRoute == ENDING_PEACEFUL && player->hasSignalAmplifier) {
                        return "Peaceful route chosen. Push for tower stabilization.";
                    }
                    if (tasks->selectedEndingRoute == ENDING_HEROIC && tasks->monolithsLit >= 3) {
                        return "Heroic route chosen. This is the best timing for the tower push after the guardian falls.";
                    }
                    if (tasks->selectedEndingRoute == ENDING_NONE) {
                        return "Do not commit here blind. Return to Loxi once the archive is complete.";
                    }
                    return "Plateau is dangerous. Follow the chosen route's prep before committing.";
                }
                if (std::strcmp(locationName, "Monolith Ring") == 0) {
                    if (tasks->monolithsLit >= 3) {
                        return "Monolith ring complete.";
                    }
                    if (tasks->monolithsLit > 0) {
                        return "Light remaining monoliths.";
                    }
                    return "Start lighting monoliths to weaken guardian.";
                }
            }
            if (std::strcmp(locationName, "Ruins Approach") == 0) {
                return "Ruins Approach: prepare and decide push depth.";
            }
            if (IsTowerPlateau(player)) {
                return "Tower area: keep oxygen margin and retreat plan.";
            }
            return "Ruins have high oxygen pressure. Use short planned runs.";
        case MAP_AREA_UNKNOWN:
        default:
            return "Watch oxygen, health, and retreat route.";
    }
}

const char *TasksContent_GetStageNameText(int stage) {
    const StageTextDef *stageText;

    stageText = FindStageTextDef(stage);
    if (stageText == NULL) {
        return "Unknown Stage";
    }

    return stageText->stageName;
}

const char *TasksContent_GetPhaseNameText(DayPhase phase) {
    switch (phase) {
        case DAY_PHASE_DAY:
            return "Day";
        case DAY_PHASE_DUSK:
            return "Dusk";
        case DAY_PHASE_NIGHT:
            return "Night";
        default:
            return "Unknown";
    }
}

const char *TasksContent_GetEventNameText(EventType eventType) {
    switch (eventType) {
        case EVENT_HARVEST:
            return "Resource Surge";
        case EVENT_CALM_BEASTS:
            return "Quiet Creatures";
        case EVENT_CLEAR_SKY:
            return "Clear Skies";
        case EVENT_SPORE_STORM:
            return "Spore Storm";
        case EVENT_MONSTER_FRENZY:
            return "Monster Frenzy";
        case EVENT_DEVICE_FAULT:
            return "Device Fault";
        default:
            return "None";
    }
}

const char *TasksContent_GetEndingTitleText(GameEnding ending) {
    const EndingTextDef *endingText;

    endingText = FindEndingTextDef(ending);
    if (endingText == NULL) {
        return "";
    }

    return endingText->title;
}

const char *TasksContent_GetEndingBodyText(GameEnding ending) {
    const EndingTextDef *endingText;

    endingText = FindEndingTextDef(ending);
    if (endingText == NULL) {
        return "";
    }

    return endingText->body;
}
