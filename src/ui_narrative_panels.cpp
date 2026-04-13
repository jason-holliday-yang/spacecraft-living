#include "ui_story_internal.h"

#include "ui_runtime_internal.h"

#include <cmath>

static const IntroSlideDef kIntroSlideDefs[INTRO_CUTSCENE_SLIDE_COUNT] = {
    {
        "Signal Trace 01",
        "An Unmarked Call",
        "The return run should have been quiet. Then a distress pulse cut through the cockpit speakers. No registry. No ship ID. Only coordinates wrapped around a world that should not have been there.",
        Color{114, 226, 255, 255},
        Color{12, 34, 58, 255},
        Color{4, 10, 20, 255}
    },
    {
        "Impact Record 02",
        "Orbit Collapse",
        "The moment the ship entered the upper atmosphere, the signal surged. Navigation died. Thrusters stalled. Something inside the storm reached up and dragged the hull straight down.",
        Color{255, 169, 129, 255},
        Color{78, 26, 24, 255},
        Color{18, 8, 14, 255}
    },
    {
        "Impact Record 03",
        "Barely Alive",
        "You wake to twisted metal, emergency sparks, and the taste of smoke. Main power is gone. Oxygen reserves are falling. The ship is still standing, but only by habit and luck.",
        Color{255, 214, 154, 255},
        Color{34, 33, 52, 255},
        Color{8, 10, 18, 255}
    },
    {
        "Survey Trace 04",
        "A World That Watches Back",
        "Outside, the air is thin but survivable. Scans catch movement in the dark and structures no human survey ever logged. The signal is still broadcasting from deeper inland, as if it knows you survived.",
        Color{137, 238, 186, 255},
        Color{16, 52, 42, 255},
        Color{4, 18, 20, 255}
    },
    {
        "Loxi Boot 05",
        "Stay Alive First",
        "Your portable terminal flickers back to life. Gather supplies. Restore oxygen and power. Repair the comms array. Stay alive long enough to learn why this planet called you here.",
        Color{166, 255, 226, 255},
        Color{18, 38, 64, 255},
        Color{5, 10, 19, 255}
    }
};

