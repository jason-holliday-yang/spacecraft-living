#include "task_content.h"

#include <array>
#include <cstring>
#include <string>

namespace {

typedef struct StageTextDef {
    int stage;
    LocalizedText stageName;
    LocalizedText defaultObjective;
} StageTextDef;

typedef struct EndingTextDef {
    GameEnding ending;
    LocalizedText title;
    LocalizedText body;
} EndingTextDef;

const std::array<StageTextDef, 7> kStageTextDefs = {{
    {1, {"Wake Up", "苏醒"}, {"Gather 3 Wood + 2 Metal Scrap, then repair the oxygen console.", "收集 3 份木材和 2 份金属残片，然后修好氧气控制台。"}},
    {2, {"First Steps", "迈出第一步"}, {"Gather Glow Moss and Ore, craft a Glow Stick, finish oxygen repair.", "收集发光苔和矿石，制作荧光棒，完成氧气修复。"}},
    {3, {"Into the Wild", "走向荒野"}, {"Open airlock, gather Vine/Fruit/Fungus, repair comm relay.", "打开气闸，收集异星藤蔓 / 植物果实 / 特殊菌株，修复通讯中继。"}},
    {4, {"Rising Risk", "风险升级"}, {"Craft Laser Gun and Suit, then inspect the crash clue.", "制作激光枪和防护服，然后调查坠毁线索。"}},
    {5, {"Power Breakthrough", "动力突破"}, {"Secure an Energy Core and restore the power bay.", "取得能源核心并恢复动力舱。"}},
    {6, {"Final Preparation", "最终准备"}, {"Collect 3 Relic Fragments and sync with Loxi terminal.", "收集 3 枚遗迹碎片，并在洛希终端同步。"}},
    {7, {"Final Choice", "最终抉择"}, {"Recover the final logs, return to Loxi, and choose the ending route.", "找回最后的日志，回到洛希处，选择最终结局路线。"}}
}};

const std::array<EndingTextDef, 4> kEndingTextDefs = {{
    {ENDING_HEROIC, {"Heroic Rescue", "强行救援"}, {"The guardian fell, the Signal Tower yielded to manual control, and the beacon finally cut into the sky. Loxi confirmed the rescue call was sent, but it was a departure bought by force and marked by what had to be broken to make it happen.", "守卫倒下后，信号塔终于服从人工控制，求救信标也随之刺入天际。洛希确认求援已经发出，但这是一场靠强行突破换来的离开，它留下的痕迹也会和启程本身一起被记住。"}},
    {ENDING_PEACEFUL, {"Peaceful Rescue", "和平救援"}, {"The amplifier folded human systems into the tower's language, and the signal rose without a final breach. Loxi confirmed the route held, and the world was left quieter than it would have been if you had forced your way through it.", "信号放大器把人类系统嵌进了塔楼的语言，于是信号在没有最后强闯的情况下升起。洛希确认路线已经成立，而你离开时，这个世界也比被强行撕开时更完整、更安静。"}},
    {ENDING_SETTLEMENT, {"Alien Settlement", "异星定居"}, {"Rescue stopped being the only future that mattered. With Loxi, the ship, and the surviving systems, the wreck became a deliberate home and the beginning of a longer duty on this world.", "求援不再是唯一值得追逐的未来。借助洛希、飞船和那些仍能维持的系统，这艘残骸被真正改造成了家，也成了你在这颗星球上继续承担下去的起点。"}},
    {ENDING_FAILURE, {"Failed Survival", "生存失败"}, {"The collapses came faster than repair could answer them. Air, power, and shelter never held long enough, and the expedition ended before this world could be understood or survived on your terms.", "崩溃来得比修复更快。空气、电力与庇护始终没能维持得足够久，这场远征最终在你真正弄清这颗世界之前就先一步结束了。"}}
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
        return Loc_Translate("Follow Loxi and keep exploring.");
    }

    stageText = FindStageTextDef(tasks->stage);
    if (tasks->stage == 6) {
        if (player != NULL && player->resources[RESOURCE_RELIC_FRAGMENT] >= 3) {
            if (IsCrossX2Ready(tasks)) {
                return Loc_Translate("Sync fragments at Loxi and review its rewritten ending picture for rescue, stabilization, and settlement.");
            }
            if (IsCrossX1Ready(tasks)) {
                return Loc_Translate("Sync fragments at Loxi and let it align the west crew trail with the south facility record.");
            }
            return Loc_Translate("Sync fragments at Loxi terminal.");
        }
        if (IsCrossX2Ready(tasks)) {
            return Loc_Translate("Collect 3 Relic Fragments and bring them to Loxi so it can compare force, stabilization, and settlement with full system context.");
        }
        if (IsCrossX1Ready(tasks)) {
            return Loc_Translate("Collect 3 Relic Fragments and bring them to Loxi so it can align the west and south evidence into one timeline.");
        }
    }
    if (tasks->stage == 7) {
        if (Tasks_IsEndingBranchReady(tasks) && tasks->selectedEndingRoute == ENDING_NONE) {
            if (!tasks->endingArchiveReviewed) {
                return Loc_Translate("Return to Loxi and review the assembled archive before choosing the final route.");
            }
            return Loc_Translate("Return to Loxi and choose the final route.");
        }
        if (tasks->selectedEndingRoute == ENDING_HEROIC) {
            if (tasks->bossDefeated) {
                return Loc_Translate("Heroic route chosen. Reach the Signal Tower and launch the rescue beacon.");
            }
            if (player != NULL && Map_GetAreaAt(player->gridX, player->gridY) == MAP_AREA_BOSS_ARENA) {
                return Loc_Translate("Heroic route chosen. Defeat the guardian in the isolated arena, then return to the Signal Tower.");
            }
            return Loc_Translate("Heroic route chosen. Open the airlock and commit to the guardian arena.");
        }
        if (tasks->selectedEndingRoute == ENDING_PEACEFUL) {
            if (player != NULL && player->hasSignalAmplifier) {
                return Loc_Translate("Peaceful route chosen. Carry the Signal Amplifier to the Signal Tower.");
            }
            return Loc_Translate("Peaceful route chosen. Craft the Signal Amplifier, then carry it to the Signal Tower.");
        }
        if (!Tasks_IsEndingBranchReady(tasks)) {
            return Loc_Translate("Recover the remaining mainline logs and finish west/south archive tasks before choosing an ending with Loxi.");
        }
        return Loc_Translate("Return to Loxi and choose the final route.");
    }

    if (stageText != NULL) {
        return Loc_PickText(stageText->defaultObjective);
    }

    return Loc_Translate("Follow Loxi and keep exploring.");
}

const char *TasksContent_GetStageGuidance(const TaskSystem *tasks, const Player *player) {
    MapArea area;
    const char *locationName;

    if (tasks == NULL) {
        return Loc_Translate("Recover vitals and move to the next objective.");
    }

    area = player != NULL ? Map_GetAreaAt(player->gridX, player->gridY) : MAP_AREA_UNKNOWN;
    locationName = player != NULL ? Map_GetLocationNameAt(player->gridX, player->gridY) : "Unknown Area";
    if (std::strcmp(locationName, "West Frontier") == 0) {
        if (!IsWestRouteAvailable(tasks)) {
            return Loc_Translate("West Frontier locked. Restore comm relay first.");
        }
        if (tasks->westW5Completed) {
            return Loc_Translate("West investigation complete.");
        }
        if (tasks->westW5Started) {
            return Loc_Translate("Finish the Last Camp investigation and return to base.");
        }
        if (tasks->westW4Completed) {
            return Loc_Translate("Echo Basin archive complete. Start the Last Camp investigation.");
        }
        if (tasks->westW4Started) {
            return Loc_Translate("Finish the Echo Basin investigation and return to base.");
        }
        if (tasks->westW3Completed) {
            return Loc_Translate("Canopy Hollow archive complete. Start the Echo Basin investigation.");
        }
        if (tasks->westW3Started) {
            return Loc_Translate("Finish the Canopy Hollow investigation and return to base.");
        }
        if (tasks->westW2Completed) {
            return Loc_Translate("Survey Break archive complete. Start the Canopy Hollow investigation.");
        }
        if (tasks->westW2Started) {
            return Loc_Translate("Finish the Survey Break investigation and return to base.");
        }
        if (tasks->westW1Completed) {
            return Loc_Translate("West Frontier archive complete. Start Survey Break.");
        }
        if (tasks->westW1Started) {
            return Loc_Translate("Finish the West Frontier investigation and return to base.");
        }
        return Loc_Translate("The west investigation is open. Complete the first West Frontier investigation and report to base.");
    }
    if (std::strcmp(locationName, "Survey Break") == 0) {
        if (!IsWestRouteAvailable(tasks)) {
            return Loc_Translate("Survey Break locked. Restore comm relay first.");
        }
        if (tasks->westW3Completed) {
            return Loc_Translate("West route archived through Echo Basin.");
        }
        if (tasks->westW3Started) {
            return Loc_Translate("The Canopy Hollow investigation is active. Complete it and return.");
        }
        if (tasks->westW2Completed) {
            return Loc_Translate("Survey Break archive complete. Proceed to Canopy Hollow.");
        }
        if (tasks->westW2Started) {
            return Loc_Translate("Finish the Survey Break objectives and return to base.");
        }
        if (tasks->westW1Completed) {
            return Loc_Translate("The Survey Break investigation is active here. Complete it and return.");
        }
        return Loc_Translate("Complete the West Frontier investigation first.");
    }
    if (std::strcmp(locationName, "Canopy Hollow") == 0) {
        if (!IsWestRouteAvailable(tasks)) {
            return Loc_Translate("Canopy Hollow locked. Restore comm relay first.");
        }
        if (tasks->westW5Completed) {
            return Loc_Translate("West route fully archived.");
        }
        if (tasks->westW5Started) {
            return Loc_Translate("The Last Camp investigation is active. Complete it and return.");
        }
        if (tasks->westW4Completed) {
            return Loc_Translate("Echo Basin archive complete. Proceed to Last Camp.");
        }
        if (tasks->westW4Started) {
            return Loc_Translate("The Echo Basin investigation is active. Complete it and return.");
        }
        if (tasks->westW3Completed) {
            return Loc_Translate("Canopy Hollow archive complete. Proceed to Echo Basin.");
        }
        if (tasks->westW3Started) {
            return Loc_Translate("Finish the Canopy Hollow investigation here and return to base.");
        }
        if (tasks->westW2Completed) {
            return Loc_Translate("The Canopy Hollow investigation is active here. Complete it and return.");
        }
        return Loc_Translate("Complete the Survey Break investigation first.");
    }
    if (std::strcmp(locationName, "Echo Basin") == 0) {
        if (!IsWestRouteAvailable(tasks)) {
            return Loc_Translate("Echo Basin locked. Restore comm relay first.");
        }
        if (tasks->westW5Completed) {
            return Loc_Translate("West route fully archived.");
        }
        if (tasks->westW5Started) {
            return Loc_Translate("The Last Camp investigation is active. Complete it and return.");
        }
        if (tasks->westW4Completed) {
            return Loc_Translate("Echo Basin archive complete. Proceed to Last Camp.");
        }
        if (tasks->westW4Started) {
            return Loc_Translate("Finish the Echo Basin investigation here and return to base.");
        }
        if (tasks->westW3Completed) {
            return Loc_Translate("The Echo Basin investigation is active here. Complete it and return.");
        }
        return Loc_Translate("Complete the Canopy Hollow investigation first.");
    }
    if (std::strcmp(locationName, "Last Camp") == 0) {
        if (!IsWestRouteAvailable(tasks)) {
            return Loc_Translate("Last Camp locked. Restore comm relay first.");
        }
        if (tasks->westW5Completed) {
            return Loc_Translate("Last Camp archive complete.");
        }
        if (tasks->westW5Started) {
            return Loc_Translate("Finish the Last Camp investigation here and return to base.");
        }
        if (tasks->westW4Completed) {
            return Loc_Translate("The Last Camp investigation is active here. Complete it and return.");
        }
        return Loc_Translate("Complete the Echo Basin investigation first.");
    }
    if (std::strcmp(locationName, "South Collapse") == 0) {
        if (!IsSouthRouteAvailable(tasks)) {
            return Loc_Translate("South Collapse locked. Restore power bay first.");
        }
        if (tasks->southS5Completed) {
            return Loc_Translate("South investigation complete.");
        }
        if (tasks->southS5Started) {
            return Loc_Translate("Finish the Root Vault investigation and return to base.");
        }
        if (tasks->southS4Completed) {
            return Loc_Translate("Purifier Ring archive complete. Start the Root Vault investigation.");
        }
        if (tasks->southS4Started) {
            return Loc_Translate("Finish the Purifier Ring investigation and return to base.");
        }
        if (tasks->southS3Completed) {
            return Loc_Translate("Service Shaft archive complete. Start the Purifier Ring investigation.");
        }
        if (tasks->southS3Started) {
            return Loc_Translate("Finish the Service Shaft investigation and return to base.");
        }
        if (tasks->southS2Completed) {
            return Loc_Translate("Vent Galleries archive complete. Start the Service Shaft investigation.");
        }
        if (tasks->southS2Started) {
            return Loc_Translate("Finish the Vent Galleries investigation and return to base.");
        }
        if (tasks->southS1Completed) {
            return Loc_Translate("South Collapse archive complete. Start Vent Galleries.");
        }
        if (tasks->southS1Started) {
            return Loc_Translate("Finish the South Collapse investigation and return to base.");
        }
        return Loc_Translate("The south investigation is open. Complete the first South Collapse investigation and report to base.");
    }
    if (std::strcmp(locationName, "Vent Galleries") == 0) {
        if (!IsSouthRouteAvailable(tasks)) {
            return Loc_Translate("Vent Galleries locked. Restore power bay first.");
        }
        if (tasks->southS3Completed) {
            return Loc_Translate("South route archived through Service Shafts.");
        }
        if (tasks->southS3Started) {
            return Loc_Translate("The Service Shaft investigation is active. Complete it and return.");
        }
        if (tasks->southS2Completed) {
            return Loc_Translate("Vent Galleries archive complete. Proceed to Service Shafts.");
        }
        if (tasks->southS2Started) {
            return Loc_Translate("Finish the Vent Galleries investigation here and return to base.");
        }
        if (tasks->southS1Completed) {
            return Loc_Translate("The Vent Galleries investigation is active here. Complete it and return.");
        }
        return Loc_Translate("Complete the South Collapse investigation first.");
    }
    if (std::strcmp(locationName, "Service Shafts") == 0) {
        if (!IsSouthRouteAvailable(tasks)) {
            return Loc_Translate("Service Shafts locked. Restore power bay first.");
        }
        if (tasks->southS5Completed) {
            return Loc_Translate("South route fully archived.");
        }
        if (tasks->southS5Started) {
            return Loc_Translate("The Root Vault investigation is active. Complete it and return.");
        }
        if (tasks->southS4Completed) {
            return Loc_Translate("Purifier Ring archive complete. Proceed to Root Vault.");
        }
        if (tasks->southS4Started) {
            return Loc_Translate("The Purifier Ring investigation is active. Complete it and return.");
        }
        if (tasks->southS3Completed) {
            return Loc_Translate("Service Shaft archive complete. Proceed to Purifier Ring.");
        }
        if (tasks->southS3Started) {
            return Loc_Translate("Finish the Service Shaft investigation here and return to base.");
        }
        if (tasks->southS2Completed) {
            return Loc_Translate("The Service Shaft investigation is active here. Complete it and return.");
        }
        return Loc_Translate("Complete the Vent Galleries investigation first.");
    }
    if (std::strcmp(locationName, "Purifier Ring") == 0) {
        if (!IsSouthRouteAvailable(tasks)) {
            return Loc_Translate("Purifier Ring locked. Restore power bay first.");
        }
        if (tasks->southS5Completed) {
            return Loc_Translate("South route fully archived.");
        }
        if (tasks->southS5Started) {
            return Loc_Translate("The Root Vault investigation is active. Complete it and return.");
        }
        if (tasks->southS4Completed) {
            return Loc_Translate("Purifier Ring archive complete. Proceed to Root Vault.");
        }
        if (tasks->southS4Started) {
            return Loc_Translate("Finish the Purifier Ring investigation here and return to base.");
        }
        if (tasks->southS3Completed) {
            return Loc_Translate("The Purifier Ring investigation is active here. Complete it and return.");
        }
        return Loc_Translate("Complete the Service Shaft investigation first.");
    }
    if (std::strcmp(locationName, "Root Vault") == 0) {
        if (!IsSouthRouteAvailable(tasks)) {
            return Loc_Translate("Root Vault locked. Restore power bay first.");
        }
        if (tasks->southS5Completed) {
            return Loc_Translate("Root Vault archive complete.");
        }
        if (tasks->southS5Started) {
            return Loc_Translate("Finish the Root Vault investigation here and return to base.");
        }
        if (tasks->southS4Completed) {
            return Loc_Translate("The Root Vault investigation is active here. Complete it and return.");
        }
        return Loc_Translate("Complete the Purifier Ring investigation first.");
    }

    switch (tasks->stage) {
        case 1:
            return Loc_Translate("Gather Wood and Scrap, then repair oxygen console.");
        case 2:
            return Loc_Translate("Gather Glow Moss and Ore, craft Glow Stick, finish repair.");
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
                    return Loc_Translate("Install Energy Core in power bay.");
                }
                return Loc_Translate("Prepare gear, then head east for Energy Core.");
            }
            if (area == MAP_AREA_SWAMP_DEEP) {
                return Loc_Translate("Secure Energy Core in deep swamp and return safely. Use Rope and Camp as your return fallback.");
            }
            return Loc_Translate("Use Rope/Camp and oxygen items for east route runs.");
        case 6:
            if (area == MAP_AREA_BASE) {
                if (player != NULL && player->resources[RESOURCE_RELIC_FRAGMENT] >= 3) {
                    if (IsCrossX2Ready(tasks)) {
                        return Loc_Translate("Fragments ready. Loxi can now rewrite the ending picture around one failing maintenance lattice.");
                    }
                    if (IsCrossX1Ready(tasks)) {
                        return Loc_Translate("Fragments ready. Loxi can now align the west crew trail with the south facility record.");
                    }
                    return Loc_Translate("Sync fragments at Loxi terminal.");
                }
                if (IsCrossX2Ready(tasks)) {
                    return Loc_Translate("Late-route analysis is unlocked. Gather 3 fragments so Loxi can finish its rewritten ending picture.");
                }
                if (IsCrossX1Ready(tasks)) {
                    return Loc_Translate("Shared evidence is unlocked. Gather 3 fragments so Loxi can join the west and south record.");
                }
                return Loc_Translate("Collect 3 Relic Fragments in north ruins.");
            }
            if (area == MAP_AREA_RUINS) {
                if (IsCrossX2Ready(tasks)) {
                    return Loc_Translate("Ruins prep now supports Loxi's rewritten ending picture. Finish the fragment set and return.");
                }
                return Loc_Translate("Collect fragments in ruins and return to base.");
            }
            return Loc_Translate("Gather fragments, then sync with Loxi.");
        case 7:
            if (area == MAP_AREA_BASE) {
                if (Tasks_IsEndingBranchReady(tasks) && tasks->selectedEndingRoute == ENDING_NONE) {
                    if (!tasks->endingArchiveReviewed) {
                        return Loc_Translate("Full archive context is assembled. Return to Loxi so it can review how the west, south, and mainline evidence reshape each ending before you choose.");
                    }
                    return Loc_Translate("Full archive context is assembled. Loxi can now frame heroic rescue, peaceful rescue, or settlement as deliberate answers to the same truth.");
                }
                if (tasks->selectedEndingRoute == ENDING_HEROIC) {
                    if (tasks->bossDefeated) {
                        return Loc_Translate("Heroic route chosen. Head to the Signal Tower.");
                    }
                    if (tasks->monolithsLit >= 3) {
                        return Loc_Translate("Heroic route chosen. The ring is fully lit. Open the airlock when you are ready for the isolated guardian arena.");
                    }
                    if (tasks->monolithsLit > 0) {
                        return Loc_Translate("Heroic route chosen. Your monolith prep carries into the arena. Open the airlock when you are ready.");
                    }
                    return Loc_Translate("Heroic route chosen. Open the airlock to begin the isolated guardian fight.");
                }
                if (tasks->selectedEndingRoute == ENDING_PEACEFUL) {
                    if (player != NULL && player->hasSignalAmplifier) {
                        return Loc_Translate("Peaceful route chosen. Take the Signal Amplifier to the tower.");
                    }
                    return Loc_Translate("Peaceful route chosen. Craft the Signal Amplifier before the tower run.");
                }
                if (IsCrossX3Ready(tasks)) {
                    return Loc_Translate("West crew testimony and south system truth now agree on what the final choice means. Finish the main archive and return here.");
                }
                if (IsCrossX2Ready(tasks)) {
                    return Loc_Translate("Loxi has rewritten the late-game comparison around one failing maintenance lattice. Finish the main archive before the branch opens.");
                }
                if (IsCrossX1Ready(tasks)) {
                    return Loc_Translate("Crew traces and facility handovers now align into one timeline. Keep recovering the main archive.");
                }
                return Loc_Translate("Final stage is now an evidence sweep: recover every remaining mainline log, then choose your ending here.");
            }
            if (tasks->selectedEndingRoute == ENDING_HEROIC) {
                if (tasks->bossDefeated) {
                    return Loc_Translate("The tower is ready for the chosen heroic ending.");
                }
                if (area == MAP_AREA_BOSS_ARENA) {
                    if (tasks->monolithsLit >= 3) {
                        return Loc_Translate("Heroic route selected. The guardian is isolated, and the fully lit ring is weakening it.");
                    }
                    return Loc_Translate("Heroic route selected. The guardian is isolated inside the arena. Finish the fight, then return to the tower.");
                }
                if (tasks->monolithsLit >= 3) {
                    return Loc_Translate("Heroic route selected. The guardian is isolated, and the ring is already weakening it. Open the airlock when ready.");
                }
                return Loc_Translate("Heroic route selected. Open the airlock to enter the guardian arena.");
            }
            if (tasks->selectedEndingRoute == ENDING_PEACEFUL) {
                if (player != NULL && player->hasSignalAmplifier) {
                    return Loc_Translate("Peaceful route selected. Bring the Signal Amplifier to the tower.");
                }
                return Loc_Translate("Peaceful route selected. Craft the Signal Amplifier first.");
            }
            if (Tasks_IsEndingBranchReady(tasks)) {
                if (!tasks->endingArchiveReviewed) {
                    return Loc_Translate("The archive is assembled, but Loxi still needs a final review at the ship before any ending route becomes a deliberate commitment.");
                }
                return Loc_Translate("The main archive is complete. Return to Loxi to choose the ending route before pushing farther.");
            }
            return Loc_Translate("Recover the remaining mainline logs and finish archive tasks before the final branch opens.");
        default:
            return Loc_Translate("Recover vitals and follow current objective.");
    }
}

