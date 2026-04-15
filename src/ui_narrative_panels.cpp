#include "ui_story_internal.h"

#include "ui_runtime_internal.h"

#include <cmath>

#define LT(en, zh) LocalizedText{en, zh}

static const IntroSlideDef kIntroSlideDefs[INTRO_CUTSCENE_SLIDE_COUNT] = {
    {
        LT("Signal Trace 01", "信号轨迹 01"),
        LT("An Unmarked Call", "一通无标识呼叫"),
        LT("The return run should have been quiet. Then a distress pulse cut through the cockpit speakers. No registry. No ship ID. Only coordinates wrapped around a world that should not have been there.",
           "返航本该平静无事。可一道求救脉冲突然穿透驾驶舱扬声器。没有注册信息，没有船只编号，只有一组坐标，指向一颗本不该出现在这里的星球。"),
        Color{114, 226, 255, 255},
        Color{12, 34, 58, 255},
        Color{4, 10, 20, 255}
    },
    {
        LT("Impact Record 02", "撞击记录 02"),
        LT("Orbit Collapse", "轨道坠毁"),
        LT("The moment the ship entered the upper atmosphere, the signal surged. Navigation died. Thrusters stalled. Something inside the storm reached up and dragged the hull straight down.",
           "飞船进入高层大气的瞬间，信号骤然增强。导航失效，推进器熄火。风暴中有什么东西伸了出来，把船体直直拽向地面。"),
        Color{255, 169, 129, 255},
        Color{78, 26, 24, 255},
        Color{18, 8, 14, 255}
    },
    {
        LT("Impact Record 03", "撞击记录 03"),
        LT("Barely Alive", "勉强活着"),
        LT("You wake to twisted metal, emergency sparks, and the taste of smoke. Main power is gone. Oxygen reserves are falling. The ship is still standing, but only by habit and luck.",
           "你在扭曲的金属、应急火花和烟雾味里醒来。主电力已经断了，氧气储备正在下降。飞船还没倒下，只是靠着惯性和运气苟着。"),
        Color{255, 214, 154, 255},
        Color{34, 33, 52, 255},
        Color{8, 10, 18, 255}
    },
    {
        LT("Survey Trace 04", "勘测轨迹 04"),
        LT("A World That Watches Back", "会回望你的世界"),
        LT("Outside, the air is thin but survivable. Scans catch movement in the dark and structures no human survey ever logged. The signal is still broadcasting from deeper inland, as if it knows you survived.",
           "外面的空气稀薄，却勉强能活。扫描捕捉到黑暗中的动静，以及人类勘测从未记录过的结构。那道信号仍在内陆深处持续发射，像是早就知道你会活下来。"),
        Color{137, 238, 186, 255},
        Color{16, 52, 42, 255},
        Color{4, 18, 20, 255}
    },
    {
        LT("Loxi Boot 05", "洛希启动 05"),
        LT("Stay Alive First", "先活下来"),
        LT("Your portable terminal flickers back to life. Gather supplies. Restore oxygen and power. Repair the comms array. Stay alive long enough to learn why this planet called you here.",
           "你的便携终端闪烁着重新启动。收集补给，恢复氧气与电力，修好通讯阵列。先活得够久，才能弄明白这颗星球为什么把你叫到这里。"),
        Color{166, 255, 226, 255},
        Color{18, 38, 64, 255},
        Color{5, 10, 19, 255}
    }
};

