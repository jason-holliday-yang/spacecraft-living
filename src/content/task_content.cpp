#include "task_content.h"
#include "task_presentation.h"

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
    {1, {"Air Triage", "呼吸止损"}, {"Recover the first ship archives, then repair the first oxygen module so the wreck stops dying room by room and gives you one more day of air.", "先回收第一批飞船档案，再修好第一组氧气模块，让残骸别再一间一间断气，也替今晚多抢回一天能呼吸的时间。"}},
    {2, {"Ship Archive", "船内归档"}, {"Recover the remaining ship archives, craft a Glow Stick, and finish the oxygen repair that opens the east airlock and turns survival into the first real field push.", "回收剩余船内档案，制作荧光棒，并完成会开启东侧气闸的最终氧气修复，让求生正式从船内挣扎变成第一次外勤推进。"}},
    {3, {"Relay Opening", "中继开路"}, {"Run the east relay sortie, restore the comm relay, and let Loxi turn the broken response outside into the first usable west-route lead.", "完成东侧中继行动，修复通讯中继，让洛希把外面那段破碎回应整理成第一条真正可追的西线线索。"}},
    {4, {"West Confirmation", "西线确认"}, {"Prepare for the west push, verify the crash clue, and begin recovering the first field archive that proves the missing crew did not simply vanish.", "为西线推进做好准备，确认坠毁线索，并回收第一份能证明失踪船员并非凭空消失的现场档案。"}},
    {5, {"Power Recovery", "动力回收"}, {"Extract the Energy Core, restore the Power Bay, and reopen the south facility trail so the world can start reading like infrastructure instead of pure hazard.", "拆出能源核心，恢复动力舱，并重新打开南线设施路线，让这颗世界开始不再只像危险地形，而像一套仍能被理解的设施系统。"}},
    {6, {"Ruins Synthesis", "遗迹拼图"}, {"Recover 3 Relic Fragments, keep advancing west and south archives, then return to Loxi so scattered evidence can close into one corrected history.", "收集 3 枚遗迹碎片，继续推进西线与南线档案，然后回到洛希处，让分散证据闭合成一段被纠正过的完整历史。"}},
    {7, {"Archive Verdict", "档案定案"}, {"Recover the remaining logs, return to Loxi, and decide how you will answer the truth the full record now supports.", "找回剩余日志，回到洛希处，并决定你要如何回应这套完整记录如今支撑起来的真相。"}}
}};

const std::array<EndingTextDef, 4> kEndingTextDefs = {{
    {ENDING_HEROIC, {"Heroic Rescue", "强行救援"}, {"The beacon finally burned into the sky, but it did so by force. You left this world carrying the people home first, and carrying the scar of what had to be broken to make that possible.", "信标最终被强行点亮，求救被送出天际。你带着活下来的人类判断离开了这颗世界，却也在它本就受损的结构上再留下了一道无法假装不存在的伤痕。这不是干净的胜利，只是你仍把“先把人送回去”排在最前的结果。"}},
    {ENDING_PEACEFUL, {"Peaceful Rescue", "和平救援"}, {"You did not tear the tower open one last time. The signal rose through compatibility instead, and you left behind a quieter world than force would have scarred.", "你没有再把高塔当成必须强闯的东西，而是让人类系统一步步学会被它正确读取。信号最终升起时，这颗世界并没有被再次粗暴撕开。你离开了，但你离开时留下的，是一个比强行救援更完整、也更安静的边界。"}},
    {ENDING_SETTLEMENT, {"Alien Settlement", "异星定居"}, {"Leaving stopped being the only honest future. The wreck became a home, and the world became a duty you chose to keep alive instead of flee.", "当离开不再是唯一诚实的未来，你选择留下。不是因为你走不了，也不是因为这颗世界突然变得温柔，而是因为你已经知道它仍可被维护，而别人没做完的工作终究需要有人接下去。开拓者号不再只是残骸，它成了你愿意长期守住的起点。"}},
    {ENDING_FAILURE, {"Failed Survival", "生存失败"}, {"Collapse outran repair. Air, power, shelter, and judgment never held long enough, and you fell before the truth of this world could be carried all the way to a final choice.", "崩溃来得比修复更快。空气、电力、庇护与判断没有一次能同时撑到最后，你在这颗世界真正被讲清楚之前先一步倒下。不是因为这里没有答案，而是因为你没能活得足够久，把那些答案接成一条路。"}}
}};

