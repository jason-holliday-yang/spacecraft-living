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
        LT("The moment the ship entered the upper atmosphere, the signal surged into a broken handshake. Navigation died. Thrusters stalled. The storm did not think; the lattice had simply mistaken your hull for something it could seize and route downward.",
           "飞船进入高层大气的瞬间，信号骤然膨胀成一次失控握手。导航失效，推进器熄火。那并不是风暴在“思考”，而是下方格网把你的船体误认成了自己还能强行接管的节点，随后一路往地表拖拽。"),
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
        LT("Outside, the air is thin but survivable. Scans catch movement in the dark and structures no human survey ever logged. The signal is still broadcasting from deeper inland, less like a mind that noticed you than a damaged system that has not stopped trying to complete a lock.",
           "外面的空气稀薄，却勉强能活。扫描捕捉到黑暗中的动静，以及人类勘测从未记录过的结构。那道信号仍在内陆深处持续发射，与其说它“知道”你活下来，不如说那套受损系统还没有放弃完成一次锁定。"),
        Color{137, 238, 186, 255},
        Color{16, 52, 42, 255},
        Color{4, 18, 20, 255}
    },
    {
        LT("Loxi Boot 05", "洛希启动 05"),
        LT("Stay Alive First", "先活下来"),
        LT("Your portable terminal flickers back to life. Gather supplies. Restore oxygen and power. Repair the comms array. Stay alive long enough to learn why this failing lattice locked onto your ship.",
           "你的便携终端闪烁着重新启动。收集补给，恢复氧气与电力，修好通讯阵列。先活得够久，才能弄明白这套失稳格网为什么会锁上你的飞船。"),
        Color{166, 255, 226, 255},
        Color{18, 38, 64, 255},
        Color{5, 10, 19, 255}
    }
};

