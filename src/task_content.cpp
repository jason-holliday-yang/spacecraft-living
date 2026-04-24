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
    {1, {"Air Triage", "呼吸止损"}, {"Recover the first ship archives, then repair the first oxygen module to raise the ship's oxygen ceiling.", "先回收第一批飞船档案，再修好第一组氧气模块，抬高飞船的供氧上限。"}},
    {2, {"Ship Archive", "船内归档"}, {"Recover the remaining ship archives, craft a Glow Stick, and finish the full oxygen repair that opens the east airlock.", "回收剩余船内档案，制作荧光棒，并完成会开启东侧气闸的最终氧气修复。"}},
    {3, {"Relay Opening", "中继开路"}, {"Run the east relay sortie, restore the comm relay, and unlock the first west-route archive lead.", "完成东侧中继行动，修复通讯中继，并解锁第一段西线档案线索。"}},
    {4, {"West Confirmation", "西线确认"}, {"Craft the Laser Gun and Protection Suit, inspect the crash clue, and prepare to recover the first west-route field archive.", "制作激光枪和防护服，调查坠毁线索，并准备回收西线的第一份现场档案。"}},
    {5, {"Power Recovery", "动力回收"}, {"Extract the Energy Core, restore the Power Bay, reopen the south-route facility trail, and clear the north ruins approach.", "拆出能源核心，恢复动力舱，重新打开南线设施档案路线，并打通北线遗迹入口。"}},
    {6, {"Ruins Synthesis", "遗迹拼图"}, {"Recover 3 Relic Fragments, keep advancing west and south archives, then sync the evidence at Loxi.", "收集 3 枚遗迹碎片，继续推进西线与南线档案，然后回洛希处同步证据。"}},
    {7, {"Archive Verdict", "档案定案"}, {"Recover the remaining logs, return to Loxi, and decide which ending the evidence supports.", "找回剩余日志，回到洛希处，并决定这些证据最终支撑哪个结局。"}}
}};