bool IsTowerPlateau(const Player *player) {
    const char *locationName;

    if (player == NULL) {
        return false;
    }

    locationName = Map_GetLocationNameAt(player->gridX, player->gridY);
    return locationName != NULL && std::strcmp(locationName, "Signal Tower Plateau") == 0;
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
        return Loc_PickLiteral("Northwest Ruins: dodge charge and shockwave commits, then strike only after the guardian's heavy momentum breaks.",
                               "西北遗迹守卫战：躲开冲锋与震荡重招，等守卫的大动作落空、节奏断开后再出手。");
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
                return Loc_PickLiteral("Northwest Ruins Phase 2: reinforcements are entering, but the lit ring is still buying control. Clear space, then punish the guardian cleanly.",
                                       "西北遗迹守卫战二阶段：增援已经开始进场，但完整石碑环仍在帮你稳住局面。先清出空间，再稳定惩罚守卫。");
            }
            return Loc_PickLiteral("Northwest Ruins Phase 2: relic-guard reinforcements can pin you down. Reposition first, then punish only after the guardian's heavy commits whiff.",
                                   "西北遗迹守卫战二阶段：遗迹守卫增援会逼你站位失衡。先重新拉开位置，再在首领重招落空后反打。");
        case 3:
        default:
            if (tasks->monolithsLit >= 3) {
                return Loc_PickLiteral("Northwest Ruins Final Phase: the ring is still buying precious time. Save oxygen, survive the faster chain pressure, and end it on one clean punish.",
                                       "西北遗迹守卫战最终阶段：石碑环仍在替你争取宝贵时间。留好氧气，顶住更快的连段压力，用一次干净惩罚收掉它。");
            }
            return Loc_PickLiteral("Northwest Ruins Final Phase: there is no safe attrition left now. Keep oxygen in reserve, dodge the faster chain pressure, and finish on the next real opening.",
                                   "西北遗迹守卫战最终阶段：现在已经没有安全磨血空间了。保留氧气，躲开更快的连段压力，并在下一次真正露出破绽时结束战斗。");
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
            return Loc_PickLiteral("Return to the workshop, craft the Signal Amplifier, then come back to Loxi so peaceful rescue can stop being an idea and become a real compatible route.",
                                   "先回到工坊制作信号放大器，再回洛希处，让和平救援不再只是设想，而真正成为一条可兼容执行的路线。");
        }
        return Loc_PickLiteral("Recover 3 Relic Fragments, craft the Signal Amplifier at the workshop, then return to Loxi so peaceful rescue can be argued from evidence instead of hope alone.",
                               "先找回 3 枚遗迹碎片，在工坊制作信号放大器，再回洛希处，让和平救援建立在证据上，而不只是希望。");
    }
    if (!IsHeroicEndingEvidenceReady(tasks)) {
        return Loc_PickLiteral("Recover the Canopy Handoff record, then return to Loxi. Heroic rescue should be chosen with the west crew's relay effort fully in view.",
                               "先补回林冠交接记录，再回洛希处。强行救援这条路，至少该在看清西线接力之后再被正式选下。");
    }
    if (!IsPeacefulEndingEvidenceReady(tasks)) {
        return Loc_PickLiteral("Recover a south-facility control record, then return to Loxi. Peaceful rescue cannot stand until the system side of the story is back on the table.",
                               "先补回一份南线设施控制记录，再回洛希处。和平救援若想成立，就不能少了系统这一侧的真相。");
    }
    if (!IsSettlementEndingEvidenceReady(tasks)) {
        return Loc_PickLiteral("Recover the Last Camp testament, then return to Loxi if you still want settlement to remain an honest option.",
                               "先补回最后营地遗言；如果你还想让定居继续作为一个诚实选项存在，再回洛希处确认。");
    }
    if (!tasks->bossDefeated) {
        return Loc_PickLiteral("Defeat the guardian in the northwest ruins, then return to Loxi if you want settlement to become a future that can actually hold.",
                               "先在西北遗迹击败守卫；如果你想让定居不只是口头选择，而成为真正站得住的未来，再回洛希处确认。");
    }

    return Loc_PickLiteral("Return to Loxi and choose which unlocked ending you are willing to answer the archive with.",
                           "返回洛希处，在已解锁的结局中选出你愿意用来回应整套档案的答案。");
}

