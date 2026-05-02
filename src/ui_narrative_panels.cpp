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
    {LT("Main Story 01", "主线剧情 01"), LT("Air For One More Day", "再撑一天的空气"), LT("The wreck is still venting smoke and your lungs still ache. This first repair is not rescue, only a little more breathable time stolen back from the night.", "残骸还在冒烟，你的肺也还在发烫。修好的这第一段氧气模块不是拯救，只是替今晚多抢回来一点能呼吸的时间。开拓者号还站着，但它现在更像一具勉强没断气的躯体。"), Color{124, 225, 255, 255}, Color{10, 34, 54, 255}, Color{4, 10, 18, 255}},
    {LT("Main Story 02", "主线剧情 02"), LT("Oxygen Cycle Restored", "第一轮氧气恢复"), LT("When the oxygen loop finally finds a rhythm, the ship stops sounding like it is dying room by room. You are not safe, only finally able to prepare for tomorrow.", "当氧气循环终于重新连成节律，开拓者号第一次不像一间一间慢慢死去。你并没有脱险，只是终于争到了一点能为明天做准备的余地。"), Color{166, 255, 226, 255}, Color{14, 38, 58, 255}, Color{5, 10, 19, 255}},
    {LT("Main Story 03", "主线剧情 03"), LT("Loxi Full Sync", "洛希完整上线"), LT("The upper terminal stabilizes and Loxi speaks in full again. In the dead ship, one more mind is finally thinking alongside you.", "上层终端稳定下来以后，洛希不再只是残缺告警。它重新以完整的声音开口，把事实一条条摆在你面前。死船里终于又有一个意识，仍在和你一起思考。"), Color{123, 225, 255, 255}, Color{16, 34, 62, 255}, Color{6, 10, 18, 255}},
    {LT("Main Story 04", "主线剧情 04"), LT("Airlock Opening", "气闸开启"), LT("The airlock does not open cleanly. It is dragged apart into thin air, wet fog, and the outline of a world that feels larger than escape.", "气闸不是轻松地打开，而是在漫长震颤里被硬生生拖开。门外有可呼吸的空气，也有薄雾、湿冷和陌生结构的轮廓。你不是从船里走向自由，而是走向一个更大的未知。"), Color{255, 214, 154, 255}, Color{34, 34, 46, 255}, Color{8, 10, 16, 255}},
    {LT("Main Story 05", "主线剧情 05"), LT("Signal Answers Back", "信号回应了"), LT("The repaired relay does not recover a human plea. It recovers a quieter answer, one that sounds older than rescue and less willing to accept refusal.", "修好的通讯中继带回来的不是人类求救，而是一段更古老、更安静、也更不愿意接受拒绝的回应。你开始怀疑，坠毁也许不是故事的起点，而只是某个更早过程被你撞上的结果。"), Color{110, 233, 255, 255}, Color{14, 36, 56, 255}, Color{5, 10, 18, 255}},
    {LT("Main Story 06", "主线剧情 06"), LT("Crash Not Accident", "坠毁并非意外"), LT("Burn scars and drag marks refuse the weather story. The ship was not simply lost in a storm; it was forced into a system already in motion.", "烧蚀痕迹和拖拽残留否定了“风暴事故”的解释。开拓者号并不是单纯失控坠落，它更像是在进入大气层后，被某种仍在运行的结构强行拖进了地表。"), Color{255, 186, 145, 255}, Color{56, 24, 24, 255}, Color{18, 8, 12, 255}},
    {LT("Main Story 07", "主线剧情 07"), LT("Deep Scan Online", "深层扫描上线"), LT("Power returns and the ship sees farther than before. Scattered anomalies begin to read like one unfinished map instead of random danger.", "动力恢复以后，飞船的感知范围也跟着醒了过来。零散的异常点、遗迹读数和设施轮廓第一次开始显出同一张版图。这个世界不再像随机荒野，而像一套你还没看懂的结构。"), Color{166, 255, 226, 255}, Color{18, 40, 62, 255}, Color{6, 10, 18, 255}},
    {LT("Main Story 08", "主线剧情 08"), LT("Relic Pattern Decode", "遗物模式解码"), LT("The relic fragments yield part of the signal grammar. The tower no longer looks like something that must only be forced open.", "整套遗物碎片终于吐露出信号语法的一部分。那座塔楼不再只是一个要被强行启动的目标，它也开始像某种可以被读懂、被接入、甚至被正确对话的结构。"), Color{174, 226, 255, 255}, Color{22, 36, 66, 255}, Color{6, 10, 20, 255}},
    {LT("Main Story 09", "主线剧情 09"), LT("East Wreck Confirmation", "东线残骸回证"), LT("The eastern residue locks the crash to the same lattice traced elsewhere. Accident, fog, and signal no longer read like separate mysteries.", "东线残骸中的残留记录，终于把飞船坠毁与区域格网彻底锁到了一起。事故、毒雾和异常信号，从这里开始不再像三件彼此无关的怪事。"), Color{166, 214, 255, 255}, Color{18, 34, 58, 255}, Color{6, 10, 18, 255}},
    {LT("Main Story 10", "主线剧情 10"), LT("Basin Access Qualification", "深潭资格确认"), LT("Recovering the energy core feels less like a prize than permission. You are no longer only surviving the outskirts; you are qualified to go deeper.", "找回能源核心的感觉不像拿到奖励，更像终于被允许继续深入。你从一个还在外围挣扎求生的幸存者，变成了真正有资格碰触更深层真相的人。"), Color{190, 234, 255, 255}, Color{18, 34, 60, 255}, Color{6, 10, 18, 255}},
    {LT("Main Story 11", "主线剧情 11"), LT("West Route Confirmed", "西线正式确认"), LT("The western signal is no longer rumor. Someone from your own ship came this way and tried to leave direction behind for whoever followed.", "西线信号不再只是传闻或底噪。你们船上的确有人走过这里，还在离开前尽力给后来者留下了方向。西侧从这一刻起，不再是边界，而是一条正式的调查路线。"), Color{161, 236, 255, 255}, Color{18, 42, 58, 255}, Color{6, 12, 20, 255}},
    {LT("Main Story 12", "主线剧情 12"), LT("Survey Break Relay", "勘测断崖接力"), LT("Survey Break's anchors and reversed arrows make the truth plain: this was not flight, but a relay built for the next exhausted person to keep going.", "勘测断崖上的锚点与反向路标，把真相说得很清楚：他们不是在溃逃，而是在替后来者搭一条还能继续往前的接力链。西线第一次从“有人走过”升级成“有人有意留下了办法”。"), Color{255, 214, 154, 255}, Color{40, 30, 22, 255}, Color{10, 8, 14, 255}},
    {LT("Main Story 13", "主线剧情 13"), LT("Canopy Handoff", "林冠交接"), LT("The canopy perch preserves more than equipment. Someone held the line here long enough to hand the next person a living responsibility.", "林冠观察点保留下来的，不只是设备和记号，还有生活过的秩序。有人曾在这里轮值、配给、等待、交接，一直撑到能把责任郑重地交给下一个活下来的人。"), Color{166, 255, 226, 255}, Color{16, 44, 40, 255}, Color{6, 12, 18, 255}},
    {LT("Main Story 14", "主线剧情 14"), LT("Echo Basin Reconstruction", "回声盆地重建"), LT("Broken pings finally resolve into a route map. What looked like disappearance now reads like a successful handoff toward the south.", "破碎回声终于被重建成路线拓扑。曾经像“失踪”的空白，如今更像一次成功的交接：西线带回来的关键数据并没有断在路上，而是被送进了更深的系统里。"), Color{166, 235, 255, 255}, Color{18, 38, 58, 255}, Color{6, 10, 18, 255}},
    {LT("Main Story 15", "主线剧情 15"), LT("Last Camp Positions", "最后营地三立场"), LT("The last camp does not leave one answer behind. It leaves three futures side by side: leave, stabilize, or stay and inherit.", "最后营地留下来的，不是唯一答案，而是三种并排摆着的未来：立即求援、先稳定环境、尝试长期定居。你的终局从这一刻起，不再像临时起意，而像接过了他们来不及做完的判断。"), Color{255, 216, 174, 255}, Color{40, 28, 22, 255}, Color{12, 8, 14, 255}},
    {LT("Main Story 16", "主线剧情 16"), LT("South Facility Wakes", "南侧设施苏醒"), LT("The south route reveals itself as damaged infrastructure, not raw geology. The world begins to look less like terrain and more like something that can still be taken over.", "南侧一路显露出来的，不是普通地下地形，而是一套受损却仍在运行的工程结构。这个世界第一次不再只像威胁，而像某种还能被理解、甚至还能被接手的系统。"), Color{182, 240, 214, 255}, Color{18, 44, 42, 255}, Color{6, 12, 18, 255}},
    {LT("Main Story 17", "主线剧情 17"), LT("Vent Network Calibrated", "通风网络校准"), LT("As the vent chain comes back online, the fog truly loosens. The environment answers just enough to prove that danger here is not fixed.", "通风链路重新上线以后，毒雾第一次真实地退开了一点。世界并不是一成不变地敌对着你，它也会对正确的修复与校准作出回应。"), Color{166, 255, 226, 255}, Color{18, 42, 42, 255}, Color{6, 12, 18, 255}},
    {LT("Main Story 18", "主线剧情 18"), LT("Service Shaft Backbone", "维护井主干连通"), LT("Once the service shaft backbone lights up, ship systems, purifier controls, and tower routing stop feeling like separate problems.", "维护井主干一旦连通，很多原本彼此分开的东西都露出了同一条骨架。飞船、净化设施和更深处的塔楼格网，不再像三个问题，而开始像同一台机器的不同入口。"), Color{204, 222, 255, 255}, Color{22, 34, 62, 255}, Color{8, 10, 20, 255}},
    {LT("Main Story 19", "主线剧情 19"), LT("Purifier Ring Sequence", "净化环恢复次序"), LT("The ring does not restore all at once. Filtration, sealing, and oxygen return in order, proving that sequence matters as much as power.", "净化环并不是一口气全部亮起，而是按正确顺序一点点恢复。净化、密封、供氧依次回归，像在提醒你：这套系统不只需要力量，更需要被正确地理解。"), Color{182, 240, 214, 255}, Color{18, 44, 42, 255}, Color{6, 12, 18, 255}},
    {LT("Main Story 20", "主线剧情 20"), LT("Root Vault Truth", "根脉核心真相"), LT("In the Root Vault, tower, monoliths, purifier ring, fog, and crash all resolve into one damaged maintenance lattice. The planet did not choose you; it misread you.", "在根脉密库深处，所有线索终于被接回同一张受损的世界维护格网。塔楼、石碑、净化环、毒雾和坠毁并不是不同谜题，而是同一故障系统的不同表征。而最沉重的纠偏也在这里出现：它从未有意召唤你们，只是把开拓者号误判成了一次还能兼容的维修响应。"), Color{204, 222, 255, 255}, Color{20, 32, 62, 255}, Color{8, 10, 20, 255}},
    {LT("Main Story 21", "主线剧情 21"), LT("West South Correlation", "西南证据对齐"), LT("West handoff notes and south maintenance records finally lock into one timeline. The scattered story closes into history.", "西线交接记录和南线维护文书终于进入了同一条时间线。那些原本像分散事故和失踪空白的东西，从这里开始真正闭合成一段历史：他们不是各自挣扎，而是在同一场崩坏里彼此替对方争时间。"), Color{166, 235, 255, 255}, Color{18, 38, 58, 255}, Color{6, 10, 18, 255}},
    {LT("Main Story 22", "主线剧情 22"), LT("Loxi Conclusion Rewrite", "洛希结论重写"), LT("Loxi openly rewrites its own earlier reading. The world is no longer a pile of unrelated threats, but one burden explained more honestly.", "洛希重新整理了自己的全部判断。它第一次明确承认，自己先前把这个世界读得太像一堆彼此无关的威胁。也正因为它愿意这样修正，后面它给出的每一句结论才变得更值得相信。"), Color{174, 226, 255, 255}, Color{18, 34, 64, 255}, Color{6, 10, 20, 255}},
    {LT("Main Story 23", "主线剧情 23"), LT("Monolith True Role", "石碑真正职责"), LT("The monoliths were never the source of danger. They were dampers, holding a larger instability back for longer than anyone remained to help them.", "石碑从来不是危险的源头。它们是阻尼器，是在更大的失稳面前替这颗世界硬撑时间的控制端。最像敌人的东西，原来也一直在拖着崩坏不要来得更快。"), Color{174, 226, 255, 255}, Color{18, 34, 64, 255}, Color{6, 10, 20, 255}},
    {LT("Main Story 24", "主线剧情 24"), LT("North Route Commitment", "北线路线承诺"), LT("The north route narrows investigation into responsibility. You are no longer only studying what happened here, but walking toward what must be decided here.", "北线真正打开以后，故事开始明显收束。你不再只是去调查一处更深的遗迹，而是在走向一个必须由你来承担后果的地点。理解在这里不再只是知识，它开始要求你表态。"), Color{255, 214, 154, 255}, Color{42, 26, 20, 255}, Color{10, 8, 12, 255}},
    {LT("Main Story 25", "主线剧情 25"), LT("Tower Not Button", "塔楼不是按钮"), LT("The tower terminal is not a switch waiting to be pressed. It is a declaration point that asks you to admit the consequence before you proceed.", "塔前终端并不是一个等你按下去的开关。它更像一个提交窗口，要求你先承认每种结果都会带来的后果，然后才允许你继续。你抵达这里，不代表你已经正确，只代表你终于不能再躲在惯性后面。"), Color{255, 214, 154, 255}, Color{42, 26, 20, 255}, Color{10, 8, 12, 255}},
    {LT("Main Story 26", "主线剧情 26"), LT("Three Costs Revealed", "三种代价浮出"), LT("Rescue, stabilization, and settlement no longer read like menu choices. They are three different answers to the same truth, each with its own cost.", "到了这里，求援、和平接入与留下定居终于不再像菜单选项。它们都成了对同一真相的不同回答：先把人送回去、先把系统稳住，或承认自己要把这份工作接下来。每条路都能救下一部分东西，也都会让另一部分留下伤痕。"), Color{255, 216, 174, 255}, Color{40, 28, 22, 255}, Color{12, 8, 14, 255}},
    {LT("Main Story 27", "主线剧情 27"), LT("Action Declaration", "行动宣言"), LT("Once the declaration is entered, the choice stops living only in thought. From here onward, what happens next carries your name beside it.", "一旦行动宣言被写进记录，选择就不再只属于你心里的犹豫。从这一刻起，之后发生的一切都将不再能被轻易归进偶然。你不是碰到了某个结果，而是在签下你愿意承担的方向。"), Color{166, 235, 255, 255}, Color{18, 38, 58, 255}, Color{6, 10, 18, 255}},
    {LT("Main Story 28", "主线剧情 28"), LT("Heroic Route Commitment", "英雄路线前章"), LT("To force the beacon alive is to keep putting getting people home first, even after learning what the damage truly means.", "强行点亮信标，意味着此刻仍把“先把人送回去”排在最前。你已经知道高塔和格网属于一套受损维护系统，也知道再一次强行介入会留下更深伤痕；可你仍决定这么做。它不光荣，只是一个带着代价也仍然诚实的人类答案。"), Color{255, 214, 154, 255}, Color{44, 28, 20, 255}, Color{12, 8, 12, 255}},
    {LT("Main Story 29", "主线剧情 29"), LT("Peaceful Route Commitment", "和平路线前章"), LT("This route does not tear the lattice wider apart. It asks for accuracy, compatibility, and the patience to let understanding do the harder work.", "和平路线并不轻松，它只是把勇敢换了一种形状。你不再试图撕开格网，而是试着让人类系统被它正确读取。这里真正的代价不是火力，而是耐心，是让理解一遍遍承担本该由冲动替你完成的重量。"), Color{166, 255, 226, 255}, Color{18, 44, 44, 255}, Color{6, 12, 18, 255}},
    {LT("Main Story 30", "主线剧情 30"), LT("Settlement Route Commitment", "定居路线前章"), LT("Leaving is no longer the only honest future. To stay is to clear what remains unstable, inherit unfinished maintenance, and accept a longer duty.", "离开不再是唯一诚实的未来。你已经知道这颗世界仍可被维护，知道飞船还能被改造成真正的居所，也知道最后那部分失控必须先被清掉。选择定居，不是拒绝求生，而是承认自己愿意把别人没做完的工作接下来。"), Color{255, 216, 174, 255}, Color{36, 28, 26, 255}, Color{10, 8, 14, 255}}
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
        LT("Day 1. Impact did not kill us first. The air almost did. The ship only stayed alive because someone chose breath before comfort.",
           "第一天。撞击没有立刻杀死我们，差点要命的是空气。主船腹着地以后，三段氧气管线在不到两分钟内连续裂开，洛希把剩余电力全压进中央走廊。我们活下来，不是因为准备充分，而是因为有人先决定：先保住呼吸，再保住体面。"),
        Color{123, 225, 255, 255},
        Color{14, 34, 58, 255},
        Color{4, 10, 18, 255}
    },
    {
        LT("Ship Log 02", "飞船日志 02"),
        LT("Split Roster", "分组名单"),
        LT("Day 3. We stopped calling it a search party. It became a split: hold the ship, push west, and confirm the east-south line before all three chances died together.",
           "第三天。我们不再把它叫作搜寻队，而叫分流。一组守船保空气，一组带着天线部件向西追信号，一组沿东南线确认石构和残骸异常。我们不是在散开，只是在把活下去和查清真相的概率拆成几份继续往前送。"),
        Color{255, 214, 154, 255},
        Color{42, 30, 24, 255},
        Color{10, 8, 14, 255}
    },
    {
        LT("Ship Log 03", "飞船日志 03"),
        LT("Pattern, Not Wilderness", "这里是模式，不是荒野"),
        LT("Day 7. Fog pulses, stone resonance, and signal returns repeat on the same windows. This is not wilderness in the human sense. It is a system still following rules after its keepers vanished.",
           "第七天。雾潮、石碑回响、信号返回和设施残留开始在同一组时间窗口里重复。这里不像荒野，更像一套运行得不完整、却仍会对闯入作出反应的系统。我们可能不是坠毁在自然里，而是掉进了一台坏掉的世界机器。"),
        Color{166, 255, 226, 255},
        Color{18, 42, 40, 255},
        Color{6, 12, 18, 255}
    },
    {
        LT("Field Record 04", "现场记录 04"),
        LT("West Signal Fragment 01", "西线信号碎片 01"),
        LT("The first west fragment is not static. It matches the same broken structure that distorted the ship on descent. This route was never salvage. It was a pursuit.",
           "西线第一段碎片不是随机噪音。它和坠毁前干扰我们导航的失真源有同样的重复结构，只是更短、更弱，也更像有人故意替后来者留住的一点抓手。我们不是往西碰碰运气，我们是在追一个问题。"),
        Color{158, 232, 255, 255},
        Color{18, 38, 58, 255},
        Color{6, 10, 18, 255}
    },
    {
        LT("Field Record 05", "现场记录 05"),
        LT("Survey Break Anchor Notes", "勘测断崖锚点笔记"),
        LT("Survey Break's anchors were never retreat markers. They were relay work: rope, direction, and silence arranged so the next exhausted person would not have to learn the same mistake by dying from it.",
           "勘测断崖上的锚点布置不像撤退，更像接力。路线、绳位、反向箭头和藏起来的备用节点都指向同一个目的：我们未必能回来，但后来者至少不该再从头犯一遍同样的错误。"),
        Color{255, 214, 154, 255},
        Color{40, 30, 22, 255},
        Color{10, 8, 14, 255}
    },
    {
        LT("Field Record 06", "现场记录 06"),
        LT("Canopy Handoff Record", "林冠交接记录"),
        LT("Under the canopy, the split stops looking like disappearance and starts looking like duty. One branch kept the watch. Another carried the notes south. Neither assumed they would both return.", 
           "林冠下的这份交接记录，把“失踪”彻底改写成了分工。一组继续往西追信号与地形，一组把整理过的数据和窗口判断转给南侧设施线。没有人以为自己一定回得来，所以他们先把责任传下去。"),
        Color{166, 255, 226, 255},
        Color{16, 44, 40, 255},
        Color{6, 12, 18, 255}
    },
    {
        LT("Field Record 07", "现场记录 07"),
        LT("Echo Basin Topology Sketch", "回声盆地拓扑草图"),
        LT("The Echo Basin sketch turns static into proof. What looked like absence now reads like routing: the packet really did make it south.", 
           "回声盆地的草图把破碎脉冲整理成了路线图。曾经像失踪与空白的地方，从这里开始都更像一次成功的转包：西进组带回来的关键判断没有断在半路，而是被送向了南侧入口与更深的系统。"),
        Color{166, 235, 255, 255},
        Color{18, 38, 58, 255},
        Color{6, 10, 18, 255}
    },
    {
        LT("Field Record 08", "现场记录 08"),
        LT("Last Camp Testament", "最后营地遗言"),
        LT("The final camp leaves no single command behind. It leaves three futures: rescue, stabilization, and settlement. None is written as pure. All are written as real.", 
           "最后营地没有留下一条统一遗命，而是留下了三种并排摆开的未来：尽快求援、先稳定环境、尝试长期留下。它们不是抽象理念，而是饥饿、坏空气和越来越少的人手里，仍被认真看作诚实答案的三种选择。"),
        Color{255, 216, 174, 255},
        Color{42, 28, 22, 255},
        Color{12, 8, 14, 255}
    },
    {
        LT("Crash Record 09", "坠毁记录 09"),
        LT("Black Box Residue", "黑匣残留"),
        LT("The black box residue proves the ship tried to steer away from the same lattice later mapped from the west. The crash was never separate from the system failure around it.",
           "黑匣残留显示，开拓者号在坠毁前曾试图规避一片异常区域格网。那不是普通风暴，也不是单点障碍，而是一张会影响导航、会误读信号、会把接近者拖进错误流程里的结构。事故从一开始就和这颗世界的系统失稳缠在一起。"),
        Color{204, 222, 255, 255},
        Color{22, 34, 62, 255},
        Color{8, 10, 20, 255}
    },
    {
        LT("Facility Record 10", "设施记录 10"),
        LT("Purifier Outage Memo", "净化器停摆备忘"),
        LT("The purifier outage memo renames the fog. It is not pure hostility. It is maintenance failure pushed back up to the surface after the chain lost its keepers.", 
           "净化器停摆备忘把毒雾重新定义成维护失败，而不是纯粹敌意。区域会变得更凶险，不是因为世界突然起了杀心，而是因为原本压住失稳的净化链断开了，环境开始把没人照看的代价直接吐在地表上。"),
        Color{182, 240, 214, 255},
        Color{18, 44, 40, 255},
        Color{6, 12, 18, 255}
    },
    {
        LT("Facility Record 11", "设施记录 11"),
        LT("Vent Calibration Handover", "通风校准交接"),
        LT("The vent handover proves west and south were buying each other minutes, not just sharing data. Cleaner air and quieter signal windows were the same work seen from two ends.", 
           "通风校准交接证明，西线与南线并不是各自挣扎。更干净的空气窗口和更安静的回声时段，本来就是同一套计划的一部分：一边替另一边争几分钟，一边替另一边多留一条还能走的路。"),
        Color{166, 255, 226, 255},
        Color{18, 42, 42, 255},
        Color{6, 12, 18, 255}
    },
    {
        LT("Facility Record 12", "设施记录 12"),
        LT("Service Shaft Sync Record", "维护井道同步记录"),
        LT("The service shaft sync record makes the worst truth plain: ship systems, purifier controls, and tower routing all share one backbone. You have been inside the same machine the whole time.", 
           "维护井道同步记录把最难承认的事实写得很直接：飞船系统、净化控制和埋藏的塔楼格网共享同一条维护主干。你不是在几个区域之间来回解决不同问题，你一直都在从不同入口走进同一台坏掉的机器里。"),
        Color{204, 222, 255, 255},
        Color{22, 34, 62, 255},
        Color{8, 10, 20, 255}
    },
    {
        LT("Facility Record 13", "设施记录 13"),
        LT("Purifier Ring Control Brief", "净化环控制简报"),
        LT("The purifier ring brief changes everything with one buried note: the monolith endpoints are not the danger source. They are field controls holding back something larger.", 
           "净化环控制简报最重要的一句，不是恢复顺序，而是页角那条几乎像怕人看不见的附注：石碑终端不是危险源，它们是场域控制器，是把更大失稳按在边界里的手。一旦明白这一点，高塔看起来就不再像武器，而更像一份职责。"),
        Color{174, 226, 255, 255},
        Color{18, 34, 64, 255},
        Color{6, 10, 20, 255}
    },
    {
        LT("Facility Record 14", "设施记录 14"),
        LT("Root Vault Core Dossier", "根脉核心档案"),
        LT("The Root Vault dossier joins every thread together. The planet did not summon your ship. Its damaged maintenance lattice misread the Ark as a compatible repair response, and every ending is changed by that correction.", 
           "根脉核心档案把塔楼、石碑、净化环、毒雾和坠毁真正接成了一体。开拓者号并不是被某种意志故意召唤，而是被这张失稳维护格网误判成了一次可兼容的紧急维修响应。也正因为这句纠偏成立，强行救援、和平接入和长期定居三条路线，才都会变成诚实却带伤的答案。"),
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
