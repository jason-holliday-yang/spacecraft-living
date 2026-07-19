#include "task_content.h"

#include <array>

namespace {

#define LT(en, zh) LocalizedText{en, zh}
#define LT_EMPTY LocalizedText{"", ""}

const std::array<TaskNodeSeed, 71> kNodeSeeds = {{
    {RESOURCE_WOOD, EXTERIOR_X(55), EXTERIOR_Y(53), 2, 4, false},
    {RESOURCE_WOOD, EXTERIOR_X(59), EXTERIOR_Y(66), 2, 4, false},
    {RESOURCE_WOOD, EXTERIOR_X(67), EXTERIOR_Y(78), 2, 4, false},
    {RESOURCE_WOOD, EXTERIOR_X(82), EXTERIOR_Y(79), 2, 4, false},
    {RESOURCE_WOOD, EXTERIOR_X(96), EXTERIOR_Y(77), 2, 4, false},
    {RESOURCE_WOOD, EXTERIOR_X(47), EXTERIOR_Y(59), 2, 4, false},
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
    {RESOURCE_WOOD, SHIP_CARGO_HOLD_X + 1, SHIP_CARGO_HOLD_Y + 1, 4, 0, false},
    {RESOURCE_GLOW_MOSS, SHIP_AIRLOCK_LINK_X + 2, SHIP_AIRLOCK_LINK_Y + 2, 3, 0, false},
    {RESOURCE_METAL_SCRAP, SHIP_WORKSHOP_X + 3, SHIP_WORKSHOP_Y + 3, 2, 0, false},
    {RESOURCE_ORE, SHIP_POWER_BAY_X + 3, SHIP_POWER_BAY_Y + 3, 1, 0, false}
}};

const std::array<TaskMonsterSeed, 8> kMonsterSeeds = {{
    {MONSTER_THORN_LARVA, EXTERIOR_X(20), EXTERIOR_Y(72), 4},
    {MONSTER_WING_BUG, EXTERIOR_X(41), EXTERIOR_Y(67), 4},
    {MONSTER_RAPTOR, EXTERIOR_X(28), EXTERIOR_Y(88), 4},
    {MONSTER_SWAMP_STALKER, EXTERIOR_X(111), EXTERIOR_Y(48), 5},
    {MONSTER_SENTINEL_JELLY, EXTERIOR_X(82), EXTERIOR_Y(100), 6},
    {MONSTER_FOG_WORM, EXTERIOR_X(120), EXTERIOR_Y(101), 6},
    {MONSTER_RELIC_GUARD, EXTERIOR_X(74), EXTERIOR_Y(18), 7},
    {MONSTER_FINAL_BOSS, BOSS_ARENA_BOSS_X, BOSS_ARENA_BOSS_Y, 7}
}};

const std::array<TaskLogSeed, 14> kLogSeeds = {{
    {
        SHIP_CARGO_HOLD_X + 3,
        SHIP_CARGO_HOLD_Y + 3,
        SHIP_LOG_MAINLINE,
        LT("Ship Log 01: Impact Protocol", "飞船日志 01：撞击应对"),
        LT("Day 1. Impact did not kill us first. The air almost did. The ship only stayed alive because someone chose breath before comfort.",
           "第一天。撞击没有立刻杀死我们，差点要命的是空气。主船腹着地以后，三段氧气管线在不到两分钟内连续裂开，洛希把剩余电力全压进中央走廊。我们活下来，不是因为准备充分，而是因为有人先决定：先保住呼吸，再保住体面。"),
        LT("Recorder: Acting Captain Mara Quinn. Timestamp: forty-seven minutes after impact. The medical hatch is still jammed, coolant is hissing somewhere behind the wall, and oxygen is already bleeding out faster than anyone can patch by hand. Quinn orders every walking body into the central corridor, sends Elias to rip a backup pump free, and keeps the last two pain injectors for the crew trapped under the bent bulkhead.\n\nNothing about the note is heroic. It smells of smoke, triage foam, and metal dust. The line she underlines twice is the only softness it allows: 'Keep the breathing corridor alive. Grief can wait until the lights stop shaking.' The whole ship, in that moment, is reduced to one brutal hierarchy: who can still move, who can still help, and who must trust strangers to hold one narrow strip of air through the night.",
           "记录人：代理舰长玛拉·奎因。时间戳停在撞击后四十七分钟。医疗舱门还卡死着，舱壁后方有冷却液在嘶嘶乱喷，氧气却已经沿着裂口逃得比人手补得更快。玛拉把还能站起来的人全赶进中央走廊，命伊莱亚斯去拆备用泵，又把最后两支镇痛剂留给被压在弯折舱壁后面的人。\n\n这页纸没有一点英雄气，只有烟味、止漏泡沫和金属粉末几乎从字里扑出来。她把其中一句重重划了两遍：‘先把能呼吸的那条走廊保住，等灯不晃了，再去哭。’在那一刻，整艘船都被压缩成一条冷酷的顺序：谁还能动，谁还能帮忙，谁只能把命交给别人去替他守住那一小段空气。")
    },
    {
        SHIP_CREW_QUARTERS_X + 1,
        SHIP_CREW_QUARTERS_Y + 2,
        SHIP_LOG_MAINLINE,
        LT("Ship Log 02: Split Roster", "飞船日志 02：分组名单"),
        LT("Day 3. We stopped calling it a search party. It became a split: hold the ship, push west, and confirm the east-south line before all three chances died together.",
           "第三天。我们不再把它叫作搜寻队，而叫分流。一组守船保空气，一组带着天线部件向西追信号，一组沿东南线确认石构和残骸异常。我们不是在散开，只是在把活下去和查清真相的概率拆成几份继续往前送。"),
        LT("The split roster reads less like a plan for victory than a list written by someone refusing to let panic choose for them. Elias Venn remains ship-side to keep air, power, and Loxi from dying completely. Tamsin Hale takes the burst antenna casing west after the repeating fragment. Sera Dane and courier Jonah Reed angle east and south to confirm worked stone under the swamp growth.\n\nThere is grease on the margin where a gloved thumb dragged across the page, and just one line that breaks the administrative tone: 'Do not call this scattering. We are dividing the chance to live into three moving parts.' That sentence changes everything. The crew are not missing yet. They are still deciding where hope should be carried, and by whom.",
           "这份分组名单看起来不像胜利方案，更像有人在最乱的时候硬逼自己别让恐慌代替思考。伊莱亚斯·文恩留船，负责把空气、电力和洛希勉强维持住；塔姆辛·黑尔带着破损天线壳向西，去追那段不断重复的异常信号；塞拉·戴恩和信使乔纳·里德则折向东南，去确认沼生层下面那些明显被加工过的石构。\n\n页边被手套上的油污拖出一道痕，唯一一句脱离公文口吻的话写在旁边：‘别把分开看成散掉，我们只是把活下去的概率拆成三份。’看到这句以后，名单就不再只是名单了。那些人还没有失踪，他们还在决定，该由谁把希望往哪个方向带。")
    },
    {
        SHIP_DIAGNOSTICS_X + 1,
        SHIP_DIAGNOSTICS_Y + 2,
        SHIP_LOG_MAINLINE,
        LT("Ship Log 03: Pattern, Not Wilderness", "飞船日志 03：这里是模式，不是荒野"),
        LT("Day 7. Fog pulses, stone resonance, and signal returns repeat on the same windows. This is not wilderness in the human sense. It is a system still following rules after its keepers vanished.",
           "第七天。雾潮、石碑回响、信号返回和设施残留开始在同一组时间窗口里重复。这里不像荒野，更像一套运行得不完整、却仍会对闯入作出反应的系统。我们可能不是坠毁在自然里，而是掉进了一台坏掉的世界机器。"),
        LT("The diagnostics tag belongs to Elias Venn. He logs fog pulse intervals, stone resonance, and tower echo returns with the stiff patience of a technician refusing to make the wrong claim too early. Yet the table itself becomes its own confession. The numbers repeat too cleanly. The harmonics answer too consistently. Even plant growth seems to bloom and recede on a schedule.\n\nNear the bottom he stops sounding like an engineer and starts sounding like a man trying not to be the first to say something impossible out loud: 'If this is natural, it has learned machinery. If this is machinery, its keepers left it too long ago.' The ship is still half-broken when he writes that. Survival is not guaranteed. But already he understands they did not crash into wilderness. They fell into a system still obeying rules after everyone who wrote them was gone.",
           "诊断舱的录音标签属于伊莱亚斯·文恩。他把雾潮脉冲、石壁共振和塔楼回波一条条抄进表格，耐心得几乎像在跟自己较劲，仿佛只要再谨慎一点，事实就会变得没那么离谱。可表格本身最后还是背叛了常识：周期太整齐，共振回应得太一致，连植被的生长和退缩都像踩在某种节拍上。\n\n写到后半段时，他的口吻开始从技术员变成一个不愿意先说破的人：‘如果这是自然，它就学会了机械；如果这是机械，那它的看护者已经离开太久。’那时飞船都还没完全脱险，可他已经意识到自己不是坠毁在荒野里，而是掉进了一套仍在遵守旧命令的系统里。")
    },
    {
        EXTERIOR_X(24),
        EXTERIOR_Y(74),
        SHIP_LOG_MAINLINE,
        LT("Field Record: West Signal Fragment 01", "现场记录：西部信号残片 01"),
        LT("The first west fragment is not static. It matches the same broken structure that distorted the ship on descent. This route was never salvage. It was a pursuit.",
           "西线第一段碎片不是随机噪音。它和坠毁前干扰我们导航的失真源有同样的重复结构，只是更短、更弱，也更像有人故意替后来者留住的一点抓手。我们不是往西碰碰运气，我们是在追一个问题。"),
        LT("Tamsin Hale hides this page in a broken antenna shell, which already tells you what she thinks it is for: not memory, but relay. She writes that the west route's real danger is not the animals under the canopy. It is the signal fragment that keeps learning just enough of human distress patterns to sound familiar at the wrong moment. To fight that, she and her runner leave silent pings, knot markers, and directional checks that can be followed without shouting into the trees.\n\nOn the back she presses the pen hard enough to score the paper: 'If it sounds like it knows your name, do not answer. Count the rhythm first.' That warning is the whole west route in one sentence. Do not trust what reaches you emotionally before you understand what it is structurally.",
           "塔姆辛·黑尔把这页纸塞进一截折断的天线壳里，这个动作本身就说明了她把它当成什么：不是遗物，而是接力。她写道，西线真正危险的从来不是林冠下的兽，而是那段会故意学人类求救频率的信号。为了对付它，她和同行者一路留下静默脉冲、绳结标记和方向校验，让后来的人不用在树影底下大喊也能跟上。\n\n纸背上有一句被她写得几乎把纸划穿：‘如果它听上去像认得你的名字，先别回，先数节奏。’这句话几乎就是整条西线的本质：不要先相信能勾住情绪的东西，要先看懂它的结构。")
    },
    {
        EXTERIOR_X(35),
        EXTERIOR_Y(76),
        SHIP_LOG_MAINLINE,
        LT("Field Record: Survey Break Anchor Notes", "现场记录：勘测断点锚点笔记"),
        LT("Survey Break's anchors were never retreat markers. They were relay work: rope, direction, and silence arranged so the next exhausted person would not have to learn the same mistake by dying from it.",
           "勘测断崖上的锚点布置不像撤退，更像接力。路线、绳位、反向箭头和藏起来的备用节点都指向同一个目的：我们未必能回来，但后来者至少不该再从头犯一遍同样的错误。"),
        LT("Jonah Reed's note is pinned beside the anchor pair with wire instead of tape, as if he no longer trusted adhesives in the damp. He explains the reversed arrows immediately: they are meant to protect the return line, not celebrate the forward push. Tamsin has taken the antenna load farther west. Jonah himself is turning south with a case full of calibration slivers and timestamped readings.\n\nThe line that lingers is almost modest: 'If only one branch stays open, let the other at least know we reached this far.' That is why the rope work here feels so sober. No flourish, no martyrdom, just the exact kind of signs that save oxygen, save minutes, and save the next exhausted climber from making one fatal mistake on the wrong cliff face.",
           "乔纳·里德的短笺是用细铁丝钉在锚桩旁的，像他已经不再相信潮气里还能有胶带撑得住。他很快解释了反向箭头的缘由：真正要保住的不是去路，而是回路。塔姆辛继续把天线载荷往西送，他自己则折向南边，带着一只装满校准片和时间戳的铁盒下去。\n\n最让人记住的一句反而写得很轻：‘如果最后只剩一条路还通，那另一条路也该知道我们至少走到了这里。’也正因为这样，这里的绳结和木桩才显得格外克制。没有英雄式姿态，只有刚刚好能替后来者省下几口气、几分钟和一次致命误判的务实。")
    },
    {
        EXTERIOR_X(41),
        EXTERIOR_Y(67),
        SHIP_LOG_SUPPLEMENTAL,
        LT("Field Record: Canopy Handoff Record", "现场记录：林冠交接记录"),
        LT("Under the canopy, the split stops looking like disappearance and starts looking like duty. One branch kept the watch. Another carried the notes south. Neither assumed they would both return.",
           "林冠下的这份交接记录，把“失踪”彻底改写成了分工。一组继续往西追信号与地形，一组把整理过的数据和窗口判断转给南侧设施线。没有人以为自己一定回得来，所以他们先把责任传下去。"),
        LT("Two handwriting styles share this page. Tamsin logs wind direction, visibility, and signal contamination in tight, efficient strokes. Sera Dane fills the margins with purifier pressure, valve thresholds, and a reminder of what must be carried south if the chance appears. Between them is Jonah's short transfer line: 'I take the notes. You hold the perch. If I do not make it back, do not abandon the watch early.'\n\nWhat gives the outpost its weight is how little anyone tries to dramatize the moment. The wind still has to be measured. Food still has to be divided. The reflective tags still have to face the right way. In that refusal to make a speech, responsibility feels more real than any last stand could.",
           "这页记录上并排留着两种笔迹。塔姆辛那一边写风向、可视距离和信号噪声，字又紧又快；塞拉则把净化器压力、阀值和准备带去南侧设施的备忘全挤在页边。中间夹着乔纳的一句转写：‘我带笔记下去，你们守住上面。如果我回不来，也别提前撤。’\n\n真正让这处观察点沉下来的，是谁都没有把这一刻写得悲壮。风照样要测，口粮照样要分，反光标记照样要摆正。正因为没有人试图发表遗言，职责才显得比任何壮烈姿态都更真实。")
    },
    {
        EXTERIOR_X(26),
        EXTERIOR_Y(90),
        SHIP_LOG_MAINLINE,
        LT("Field Record: Echo Basin Topology Sketch", "现场记录：回声盆地地形草图"),
        LT("The Echo Basin sketch turns static into proof. What looked like absence now reads like routing: the packet really did make it south.",
           "回声盆地的草图把破碎脉冲整理成了路线图。曾经像失踪与空白的地方，从这里开始都更像一次成功的转包：西进组带回来的关键判断没有断在半路，而是被送向了南侧入口与更深的系统。"),
        LT("The topology sketch is ugly in the most convincing way: water stains, overwritten lines, bearings corrected by hand, as if whoever drafted it kept adjusting while conditions changed around them. Once Loxi reconstructs the echoes, the shape resolves into motion instead of loss. Jonah Reed really did cross the dangerous segment carrying tower timing, vent drift, and west-route observations. A packet truly reached the southern threshold.\n\nBelow the final mark is a transcribed line that almost undoes the whole tragedy by a single degree: 'If we break here, let the next one know the west was never a dead route.' That is what Echo Basin gives back. Not the people themselves, but proof that their work kept moving after their voices disappeared.",
           "这张拓扑草图丑得很有说服力：边缘被水泡过，方位线重画了好几遍，很多地方都像是在环境不断变化时硬生生修正出来的。可一旦洛希把回声重建完成，它呈现出来的就不再是损失，而是行动。乔纳·里德真的带着塔楼时序、通风漂移和西线观察穿过了最危险的一段，那份数据包也真的抵达了南侧门槛。\n\n最后一个标记下方留着一段转录，轻得几乎能把整个悲剧掰回一点点：‘如果我们断在这里，至少让下一位知道，西边从来不是死路。’这就是回声盆地真正还回来的东西。不是人，而是他们在失声之后仍继续向前的证据。")
    },
    {
        EXTERIOR_X(45),
        EXTERIOR_Y(84),
        SHIP_LOG_SUPPLEMENTAL,
        LT("Field Record: Last Camp Testament", "现场记录：最后营地遗言"),
        LT("The final camp leaves no single command behind. It leaves three futures: rescue, stabilization, and settlement. None is written as pure. All are written as real.",
           "最后营地没有留下一条统一遗命，而是留下了三种并排摆开的未来：尽快求援、先稳定环境、尝试长期留下。它们不是抽象理念，而是饥饿、坏空气和越来越少的人手里，仍被认真看作诚实答案的三种选择。"),
        LT("Three thin notebooks remain in the last camp, each bearing a different name and a different future. Mara Quinn argues for lighting the tower and accepting the damage if that is what getting people home costs. Sera Dane insists the lattice should be stabilized first, because rescue that deepens collapse is only another form of abandonment. Medic Mira Soren writes the fewest words and perhaps the hardest: 'If nobody stays, this world will drag another ship down next year.'\n\nWhat makes the camp unforgettable is that none of them pretend to be clean. No route is written as pure. No price is hidden. The notebooks do not ask you to admire them. They ask you, when your turn comes, not to choose in ignorance.",
           "最后营地里留下三本薄册子，封面上分别写着不同的名字，也写着不同的未来。玛拉·奎因主张点亮高塔，哪怕代价更重，也要先把人送回去；塞拉·戴恩坚持必须先稳住格网，否则救援本身只会变成另一种抛弃；医疗官米拉·索伦写得最少，却也最狠：‘如果没人留下，明年这里还会再把另一艘船拖下来。’\n\n这个营地最难忘的地方，是谁都没有把自己的答案写成无辜。没有谁隐瞒代价，也没有谁要求后来者赞同自己。她们只是把三条路并排摆好，要求你轮到自己时，别在误会里做决定。")
    },
    {
        EXTERIOR_X(118),
        EXTERIOR_Y(42),
        SHIP_LOG_MAINLINE,
        LT("Crash Recorder: Black Box Residue", "坠毁记录：黑匣残留"),
        LT("The black box residue proves the ship tried to steer away from the same lattice later mapped from the west. The crash was never separate from the system failure around it.",
           "黑匣残留显示，开拓者号在坠毁前曾试图规避一片异常区域格网。那不是普通风暴，也不是单点障碍，而是一张会影响导航、会误读信号、会把接近者拖进错误流程里的结构。事故从一开始就和这颗世界的系统失稳缠在一起。"),
        LT("Elias runs the black box residue through Loxi's filters four times before the last correction pattern separates from the crash noise. What emerges is deeply specific and therefore impossible to dismiss: the ship was not tumbling randomly in its final seconds. It was trying to steer away from a lattice contour that western scouts would later sketch from canopy observations. The correction attempts are visible. So is the pull that overrode them.\n\nHe breaks tone only once in the note, and that single break carries the whole truth: 'We were not dropped by weather. We hit someone else's failed maintenance scene at speed.' After this page, the forest scars, swamp echoes, and tower returns no longer belong to different stories. They lock into one chain of cause.",
           "伊莱亚斯把黑匣残留送进洛希的过滤表里跑了四遍，才终于把那段最后修正从撞击噪声里拽出来。结果具体到无法再推给巧合：飞船在坠地前的最后几秒并不是胡乱翻滚，它一直在试图避开一张后来又被西线侦察员从林冠上描出的格网轮廓。修正动作看得见，覆盖掉它的下拽也看得见。\n\n整份笔记里，伊莱亚斯只在一句话上失了平时那份冷静，而这句话也把事实一下子说死：‘我们不是被天气打下来，我们是高速撞进了别人已经失手的维护现场。’从这一页开始，森林里的伤痕、沼泽里的残响和塔楼回波就再也不属于不同故事了。")
    },
    {
        EXTERIOR_X(80),
        EXTERIOR_Y(98),
        SHIP_LOG_MAINLINE,
        LT("Facility Record: Purifier Outage Memo", "设施记录：净化器停摆备忘"),
        LT("The purifier outage memo renames the fog. It is not pure hostility. It is maintenance failure pushed back up to the surface after the chain lost its keepers.",
           "净化器停摆备忘把毒雾重新定义成维护失败，而不是纯粹敌意。区域会变得更凶险，不是因为世界突然起了杀心，而是因为原本压住失稳的净化链断开了，环境开始把没人照看的代价直接吐在地表上。"),
        LT("The outage memo is signed by Sera Dane and written in the crisp tone of someone explaining failure to an authority that is no longer present to judge her. Toxic backflow, she notes, did not surge because the fog 'turned hostile.' It surged because the purifier chain was already drifting, and the crash removed the last bodies still correcting its bad habits by hand. That distinction matters. Malice is easier to hate than neglect.\n\nIn the margin she circles one sentence hard enough to leave a groove: 'North stone nodes are dampers, not the source. Tell anyone who sees them not to panic-fire first.' It is a tiny correction with enormous emotional force. The world is still dangerous, but part of what frightened everyone was also part of what had been trying to hold the damage back.",
           "这份停摆备忘署名塞拉·戴恩，口气像在向一个早已不在场的上级汇报。她写得非常清楚：毒性回流不是因为雾“突然变坏”，而是因为净化链本来就已经开始跑偏，坠毁又把最后那几个还在靠人手强行纠正的人一起掀走了。这个区别很重要，因为恶意容易憎恨，失修却更难面对。\n\n她在页边重重圈出一句，几乎把纸都压出一道痕：‘北侧石节点是阻尼器，不是源头。告诉看见它们的人，别先慌着开火。’这句小小的纠偏分量极大。世界依旧危险，但其中一部分曾吓坏所有人的东西，其实也一直在替整套系统拖住更大的失稳。")
    },
    {
        EXTERIOR_X(88),
        EXTERIOR_Y(96),
        SHIP_LOG_SUPPLEMENTAL,
        LT("Facility Record: Vent Calibration Handover", "设施记录：通风校准交接"),
        LT("The vent handover proves west and south were buying each other minutes, not just sharing data. Cleaner air and quieter signal windows were the same work seen from two ends.",
           "通风校准交接证明，西线与南线并不是各自挣扎。更干净的空气窗口和更安静的回声时段，本来就是同一套计划的一部分：一边替另一边争几分钟，一边替另一边多留一条还能走的路。"),
        LT("The handover is unsigned, but Loxi later tags the two main scripts as Tamsin and Sera. Tamsin promises a quieter signal window from the west if the canopy route holds long enough. Sera promises to push the vent chain back into tolerance so the northern push stops feeling like it is breathing through a fist. They are not trading favors. They are working the same wound from opposite sides.\n\nJonah adds a final line in smaller handwriting: 'This is not cooperation. It is one breath being kept alive from two directions.' That is why the note matters. It turns route pressure, bad air, and environmental hostility into one shared maintenance problem instead of three separate miseries.",
           "这份交接没有正式署名，但洛希后来把两段主要笔迹分别比对成了塔姆辛和塞拉。塔姆辛答应只要林冠那边还能稳住，就替南线争取一段更安静的信号窗口；塞拉则承诺把通风链推回容差内，好让北上的路别再像每一步都在掐着人的喉咙。她们不是在彼此帮忙，而是在从两头同时缝同一道裂口。\n\n乔纳在页底补了一句更小的字：‘这不是合作，这是从两个方向替同一口气续命。’也正因为这句话，这份交接把路线压力、坏空气和环境敌意一下子都并到了同一个问题下面。")
    },
    {
        EXTERIOR_X(106),
        EXTERIOR_Y(100),
        SHIP_LOG_MAINLINE,
        LT("Facility Record: Service Shaft Sync Record", "设施记录：维护井道同步记录"),
        LT("The service shaft sync record makes the worst truth plain: ship systems, purifier controls, and tower routing all share one backbone. You have been inside the same machine the whole time.",
           "维护井道同步记录把最难承认的事实写得很直接：飞船系统、净化控制和埋藏的塔楼格网共享同一条维护主干。你不是在几个区域之间来回解决不同问题，你一直都在从不同入口走进同一台坏掉的机器里。"),
        LT("The restored sync record comes with one engineering note from Elias that changes the map forever. Ship power, purifier control, buried routing, and the tower lattice all sit on the same maintenance backbone. In other words, the expedition never moved between separate problems as cleanly as it thought. It kept entering the same problem through different doors.\n\nHis last line carries the kind of tired humor people use only when the truth is too large to hold straight-faced: 'Good news: we are not lost. Bad news: we have been in the machine room the whole time.' After reading that, every reopened shaft feels less like a shortcut and more like consent to walk farther into the same failing system.",
           "这份同步记录里最要命的是伊莱亚斯补上的那句工程注释：飞船电网、净化控制、地下路由和塔楼格网全都挂在同一条维护主干上。换句话说，这场远征并没有像自己以为的那样在几个独立问题之间来回切换，而是一直在从不同门口反复走进同一个问题里。\n\n他最后留了一句只有太累的人才写得出的黑色幽默：‘好消息，我们没迷路；坏消息，我们一直都在机房里。’看到这里以后，所谓捷径就会失去那种轻松感。每重新打通一段井道，都更像是在同意自己继续往同一个故障核心里走。")
    },
    {
        EXTERIOR_X(117),
        EXTERIOR_Y(95),
        SHIP_LOG_SUPPLEMENTAL,
        LT("Facility Record: Purifier Ring Control Brief", "设施记录：净化环控制简报"),
        LT("The purifier ring brief changes everything with one buried note: the monolith endpoints are not the danger source. They are field controls holding back something larger.",
           "净化环控制简报最重要的一句，不是恢复顺序，而是页角那条几乎像怕人看不见的附注：石碑终端不是危险源，它们是场域控制器，是把更大失稳按在边界里的手。一旦明白这一点，高塔看起来就不再像武器，而更像一份职责。"),
        LT("The control brief comes out of Sera's manual and begins with pure sequence: purify, seal, then recirculate oxygen. Nothing dramatic, nothing mystical, just the order required if the region is to become survivable again. The note buried in the corner is what detonates the whole meaning of the page: 'Monolith endpoints are not the danger source. They are the hands holding a larger instability in place.'\n\nThat sentence recasts everything around the tower. If the stones are field controls rather than origin points, then force is no longer the only imaginable relation to the system. The tower stops looking like a prize to seize and starts looking like a decision that must be understood before it is touched.",
           "这份控制简报出自塞拉的工作手册，开头全是极其冷静的顺序：先净化，再封闭，最后才让氧气回流。没有任何神秘色彩，只有一套想让区域重新变得可住下去所必须遵守的步骤。真正把整页意义炸开的，是页角那句几乎像怕人忽略的夹注：‘石碑终端不是危险源，它们是把更大失稳按住的手。’\n\n这句话会把高塔周围的一切都改写。既然石碑是场域控制而不是源头，那么“对这套系统做什么”就不再只剩强攻一条路。高塔也不再像一件抢到就能用的战利品，而像一个必须先弄懂再碰的决定。")
    },
    {
        EXTERIOR_X(122),
        EXTERIOR_Y(102),
        SHIP_LOG_MAINLINE,
        LT("Facility Record: Root Vault Core Dossier", "设施记录：根脉核心档案"),
        LT("The Root Vault dossier joins every thread together. The planet did not summon your ship. Its damaged maintenance lattice misread the Ark as a compatible repair response, and every ending is changed by that correction.",
           "根脉核心档案把塔楼、石碑、净化环、毒雾和坠毁真正接成了一体。开拓者号并不是被某种意志故意召唤，而是被这张失稳维护格网误判成了一次可兼容的紧急维修响应。也正因为这句纠偏成立，强行救援、和平接入和长期定居三条路线，才都会变成诚实却带伤的答案。"),
        LT("The Root Vault dossier feels less like one report than a volume stitched together from the expedition's surviving minds. Loxi assembles Mara's command notes, Elias's diagnostics, Tamsin's route fragments, Sera's facility records, and Mira Soren's final moral objections into one buried history. Tower routing, purifier control, monolith resonance, toxic spread, and the crash itself all terminate in the same damaged lattice.\n\nIts most important correction is also the simplest: this world did not summon your ship out of intention. The lattice seized it because human comms and power signatures were misread as a compatible emergency repair node. That one sentence keeps the whole story honest. It is why the final routes can all remain true at once. Leave by force. Leave through understanding. Stay and inherit the work. None are clean. All are answers to the same corrected truth.",
           "根脉核心档案读起来不像一份单独完成的报告，更像洛希把远征中还剩下的几种意识重新缝成了一本总卷。玛拉的指挥记录、伊莱亚斯的诊断表、塔姆辛的路线碎片、塞拉的设施文书，连同米拉·索伦最后那些带着道德重量的异议，全都被并到了一起。高塔路由、净化控制、石碑共振、毒雾扩散，甚至你们的坠毁本身，最后都落回同一张受损格网上。\n\n而这份总卷最重要的纠偏，也是最朴素的一句：这颗世界从来不是出于某种意志在召唤你们，它只是把人类飞船的通讯和供能特征误判成了自己仍可兼容的一次紧急维修响应。正是这句话，让整个故事重新站稳，也让三条终局都能同时成立。强行离开、通过理解完成救援，或留下来继承维护，没有哪一条是干净的，但它们都是真相被纠正之后的诚实回答。")
    }
}};

const std::array<TaskMonsterSpec, 8> kMonsterSpecs = {{
    {92.0f, 6.0f,  {0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, false},
    {82.0f, 5.0f,  {0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0}, false},
    {148.0f, 10.0f, {0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, false},
    {142.0f, 9.0f, {0, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0}, false},
    {166.0f, 9.0f, {0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, false},
    {158.0f, 10.0f, {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0}, false},
    {224.0f, 15.0f,{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0}, false},
    {720.0f, 21.0f,{0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0}, true}
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

EventType TasksContent_GetDailyEvent(int dayCount) {
    if (dayCount < 0) {
        dayCount = 0;
    }

    return kEventCycle[static_cast<std::size_t>(dayCount % static_cast<int>(kEventCycle.size()))];
}