const char *TasksContent_GetFieldNote(const TaskSystem *tasks, const Player *player) {
    MapArea area;
    const char *locationName;

    if (tasks == NULL || player == NULL) {
        return Loc_Translate("Watch oxygen, health, and retreat route.");
    }

    if (player->oxygen < 30.0f) {
        return Loc_Translate("Low oxygen. Retreat or recover now.");
    }

    if (player->poison >= 40.0f) {
        return Loc_Translate("High poison. Use Calm Mushroom or retreat.");
    }

    if (Player_HasStatus(player, PLAYER_STATUS_OXYGEN_LEAK)) {
        return Loc_Translate("Oxygen leak active. Repair suit soon.");
    }

    if (!Player_CanCraftAdvanced(player)) {
        return Loc_Translate("Vitals unstable for advanced crafting.");
    }

    area = Map_GetAreaAt(player->gridX, player->gridY);
    locationName = Map_GetLocationNameAt(player->gridX, player->gridY);
    if (std::strcmp(locationName, "West Frontier") == 0) {
        if (!IsWestRouteAvailable(tasks)) {
            return Loc_Translate("West route stays closed until the comm relay is restored.");
        }
        if (tasks->westW5Completed) {
            return Loc_Translate("West investigation complete.");
        }
        if (tasks->westW5Started) {
            return Loc_Translate("Last Camp investigation in progress. Finish and return.");
        }
        if (tasks->westW4Completed) {
            return Loc_Translate("Echo Basin archive complete. Move to Last Camp.");
        }
        if (tasks->westW4Started) {
            return Loc_Translate("Echo Basin investigation in progress. Finish and return.");
        }
        if (tasks->westW3Completed) {
            return Loc_Translate("Canopy Hollow archive complete. Move to Echo Basin.");
        }
        if (tasks->westW3Started) {
            return Loc_Translate("Canopy Hollow investigation in progress. Finish and return.");
        }
        if (tasks->westW2Completed) {
            return Loc_Translate("Survey Break archive complete. Move to Canopy Hollow.");
        }
        if (tasks->westW2Started) {
            return Loc_Translate("Survey Break investigation in progress. Finish and return.");
        }
        if (tasks->westW1Completed) {
            return Loc_Translate("West Frontier archive complete. Start Survey Break.");
        }
        return Loc_Translate("West Frontier investigation in progress. Complete and return.");
    }
    if (std::strcmp(locationName, "Survey Break") == 0) {
        if (!IsWestRouteAvailable(tasks)) {
            return Loc_Translate("Survey Break stays closed until the comm relay is restored.");
        }
        if (tasks->westW3Completed) {
            return Loc_Translate("West route archived through Echo Basin.");
        }
        if (tasks->westW3Started) {
            return Loc_Translate("The Canopy Hollow investigation is active. Finish it and return.");
        }
        if (tasks->westW2Completed) {
            return Loc_Translate("Survey Break archive complete. Proceed to Canopy Hollow.");
        }
        if (tasks->westW2Started) {
            return Loc_Translate("Survey Break investigation in progress.");
        }
        if (tasks->westW1Completed) {
            return Loc_Translate("The Survey Break investigation is active here after the West Frontier archive is filed.");
        }
        return Loc_Translate("Complete the West Frontier investigation first.");
    }
    if (std::strcmp(locationName, "Canopy Hollow") == 0) {
        if (!IsWestRouteAvailable(tasks)) {
            return Loc_Translate("Canopy Hollow stays closed until the comm relay is restored.");
        }
        if (tasks->westW5Completed) {
            return Loc_Translate("West route fully archived.");
        }
        if (tasks->westW5Started) {
            return Loc_Translate("The Last Camp investigation is active. Finish it and return.");
        }
        if (tasks->westW4Completed) {
            return Loc_Translate("Canopy Hollow archive complete. Move to Last Camp.");
        }
        if (tasks->westW4Started) {
            return Loc_Translate("The Echo Basin investigation is active. Finish it and return.");
        }
        if (tasks->westW3Completed) {
            return Loc_Translate("Canopy Hollow archive complete. Proceed to Echo Basin.");
        }
        if (tasks->westW3Started) {
            return Loc_Translate("Canopy Hollow investigation in progress.");
        }
        if (tasks->westW2Completed) {
            return Loc_Translate("The Canopy Hollow investigation is active here after the Survey Break archive is filed.");
        }
        return Loc_Translate("Complete the Survey Break investigation first.");
    }
    if (std::strcmp(locationName, "Echo Basin") == 0) {
        if (!IsWestRouteAvailable(tasks)) {
            return Loc_Translate("Echo Basin stays closed until the comm relay is restored.");
        }
        if (tasks->westW5Completed) {
            return Loc_Translate("West route fully archived.");
        }
        if (tasks->westW5Started) {
            return Loc_Translate("The Last Camp investigation is active. Finish it and return.");
        }
        if (tasks->westW4Completed) {
            return Loc_Translate("Echo Basin archive complete. Proceed to Last Camp.");
        }
        if (tasks->westW4Started) {
            return Loc_Translate("Echo Basin investigation in progress.");
        }
        if (tasks->westW3Completed) {
            return Loc_Translate("The Echo Basin investigation is active here after the Canopy Hollow archive is filed.");
        }
        return Loc_Translate("Complete the Canopy Hollow investigation first.");
    }
    if (std::strcmp(locationName, "Last Camp") == 0) {
        if (!IsWestRouteAvailable(tasks)) {
            return Loc_Translate("Last Camp stays closed until the comm relay is restored.");
        }
        if (tasks->westW5Completed) {
            return Loc_Translate("Last Camp archive complete.");
        }
        if (tasks->westW5Started) {
            return Loc_Translate("Last Camp investigation in progress.");
        }
        if (tasks->westW4Completed) {
            return Loc_Translate("The Last Camp investigation is active here after the Echo Basin archive is filed.");
        }
        return Loc_Translate("Complete the Echo Basin investigation first.");
    }
    if (std::strcmp(locationName, "South Collapse") == 0) {
        if (!IsSouthRouteAvailable(tasks)) {
            return Loc_Translate("South route stays closed until the Power Bay is restored.");
        }
        if (tasks->southS5Completed) {
            return Loc_Translate("South investigation complete.");
        }
        if (tasks->southS5Started) {
            return Loc_Translate("Root Vault investigation in progress. Finish and return.");
        }
        if (tasks->southS4Completed) {
            return Loc_Translate("Purifier Ring archive complete. Move to Root Vault.");
        }
        if (tasks->southS4Started) {
            return Loc_Translate("Purifier Ring investigation in progress. Finish and return.");
        }
        if (tasks->southS3Completed) {
            return Loc_Translate("Service Shaft archive complete. Move to Purifier Ring.");
        }
        if (tasks->southS3Started) {
            return Loc_Translate("Service Shaft investigation in progress. Finish and return.");
        }
        if (tasks->southS2Completed) {
            return Loc_Translate("Vent Galleries archive complete. Move to Service Shafts.");
        }
        if (tasks->southS2Started) {
            return Loc_Translate("Vent Galleries investigation in progress. Finish and return.");
        }
        if (tasks->southS1Completed) {
            return Loc_Translate("South Collapse archive complete. Start Vent Galleries.");
        }
        return Loc_Translate("South Collapse investigation in progress. Complete and return.");
    }
    if (std::strcmp(locationName, "Vent Galleries") == 0) {
        if (!IsSouthRouteAvailable(tasks)) {
            return Loc_Translate("Vent Galleries stay closed until the Power Bay is restored.");
        }
        if (tasks->southS3Completed) {
            return Loc_Translate("South route archived through Service Shafts.");
        }
        if (tasks->southS3Started) {
            return Loc_Translate("The Service Shaft investigation is active. Finish it and return.");
        }
        if (tasks->southS2Completed) {
            return Loc_Translate("Vent Galleries archive complete. Proceed to Service Shafts.");
        }
        if (tasks->southS2Started) {
            return Loc_Translate("Vent Galleries investigation in progress.");
        }
        if (tasks->southS1Completed) {
            return Loc_Translate("The Vent Galleries investigation is active here after the South Collapse archive is filed.");
        }
        return Loc_Translate("Complete the South Collapse investigation first.");
    }
    if (std::strcmp(locationName, "Service Shafts") == 0) {
        if (!IsSouthRouteAvailable(tasks)) {
            return Loc_Translate("Service Shafts stay closed until the Power Bay is restored.");
        }
        if (tasks->southS5Completed) {
            return Loc_Translate("South route fully archived.");
        }
        if (tasks->southS5Started) {
            return Loc_Translate("The Root Vault investigation is active. Finish it and return.");
        }
        if (tasks->southS4Completed) {
            return Loc_Translate("Service Shaft archive complete. Move to Root Vault.");
        }
        if (tasks->southS4Started) {
            return Loc_Translate("The Purifier Ring investigation is active. Finish it and return.");
        }
        if (tasks->southS3Completed) {
            return Loc_Translate("Service Shaft archive complete. Proceed to Purifier Ring.");
        }
        if (tasks->southS3Started) {
            return Loc_Translate("Service Shaft investigation in progress.");
        }
        if (tasks->southS2Completed) {
            return Loc_Translate("The Service Shaft investigation is active here after the Vent Galleries archive is filed.");
        }
        return Loc_Translate("Complete the Vent Galleries investigation first.");
    }
    if (std::strcmp(locationName, "Purifier Ring") == 0) {
        if (!IsSouthRouteAvailable(tasks)) {
            return Loc_Translate("Purifier Ring stays closed until the Power Bay is restored.");
        }
        if (tasks->southS5Completed) {
            return Loc_Translate("South route fully archived.");
        }
        if (tasks->southS5Started) {
            return Loc_Translate("The Root Vault investigation is active. Finish it and return.");
        }
        if (tasks->southS4Completed) {
            return Loc_Translate("Purifier Ring archive complete. Proceed to Root Vault.");
        }
        if (tasks->southS4Started) {
            return Loc_Translate("Purifier Ring investigation in progress.");
        }
        if (tasks->southS3Completed) {
            return Loc_Translate("The Purifier Ring investigation is active here after the Service Shaft archive is filed.");
        }
        return Loc_Translate("Complete the Service Shaft investigation first.");
    }
    if (std::strcmp(locationName, "Root Vault") == 0) {
        if (!IsSouthRouteAvailable(tasks)) {
            return Loc_Translate("Root Vault stays closed until the Power Bay is restored.");
        }
        if (tasks->southS5Completed) {
            return Loc_Translate("Root Vault archive complete.");
        }
        if (tasks->southS5Started) {
            return Loc_Translate("Root Vault investigation in progress.");
        }
        if (tasks->southS4Completed) {
            return Loc_Translate("The Root Vault investigation is active here after the Purifier Ring archive is filed.");
        }
        return Loc_Translate("Complete the Purifier Ring investigation first.");
    }

    switch (area) {
        case MAP_AREA_BASE:
            if (tasks->stage == 3) {
                return Loc_Translate("Base is your relay-run hub. Prepare and return safely.");
            }
            if (tasks->stage == 4) {
                if (!player->hasLaserGun || !player->hasProtectionSuit) {
                    return Loc_Translate("Craft Laser Gun and Protection Suit before heading out.");
                }
                return Loc_Translate("Gear ready. Inspect crash clue.");
            }
            if (tasks->stage == 5) {
                if (player->resources[RESOURCE_ENERGY_CORE] > 0) {
                    return Loc_Translate("Install Energy Core in power bay.");
                }
                return Loc_Translate("Prepare Rope/Camp/Suit before deep swamp run.");
            }
            if (tasks->stage == 6) {
                if (player->resources[RESOURCE_RELIC_FRAGMENT] >= 3) {
                    if (IsCrossX2Ready(tasks)) {
                        return Loc_Translate("Fragments can now let Loxi compare rescue, repair, and settlement with full system context.");
                    }
                    if (IsCrossX1Ready(tasks)) {
                        return Loc_Translate("Fragments can now let Loxi align west crew traces with south facility handovers.");
                    }
                    return Loc_Translate("Return fragments to Loxi terminal.");
                }
                if (IsCrossX2Ready(tasks)) {
                    return Loc_Translate("Finish fragment runs so Loxi can complete its maintenance-lattice ending picture.");
                }
                if (IsCrossX1Ready(tasks)) {
                    return Loc_Translate("Finish fragment runs so Loxi can lock the west/south timeline together.");
                }
                return Loc_Translate("Collect 3 Relic Fragments in north ruins.");
            }
            if (tasks->stage == 7) {
                if (Tasks_IsEndingBranchReady(tasks) && tasks->selectedEndingRoute == ENDING_NONE) {
                    if (!tasks->endingArchiveReviewed) {
                        return Loc_Translate("All mainline logs recovered. Return to Loxi and review the assembled truth before choosing how to answer it.");
                    }
                    return Loc_Translate("All mainline logs recovered. Return to Loxi and choose how to answer the full archive truth.");
                }
                if (tasks->selectedEndingRoute == ENDING_HEROIC) {
                    if (tasks->bossDefeated) {
                        return Loc_Translate("Heroic route chosen. Finish at the Signal Tower.");
                    }
                    if (tasks->monolithsLit >= 3) {
                        return Loc_Translate("Heroic route chosen. The ring is ready. Open the airlock when you are ready for the guardian arena.");
                    }
                    return Loc_Translate("Heroic route chosen. Open the airlock to start the guardian fight.");
                }
                if (tasks->selectedEndingRoute == ENDING_PEACEFUL) {
                    if (player->hasSignalAmplifier) {
                        return Loc_Translate("Peaceful route chosen. Carry the amplifier to the tower.");
                    }
                    return Loc_Translate("Peaceful route chosen. Craft the amplifier before leaving base.");
                }
                if (IsCrossX3Ready(tasks)) {
                    return Loc_Translate("Archive almost complete. One final sweep should let Loxi frame the ending choice with full context.");
                }
                if (IsCrossX2Ready(tasks)) {
                    return Loc_Translate("Main archive is now the last blocker before Loxi's rewritten ending picture becomes a real choice.");
                }
                if (IsCrossX1Ready(tasks)) {
                    return Loc_Translate("Aligned crew and facility evidence is ready. Keep bringing back mainline proof.");
                }
                return Loc_Translate("Final stage evidence sweep: collect truth first, choose an ending second.");
            }
            return Loc_Translate("Base is the safest place to recover, craft, and sync.");
        case MAP_AREA_FOREST:
            if (player->crouching) {
                return Loc_Translate("Crouching in forest reduces detection.");
            }
            return Loc_Translate("Use Crouch in forest for stealth.");
        case MAP_AREA_SWAMP_OUTER:
            if (std::strcmp(locationName, "Outer Swamp Rim") == 0) {
                return Loc_Translate("Outer Swamp Rim is safer; practice short runs and returns.");
            }
            if (std::strcmp(locationName, "Flooded Detour") == 0) {
                return Loc_Translate("Flooded Detour is longer. Rope and Camp help return.");
            }
            if (player->hasRope && player->hasFieldCamp) {
                return Loc_Translate("Use Rope shortcuts and Field Camp fallback.");
            }
            if (player->hasRope) {
                return Loc_Translate("Rope reduces route length and oxygen pressure.");
            }
            if (player->hasFieldCamp) {
                return Loc_Translate("Field Camp gives a safe fallback point.");
            }
            return Loc_Translate("Outer swamp drains oxygen steadily. Plan return route.");
        case MAP_AREA_SWAMP_DEEP:
            if (std::strcmp(locationName, "Deep Gate") == 0) {
                return Loc_Translate("Deep Gate is a good turn-back checkpoint.");
            }
            if (std::strcmp(locationName, "Deep Basin") == 0) {
                return Loc_Translate("Deep Basin is high risk. Keep suit and oxygen support ready.");
            }
            if (player->hasProtectionSuit) {
                return Loc_Translate("Protection Suit helps, but deep swamp is still dangerous.");
            }
            return Loc_Translate("Deep swamp needs Protection Suit and poison control.");
        case MAP_AREA_RUINS:
            if (tasks->stage >= 7) {
                if (std::strcmp(locationName, "Signal Tower Plateau") == 0) {
                    if (tasks->selectedEndingRoute == ENDING_PEACEFUL && player->hasSignalAmplifier) {
                        return Loc_Translate("Peaceful route chosen. Push for tower stabilization.");
                    }
                    if (tasks->selectedEndingRoute == ENDING_HEROIC && tasks->monolithsLit >= 3) {
                        return Loc_Translate("Heroic route chosen. This is the best timing for the tower push after the guardian falls.");
                    }
                    if (tasks->selectedEndingRoute == ENDING_NONE) {
                        if (Tasks_IsEndingBranchReady(tasks) && !tasks->endingArchiveReviewed) {
                            return Loc_Translate("Do not commit here blind. Return to Loxi and review the assembled archive first.");
                        }
                        return Loc_Translate("Do not commit here blind. Return to Loxi once the archive is complete.");
                    }
                    return Loc_Translate("Plateau is dangerous. Follow the chosen route's prep before committing.");
                }
                if (std::strcmp(locationName, "Monolith Ring") == 0) {
                    if (tasks->monolithsLit >= 3) {
                        return Loc_Translate("Monolith ring complete.");
                    }
                    if (tasks->monolithsLit > 0) {
                        return Loc_Translate("Light remaining monoliths.");
                    }
                    return Loc_Translate("Start lighting monoliths to weaken guardian.");
                }
            }
            if (std::strcmp(locationName, "Ruins Approach") == 0) {
                return Loc_Translate("Ruins Approach: prepare and decide push depth.");
            }
            if (IsTowerPlateau(player)) {
                return Loc_Translate("Tower area: keep oxygen margin and retreat plan.");
            }
            return Loc_Translate("Ruins have high oxygen pressure. Use short planned runs.");
        case MAP_AREA_BOSS_ARENA:
            if (tasks->bossDefeated) {
                return Loc_Translate("Guardian Arena is clear. Return to the ship and finish at the Signal Tower.");
            }
            if (tasks->monolithsLit >= 3) {
                return Loc_Translate("Guardian Arena: the monolith ring is already softening this fight. Keep oxygen margin for the last phase.");
            }
            return Loc_Translate("Guardian Arena: no route tricks remain. Stabilize oxygen and win the fight cleanly.");
        case MAP_AREA_UNKNOWN:
        default:
            return Loc_Translate("Watch oxygen, health, and retreat route.");
    }
}