const char *GetStage7EndingChoiceObjective(const TaskSystem *tasks, const Player *player) {
    const int availableEndingCount = Tasks_GetAvailableEndingCount(tasks);

    if (availableEndingCount <= 0) {
        return GetStage7PendingUnlockObjective(tasks, player);
    }
    if (availableEndingCount == 1) {
        return FormatSingleEndingSummary(tasks,
                                         "Return to Loxi and confirm the only unlocked ending the archive still supports: %s.",
                                         "返回洛希处，确认这套档案如今唯一仍然支撑的结局：%s。");
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
        return Loc_Translate("Stay alive, follow Loxi, and keep piecing the story together.");
    }

    stageText = FindStageTextDef(tasks->stage);
    if (tasks->stage == 1) {
        if (shipIntroLogsCollected < 3) {
            return Loc_PickLiteral("Recover the first ship archives in Cargo Hold, Crew Quarters, and Diagnostics, then repair the first oxygen module so the wreck stops dying room by room.",
                                   "先回收货舱、船员舱和诊断舱里的第一批飞船档案，再修好第一组氧气模块，让残骸别再一间一间断气。");
        }
        return Loc_PickLiteral("Repair the first oxygen module to raise the ship's oxygen ceiling and steady the wreck before you ask anything more from it.",
                               "修好第一组氧气模块，抬高飞船的供氧上限，也先把这具残骸稳住，再继续逼它撑下去。");
    }
    if (tasks->stage == 2) {
        if (shipIntroLogsCollected < 3) {
            return Loc_PickLiteral("Recover the remaining ship archives, craft a Glow Stick, and finish the oxygen repair that opens the east airlock for the first true push outside.",
                                   "回收剩余船内档案，制作荧光棒，并完成会开启东侧气闸的最终氧气修复，为第一次真正走出飞船做准备。");
        }
        if (player != NULL && player->hasGlowStick) {
            return Loc_PickLiteral("Spend 1 Glow Moss and 1 Ore on the final oxygen repair, then open the east airlock and turn ship survival into your first field archive run.",
                                   "用 1 份发光苔和 1 份矿石完成最终氧气修复，然后打开东侧气闸，把船内求生正式推进成第一段外勤档案回收。");
        }
        return Loc_PickLiteral("Craft a Glow Stick, spend 1 Glow Moss and 1 Ore on the final oxygen repair, then open the east airlock for the first field archive run.",
                               "制作荧光棒，用 1 份发光苔和 1 份矿石完成最终氧气修复，然后打开东侧气闸，开始第一段外勤档案回收。");
    }
    if (tasks->stage == 3) {
        return Loc_PickLiteral("Use the east airlock, gather relay materials, restore the comm relay, and let Loxi turn the answering signal into the first west-route pursuit.",
                               "使用东侧气闸，收集中继修复材料，恢复通讯中继，让洛希把那段回应中的异常信号整理成第一条正式的西线追踪。");
    }
    if (tasks->stage == 4) {
        if (player != NULL && (!player->hasLaserGun || !player->hasProtectionSuit)) {
            return Loc_PickLiteral("Craft the Laser Gun and Protection Suit first. Then inspect the Crash Forest wreck clue and begin recovering the first west-route archive in Echo Wilds.",
                                   "先制作激光枪和防护服，然后调查坠毁森林里的残骸线索，并开始回收回响荒野中的第一份西线档案。");
        }
        return Loc_PickLiteral("Inspect the Crash Forest wreck clue, confirm the Deep Basin black-box trail, and begin recovering the first west archive that proves the crew left a route behind.",
                               "调查坠毁森林里的残骸线索，确认深潭区的黑匣轨迹，并开始回收第一份能证明船员确实留下过路线的西线档案。");
    }
    if (tasks->stage == 6) {
        if (player != NULL && player->resources[RESOURCE_RELIC_FRAGMENT] >= 3) {
            if (IsCrossX2Ready(tasks)) {
                return Loc_PickLiteral("Sync the fragments at Loxi and review how Echo Basin, the Purifier Ring, and the Monolith Ring now recast rescue, stabilization, and settlement as answers to one shared truth.",
                                       "把碎片带回洛希处同步，并复核回声盆地、净化环区与石碑环区如今如何把救援、稳定与定居重新写成对同一真相的不同回答。");
            }
            if (IsCrossX1Ready(tasks)) {
                return FormatDualArchiveLocationMessage(tasks,
                                                        "Bring the fragments back to Loxi so it can connect %s with %s into one clear history instead of two parallel mysteries.",
                                                        "把碎片带回洛希处同步，让它把%s与%s连成一段清楚历史，而不再只是两条并行的谜团。");
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
        if (Tasks_IsEndingPreCheckReady(tasks) && !tasks->endingArchiveReviewed && tasks->selectedEndingRoute == ENDING_NONE) {
            return Loc_PickLiteral("Return to Loxi and review the assembled archive before choosing the final route. The story is finally whole enough to judge what your answer should cost.",
                                   "返回洛希处，先复核已经拼完整的档案，再选择最终路线。故事已经完整到足以判断，你的回答究竟该付出什么代价。");
        }
        if (Tasks_IsEndingBranchReady(tasks) && tasks->selectedEndingRoute == ENDING_NONE) {
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
                return Loc_PickLiteral("Settlement route chosen. Return to Loxi and confirm that staying here is no longer an emergency fallback, but your deliberate future.",
                                       "已选择定居路线。回到洛希处，确认留下来不再只是权宜之计，而是你主动承担的未来。");
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
            return Loc_PickLiteral("Return to the Power Bay, install the extracted Energy Core, finish the ore reinforcement, and reopen South Collapse so the facility side of the story can wake up.",
                                   "返回动力舱，安装已经拆出的能源核心，完成矿石加固，并重新打开南部塌陷区，让这段属于设施与系统的故事真正苏醒。");
        }
        if (HasEnergyCoreExtractionMaterials(player)) {
            return Loc_PickLiteral("Return to the Crash Forest wreck, extract the Energy Core with the salvage bundle you assembled, then bring that proof back to the Power Bay.",
                                   "返回坠毁森林里的残骸，用你已经凑齐的打捞材料把能源核心拆出来，再把这份能继续推开南线的证明带回动力舱。");
        }
        return Loc_PickLiteral("Gather 1 Junk Metal, 1 Protective Fiber, and 1 Energy Crystal around Flooded Detour and Deep Basin, then return to the Crash Forest wreck to extract the Energy Core and push open the south facility trail.",
                               "前往积水绕路和深潭区收集 1 份废旧金属、1 份防护纤维和 1 份能量晶体，然后回到坠毁森林残骸拆出能源核心，把南线设施路线真正推开。");
    }

    if (stageText != NULL) {
        return Loc_PickText(stageText->defaultObjective);
    }

    return Loc_Translate("Stay with Loxi's thread and keep pushing the story forward.");
}

const char *TasksContent_GetStageGuidance(const TaskSystem *tasks, const Player *player) {
    MapArea area;
    const char *locationName;
    const int shipIntroLogsCollected = CountCollectedShipIntroLogs(tasks);

    if (tasks == NULL) {
        return Loc_Translate("Recover your breathing, steady yourself, and move toward the next piece of the record.");
    }

    area = player != NULL ? Map_GetAreaAt(player->gridX, player->gridY) : MAP_AREA_UNKNOWN;
    locationName = player != NULL ? Map_GetLocationNameAt(player->gridX, player->gridY) : Loc_Translate("Unknown Area");
    if (std::strcmp(locationName, "West Frontier") == 0) {
        if (!IsWestRouteAvailable(tasks)) {
            return Loc_PickLiteral("West Frontier is still sealed. Restore the comm relay before the western signal can become a real route.",
                                   "西部前线仍处于封锁状态。先修复通讯中继，然后西线信号才能成为真正的路线。");
        }
        if (tasks->westW5Completed) {
            return Loc_PickLiteral("The west archive trail is complete. The missing crew's relay now stands as a closed record.",
                                   "西线档案路线已完整。失踪船员的中继如今已成为一份已结案的记录。");
        }
        if (tasks->westW5Started) {
            return Loc_PickLiteral("Finish the Last Camp investigation, then return to base with the final west-side argument.",
                                   "完成最后营地调查，然后带着最终的西线论点返回基地。");
        }
        if (tasks->westW4Completed) {
            return Loc_PickLiteral("Echo Basin is archived. Move on to Last Camp, where the west trail stops being route work and becomes a final position.",
                                   "回声盆地已归档。继续前往最后营地，在那里西线不再只是路线工作，而是变成了最终立场。");
        }
        if (tasks->westW4Started) {
            return Loc_PickLiteral("Finish the Echo Basin investigation, then return to base so the relay can be rebuilt into readable history.",
                                   "完成回声盆地调查，然后返回基地，让中继能被重建为可读的历史。");
        }
        if (tasks->westW3Completed) {
            return Loc_PickLiteral("Canopy Hollow is archived. Move on to Echo Basin, where disappearance starts turning into proof of successful handoff.",
                                   "林中空地已归档。继续前往回声盆地，在那里失踪开始转变为成功交接的证据。");
        }
        if (tasks->westW3Started) {
            return Loc_PickLiteral("Finish the Canopy Hollow investigation, then return to base with the handoff record intact.",
                                   "完成林中空地调查，然后带着完整的交接记录返回基地。");
        }
        if (tasks->westW2Completed) {
            return Loc_PickLiteral("Survey Break is archived. Move on to Canopy Hollow, where the route starts speaking in people instead of markers.",
                                   "勘探断点已归档。继续前往林中空地，在那里路线开始由人而非标记来讲述。");
        }
        if (tasks->westW2Started) {
            return Loc_PickLiteral("Finish the Survey Break investigation, then return to base with the anchor chain restored.",
                                   "完成勘探断点调查，然后带着已修复的锚链返回基地。");
        }
        if (tasks->westW1Completed) {
            return Loc_PickLiteral("West Frontier is archived. Move on to Survey Break and follow the route the scouts meant to leave behind.",
                                   "西部前线已归档。继续前往勘探断点，跟随侦察员本想留下的路线前进。");
        }
        if (tasks->westW1Started) {
            return Loc_PickLiteral("Finish the West Frontier investigation, then return to base with the first signal fragment.",
                                   "完成西部前线调查，然后带着第一块信号碎片返回基地。");
        }
        return Loc_PickLiteral("The west trail is open. Recover the first fragment and bring it back so Loxi can prove this signal was never random noise.",
                               "西线已开启。回收第一块碎片并带回来，让洛希能证明这个信号从来就不是随机噪音。");
    }
    if (std::strcmp(locationName, "Survey Break") == 0) {
        if (!IsWestRouteAvailable(tasks)) {
            return Loc_PickLiteral("Survey Break is still sealed. Restore the comm relay before the west relay chain can continue.",
                                   "勘探断点仍处于封锁状态。先修复通讯中继，然后西线中继链才能继续。");
        }
        if (tasks->westW3Completed) {
            return Loc_PickLiteral("The west trail has already been carried past Echo Basin. Survey Break now stands as part of a longer relay, not a dead end.",
                                   "西线路线已经推进过了回声盆地。勘探断点如今成为更长中继链的一部分，而不是死路。");
        }
        if (tasks->westW3Started) {
            return Loc_PickLiteral("The Canopy Hollow investigation is already active farther ahead. Survey Break has done its part in keeping that chain alive.",
                                   "林中空地调查已在更前方进行中。勘探断点已经为保持这条链的运作做出了贡献。");
        }
        if (tasks->westW2Completed) {
            return Loc_PickLiteral("Survey Break is archived. Continue to Canopy Hollow and follow the route into its first true handoff point.",
                                   "勘探断点已归档。继续前往林中空地，跟随路线进入第一个真正的交接点。");
        }
        if (tasks->westW2Started) {
            return Loc_PickLiteral("Finish the Survey Break objectives, then return to base. The anchors only matter if the record survives the walk back.",
                                   "完成勘探断点目标，然后返回基地。锚点只有在记录能活着带回去的情况下才有意义。");
        }
        if (tasks->westW1Completed) {
            return Loc_PickLiteral("The Survey Break investigation is active here. Restore the relay work, then return to base before pushing deeper.",
                                   "勘探断点调查已在此处开始。修复中继工作，然后在深入之前返回基地。");
        }
        return Loc_PickLiteral("Complete the West Frontier investigation first. Survey Break only matters once the first signal fragment is archived.",
                               "先完成西部前线调查。只有在第一块信号碎片归档后，勘探断点才有意义。");
    }
    if (std::strcmp(locationName, "Canopy Hollow") == 0) {
        if (!IsWestRouteAvailable(tasks)) {
            return Loc_PickLiteral("Canopy Hollow is still sealed. Restore the comm relay before the west trail can rise into its first watch post.",
                                   "林中空地仍处于封锁状态。先修复通讯中继，然后西线才能升到它的第一个瞭望点。");
        }
        if (tasks->westW5Completed) {
            return Loc_PickLiteral("The west trail is fully archived. Canopy Hollow now reads as part of a complete human relay line.",
                                   "西线已完全归档。林中空地如今读起来像是一条完整的人类中继线的一部分。");
        }
        if (tasks->westW5Started) {
            return Loc_PickLiteral("The Last Camp investigation is active. Canopy Hollow has already passed its burden forward.",
                                   "最后营地调查正在进行中。林中空地已经把它的担子递交给了前方。");
        }
        if (tasks->westW4Completed) {
            return Loc_PickLiteral("Echo Basin is archived. Continue to Last Camp, where the west trail's final judgment was left behind.",
                                   "回声盆地已归档。继续前往最后营地，在那里留下了西线路线的最终判断。");
        }
        if (tasks->westW4Started) {
            return Loc_PickLiteral("The Echo Basin investigation is active. Canopy Hollow has already handed the route onward.",
                                   "回声盆地调查正在进行中。林中空地已经把路线递交给了前方。");
        }
        if (tasks->westW3Completed) {
            return Loc_PickLiteral("Canopy Hollow is archived. Continue to Echo Basin, where the broken pings finally start reading like proof.",
                                   "林中空地已归档。继续前往回声盆地，在那里破碎的信号终于开始读起来像是证据。");
        }
        if (tasks->westW3Started) {
            return Loc_PickLiteral("Finish the Canopy Hollow investigation here, then return to base with the record that turns absence into duty.",
                                   "在这里完成林中空地调查，然后带着那份把缺席转化为责任的记录返回基地。");
        }
        if (tasks->westW2Completed) {
            return Loc_PickLiteral("The Canopy Hollow investigation is active here. Hold the perch long enough to recover the handoff, then return.",
                                   "林中空地调查已在此处开始。在制高点停留足够长时间来恢复交接，然后返回。");
        }
        return Loc_PickLiteral("Complete the Survey Break investigation first. The climb to Canopy Hollow only matters once the relay markers are restored.",
                               "先完成勘探断点调查。只有在中继标记被修复后，攀登到林中空地才有意义。");
    }
    if (std::strcmp(locationName, "Echo Basin") == 0) {
        if (!IsWestRouteAvailable(tasks)) {
            return Loc_PickLiteral("Echo Basin is still sealed. Restore the comm relay before the west fragments can be rebuilt into route truth.",
                                   "回声盆地仍处于封锁状态。先修复通讯中继，然后西线碎片才能被重建为路线真相。");
        }
        if (tasks->westW5Completed) {
            return Loc_PickLiteral("The west trail is fully archived. Echo Basin now stands as proof that the handoff really reached the south.",
                                   "西线路线已完全归档。回声盆地如今成为交接确实到达南线的证据。");
        }
        if (tasks->westW5Started) {
            return Loc_PickLiteral("The Last Camp investigation is active. Echo Basin has already done its work of turning static into proof.",
                                   "最后营地调查正在进行中。回声盆地已经完成了把静态转化为证据的工作。");
        }
        if (tasks->westW4Completed) {
            return Loc_PickLiteral("Echo Basin is archived. Continue to Last Camp and recover the argument left at the end of the relay.",
                                   "回声盆地已归档。继续前往最后营地，回收中继尽头留下的论点。");
        }
        if (tasks->westW4Started) {
            return Loc_PickLiteral("Finish the Echo Basin investigation here, then return to base so Loxi can reconstruct the crossing properly.",
                                   "在这里完成回声盆地调查，然后返回基地，让洛希能正确重建那次穿越。");
        }
        if (tasks->westW3Completed) {
            return Loc_PickLiteral("The Echo Basin investigation is active here. Recover the topology proof, then return before the route loses coherence again.",
                                   "回声盆地调查已在此处开始。回收拓扑证明，然后在路线再次失去连贯性之前返回。");
        }
        return Loc_PickLiteral("Complete the Canopy Hollow investigation first. Echo Basin only closes once the handoff above it is understood.",
                               "先完成林中空地调查。只有在上方的交接被理解后，回声盆地才算结束。");
    }
    if (std::strcmp(locationName, "Last Camp") == 0) {
        if (!IsWestRouteAvailable(tasks)) {
            return Loc_PickLiteral("Last Camp is still sealed. Restore the comm relay before the west trail can reach its final testimony.",
                                   "最后营地仍处于封锁状态。先修复通讯中继，然后西线路线才能到达它的最终证词。");
        }
        if (tasks->westW5Completed) {
            return Loc_PickLiteral("Last Camp is archived. The west route now ends in argument and intention, not disappearance.",
                                   "最后营地已归档。西线路线如今以论点和意图而非失踪告终。");
        }
        if (tasks->westW5Started) {
            return Loc_PickLiteral("Finish the Last Camp investigation here, then return to base with the final west-side testament.",
                                   "在这里完成最后营地调查，然后带着最终的西线证词返回基地。");
        }
        if (tasks->westW4Completed) {
            return Loc_PickLiteral("The Last Camp investigation is active here. Recover the final notebooks, then return before this answer is lost with the camp.",
                                   "最后营地调查已在此处开始。回收最终笔记本，然后在这个答案随营地消失前返回。");
        }
        return Loc_PickLiteral("Complete the Echo Basin investigation first. The last camp only makes sense once the relay that reached it is proven.",
                               "先完成回声盆地调查。只有在到达它的中继被证明后，最后营地才有意义。");
    }
    if (std::strcmp(locationName, "South Collapse") == 0) {
        if (!IsSouthRouteAvailable(tasks)) {
            return Loc_PickLiteral("South Collapse is still sealed. Restore the Power Bay before the facility side of the story can reopen.",
                                   "南部塌陷仍处于封锁状态。先恢复动力舱，然后设施方面的故事才能重新开始。");
        }
        if (tasks->southS5Completed) {
            return Loc_PickLiteral("The south archive trail is complete. The facility chain now closes as one damaged maintenance history.",
                                   "南线档案路线已完整。设施链如今以一份受损的维护历史告终。");
        }
        if (tasks->southS5Started) {
            return Loc_PickLiteral("Finish the Root Vault investigation, then return to base with the system-level truth intact.",
                                   "完成根脉核心调查，然后带着完整的系统级真相返回基地。");
        }
        if (tasks->southS4Completed) {
            return Loc_PickLiteral("Purifier Ring is archived. Move on to Root Vault, where the system finally explains what all of this has been.",
                                   "净化环区已归档。继续前往根脉核心，在那里系统终于会解释这一切到底是什么。");
        }
        if (tasks->southS4Started) {
            return Loc_PickLiteral("Finish the Purifier Ring investigation, then return to base with the control sequence preserved.",
                                   "完成净化环区调查，然后带着保存好的控制序列返回基地。");
        }
        if (tasks->southS3Completed) {
            return Loc_PickLiteral("Service Shafts are archived. Move on to Purifier Ring, where the machine stops feeling buried and starts answering back.",
                                   "竖井区已归档。继续前往净化环区，在那里机器不再感觉像是被掩埋的，而是开始给予回应。");
        }
        if (tasks->southS3Started) {
            return Loc_PickLiteral("Finish the Service Shaft investigation, then return to base with the backbone record.",
                                   "完成竖井区调查，然后带着骨干记录返回基地。");
        }
        if (tasks->southS2Completed) {
            return Loc_PickLiteral("Vent Galleries are archived. Move on to Service Shafts, where ship and facility begin reading as one machine.",
                                   "通风廊道已归档。继续前往竖井区，在那里飞船和设施开始读起来像是同一台机器。");
        }
        if (tasks->southS2Started) {
            return Loc_PickLiteral("Finish the Vent Galleries investigation, then return to base with the handover that bought everyone cleaner minutes.",
                                   "完成通风廊道调查，然后带着那份为所有人买来了干净空气时间的交接返回基地。");
        }
        if (tasks->southS1Completed) {
            return Loc_PickLiteral("South Collapse is archived. Move on to Vent Galleries and keep following the trail from hazard into infrastructure.",
                                   "南部塌陷已归档。继续前往通风廊道，继续沿着从危险到基础设施的轨迹前进。");
        }
        if (tasks->southS1Started) {
            return Loc_PickLiteral("Finish the South Collapse investigation, then return to base with the outage memo that renames the fog.",
                                   "完成南部塌陷调查，然后带着那份重新定义了迷雾的停机备忘录返回基地。");
        }
        return Loc_PickLiteral("The south trail is open. Recover the first outage record and bring it back so the fog can stop reading like pure hostility.",
                               "南线已开启。回收第一份停机记录并带回来，让迷雾不再读起来像是纯粹的敌意。");
    }
    if (std::strcmp(locationName, "Vent Galleries") == 0) {
        if (!IsSouthRouteAvailable(tasks)) {
            return Loc_PickLiteral("Vent Galleries are still sealed. Restore the Power Bay before the south route can begin breathing properly again.",
                                   "通风廊道仍处于封锁状态。在南线能再次正常呼吸之前，先恢复动力舱。");
        }
        if (tasks->southS3Completed) {
            return Loc_PickLiteral("The south trail has already been carried through the Service Shafts. Vent Galleries now stand as the first point where the system answered back.",
                                   "南线已经通过竖井区继续推进。通风廊道现在是系统首次作出回应的第一点。");
        }
        if (tasks->southS3Started) {
            return Loc_PickLiteral("The Service Shafts investigation is already active deeper in. Vent Galleries have already handed the route onward.",
                                   "竖井区调查已在更深处进行。通风廊道已经把路线递交给前方了。");
        }
        if (tasks->southS2Completed) {
            return Loc_PickLiteral("Vent Galleries are archived. Continue to Service Shafts and follow the route down into the backbone.",
                                   "通风廊道已归档。继续前往竖井区，沿着路线深入到骨干。");
        }
        if (tasks->southS2Started) {
            return Loc_PickLiteral("Finish the Vent Galleries investigation here, then return to base with the calibration handover intact.",
                                   "在此完成通风廊道调查，然后带着完好的校准交接返回基地。");
        }
        if (tasks->southS1Completed) {
            return Loc_PickLiteral("The Vent Galleries investigation is active here. Restore the handover, then return before the cleaner window closes.",
                                   "通风廊道调查已在此进行。恢复交接，然后在清洁窗口关闭前返回。");
        }
        return Loc_PickLiteral("Complete the South Collapse investigation first. The vents only matter once the outage memo is archived.",
                               "先完成南部塌陷调查。只有在停机备忘录归档后，通风廊道才有意义。");
    }
    if (std::strcmp(locationName, "Service Shafts") == 0) {
        if (!IsSouthRouteAvailable(tasks)) {
            return Loc_PickLiteral("Service Shafts are still sealed. Restore the Power Bay before the south route can enter the buried backbone.",
                                   "竖井区仍处于封锁状态。在南线能进入埋藏的骨干之前，先恢复动力舱。");
        }
        if (tasks->southS5Completed) {
            return Loc_PickLiteral("The south trail is fully archived. Service Shafts now read as one doorway into the same machine that caught the ship.",
                                   "南线已完全归档。竖井区现在读起来像是通往捕获飞船的同一台机器的一道门。");
        }
        if (tasks->southS5Started) {
            return Loc_PickLiteral("The Root Vault investigation is active. The shaft backbone has already done its work of carrying you there.",
                                   "根脉核心调查正在进行中。竖井骨干已经完成了把你带到那里的工作。");
        }
        if (tasks->southS4Completed) {
            return Loc_PickLiteral("Purifier Ring is archived. Continue to Root Vault and pull the full system truth into the open.",
                                   "净化环区已归档。继续前往根脉核心，把完整的系统真相拉出来。");
        }
        if (tasks->southS4Started) {
            return Loc_PickLiteral("The Purifier Ring investigation is active. Service Shafts have already handed the route forward.",
                                   "净化环区调查正在进行中。竖井区已经把路线递交给前方了。");
        }
        if (tasks->southS3Completed) {
            return Loc_PickLiteral("Service Shafts are archived. Continue to Purifier Ring, where sequence starts mattering more than force.",
                                   "竖井区已归档。继续前往净化环区，在那里序列开始比武力更重要。");
        }
        if (tasks->southS3Started) {
            return Loc_PickLiteral("Finish the Service Shafts investigation here, then return to base with the sync record that proves ship and facility share one backbone.",
                                   "在此完成竖井区调查，然后带着证明飞船和设施共用一个骨干的同步记录返回基地。");
        }
        if (tasks->southS2Completed) {
            return Loc_PickLiteral("The Service Shafts investigation is active here. Recover the sync record, then return before the route buries the pattern again.",
                                   "竖井区调查已在此进行。恢复同步记录，然后在路线再次埋藏模式之前返回。");
        }
        return Loc_PickLiteral("Complete the Vent Galleries investigation first. The shafts only make sense once the vent handover is archived.",
                               "先完成通风廊道调查。只有在通风交接归档后，竖井区才有意义。");
    }
    if (std::strcmp(locationName, "Purifier Ring") == 0) {
        if (!IsSouthRouteAvailable(tasks)) {
            return Loc_PickLiteral("Purifier Ring is still sealed. Restore the Power Bay first.",
                                   "净化环区仍处于封锁状态。先恢复动力舱。");
        }
        if (tasks->southS5Completed) {
            return Loc_PickLiteral("The south trail is fully archived. Purifier Ring now stands as the place where danger was finally re-read as control.",
                                   "南线档案路线已完整。净化环区如今代表着那个危险终于被重新解读为控制系统的地方。");
        }
        if (tasks->southS5Started) {
            return Loc_PickLiteral("The Root Vault investigation is active. Purifier Ring has already handed the route toward the final explanation.",
                                   "根脉核心调查正在进行中。净化环区已经把路线递交给了最终的答案。");
        }
        if (tasks->southS4Completed) {
            return Loc_PickLiteral("Purifier Ring is archived. Continue to Root Vault, where the system finally explains what all of this has been.",
                                   "净化环区已归档。继续前往根脉核心，在那里系统终于会解释这一切到底是什么。");
        }
        if (tasks->southS4Started) {
            return Loc_PickLiteral("Finish the Purifier Ring investigation here. Recover the control brief that changes what the monoliths mean, then return to base.",
                                   "在这里完成净化环区调查。回收那份改变了石碑含义的控制简报，然后返回基地。");
        }
        if (tasks->southS3Completed) {
            return Loc_PickLiteral("The Purifier Ring investigation is active here now that Service Shafts are archived. Recover the brief that turns the system from threat into responsibility.",
                                   "净化环区调查已在此处开始，因为竖井区已归档。回收那份把系统从威胁转变为责任的简报。");
        }
        return Loc_PickLiteral("Complete the Service Shaft investigation first. The ring only speaks clearly once the backbone record is in hand.",
                               "先完成竖井区调查。只有拿到骨干记录后，环区才会真正开始说话。");
    }
    if (std::strcmp(locationName, "Root Vault") == 0) {
        if (!IsSouthRouteAvailable(tasks)) {
            return Loc_PickLiteral("Root Vault stays sealed until the Power Bay is restored and the south route can reach its final archive.",
                                   "根脉核心保持封锁，直到动力舱恢复且南线能到达最终归档处。");
        }
        if (tasks->southS5Completed) {
            return Loc_PickLiteral("Root Vault is archived. The tower, fog, purifier chain, monoliths, and crash now all sit inside one corrected record.",
                                   "根脉核心已归档。塔楼、迷雾、净化链、石碑和坠机现在都已纳入同一套纠正后的记录中。");
        }
        if (tasks->southS5Started) {
            return Loc_PickLiteral("Root Vault investigation in progress. Recover the core dossier that finally tells the same story as every other clue.",
                                   "根脉核心调查进行中。回收那份终于能和其他所有线索讲述同一个故事的核心档案。");
        }
        if (tasks->southS4Completed) {
            return Loc_PickLiteral("The Root Vault investigation is active here now that Purifier Ring is archived. Pull the final dossier out before the whole system sinks back into fragments.",
                                   "根脉核心调查已在此处开始，因为净化环区已归档。在整个系统再次碎裂前，把最终档案取出来。");
        }
        return Loc_PickLiteral("Complete the Purifier Ring investigation first. Root Vault only matters once the control layer has been understood.",
                               "先完成净化环区调查。只有理解了控制层，根脉核心才会有意义。");
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
                ? Loc_PickLiteral("Use the east airlock, repair the comm relay, and reopen the West Frontier archive trail in Echo Wilds.",
                                  "使用东侧气闸，修复通讯中继，并重新打开回声荒野中的西部前线档案路线。")
                : (std::strcmp(locationName, "Outer Swamp Rim") == 0
                    ? Loc_PickLiteral("Gather Vine, Shell Fruit, and Fungus on Outer Swamp Rim for the relay. This sortie is what unlocks West Frontier in Echo Wilds.",
                                      "在外层沼泽边缘收集藤蔓、壳果和菌株，用来修复中继。这次外勤就是解锁回声荒野西部前线的关键。")
                    : Loc_PickLiteral("Stay on the relay route. This east run exists to reopen archive tracking, not to drift into the deeper swamp.",
                                      "继续沿着中继路线推进。这次东线行动的目的，是重新接通档案追踪，不是偏离去更深的沼泽乱闯。"));
        case 4:
            return area == MAP_AREA_BASE
                ? Loc_PickLiteral("Craft the Laser Gun and Protection Suit. Loxi wants the Crash Forest wreck clue and the newly reopened West Frontier archive next.",
                                  "制作激光枪和防护服。接下来，洛希希望你先确认东侧坠毁森林的残骸线索，再推进刚重新打开的西部前线档案。")
                : Loc_PickLiteral("Confirm the Crash Forest wreck clue in the east, then start recovering West Frontier in Echo Wilds.",
                                  "先确认东侧坠毁森林的残骸线索，再开始回收回声荒野中的西部前线档案。");
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
                        return Loc_PickLiteral("Heroic route chosen. The ring is fully lit. Push into the northwest ruins when you are ready for the guardian and turn that extra stability into a real win.",
                                               "已选择强行救援路线。石碑环已完全点亮。准备好后就推进西北遗迹，迎战守卫，并把这份额外稳定性真正转化成胜势。");
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
        return Loc_PickLiteral("High poison. Use a Recovery Ration or retreat.", "毒素过高。使用复苏口粮，或立刻撤退。");
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
            return Loc_PickLiteral("Purifier Ring stays sealed until the Power Bay is restored and the south route can reach the control layer.",
                                   "净化环区保持封锁，直到动力舱恢复且南线能到达控制层。");
        }
        if (tasks->southS5Completed) {
            return Loc_PickLiteral("The south trail is fully archived. Purifier Ring now stands as the place where danger was finally re-read as control.",
                                   "南线已完全归档。净化环区现在代表着那个危险终于被重新解读为控制的地方。");
        }
        if (tasks->southS5Started) {
            return Loc_PickLiteral("The Root Vault investigation is active. Purifier Ring has already handed the route toward the final explanation.",
                                   "根脉核心调查正在进行中。净化环区已经把路线递交给了最终解释。");
        }
        if (tasks->southS4Completed) {
            return Loc_PickLiteral("Purifier Ring is archived. Proceed to Root Vault, where the whole lattice can finally be named.",
                                   "净化环区已归档。继续前往根脉核心，在那里整个网络终于能被命名。");
        }
        if (tasks->southS4Started) {
            return Loc_PickLiteral("Purifier Ring investigation in progress. Recover the control brief that changes what the monoliths mean.",
                                   "净化环区调查进行中。恢复那份改变了石碑含义的控制简报。");
        }
        if (tasks->southS3Completed) {
            return Loc_PickLiteral("The Purifier Ring investigation is active here now that Service Shafts are archived. Recover the brief that turns the system from threat into responsibility.",
                                   "净化环区调查已在此进行，因为竖井区已归档。恢复那份把系统从威胁转变为责任的简报。");
        }
        return Loc_PickLiteral("Complete the Service Shaft investigation first. The ring only speaks clearly once the backbone record is in hand.",
                               "先完成竖井区调查。只有拿到骨干记录后，环区才会清楚说话。");
    }
    if (std::strcmp(locationName, "Root Vault") == 0) {
        if (!IsSouthRouteAvailable(tasks)) {
            return Loc_PickLiteral("Root Vault stays sealed until the Power Bay is restored and the south route can reach its final archive.",
                                   "根脉核心保持封锁，直到动力舱恢复且南线能到达它的最终归档。");
        }
        if (tasks->southS5Completed) {
            return Loc_PickLiteral("Root Vault is archived. The tower, fog, purifier chain, monoliths, and crash now all sit inside one corrected record.",
                                   "根脉核心已归档。塔楼、迷雾、净化链、石碑和坠毁现在都安放在一份纠正后的记录中。");
        }
        if (tasks->southS5Started) {
            return Loc_PickLiteral("Root Vault investigation in progress. Recover the core dossier that finally tells the same story as every other clue.",
                                   "根脉核心调查进行中。恢复那份终于能和所有其他线索讲同一个故事的核心档案。");
        }
        if (tasks->southS4Completed) {
            return Loc_PickLiteral("The Root Vault investigation is active here now that Purifier Ring is archived. Pull the final dossier out before the whole system sinks back into fragments.",
                                   "根脉核心调查已在此进行，因为净化环区已归档。在整个系统再次碎裂之前，把最终档案拉出来。");
        }
        return Loc_PickLiteral("Complete the Purifier Ring investigation first. Root Vault only matters once the control layer has been understood.",
                               "先完成净化环区调查。只有在控制层被理解后，根脉核心才有意义。");
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
                    return Loc_PickLiteral("The fragments now let Loxi compare rescue, repair, and settlement across Echo Basin, the Purifier Ring, and the Monolith Ring as three answers to one corrected history.",
                                           "这些碎片现在能让洛希沿着回声盆地、净化环区与石碑环区的证据，把救援、修复与定居并读成对同一段纠偏历史的三种回答。");
                }
                if (IsCrossX1Ready(tasks)) {
                    return FormatDualArchiveLocationMessage(tasks,
                                                                "The fragments now let Loxi align %s with %s into one shared chain of cause.",
                                                                "这些碎片现在能让洛希把%s与%s对齐成同一条因果链。");
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
                        return Loc_Translate("All mainline logs recovered. Return to Loxi and review the assembled truth before choosing how you will answer it.");
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
                        return Loc_PickLiteral("Settlement route chosen. Return to Loxi when you are ready to make staying here permanent and deliberate.",
                                               "已选择定居路线。等你准备好把留下来变成一个正式而长久的决定时，回到洛希处。");
                    }
                    return Loc_PickLiteral("Settlement route chosen. Defeat the guardian in the northwest ruins before you settle here.",
                                           "已选择定居路线。想在这里定居之前，先去西北遗迹击败守卫。");
                }
                if (IsCrossX3Ready(tasks)) {
                    return Loc_PickLiteral("Archive almost complete. Last Camp and Root Vault are filed; one final sweep of the ship logs should let Loxi lay the ending choices out with names, causes, and costs attached.",
                                           "档案已接近完整。最后营地与根脉核心都已归档；再完成最后一次飞船日志清扫，洛希就能把终局选择连同名字、因果与代价一起清楚摆出来。");
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
            return Loc_Translate("Base is where scraps become plans: recover here, craft here, and let Loxi turn fragments into meaning.");
        case MAP_AREA_FOREST:
            return Loc_Translate("Forest cover can break up long sight lines, but movement and route choice matter more than hiding in place.");
        case MAP_AREA_SWAMP_OUTER:
            if (std::strcmp(locationName, "Outer Swamp Rim") == 0) {
                return Loc_Translate("Outer Swamp Rim is the safer edge. Use it to learn the swamp in short runs before the deeper routes start asking for more.");
            }
            if (std::strcmp(locationName, "Flooded Detour") == 0) {
                return Loc_Translate("Flooded Detour takes longer than it looks. Carry extra oxygen margin and keep a fallback camp in mind before you commit.");
            }
            if (player->hasRope && player->hasFieldCamp) {
                return Loc_Translate("Use rope shortcuts when they save real time, and keep the Field Camp as the place you retreat before the swamp decides for you.");
            }
            if (player->hasRope) {
                return Loc_Translate("The rope can cut distance, but do not let a shortcut convince you the swamp has become forgiving.");
            }
            if (player->hasFieldCamp) {
                return Loc_Translate("The Field Camp is your promise that this run still has a place to fall back to.");
            }
            return Loc_Translate("The outer swamp drains oxygen without drama. Plan the return first, then earn the trip out.");
        case MAP_AREA_SWAMP_DEEP:
            if (std::strcmp(locationName, "Deep Gate") == 0) {
                return Loc_Translate("Deep Gate is the last place that still behaves like a sensible turn-back point. Decide there whether this run is worth carrying deeper.");
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
                return Loc_Translate("Deep Basin is where caution stops being style and becomes survival. Keep the suit sealed and your oxygen support ready.");
            }
            if (player->hasProtectionSuit) {
                return Loc_Translate("The Protection Suit helps, but deep swamp danger does not care that you came prepared.");
            }
            return Loc_Translate("The deep swamp punishes optimism. Bring a Protection Suit, poison control, and a reason strong enough to justify the risk.");
        case MAP_AREA_RUINS:
            if (tasks->stage >= 7) {
                if (std::strcmp(locationName, "Signal Tower Plateau") == 0) {
                if (tasks->selectedEndingRoute == ENDING_PEACEFUL && player->hasSignalAmplifier) {
                    return Loc_Translate("Peaceful route chosen. You are not here to tear the tower open, but to make it read you correctly. Push for stabilization.");
                }
                if (tasks->selectedEndingRoute == ENDING_HEROIC && tasks->monolithsLit >= 3) {
                    return Loc_Translate("Heroic route chosen. The ring is giving you the cleanest window you will get. Finish the guardian, then force the tower while the chance still holds.");
                }
                if (tasks->selectedEndingRoute == ENDING_SETTLEMENT) {
                    return Loc_PickLiteral("Settlement route chosen. The tower is no longer your goal; what matters now is closing the archive back at Loxi.",
                                           "已选择定居路线。塔楼已经不是你的目标；你现在真正要做的，是回到洛希处完成档案闭合。");
                }
                    if (tasks->selectedEndingRoute == ENDING_NONE) {
                        if (Tasks_IsEndingBranchReady(tasks) && !tasks->endingArchiveReviewed) {
                            return Loc_Translate("Do not commit here blind. Return to Loxi and review the assembled archive before this place becomes a choice instead of a threat.");
                        }
                        return Loc_Translate("Do not commit here blind. The tower can wait; the archive still cannot.");
                    }
                    return Loc_Translate("The plateau is where intention stops hiding. Follow your chosen route's preparation before you offer the tower an answer.");
                }
                if (std::strcmp(locationName, "Monolith Ring") == 0) {
                    if (tasks->monolithsLit >= 3) {
                        return Loc_Translate("The monolith ring is lit. What looked like menace is now doing the work of restraint.");
                    }
                    if (tasks->monolithsLit > 0) {
                        return Loc_Translate("Light the remaining monoliths and keep the ring from failing in pieces.");
                    }
                    return Loc_Translate("Start lighting monoliths. The ring is not decoration here; it is the only thing already trying to hold the damage back.");
                }
            }
            if (std::strcmp(locationName, "Ruins Approach") == 0) {
                return Loc_Translate("Ruins Approach is where every deep run should still feel reversible. Prepare here, then decide how much truth you can afford to chase in one push.");
            }
            if (IsTowerPlateau(player)) {
                return Loc_Translate("Near the tower, oxygen margin is not spare comfort. It is the line between making a decision and becoming part of the wreckage.");
            }
            return Loc_Translate("The ruins tax oxygen faster than they look. Treat every advance here as a planned run, not a wandering search.");
        case MAP_AREA_BOSS_ARENA:
            return GetGuardianArenaHint(tasks);
        case MAP_AREA_UNKNOWN:
        default:
            return Loc_Translate("Watch oxygen, health, and the road back. Every answer on this world still has to be survived first.");
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
