#include "ui_story_internal.h"

#include "localization.h"
#include "task_runtime_internal.h"
#include "ui_runtime_internal.h"

#include <cstdio>
#include <cstring>

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
    },
    {
        LT("Heroic Rescue", "强行救援"),
        LT("Heroic Rescue With Records", "强行救援与完整记录"),
        LT("", ""),
        Color{255, 214, 154, 255},
        Color{48, 28, 20, 255},
        Color{12, 8, 12, 255}
    },
    {
        LT("Peaceful Rescue", "和平救援"),
        LT("Peaceful Rescue With Repair", "和平救援与修复成果"),
        LT("", ""),
        Color{166, 255, 226, 255},
        Color{16, 44, 40, 255},
        Color{6, 12, 18, 255}
    },
    {
        LT("Settlement Ending", "定居结局"),
        LT("Settlement With Legacy", "定居与遗产传承"),
        LT("", ""),
        Color{255, 214, 154, 255},
        Color{34, 28, 24, 255},
        Color{10, 8, 12, 255}
    }
};

static bool IsCrossX3Ready(const TaskSystem *tasks);

static int GetEndingBackdropIndex(GameEnding ending, const TaskSystem *tasks) {
    const bool fullArchiveReady = IsCrossX3Ready(tasks);

    switch (ending) {
        case ENDING_SETTLEMENT:
            return fullArchiveReady ? 6 : 0;
        case ENDING_FAILURE:
            return 1;
        case ENDING_HEROIC:
            return fullArchiveReady ? 4 : 2;
        case ENDING_PEACEFUL:
            return fullArchiveReady ? 5 : 3;
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
                return Loc_Translate("After Loxi's final review, the full archive frames this as an open-eyed rescue: get people home first, even if the damaged lattice leaves with one more scar.");
            case ENDING_PEACEFUL:
                return Loc_Translate("After Loxi's final review, the full archive frames this as rescue through compatibility: not tearing the tower wider open, but teaching human systems to be read without deepening the wound.");
            case ENDING_SETTLEMENT:
                return Loc_Translate("After Loxi's final review, settlement stops reading like surrender. It becomes deliberate stewardship: stay, stabilize what can be kept alive, and inherit the work no one else is returning to finish.");
            case ENDING_FAILURE:
                return Loc_Translate("Even after Loxi's final review, air, pressure, and time still failed to hold. The truth was assembled, but the run broke before any final answer could live long enough to stand.");
            case ENDING_NONE:
            default:
                return "";
        }
    }

    if (IsCrossX2Ready(tasks)) {
        switch (ending) {
            case ENDING_HEROIC:
                return Loc_Translate("By the time Loxi confirmed it, the late findings had already shown tower, purifier ring, and monoliths were one damaged lattice, which meant any forced rescue had to be chosen as a cost, not mistaken for clean victory.");
            case ENDING_PEACEFUL:
                return Loc_Translate("By the time Loxi confirmed it, the late findings had already shown tower and purifier controls belonged to the same failing system, so stabilization demanded sequence, patience, and correct reading rather than brute force.");
            case ENDING_SETTLEMENT:
                return Loc_Translate("By the time Loxi confirmed it, settlement already meant inheriting the maintenance lattice instead of escaping it, along with every unfinished repair the earlier crew could not carry across the line.");
            case ENDING_FAILURE:
                return Loc_Translate("Loxi could already read the late findings, but the damaged lattice still outlasted the run. Understanding arrived, yet the body and the ship failed before it could be fully answered.");
            case ENDING_NONE:
            default:
                return "";
        }
    }

    if (IsCrossX1Ready(tasks)) {
        switch (ending) {
            case ENDING_HEROIC:
                return Loc_Translate("By the time Loxi confirmed it, west-route handoffs and south-route facility records had already aligned into one survival timeline, turning rescue into an inheritance of everyone who kept the chain alive.");
            case ENDING_PEACEFUL:
                return Loc_Translate("By the time Loxi confirmed it, the archive had already shown that coordination mattered more than panic: cleaner air, quieter signal windows, and shared timing all belonged to the same answer.");
            case ENDING_SETTLEMENT:
                return Loc_Translate("By the time Loxi confirmed it, the archive had already shown that staying was deliberate, not surrender. The earlier crew had already begun imagining a future that did not end with immediate escape.");
            case ENDING_FAILURE:
                return Loc_Translate("By the time Loxi framed the choice, the shared trail was finally visible, but the run still collapsed before that fragile line of meaning could be carried all the way through.");
            case ENDING_NONE:
            default:
                return "";
        }
    }

    return "";
}