static const IntroSlideDef kStoryMainDefs[STORY_MAIN_SCENE_COUNT] = {
    {
        LT("Repair Log", "修复记录"),
        LT("Air For One More Day", "再撑一天的空气"),
        LT("The lower-deck oxygen console coughs back to life under your hands. It is not a full repair, only enough clean air to keep the wreck from becoming your tomb tonight.",
           "下层甲板的氧气控制台在你手中勉强恢复运转。这还谈不上完全修好，却足够让这艘残骸今夜不至于变成你的坟墓。"),
        Color{124, 225, 255, 255},
        Color{10, 34, 54, 255},
        Color{4, 10, 18, 255}
    },
    {
        LT("Repair Log", "修复记录"),
        LT("Breathing Room Restored", "呼吸空间恢复"),
        LT("The full oxygen cycle steadies across the base. For the first time since impact, every breath no longer feels borrowed. Survival stops being a countdown and becomes a plan.",
           "完整的氧气循环终于在基地内稳定下来。自撞击以来，你第一次觉得每一次呼吸都不再像借来的。生存不再只是倒计时，而开始变成一项计划。"),
        Color{166, 255, 226, 255},
        Color{14, 38, 58, 255},
        Color{5, 10, 19, 255}
    },
    {
        LT("Loxi Sync", "洛希同步"),
        LT("A Voice In The Wreck", "残骸中的声音"),
        LT("The upper terminal finally stabilizes. Loxi returns in full, no longer a fragment of emergency code, but a calm voice in a dead ship that still remembers the mission and the signal waiting outside.",
           "上层终端终于稳定下来。洛希完整归来，不再只是残缺的应急代码，而成了这艘死船里仍记得任务和外头那道信号的冷静声音。"),
        Color{123, 225, 255, 255},
        Color{16, 34, 62, 255},
        Color{6, 10, 18, 255}
    },
    {
        LT("Airlock Cycle", "气闸循环"),
        LT("The World Outside Opens", "外面的世界打开了"),
        LT("The blast door unlocks with a grinding shudder. Beyond the threshold is breathable air, unstable ground, and a landscape that feels less discovered than waiting.",
           "防爆门伴着沉重摩擦缓缓开启。门外有可呼吸的空气、不稳定的地面，以及一片与其说被发现、不如说一直在等待你的景观。"),
        Color{255, 214, 154, 255},
        Color{34, 34, 46, 255},
        Color{8, 10, 16, 255}
    },
    {
        LT("Relay Repair", "中继修复"),
        LT("The Signal Answers Back", "信号回应了"),
        LT("The comm relay rises from static and dead metal. Loxi confirms the source is still active, but what comes through is not a distress call. It behaves more like a summons that never expected refusal.",
           "通讯中继从静电和死寂金属里重新响起。洛希确认信号源仍然活跃，但传来的并不像求救，而更像一场从未预想你会拒绝的召唤。"),
        Color{110, 233, 255, 255},
        Color{14, 36, 56, 255},
        Color{5, 10, 18, 255}
    },
    {
        LT("Crash Analysis", "坠毁分析"),
        LT("This Was Not An Accident", "这不是意外"),
        LT("The wreckage tells a different story than the cockpit logs. Burn marks, drag traces, and warped readings suggest the ship was pulled down, not simply lost to weather or bad luck.",
           "残骸讲述的故事与驾驶舱日志完全不同。灼烧痕、拖拽轨迹和扭曲读数都表明，飞船是被硬生生拉下来的，而不只是输给天气或坏运气。"),
        Color{255, 186, 145, 255},
        Color{56, 24, 24, 255},
        Color{18, 8, 12, 255}
    },
    {
        LT("Power Restore", "动力恢复"),
        LT("The Base Wakes Up", "基地醒来了"),
        LT("When the energy console comes online, dormant systems across the ship answer in sequence. Light returns to empty rooms. Loxi can finally scan farther inland, and every new trace points toward the ruins.",
           "当动力控制台重新上线，飞船各处沉睡的系统逐个回应。灯光回到空荡舱室，洛希终于能向更远的内陆扫描，而每一道新痕迹都指向遗迹。"),
        Color{166, 255, 226, 255},
        Color{18, 40, 62, 255},
        Color{6, 10, 18, 255}
    },
    {
        LT("Relic Analysis", "遗物分析"),
        LT("The Pattern Is Alien", "这套模式并非人类所有"),
        LT("Three relic fragments are enough for Loxi to decode the signal structure. It is partly mathematical, partly architectural, and unmistakably non-human. The tower can be forced, but it can also be understood.",
           "三枚遗迹碎片已足够让洛希解码信号结构。它既像数学，又像建筑，而且毫无疑问并非人类产物。塔楼既可以被强行启动，也可以被真正理解。"),
        Color{174, 226, 255, 255},
        Color{22, 36, 66, 255},
        Color{6, 10, 20, 255}
    },
    {
        LT("Monolith Contact", "石碑接触"),
        LT("The Ruins Notice You", "遗迹注意到了你"),
        LT("The first monolith stirs as if it has been waiting for a witness. Its light feels less like magic and more like an ancient security system deciding whether you are allowed to go any farther.",
           "第一座石碑苏醒了，像是一直在等待见证者。它的光不像魔法，更像某个古老安保系统正在判断你是否有资格继续前进。"),
        Color{137, 238, 186, 255},
        Color{16, 42, 40, 255},
        Color{4, 14, 18, 255}
    },
    {
        LT("Monolith Harmony", "石碑共鸣"),
        LT("The Sequence Holds", "序列已稳固"),
        LT("All three monoliths resonate in perfect order. Something deep in the ruin shifts beneath your feet. Loxi believes the guardian has just lost part of whatever power kept it waiting above you.",
           "三座石碑按完美顺序同时共鸣。你脚下的遗迹深处传来变化。洛希认为，那只守卫刚刚失去了一部分让它盘踞于此的力量。"),
        Color{166, 255, 226, 255},
        Color{14, 44, 46, 255},
        Color{4, 16, 20, 255}
    },
    {
        LT("Guardian Fall", "守卫倒下"),
        LT("The Last Barrier Breaks", "最后的阻隔崩塌"),
        LT("The ruins fall quiet as the guardian collapses. No triumph answers back, only the sudden stillness that comes when the last barrier between you and the tower is gone.",
           "守卫倒下时，整片遗迹骤然安静。没有凯旋回应，只有你与塔楼之间最后一道阻隔消失后突如其来的寂静。"),
        Color{255, 198, 156, 255},
        Color{44, 24, 24, 255},
        Color{14, 8, 12, 255}
    },
    {
        LT("Rescue Route", "救援路线"),
        LT("Beacon Through Force", "以强硬点亮信标"),
        LT("With the guardian down, the signal tower obeys manual control. The rescue beacon cuts into the sky, carrying proof that someone survived long enough to fight this world and leave a scar on it.",
           "守卫倒下后，信号塔服从人工控制。求救信标刺入天际，带着某人曾活得足够久、足以与这颗星球正面对抗并留下伤痕的证据。"),
        Color{255, 214, 154, 255},
        Color{44, 28, 20, 255},
        Color{12, 8, 12, 255}
    },
    {
        LT("Rescue Route", "救援路线"),
        LT("Beacon Through Understanding", "以理解点亮信标"),
        LT("The amplifier folds human systems into the tower's language. No last battle. No final breach. The signal rises because, at last, the ruins accept your presence instead of trying to erase it.",
           "信号放大器把人类系统嵌入塔楼的语言。没有最终战，也没有最后的强闯。信号之所以升起，是因为遗迹终于接受了你的存在，而不再试图抹除你。"),
        Color{166, 255, 226, 255},
        Color{18, 44, 44, 255},
        Color{6, 12, 18, 255}
    },
    {
        LT("West Route", "西线调查"),
        LT("Echo Basin Lock", "回声盆地锁定"),
        LT("Three-point echo traces finally align in the basin. The westward crew trail does not end in silence. One courier carried the decisive data south, turning a missing-person search into a system map.",
           "回声盆地里的三点回声终于对齐。西进队伍的痕迹并没有死在沉默里。有人曾把关键数据送往南侧，让一条失踪线索真正变成了系统结构图。"),
        Color{161, 236, 255, 255},
        Color{18, 42, 58, 255},
        Color{6, 12, 20, 255}
    },
    {
        LT("Loxi Analysis", "洛希分析"),
        LT("Loxi Route Rewrite", "洛希路线重写"),
        LT("Back inside the ship, the terminal fills with rewritten route tables and quieter conclusions. Rescue, stabilization, and settlement stop reading like abstract endings and start feeling like real futures with distinct cost, risk, and weight.",
           "回到飞船后，终端上重新铺满了被改写的路线表和更冷静的结论。求援、稳定环境和留下定居不再像抽象结局，而开始显出各自清楚的代价、风险与分量。"),
        Color{146, 228, 255, 255},
        Color{18, 34, 60, 255},
        Color{6, 10, 20, 255}
    },
    {
        LT("West Archive", "西线档案"),
        LT("Last Camp Archive", "最后营地档案"),
        LT("The last camp does not hand you one answer. It preserves three positions side by side: leave now, stabilize first, or stay and inherit the work. The ending choice stops feeling accidental.",
           "最后营地并没有交给你唯一答案。它把三种立场并排保留下来：立刻离开、先稳定环境，或留下继承这份工作。终局选择不再像一时冲动。"),
        Color{255, 216, 174, 255},
        Color{40, 28, 22, 255},
        Color{12, 8, 14, 255}
    },
    {
        LT("South Route", "南线设施"),
        LT("Purifier Ring Boot", "净化环启动"),
        LT("The purifier ring answers one subsystem at a time. Filtration, sealing, and oxygen control come back in sequence, proving the south side was always an engineered lifeline rather than a dead ruin.",
           "净化环按子系统顺序一段段醒来。净化、密封和供氧控制逐步恢复，也证明南侧从来不是死掉的废墟，而是一条仍能接管的工程命脉。"),
        Color{182, 240, 214, 255},
        Color{18, 44, 42, 255},
        Color{6, 12, 18, 255}
    },
    {
        LT("Risk Update", "风险更新"),
        LT("Global Risk Drop", "全局风险下降"),
        LT("As the ring settles into rhythm, the whole region answers. Toxic pressure eases, routes stop feeling like gambles, and even the tower's shadow grows less absolute. For the first time, Loxi can describe the endgame as a damaged system instead of a curse.",
           "当净化环重新进入稳定节律，整片区域都跟着作出回应。毒性压力回落，路线不再像赌命，而连塔区投下来的阴影都不再那么绝对。洛希也第一次能把终局描述成一套受损系统，而不是诅咒。"),
        Color{166, 255, 226, 255},
        Color{16, 46, 42, 255},
        Color{6, 12, 18, 255}
    },
    {
        LT("Vault Record", "密库记录"),
        LT("Root Vault Core", "根脉核心"),
        LT("The deep archive confirms the tower, purifier ring, and monoliths were never separate mysteries. They are one maintenance lattice, and the crash only made its failing balance visible.",
           "深层档案确认，塔楼、净化环和石碑从来不是彼此独立的谜团。它们原本就是同一张维护格网，而坠毁只是把它失衡的真相暴露了出来。"),
        Color{204, 222, 255, 255},
        Color{22, 34, 62, 255},
        Color{8, 10, 20, 255}
    },
    {
        LT("Cross Analysis", "跨区对照"),
        LT("Trace Correlation", "痕迹对照"),
        LT("Laid side by side, the west handoff records and south maintenance notes finally lock into one timeline. What looked like scattering resolves into coordination: windows held open, routes handed off, survival treated like shared work instead of private panic.",
           "当西线交接记录和南线维护笔记并排展开时，它们终于锁成了同一条时间线。原本像四散失序的痕迹，如今显出真正的样子：窗口被接力维持，路线被彼此转交，求生从来不是各自逃命，而是一份被共同承担的工作。"),
        Color{166, 235, 255, 255},
        Color{18, 38, 58, 255},
        Color{6, 10, 18, 255}
    },
    {
        LT("Loxi Sync", "洛希同步"),
        LT("Loxi Sync Rewrite", "洛希同步重写"),
        LT("With west and south findings finally aligned, even Loxi's final briefing changes tone. Force, repair, and inheritance no longer read like mere route options. They read like different answers to the same burden.",
           "当西线和南线的结论终于对齐后，就连洛希给出的终局简报都变了语气。强行突破、完成修复和留下继承不再只是路线选项，而开始像是对同一份重担作出的三种不同回答。"),
        Color{174, 226, 255, 255},
        Color{18, 34, 64, 255},
        Color{6, 10, 20, 255}
    },
    {
        LT("Final Briefing", "终局简报"),
        LT("Final Stance", "最终立场"),
        LT("Before the tower, before departure, before the choice hardens into consequence, the ship asks one last question. Not whether you can act, but what exactly you mean to preserve by acting now.",
           "在塔前、在离开之前、在选择真正凝成后果之前，飞船最后问出一个问题。它问的已经不再是你能不能行动，而是你此刻行动，究竟想保住什么。"),
        Color{255, 214, 154, 255},
        Color{42, 26, 20, 255},
        Color{10, 8, 12, 255}
    }
};