const char *TasksContent_GetStageNameText(int stage) {
    const StageTextDef *stageText;

    stageText = FindStageTextDef(stage);
    if (stageText == NULL) {
        return Loc_Translate("Unknown Stage");
    }

    return Loc_PickText(stageText->stageName);
}

const char *TasksContent_GetPhaseNameText(DayPhase phase) {
    switch (phase) {
        case DAY_PHASE_DAY:
            return Loc_Translate("Day");
        case DAY_PHASE_DUSK:
            return Loc_Translate("Dusk");
        case DAY_PHASE_NIGHT:
            return Loc_Translate("Night");
        default:
            return Loc_Translate("Unknown");
    }
}

const char *TasksContent_GetEventNameText(EventType eventType) {
    switch (eventType) {
        case EVENT_HARVEST:
            return Loc_Translate("Resource Surge");
        case EVENT_CALM_BEASTS:
            return Loc_Translate("Quiet Creatures");
        case EVENT_CLEAR_SKY:
            return Loc_Translate("Clear Skies");
        case EVENT_SPORE_STORM:
            return Loc_Translate("Spore Storm");
        case EVENT_MONSTER_FRENZY:
            return Loc_Translate("Monster Frenzy");
        case EVENT_DEVICE_FAULT:
            return Loc_Translate("Device Fault");
        default:
            return Loc_Translate("None");
    }
}