static const IntroSlideDef kStoryMainDefs[STORY_MAIN_SCENE_COUNT] = {
    {
        "Repair Log",
        "Air For One More Day",
        "The lower-deck oxygen console coughs back to life under your hands. It is not a full repair, only enough clean air to keep the wreck from becoming your tomb tonight.",
        Color{124, 225, 255, 255},
        Color{10, 34, 54, 255},
        Color{4, 10, 18, 255}
    },
    {
        "Repair Log",
        "Breathing Room Restored",
        "The full oxygen cycle steadies across the base. For the first time since impact, every breath no longer feels borrowed. Survival stops being a countdown and becomes a plan.",
        Color{166, 255, 226, 255},
        Color{14, 38, 58, 255},
        Color{5, 10, 19, 255}
    },
    {
        "Loxi Sync",
        "A Voice In The Wreck",
        "The upper terminal finally stabilizes. Loxi returns in full, no longer a fragment of emergency code, but a calm voice in a dead ship that still remembers the mission and the signal waiting outside.",
        Color{123, 225, 255, 255},
        Color{16, 34, 62, 255},
        Color{6, 10, 18, 255}
    },
    {
        "Airlock Cycle",
        "The World Outside Opens",
        "The blast door unlocks with a grinding shudder. Beyond the threshold is breathable air, unstable ground, and a landscape that feels less discovered than waiting.",
        Color{255, 214, 154, 255},
        Color{34, 34, 46, 255},
        Color{8, 10, 16, 255}
    },
    {
        "Relay Repair",
        "The Signal Answers Back",
        "The comm relay rises from static and dead metal. Loxi confirms the source is still active, but what comes through is not a distress call. It behaves more like a summons that never expected refusal.",
        Color{110, 233, 255, 255},
        Color{14, 36, 56, 255},
        Color{5, 10, 18, 255}
    },
    {
        "Crash Analysis",
        "This Was Not An Accident",
        "The wreckage tells a different story than the cockpit logs. Burn marks, drag traces, and warped readings suggest the ship was pulled down, not simply lost to weather or bad luck.",
        Color{255, 186, 145, 255},
        Color{56, 24, 24, 255},
        Color{18, 8, 12, 255}
    },
    {
        "Power Restore",
        "The Base Wakes Up",
        "When the energy console comes online, dormant systems across the ship answer in sequence. Light returns to empty rooms. Loxi can finally scan farther inland, and every new trace points toward the ruins.",
        Color{166, 255, 226, 255},
        Color{18, 40, 62, 255},
        Color{6, 10, 18, 255}
    },
    {
        "Relic Analysis",
        "The Pattern Is Alien",
        "Three relic fragments are enough for Loxi to decode the signal structure. It is partly mathematical, partly architectural, and unmistakably non-human. The tower can be forced, but it can also be understood.",
        Color{174, 226, 255, 255},
        Color{22, 36, 66, 255},
        Color{6, 10, 20, 255}
    },
    {
        "Monolith Contact",
        "The Ruins Notice You",
        "The first monolith stirs as if it has been waiting for a witness. Its light feels less like magic and more like an ancient security system deciding whether you are allowed to go any farther.",
        Color{137, 238, 186, 255},
        Color{16, 42, 40, 255},
        Color{4, 14, 18, 255}
    },
    {
        "Monolith Harmony",
        "The Sequence Holds",
        "All three monoliths resonate in perfect order. Something deep in the ruin shifts beneath your feet. Loxi believes the guardian has just lost part of whatever power kept it waiting above you.",
        Color{166, 255, 226, 255},
        Color{14, 44, 46, 255},
        Color{4, 16, 20, 255}
    },
    {
        "Guardian Fall",
        "The Last Barrier Breaks",
        "The ruins fall quiet as the guardian collapses. No triumph answers back, only the sudden stillness that comes when the last barrier between you and the tower is gone.",
        Color{255, 198, 156, 255},
        Color{44, 24, 24, 255},
        Color{14, 8, 12, 255}
    },
    {
        "Rescue Route",
        "Beacon Through Force",
        "With the guardian down, the signal tower obeys manual control. The rescue beacon cuts into the sky, carrying proof that someone survived long enough to fight this world and leave a scar on it.",
        Color{255, 214, 154, 255},
        Color{44, 28, 20, 255},
        Color{12, 8, 12, 255}
    },
    {
        "Rescue Route",
        "Beacon Through Understanding",
        "The amplifier folds human systems into the tower's language. No last battle. No final breach. The signal rises because, at last, the ruins accept your presence instead of trying to erase it.",
        Color{166, 255, 226, 255},
        Color{18, 44, 44, 255},
        Color{6, 12, 18, 255}
    }
};

static const IntroSlideDef kStoryLogDefs[STORY_LOG_SCENE_COUNT] = {
    {
        "Ship Log 01",
        "Impact Protocol",
        "The first recovered entry confirms the crew survived impact long enough to define the new rule of survival: secure air, accept the crash, and delay fear until the ship can keep someone breathing.",
        Color{123, 225, 255, 255},
        Color{14, 34, 58, 255},
        Color{4, 10, 18, 255}
    },
    {
        "Ship Log 02",
        "Split Roster",
        "The second log reframes the missing crew as a deliberate split. Different teams pushed west, east, and ship-side maintenance so at least one route could carry answers home.",
        Color{255, 214, 154, 255},
        Color{42, 30, 24, 255},
        Color{10, 8, 14, 255}
    },
    {
        "Ship Log 03",
        "Pattern, Not Wilderness",
        "The third log suggests the planet is engineered rather than wild. Monoliths, purifier rhythms, and tower signal all belong to one unfinished system that still reacts to every intrusion.",
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
    UIRuntime_DrawText(assets, slide->eyebrow, eyebrowPosition, 15.0f * scale, slide->accent);
    UIRuntime_DrawText(assets, slide->title, titlePosition, 33.0f * scale, WHITE);
    UIRuntime_DrawWrappedText(assets, slide->body, bodyRect, 18.0f * scale, 22.0f * scale, Color{223, 234, 242, 255});

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
    hint = slideIndex == INTRO_CUTSCENE_SLIDE_COUNT - 1 ? "Enter / click to deploy. ESC to skip." : "Enter / click to continue. ESC to skip.";

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
    hint = "Enter / click to continue. ESC to close.";

    UIStory_DrawBackdropTexture(texture, slide, (int)scene, screenWidth, screenHeight, sceneElapsed, scale);
    UIStory_DrawNarrativePanel(assets, slide, textPanel, bodyRect, eyebrowPosition, titlePosition, hint, scale, screenWidth, screenHeight);
}