static const IntroSlideDef kStoryMainDefs[STORY_MAIN_SCENE_COUNT] = {
    {LT("Main Story 01", "主线剧情 01"), LT("Air For One More Day", "再撑一天的空气"), LT("The wreck is still leaking smoke and your lungs still burn. This first repair is not salvation, only enough air to keep tonight from becoming the end.", "残骸还在冒烟，你的肺也还在发烫。这次修复不是得救，只是先争到今晚不至于死去的一口气。"), Color{124, 225, 255, 255}, Color{10, 34, 54, 255}, Color{4, 10, 18, 255}},
    {LT("Main Story 02", "主线剧情 02"), LT("Oxygen Cycle Restored", "第一轮氧气恢复"), LT("The lower decks finally breathe in rhythm again. Survival stops feeling like a countdown and starts becoming something you might be able to plan.", "下层甲板终于重新恢复呼吸节律。生存不再只是倒计时，而开始像一件还能筹划的事。"), Color{166, 255, 226, 255}, Color{14, 38, 58, 255}, Color{5, 10, 19, 255}},
    {LT("Main Story 03", "主线剧情 03"), LT("Loxi Full Sync", "洛希完整上线"), LT("The upper terminal stabilizes and Loxi returns as more than a warning routine. In the dead ship, one calm intelligence is finally speaking with you again.", "上层终端稳定下来，洛希不再只是告警残片。在这艘死船里，终于又有一个冷静的意识在与你并肩说话。"), Color{123, 225, 255, 255}, Color{16, 34, 62, 255}, Color{6, 10, 18, 255}},
    {LT("Main Story 04", "主线剧情 04"), LT("Airlock Opening", "气闸开启"), LT("The blast door yields with a long shudder. Beyond it is breathable air, thin fog, and a world that feels less discovered than waiting.", "防爆门在漫长的震颤中打开。门外有可呼吸的空气、稀薄的雾，以及一颗像在等你走出来的星球。"), Color{255, 214, 154, 255}, Color{34, 34, 46, 255}, Color{8, 10, 16, 255}},
    {LT("Main Story 05", "主线剧情 05"), LT("Signal Answers Back", "信号回应了"), LT("The repaired relay does not recover a plea for help. It recovers something older, calmer, and far less willing to accept refusal.", "修好的中继带回来的不是求救，而是某种更古老、更冷静、也更不打算接受拒绝的回应。"), Color{110, 233, 255, 255}, Color{14, 36, 56, 255}, Color{5, 10, 18, 255}},
    {LT("Main Story 06", "主线剧情 06"), LT("Crash Not Accident", "坠毁并非意外"), LT("Burn scars and drag marks refuse the official cockpit story. The ship was not merely lost in weather; it was pulled into a system already in motion.", "灼痕与拖拽轨迹否定了驾驶舱里的官方说法。飞船并不是单纯栽进风暴，而是被卷进一套早已运转中的系统。"), Color{255, 186, 145, 255}, Color{56, 24, 24, 255}, Color{18, 8, 12, 255}},
    {LT("Main Story 07", "主线剧情 07"), LT("Deep Scan Online", "深层扫描上线"), LT("Power returns and the ship wakes farther than before. For the first time, scattered anomalies begin to look like one connected map.", "动力回来了，飞船的感知范围也跟着苏醒。那些零散异常第一次开始显出同一张版图。"), Color{166, 255, 226, 255}, Color{18, 40, 62, 255}, Color{6, 10, 18, 255}},
    {LT("Main Story 08", "主线剧情 08"), LT("Relic Pattern Decode", "遗物模式解码"), LT("The fragment set yields part of the signal grammar. The tower is not only something to force open; it is also something that might be understood.", "整套碎片终于吐露出信号语法的一部分。塔楼不只是能被强行打开的目标，它也可能被真正理解。"), Color{174, 226, 255, 255}, Color{22, 36, 66, 255}, Color{6, 10, 20, 255}},
    {LT("Main Story 09", "主线剧情 09"), LT("East Wreck Confirmation", "东线残骸回证"), LT("The eastern residue locks the crash into the same regional lattice traced elsewhere. Accident, fog, and signal stop reading like separate mysteries.", "东线残留把坠毁与那张区域格网彻底锁到了一起。事故、毒雾与信号，不再像彼此无关的谜题。"), Color{166, 214, 255, 255}, Color{18, 34, 58, 255}, Color{6, 10, 18, 255}},
    {LT("Main Story 10", "主线剧情 10"), LT("Basin Access Qualification", "深潭资格确认"), LT("Recovering the energy core does not feel like a prize. It feels like permission to go deeper into a system that has finally decided you qualify to proceed.", "找回能源核心的感觉不像领奖，更像是被允许继续深入。那套系统终于承认你有资格往前走了。"), Color{190, 234, 255, 255}, Color{18, 34, 60, 255}, Color{6, 10, 18, 255}},
    {LT("Main Story 11", "主线剧情 11"), LT("West Route Confirmed", "西线正式确认"), LT("The western signal is no longer rumor or background noise. Someone from your own ship went this way and tried to leave a path for whoever came after.", "西线信号不再只是传闻或底噪。你们船上的确有人走过这里，并试图给后来者留下路。"), Color{161, 236, 255, 255}, Color{18, 42, 58, 255}, Color{6, 12, 20, 255}},
    {LT("Main Story 12", "主线剧情 12"), LT("Survey Break Relay", "勘测断崖接力"), LT("Anchor lines and reverse markers make the truth plain: they were not fleeing. They were building a relay chain for the next person to keep going.", "锚点与反向标记把真相说得很清楚：他们不是在溃逃，而是在给下一个人搭好继续前进的接力线。"), Color{255, 214, 154, 255}, Color{40, 30, 22, 255}, Color{10, 8, 14, 255}},
    {LT("Main Story 13", "主线剧情 13"), LT("Canopy Handoff", "林冠交接"), LT("The hidden canopy post still feels organized, not abandoned. Someone held the line here long enough to hand responsibility to whoever survived next.", "这处林冠观察点仍然保留着秩序，而不是纯粹的荒废。有人曾在这里守得足够久，把责任交给下一个活下来的人。"), Color{166, 255, 226, 255}, Color{16, 44, 40, 255}, Color{6, 12, 18, 255}},
    {LT("Main Story 14", "主线剧情 14"), LT("Echo Basin Reconstruction", "回声盆地重建"), LT("Broken pings finally resolve into a route map. What once looked like disappearance now reads like a successful handoff toward the south.", "破碎脉冲终于被重建成路线图。曾经像“失踪”的空白，如今更像一次把关键东西成功转向南侧的交接。"), Color{166, 235, 255, 255}, Color{18, 38, 58, 255}, Color{6, 10, 18, 255}},
    {LT("Main Story 15", "主线剧情 15"), LT("Last Camp Positions", "最后营地三立场"), LT("The last camp keeps three futures side by side: leave, repair, or stay. Your eventual ending stops feeling invented and starts feeling inherited.", "最后营地把离开、修复与留下三种未来并排放着。你的终局不再像临时起意，而更像继承了他们的判断。"), Color{255, 216, 174, 255}, Color{40, 28, 22, 255}, Color{12, 8, 14, 255}},
    {LT("Main Story 16", "主线剧情 16"), LT("South Facility Wakes", "南侧设施苏醒"), LT("The southern route reveals itself as machinery, not geology. Damaged, dangerous, and still repairable, it changes the way this whole world reads.", "南线终于显露出它是设施，而不是地貌。它残破、危险，却仍可被接手，也因此改写了整颗星球的读法。"), Color{182, 240, 214, 255}, Color{18, 44, 42, 255}, Color{6, 12, 18, 255}},
    {LT("Main Story 17", "主线剧情 17"), LT("Vent Network Calibrated", "通风网络校准"), LT("As the vent chain comes back online, the world answers in kind. The fog pressure eases just enough to prove the environment can still be changed.", "通风链路重新上线后，环境也开始作出回应。毒雾压力第一次真实回落，证明这颗星球仍会被修复所影响。"), Color{166, 255, 226, 255}, Color{18, 42, 42, 255}, Color{6, 12, 18, 255}},
    {LT("Main Story 18", "主线剧情 18"), LT("Service Shaft Backbone", "维护井主干连通"), LT("The service shaft lights up like exposed anatomy. Separate regions stop feeling separate once the same backbone is visible beneath them all.", "维护井像被剥开的机体骨架一样亮了起来。当地底那条共同主干显形后，各个区域也不再像彼此孤立。"), Color{204, 222, 255, 255}, Color{22, 34, 62, 255}, Color{8, 10, 20, 255}},
    {LT("Main Story 19", "主线剧情 19"), LT("Purifier Ring Sequence", "净化环恢复次序"), LT("The ring does not wake all at once; it wakes correctly. Filtration, sealing, and oxygen return in sequence, proving method matters as much as power.", "净化环并不是一下子亮起，而是按正确次序醒来。净化、密封与供氧依次回归，也证明方法和力量同样重要。"), Color{182, 240, 214, 255}, Color{18, 44, 42, 255}, Color{6, 12, 18, 255}},
    {LT("Main Story 20", "主线剧情 20"), LT("Root Vault Truth", "根脉核心真相"), LT("Deep in the vault, the tower, monoliths, purifier ring, fog, and crash all collapse into one maintenance lattice. The world stops being chaotic and starts being legible.", "在根脉密库深处，塔楼、石碑、净化环、毒雾与坠毁终于被讲成同一张维护格网。世界不再只是混乱，而开始变得可读。"), Color{204, 222, 255, 255}, Color{20, 32, 62, 255}, Color{8, 10, 20, 255}},
    {LT("Main Story 21", "主线剧情 21"), LT("West South Correlation", "西南证据对齐"), LT("West handoff notes and south maintenance records finally share one timeline. The story stops scattering and starts closing into history.", "西线交接记录与南线维护文书终于进入同一条时间线。故事不再四散，而开始真正闭合成历史。"), Color{166, 235, 255, 255}, Color{18, 38, 58, 255}, Color{6, 10, 18, 255}},
    {LT("Main Story 22", "主线剧情 22"), LT("Loxi Conclusion Rewrite", "洛希结论重写"), LT("Loxi revises its own earlier reading and becomes more trustworthy for it. The world is no longer a pile of unrelated threats, but one burden explained clearly.", "洛希主动修正了自己先前的判断，也因此变得更值得信任。这个世界不再是一堆彼此无关的威胁，而是一份被逐渐讲清楚的重担。"), Color{174, 226, 255, 255}, Color{18, 34, 64, 255}, Color{6, 10, 20, 255}},
    {LT("Main Story 23", "主线剧情 23"), LT("Monolith True Role", "石碑真正职责"), LT("The monoliths were never the source of the danger. They were dampers, tired guardians buying time for a broken system that was still trying to hold together.", "石碑从来不是危险源头。它们是阻尼器，是一群疲惫的守卫，在替一套仍想维持运转的破损系统继续拖时间。"), Color{174, 226, 255, 255}, Color{18, 34, 64, 255}, Color{6, 10, 20, 255}},
    {LT("Main Story 24", "主线剧情 24"), LT("North Route Commitment", "北线路线承诺"), LT("The path toward the tower narrows the story into responsibility. You are no longer only investigating what happened here, but approaching what must be decided here.", "通往高塔的道路让整个故事开始收束为责任。你不再只是调查发生过什么，而是在走向必须由你决定的地方。"), Color{255, 214, 154, 255}, Color{42, 26, 20, 255}, Color{10, 8, 12, 255}},
    {LT("Main Story 25", "主线剧情 25"), LT("Tower Not Button", "塔楼不是按钮"), LT("The terminal does not present a simple switch. It presents a declaration point, a place where every ending must first be acknowledged as a consequence.", "塔前终端并不是一个普通开关。它更像一个提交窗口，要求你先承认每种终局都意味着后果。"), Color{255, 214, 154, 255}, Color{42, 26, 20, 255}, Color{10, 8, 12, 255}},
    {LT("Main Story 26", "主线剧情 26"), LT("Three Costs Revealed", "三种代价浮出"), LT("Rescue, peaceful stabilization, and settlement stop reading like menu choices. Each one now carries a different answer to the same truth and a different cost to live with.", "求援、和平接入与留下定居，不再像菜单选项。它们都成了对同一真相的不同回答，也都带着必须承担的代价。"), Color{255, 216, 174, 255}, Color{40, 28, 22, 255}, Color{12, 8, 14, 255}},
    {LT("Main Story 27", "主线剧情 27"), LT("Action Declaration", "行动宣言"), LT("The choice leaves the realm of private thought and enters the record. What happens next will not be an accident, but your name placed beside a decision.", "选择离开了内心独白，正式进入记录。从这一刻起，接下来发生的一切都不再是偶然，而是你的名字与决定写在了一起。"), Color{166, 235, 255, 255}, Color{18, 38, 58, 255}, Color{6, 10, 18, 255}},
    {LT("Main Story 28", "主线剧情 28"), LT("Heroic Route Commitment", "英雄路线前章"), LT("To force the beacon alive is to put survival first even now. It is not a clean victory, only a hard human answer chosen with full knowledge of the damage.", "强行点亮信标，意味着此刻仍把“先把人送出去”放在最前。这不是干净的胜利，而是在明知代价后仍作出的强硬人类回答。"), Color{255, 214, 154, 255}, Color{44, 28, 20, 255}, Color{12, 8, 12, 255}},
    {LT("Main Story 29", "主线剧情 29"), LT("Peaceful Route Commitment", "和平路线前章"), LT("The lattice can be approached without tearing it wider apart. This route asks for restraint, compatibility, and the patience to let understanding do the lifting.", "这张格网并不一定只能被撕开。和平路线要求克制、兼容，以及让“理解”真正承担起重量的耐心。"), Color{166, 255, 226, 255}, Color{18, 44, 44, 255}, Color{6, 12, 18, 255}},
    {LT("Main Story 30", "主线剧情 30"), LT("Settlement Route Commitment", "定居路线前章"), LT("Leaving is no longer the only honest future. To stay is to clear the last rogue guardian, inherit unfinished maintenance, and accept a long duty instead of a clean escape.", "离开不再是唯一诚实的未来。选择留下，不只是继承那些未完成的维护工作，也意味着先清掉最后一道失控守卫，并接受一份漫长责任，而不是一场干净撤离。"), Color{255, 216, 174, 255}, Color{36, 28, 26, 255}, Color{10, 8, 14, 255}}
};