const std::array<EndingTextDef, 4> kEndingTextDefs = {{
    {ENDING_HEROIC, {"Heroic Rescue", "强行救援"}, {"The guardian fell, the Signal Tower gave way to manual control, and the beacon finally burned into the sky. Loxi confirmed the rescue call, but it was a departure bought by force, marked forever by what had to be broken to make it happen.", "守卫倒下后，信号塔终于服从人工控制，求救信标也随之刺入天际。洛希确认求援已经发出，但这是一场靠强行突破换来的离开，它留下的痕迹也会和启程本身一起被记住。"}},
    {ENDING_PEACEFUL, {"Peaceful Rescue", "和平救援"}, {"The amplifier taught human systems how to speak to the tower, and the signal rose without one last breach. Loxi confirmed the route held. You left a quieter world behind than the one a forced rescue would have scarred.", "信号放大器把人类系统嵌进了塔楼的语言，于是信号在没有最后强闯的情况下升起。洛希确认路线已经成立，而你离开时，这个世界也比被强行撕开时更完整、更安静。"}},
    {ENDING_SETTLEMENT, {"Alien Settlement", "异星定居"}, {"With the last rogue guardian put down, rescue stopped being the only future worth chasing. With Loxi, the ship, and the surviving systems, the wreck became a deliberate home and the beginning of a longer duty on this world.", "当最后一道失控守卫被清除后，求援不再是唯一值得追逐的未来。借助洛希、飞船和那些仍能维持的系统，这艘残骸被真正改造成了家，也成了你在这颗星球上继续承担下去的起点。"}},
    {ENDING_FAILURE, {"Failed Survival", "生存失败"}, {"The collapses came faster than repair could answer them. Air, power, and shelter never held long enough, and the expedition ended before this world could be understood, let alone survived on your terms.", "崩溃来得比修复更快。空气、电力与庇护始终没能维持得足够久，这场远征最终在你真正弄清这颗世界之前就先一步结束了。"}}
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

int CountCollectedLogsForCategory(const TaskSystem *tasks, ShipLogCategory category) {
    int count;
    int index;

    if (tasks == NULL) {
        return 0;
    }

    count = 0;
    for (index = 0; index < tasks->logCount; ++index) {
        if (tasks->logs[index].category == category && tasks->logs[index].collected) {
            count += 1;
        }
    }

    return count;
}

int CountTotalLogsForCategory(const TaskSystem *tasks, ShipLogCategory category) {
    int count;
    int index;

    if (tasks == NULL) {
        return 0;
    }

    count = 0;
    for (index = 0; index < tasks->logCount; ++index) {
        if (tasks->logs[index].category == category) {
            count += 1;
        }
    }

    return count;
}

bool AreAllMainlineLogsRecovered(const TaskSystem *tasks) {
    int totalCount;

    totalCount = CountTotalLogsForCategory(tasks, SHIP_LOG_MAINLINE);
    return totalCount > 0
        && CountCollectedLogsForCategory(tasks, SHIP_LOG_MAINLINE) >= totalCount;
}

enum {
    LOG_INDEX_SHIP_IMPACT_PROTOCOL = 0,
    LOG_INDEX_SHIP_SPLIT_ROSTER = 1,
    LOG_INDEX_SHIP_PATTERN_NOT_WILDERNESS = 2,
    LOG_INDEX_CANOPY_HANDOFF_RECORD = 5,
    LOG_INDEX_LAST_CAMP_TESTAMENT = 7,
    LOG_INDEX_VENT_CALIBRATION_HANDOVER = 10,
    LOG_INDEX_PURIFIER_RING_CONTROL_BRIEF = 12
};

static bool IsCollectedLogIndex(const TaskSystem *tasks, int logIndex) {
    return tasks != NULL
        && logIndex >= 0
        && logIndex < tasks->logCount
        && tasks->logs[logIndex].collected;
}

static bool IsHeroicEndingEvidenceReady(const TaskSystem *tasks) {
    return IsCollectedLogIndex(tasks, LOG_INDEX_CANOPY_HANDOFF_RECORD);
}

static bool IsPeacefulEndingEvidenceReady(const TaskSystem *tasks) {
    return IsCollectedLogIndex(tasks, LOG_INDEX_VENT_CALIBRATION_HANDOVER)
        || IsCollectedLogIndex(tasks, LOG_INDEX_PURIFIER_RING_CONTROL_BRIEF);
}

static bool IsPeacefulEndingChoiceBlockedByAmplifier(const TaskSystem *tasks) {
    return tasks != NULL
        && !tasks->signalAmplifierCrafted
        && IsPeacefulEndingEvidenceReady(tasks);
}

static bool IsSettlementEndingEvidenceReady(const TaskSystem *tasks) {
    return IsCollectedLogIndex(tasks, LOG_INDEX_LAST_CAMP_TESTAMENT);
}

static int CountCollectedShipIntroLogs(const TaskSystem *tasks) {
    int count = 0;

    count += IsCollectedLogIndex(tasks, LOG_INDEX_SHIP_IMPACT_PROTOCOL) ? 1 : 0;
    count += IsCollectedLogIndex(tasks, LOG_INDEX_SHIP_SPLIT_ROSTER) ? 1 : 0;
    count += IsCollectedLogIndex(tasks, LOG_INDEX_SHIP_PATTERN_NOT_WILDERNESS) ? 1 : 0;
    return count;
}

const char *GetNextWestArchiveLocation(const TaskSystem *tasks) {
    if (tasks == NULL || !tasks->westW1Completed) {
        return "West Frontier";
    }
    if (!tasks->westW2Completed) {
        return "Survey Break";
    }
    if (!tasks->westW3Completed) {
        return "Canopy Hollow";
    }
    if (!tasks->westW4Completed) {
        return "Echo Basin";
    }
    return "Last Camp";
}

const char *GetNextSouthArchiveLocation(const TaskSystem *tasks) {
    if (tasks == NULL || !tasks->southS1Completed) {
        return "South Collapse";
    }
    if (!tasks->southS2Completed) {
        return "Vent Galleries";
    }
    if (!tasks->southS3Completed) {
        return "Service Shafts";
    }
    if (!tasks->southS4Completed) {
        return "Purifier Ring";
    }
    return "Root Vault";
}

const char *FormatDualArchiveLocationMessage(const TaskSystem *tasks,
                                             const char *englishFormat,
                                             const char *simplifiedChineseFormat) {
    static std::string message;
    char buffer[512];

    std::snprintf(buffer,
                  sizeof(buffer),
                  Loc_PickLiteral(englishFormat, simplifiedChineseFormat),
                  Loc_GetLocationNameText(GetNextWestArchiveLocation(tasks)),
                  Loc_GetLocationNameText(GetNextSouthArchiveLocation(tasks)));
    message = buffer;
    return message.c_str();
}

bool HasEnergyCoreExtractionMaterials(const Player *player) {
    return player != NULL
        && player->resources[RESOURCE_JUNK_METAL] >= 1
        && player->resources[RESOURCE_PROTECTIVE_FIBER] >= 1
        && player->resources[RESOURCE_ENERGY_CRYSTAL] >= 1;
}

const Monster *FindActiveGuardian(const TaskSystem *tasks) {
    int index;

    if (tasks == NULL) {
        return NULL;
    }

    for (index = 0; index < tasks->monsterCount; ++index) {
        const Monster *monster = &tasks->monsters[index];

        if (monster->active && monster->type == MONSTER_FINAL_BOSS && monster->area == MAP_AREA_BOSS_ARENA) {
            return monster;
        }
    }

    return NULL;
}

int GetGuardianPhase(const Monster *boss) {
    const float healthPercent = boss != NULL && boss->maxHealth > 0.0f ? boss->health / boss->maxHealth : 1.0f;

    if (healthPercent > 0.70f) {
        return 1;
    }
    if (healthPercent > 0.35f) {
        return 2;
    }
    return 3;
}

const char *GetGuardianArenaHint(const TaskSystem *tasks) {
    const Monster *boss = FindActiveGuardian(tasks);

    if (tasks == NULL) {
        return Loc_PickLiteral("Northwest Ruins: stay mobile, protect oxygen, and punish only real openings.",
                               "西北遗迹守卫战：保持移动，稳住氧气，只在真正露出破绽时出手。");
    }
    if (tasks->bossDefeated) {
        return Loc_Translate("Northwest ruins are clear. Return to the ship and finish at the Signal Tower.");
    }
    if (boss == NULL) {
        if (tasks->monolithsLit >= 3) {
            return Loc_PickLiteral("Northwest Ruins: the monolith ring is already helping. Keep oxygen for the last phase and wait for a clean punish window.",
                                   "西北遗迹守卫战：石碑环已经在提供帮助。为最后阶段保留氧气，等真正干净的破绽再出手。");
        }
        return Loc_PickLiteral("Northwest Ruins: dodge charge and shockwave commits to expose the guardian's weak point, then strike before it recovers.",
                               "西北遗迹守卫战：躲开冲锋与震荡重招，逼出守卫弱点，再在它恢复前补刀。");
    }

    switch (GetGuardianPhase(boss)) {
        case 1:
            if (tasks->monolithsLit >= 3) {
                return Loc_PickLiteral("Northwest Ruins Phase 1: the lit ring is buying space. Bait charge or shockwave commits and do not overtrade early.",
                                       "西北遗迹守卫战一阶段：点亮的石碑环正在帮你争取空间。先引出冲锋或震荡重招，不要在前半段硬换血。");
            }
            return Loc_PickLiteral("Northwest Ruins Phase 1: stay patient, bait charge or shockwave commits, and learn the guardian's tempo before punishing.",
                                   "西北遗迹守卫战一阶段：先稳住节奏，引出冲锋或震荡重招，摸清守卫节拍后再惩罚。");
        case 2:
            if (tasks->monolithsLit >= 3) {
                return Loc_PickLiteral("Northwest Ruins Phase 2: reinforcements are entering, but the lit ring is extending your punish windows. Clear space, then punish the guardian cleanly.",
                                       "西北遗迹守卫战二阶段：增援已经开始进场，但完整石碑环会拉长你的处决窗口。先清出空间，再稳定惩罚守卫。");
            }
            return Loc_PickLiteral("Northwest Ruins Phase 2: relic-guard reinforcements can pin you down. Reposition first, then punish only after the guardian's heavy commits whiff.",
                                   "西北遗迹守卫战二阶段：遗迹守卫增援会逼你站位失衡。先重新拉开位置，再在首领重招落空后反打。");
        case 3:
        default:
            if (tasks->monolithsLit >= 3) {
                return Loc_PickLiteral("Northwest Ruins Final Phase: the ring is stretching the last weak-point windows. Save oxygen, survive the faster chain pressure, and end it on one clean punish.",
                                       "西北遗迹守卫战最终阶段：石碑环会把最后的弱点窗口再拉长一点。留好氧气，顶住更快的连段压力，用一次干净惩罚收掉它。");
            }
            return Loc_PickLiteral("Northwest Ruins Final Phase: there is no safe attrition left now. Keep oxygen in reserve, dodge the faster chain pressure, and finish on the next real weak-point opening.",
                                   "西北遗迹守卫战最终阶段：现在已经没有安全磨血空间了。保留氧气，躲开更快的连段压力，并在下一次真正露出弱点时结束战斗。");
    }
}

const char *FormatSingleEndingSummary(const TaskSystem *tasks,
                                      const char *englishFormat,
                                      const char *simplifiedChineseFormat) {
    static std::string message;
    char buffer[256];
    const char *endingTitle;

    endingTitle = Tasks_GetEndingTitle(Tasks_GetAvailableEndingAt(tasks, 0));
    std::snprintf(buffer,
                  sizeof(buffer),
                  Loc_PickLiteral(englishFormat, simplifiedChineseFormat),
                  endingTitle);
    message = buffer;
    return message.c_str();
}

const char *GetStage7PendingUnlockObjective(const TaskSystem *tasks, const Player *player) {
    if (tasks == NULL) {
        return Loc_Translate("Return to Loxi.");
    }

    if (IsPeacefulEndingChoiceBlockedByAmplifier(tasks)) {
        if (player != NULL && player->resources[RESOURCE_RELIC_FRAGMENT] >= 3) {
            return Loc_PickLiteral("Return to the workshop, craft the Signal Amplifier, then come back to Loxi to confirm peaceful rescue.",
                                   "先回到工坊制作信号放大器，再回洛希处确认和平救援。");
        }
        return Loc_PickLiteral("Recover 3 Relic Fragments, craft the Signal Amplifier at the workshop, then return to Loxi for peaceful rescue.",
                               "先找回 3 枚遗迹碎片，在工坊制作信号放大器，再回洛希处确认和平救援。");
    }
    if (!IsHeroicEndingEvidenceReady(tasks)) {
        return Loc_PickLiteral("Recover the Canopy Handoff record, then return to Loxi to confirm heroic rescue.",
                               "先补回林冠交接记录，再回洛希处确认强行救援。");
    }
    if (!IsPeacefulEndingEvidenceReady(tasks)) {
        return Loc_PickLiteral("Recover a south-facility archive control record, then return to Loxi to confirm peaceful rescue.",
                               "先补回一份南线设施控制记录，再回洛希处确认和平救援。");
    }
    if (!IsSettlementEndingEvidenceReady(tasks)) {
        return Loc_PickLiteral("Recover the Last Camp testament, then return to Loxi if you still want settlement on the table.",
                               "先补回最后营地遗言；如果你还想把定居保留下来，再回洛希处确认。");
    }
    if (!tasks->bossDefeated) {
        return Loc_PickLiteral("Defeat the guardian in the northwest ruins, then return to Loxi if you want settlement to become a real option.",
                               "先在西北遗迹击败守卫；如果你想让定居真正成为选项，再回洛希处确认。");
    }

    return Loc_PickLiteral("Return to Loxi and choose among the unlocked endings.",
                           "返回洛希处，在已解锁的结局中作出选择。");
}

const char *GetStage7EndingChoiceObjective(const TaskSystem *tasks, const Player *player) {
    const int availableEndingCount = Tasks_GetAvailableEndingCount(tasks);

    if (availableEndingCount <= 0) {
        return GetStage7PendingUnlockObjective(tasks, player);
    }
    if (availableEndingCount == 1) {
        return FormatSingleEndingSummary(tasks,
                                         "Return to Loxi and confirm the only unlocked ending: %s.",
                                         "返回洛希处，确认当前唯一已解锁的结局：%s。");
    }
    return Loc_PickLiteral("Return to Loxi and choose among the unlocked endings.",
                           "返回洛希处，在已解锁的结局中作出选择。");
}

const char *FormatStage7ArchiveHint(const TaskSystem *tasks,
                                    const char *englishDetail,
                                    const char *simplifiedChineseDetail) {
    (void) tasks;
    return Loc_PickLiteral(englishDetail, simplifiedChineseDetail);
}

const char *FormatStage7NamedArchiveHint(const TaskSystem *tasks,
                                         const char *englishFormat,
                                         const char *simplifiedChineseFormat) {
    return FormatDualArchiveLocationMessage(tasks, englishFormat, simplifiedChineseFormat);
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
    const int shipIntroLogsCollected = CountCollectedShipIntroLogs(tasks);

    if (tasks == NULL) {
        return Loc_Translate("Follow Loxi and keep exploring.");
    }

    stageText = FindStageTextDef(tasks->stage);
    if (tasks->stage == 1) {
        if (shipIntroLogsCollected < 3) {
            return Loc_PickLiteral("Recover the first ship archives in Cargo Hold, Crew Quarters, and Diagnostics, then repair the first oxygen module to raise the ship's oxygen ceiling.",
                                   "先回收货舱、船员舱和诊断舱里的第一批飞船档案，再修好第一组氧气模块，提高飞船的供氧上限。");
        }
        return Loc_PickLiteral("Repair the first oxygen module to raise the ship's oxygen ceiling and steady the situation before the next push.",
                               "修好第一组氧气模块，抬高飞船的供氧上限，也先把局面稳住，再准备下一步推进。");
    }
    if (tasks->stage == 2) {
        if (shipIntroLogsCollected < 3) {
            return Loc_PickLiteral("Recover the remaining ship archives, craft a Glow Stick, and finish the oxygen repair that opens the east airlock.",
                                   "回收剩余船内档案，制作荧光棒，并完成会开启东侧气闸的最终氧气修复。");
        }
        if (player != NULL && player->hasGlowStick) {
            return Loc_PickLiteral("Spend 1 Glow Moss and 1 Ore on the final oxygen repair, then open the east airlock for the first field archive run.",
                                   "用 1 份发光苔和 1 份矿石完成最终氧气修复，然后打开东侧气闸，开始第一段外勤档案回收。");
        }
        return Loc_PickLiteral("Craft a Glow Stick, spend 1 Glow Moss and 1 Ore on the final oxygen repair, then open the east airlock for the first field archive run.",
                               "制作荧光棒，用 1 份发光苔和 1 份矿石完成最终氧气修复，然后打开东侧气闸，开始第一段外勤档案回收。");
    }
    if (tasks->stage == 3) {
        return Loc_PickLiteral("Use the east airlock, gather relay materials, restore the comm relay, and let Loxi unlock West Frontier in Echo Wilds.",
                               "使用东侧气闸，收集中继修复材料，恢复通讯中继，让洛希解锁回响荒野中的西部前线。");
    }
    if (tasks->stage == 4) {
        if (player != NULL && (!player->hasLaserGun || !player->hasProtectionSuit)) {
            return Loc_PickLiteral("Craft the Laser Gun and Protection Suit first. Then inspect the Crash Forest wreck clue and begin recovering West Frontier in Echo Wilds.",
                                   "先制作激光枪和防护服，然后调查坠毁森林里的残骸线索，并开始回收回响荒野中的西部前线档案。");
        }
        return Loc_PickLiteral("Inspect the Crash Forest wreck clue, confirm the Deep Basin black-box trail, and begin recovering West Frontier in Echo Wilds.",
                               "调查坠毁森林里的残骸线索，确认深潭区的黑匣轨迹，并开始回收回响荒野中的西部前线档案。");
    }
    if (tasks->stage == 6) {
        if (player != NULL && player->resources[RESOURCE_RELIC_FRAGMENT] >= 3) {
            if (IsCrossX2Ready(tasks)) {
                return Loc_PickLiteral("Sync the fragments at Loxi and review how Echo Basin, Purifier Ring, and the Monolith Ring now reframe rescue, stabilization, and settlement.",
                                       "把碎片带回洛希处同步，并复核回声盆地、净化环区与石碑环区如何重新定义救援、稳定与定居。");
            }
            if (IsCrossX1Ready(tasks)) {
                return FormatDualArchiveLocationMessage(tasks,
                                                        "Bring the fragments back to Loxi so it can connect %s with %s into one clear account.",
                                                        "把碎片带回洛希处同步，让它把%s与%s连成一段清楚完整的来龙去脉。");
            }
            return Loc_PickLiteral("Sync fragments at Loxi terminal.",
                                   "在洛希终端同步碎片。");
        }
        if (IsCrossX2Ready(tasks)) {
            return FormatDualArchiveLocationMessage(tasks,
                                                    "Recover 3 Relic Fragments across Ruins Approach, Monolith Ring, and Signal Tower Plateau, keep advancing %s and %s, then bring everything back to Loxi so it can judge which ending truly fits the whole record.",
                                                    "去遗迹前沿、石碑环区和信号塔高台收集 3 枚遗迹碎片，继续推进%s与%s的档案，再把它们一起带回洛希，让它判断哪条终局路线才真正贴合整套记录。");
        }
        if (IsCrossX1Ready(tasks)) {
            return FormatDualArchiveLocationMessage(tasks,
                                                    "Recover 3 Relic Fragments across Ruins Approach, Monolith Ring, and Signal Tower Plateau, and keep filling %s and %s so Loxi can see how both trails fit together.",
                                                    "去遗迹前沿、石碑环区和信号塔高台收集 3 枚遗迹碎片，并继续补全%s与%s的档案，好让洛希看清两条线索究竟是怎样接上的。");
        }
    }
    if (tasks->stage == 7) {
        if (Tasks_IsEndingBranchReady(tasks) && tasks->selectedEndingRoute == ENDING_NONE) {
            if (!tasks->endingArchiveReviewed) {
                return Loc_PickLiteral("Return to Loxi and review the assembled archive before choosing the final route.",
                                       "返回洛希处，先复核整理好的档案，再选择最终路线。");
            }
            return GetStage7EndingChoiceObjective(tasks, player);
        }
        if (tasks->selectedEndingRoute == ENDING_HEROIC) {
            if (tasks->bossDefeated) {
                return Loc_PickLiteral("Heroic route chosen. Reach the Signal Tower and launch the rescue beacon.",
                                       "已选择强行救援路线。前往信号塔并发射求救信标。");
            }
            return Loc_PickLiteral("Heroic route chosen. Defeat the guardian in the northwest ruins, then return to the Signal Tower.",
                                   "已选择强行救援路线。先在西北遗迹击败守卫，再返回信号塔。");
        }
        if (tasks->selectedEndingRoute == ENDING_PEACEFUL) {
            if (player != NULL && player->hasSignalAmplifier) {
                return Loc_PickLiteral("Peaceful route chosen. Carry the Signal Amplifier to the Signal Tower.",
                                       "已选择和平救援路线。把信号放大器带到信号塔。");
            }
            if (player != NULL && player->resources[RESOURCE_RELIC_FRAGMENT] >= 3) {
                return Loc_PickLiteral("Peaceful route chosen. Craft the Signal Amplifier at the workshop, then carry it to the Signal Tower.",
                                       "已选择和平救援路线。先在工坊制作信号放大器，再把它带到信号塔。");
            }
            return Loc_PickLiteral("Peaceful route chosen. Recover 3 Relic Fragments, craft the Signal Amplifier at the workshop, then carry it to the Signal Tower.",
                                   "已选择和平救援路线。先找回 3 枚遗迹碎片，再在工坊制作信号放大器并把它带到信号塔。");
        }
        if (tasks->selectedEndingRoute == ENDING_SETTLEMENT) {
            if (tasks->bossDefeated) {
                return Loc_PickLiteral("Settlement route chosen. Return to Loxi and confirm that you will stay here.",
                                       "已选择定居路线。回到洛希处，确认你将留在这里。");
            }
            return Loc_PickLiteral("Settlement route chosen. Defeat the guardian in the northwest ruins before you confirm staying.",
                                   "已选择定居路线。先去西北遗迹击败守卫，再确认你要留下来。");
        }
        if (!Tasks_IsEndingBranchReady(tasks)) {
            if (AreAllMainlineLogsRecovered(tasks)) {
                return FormatStage7NamedArchiveHint(tasks,
                                                    "Main archive is complete. Finish %s and %s, then return to Loxi.",
                                                    "主线档案已经齐了。完成%s与%s的档案后，再回洛希处。");
            }
            if (IsCrossX3Ready(tasks)) {
                return FormatStage7ArchiveHint(tasks,
                                               "Last Camp and Root Vault are already archived. Recover the last mainline logs, then return to Loxi.",
                                               "最后营地与根脉核心已经归档。找回最后的主线日志后，再回洛希处。");
            }
            return FormatStage7NamedArchiveHint(tasks,
                                                "Recover %s, %s, and the remaining mainline logs before choosing an ending with Loxi.",
                                                "先补回%s、%s以及剩余主线日志，然后再回洛希处选择结局。");
        }
        return GetStage7EndingChoiceObjective(tasks, player);
    }
    if (tasks->stage == 5) {
        if (player != NULL && player->resources[RESOURCE_ENERGY_CORE] > 0) {
            return Loc_PickLiteral("Return to the Power Bay, install the extracted Energy Core, finish the ore reinforcement, and reopen South Collapse in the Subsurface Sink.",
                                   "返回动力舱，安装已经拆出的能源核心，完成矿石加固，并重新打开地下沉降带里的南部塌陷区。");
        }
        if (HasEnergyCoreExtractionMaterials(player)) {
            return Loc_PickLiteral("Return to the Crash Forest wreck, extract the Energy Core with the salvage bundle you assembled, then bring that proof back to the Power Bay.",
                                   "返回坠毁森林里的残骸，用你已经凑齐的打捞材料把能源核心拆出来，再把这份证明带回动力舱。");
        }
        return Loc_PickLiteral("Gather 1 Junk Metal, 1 Protective Fiber, and 1 Energy Crystal around Flooded Detour and Deep Basin, then return to the Crash Forest wreck to extract the Energy Core and open South Collapse.",
                               "前往积水绕路和深潭区收集 1 份废旧金属、1 份防护纤维和 1 份能量晶体，然后回到坠毁森林残骸拆出能源核心，并打开南部塌陷区。");
    }

    if (stageText != NULL) {
        return Loc_PickText(stageText->defaultObjective);
    }

    return Loc_Translate("Follow Loxi and keep exploring.");
}

const char *TasksContent_GetStageGuidance(const TaskSystem *tasks, const Player *player) {
    MapArea area;
    const char *locationName;
    const int shipIntroLogsCollected = CountCollectedShipIntroLogs(tasks);

    if (tasks == NULL) {
        return Loc_Translate("Recover vitals and move to the next objective.");
    }

    area = player != NULL ? Map_GetAreaAt(player->gridX, player->gridY) : MAP_AREA_UNKNOWN;
    locationName = player != NULL ? Map_GetLocationNameAt(player->gridX, player->gridY) : "Unknown Area";
    if (std::strcmp(locationName, "West Frontier") == 0) {
        if (!IsWestRouteAvailable(tasks)) {
            return Loc_Translate("West Frontier is still sealed. Restore the comm relay first.");
        }
        if (tasks->westW5Completed) {
            return Loc_Translate("The west archive trail is complete.");
        }
        if (tasks->westW5Started) {
            return Loc_Translate("Finish the Last Camp investigation, then return to base.");
        }
        if (tasks->westW4Completed) {
            return Loc_Translate("Echo Basin is archived. Move on to the Last Camp investigation.");
        }
        if (tasks->westW4Started) {
            return Loc_Translate("Finish the Echo Basin investigation, then return to base.");
        }
        if (tasks->westW3Completed) {
            return Loc_Translate("Canopy Hollow is archived. Move on to the Echo Basin investigation.");
        }
        if (tasks->westW3Started) {
            return Loc_Translate("Finish the Canopy Hollow investigation, then return to base.");
        }
        if (tasks->westW2Completed) {
            return Loc_Translate("Survey Break is archived. Move on to the Canopy Hollow investigation.");
        }
        if (tasks->westW2Started) {
            return Loc_Translate("Finish the Survey Break investigation, then return to base.");
        }
        if (tasks->westW1Completed) {
            return Loc_Translate("West Frontier is archived. Move on to Survey Break.");
        }
        if (tasks->westW1Started) {
            return Loc_Translate("Finish the West Frontier investigation, then return to base.");
        }
        return Loc_Translate("The west trail is open. Complete the first West Frontier investigation and report back to base.");
    }
    if (std::strcmp(locationName, "Survey Break") == 0) {
        if (!IsWestRouteAvailable(tasks)) {
            return Loc_Translate("Survey Break is still sealed. Restore the comm relay first.");
        }
        if (tasks->westW3Completed) {
            return Loc_Translate("The west trail is archived through Echo Basin.");
        }
        if (tasks->westW3Started) {
            return Loc_Translate("The Canopy Hollow investigation is active. Finish it here, then return.");
        }
        if (tasks->westW2Completed) {
            return Loc_Translate("Survey Break is archived. Continue to Canopy Hollow.");
        }
        if (tasks->westW2Started) {
            return Loc_Translate("Finish the Survey Break objectives, then return to base.");
        }
        if (tasks->westW1Completed) {
            return Loc_Translate("The Survey Break investigation is active here. Finish it, then return.");
        }
        return Loc_Translate("Complete the West Frontier investigation first.");
    }
    if (std::strcmp(locationName, "Canopy Hollow") == 0) {
        if (!IsWestRouteAvailable(tasks)) {
            return Loc_Translate("Canopy Hollow is still sealed. Restore the comm relay first.");
        }
        if (tasks->westW5Completed) {
            return Loc_Translate("The west trail is fully archived.");
        }
        if (tasks->westW5Started) {
            return Loc_Translate("The Last Camp investigation is active. Finish it, then return.");
        }
        if (tasks->westW4Completed) {
            return Loc_Translate("Echo Basin is archived. Continue to Last Camp.");
        }
        if (tasks->westW4Started) {
            return Loc_Translate("The Echo Basin investigation is active. Finish it, then return.");
        }
        if (tasks->westW3Completed) {
            return Loc_Translate("Canopy Hollow is archived. Continue to Echo Basin.");
        }
        if (tasks->westW3Started) {
            return Loc_Translate("Finish the Canopy Hollow investigation here, then return to base.");
        }
        if (tasks->westW2Completed) {
            return Loc_Translate("The Canopy Hollow investigation is active here. Finish it, then return.");
        }
        return Loc_Translate("Complete the Survey Break investigation first.");
    }
    if (std::strcmp(locationName, "Echo Basin") == 0) {
        if (!IsWestRouteAvailable(tasks)) {
            return Loc_Translate("Echo Basin is still sealed. Restore the comm relay first.");
        }
        if (tasks->westW5Completed) {
            return Loc_Translate("The west trail is fully archived.");
        }
        if (tasks->westW5Started) {
            return Loc_Translate("The Last Camp investigation is active. Finish it, then return.");
        }
        if (tasks->westW4Completed) {
            return Loc_Translate("Echo Basin is archived. Continue to Last Camp.");
        }
        if (tasks->westW4Started) {
            return Loc_Translate("Finish the Echo Basin investigation here, then return to base.");
        }
        if (tasks->westW3Completed) {
            return Loc_Translate("The Echo Basin investigation is active here. Finish it, then return.");
        }
        return Loc_Translate("Complete the Canopy Hollow investigation first.");
    }
    if (std::strcmp(locationName, "Last Camp") == 0) {
        if (!IsWestRouteAvailable(tasks)) {
            return Loc_Translate("Last Camp is still sealed. Restore the comm relay first.");
        }
        if (tasks->westW5Completed) {
            return Loc_Translate("Last Camp is archived.");
        }
        if (tasks->westW5Started) {
            return Loc_Translate("Finish the Last Camp investigation here, then return to base.");
        }
        if (tasks->westW4Completed) {
            return Loc_Translate("The Last Camp investigation is active here. Finish it, then return.");
        }
        return Loc_Translate("Complete the Echo Basin investigation first.");
    }
    if (std::strcmp(locationName, "South Collapse") == 0) {
        if (!IsSouthRouteAvailable(tasks)) {
            return Loc_Translate("South Collapse is still sealed. Restore the Power Bay first.");
        }
        if (tasks->southS5Completed) {
            return Loc_Translate("The south archive trail is complete.");
        }
        if (tasks->southS5Started) {
            return Loc_Translate("Finish the Root Vault investigation, then return to base.");
        }
        if (tasks->southS4Completed) {
            return Loc_Translate("Purifier Ring is archived. Move on to the Root Vault investigation.");
        }
        if (tasks->southS4Started) {
            return Loc_Translate("Finish the Purifier Ring investigation, then return to base.");
        }
        if (tasks->southS3Completed) {
            return Loc_Translate("Service Shafts are archived. Move on to the Purifier Ring investigation.");
        }
        if (tasks->southS3Started) {
            return Loc_Translate("Finish the Service Shaft investigation, then return to base.");
        }
        if (tasks->southS2Completed) {
            return Loc_Translate("Vent Galleries are archived. Move on to the Service Shaft investigation.");
        }
        if (tasks->southS2Started) {
            return Loc_Translate("Finish the Vent Galleries investigation, then return to base.");
        }
        if (tasks->southS1Completed) {
            return Loc_Translate("South Collapse is archived. Move on to Vent Galleries.");
        }
        if (tasks->southS1Started) {
            return Loc_Translate("Finish the South Collapse investigation, then return to base.");
        }
        return Loc_Translate("The south trail is open. Complete the first South Collapse investigation and report back to base.");
    }
    if (std::strcmp(locationName, "Vent Galleries") == 0) {
        if (!IsSouthRouteAvailable(tasks)) {
            return Loc_Translate("Vent Galleries are still sealed. Restore the Power Bay first.");
        }
        if (tasks->southS3Completed) {
            return Loc_Translate("The south trail is archived through the Service Shafts.");
        }
        if (tasks->southS3Started) {
            return Loc_Translate("The Service Shafts investigation is active. Finish it here, then return.");
        }
        if (tasks->southS2Completed) {
            return Loc_Translate("Vent Galleries are archived. Continue to the Service Shafts.");
        }
        if (tasks->southS2Started) {
            return Loc_Translate("Finish the Vent Galleries investigation here, then return to base.");
        }
        if (tasks->southS1Completed) {
            return Loc_Translate("The Vent Galleries investigation is active here. Finish it, then return.");
        }
        return Loc_Translate("Complete the South Collapse investigation first.");
    }
    if (std::strcmp(locationName, "Service Shafts") == 0) {
        if (!IsSouthRouteAvailable(tasks)) {
            return Loc_Translate("Service Shafts are still sealed. Restore the Power Bay first.");
        }
        if (tasks->southS5Completed) {
            return Loc_Translate("The south trail is fully archived.");
        }
        if (tasks->southS5Started) {
            return Loc_Translate("The Root Vault investigation is active. Finish it, then return.");
        }
        if (tasks->southS4Completed) {
            return Loc_Translate("Purifier Ring is archived. Continue to Root Vault.");
        }
        if (tasks->southS4Started) {
            return Loc_Translate("The Purifier Ring investigation is active. Finish it, then return.");
        }
        if (tasks->southS3Completed) {
            return Loc_Translate("Service Shafts are archived. Continue to Purifier Ring.");
        }
        if (tasks->southS3Started) {
            return Loc_Translate("Finish the Service Shafts investigation here, then return to base.");
        }
        if (tasks->southS2Completed) {
            return Loc_Translate("The Service Shafts investigation is active here. Finish it, then return.");
        }
        return Loc_Translate("Complete the Vent Galleries investigation first.");
    }
    if (std::strcmp(locationName, "Purifier Ring") == 0) {
        if (!IsSouthRouteAvailable(tasks)) {
            return Loc_Translate("Purifier Ring is still sealed. Restore the Power Bay first.");
        }
        if (tasks->southS5Completed) {
            return Loc_Translate("The south trail is fully archived.");
        }
        if (tasks->southS5Started) {
            return Loc_Translate("The Root Vault investigation is active. Finish it, then return.");
        }
        if (tasks->southS4Completed) {
            return Loc_Translate("Purifier Ring is archived. Continue to Root Vault.");
        }
        if (tasks->southS4Started) {
            return Loc_Translate("Finish the Purifier Ring investigation here, then return to base.");
        }
        if (tasks->southS3Completed) {
            return Loc_Translate("The Purifier Ring investigation is active here. Finish it, then return.");
        }
        return Loc_Translate("Complete the Service Shaft investigation first.");
    }
    if (std::strcmp(locationName, "Root Vault") == 0) {
        if (!IsSouthRouteAvailable(tasks)) {
            return Loc_Translate("Root Vault is still sealed. Restore the Power Bay first.");
        }
        if (tasks->southS5Completed) {
            return Loc_Translate("Root Vault is archived.");
        }
        if (tasks->southS5Started) {
            return Loc_Translate("Finish the Root Vault investigation here, then return to base.");
        }
        if (tasks->southS4Completed) {
            return Loc_Translate("The Root Vault investigation is active here. Finish it, then return.");
        }
        return Loc_Translate("Complete the Purifier Ring investigation first.");
    }

    switch (tasks->stage) {
        case 1:
            if (shipIntroLogsCollected < 3) {
                return Loc_PickLiteral("Loxi wants the opening ship archive set first: Cargo Hold, Crew Quarters, and Diagnostics. Then stabilize the first oxygen module.",
                                       "洛希现在最需要的是开场飞船档案：货舱、船员舱和诊断舱。收完之后，再稳住第一组氧气模块。");
            }
            return Loc_PickLiteral("The opening ship archive now explains the beginning clearly enough. Finish the first oxygen repair so the run stops bleeding breathing margin.",
                                   "开场飞船档案现在已经足够把开头说清楚。先完成第一组氧气修复，别再让这趟推进继续消耗呼吸余量。");
        case 2:
            if (shipIntroLogsCollected < 3) {
                return Loc_PickLiteral("Do not leave blind yet. Finish the remaining ship archives, then use Glow Moss, Ore, and a Glow Stick to complete the oxygen repair.",
                                       "先别带着残缺档案离船。把剩余船内记录补齐，再用发光苔、矿石和荧光棒完成最终氧气修复。");
            }
            return Loc_PickLiteral("The ship archive batch is stable. Finish the oxygen repair, open the east airlock, and start the first field archive route.",
                                   "船内档案这一批已经稳定。完成氧气修复，打开东侧气闸，开始第一段外勤档案路线。");
        case 3:
            return area == MAP_AREA_BASE
                ? "Use the east airlock, repair the comm relay, and reopen the West Frontier archive trail in Echo Wilds."
                : (std::strcmp(locationName, "Outer Swamp Rim") == 0
                    ? "Gather Vine, Shell Fruit, and Fungus on Outer Swamp Rim for the relay. This sortie is what unlocks West Frontier in Echo Wilds."
                    : "Stay on the relay route. This east run exists to reopen archive tracking, not to drift into the deeper swamp.");
        case 4:
            return area == MAP_AREA_BASE
                ? "Craft the Laser Gun and Protection Suit. Loxi wants the Crash Forest wreck clue and the newly reopened West Frontier archive next."
                : "Confirm the Crash Forest wreck clue in the east, then start recovering West Frontier in Echo Wilds.";
        case 5:
            if (area == MAP_AREA_BASE) {
                if (player != NULL && player->resources[RESOURCE_ENERGY_CORE] > 0) {
                    return Loc_PickLiteral("Install the extracted Energy Core in the power bay, finish the ore bracing, and reopen South Collapse in the Subsurface Sink.",
                                           "把拆出的能源核心装进动力舱，完成矿石支架加固，并重新打开地下沉降带里的南部塌陷区。");
                }
                if (HasEnergyCoreExtractionMaterials(player)) {
                    return Loc_PickLiteral("Your salvage bundle is ready. Return to the Crash Forest wreck, pull the Energy Core free, and convert the east run into South Collapse progress.",
                                           "你的打捞材料已经齐了。回到坠毁森林残骸，把能源核心拆出来，并把这次东线成果转成南部塌陷区推进。");
                }
                return Loc_PickLiteral("Prepare the suit, camp, and oxygen support, then gather extraction materials through Flooded Detour and Deep Basin that will reopen South Collapse.",
                                       "先准备好防护服、营地和补氧支援，再穿过积水绕路和深潭区收集会重新打开南部塌陷区的提取材料。");
            }
            if (area == MAP_AREA_SWAMP_DEEP) {
                if (HasEnergyCoreExtractionMaterials(player)) {
                    return Loc_PickLiteral("You already have the extraction materials. Do not overstay Deep Basin; return to the Crash Forest wreck now.",
                                           "你已经拿到提取材料了，不要继续在深潭区逗留，立刻回坠毁森林残骸。");
                }
                return Loc_PickLiteral("This Deep Basin run is for wreck-extraction materials. Bring them home, then return to the Crash Forest wreck for the core.",
                                       "这次深入深潭区的目标是残骸提取材料。把材料带回去，然后再回坠毁森林残骸拆核心。");
            }
            return Loc_PickLiteral("Use camp support and oxygen supplies to manage the eastern salvage runs.",
                                   "用营地支援和补氧手段去管理这几次东线打捞往返。");
        case 6:
            if (area == MAP_AREA_BASE) {
                if (player != NULL && player->resources[RESOURCE_RELIC_FRAGMENT] >= 3) {
                    if (IsCrossX2Ready(tasks)) {
                        return Loc_PickLiteral("The fragments are ready. Loxi can now piece together what Echo Basin, Purifier Ring, and the Monolith Ring are really pointing toward.",
                                               "碎片已经齐了。洛希现在可以把回声盆地、净化环区与石碑环区真正指向的东西拼出来。");
                    }
                    if (IsCrossX1Ready(tasks)) {
                        return FormatDualArchiveLocationMessage(tasks,
                                                                "The fragments are ready. Loxi can now align %s with %s.",
                                                                "碎片已经齐了。洛希现在可以把%s与%s对齐起来。");
                    }
                    return Loc_Translate("Sync fragments at Loxi terminal.");
                }
                if (IsCrossX2Ready(tasks)) {
                    return FormatDualArchiveLocationMessage(tasks,
                                                            "Late-route evidence is open. Recover fragments across Ruins Approach, Monolith Ring, and Signal Tower Plateau, then keep filling %s and %s so Loxi can finish reading what the ending truly asks of you.",
                                                            "终局后段的证据已经展开。去遗迹前沿、石碑环区和信号塔高台收齐碎片，并继续补完%s与%s的档案，好让洛希读清结局真正要求你做出的回答。");
                }
                if (IsCrossX1Ready(tasks)) {
                    return FormatDualArchiveLocationMessage(tasks,
                                                            "Shared evidence is unlocked. Recover fragments across Ruins Approach, Monolith Ring, and Signal Tower Plateau, then keep pushing %s and %s so Loxi can see how both trails fit together.",
                                                            "共享证据已经开启。去遗迹前沿、石碑环区和信号塔高台收齐碎片，并继续推进%s与%s的档案，好让洛希看清两条线索怎样拼成一件事。");
                }
                return Loc_PickLiteral("Collect 3 Relic Fragments across Ruins Approach, Monolith Ring, and Signal Tower Plateau. Those fragments and the route archives are what turn the endgame into a decision you can actually make.",
                                       "去遗迹前沿、石碑环区和信号塔高台收集 3 枚遗迹碎片。真正让终局变成可以落下去的选择的，是这些碎片和路线档案一起拼出来的结果。");
            }
            if (area == MAP_AREA_RUINS) {
                if (IsCrossX2Ready(tasks)) {
                    return Loc_PickLiteral("The ruins now hold enough evidence for Loxi to piece the last route together. Finish the fragment set and return.",
                                           "遗迹里的证据如今已经足够让洛希把最后的路线拼起来了。收齐碎片后返回。");
                }
                return Loc_Translate("Recover the fragments in the ruins, then return to base.");
            }
            return Loc_Translate("Recover the fragments, then sync with Loxi.");
        case 7:
            if (area == MAP_AREA_BASE) {
                if (Tasks_IsEndingBranchReady(tasks) && tasks->selectedEndingRoute == ENDING_NONE) {
                    if (!tasks->endingArchiveReviewed) {
                        return Loc_Translate("The whole archive is finally in one place. Return to Loxi and review what the west, south, and mainline evidence now mean before you choose.");
                    }
                    if (Tasks_GetAvailableEndingCount(tasks) <= 0) {
                        return GetStage7PendingUnlockObjective(tasks, player);
                    }
                    if (Tasks_GetAvailableEndingCount(tasks) == 1) {
                        return FormatSingleEndingSummary(tasks,
                                                         "The assembled archive now supports only %s. Loxi can confirm that ending immediately at the terminal.",
                                                         "拼齐后的档案如今只支撑“%s”这一种结局，洛希现在就能在终端为你确认它。");
                    }
                    return Loc_PickLiteral("The assembled archive now supports multiple endings. Loxi can let you choose among them immediately at the terminal.",
                                           "拼齐后的档案如今已经支撑起多个结局，洛希现在就能在终端让你作出选择。");
                }
                if (tasks->selectedEndingRoute == ENDING_HEROIC) {
                    if (tasks->bossDefeated) {
                        return Loc_Translate("Heroic route chosen. Head to the Signal Tower.");
                    }
                    if (tasks->monolithsLit >= 3) {
                        return Loc_PickLiteral("Heroic route chosen. The ring is fully lit. Push into the northwest ruins when you are ready for the guardian and its longer weak-point windows.",
                                               "已选择强行救援路线。石碑环已完全点亮。准备好后就推进西北遗迹，迎战守卫，并利用更长的弱点窗口。");
                    }
                    if (tasks->monolithsLit > 0) {
                        return Loc_PickLiteral("Heroic route chosen. Your monolith prep carries into the northwest ruins. Push there when you are ready to bait openings and punish the guardian cleanly.",
                                               "已选择强行救援路线。你的石碑准备会继续带进西北遗迹。准备好后就推进过去，引出破绽并稳定惩罚守卫。");
                    }
                    return Loc_PickLiteral("Heroic route chosen. Hunt the guardian in the northwest ruins before you touch the tower.",
                                           "已选择强行救援路线。先去西北遗迹猎杀守卫，再触碰塔楼。");
                }
                if (tasks->selectedEndingRoute == ENDING_PEACEFUL) {
                    if (player != NULL && player->hasSignalAmplifier) {
                        return Loc_Translate("Peaceful route chosen. Take the Signal Amplifier to the tower.");
                    }
                    if (player != NULL && player->resources[RESOURCE_RELIC_FRAGMENT] >= 3) {
                        return Loc_Translate("Peaceful route chosen. Craft the Signal Amplifier at the workshop before the tower run.");
                    }
                    return Loc_Translate("Peaceful route chosen. Recover 3 Relic Fragments, then craft the Signal Amplifier at the workshop before the tower run.");
                }
                if (tasks->selectedEndingRoute == ENDING_SETTLEMENT) {
                    if (tasks->bossDefeated) {
                        return Loc_PickLiteral("Settlement route chosen. The guardian is gone, so return to Loxi whenever you are ready to confirm staying.",
                                               "已选择定居路线。守卫已经清除，等你准备好时就回洛希处确认留下来。");
                    }
                    return Loc_PickLiteral("Settlement route chosen. Defeat the guardian in the northwest ruins before you confirm staying.",
                                           "已选择定居路线。先去西北遗迹击败守卫，再确认你要留下来。");
                }
                if (!AreAllMainlineLogsRecovered(tasks)) {
                    if (IsCrossX3Ready(tasks)) {
                        return FormatStage7ArchiveHint(tasks,
                                                       "Last Camp and Root Vault are already archived. Sweep the last missing ship logs, then bring the full record back to Loxi.",
                                                       "最后营地与根脉核心已经归档。把最后缺失的飞船日志找齐，再带着完整记录回洛希处。");
                    }
                    if (IsCrossX2Ready(tasks)) {
                        return FormatStage7NamedArchiveHint(tasks,
                                                            "Late-route evidence is ready around %s and %s, but the ship record is still incomplete. Recover the remaining mainline logs before the final branch opens.",
                                                            "后段证据已经在%s与%s就位，但飞船记录仍不完整。先找回剩余主线日志，再开启最终分支。");
                    }
                    if (IsCrossX1Ready(tasks)) {
                        return FormatStage7NamedArchiveHint(tasks,
                                                            "Cross-route evidence is starting to align between %s and %s. Keep recovering the missing mainline ship logs so Loxi can finish laying out what happened.",
                                                            "交叉线索已经开始在%s与%s之间对齐。继续找回缺失的主线飞船日志，让洛希把整件事的来龙去脉补齐。");
                    }
                    return FormatStage7ArchiveHint(tasks,
                                                   "This is the final archive sweep. Recover every missing mainline ship log, then return to Loxi for the last review.",
                                                   "现在就是最后的档案清扫。找回所有缺失的主线飞船日志，然后回洛希处做最终复核。");
                }
                if (IsCrossX3Ready(tasks)) {
                    return FormatStage7ArchiveHint(tasks,
                                                   "Main archive is complete. Last Camp and Root Vault are filed, so return to Loxi after your last archive check.",
                                                   "主线档案已经齐了。最后营地与根脉核心都已归档，做完最后一次档案核查后就回洛希处。");
                }
                if (IsCrossX2Ready(tasks)) {
                    return FormatStage7NamedArchiveHint(tasks,
                                                        "Main archive is complete. Finish the late archive steps at %s and %s so Loxi can open the final branch.",
                                                        "主线档案已经齐了。完成%s与%s的后段档案步骤后，洛希才会开启最终分支。");
                }
                if (IsCrossX1Ready(tasks)) {
                    return FormatStage7NamedArchiveHint(tasks,
                                                        "Main archive is complete. Finish %s and %s so both trails finally lock into one conclusion.",
                                                        "主线档案已经齐了。完成%s与%s后，两条线索才会真正锁成同一个结论。");
                }
                return FormatStage7NamedArchiveHint(tasks,
                                                    "Main archive is complete. Finish %s and %s before choosing your ending here.",
                                                    "主线档案已经齐了。完成%s与%s后，才能在这里选择结局。");
            }
            if (tasks->selectedEndingRoute == ENDING_HEROIC) {
                if (tasks->bossDefeated) {
                    return Loc_Translate("The tower is ready for the chosen heroic ending.");
                }
                if (area == MAP_AREA_BOSS_ARENA) {
                    if (tasks->monolithsLit >= 3) {
                        return Loc_PickLiteral("Heroic route selected. The guardian is cornered in the northwest ruins, and the fully lit ring is weakening it.",
                                               "已选择强行救援路线。守卫已被逼在西北遗迹内，而完整点亮的石碑环正在削弱它。");
                    }
                    return Loc_PickLiteral("Heroic route selected. The guardian is in front of you now. Finish the fight in the northwest ruins, then return to the tower.",
                                           "已选择强行救援路线。守卫此刻就在你面前。先在西北遗迹结束这场战斗，再回到塔楼。");
                }
                if (tasks->monolithsLit >= 3) {
                    return Loc_PickLiteral("Heroic route selected. The guardian is waiting in the northwest ruins, and the ring is already weakening it. Push there when ready.",
                                           "已选择强行救援路线。守卫正在西北遗迹等待，而石碑环已经在削弱它。准备好后就推进过去。");
                }
                return Loc_PickLiteral("Heroic route selected. Hunt the guardian in the northwest ruins.",
                                       "已选择强行救援路线。去西北遗迹猎杀守卫。");
            }
            if (tasks->selectedEndingRoute == ENDING_PEACEFUL) {
                if (player != NULL && player->hasSignalAmplifier) {
                    return Loc_Translate("Peaceful route selected. Bring the Signal Amplifier to the tower.");
                }
                if (player != NULL && player->resources[RESOURCE_RELIC_FRAGMENT] >= 3) {
                    return Loc_Translate("Peaceful route selected. Craft the Signal Amplifier at the workshop first.");
                }
                return Loc_Translate("Peaceful route selected. Recover 3 Relic Fragments, then craft the Signal Amplifier at the workshop.");
            }
            if (tasks->selectedEndingRoute == ENDING_SETTLEMENT) {
                if (tasks->bossDefeated) {
                    return Loc_PickLiteral("Settlement route selected. Return to Loxi and close the archive as a deliberate choice to stay.",
                                           "已选择定居路线。回到洛希处，以主动留下来的决定收束整份档案。");
                }
                return Loc_PickLiteral("Settlement route selected. The guardian still blocks any safe future here. Clear the northwest ruins first.",
                                       "已选择定居路线。守卫仍阻挡着这里的安全未来。先清理西北遗迹。");
            }
            if (Tasks_IsEndingBranchReady(tasks)) {
                if (!tasks->endingArchiveReviewed) {
                    return Loc_Translate("The archive is assembled, but Loxi still needs a final review at the ship before any ending route becomes a deliberate commitment.");
                }
                return GetStage7EndingChoiceObjective(tasks, player);
            }
            if (!AreAllMainlineLogsRecovered(tasks)) {
                if (IsCrossX3Ready(tasks)) {
                    return FormatStage7ArchiveHint(tasks,
                                                   "Last Camp and Root Vault are already filed. Keep hunting the missing mainline ship logs before you head back to Loxi.",
                                                   "最后营地与根脉核心已经归档。继续追查缺失的主线飞船日志，再回洛希处。");
                }
                if (IsCrossX2Ready(tasks)) {
                    return FormatStage7NamedArchiveHint(tasks,
                                                        "You already have the late-route comparison pieces at %s and %s. The remaining blocker is the missing mainline ship record.",
                                                        "你已经拿到%s与%s的后段对照线索。现在真正卡住你的，是还没找回的主线飞船记录。");
                }
                if (IsCrossX1Ready(tasks)) {
                    return FormatStage7NamedArchiveHint(tasks,
                                                        "The archive trail is beginning to line up between %s and %s. Keep recovering the missing ship logs so Loxi can close the archive cleanly.",
                                                        "档案线索已经开始在%s与%s之间吻合。继续找回缺失的飞船日志，让洛希完整收束档案。");
                }
                return FormatStage7ArchiveHint(tasks,
                                               "Keep sweeping the remaining mainline ship logs before the ending route can open.",
                                               "继续扫清剩余的主线飞船日志，最终路线才会开启。");
            }
            return FormatStage7NamedArchiveHint(tasks,
                                                "Main archive is complete. Finish %s and %s before the final branch opens.",
                                                "主线档案已经齐了。完成%s与%s后，最终分支才会开启。");
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
            return Loc_Translate("The west trail stays sealed until the comm relay is restored.");
        }
        if (tasks->westW5Completed) {
            return Loc_Translate("The west archive trail is complete.");
        }
        if (tasks->westW5Started) {
            return Loc_Translate("Last Camp investigation in progress. Finish it and return.");
        }
        if (tasks->westW4Completed) {
            return Loc_Translate("Echo Basin is archived. Move on to Last Camp.");
        }
        if (tasks->westW4Started) {
            return Loc_Translate("Echo Basin investigation in progress. Finish it and return.");
        }
        if (tasks->westW3Completed) {
            return Loc_Translate("Canopy Hollow is archived. Move on to Echo Basin.");
        }
        if (tasks->westW3Started) {
            return Loc_Translate("Canopy Hollow investigation in progress. Finish it and return.");
        }
        if (tasks->westW2Completed) {
            return Loc_Translate("Survey Break is archived. Move on to Canopy Hollow.");
        }
        if (tasks->westW2Started) {
            return Loc_Translate("Survey Break investigation in progress. Finish it and return.");
        }
        if (tasks->westW1Completed) {
            return Loc_Translate("West Frontier is archived. Start Survey Break.");
        }
        return Loc_Translate("West Frontier investigation in progress. Complete it and return.");
    }
    if (std::strcmp(locationName, "Survey Break") == 0) {
        if (!IsWestRouteAvailable(tasks)) {
            return Loc_Translate("Survey Break stays sealed until the comm relay is restored.");
        }
        if (tasks->westW3Completed) {
            return Loc_Translate("The west trail is archived through Echo Basin.");
        }
        if (tasks->westW3Started) {
            return Loc_Translate("The Canopy Hollow investigation is active. Finish it and return.");
        }
        if (tasks->westW2Completed) {
            return Loc_Translate("Survey Break is archived. Proceed to Canopy Hollow.");
        }
        if (tasks->westW2Started) {
            return Loc_Translate("Survey Break investigation in progress.");
        }
        if (tasks->westW1Completed) {
            return Loc_Translate("The Survey Break investigation is active here now that West Frontier is archived.");
        }
        return Loc_Translate("Complete the West Frontier investigation first.");
    }
    if (std::strcmp(locationName, "Canopy Hollow") == 0) {
        if (!IsWestRouteAvailable(tasks)) {
            return Loc_Translate("Canopy Hollow stays sealed until the comm relay is restored.");
        }
        if (tasks->westW5Completed) {
            return Loc_Translate("The west trail is fully archived.");
        }
        if (tasks->westW5Started) {
            return Loc_Translate("The Last Camp investigation is active. Finish it and return.");
        }
        if (tasks->westW4Completed) {
            return Loc_Translate("Echo Basin is archived. Move on to Last Camp.");
        }
        if (tasks->westW4Started) {
            return Loc_Translate("The Echo Basin investigation is active. Finish it and return.");
        }
        if (tasks->westW3Completed) {
            return Loc_Translate("Canopy Hollow is archived. Proceed to Echo Basin.");
        }
        if (tasks->westW3Started) {
            return Loc_Translate("Canopy Hollow investigation in progress.");
        }
        if (tasks->westW2Completed) {
            return Loc_Translate("The Canopy Hollow investigation is active here now that Survey Break is archived.");
        }
        return Loc_Translate("Complete the Survey Break investigation first.");
    }
    if (std::strcmp(locationName, "Echo Basin") == 0) {
        if (!IsWestRouteAvailable(tasks)) {
            return Loc_Translate("Echo Basin stays sealed until the comm relay is restored.");
        }
        if (tasks->westW5Completed) {
            return Loc_Translate("The west trail is fully archived.");
        }
        if (tasks->westW5Started) {
            return Loc_Translate("The Last Camp investigation is active. Finish it and return.");
        }
        if (tasks->westW4Completed) {
            return Loc_Translate("Echo Basin is archived. Proceed to Last Camp.");
        }
        if (tasks->westW4Started) {
            return Loc_Translate("Echo Basin investigation in progress.");
        }
        if (tasks->westW3Completed) {
            return Loc_Translate("The Echo Basin investigation is active here now that Canopy Hollow is archived.");
        }
        return Loc_Translate("Complete the Canopy Hollow investigation first.");
    }
    if (std::strcmp(locationName, "Last Camp") == 0) {
        if (!IsWestRouteAvailable(tasks)) {
            return Loc_Translate("Last Camp stays sealed until the comm relay is restored.");
        }
        if (tasks->westW5Completed) {
            return Loc_Translate("Last Camp is archived.");
        }
        if (tasks->westW5Started) {
            return Loc_Translate("Last Camp investigation in progress.");
        }
        if (tasks->westW4Completed) {
            return Loc_Translate("The Last Camp investigation is active here now that Echo Basin is archived.");
        }
        return Loc_Translate("Complete the Echo Basin investigation first.");
    }
    if (std::strcmp(locationName, "South Collapse") == 0) {
        if (!IsSouthRouteAvailable(tasks)) {
            return Loc_Translate("The south trail stays sealed until the Power Bay is restored.");
        }
        if (tasks->southS5Completed) {
            return Loc_Translate("The south archive trail is complete.");
        }
        if (tasks->southS5Started) {
            return Loc_Translate("Root Vault investigation in progress. Finish it and return.");
        }
        if (tasks->southS4Completed) {
            return Loc_Translate("Purifier Ring is archived. Move on to Root Vault.");
        }
        if (tasks->southS4Started) {
            return Loc_Translate("Purifier Ring investigation in progress. Finish it and return.");
        }
        if (tasks->southS3Completed) {
            return Loc_Translate("Service Shafts are archived. Move on to Purifier Ring.");
        }
        if (tasks->southS3Started) {
            return Loc_Translate("Service Shafts investigation in progress. Finish it and return.");
        }
        if (tasks->southS2Completed) {
            return Loc_Translate("Vent Galleries are archived. Move on to Service Shafts.");
        }
        if (tasks->southS2Started) {
            return Loc_Translate("Vent Galleries investigation in progress. Finish it and return.");
        }
        if (tasks->southS1Completed) {
            return Loc_Translate("South Collapse is archived. Start Vent Galleries.");
        }
        return Loc_Translate("South Collapse investigation in progress. Complete it and return.");
    }
    if (std::strcmp(locationName, "Vent Galleries") == 0) {
        if (!IsSouthRouteAvailable(tasks)) {
            return Loc_Translate("Vent Galleries stay sealed until the Power Bay is restored.");
        }
        if (tasks->southS3Completed) {
            return Loc_Translate("The south trail is archived through the Service Shafts.");
        }
        if (tasks->southS3Started) {
            return Loc_Translate("The Service Shafts investigation is active. Finish it and return.");
        }
        if (tasks->southS2Completed) {
            return Loc_Translate("Vent Galleries are archived. Proceed to Service Shafts.");
        }
        if (tasks->southS2Started) {
            return Loc_Translate("Vent Galleries investigation in progress.");
        }
        if (tasks->southS1Completed) {
            return Loc_Translate("The Vent Galleries investigation is active here now that South Collapse is archived.");
        }
        return Loc_Translate("Complete the South Collapse investigation first.");
    }
    if (std::strcmp(locationName, "Service Shafts") == 0) {
        if (!IsSouthRouteAvailable(tasks)) {
            return Loc_Translate("Service Shafts stay sealed until the Power Bay is restored.");
        }
        if (tasks->southS5Completed) {
            return Loc_Translate("The south trail is fully archived.");
        }
        if (tasks->southS5Started) {
            return Loc_Translate("The Root Vault investigation is active. Finish it and return.");
        }
        if (tasks->southS4Completed) {
            return Loc_Translate("Purifier Ring is archived. Move on to Root Vault.");
        }
        if (tasks->southS4Started) {
            return Loc_Translate("The Purifier Ring investigation is active. Finish it and return.");
        }
        if (tasks->southS3Completed) {
            return Loc_Translate("Service Shafts are archived. Proceed to Purifier Ring.");
        }
        if (tasks->southS3Started) {
            return Loc_Translate("Service Shafts investigation in progress.");
        }
        if (tasks->southS2Completed) {
            return Loc_Translate("The Service Shafts investigation is active here now that Vent Galleries are archived.");
        }
        return Loc_Translate("Complete the Vent Galleries investigation first.");
    }
    if (std::strcmp(locationName, "Purifier Ring") == 0) {
        if (!IsSouthRouteAvailable(tasks)) {
            return Loc_Translate("Purifier Ring stays sealed until the Power Bay is restored.");
        }
        if (tasks->southS5Completed) {
            return Loc_Translate("The south trail is fully archived.");
        }
        if (tasks->southS5Started) {
            return Loc_Translate("The Root Vault investigation is active. Finish it and return.");
        }
        if (tasks->southS4Completed) {
            return Loc_Translate("Purifier Ring is archived. Proceed to Root Vault.");
        }
        if (tasks->southS4Started) {
            return Loc_Translate("Purifier Ring investigation in progress.");
        }
        if (tasks->southS3Completed) {
            return Loc_Translate("The Purifier Ring investigation is active here now that Service Shafts are archived.");
        }
        return Loc_Translate("Complete the Service Shaft investigation first.");
    }
    if (std::strcmp(locationName, "Root Vault") == 0) {
        if (!IsSouthRouteAvailable(tasks)) {
            return Loc_Translate("Root Vault stays sealed until the Power Bay is restored.");
        }
        if (tasks->southS5Completed) {
            return Loc_Translate("Root Vault is archived.");
        }
        if (tasks->southS5Started) {
            return Loc_Translate("Root Vault investigation in progress.");
        }
        if (tasks->southS4Completed) {
            return Loc_Translate("The Root Vault investigation is active here now that Purifier Ring is archived.");
        }
        return Loc_Translate("Complete the Purifier Ring investigation first.");
    }

    switch (area) {
        case MAP_AREA_BASE:
            if (tasks->stage == 3) {
                return Loc_Translate("Base is your relay hub. Prepare here, then return safely.");
            }
            if (tasks->stage == 4) {
                if (!player->hasLaserGun || !player->hasProtectionSuit) {
                    return Loc_Translate("Craft the Laser Gun and Protection Suit before heading out.");
                }
                return Loc_Translate("Your gear is ready. Inspect the crash clue.");
            }
            if (tasks->stage == 5) {
                if (player->resources[RESOURCE_ENERGY_CORE] > 0) {
                    return Loc_PickLiteral("Install the extracted Energy Core in the power bay, finish the ore reinforcement, and reopen South Collapse in the Subsurface Sink.",
                                           "把拆出的能源核心装进动力舱，完成矿石加固，并重新打开地下沉降带里的南部塌陷区。");
                }
                if (HasEnergyCoreExtractionMaterials(player)) {
                    return Loc_PickLiteral("The salvage bundle is ready. Return to the Crash Forest wreck and extract the core.",
                                           "打捞材料已经齐了。回到坠毁森林残骸，把核心拆出来。");
                }
                return Loc_PickLiteral("Prepare the suit and camp before pushing through Flooded Detour and Deep Basin for wreck-extraction materials.",
                                       "在穿过积水绕路和深潭区收集残骸提取材料之前，先准备好防护服和营地。");
            }
            if (tasks->stage == 6) {
                if (player->resources[RESOURCE_RELIC_FRAGMENT] >= 3) {
                    if (IsCrossX2Ready(tasks)) {
                        return Loc_PickLiteral("The fragments now let Loxi compare rescue, repair, and settlement through Echo Basin, Purifier Ring, and the Monolith Ring.",
                                               "这些碎片现在能让洛希沿着回声盆地、净化环区与石碑环区的证据比较救援、修复与定居。");
                    }
                    if (IsCrossX1Ready(tasks)) {
                        return FormatDualArchiveLocationMessage(tasks,
                                                                "The fragments now let Loxi align %s with %s.",
                                                                "这些碎片现在能让洛希把%s与%s对齐起来。");
                    }
                    return Loc_Translate("Bring the fragments back to Loxi's terminal.");
                }
                if (IsCrossX2Ready(tasks)) {
                    return FormatDualArchiveLocationMessage(tasks,
                                                            "Finish the fragment runs, then keep advancing %s and %s so Loxi can finish reading what this ending really demands.",
                                                            "完成碎片搜集后，继续推进%s与%s，好让洛希读清这个结局到底真正要求你承担什么。");
                }
                if (IsCrossX1Ready(tasks)) {
                    return FormatDualArchiveLocationMessage(tasks,
                                                            "Finish the fragment runs, then keep pushing %s and %s so Loxi can see how both trails finally connect.",
                                                            "完成碎片搜集后，继续推进%s与%s，好让洛希看清这两条线索最后是怎样接到一起的。");
                }
                return Loc_PickLiteral("Collect 3 Relic Fragments across Ruins Approach, Monolith Ring, and Signal Tower Plateau.",
                                       "去遗迹前沿、石碑环区和信号塔高台收集 3 枚遗迹碎片。");
            }
            if (tasks->stage == 7) {
                if (Tasks_IsEndingBranchReady(tasks) && tasks->selectedEndingRoute == ENDING_NONE) {
                    if (!tasks->endingArchiveReviewed) {
                        return Loc_Translate("All mainline logs recovered. Return to Loxi and review the assembled truth before choosing how to answer it.");
                    }
                    return Loc_PickLiteral("All mainline logs recovered. Return to Loxi and choose how you are going to answer what the assembled archive now says.",
                                           "所有主线日志都已回收。回到洛希处，决定你要如何回应这套完整档案如今给出的答案。");
                }
                if (tasks->selectedEndingRoute == ENDING_HEROIC) {
                    if (tasks->bossDefeated) {
                        return Loc_Translate("Heroic route chosen. Finish at the Signal Tower.");
                    }
                    if (tasks->monolithsLit >= 3) {
                        return Loc_PickLiteral("Heroic route chosen. The ring is ready. Push into the northwest ruins when you are ready for the guardian fight.",
                                               "已选择强行救援路线。石碑环已经准备完毕。准备好后就推进西北遗迹，开始守卫战。");
                    }
                    return Loc_PickLiteral("Heroic route chosen. Start the guardian fight in the northwest ruins.",
                                           "已选择强行救援路线。去西北遗迹开始守卫战。");
                }
                if (tasks->selectedEndingRoute == ENDING_PEACEFUL) {
                    if (player->hasSignalAmplifier) {
                        return Loc_Translate("Peaceful route chosen. Carry the amplifier to the tower.");
                    }
                    if (player->resources[RESOURCE_RELIC_FRAGMENT] >= 3) {
                        return Loc_Translate("Peaceful route chosen. Craft the amplifier at the workshop before leaving base.");
                    }
                    return Loc_Translate("Peaceful route chosen. Recover 3 fragments, then craft the amplifier at the workshop before leaving base.");
                }
                if (tasks->selectedEndingRoute == ENDING_SETTLEMENT) {
                    if (tasks->bossDefeated) {
                        return Loc_PickLiteral("Settlement route chosen. Return to Loxi when you are ready to stay here for good.",
                                               "已选择定居路线。等你准备好彻底留下来时，回到洛希处。");
                    }
                    return Loc_PickLiteral("Settlement route chosen. Defeat the guardian in the northwest ruins before you settle here.",
                                           "已选择定居路线。想在这里定居之前，先去西北遗迹击败守卫。");
                }
                if (IsCrossX3Ready(tasks)) {
                    return Loc_PickLiteral("Archive almost complete. Last Camp and Root Vault are filed; one final ship-log sweep should let Loxi lay the ending choices out clearly.",
                                           "档案已接近完整。最后营地与根脉核心都已归档；再完成最后一次飞船日志清扫，洛希就能把终局选择清楚摆出来。");
                }
                if (IsCrossX2Ready(tasks)) {
                    return FormatDualArchiveLocationMessage(tasks,
                                                            "Main archive is now the last blocker before what Loxi pieced together around %s and %s becomes a real choice.",
                                                            "主线档案如今是最后一道阻碍。补完它之后，洛希围绕%s与%s拼出来的判断才会变成真正的选择。");
                }
                if (IsCrossX1Ready(tasks)) {
                    return FormatDualArchiveLocationMessage(tasks,
                                                            "Aligned crew and facility evidence is ready around %s and %s. Keep bringing back mainline proof.",
                                                            "围绕%s与%s的船员与设施证据已经开始对齐。继续把主线证据带回来。");
                }
                return FormatDualArchiveLocationMessage(tasks,
                                                        "Final stage evidence sweep: recover %s, %s, and the missing ship logs first, then choose an ending second.",
                                                        "最终阶段是一场证据清扫：先补回%s、%s以及缺失的飞船日志，再考虑结局。");
            }
            return Loc_Translate("Base is the safest place to recover, craft, and sync.");
        case MAP_AREA_FOREST:
            if (player->crouching) {
                return Loc_Translate("Crouching in the forest makes you harder to detect.");
            }
            return Loc_Translate("Use crouch in the forest to stay hidden.");
        case MAP_AREA_SWAMP_OUTER:
            if (std::strcmp(locationName, "Outer Swamp Rim") == 0) {
                return Loc_Translate("Outer Swamp Rim is the safer approach. Practice short runs and quick returns.");
            }
            if (std::strcmp(locationName, "Flooded Detour") == 0) {
                return Loc_Translate("Flooded Detour is longer. Keep an oxygen buffer and a camp fallback ready.");
            }
            if (player->hasRope && player->hasFieldCamp) {
                return Loc_Translate("Use rope shortcuts if they help, and keep the Field Camp as your fallback.");
            }
            if (player->hasRope) {
                return Loc_Translate("The rope can shorten the route, but you can finish the run without it.");
            }
            if (player->hasFieldCamp) {
                return Loc_Translate("The Field Camp gives you a safe fallback point.");
            }
            return Loc_Translate("The outer swamp drains oxygen steadily. Plan your return before you commit.");
        case MAP_AREA_SWAMP_DEEP:
            if (std::strcmp(locationName, "Deep Gate") == 0) {
                return Loc_Translate("Deep Gate is a reliable turn-back checkpoint.");
            }
            if (std::strcmp(locationName, "Deep Basin") == 0) {
                if (tasks->stage == 5 && !HasEnergyCoreExtractionMaterials(player)) {
                    return Loc_PickLiteral("Deep Basin is where the wreck-extraction materials start to justify the risk. Take what you need and get out.",
                                           "深潭区里那些残骸提取材料，才是值得你冒险的东西。拿够就走。");
                }
                if (tasks->stage == 5 && HasEnergyCoreExtractionMaterials(player)) {
                    return Loc_PickLiteral("Deep Basin has already given you what the wreck needs. Do not linger here longer than necessary.",
                                           "深潭区已经给了你残骸所需的材料，不要再做无谓停留。");
                }
                return Loc_Translate("Deep Basin is high risk. Keep the suit and oxygen support ready.");
            }
            if (player->hasProtectionSuit) {
                return Loc_Translate("The Protection Suit helps, but the deep swamp is still dangerous.");
            }
            return Loc_Translate("The deep swamp demands a Protection Suit and poison control.");
        case MAP_AREA_RUINS:
            if (tasks->stage >= 7) {
                if (std::strcmp(locationName, "Signal Tower Plateau") == 0) {
                if (tasks->selectedEndingRoute == ENDING_PEACEFUL && player->hasSignalAmplifier) {
                    return Loc_Translate("Peaceful route chosen. Push for tower stabilization.");
                }
                if (tasks->selectedEndingRoute == ENDING_HEROIC && tasks->monolithsLit >= 3) {
                    return Loc_Translate("Heroic route chosen. This is the best timing for the tower push after the guardian falls.");
                }
                if (tasks->selectedEndingRoute == ENDING_SETTLEMENT) {
                    return Loc_PickLiteral("Settlement route chosen. The tower is no longer your goal; what matters now is closing the archive back at Loxi.",
                                           "已选择定居路线。塔楼已经不是你的目标；你现在真正要做的，是回到洛希处完成档案闭合。");
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
            return GetGuardianArenaHint(tasks);
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