static const IntroSlideDef kStoryLogDefs[STORY_LOG_SCENE_COUNT] = {
    {
        LT("Ship Log 01", "飞船日志 01"),
        LT("Impact Protocol", "撞击应对"),
        LT("The first recovered entry confirms the crew survived impact long enough to define the new rule of survival: secure air, accept the crash, and delay fear until the ship can keep someone breathing.",
           "第一份回收记录证实，船员们在撞击后活得足够久，足以定下一条新的生存法则：先保住空气，接受坠毁，把恐惧延后到飞船还能让人继续呼吸之后。"),
        Color{123, 225, 255, 255},
        Color{14, 34, 58, 255},
        Color{4, 10, 18, 255}
    },
    {
        LT("Ship Log 02", "飞船日志 02"),
        LT("Split Roster", "分组名单"),
        LT("The second log reframes the missing crew as a deliberate split. Different teams pushed west, east, and ship-side maintenance so at least one route could carry answers home.",
           "第二份日志把“失踪船员”重新定义为一次主动分组。不同小队分别推进西线、东线和飞船维护，只为确保至少有一条路线能把答案带回去。"),
        Color{255, 214, 154, 255},
        Color{42, 30, 24, 255},
        Color{10, 8, 14, 255}
    },
    {
        LT("Ship Log 03", "飞船日志 03"),
        LT("Pattern, Not Wilderness", "这里是模式，不是荒野"),
        LT("The third log suggests the planet is engineered rather than wild. Monoliths, purifier rhythms, and tower signal all belong to one unfinished system that still reacts to every intrusion.",
           "第三份日志暗示，这颗星球并非天然荒野，而是被建造出来的系统。石碑、净化节律和塔楼信号都属于同一套尚未完成、却仍会对每次入侵作出反应的装置。"),
        Color{166, 255, 226, 255},
        Color{18, 42, 40, 255},
        Color{6, 12, 18, 255}
    },
    {
        LT("Field Record 04", "现场记录 04"),
        LT("West Signal Fragment 01", "西线信号碎片 01"),
        LT("The west scouts did not chase noise at random. They were already tracing a repeated fragment that matched the ship's distress distortion, suggesting the signal had structure from the start.",
           "西线侦察员并不是在盲追杂音。他们早就抓到了一段会重复出现的碎片信号，它和飞船遇到的失真源相呼应，说明这道召唤从一开始就有结构。"),
        Color{158, 232, 255, 255},
        Color{18, 38, 58, 255},
        Color{6, 10, 18, 255}
    },
    {
        LT("Field Record 05", "现场记录 05"),
        LT("Survey Break Anchor Notes", "勘测断崖锚点笔记"),
        LT("Anchor placements across Survey Break read less like a retreat and more like deliberate relay work. The west team was building a stable chain to pass people and evidence deeper inland.",
           "勘测断崖一带的锚点布置看起来不像撤退，更像一次有计划的接力。西线队伍是在搭一条能把人和证据继续往内陆传下去的稳定链路。"),
        Color{255, 214, 154, 255},
        Color{40, 30, 22, 255},
        Color{10, 8, 14, 255}
    },
    {
        LT("Field Record 06", "现场记录 06"),
        LT("Canopy Handoff Record", "林冠交接记录"),
        LT("A quiet exchange under the canopy confirms the missing crew split roles on purpose. One branch moved information, another kept routes usable, and both expected the south side to matter later.",
           "林冠下的一次安静交接证明，失踪船员是有意分工的。一组负责转运信息，一组负责维持路线可用，而双方都预料到南侧设施迟早会变得关键。"),
        Color{166, 255, 226, 255},
        Color{16, 44, 40, 255},
        Color{6, 12, 18, 255}
    },
    {
        LT("Field Record 07", "现场记录 07"),
        LT("Echo Basin Topology Sketch", "回声盆地拓扑草图"),
        LT("The basin sketch turns broken pings into a topology map. What looked like disappearances now reads like routing: one trail bending west, one trail pointing south, both converging on the same hidden system.",
           "回声盆地的草图把破碎脉冲整理成了拓扑图。原本像是失踪的空白，如今更像一次路由：一条线偏向西侧，一条线通往南侧，而两者最终都指向同一套隐藏系统。"),
        Color{166, 235, 255, 255},
        Color{18, 38, 58, 255},
        Color{6, 10, 18, 255}
    },
    {
        LT("Field Record 08", "现场记录 08"),
        LT("Last Camp Testament", "最后营地遗言"),
        LT("The final camp refuses a single heroic answer. The surviving crew weighed rescue, repair, and settlement as three honest futures, making your eventual choice feel less like improvisation and more like an inheritance.",
           "最后营地并不拥护唯一的英雄答案。留守者把求援、修复和定居并列成三种诚实的未来，也让你最后的选择不再像临时起意，而更像继承了他们留下来的判断。"),
        Color{255, 216, 174, 255},
        Color{42, 28, 22, 255},
        Color{12, 8, 14, 255}
    },
    {
        LT("Crash Record 09", "坠毁记录 09"),
        LT("Black Box Residue", "黑匣残留"),
        LT("Recovered residue shows the eastern wreck tried to steer away from the same lattice later mapped from the west. The crash was already entangled with a region-wide control failure.",
           "恢复出来的残留记录显示，东侧残骸最后尝试避开的，正是后来在西线被描出的那片格网。坠毁从一开始就与整片区域的控制失效缠在一起。"),
        Color{204, 222, 255, 255},
        Color{22, 34, 62, 255},
        Color{8, 10, 20, 255}
    },
    {
        LT("Facility Record 10", "设施记录 10"),
        LT("Purifier Outage Memo", "净化器停摆备忘"),
        LT("The purifier memo reframes the toxic fog as maintenance collapse rather than pure hostility. The region became harsher because the system lost caretakers, not because the world suddenly changed its mind.",
           "净化器备忘把毒雾重新定义成维护链断裂后的后果，而不是纯粹敌意。区域会变得更凶险，是因为系统失去了看护者，而不是世界突然起了杀心。"),
        Color{182, 240, 214, 255},
        Color{18, 44, 40, 255},
        Color{6, 12, 18, 255}
    },
    {
        LT("Facility Record 11", "设施记录 11"),
        LT("Vent Calibration Handover", "通风校准交接"),
        LT("The vent handover proves west and south teams were buying each other time. Cleaner air and quieter signal windows were part of the same collaborative survival plan.",
           "通风校准交接证明，西线和南线队伍当时是在互相争取时间。更干净的空气和更安静的信号窗口，本来就是同一套协作求生计划。"),
        Color{166, 255, 226, 255},
        Color{18, 42, 42, 255},
        Color{6, 12, 18, 255}
    },
    {
        LT("Facility Record 12", "设施记录 12"),
        LT("Service Shaft Sync Record", "维护井道同步记录"),
        LT("The shaft record makes the infrastructure link explicit: ship systems, purifier controls, and the buried tower lattice all share one backbone. Restoring access means entering the same machine from different doors.",
           "井道同步记录把基础设施关系写得很直接：飞船系统、净化控制和埋藏的塔楼格网共享同一条主干。重新打通它们，等于从不同入口进入同一台机器。"),
        Color{204, 222, 255, 255},
        Color{22, 34, 62, 255},
        Color{8, 10, 20, 255}
    },
    {
        LT("Facility Record 13", "设施记录 13"),
        LT("Purifier Ring Control Brief", "净化环控制简报"),
        LT("The ring brief reveals the monoliths were never the source of the danger. They were dampers. Once that is clear, the tower stops reading as a weapon and starts reading as a responsibility.",
           "净化环控制简报揭示，石碑从来不是危险源，而是阻尼器。一旦这一点成立，塔楼看起来就不再像武器，而更像一份必须承担的职责。"),
        Color{174, 226, 255, 255},
        Color{18, 34, 64, 255},
        Color{6, 10, 20, 255}
    },
    {
        LT("Facility Record 14", "设施记录 14"),
        LT("Root Vault Core Dossier", "根脉核心档案"),
        LT("The core dossier joins every thread together: tower signal, purifier ring, monolith resonance, and the crash all belong to one damaged world-maintenance lattice. The endgame becomes a question of stewardship.",
           "根脉核心档案把所有线索真正接成了一体：塔楼信号、净化环、石碑共振与坠毁本身，都属于同一张受损的世界维护格网。终局因此变成了如何承担的问题。"),
        Color{204, 222, 255, 255},
        Color{20, 32, 62, 255},
        Color{8, 10, 20, 255}
    }
};