static void BuildEndingBadges(const TaskSystem *tasks, const Player *player, char *buffer, size_t bufferSize) {
    bool wroteAny = false;

    if (buffer == nullptr || bufferSize == 0) {
        return;
    }

    buffer[0] = '\0';
    if (player != nullptr && player->deathCount == 0) {
        std::snprintf(buffer + std::strlen(buffer),
                      bufferSize - std::strlen(buffer),
                      "%s%s",
                      wroteAny ? "  " : "",
                      Loc_PickLiteral("No Death", "零死亡"));
        wroteAny = true;
    }
    if (tasks != nullptr && Tasks_GetCollectedLogCount(tasks) >= tasks->logCount && tasks->logCount > 0) {
        std::snprintf(buffer + std::strlen(buffer),
                      bufferSize - std::strlen(buffer),
                      "%s%s",
                      wroteAny ? "  " : "",
                      Loc_PickLiteral("All Logs Recovered", "日志全收集"));
        wroteAny = true;
    }
    if (tasks != nullptr
        && Tasks_GetCombatEncounterCount() > 0
        && Tasks_GetCombatScore(tasks) >= Tasks_GetCombatScoreMax()) {
        std::snprintf(buffer + std::strlen(buffer),
                      bufferSize - std::strlen(buffer),
                      "%s%s",
                      wroteAny ? "  " : "",
                      Loc_PickLiteral("All Encounters Cleared", "关键遭遇全清"));
        wroteAny = true;
    }
}

static void BuildEncounterHighlights(const TaskSystem *tasks, char *buffer, size_t bufferSize) {
    bool wroteAny = false;

    if (buffer == nullptr || bufferSize == 0) {
        return;
    }

    buffer[0] = '\0';
    if (tasks == nullptr) {
        return;
    }

    std::snprintf(buffer + std::strlen(buffer),
                  bufferSize - std::strlen(buffer),
                  "%s ",
                  Loc_PickLiteral("Cleared", "已清除"));
    for (int encounter = COMBAT_ENCOUNTER_WEST_FRONTIER; encounter < COMBAT_ENCOUNTER_COUNT; ++encounter) {
        const CombatEncounterId encounterId = static_cast<CombatEncounterId>(encounter);

        if (!Tasks_IsCombatEncounterCompleted(tasks, encounterId)) {
            continue;
        }
        std::snprintf(buffer + std::strlen(buffer),
                      bufferSize - std::strlen(buffer),
                      "%s%s",
                      wroteAny ? " / " : "",
                      Tasks_GetCombatEncounterName(encounterId));
        wroteAny = true;
    }

    if (!wroteAny) {
        std::snprintf(buffer,
                      bufferSize,
                      "%s",
                      Loc_PickLiteral("Cleared none", "尚未清除关键遭遇"));
    }
}

#undef LT