const char *TasksContent_GetEndingTitleText(GameEnding ending) {
    const EndingTextDef *endingText;

    endingText = FindEndingTextDef(ending);
    if (endingText == NULL) {
        return Loc_Translate("");
    }

    return Loc_PickText(endingText->title);
}

const char *TasksContent_GetEndingBodyText(GameEnding ending) {
    const EndingTextDef *endingText;

    endingText = FindEndingTextDef(ending);
    if (endingText == NULL) {
        return Loc_Translate("");
    }

    return Loc_PickText(endingText->body);
}

const char *TasksContent_GetUIFontSampleText(void) {
    static std::string sample;

    if (sample.empty()) {
        for (const StageTextDef &entry : kStageTextDefs) {
            sample.append(entry.stageName.english);
            sample.push_back(' ');
            sample.append(entry.stageName.simplifiedChinese);
            sample.push_back(' ');
            sample.append(entry.defaultObjective.english);
            sample.push_back(' ');
            sample.append(entry.defaultObjective.simplifiedChinese);
            sample.push_back(' ');
        }
        for (const EndingTextDef &entry : kEndingTextDefs) {
            sample.append(entry.title.english);
            sample.push_back(' ');
            sample.append(entry.title.simplifiedChinese);
            sample.push_back(' ');
            sample.append(entry.body.english);
            sample.push_back(' ');
            sample.append(entry.body.simplifiedChinese);
            sample.push_back(' ');
        }
    }

    return sample.c_str();
}