const IntroSlideDef *UIStory_GetIntroSlideDef(int slideIndex) {
    if (slideIndex < 0 || slideIndex >= INTRO_CUTSCENE_SLIDE_COUNT) {
        return &kIntroSlideDefs[0];
    }

    return &kIntroSlideDefs[slideIndex];
}

static bool IsMainStoryScene(StoryScene scene) {
    return scene >= STORY_SCENE_MAIN_OXYGEN_PATCH
        && scene < (StoryScene)(STORY_SCENE_MAIN_OXYGEN_PATCH + STORY_MAIN_SCENE_COUNT);
}

static bool IsLogStoryScene(StoryScene scene) {
    return scene >= STORY_SCENE_LOG_THE_CRASH
        && scene < (StoryScene)(STORY_SCENE_LOG_THE_CRASH + STORY_LOG_SCENE_COUNT);
}

const IntroSlideDef *UIStory_GetStorySceneDef(StoryScene scene) {
    if (IsMainStoryScene(scene)) {
        return &kStoryMainDefs[scene - STORY_SCENE_MAIN_OXYGEN_PATCH];
    }

    if (IsLogStoryScene(scene)) {
        return &kStoryLogDefs[scene - STORY_SCENE_LOG_THE_CRASH];
    }

    return &kStoryMainDefs[0];
}