void UI_DrawEnding(GameEnding ending, const Player *player, const TaskSystem *tasks, const AssetBundle *assets, int screenWidth, int screenHeight, float elapsedSeconds) {
    char detail[256];
    char combatSummary[256];
    char encounterHighlights[256];
    char scoreBreakdown[256];
    char badgeSummary[256];
    char contextBuffer[256];
    const char *scoreRank;
    const char *routeContext;
    int endingBackdropIndex;
    int endingScore;
    int archiveScore;
    int investigationScore;
    int combatScore;
    int survivalScore;
    int completionScore;
    int completedEncounterCount;
    const IntroSlideDef *endingBackdrop;
    const TextureAsset *endingTexture;
    float scale;
    Rectangle textPanel;
    Rectangle bodyRect;
    Rectangle contextRect;
    Rectangle breakdownRect;
    Rectangle combatRect;
    Rectangle encounterRect;
    Rectangle badgeRect;

    endingBackdropIndex = GetEndingBackdropIndex(ending, tasks);
    endingBackdrop = &kEndingBackdropDefs[endingBackdropIndex];
    endingTexture = &assets->storyEndingScenes[endingBackdropIndex];
    scale = UIRuntime_GetScale(screenWidth, screenHeight);
    textPanel = Rectangle{
        screenWidth * 0.5f - 470.0f * scale,
        screenHeight - 304.0f * scale,
        940.0f * scale,
        230.0f * scale
    };
    bodyRect = Rectangle{
        textPanel.x + 32.0f * scale,
        textPanel.y + 72.0f * scale,
        textPanel.width - 64.0f * scale,
        52.0f * scale
    };
    contextRect = Rectangle{
        textPanel.x + 32.0f * scale,
        textPanel.y + 128.0f * scale,
        textPanel.width - 64.0f * scale,
        20.0f * scale
    };
    breakdownRect = Rectangle{
        textPanel.x + 32.0f * scale,
        textPanel.y + 146.0f * scale,
        textPanel.width - 64.0f * scale,
        20.0f * scale
    };
    combatRect = Rectangle{
        textPanel.x + 32.0f * scale,
        textPanel.y + 164.0f * scale,
        textPanel.width - 64.0f * scale,
        20.0f * scale
    };
    encounterRect = Rectangle{
        textPanel.x + 32.0f * scale,
        textPanel.y + 184.0f * scale,
        textPanel.width - 64.0f * scale,
        28.0f * scale
    };
    badgeRect = Rectangle{
        textPanel.x + 32.0f * scale,
        textPanel.y + 208.0f * scale,
        textPanel.width - 64.0f * scale,
        20.0f * scale
    };
    routeContext = GetEndingRouteContext(ending, tasks);
    endingScore = Tasks_CalculateEndingScore(tasks, player);
    archiveScore = Tasks_GetArchiveScore(tasks);
    investigationScore = Tasks_GetInvestigationScore(tasks);
    combatScore = Tasks_GetCombatScore(tasks);
    survivalScore = Tasks_GetSurvivalScore(tasks, player);
    completionScore = Tasks_GetEndingCompletionScore(tasks, player);
    completedEncounterCount = 0;
    for (int encounter = COMBAT_ENCOUNTER_WEST_FRONTIER; encounter < COMBAT_ENCOUNTER_COUNT; ++encounter) {
        if (Tasks_IsCombatEncounterCompleted(tasks, static_cast<CombatEncounterId>(encounter))) {
            completedEncounterCount += 1;
        }
    }
    scoreRank = Tasks_GetEndingScoreRank(endingScore);
    TasksRuntime_SanitizeDisplayText(routeContext, contextBuffer, sizeof(contextBuffer));
    std::snprintf(scoreBreakdown,
                  sizeof(scoreBreakdown),
                  "%s %d  %s %d  %s %d  %s %d",
                  Loc_PickLiteral("Archive", "档案"),
                  archiveScore,
                  Loc_PickLiteral("Route", "路线"),
                  investigationScore,
                  Loc_PickLiteral("Survival", "生存"),
                  survivalScore,
                  Loc_PickLiteral("Ending", "结局"),
                  completionScore);
    std::snprintf(combatSummary,
                  sizeof(combatSummary),
                  "%s %d/%d  %s %d/%d",
                  Loc_PickLiteral("Combat", "战斗分"),
                  combatScore,
                  Tasks_GetCombatScoreMax(),
                  Loc_PickLiteral("Key Encounters", "关键遭遇"),
                  completedEncounterCount,
                  Tasks_GetCombatEncounterCount());
    BuildEncounterHighlights(tasks, encounterHighlights, sizeof(encounterHighlights));
    BuildEndingBadges(tasks, player, badgeSummary, sizeof(badgeSummary));

    UIStory_DrawBackdropTexture(endingTexture, endingBackdrop, endingBackdropIndex + 40, screenWidth, screenHeight, elapsedSeconds, scale);
    DrawRectangleGradientV(0, 0, screenWidth, screenHeight, Color{4, 8, 14, 52}, Color{2, 6, 12, 230});
    UIRuntime_DrawPanel(textPanel, Color{6, 12, 21, 198}, Color{endingBackdrop->accent.r, endingBackdrop->accent.g, endingBackdrop->accent.b, 78});
    UIRuntime_DrawText(assets, Tasks_GetEndingTitle(ending), Vector2{textPanel.x + 30.0f * scale, textPanel.y + 24.0f * scale}, 36.0f * scale, WHITE);
    UIRuntime_DrawWrappedText(assets, Tasks_GetEndingBody(ending), bodyRect, 17.0f * scale, 21.0f * scale, Color{223, 234, 242, 255});
    if (contextBuffer[0] != '\0') {
        UIRuntime_DrawWrappedText(assets, contextBuffer, contextRect, 13.5f * scale, 15.0f * scale, Color{188, 218, 236, 255});
    }
    UIRuntime_DrawWrappedText(assets, scoreBreakdown, breakdownRect, 13.2f * scale, 15.0f * scale, Color{203, 219, 235, 255});
    UIRuntime_DrawWrappedText(assets, combatSummary, combatRect, 14.0f * scale, 16.0f * scale, Color{255, 214, 154, 255});
    UIRuntime_DrawWrappedText(assets, encounterHighlights, encounterRect, 12.6f * scale, 14.5f * scale, Color{196, 226, 241, 255});
    if (badgeSummary[0] != '\0') {
        UIRuntime_DrawWrappedText(assets, badgeSummary, badgeRect, 13.0f * scale, 15.0f * scale, Color{184, 230, 210, 255});
    }
    std::snprintf(detail,
                  sizeof(detail),
                  "%s %d  %s %s  %s %.0f  %s %.0f  %s %d",
                  Loc_PickLiteral("Score", "得分"),
                  endingScore,
                  Loc_PickLiteral("Rank", "评级"),
                  scoreRank,
                  Loc_PickLiteral("Health", "生命"),
                  player->health,
                  Loc_PickLiteral("Oxygen", "氧气"),
                  player->oxygen,
                  Loc_PickLiteral("Total Deaths", "总死亡次数"),
                  player->deathCount);
    UIRuntime_DrawText(assets, detail, Vector2{textPanel.x + 30.0f * scale, textPanel.y + textPanel.height - 24.0f * scale}, 15.5f * scale, Color{255, 205, 151, 255});
    UIRuntime_DrawText(assets, LOC_UI_END_EXIT, Vector2{screenWidth - UIRuntime_MeasureText(assets, LOC_UI_END_EXIT, 16.0f * scale).x - 28.0f * scale, screenHeight - 30.0f * scale}, 16.0f * scale, Color{196, 213, 225, 255});
}
