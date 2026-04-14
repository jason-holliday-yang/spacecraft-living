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
        LT("Loxi Boot 05", "洛西启动 05"),
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
        LT("Loxi Sync", "洛西同步"),
        LT("A Voice In The Wreck", "残骸中的声音"),
        LT("The upper terminal finally stabilizes. Loxi returns in full, no longer a fragment of emergency code, but a calm voice in a dead ship that still remembers the mission and the signal waiting outside.",
           "上层终端终于稳定下来。洛西完整归来，不再只是残缺的应急代码，而成了这艘死船里仍记得任务和外头那道信号的冷静声音。"),
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
           "通讯中继从静电和死寂金属里重新响起。洛西确认信号源仍然活跃，但传来的并不像求救，而更像一场从未预想你会拒绝的召唤。"),
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
           "当动力控制台重新上线，飞船各处沉睡的系统逐个回应。灯光回到空荡舱室，洛西终于能向更远的内陆扫描，而每一道新痕迹都指向遗迹。"),
        Color{166, 255, 226, 255},
        Color{18, 40, 62, 255},
        Color{6, 10, 18, 255}
    },
    {
        LT("Relic Analysis", "遗物分析"),
        LT("The Pattern Is Alien", "这套模式并非人类所有"),
        LT("Three relic fragments are enough for Loxi to decode the signal structure. It is partly mathematical, partly architectural, and unmistakably non-human. The tower can be forced, but it can also be understood.",
           "三枚遗迹碎片已足够让洛西解码信号结构。它既像数学，又像建筑，而且毫无疑问并非人类产物。塔楼既可以被强行启动，也可以被真正理解。"),
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
           "三座石碑按完美顺序同时共鸣。你脚下的遗迹深处传来变化。洛西认为，那只守卫刚刚失去了一部分让它盘踞于此的力量。"),
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
    }
};

const IntroSlideDef *UIStory_GetIntroSlideDef(int slideIndex) {
    if (slideIndex < 0 || slideIndex >= INTRO_CUTSCENE_SLIDE_COUNT) {
        return &kIntroSlideDefs[0];
    }

    return &kIntroSlideDefs[slideIndex];
}

const IntroSlideDef *UIStory_GetStorySceneDef(StoryScene scene) {
    if (scene >= STORY_SCENE_MAIN_OXYGEN_PATCH && scene <= STORY_SCENE_MAIN_SIGNAL_TOWER_PEACEFUL) {
        return &kStoryMainDefs[scene - STORY_SCENE_MAIN_OXYGEN_PATCH];
    }

    if (scene >= STORY_SCENE_LOG_THE_CRASH && scene <= STORY_SCENE_LOG_ALIEN_ECOLOGY) {
        return &kStoryLogDefs[scene - STORY_SCENE_LOG_THE_CRASH];
    }

    return &kStoryMainDefs[0];
}

const TextureAsset *UIStory_GetStorySceneTexture(const AssetBundle *assets, StoryScene scene) {
    if (scene >= STORY_SCENE_MAIN_OXYGEN_PATCH && scene <= STORY_SCENE_MAIN_SIGNAL_TOWER_PEACEFUL) {
        return &assets->storyMainScenes[scene - STORY_SCENE_MAIN_OXYGEN_PATCH];
    }

    if (scene >= STORY_SCENE_LOG_THE_CRASH && scene <= STORY_SCENE_LOG_ALIEN_ECOLOGY) {
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
