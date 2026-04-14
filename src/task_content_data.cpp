#include "task_content.h"

#include <array>

namespace {

#define LT(en, zh) LocalizedText{en, zh}

const std::array<TaskNodeSeed, 71> kNodeSeeds = {{
    {RESOURCE_WOOD, EXTERIOR_X(55), EXTERIOR_Y(53), 2, 4, false},
    {RESOURCE_WOOD, EXTERIOR_X(59), EXTERIOR_Y(66), 2, 4, false},
    {RESOURCE_WOOD, EXTERIOR_X(67), EXTERIOR_Y(78), 2, 4, false},
    {RESOURCE_WOOD, EXTERIOR_X(82), EXTERIOR_Y(79), 2, 4, false},
    {RESOURCE_WOOD, EXTERIOR_X(96), EXTERIOR_Y(77), 2, 4, false},
    {RESOURCE_WOOD, EXTERIOR_X(47), EXTERIOR_Y(59), 2, 4, false},
    {RESOURCE_WOOD, EXTERIOR_X(52), EXTERIOR_Y(42), 2, 4, false},
    {RESOURCE_WOOD, EXTERIOR_X(93), EXTERIOR_Y(30), 2, 4, false},
    {RESOURCE_WOOD, EXTERIOR_X(104), EXTERIOR_Y(65), 2, 4, false},
    {RESOURCE_WOOD, EXTERIOR_X(68), EXTERIOR_Y(96), 2, 4, false},
    {RESOURCE_METAL_SCRAP, EXTERIOR_X(58), EXTERIOR_Y(54), 2, 4, false},
    {RESOURCE_METAL_SCRAP, EXTERIOR_X(62), EXTERIOR_Y(69), 2, 4, false},
    {RESOURCE_METAL_SCRAP, EXTERIOR_X(106), EXTERIOR_Y(56), 1, 3, false},
    {RESOURCE_METAL_SCRAP, EXTERIOR_X(30), EXTERIOR_Y(80), 1, 3, false},
    {RESOURCE_METAL_SCRAP, EXTERIOR_X(47), EXTERIOR_Y(67), 1, 3, false},
    {RESOURCE_METAL_SCRAP, EXTERIOR_X(105), EXTERIOR_Y(98), 1, 3, false},
    {RESOURCE_FRUIT, EXTERIOR_X(98), EXTERIOR_Y(48), 2, 4, false},
    {RESOURCE_FRUIT, EXTERIOR_X(107), EXTERIOR_Y(64), 2, 4, false},
    {RESOURCE_FRUIT, EXTERIOR_X(110), EXTERIOR_Y(80), 2, 4, false},
    {RESOURCE_FRUIT, EXTERIOR_X(109), EXTERIOR_Y(96), 2, 4, false},
    {RESOURCE_FRUIT, EXTERIOR_X(116), EXTERIOR_Y(103), 2, 4, false},
    {RESOURCE_FRUIT, EXTERIOR_X(45), EXTERIOR_Y(76), 2, 4, false},
    {RESOURCE_FRUIT, EXTERIOR_X(47), EXTERIOR_Y(87), 1, 3, false},
    {RESOURCE_GLOW_MOSS, EXTERIOR_X(70), EXTERIOR_Y(80), 1, 4, false},
    {RESOURCE_GLOW_MOSS, EXTERIOR_X(84), EXTERIOR_Y(80), 1, 4, false},
    {RESOURCE_GLOW_MOSS, EXTERIOR_X(98), EXTERIOR_Y(40), 2, 4, false},
    {RESOURCE_GLOW_MOSS, EXTERIOR_X(99), EXTERIOR_Y(86), 2, 4, false},
    {RESOURCE_GLOW_MOSS, EXTERIOR_X(61), EXTERIOR_Y(94), 1, 4, false},
    {RESOURCE_GLOW_MOSS, EXTERIOR_X(39), EXTERIOR_Y(82), 1, 4, false},
    {RESOURCE_GLOW_MOSS, EXTERIOR_X(43), EXTERIOR_Y(68), 1, 4, false},
    {RESOURCE_GLOW_MOSS, EXTERIOR_X(94), EXTERIOR_Y(100), 1, 4, false},
    {RESOURCE_ORE, EXTERIOR_X(73), EXTERIOR_Y(79), 1, 3, false},
    {RESOURCE_ORE, EXTERIOR_X(91), EXTERIOR_Y(82), 1, 3, false},
    {RESOURCE_ORE, EXTERIOR_X(105), EXTERIOR_Y(37), 1, 3, false},
    {RESOURCE_ORE, EXTERIOR_X(106), EXTERIOR_Y(38), 1, 3, false},
    {RESOURCE_ORE, EXTERIOR_X(76), EXTERIOR_Y(100), 1, 3, false},
    {RESOURCE_ORE, EXTERIOR_X(90), EXTERIOR_Y(98), 1, 3, false},
    {RESOURCE_SPECIAL_FUNGUS, EXTERIOR_X(100), EXTERIOR_Y(44), 1, 3, false},
    {RESOURCE_SPECIAL_FUNGUS, EXTERIOR_X(108), EXTERIOR_Y(74), 1, 3, false},
    {RESOURCE_SPECIAL_FUNGUS, EXTERIOR_X(112), EXTERIOR_Y(98), 1, 3, false},
    {RESOURCE_ALIEN_VINE, EXTERIOR_X(98), EXTERIOR_Y(50), 2, 3, false},
    {RESOURCE_ALIEN_VINE, EXTERIOR_X(106), EXTERIOR_Y(70), 2, 3, false},
    {RESOURCE_ALIEN_VINE, EXTERIOR_X(109), EXTERIOR_Y(100), 2, 3, false},
    {RESOURCE_ALIEN_VINE, EXTERIOR_X(44), EXTERIOR_Y(83), 1, 3, false},
    {RESOURCE_SHELL_FRUIT, EXTERIOR_X(102), EXTERIOR_Y(67), 1, 3, false},
    {RESOURCE_SHELL_FRUIT, EXTERIOR_X(110), EXTERIOR_Y(103), 1, 3, false},
    {RESOURCE_SHELL_FRUIT, EXTERIOR_X(48), EXTERIOR_Y(76), 1, 3, false},
    {RESOURCE_JUNK_METAL, EXTERIOR_X(96), EXTERIOR_Y(56), 1, 3, false},
    {RESOURCE_JUNK_METAL, EXTERIOR_X(101), EXTERIOR_Y(90), 1, 3, false},
    {RESOURCE_JUNK_METAL, EXTERIOR_X(113), EXTERIOR_Y(50), 1, 3, false},
    {RESOURCE_JUNK_METAL, EXTERIOR_X(36), EXTERIOR_Y(72), 1, 3, false},
    {RESOURCE_JUNK_METAL, EXTERIOR_X(48), EXTERIOR_Y(90), 1, 2, false},
    {RESOURCE_ENERGY_CRYSTAL, EXTERIOR_X(112), EXTERIOR_Y(56), 1, 2, false},
    {RESOURCE_ENERGY_CRYSTAL, EXTERIOR_X(114), EXTERIOR_Y(86), 1, 2, false},
    {RESOURCE_ENERGY_CRYSTAL, EXTERIOR_X(120), EXTERIOR_Y(98), 1, 2, false},
    {RESOURCE_CALM_MUSHROOM, EXTERIOR_X(111), EXTERIOR_Y(71), 1, 2, false},
    {RESOURCE_CALM_MUSHROOM, EXTERIOR_X(116), EXTERIOR_Y(95), 1, 2, false},
    {RESOURCE_PROTECTIVE_FIBER, EXTERIOR_X(109), EXTERIOR_Y(81), 1, 2, false},
    {RESOURCE_PROTECTIVE_FIBER, EXTERIOR_X(114), EXTERIOR_Y(101), 1, 2, false},
    {RESOURCE_PROTECTIVE_FIBER, EXTERIOR_X(123), EXTERIOR_Y(102), 1, 2, false},
    {RESOURCE_RELIC_FRAGMENT, EXTERIOR_X(70), EXTERIOR_Y(18), 1, 2, true},
    {RESOURCE_RELIC_FRAGMENT, EXTERIOR_X(86), EXTERIOR_Y(12), 1, 2, true},
    {RESOURCE_RELIC_FRAGMENT, EXTERIOR_X(88), EXTERIOR_Y(18), 1, 2, true},
    {RESOURCE_ALIEN_SLIME, EXTERIOR_X(60), EXTERIOR_Y(23), 1, 2, true},
    {RESOURCE_ALIEN_SLIME, EXTERIOR_X(73), EXTERIOR_Y(14), 1, 2, true},
    {RESOURCE_WOOD, EXTERIOR_X(24), EXTERIOR_Y(72), 2, 4, false},
    {RESOURCE_WOOD, SHIP_CARGO_HOLD_X + 1, SHIP_CARGO_HOLD_Y + 1, 2, 0, false},
    {RESOURCE_WOOD, SHIP_CREW_QUARTERS_X + 3, SHIP_CREW_QUARTERS_Y + 1, 2, 0, false},
    {RESOURCE_GLOW_MOSS, SHIP_AIRLOCK_LINK_X + 2, SHIP_AIRLOCK_LINK_Y + 2, 3, 0, false},
    {RESOURCE_METAL_SCRAP, SHIP_WORKSHOP_X + 3, SHIP_WORKSHOP_Y + 3, 2, 0, false},
    {RESOURCE_ORE, SHIP_POWER_BAY_X + 3, SHIP_POWER_BAY_Y + 3, 1, 0, false}
}};

const std::array<TaskMonsterSeed, 8> kMonsterSeeds = {{
    {MONSTER_THORN_LARVA, EXTERIOR_X(26), EXTERIOR_Y(74), 4},
    {MONSTER_WING_BUG, EXTERIOR_X(37), EXTERIOR_Y(76), 4},
    {MONSTER_RAPTOR, EXTERIOR_X(44), EXTERIOR_Y(69), 4},
    {MONSTER_SWAMP_STALKER, EXTERIOR_X(91), EXTERIOR_Y(98), 5},
    {MONSTER_SENTINEL_JELLY, EXTERIOR_X(114), EXTERIOR_Y(98), 6},
    {MONSTER_FOG_WORM, EXTERIOR_X(121), EXTERIOR_Y(100), 6},
    {MONSTER_RELIC_GUARD, EXTERIOR_X(74), EXTERIOR_Y(18), 7},
    {MONSTER_FINAL_BOSS, EXTERIOR_X(84), EXTERIOR_Y(20), 7}
}};

const std::array<TaskLogSeed, 14> kLogSeeds = {{
    {
        SHIP_CARGO_HOLD_X + 3,
        SHIP_CARGO_HOLD_Y + 3,
        SHIP_LOG_MAINLINE,
        0,
        LT("Ship Log 01: Impact Protocol", "飞船日志 01：撞击应对"),
        LT("Day 1. Impact did not kill us. The air nearly did. Oxygen lines burst through three compartments, and Loxi forced every remaining watt into keeping one corridor breathable. We are alive only because the ship chose triage over comfort. If this record survives, remember the order that kept us alive: air first, panic later.",
           "第 1 天。撞击没有杀死我们，差点要命的是空气。氧气管线在三个舱段里同时爆裂，洛西把剩下的每一瓦电力都压进了一条还能呼吸的走廊。我们活下来，不是因为舒适，而是因为飞船先做了取舍。如果这份记录还能留下，请记住让我们活下来的顺序：先保住空气，恐慌以后再说。"),
        LT("Permanent max health +6", "永久最大生命值 +6")
    },
    {
        SHIP_CREW_QUARTERS_X + 1,
        SHIP_CREW_QUARTERS_Y + 2,
        SHIP_LOG_MAINLINE,
        1,
        LT("Ship Log 02: Split Roster", "飞船日志 02：分组名单"),
        LT("Day 3. We stopped calling it a search party. It became a split. One group stayed close to the ship to keep air, power, and comms alive. Another pushed west with burst antenna parts. A third traced the eastern edge after spotting worked stone under the swamp growth. We are not wandering. We are widening the odds that somebody learns what this planet is.",
           "第 3 天。我们不再把它叫作搜寻队，而是一次分组。一队留在飞船附近，维持空气、电力和通讯；一队带着破损天线零件向西推进；第三队在沼泽植被下发现加工过的石构后，沿着东侧边缘前进。我们不是在乱闯，我们是在扩大有人查清这颗星球真相的概率。"),
        LT("Permanent attack power +4", "永久攻击力 +4")
    },
    {
        SHIP_DIAGNOSTICS_X + 1,
        SHIP_DIAGNOSTICS_Y + 2,
        SHIP_LOG_MAINLINE,
        2,
        LT("Ship Log 03: Pattern, Not Wilderness", "飞船日志 03：这不是荒野"),
        LT("Day 7. Growth rings, fog pulses, and stone harmonics repeat on a schedule. This is not wilderness in the human sense. Someone engineered a living maintenance field and then walked away before it finished learning how to heal itself. Loxi thinks the monoliths, purifier structures, and tower signal are all parts of the same unfinished response.",
           "第 7 天。生长环、雾潮脉冲和石质共振都在按固定节律重复。这不是人类意义上的荒野。有人建造了一片活着的维护场，却在它学会自我修复之前就离开了。洛西认为，石碑、净化结构和塔楼信号，其实都属于同一个尚未完成的应答系统。"),
        LT("Instant oxygen +18", "立即恢复氧气 +18")
    },
    {
        EXTERIOR_X(24),
        EXTERIOR_Y(63),
        SHIP_LOG_MAINLINE,
        3,
        LT("Field Record: West Signal Fragment 01", "现场记录：西部信号残片 01"),
        LT("The emergency stakes end here. Beyond this point, the west trail was deliberate: two crew carried burst antenna parts and left coded pings so the next runner could follow without shouting into the canopy. Their final note is blunt. Do not treat the west route like salvage. Read the path, copy the signal, and leave before the tower can triangulate you.",
           "应急标记到这里结束。再往前，西侧路线就不是慌乱逃生，而是有意布置：两名船员带着破损天线部件前行，并留下编码脉冲，让后继者无需在林冠下大喊也能跟上。最后一条备注写得很直白：不要把西线当成拾荒路。读懂路径，复制信号，然后在塔楼锁定你之前离开。"),
        LT("Instant oxygen +22", "立即恢复氧气 +22")
    },
    {
        EXTERIOR_X(34),
        EXTERIOR_Y(72),
        SHIP_LOG_MAINLINE,
        0,
        LT("Field Record: Survey Break Anchor Notes", "现场记录：勘测断点锚点笔记"),
        LT("Anchor pair A/B restored. The painted arrows were reversed on purpose. The scouts carried the antenna load farther west, but the calibration case and facility timestamps were handed south to a second crew. This was not a panic split. It was a relay. If one route went dark, the other still carried the truth forward.",
           "A/B 锚点对已恢复。那些涂漆箭头是故意反着画的。侦察员把天线载荷继续向西运送，而校准箱和设施时间戳则交给第二支队伍向南带走。这不是恐慌中的分裂，而是一场接力。如果一条路线失联，另一条仍能把真相往前送。"),
        LT("Permanent max health +6", "永久最大生命值 +6")
    },
    {
        EXTERIOR_X(41),
        EXTERIOR_Y(68),
        SHIP_LOG_SUPPLEMENTAL,
        1,
        LT("Field Record: Canopy Handoff Record", "现场记录：林冠交接记录"),
        LT("Observation perch still maintained nine days after impact. Wind vanes were trimmed, reflective tags reset, and food portions counted with almost absurd care. One entry confirms the handoff: two crew stayed west to watch the canopy, while one turned back south carrying purifier notes and tower timing data. Leaving was duty. Staying was duty too.",
           "撞击后第 9 天，观察点依然有人维护。风向片被修整过，反光标记被重新摆正，连食物份量都被近乎固执地仔细记录。其中一条写明了交接：两名船员留在西侧监视林冠，另一人则带着净化器笔记和塔楼计时数据折返南下。离开是职责，留下也是职责。"),
        LT("Permanent attack power +4", "永久攻击力 +4")
    },
    {
        EXTERIOR_X(47),
        EXTERIOR_Y(72),
        SHIP_LOG_MAINLINE,
        2,
        LT("Field Record: Echo Basin Topology Sketch", "现场记录：回声盆地地形草图"),
        LT("Three-point echo reconstruction complete. The last transmissions did not end in a wipeout. One courier reached the southern facility threshold and pushed through a packet linking tower behavior, vent timing, and monolith resonance. The important conclusion is almost comforting: the crews kept coordinating after the crash. We lost contact, not intent.",
           "三点回声重构完成。最后的传输并不是全员覆灭前的终点。有一名信使抵达了南部设施门槛，并送出了一份把塔楼反应、通风时序和石碑共振串在一起的数据包。最重要的结论甚至让人稍感宽慰：坠毁之后，船员之间的协作从未中断。失去的是联系，不是意图。"),
        LT("Instant oxygen +18", "立即恢复氧气 +18")
    },
    {
        EXTERIOR_X(48),
        EXTERIOR_Y(86),
        SHIP_LOG_SUPPLEMENTAL,
        1,
        LT("Field Record: Last Camp Testament", "现场记录：最后营地遗言"),
        LT("The final camp roll lists three positions, not one. First: light the tower and leave before the system closes again. Second: stabilize the environment so rescue does not become another collapse. Third: stay and turn survival into stewardship. No one writes as if only one answer is pure. They write as if the truth should survive, even if the crew does not.",
           "最后营地的点名记录里列出了三种立场，而不是唯一答案。第一：点亮信号塔，在系统再次关闭前离开。第二：先稳定环境，让救援不再演变成另一场崩溃。第三：留下来，把求生变成守护。没有人写得像是只有一种答案才算正确。他们写得更像是在说：哪怕船员活不下来，真相也该被保留下去。"),
        LT("Permanent attack power +4", "永久攻击力 +4")
    },
    {
        EXTERIOR_X(94),
        EXTERIOR_Y(57),
        SHIP_LOG_MAINLINE,
        3,
        LT("Crash Recorder: Black Box Residue", "坠毁记录：黑匣残留"),
        LT("Recovered after the residue scan matched Loxi's filter tables. The eastern wreck did not simply fall here. Its final corrections were trying to avoid the same tower lattice the western scouts later mapped from the canopy. The crash clue matters because it proves the crew was already fighting a system-wide failure, not a random storm. What happened in the forest, the swamp, and the ruins was one collapse seen from different angles.",
           "在残留扫描与洛西的过滤表匹配后才成功恢复。东侧残骸并不是单纯坠落到这里。它最后的修正动作，是在试图避开后来被西侧侦察员从林冠上标出的那片塔楼格网。这个坠毁线索之所以关键，是因为它证明船员面对的从来不是一场随机风暴，而是一次系统级失效。森林、沼泽和遗迹里发生的，其实是同一场崩塌，只是从不同角度看到。"),
        LT("Instant oxygen +22", "立即恢复氧气 +22")
    },
    {
        EXTERIOR_X(80),
        EXTERIOR_Y(98),
        SHIP_LOG_MAINLINE,
        2,
        LT("Facility Record: Purifier Outage Memo", "设施记录：净化器停摆备忘"),
        LT("South outer ring outage report. Toxic backflow rose after the crash because the maintenance chain failed, not because the fog suddenly became hostile on its own. The purifier ring was already unstable. The impact simply removed the people who kept it honest. Stone nodes on the north side are listed as dampers, not generators.",
           "南部外环停摆报告。坠毁后毒性回流加剧，并不是因为雾气突然自己变得更危险，而是因为维护链条断了。净化环本来就已经不稳定。撞击只是把那些一直勉强维持它运转的人彻底移除了。北侧石节点在记录里被标注为阻尼器，而不是能量源。"),
        LT("Instant oxygen +18", "立即恢复氧气 +18")
    },
    {
        EXTERIOR_X(92),
        EXTERIOR_Y(98),
        SHIP_LOG_SUPPLEMENTAL,
        0,
        LT("Facility Record: Vent Calibration Handover", "设施记录：通风校准交接"),
        LT("Unsigned handover from the vent galleries. The west-route crew promised a quiet signal window; the south-route crew promised cleaner air in return. They were buying each other time. The note ends with a warning: if the vents drift again, every northern push will feel harsher than it should. Environment and route pressure are the same problem seen through different doors.",
           "来自通风廊道的一份无签名交接。西线队伍承诺会创造一个安静的信号窗口；南线队伍则承诺换来更干净的空气。他们是在互相争取时间。记录最后留下一句警告：如果通风系统再次偏移，所有向北的推进都会比应有的更艰难。环境压力和路线压力，其实是同一个问题从不同门里看出去的样子。"),
        LT("Permanent max health +6", "永久最大生命值 +6")
    },
    {
        EXTERIOR_X(106),
        EXTERIOR_Y(99),
        SHIP_LOG_MAINLINE,
        1,
        LT("Facility Record: Service Shaft Sync Record", "设施记录：维护井道同步记录"),
        LT("Main shaft synchronization log. Ship systems, purifier controls, and the buried tower lattice share one maintenance backbone. Reopening these shafts does more than shorten the walk; it proves the crash site and ruins were always part of the same machine room. Every restored loop turns scattered survival into deliberate access.",
           "主井道同步日志。飞船系统、净化控制和埋藏的塔楼格网共享同一条维护主干。重新打开这些井道，不只是缩短路程；它还证明坠毁点和遗迹从来都属于同一个机房。每恢复一段闭环，零散的求生就更像一次有意识的进入。"),
        LT("Permanent attack power +4", "永久攻击力 +4")
    },
    {
        EXTERIOR_X(114),
        EXTERIOR_Y(99),
        SHIP_LOG_SUPPLEMENTAL,
        3,
        LT("Facility Record: Purifier Ring Control Brief", "设施记录：净化环控制简报"),
        LT("Control brief for purification, sealing, and oxygen recirculation. Restore the ring in that order and the whole region becomes less hostile. One buried annotation matters more than the rest: monolith endpoints are not the source of the danger. They are field controls holding a larger instability in check. Once you understand that, the tower stops looking like a weapon and starts looking like a choice.",
           "关于净化、封闭和氧气再循环的控制简报。按这个顺序恢复环区，整片区域都会变得不那么敌对。埋在页角的一条注释比其他内容都更重要：石碑终端不是危险的源头，它们是用来压住更大规模不稳定性的场域控制器。一旦明白这一点，塔楼看起来就不再像武器，而更像一个选择。"),
        LT("Instant oxygen +22", "立即恢复氧气 +22")
    },
    {
        EXTERIOR_X(122),
        EXTERIOR_Y(102),
        SHIP_LOG_MAINLINE,
        2,
        LT("Facility Record: Root Vault Core Dossier", "设施记录：根脉核心档案"),
        LT("Root Vault core archive. Tower signal routing, purifier control, and monolith resonance all terminate in the same buried lattice. The crash did not awaken a simple enemy; it fractured a maintenance network already struggling to contain its own world. That changes the final question. Force a rescue, rescue through understanding, or stay and inherit the work are all possible. None of them are innocent anymore, but all of them can be chosen with open eyes.",
           "根脉核心档案。塔楼信号路由、净化控制与石碑共振，最终都汇入同一片埋藏格网。坠毁唤醒的并不是一个单纯的敌人，而是击碎了一张原本就勉力维持整颗世界稳定的维护网络。这也改变了最后的问题：强行求援、通过理解完成救援，或者留下来继承这份工作，都是真实可行的路。它们已经没有哪一条还是无辜的，但每一条都能在看清真相后被选择。"),
        LT("Instant oxygen +18", "立即恢复氧气 +18")
    }
}};

const std::array<TaskMonsterSpec, 8> kMonsterSpecs = {{
    {28.0f, 3.0f, 8.0f,  {0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, false},
    {18.0f, 1.0f, 5.0f,  {0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0}, false},
    {52.0f, 6.0f, 12.0f, {0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, false},
    {48.0f, 5.0f, 10.0f, {0, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0}, false},
    {72.0f, 5.0f, 10.0f, {0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, false},
    {64.0f, 5.0f, 11.0f, {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0}, false},
    {92.0f, 10.0f, 15.0f,{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0}, false},
    {220.0f, 15.0f, 20.0f,{0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0}, true}
}};

const std::array<TaskLogRewardSpec, 5> kLogRewardSpecs = {{
    {6.0f, 0.0f, 0.0f, 0.0f, {0}},
    {0.0f, 4.0f, 0.0f, 0.0f, {0}},
    {0.0f, 0.0f, 18.0f, 0.0f, {0}},
    {0.0f, 0.0f, 22.0f, 0.0f, {0}},
    {0.0f, 0.0f, 0.0f, 0.0f, {0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}}
}};

const std::array<EventType, EVENT_COUNT> kEventCycle = {{
    EVENT_HARVEST,
    EVENT_CALM_BEASTS,
    EVENT_CLEAR_SKY,
    EVENT_SPORE_STORM,
    EVENT_MONSTER_FRENZY,
    EVENT_DEVICE_FAULT
}};

template <typename T>
bool CopySeedAt(const T &items, int index, typename T::value_type *output) {
    if (output == NULL || index < 0 || index >= static_cast<int>(items.size())) {
        return false;
    }

    *output = items[static_cast<std::size_t>(index)];
    return true;
}

}  // namespace

int TasksContent_GetNodeSeedCount(void) {
    return static_cast<int>(kNodeSeeds.size());
}

bool TasksContent_GetNodeSeed(int index, TaskNodeSeed *seed) {
    return CopySeedAt(kNodeSeeds, index, seed);
}

int TasksContent_GetMonsterSeedCount(void) {
    return static_cast<int>(kMonsterSeeds.size());
}

bool TasksContent_GetMonsterSeed(int index, TaskMonsterSeed *seed) {
    return CopySeedAt(kMonsterSeeds, index, seed);
}

int TasksContent_GetLogSeedCount(void) {
    return static_cast<int>(kLogSeeds.size());
}

bool TasksContent_GetLogSeed(int index, TaskLogSeed *seed) {
    return CopySeedAt(kLogSeeds, index, seed);
}

bool TasksContent_GetMonsterSpec(MonsterType type, TaskMonsterSpec *spec) {
    const int monsterIndex = (int)type;

    if (spec == NULL || monsterIndex < 0 || monsterIndex >= static_cast<int>(kMonsterSpecs.size())) {
        return false;
    }

    *spec = kMonsterSpecs[static_cast<std::size_t>(monsterIndex)];
    return true;
}

bool TasksContent_GetLogRewardSpec(int rewardKind, TaskLogRewardSpec *spec) {
    if (spec == NULL || rewardKind < 0 || rewardKind >= static_cast<int>(kLogRewardSpecs.size())) {
        return false;
    }

    *spec = kLogRewardSpecs[static_cast<std::size_t>(rewardKind)];
    return true;
}

EventType TasksContent_GetDailyEvent(int dayCount) {
    if (dayCount < 0) {
        dayCount = 0;
    }

    return kEventCycle[static_cast<std::size_t>(dayCount % static_cast<int>(kEventCycle.size()))];
}