const TextureAsset *UIStory_GetStorySceneTexture(const AssetBundle *assets, StoryScene scene) {
    if (IsMainStoryScene(scene)) {
        return &assets->storyMainScenes[scene - STORY_SCENE_MAIN_OXYGEN_PATCH];
    }

    if (IsLogStoryScene(scene)) {
        return &assets->storyLogScenes[scene - STORY_SCENE_LOG_THE_CRASH];
    }

    return NULL;
}

static void DrawCutsceneFallbackBackground(const IntroSlideDef *slide, int screenWidth, int screenHeight, float scale, float elapsed) {
    float pulse;

    pulse = 0.5f + std::sin(elapsed * 1.2f) * 0.5f;
    DrawRectangleGradientV(0, 0, screenWidth, screenHeight, slide->fallbackTop, slide->fallbackBottom);
    DrawCircleGradient((int)(screenWidth * 0.24f), (int)(screenHeight * 0.26f), 180.0f * scale, Color{slide->accent.r, slide->accent.g, slide->accent.b, (unsigned char)(34 + pulse * 20.0f)}, Color{slide->accent.r, slide->accent.g, slide->accent.b, 0});
    DrawCircleGradient((int)(screenWidth * 0.76f), (int)(screenHeight * 0.30f), 210.0f * scale, Color{slide->accent.r, slide->accent.g, slide->accent.b, (unsigned char)(18 + pulse * 16.0f)}, Color{slide->accent.r, slide->accent.g, slide->accent.b, 0});
}