static const LocalizedText kStoryMainDetailTexts[STORY_MAIN_SCENE_COUNT] = {
    LT("The first repair comes with the sound of metal coughing back to life. Air pushes through the vent in short, uneven bursts, carrying rust, smoke, and the taste of heated wiring. Nothing is truly stable yet, but one corridor can be breathed in without panic, and that alone is enough to turn a crash site into a place where the night might be survived.", "第一轮修补完成时，最先回来的不是安全感，而是风声。送风口断断续续吐出空气，夹着铁锈、焦味和过热线路的苦涩，像这艘船也在勉强喘气。没有任何东西真正恢复正常，只是有一条走廊终于能让人不必一边吸气一边数剩余时间，而这一点点余地，已经足够把残骸从坟墓改写成可以熬过今夜的地方。"),
    LT("When the oxygen cycle finally steadies, the ship stops sounding like something that is dying room by room. The alarms thin out. Condensation begins to bead on cold panels instead of freezing in the mask. You can stand still for a moment and hear pumps, valves, and recycling loops finding a shared rhythm. For the first time since impact, tomorrow feels like something that can be prepared for, not merely endured.", "当氧气循环真正稳下来时，整艘船不再像一间一间慢慢死去。警报稀了下去，面罩里的白雾不再每次呼吸都扑上视线，冷下来的舱壁重新结出细小水珠。你甚至能短暂站住，听见泵体、阀门和回收装置逐渐踩到同一拍节上。自从坠毁以来，明天第一次不像审判，而像一件可以提前准备的事。"),
    LT("Loxi's full return changes the wreck more than fresh power does. Until now the ship has spoken only in warnings, buzzers, and partial readouts. Then the terminal glow stabilizes, and a clear voice begins laying one fact beside another with unnerving calm. In the half-lit metal shell, that calm becomes shelter of its own: not warmth, not comfort, but the feeling that at least one mind in this ruin is still thinking clearly with you.", "洛希完整上线之后，残骸里最明显的变化甚至不是电压恢复，而是“回应”重新出现。在此之前，飞船只会用告警、噪声和残缺读数说话；可当上层终端彻底稳定下来，一道清晰而克制的声音开始把事实一条条摆开。那种冷静在半亮半暗的金属舱里反而像另一种庇护，它不温暖，却让你第一次觉得这片废墟里至少还有一个意识仍在和你并肩思考。"),
    LT("The airlock does not open cleanly. It jerks, drags, and peels itself aside with the long complaint of old seals being forced awake. Then the outside air comes in: thin enough to keep you cautious, damp enough to cling to skin, carrying a smell of wet growth and stone that has held rain for far too long. Beyond the threshold, the planet does not feel discovered. It feels as though it has been standing there, watching the door, waiting to see who emerges alive.", "气闸不是利落地开启，而是抖着、拖着，像一圈早该报废的密封条被硬生生从沉睡里拽醒。随后门外的空气涌进来，稀薄得让人不敢松懈，潮得能贴上皮肤，还带着湿植被和久积雨水的石头气味。跨过门槛时，你面对的也不是普通风景，而是一整颗像早已守在外面、只等着看谁能活着走出来的星球。"),
    LT("Repairing the relay changes the signal from background unease into something far more personal. What comes back is not a cry for help but a pattern: timed returns, repeated handshakes, routing attempts that should have died with the crash. The more Loxi peels noise away, the clearer the implication becomes. Whatever touched your ship in the sky was not a storm making a choice. It was a damaged process still trying to complete an old lock, as if the crash had interrupted a task it never meant to abandon.", "通讯中继修好之后，那道信号不再只是让人心里发毛的背景噪音，而开始露出具体形状。回来的不是求救，而是节律、握手包和一遍遍失败后又重试的路由动作，像某个流程明明早该终止，却还在硬撑着向前推进。洛希一点点剥掉干扰后，真正令人发冷的含义也跟着显现：在高空里碰到你的，从来不是会做决定的风暴，而是一套受损却仍想完成旧任务的系统。"),
    LT("The crash clue makes the wreck impossible to treat as bad luck any longer. Metal is scorched in long arcs instead of random bursts. Hull gouges lean in the same direction, as though the ship had been dragged while still fighting for control. Readings preserved in residue don't describe impact alone; they describe correction attempts, resistance, and a final downward pull. Standing over the scarred debris, you can almost feel the moment the ship realized it was no longer falling through weather, but being taken somewhere.", "坠毁线索真正残忍的地方，在于它彻底夺走了“只是倒霉”这种解释。残骸上的灼痕不是乱炸开的，而是拉成长弧；船体上的撕裂也明显朝着同一方向倾倒，像整艘船曾一边反抗一边被拖拽。残留数据里记录的不只是撞击，还有修正、抗衡，以及最后那一下不讲理的下坠。站在那堆扭曲金属前，你几乎能反过来感到那一刻的恐惧：飞船不是失手落进了风暴里，而是被某种东西带去了地面。"),
    LT("Deep scan online is the moment chaos begins to reveal a skeleton. What used to be isolated dangers on the map now answer one another across distance: fog density matching buried valves, monolith resonance lining up with tower pulses, ruined corridors beneath what once looked like natural ground. The planet does not become safer under the new readout. It becomes legible, which is more unsettling. Wilderness is one kind of fear. A broken machine large enough to imitate a world is another.", "深层扫描上线之后，混乱第一次露出骨架。原本像各自独立的危险开始在图上彼此呼应：毒雾浓度和地下阀列对得上，石碑共振与塔楼脉冲踩在同一拍节上，所谓天然地貌下面则埋着像设施走廊一样清晰的线条。这个世界并没有因此变得更安全，它只是变得可读，而“可读”反而更让人不安。荒野是一种威胁，会运转的残破系统则是另一种。"),
    LT("When the relic fragments begin to decode, they do not hand over answers so much as a grammar. Repeated symbols stop looking decorative. Frequency shifts stop sounding like noise. Loxi starts aligning them with tower behavior, and suddenly the possibility emerges that the structure ahead may be understood rather than merely breached. That is the first real opening for mercy in the story: the realization that not every locked thing must be broken open if its language can still be learned in time.", "遗迹碎片真正吐出来的，不是现成答案，而是一套语法。那些重复纹样不再只是装饰，频率起伏也不再只是噪声；洛希把它们和塔楼反应一一对齐后，你才第一次意识到，前方那套结构也许不是只能被强行撬开。这个念头很轻，却是故事里第一道真正像希望的缝隙：如果还来得及听懂它的语言，也许就不必把所有封闭都理解成对抗。"),
    LT("The eastern wreck confirmation binds together details that were too easy to treat separately. Residue from the black box, drag signatures in the mud, and relay distortions from other routes all point back to the same regional lattice. Once that connection is made, the crash, the fog, and the tower stop being three mysteries competing for attention. They become three wounds in the same failing body, each painful on its own, each impossible to explain without the others.", "东线残骸回证把那些原本可以各自成谜的东西硬生生钉回了一起。黑匣残留、泥地里的拖痕，以及其他路线收到的失真回波，最后都落回同一片区域格网上。连接一旦成立，坠毁、毒雾和塔楼就不再是三件抢着解释自己的怪事，而像同一具身体上裂开的三道伤口，各自疼痛，却谁也离不开另外两道。"),
    LT("Recovering the energy core does not feel like looting a prize. The housing has to be braced open through heat, residue, and a chemical tang that catches in the throat. When the core finally comes free, its light reflects off ruined plating and your gloves shake from effort rather than triumph. The sensation is not victory but admission. Something deeper in the system has begun to recognize that you can still carry a load forward, and that recognition opens doors it would have kept shut to the unprepared.", "拆出能源核心时，手感更像从一具还在发热的尸体里抢回器官，而不是拿到战利品。外壳要在高温、残留和呛喉的化学气味里一点点撬开，等核心终于脱离卡槽，它的冷光照在扭曲板材上，映出来的是你因为用力过度而发颤的手套。那一刻没有胜利感，只有一种被默许的压迫感，像更深层的系统终于承认你能把重量继续往前带。"),
    LT("The west route matters because it puts other human footsteps back into the world. Once the signal trail is confirmed, the forest changes. Markers stop looking random. Broken branches start reading like caution. A tied cord, a cut notch, a burst antenna casing half-buried in damp leaves: all of it says the same thing. Someone from your ship came through here thinking clearly, tired but deliberate, and tried to turn wilderness into instructions for the next survivor.", "西线路线一旦被正式确认，树林就不再只是陌生地形。那些原本像偶然留下的痕迹忽然全都开始说话：绑在树皮上的细绳、削出来的刻痕、半埋在湿叶里的破损天线壳，全都在证明同一件事。你们船上的人真的从这里走过，而且是在疲惫、缺氧、资源紧张的情况下，仍然试图把难以辨认的环境改写成能留给后来者的路标。"),
    LT("Survey Break carries the feeling of a handoff more than a frontier. The anchors are placed with exhaustion and precision at once, exactly where a climber would need courage second and certainty first. Reverse arrows, rope fixes, and practical notes reveal people thinking beyond their own crossing. Reading them, you stop imagining a desperate retreat and start seeing disciplined relay work: crew members spending their remaining strength on the possibility that someone else might arrive later and still make it across.", "勘测断崖最强烈的气氛不是险，而是交接。那些锚点钉在最需要它们的地方，既显出体力快见底的仓促，也显出不肯出错的精准；反向箭头、补过的绳位和短短几句实用提醒，全都说明留下它们的人考虑的早已不是自己这一次怎么过去，而是下一个赶到这里的人会不会死在错误的地方。读到这里，所谓撤退就再也站不住脚，剩下的只是一群人把最后力气花在接力上的样子。"),
    LT("The canopy outpost restores tenderness to the crew's absence. Wind vanes are still trimmed. Portions are still counted. Someone kept recording visibility, signal drift, and return timing as if order itself were a duty that should not be allowed to die first. In that quiet routine, the missing stop being abstract names. They become watchers, couriers, maintainers, people who stayed at a high, damp post long enough to make sure responsibility changed hands instead of simply ending there.", "林冠观察点最让人心口发紧的，并不是危险，而是它还保留着人的耐心。风向片修过，口粮照旧分成小份，能见度和回波偏移仍被一行行记下，像有人相信秩序这东西不该比人更早死去。也正因为这份平静，失踪者才第一次不再像一串名单，而重新变成守望的人、传递的人、撑住岗位直到责任得以交到下一只手里的活人。"),
    LT("Echo Basin reconstruction is one of those rare moments when grief and relief arrive together. The broken pings do not reconstruct into a grave marker. They reconstruct into movement: a courier reaching the southern threshold, a packet pushed through, timing data preserved under interference instead of erased by it. The basin's value lies in that reversal. What looked like silence turns out to have been transfer. What looked like disappearance turns out to have been work completed out of sight.", "回声盆地重建出来的，不是一块墓碑，而是一条还在向前的线，这正是它最有力量的地方。断裂脉冲重新拼合后，你看到的不是终止，而是一次抵达：有人真的把数据包送到了南侧门槛，把关键时序从干扰里硬挤了过去。那种感觉很复杂，像悲伤被轻轻改写了一下。你以为自己面对的是彻底失联，结果看到的却是有人在没人看见的地方把事情做完了。"),
    LT("The last camp speaks with the exhausted honesty of people who have already argued the future into the ground. There are no grand slogans left there, only three positions written beside dwindling supplies, dying systems, and the names of people who did not all agree. Leave. Repair. Stay. The camp's great gift is not certainty but seriousness. It refuses to pretend one answer is painless, and because of that, every ending that follows feels inherited from real thought rather than invented by genre convention.", "最后营地留下来的，不是口号，而是争论过后的诚实。补给见底，系统半死不活，连活下来的人也没有在一个答案上彻底统一，于是三种未来被并排写下：离开、修复、留下。这个地方最珍贵的不是给出标准答案，而是拒绝假装哪条路可以不付代价。也正因为如此，终局从这里开始不再像玩法分支，而像你从前人手里接过的一场真正思考。"),
    LT("When the south facility wakes, the ground itself changes category. What had read as poisoned terrain reveals ducts, maintenance doors, sealed chambers, and systems that answer once power and pressure return in the correct order. The sensation is almost uncanny. You are no longer just crossing a hostile region; you are walking through neglected infrastructure, through the inside of a machine that has suffered too many years without hands. The world becomes less monstrous and more tragic, which can be worse.", "南侧设施真正苏醒的那一刻，脚下的世界几乎像换了一个名字。原本像天然毒地的地方陆续露出风管、检修门、密封段和按顺序回应的控制结构。那种变化近乎怪异，因为你忽然不再只是穿越险地，而是在穿过一套被长久弃置的基础设施，像走进一台多年无人接手的巨型机器内部。这个世界也因此少了几分纯粹敌意，多了几分更难承受的悲凉。"),
    LT("Vent calibration is felt in the body before it is trusted by the mind. The mask stops catching as often. The sting in the throat eases. Mist that used to cling low over the path begins to thin just enough for depth to return to the world. That tiny improvement matters because it proves the environment is not a fixed curse. It can still be influenced, still be negotiated with, still be made less lethal by careful work done in the right place and in the right order.", "通风网络校准的效果，往往是身体先察觉到，脑子才敢相信。呼吸面罩没那么频繁地卡顿了，喉咙里那种细小的灼痛退了一点，原本贴着地表不肯散的雾也终于薄到能让远近重新分开。改善幅度并不夸张，却因此更有说服力：环境不是不可更改的诅咒，只要顺序和位置对了，这个世界仍会对修复作出回应。"),
    LT("The service shafts expose the planet's spine. Once the passages light up and the alignments are understood, the old map loses its innocence. Ship systems, buried controls, purifier routing, and tower access are not remote points on different adventures. They are connected cavities inside the same body. Every reopened shaft makes that clearer, and every shortcut gained comes with a new unease: you are not getting around the machine anymore. You are going deeper into it.", "维护井道一旦点亮，整颗星球就像被人揭开了脊背。等那些通路和对位关系被真正看懂，旧地图立刻失去那种彼此独立的轻松。飞船系统、地下控制、净化链路和塔楼入口不再是几场不同冒险的目的地，而是同一具庞大机体内部连通的空腔。每重新打通一段井道，你都会得到一条捷径，也都会更清楚地意识到自己不是在绕开这台机器，而是在往它内部更深处走。"),
    LT("The purifier ring restores credibility through sequence. Lights return in sections. Pressure settles in steps. Circulation comes back only after sealing is respected. Nothing about it resembles a miracle. That is precisely why the ring changes the story. It proves that this world can still be stabilized, but only by people willing to honor process over impulse. In a setting built on collapse, order itself becomes emotional, because every correct step feels like defiance against a larger drift toward ruin.", "净化环最打动人的地方，在于它不肯像奇迹那样一次性解决一切。灯一段一段亮起，压力一级一级回落，循环也只在密封顺序被正确执行后才愿意回来。这份迟缓反而让它更可信，也更动人：它证明这个世界不是不能稳住，只是再也不接受粗暴省略。于是“按步骤来”这种听上去毫不浪漫的事，反而成了最接近希望的姿态。"),
    LT("At the root vault, scale and intimacy collide. The buried routing makes the world suddenly vast: tower signal, purifier ring, monolith resonance, fog pressure, crash vectors, all sharing one damaged maintenance lattice. Yet the scene lands personally, because the files make clear how little separated your ship from being misread as part of that work. The planet never chose you in any mystical sense. A failing system mistook you for something it could still use, and that correction makes the truth heavier, not lighter.", "抵达根脉核心时，故事第一次同时拥有了极大的尺度和极近的触感。埋藏路由把一切都接到同一张受损维护格网上：塔楼、净化环、石碑、毒雾、坠毁，全都属于同一套失衡中的系统。可它真正沉下来的瞬间，却很私人，因为档案同时说明了一件事：这颗世界从未以某种神秘意志“选中”你，它只是把你的飞船误判成了仍可兼容的一次维修响应。这个纠偏不会让一切变轻，只会让真相更重。"),
    LT("When west and south evidence finally align, the reward is not novelty but closure. Notes from the canopy, vent handovers, topology sketches, and facility records begin to explain one another with almost painful neatness. Timelines match. Motives line up. The crew stop feeling scattered across disconnected tragedies and begin to read like one fraying but coordinated effort to hold meaning together while the world came apart around them.", "西南证据真正对齐时，最强的感受不是发现新东西，而是终于闭合。林冠记录、通风交接、拓扑草图和设施文书开始彼此解释，像一张被撕碎很久的纸终于能重新拼回整句。时间线对上了，动机对上了，就连那些原本像各自失落的人也重新站回同一场努力里，成了一群在世界崩开时还想把意义维持住的人。"),
    LT("Loxi rewriting its own conclusion is one of the quietest but most important emotional turns in the story. It admits that its earlier reading leaned too heavily on available symptoms, not enough on recovered context. That humility sharpens rather than weakens it. From then on, Loxi no longer feels like an oracle speaking from above events. It feels like a companion intelligence willing to revise itself so that you do not have to carry the burden of misunderstanding all the way to the end.", "洛希重写结论的分量，来自它愿意承认自己也曾看错。它并没有把此前判断包装成必要阶段，而是明确承认自己曾经只看到了症状，没有看够背景。也正因为这样，它后来每一句话反而都更值得信任。它不再像高高在上的判定装置，而像一个愿意跟你一起纠正误读、因此更配陪你走到最后的同伴。"),
    LT("The monolith revelation hurts because it reverses fear after fear has already become habit. These stones looked ominous from a distance, and the world gave every reason to distrust them. Yet the records show they were dampers, exhausted control points, things left straining against a larger instability long after their keepers vanished. Once that truth lands, the whole region changes tone. What was terrifying remains dangerous, but it also becomes pitiable, and pity is a harder weight to carry than dread.", "石碑真相最难承受的地方，在于它让你已经养成的恐惧突然失去支点。它们从外表到回声都像威胁，整个环境也给了你足够多不去相信它们的理由；可档案最后揭示，它们其实是阻尼器，是一批在看护者消失后仍继续硬撑的场域控制点。真相落下来的瞬间，危险并没有消失，只是多出了一层更沉的东西：原来最令人害怕的物件之一，也一直在替整套系统拖住崩塌。"),
    LT("By the time the north route fully commits you, landscape has become judgment. The wind is harsher here, the elevation more exposed, the tower too large to treat as scenery. Every approach step strips away a little of the comfortable distance investigation once gave you. You are no longer gathering context for a future decision. You are walking toward the place where context ends and responsibility begins.", "等北线路线真正把你推上去时，地形本身都开始像在逼问人。风更硬，位置更高，塔楼也大得再也不能被当成背景看待。随着高度拉开，调查曾经给你的那点安全距离被一层层剥掉。你已经不是在为未来做准备，而是在往那个“理解到此为止、责任从此开始”的地方走。"),
    LT("The tower terminal matters because it denies fantasy. There is no triumphant master switch waiting at the top, no clean mechanism eager to reward arrival. What waits instead is a point of acknowledgment, a place that demands you understand what any activation means before allowing you to proceed. That is why the tower feels so heavy. It is not merely an objective. It is where the story forces intent to stop hiding behind momentum.", "塔楼终端真正沉重，是因为它不给人幻想。那里没有一个等着被按下的英雄开关，也没有一套会因为你终于抵达而自动奖励你的装置。它更像一个要求确认后果的提交点，先逼你明白每种启动意味着什么，再决定要不要放你继续。也正因为如此，塔楼才不像目标，而像一份不准意图继续躲在“已经走到这里了”后面的裁定书。"),
    LT("Once the three costs are visible side by side, the endgame ceases to feel like selection and begins to feel like character. Heroic rescue, peaceful stabilization, and settlement are not simply routes through content. They are three answers to the same damaged world: prioritize extraction, prioritize repair, or accept inheritance. The power of the moment lies in the fact that none of them are innocent. Every one saves something. Every one leaves something else marked forever.", "当三种代价被真正并排摆开时，终局就不再像选择菜单，而更像一次性格表态。强行救援、和平接入和留下定居，不只是三条内容路线，而是面对同一颗受损世界时的三种回答：先把人送出去，先把系统稳下来，或者承认自己要接手余下的年岁。这个时刻之所以有力，正是因为没有哪条路是无辜的。每条路都会救下一部分东西，也都会让另一部分东西永远留下痕迹。"),
    LT("A route declaration matters because it leaves privacy behind. Up to this point, doubt could live inside thought. Once the choice is formally entered, hesitation becomes record, and record becomes responsibility. The world does not look different the instant you commit, but you do. From here onward, what happens next carries your name beside it. The system may be damaged, the planet may be old, but the decision will no longer be accidental.", "行动宣言一旦写入记录，选择就不再属于只发生在心里的犹豫。在那之前，迟疑还能藏在沉默里；可一旦正式提交，犹豫就会变成责任，责任就会变成带名字的方向。世界在这一刻看上去并不会立刻改变，但你会。因为从这里起，之后发生的一切都不再能被轻易归进偶然。"),
    LT("The heroic route only works emotionally if it is understood as deliberate violence against a truth you already know. By now you understand the lattice is damaged, the tower is part of a maintenance world, and forcing the beacon alive will cut harder into a fragile structure. Choosing it anyway means accepting that getting people home comes first, even when home must be bought by leaving visible scars behind. That is what makes it human rather than glorious.", "英雄路线真正成立的前提，是你已经知道自己在对什么下手。你明白格网受损，明白高塔属于一套本来用于维护世界的系统，也明白强行点亮会把这份脆弱再撕深一道；可你还是决定让“先把人送回去”排在最前。也正因为如此，这条路才不该被写成轻易的凯旋。它更像一种非常人类的残忍：知道代价，仍然承担。"),
    LT("The peaceful route transforms understanding into labor. It asks you to trust pattern over impulse, compatibility over rupture, sequence over domination. Nothing about it is passive. On the contrary, it demands more patience than force does, because it requires you to listen long enough for the lattice to stop reading you as an intrusion and start accepting you as a participant in repair. That patience is the route's courage.", "和平路线并不温软，它只是把勇敢换了一种形状。它要求你相信节律多过冲动，相信兼容多过撕裂，相信顺序多过征服。和强攻相比，它一点也不轻松，反而更费耐心，因为你必须听得够久、调得够准，直到格网不再把你视作闯入者，而开始接受你是在参与修复。能把手放慢到这种程度，本身就是这条路的勇气。"),
    LT("Settlement becomes believable only after the last illusion of a clean exit has died. By then you know the world can still be maintained, know the ship can be repurposed, know the lattice can be lived with rather than merely escaped from. But you also know none of that matters while the last rogue guardian still stands between survival and stewardship. To choose settlement is therefore two decisions at once: clear what remains violently unstable, then stay long enough to inherit work no one else is coming back to finish.", "定居路线之所以成立，是因为“干净离开”这件事到这里已经不再像唯一诚实的未来。你知道这颗世界仍能被维护，知道飞船还能被改造成真正的居所，也知道格网未必只能被逃离；可你同样知道，在最后一道失控守卫还没被清掉之前，留下来就只是另一种自欺。于是定居也就成了两次决定叠在一起：先解决掉仍在失稳的那部分，再承认自己会把剩下的工作接下来，而且很可能不会再有人回来替你做完。")
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

bool UIStory_IsMainStoryScene(StoryScene scene) {
    return scene >= STORY_SCENE_MAIN_AIR_FOR_ONE_MORE_DAY
        && scene < (StoryScene)(STORY_SCENE_MAIN_AIR_FOR_ONE_MORE_DAY + STORY_MAIN_SCENE_COUNT);
}

static bool IsLogStoryScene(StoryScene scene) {
    return scene >= STORY_SCENE_LOG_THE_CRASH
        && scene < (StoryScene)(STORY_SCENE_LOG_THE_CRASH + STORY_LOG_SCENE_COUNT);
}

const IntroSlideDef *UIStory_GetStorySceneDef(StoryScene scene) {
    if (UIStory_IsMainStoryScene(scene)) {
        return &kStoryMainDefs[scene - STORY_SCENE_MAIN_AIR_FOR_ONE_MORE_DAY];
    }

    if (IsLogStoryScene(scene)) {
        return &kStoryLogDefs[scene - STORY_SCENE_LOG_THE_CRASH];
    }

    return &kStoryMainDefs[0];
}

const TextureAsset *UIStory_GetStorySceneTexture(const AssetBundle *assets, StoryScene scene) {
    if (UIStory_IsMainStoryScene(scene)) {
        return &assets->storyMainScenes[scene - STORY_SCENE_MAIN_AIR_FOR_ONE_MORE_DAY];
    }

    if (IsLogStoryScene(scene)) {
        return &assets->storyLogScenes[scene - STORY_SCENE_LOG_THE_CRASH];
    }

    return NULL;
}

const char *UIStory_GetStorySceneDetailText(StoryScene scene) {
    if (UIStory_IsMainStoryScene(scene)) {
        return Loc_PickText(kStoryMainDetailTexts[scene - STORY_SCENE_MAIN_AIR_FOR_ONE_MORE_DAY]);
    }

    if (IsLogStoryScene(scene)) {
        return Loc_PickText(kStoryLogDefs[scene - STORY_SCENE_LOG_THE_CRASH].body);
    }

    return "";
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

void UI_DrawOpeningStandby(const AssetBundle *assets, bool showContinueHint, int screenWidth, int screenHeight, float elapsedSeconds) {
    const char *title;
    const char *body;
    const char *hint;
    float scale;
    float pulseAlpha;
    Rectangle textRect;
    Vector2 titleSize;

    title = Loc_PickLiteral("Incoming link stabilizing", "接入链路正在稳定");
    body = Loc_PickLiteral("Hold position. Visual feed is still aligning with the crash recorder.", "请暂时保持等待。视觉记录仍在与坠毁日志重新对齐。");
    hint = Loc_PickLiteral("Click or press Enter again to continue.", "再次点击或按 Enter 继续。");
    scale = UIRuntime_GetScale(screenWidth, screenHeight);
    pulseAlpha = 0.42f + 0.26f * (0.5f + 0.5f * std::sin(elapsedSeconds * 1.9f));
    textRect = Rectangle{
        screenWidth * 0.5f - 340.0f * scale,
        screenHeight * 0.5f - 90.0f * scale,
        680.0f * scale,
        180.0f * scale
    };
    titleSize = UIRuntime_MeasureText(assets, title, 28.0f * scale);

    DrawRectangleGradientV(0, 0, screenWidth, screenHeight, Color{0, 0, 0, 255}, Color{4, 8, 14, 255});
    DrawCircleGradient(screenWidth / 2, screenHeight / 2, 180.0f * scale, Color{24, 54, 76, 22}, Color{0, 0, 0, 0});
    DrawCircleLines(screenWidth / 2, screenHeight / 2, 58.0f * scale + std::sin(elapsedSeconds * 1.6f) * 3.0f * scale, Color{86, 144, 180, (unsigned char)(80.0f * pulseAlpha)});

    UIRuntime_DrawText(assets,
                       title,
                       Vector2{screenWidth * 0.5f - titleSize.x * 0.5f, textRect.y + 24.0f * scale},
                       28.0f * scale,
                       Color{214, 230, 240, 255});
    UIRuntime_DrawWrappedText(assets,
                              body,
                              Rectangle{textRect.x, textRect.y + 70.0f * scale, textRect.width, 48.0f * scale},
                              18.0f * scale,
                              22.0f * scale,
                              Color{170, 188, 202, 255});

    if (showContinueHint) {
        Vector2 hintSize = UIRuntime_MeasureText(assets, hint, 17.0f * scale);

        UIRuntime_DrawText(assets,
                           hint,
                           Vector2{screenWidth * 0.5f - hintSize.x * 0.5f, textRect.y + 128.0f * scale},
                           17.0f * scale,
                           Color{190, 222, 242, (unsigned char)(170.0f + 60.0f * pulseAlpha)});
    }
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
