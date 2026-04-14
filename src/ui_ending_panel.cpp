#include "ui_story_internal.h"

#include "localization.h"
#include "task_runtime_internal.h"
#include "ui_runtime_internal.h"

#include <cstdio>

#define LT(en, zh) LocalizedText{en, zh}

static const IntroSlideDef kEndingBackdropDefs[STORY_ENDING_SCENE_COUNT] = {
    {
        LT("Settlement Ending", "定居结局"),
        LT("Alien Settlement", "异星定居"),
        LT("", ""),
        Color{255, 214, 154, 255},
        Color{34, 28, 24, 255},
        Color{10, 8, 12, 255}
    },
    {
        LT("Failure Ending", "失败结局"),
        LT("Failed Survival", "生存失败"),
        LT("", ""),
        Color{255, 150, 150, 255},
        Color{40, 16, 18, 255},
        Color{10, 6, 10, 255}
    },
    {
        LT("Heroic Rescue", "强行救援"),
        LT("Heroic Rescue", "强行救援"),
        LT("", ""),
        Color{255, 214, 154, 255},
        Color{42, 28, 22, 255},
        Color{10, 8, 12, 255}
    },
    {
        LT("Peaceful Rescue", "和平救援"),
        LT("Peaceful Rescue", "和平救援"),
        LT("", ""),
        Color{166, 255, 226, 255},
        Color{16, 42, 42, 255},
        Color{6, 12, 18, 255}
    }
};

static int GetEndingBackdropIndex(GameEnding ending) {
    switch (ending) {
        case ENDING_SETTLEMENT:
            return 0;
        case ENDING_FAILURE:
            return 1;
        case ENDING_HEROIC:
            return 2;
        case ENDING_PEACEFUL:
            return 3;
        case ENDING_NONE:
        default:
            return 2;
    }
}

static bool IsCrossX1Ready(const TaskSystem *tasks) {
    return tasks != nullptr
        && tasks->westW3Completed
        && tasks->southS2Completed;
}

static bool IsCrossX2Ready(const TaskSystem *tasks) {
    return tasks != nullptr
        && tasks->westW4Completed
        && tasks->southS4Completed;
}

static bool IsCrossX3Ready(const TaskSystem *tasks) {
    return tasks != nullptr
        && tasks->westW5Completed
        && tasks->southS5Completed;
}

static const char *GetEndingRouteContext(GameEnding ending, const TaskSystem *tasks) {
    if (tasks == nullptr) {
        return "";
    }

    if (IsCrossX3Ready(tasks)) {
        switch (ending) {
            case ENDING_HEROIC:
                return Loc_Translate("With the full archive in hand, west and south findings confirm a deliberate high-cost rescue.");
            case ENDING_PEACEFUL:
                return Loc_Translate("With the full archive in hand, west and south findings frame this as full-system stabilization through understanding.");
            case ENDING_SETTLEMENT:
                return Loc_Translate("With the full archive in hand, west and south findings frame settlement as deliberate long-term stewardship.");
            case ENDING_FAILURE:
                return Loc_Translate("Even with the full archive in hand, the final attempt still collapsed under sustained pressure.");
            case ENDING_NONE:
            default:
                return "";
        }
    }

    if (IsCrossX2Ready(tasks)) {
        switch (ending) {
            case ENDING_HEROIC:
                return Loc_Translate("With the late findings revealed, you chose force knowing the tower, purifier ring, and monoliths were one damaged maintenance lattice.");
            case ENDING_PEACEFUL:
                return Loc_Translate("With the late findings revealed, you chose stabilization knowing the tower and purifier controls were the same failing system.");
            case ENDING_SETTLEMENT:
                return Loc_Translate("With the late findings revealed, settlement means inheriting the maintenance lattice instead of escaping it.");
            case ENDING_FAILURE:
                return Loc_Translate("The late findings were already visible, but the damaged lattice still outlasted the run.");
            case ENDING_NONE:
            default:
                return "";
        }
    }

    if (IsCrossX1Ready(tasks)) {
        switch (ending) {
            case ENDING_HEROIC:
                return Loc_Translate("The shared trail was already visible: west crew handoffs and south facility records had aligned into one survival timeline.");
            case ENDING_PEACEFUL:
                return Loc_Translate("The shared trail was already visible: the archive had already shown that coordination mattered more than panic.");
            case ENDING_SETTLEMENT:
                return Loc_Translate("The shared trail was already visible: the archive had already shown that staying was a deliberate option, not surrender.");
            case ENDING_FAILURE:
                return Loc_Translate("The shared trail was already visible, but the run still collapsed before it could be carried through.");
            case ENDING_NONE:
            default:
                return "";
        }
    }

    return "";
}