void UIStory_DrawBackdropTexture(const TextureAsset *asset,
                                 const IntroSlideDef *slide,
                                 int slideIndex,
                                 int screenWidth,
                                 int screenHeight,
                                 float elapsed,
                                 float scale) {
    Rectangle source;
    Rectangle dest;
    float zoom;
    float sourceWidth;
    float sourceHeight;
    float travelX;
    float travelY;

    if (asset == NULL || !asset->loaded || asset->texture.width <= 0 || asset->texture.height <= 0) {
        DrawCutsceneFallbackBackground(slide, screenWidth, screenHeight, scale, elapsed);
        return;
    }

    zoom = 1.05f + 0.012f * std::sin(elapsed * 0.7f + slideIndex * 0.6f);
    sourceWidth = (float)asset->texture.width / zoom;
    sourceHeight = (float)asset->texture.height / zoom;
    travelX = std::sin(elapsed * 0.28f + slideIndex * 0.75f) * ((float)asset->texture.width - sourceWidth) * 0.25f;
    travelY = std::cos(elapsed * 0.22f + slideIndex * 0.65f) * ((float)asset->texture.height - sourceHeight) * 0.22f;
    source = Rectangle{
        ((float)asset->texture.width - sourceWidth) * 0.5f + travelX,
        ((float)asset->texture.height - sourceHeight) * 0.5f + travelY,
        sourceWidth,
        sourceHeight
    };
    if (source.x < 0.0f) {
        source.x = 0.0f;
    }
    if (source.y < 0.0f) {
        source.y = 0.0f;
    }
    if (source.x + source.width > (float)asset->texture.width) {
        source.x = (float)asset->texture.width - source.width;
    }
    if (source.y + source.height > (float)asset->texture.height) {
        source.y = (float)asset->texture.height - source.height;
    }

    dest = Rectangle{0.0f, 0.0f, (float)screenWidth, (float)screenHeight};
    DrawTexturePro(asset->texture, source, dest, Vector2{0.0f, 0.0f}, 0.0f, WHITE);
}

void UIStory_DrawNarrativePanel(const AssetBundle *assets,
                                const IntroSlideDef *slide,
                                Rectangle panel,
                                Rectangle bodyRect,
                                Vector2 eyebrowPosition,
                                Vector2 titlePosition,
                                const char *hint,
                                float scale,
                                int screenWidth,
                                int screenHeight) {
    Vector2 hintSize;

    DrawRectangleGradientV(0, 0, screenWidth, screenHeight, Color{4, 8, 14, 36}, Color{2, 6, 12, 220});
    DrawRectangleGradientV(0, screenHeight / 2, screenWidth, screenHeight / 2, Color{3, 7, 12, 0}, Color{3, 7, 12, 220});
    UIRuntime_DrawPanel(panel, Color{6, 12, 21, 196}, Color{slide->accent.r, slide->accent.g, slide->accent.b, 78});
    UIRuntime_DrawText(assets, Loc_PickText(slide->eyebrow), eyebrowPosition, 15.0f * scale, slide->accent);
    UIRuntime_DrawText(assets, Loc_PickText(slide->title), titlePosition, 33.0f * scale, WHITE);
    UIRuntime_DrawWrappedText(assets, Loc_PickText(slide->body), bodyRect, 18.0f * scale, 22.0f * scale, Color{223, 234, 242, 255});

    hintSize = UIRuntime_MeasureText(assets, hint, 14.5f * scale);
    UIRuntime_DrawText(assets,
                       hint,
                       Vector2{screenWidth - hintSize.x - 28.0f * scale, screenHeight - 30.0f * scale},
                       14.5f * scale,
                       Color{192, 206, 220, 255});
}