#undef LT

void UI_DrawEnding(GameEnding ending, const Player *player, const TaskSystem *tasks, const AssetBundle *assets, int screenWidth, int screenHeight, float elapsedSeconds) {
    char detail[256];
    char contextBuffer[256];
    const char *routeContext;
    int endingBackdropIndex;
    const IntroSlideDef *endingBackdrop;
    const TextureAsset *endingTexture;
    float scale;
    Rectangle textPanel;
    Rectangle bodyRect;
    Rectangle contextRect;

    endingBackdropIndex = GetEndingBackdropIndex(ending);
    endingBackdrop = &kEndingBackdropDefs[endingBackdropIndex];
    endingTexture = &assets->storyEndingScenes[endingBackdropIndex];
    scale = UIRuntime_GetScale(screenWidth, screenHeight);
    textPanel = Rectangle{
        screenWidth * 0.5f - 470.0f * scale,
        screenHeight - 246.0f * scale,
        940.0f * scale,
        172.0f * scale
    };
    bodyRect = Rectangle{
        textPanel.x + 32.0f * scale,
        textPanel.y + 72.0f * scale,
        textPanel.width - 64.0f * scale,
        60.0f * scale
    };
    contextRect = Rectangle{
        textPanel.x + 32.0f * scale,
        textPanel.y + 136.0f * scale,
        textPanel.width - 64.0f * scale,
        20.0f * scale
    };
    routeContext = GetEndingRouteContext(ending, tasks);
    TasksRuntime_SanitizeDisplayText(routeContext, contextBuffer, sizeof(contextBuffer));

    UIStory_DrawBackdropTexture(endingTexture, endingBackdrop, endingBackdropIndex + 40, screenWidth, screenHeight, elapsedSeconds, scale);
    DrawRectangleGradientV(0, 0, screenWidth, screenHeight, Color{4, 8, 14, 52}, Color{2, 6, 12, 230});
    UIRuntime_DrawPanel(textPanel, Color{6, 12, 21, 198}, Color{endingBackdrop->accent.r, endingBackdrop->accent.g, endingBackdrop->accent.b, 78});
    UIRuntime_DrawText(assets, Tasks_GetEndingTitle(ending), Vector2{textPanel.x + 30.0f * scale, textPanel.y + 24.0f * scale}, 36.0f * scale, WHITE);
    UIRuntime_DrawWrappedText(assets, Tasks_GetEndingBody(ending), bodyRect, 17.0f * scale, 21.0f * scale, Color{223, 234, 242, 255});
    if (contextBuffer[0] != '\0') {
        UIRuntime_DrawWrappedText(assets, contextBuffer, contextRect, 13.5f * scale, 15.0f * scale, Color{188, 218, 236, 255});
    }
    std::snprintf(detail,
                  sizeof(detail),
                  "%s %.0f  %s %.0f  %s %d",
                  Loc_PickLiteral("Health", "生命"),
                  player->health,
                  Loc_PickLiteral("Oxygen", "氧气"),
                  player->oxygen,
                  Loc_PickLiteral("Total Deaths", "总死亡次数"),
                  player->deathCount);
    UIRuntime_DrawText(assets, detail, Vector2{textPanel.x + 30.0f * scale, textPanel.y + textPanel.height - 28.0f * scale}, 15.5f * scale, Color{255, 205, 151, 255});
    UIRuntime_DrawText(assets, LOC_UI_END_EXIT, Vector2{screenWidth - UIRuntime_MeasureText(assets, LOC_UI_END_EXIT, 16.0f * scale).x - 28.0f * scale, screenHeight - 30.0f * scale}, 16.0f * scale, Color{196, 213, 225, 255});
}