void UI_DrawOpeningCutscene(const AssetBundle *assets, int slideIndex, float slideElapsed, int screenWidth, int screenHeight) {
    const IntroSlideDef *slide;
    float scale;
    Rectangle textPanel;
    Rectangle bodyRect;
    Vector2 titlePosition;
    Vector2 eyebrowPosition;
    const char *hint;
    int dotIndex;

    slide = UIStory_GetIntroSlideDef(slideIndex);
    scale = UIRuntime_GetScale(screenWidth, screenHeight);
    textPanel = Rectangle{
        screenWidth * 0.5f - 470.0f * scale,
        screenHeight - 248.0f * scale,
        940.0f * scale,
        176.0f * scale
    };
    bodyRect = Rectangle{
        textPanel.x + 32.0f * scale,
        textPanel.y + 74.0f * scale,
        textPanel.width - 64.0f * scale,
        78.0f * scale
    };
    eyebrowPosition = Vector2{textPanel.x + 32.0f * scale, textPanel.y + 22.0f * scale};
    titlePosition = Vector2{textPanel.x + 30.0f * scale, textPanel.y + 42.0f * scale};
    hint = slideIndex == INTRO_CUTSCENE_SLIDE_COUNT - 1
        ? Loc_PickLiteral("Enter / click to deploy. ESC to skip.", "按 Enter 或点击开始部署，按 ESC 跳过。")
        : Loc_PickLiteral("Enter / click to continue. ESC to skip.", "按 Enter 或点击继续，按 ESC 跳过。");

    UIStory_DrawBackdropTexture(&assets->introSlides[slideIndex], slide, slideIndex, screenWidth, screenHeight, slideElapsed, scale);
    UIStory_DrawNarrativePanel(assets, slide, textPanel, bodyRect, eyebrowPosition, titlePosition, hint, scale, screenWidth, screenHeight);

    for (dotIndex = 0; dotIndex < INTRO_CUTSCENE_SLIDE_COUNT; dotIndex++) {
        float x;
        float y;
        float radius;
        Color tint;

        x = screenWidth * 0.5f - 44.0f * scale + dotIndex * 22.0f * scale;
        y = textPanel.y - 24.0f * scale;
        radius = dotIndex == slideIndex ? 6.0f * scale : 4.0f * scale;
        tint = dotIndex == slideIndex ? slide->accent : Color{171, 187, 201, 104};
        DrawCircleV(Vector2{x, y}, radius, tint);
    }
}

void UI_DrawStoryScene(const AssetBundle *assets, StoryScene scene, float sceneElapsed, int screenWidth, int screenHeight) {
    const IntroSlideDef *slide;
    const TextureAsset *texture;
    float scale;
    Rectangle textPanel;
    Rectangle bodyRect;
    Vector2 eyebrowPosition;
    Vector2 titlePosition;
    const char *hint;

    slide = UIStory_GetStorySceneDef(scene);
    texture = UIStory_GetStorySceneTexture(assets, scene);
    scale = UIRuntime_GetScale(screenWidth, screenHeight);
    textPanel = Rectangle{
        screenWidth * 0.5f - 490.0f * scale,
        screenHeight - 252.0f * scale,
        980.0f * scale,
        182.0f * scale
    };
    bodyRect = Rectangle{
        textPanel.x + 32.0f * scale,
        textPanel.y + 76.0f * scale,
        textPanel.width - 64.0f * scale,
        82.0f * scale
    };
    eyebrowPosition = Vector2{textPanel.x + 32.0f * scale, textPanel.y + 22.0f * scale};
    titlePosition = Vector2{textPanel.x + 30.0f * scale, textPanel.y + 42.0f * scale};
    hint = Loc_PickLiteral("Enter / click to continue. ESC to close.", "按 Enter 或点击继续，按 ESC 关闭。");

    UIStory_DrawBackdropTexture(texture, slide, (int)scene, screenWidth, screenHeight, sceneElapsed, scale);
    UIStory_DrawNarrativePanel(assets, slide, textPanel, bodyRect, eyebrowPosition, titlePosition, hint, scale, screenWidth, screenHeight);
}

#undef LT
