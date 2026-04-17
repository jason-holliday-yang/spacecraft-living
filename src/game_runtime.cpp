#include "game_manager_internal.h"

#include "localization.h"
#include "task_runtime_internal.h"

#include <cstdio>
#include <cstring>

void Game_PostMessage(Game *game, const char *text, float duration) {
    const char *displayText;

    if (game == NULL || text == NULL) {
        return;
    }

    displayText = Loc_Translate(text);
    TasksRuntime_SanitizeDisplayText(displayText, game->hudMessage.text, sizeof(game->hudMessage.text));
    game->hudMessage.timer = duration + 2.0f;
}

void Game_ClearMessage(Game *game) {
    if (game == NULL) {
        return;
    }

    game->hudMessage.text[0] = '\0';
    game->hudMessage.timer = 0.0f;
}

bool Game_OpenStoryScene(Game *game, StoryScene scene) {
    if (game == NULL || scene <= STORY_SCENE_NONE || scene >= STORY_SCENE_COUNT) {
        return false;
    }

    if (game->storySceneShown[scene] || game->storySceneOpen) {
        return false;
    }

    game->storySceneShown[scene] = true;
    game->storySceneOpen = true;
    game->storyScene = scene;
    game->storySceneElapsed = 0.0f;
    return true;
}

void Game_CloseStoryScene(Game *game) {
    if (game == NULL) {
        return;
    }

    game->storySceneOpen = false;
    game->storyScene = STORY_SCENE_NONE;
    game->storySceneElapsed = 0.0f;
    game->narrativeTransitionActive = false;
    game->narrativeTransitionElapsed = 0.0f;
    game->narrativeTransitionAction = NARRATIVE_TRANSITION_NONE;
}

AudioScene Game_SelectAudioScene(const Game *game) {
    MapArea area;
    const char *locationName;

    if (game == NULL) {
        return AUDIO_SCENE_NONE;
    }

    if (game->tasks.ending != ENDING_NONE) {
        return AUDIO_SCENE_ENDING;
    }

    area = Map_GetAreaAt(game->player.gridX, game->player.gridY);
    locationName = Map_GetLocationNameAt(game->player.gridX, game->player.gridY);

    if (area == MAP_AREA_BOSS_ARENA) {
        return AUDIO_SCENE_BOSS_ARENA;
    }

    if (!game->tasks.bossDefeated
        && game->tasks.stage >= 7
        && locationName != NULL
        && (std::strcmp(locationName, "Monolith Ring") == 0
            || std::strcmp(locationName, "Signal Tower Plateau") == 0)) {
        return AUDIO_SCENE_BOSS;
    }

    if (locationName != NULL) {
        if (std::strcmp(locationName, "West Frontier") == 0
            || std::strcmp(locationName, "Survey Break") == 0
            || std::strcmp(locationName, "Canopy Hollow") == 0
            || std::strcmp(locationName, "Echo Basin") == 0
            || std::strcmp(locationName, "Last Camp") == 0) {
            return AUDIO_SCENE_FOREST_ROUTE;
        }
        if (std::strcmp(locationName, "Deep Gate") == 0
            || std::strcmp(locationName, "Deep Basin") == 0) {
            return AUDIO_SCENE_SWAMP_DEEP;
        }
        if (std::strcmp(locationName, "South Collapse") == 0
            || std::strcmp(locationName, "Vent Galleries") == 0
            || std::strcmp(locationName, "Service Shafts") == 0
            || std::strcmp(locationName, "Purifier Ring") == 0
            || std::strcmp(locationName, "Root Vault") == 0) {
            return AUDIO_SCENE_RUINS_FACILITY;
        }
    }

    switch (area) {
        case MAP_AREA_BASE:
            return AUDIO_SCENE_BASE;
        case MAP_AREA_FOREST:
            return AUDIO_SCENE_FOREST;
        case MAP_AREA_SWAMP_OUTER:
            return AUDIO_SCENE_SWAMP;
        case MAP_AREA_SWAMP_DEEP:
            return AUDIO_SCENE_SWAMP_DEEP;
        case MAP_AREA_RUINS:
            return AUDIO_SCENE_RUINS;
        case MAP_AREA_UNKNOWN:
        default:
            return AUDIO_SCENE_BASE;
    }
}

void Game_SyncTrackedLocation(Game *game) {
    const char *locationName;

    if (game == NULL) {
        return;
    }

    locationName = Map_GetLocationNameAt(game->player.gridX, game->player.gridY);
    if (locationName == NULL) {
        game->lastLocationName[0] = '\0';
        return;
    }

    std::snprintf(game->lastLocationName, sizeof(game->lastLocationName), "%s", locationName);
}

static bool IsTrackedRuinsLocation(const char *locationName) {
    return locationName != NULL
        && (std::strcmp(locationName, "Ruins Approach") == 0
            || std::strcmp(locationName, "Monolith Ring") == 0
            || std::strcmp(locationName, "Signal Tower Plateau") == 0);
}

static bool IsTrackedEastLocation(const char *locationName) {
    return locationName != NULL
        && (std::strcmp(locationName, "Outer Swamp Rim") == 0
            || std::strcmp(locationName, "Flooded Detour") == 0
            || std::strcmp(locationName, "Deep Gate") == 0
            || std::strcmp(locationName, "Deep Basin") == 0);
}

static bool IsTrackedBoundaryLocation(const char *locationName) {
    return locationName != NULL
        && (std::strcmp(locationName, "West Frontier") == 0
            || std::strcmp(locationName, "Survey Break") == 0
            || std::strcmp(locationName, "Canopy Hollow") == 0
            || std::strcmp(locationName, "Echo Basin") == 0
            || std::strcmp(locationName, "Last Camp") == 0
            || std::strcmp(locationName, "South Collapse") == 0
            || std::strcmp(locationName, "Vent Galleries") == 0
            || std::strcmp(locationName, "Service Shafts") == 0
            || std::strcmp(locationName, "Purifier Ring") == 0
            || std::strcmp(locationName, "Root Vault") == 0);
}

static bool IsWestRouteAvailable(const TaskSystem *tasks) {
    return tasks != NULL
        && tasks->stage >= 4
        && tasks->commRepairLevel >= 1;
}

static bool IsSouthRouteAvailable(const TaskSystem *tasks) {
    return tasks != NULL
        && tasks->stage >= 5
        && tasks->energyRepairLevel >= 1;
}

static bool IsCrossX1Ready(const TaskSystem *tasks) {
    return tasks != NULL
        && tasks->westW3Completed
        && tasks->southS2Completed;
}

static bool IsCrossX2Ready(const TaskSystem *tasks) {
    return tasks != NULL
        && tasks->westW4Completed
        && tasks->southS4Completed;
}

static bool IsCrossX3Ready(const TaskSystem *tasks) {
    return tasks != NULL
        && tasks->westW5Completed
        && tasks->southS5Completed;
}

static bool IsShipBaseLocation(const char *locationName) {
    return locationName != NULL
        && (std::strcmp(locationName, "Central Corridor") == 0
            || std::strcmp(locationName, "Command Deck") == 0
            || std::strcmp(locationName, "Workshop") == 0
            || std::strcmp(locationName, "Terminal Bay") == 0
            || std::strcmp(locationName, "Life Support") == 0
            || std::strcmp(locationName, "Life Support Bay") == 0
            || std::strcmp(locationName, "Cargo Hold") == 0
            || std::strcmp(locationName, "Diagnostics") == 0
            || std::strcmp(locationName, "Recovery Zone") == 0
            || std::strcmp(locationName, "Airlock Link") == 0
            || std::strcmp(locationName, "Airlock Passage") == 0
            || std::strcmp(locationName, "Power Bay") == 0);
}

static bool IsLogCollectedAtIndex(const TaskSystem *tasks, int logIndex) {
    return tasks != NULL
        && logIndex >= 0
        && logIndex < tasks->logCount
        && tasks->logs[logIndex].collected;
}

static int GetRequiredArchiveEvidenceLogIndex(const char *locationName) {
    if (locationName == NULL) {
        return -1;
    }

    if (std::strcmp(locationName, "West Frontier") == 0) {
        return 3;
    }
    if (std::strcmp(locationName, "Survey Break") == 0) {
        return 4;
    }
    if (std::strcmp(locationName, "Canopy Hollow") == 0) {
        return 5;
    }
    if (std::strcmp(locationName, "Echo Basin") == 0) {
        return 6;
    }
    if (std::strcmp(locationName, "Last Camp") == 0) {
        return 7;
    }
    if (std::strcmp(locationName, "South Collapse") == 0) {
        return 9;
    }
    if (std::strcmp(locationName, "Vent Galleries") == 0) {
        return 10;
    }
    if (std::strcmp(locationName, "Service Shafts") == 0) {
        return 11;
    }
    if (std::strcmp(locationName, "Purifier Ring") == 0) {
        return 12;
    }
    if (std::strcmp(locationName, "Root Vault") == 0) {
        return 13;
    }

    return -1;
}

static bool CanArchiveEvidencePass(const TaskSystem *tasks,
                                   const char *locationName,
                                   const char *previousLocationName,
                                   bool started,
                                   bool completed,
                                   const char *targetLocationName) {
    return IsShipBaseLocation(locationName)
        && previousLocationName != NULL
        && !IsShipBaseLocation(previousLocationName)
        && started
        && !completed
        && targetLocationName != NULL
        && IsLogCollectedAtIndex(tasks, GetRequiredArchiveEvidenceLogIndex(targetLocationName));
}

static bool TryAdvanceWestSouthRouteFlags(Game *game, const char *locationName, const char *previousLocationName) {
    if (game == NULL || locationName == NULL) {
        return false;
    }

    if (std::strcmp(locationName, "Survey Break") == 0
        && IsWestRouteAvailable(&game->tasks)
        && game->tasks.westW1Completed
        && !game->tasks.westW2Started) {
        game->tasks.westW2Started = true;
        return true;
    }

    if (std::strcmp(locationName, "Canopy Hollow") == 0
        && IsWestRouteAvailable(&game->tasks)
        && game->tasks.westW2Completed
        && !game->tasks.westW3Started) {
        game->tasks.westW3Started = true;
        return true;
    }

    if (std::strcmp(locationName, "Echo Basin") == 0
        && IsWestRouteAvailable(&game->tasks)
        && game->tasks.westW3Completed
        && !game->tasks.westW4Started) {
        game->tasks.westW4Started = true;
        return true;
    }

    if (std::strcmp(locationName, "Last Camp") == 0
        && IsWestRouteAvailable(&game->tasks)
        && game->tasks.westW4Completed
        && !game->tasks.westW5Started) {
        game->tasks.westW5Started = true;
        return true;
    }

    if (std::strcmp(locationName, "West Frontier") == 0
        && IsWestRouteAvailable(&game->tasks)
        && !game->tasks.westW1Started) {
        game->tasks.westW1Started = true;
        return true;
    }

    if (std::strcmp(locationName, "Vent Galleries") == 0
        && IsSouthRouteAvailable(&game->tasks)
        && game->tasks.southS1Completed
        && !game->tasks.southS2Started) {
        game->tasks.southS2Started = true;
        return true;
    }

    if (std::strcmp(locationName, "Service Shafts") == 0
        && IsSouthRouteAvailable(&game->tasks)
        && game->tasks.southS2Completed
        && !game->tasks.southS3Started) {
        game->tasks.southS3Started = true;
        return true;
    }

    if (std::strcmp(locationName, "Purifier Ring") == 0
        && IsSouthRouteAvailable(&game->tasks)
        && game->tasks.southS3Completed
        && !game->tasks.southS4Started) {
        game->tasks.southS4Started = true;
        return true;
    }

    if (std::strcmp(locationName, "Root Vault") == 0
        && IsSouthRouteAvailable(&game->tasks)
        && game->tasks.southS4Completed
        && !game->tasks.southS5Started) {
        game->tasks.southS5Started = true;
        return true;
    }

    if (std::strcmp(locationName, "South Collapse") == 0
        && IsSouthRouteAvailable(&game->tasks)
        && !game->tasks.southS1Started) {
        game->tasks.southS1Started = true;
        return true;
    }

    if (CanArchiveEvidencePass(&game->tasks,
                               locationName,
                               previousLocationName,
                               game->tasks.westW5Started,
                               game->tasks.westW5Completed,
                               "Last Camp")) {
        game->tasks.westW5Completed = true;
        return true;
    }

    if (CanArchiveEvidencePass(&game->tasks,
                               locationName,
                               previousLocationName,
                               game->tasks.westW4Started,
                               game->tasks.westW4Completed,
                               "Echo Basin")) {
        game->tasks.westW4Completed = true;
        return true;
    }

    if (CanArchiveEvidencePass(&game->tasks,
                               locationName,
                               previousLocationName,
                               game->tasks.westW3Started,
                               game->tasks.westW3Completed,
                               "Canopy Hollow")) {
        game->tasks.westW3Completed = true;
        return true;
    }

    if (CanArchiveEvidencePass(&game->tasks,
                               locationName,
                               previousLocationName,
                               game->tasks.westW2Started,
                               game->tasks.westW2Completed,
                               "Survey Break")) {
        game->tasks.westW2Completed = true;
        return true;
    }

    if (CanArchiveEvidencePass(&game->tasks,
                               locationName,
                               previousLocationName,
                               game->tasks.westW1Started,
                               game->tasks.westW1Completed,
                               "West Frontier")) {
        game->tasks.westW1Completed = true;
        return true;
    }

    if (CanArchiveEvidencePass(&game->tasks,
                               locationName,
                               previousLocationName,
                               game->tasks.southS5Started,
                               game->tasks.southS5Completed,
                               "Root Vault")) {
        game->tasks.southS5Completed = true;
        return true;
    }

    if (CanArchiveEvidencePass(&game->tasks,
                               locationName,
                               previousLocationName,
                               game->tasks.southS4Started,
                               game->tasks.southS4Completed,
                               "Purifier Ring")) {
        game->tasks.southS4Completed = true;
        return true;
    }

    if (CanArchiveEvidencePass(&game->tasks,
                               locationName,
                               previousLocationName,
                               game->tasks.southS3Started,
                               game->tasks.southS3Completed,
                               "Service Shafts")) {
        game->tasks.southS3Completed = true;
        return true;
    }

    if (CanArchiveEvidencePass(&game->tasks,
                               locationName,
                               previousLocationName,
                               game->tasks.southS2Started,
                               game->tasks.southS2Completed,
                               "Vent Galleries")) {
        game->tasks.southS2Completed = true;
        return true;
    }

    if (CanArchiveEvidencePass(&game->tasks,
                               locationName,
                               previousLocationName,
                               game->tasks.southS1Started,
                               game->tasks.southS1Completed,
                               "South Collapse")) {
        game->tasks.southS1Completed = true;
        return true;
    }

    return false;
}

void Game_MaybePostNorthRouteTransitionHint(Game *game) {
    const char *locationName;
    const char *previousLocationName;
    bool routeFlagsChanged;
    bool westCompletedNow;
    bool southCompletedNow;
    bool westSecondCompletedNow;
    bool southSecondCompletedNow;
    bool westThirdCompletedNow;
    bool southThirdCompletedNow;
    bool westFourthCompletedNow;
    bool southFourthCompletedNow;
    bool westFifthCompletedNow;
    bool southFifthCompletedNow;
    bool x1ReadyBefore;
    bool x2ReadyBefore;
    bool x3ReadyBefore;
    bool x1ReadyNow;
    bool x2ReadyNow;
    bool x3ReadyNow;
    const bool detailedLocationHintsEnabled = false;

    if (game == NULL) {
        return;
    }

    locationName = Map_GetLocationNameAt(game->player.gridX, game->player.gridY);
    if (locationName == NULL || std::strcmp(locationName, game->lastLocationName) == 0) {
        return;
    }

    previousLocationName = game->lastLocationName;
    x1ReadyBefore = IsCrossX1Ready(&game->tasks);
    x2ReadyBefore = IsCrossX2Ready(&game->tasks);
    x3ReadyBefore = IsCrossX3Ready(&game->tasks);
    westCompletedNow = CanArchiveEvidencePass(&game->tasks,
                                              locationName,
                                              previousLocationName,
                                              game->tasks.westW1Started,
                                              game->tasks.westW1Completed,
                                              "West Frontier");
    southCompletedNow = CanArchiveEvidencePass(&game->tasks,
                                               locationName,
                                               previousLocationName,
                                               game->tasks.southS1Started,
                                               game->tasks.southS1Completed,
                                               "South Collapse");
    westSecondCompletedNow = CanArchiveEvidencePass(&game->tasks,
                                                    locationName,
                                                    previousLocationName,
                                                    game->tasks.westW2Started,
                                                    game->tasks.westW2Completed,
                                                    "Survey Break");
    southSecondCompletedNow = CanArchiveEvidencePass(&game->tasks,
                                                     locationName,
                                                     previousLocationName,
                                                     game->tasks.southS2Started,
                                                     game->tasks.southS2Completed,
                                                     "Vent Galleries");
    westThirdCompletedNow = CanArchiveEvidencePass(&game->tasks,
                                                   locationName,
                                                   previousLocationName,
                                                   game->tasks.westW3Started,
                                                   game->tasks.westW3Completed,
                                                   "Canopy Hollow");
    southThirdCompletedNow = CanArchiveEvidencePass(&game->tasks,
                                                    locationName,
                                                    previousLocationName,
                                                    game->tasks.southS3Started,
                                                    game->tasks.southS3Completed,
                                                    "Service Shafts");
    westFourthCompletedNow = CanArchiveEvidencePass(&game->tasks,
                                                    locationName,
                                                    previousLocationName,
                                                    game->tasks.westW4Started,
                                                    game->tasks.westW4Completed,
                                                    "Echo Basin");
    southFourthCompletedNow = CanArchiveEvidencePass(&game->tasks,
                                                     locationName,
                                                     previousLocationName,
                                                     game->tasks.southS4Started,
                                                     game->tasks.southS4Completed,
                                                     "Purifier Ring");
    westFifthCompletedNow = CanArchiveEvidencePass(&game->tasks,
                                                   locationName,
                                                   previousLocationName,
                                                   game->tasks.westW5Started,
                                                   game->tasks.westW5Completed,
                                                   "Last Camp");
    southFifthCompletedNow = CanArchiveEvidencePass(&game->tasks,
                                                    locationName,
                                                    previousLocationName,
                                                    game->tasks.southS5Started,
                                                    game->tasks.southS5Completed,
                                                    "Root Vault");
    routeFlagsChanged = TryAdvanceWestSouthRouteFlags(game, locationName, previousLocationName);
    x1ReadyNow = IsCrossX1Ready(&game->tasks);
    x2ReadyNow = IsCrossX2Ready(&game->tasks);
    x3ReadyNow = IsCrossX3Ready(&game->tasks);
    if (routeFlagsChanged) {
        Tasks_UpdateObjective(&game->tasks, &game->player);
    }
    std::snprintf(game->lastLocationName, sizeof(game->lastLocationName), "%s", locationName);
    if (westFifthCompletedNow) {
        Game_PostMessage(game, "Base summary: the final west archive has been filed.", 3.4f);
        return;
    }
    if (southFifthCompletedNow) {
        Game_PostMessage(game, "Base summary: the final south archive has been filed.", 3.4f);
        return;
    }
    if (!x3ReadyBefore && x3ReadyNow) {
        Game_PostMessage(game, "Base summary: west and south evidence now supports a fully informed rescue-versus-settlement choice.", 3.4f);
        return;
    }
    if (westFourthCompletedNow) {
        Game_PostMessage(game, "Base summary: Echo Basin findings are archived. The west route now points toward Last Camp.", 3.4f);
        return;
    }
    if (southFourthCompletedNow) {
        Game_PostMessage(game, "Base summary: Purifier Ring controls are archived. The south route now points toward the Root Vault.", 3.4f);
        return;
    }
    if (!x2ReadyBefore && x2ReadyNow) {
        Game_PostMessage(game, "Base summary: Loxi can now redraw the final choice using both investigations.", 3.4f);
        return;
    }
    if (westThirdCompletedNow) {
        Game_PostMessage(game, "Base summary: Canopy Hollow evidence is archived. The west route now points toward Echo Basin.", 3.4f);
        return;
    }
    if (southThirdCompletedNow) {
        Game_PostMessage(game, "Base summary: Service Shaft records are archived. The south route now points toward the Purifier Ring.", 3.4f);
        return;
    }
    if (!x1ReadyBefore && x1ReadyNow) {
        Game_PostMessage(game, "Base summary: west traces and south facility records now align into one investigation.", 3.4f);
        return;
    }
    if (westSecondCompletedNow) {
        Game_PostMessage(game, "Base summary: Survey Break anchors are archived. The west trail now points deeper into Canopy Hollow.", 3.4f);
        return;
    }
    if (southSecondCompletedNow) {
        Game_PostMessage(game, "Base summary: Vent calibration records are archived. The south route now points deeper into the Service Shafts.", 3.4f);
        return;
    }
    if (westCompletedNow) {
        Game_PostMessage(game, "Base summary: the opening west signal fragment is archived.", 3.4f);
        return;
    }
    if (southCompletedNow) {
        Game_PostMessage(game, "Base summary: the South Collapse outage memo is archived.", 3.4f);
        return;
    }

    if (detailedLocationHintsEnabled && game->tasks.stage >= 3 && game->tasks.stage <= 5 && IsTrackedEastLocation(locationName)) {
        if (std::strcmp(locationName, "Outer Swamp Rim") == 0) {
            if (game->tasks.stage == 3) {
                Game_PostMessage(game, "Outer Swamp Rim: this is the first real out-and-back stretch. Repair the relay, learn the oxygen rhythm, then head home before the east stops feeling manageable.", 3.2f);
            } else if (game->tasks.stage == 4) {
                Game_PostMessage(game, "Outer Swamp Rim: east is no longer just relay ground. Use this shelf as the safer lead-in while you finish gun-and-suit prep for the crash clue.", 3.2f);
            } else {
                Game_PostMessage(game, "Outer Swamp Rim: this is now the last easy shelf before the real eastern push. Stabilize Rope, camp, and oxygen support here before you go farther.", 3.2f);
            }
            return;
        }

        if (std::strcmp(locationName, "Flooded Detour") == 0) {
            if (game->tasks.stage == 3) {
                Game_PostMessage(game, "Flooded Detour: you have moved past the relay teaching lane. This route is already about oxygen commitment, not a quick repair errand.", 3.1f);
            } else {
                Game_PostMessage(game, "Flooded Detour: this is where Rope and Field Camp stop being conveniences and become route-planning tools. Do not treat it like a short walk home.", 3.2f);
            }
            return;
        }

        if (std::strcmp(locationName, "Deep Gate") == 0) {
            Game_PostMessage(game, "Deep Gate: this is the last forgiving shelf before the real east commitment. If suit, camp, or oxygen backup looks shaky, reset here instead of gambling on the basin.", 3.3f);
            return;
        }

        if (std::strcmp(locationName, "Deep Basin") == 0) {
            Game_PostMessage(game, "Deep Basin: this is the full east-route commitment. Bring the Energy Core home from here so the power bay can turn eastern survival into a path north.", 3.4f);
            return;
        }
    }

    if (detailedLocationHintsEnabled && IsTrackedBoundaryLocation(locationName)) {
        if (std::strcmp(locationName, "West Frontier") == 0) {
            if (IsWestRouteAvailable(&game->tasks)) {
                Game_PostMessage(game, "West Frontier: this is the first west investigation shelf. Recover the opening dead-signal record here, then move toward Survey Break for the next lead.", 3.3f);
            } else {
                Game_PostMessage(game, "West Frontier: Echo Wilds is still sealed under dead comms. Restore the relay first, then this shelf becomes the first west investigation route.", 3.3f);
            }
        } else if (std::strcmp(locationName, "Survey Break") == 0) {
            if (!IsWestRouteAvailable(&game->tasks)) {
                Game_PostMessage(game, "Survey Break: this middle stretch stays unstable while comms are dark. Restore relay control before working here.", 3.3f);
            } else if (game->tasks.westW3Completed) {
                Game_PostMessage(game, "Survey Break: the Canopy Hollow archive is already filed and the full west chain is complete. This stretch now serves only as repeat support.", 3.3f);
            } else if (game->tasks.westW3Started) {
                Game_PostMessage(game, "Survey Break: the Canopy Hollow follow-up is active. Finish the investigation and return to base to archive the full west chain.", 3.3f);
            } else if (game->tasks.westW2Completed) {
                Game_PostMessage(game, "Survey Break: the anchor pair is archived. Move into Canopy Hollow to start the next lead.", 3.3f);
            } else if (game->tasks.westW2Started) {
                Game_PostMessage(game, "Survey Break: the second survey sweep is active. Finish it here and return to base to archive the west-route extension.", 3.3f);
            } else if (game->tasks.westW1Completed) {
                Game_PostMessage(game, "Survey Break: the opening west archive is filed, so this stretch is now the active follow-up route.", 3.3f);
            } else {
                Game_PostMessage(game, "Survey Break: west midline data is still noisy. Close the West Frontier investigation first, then this route will open cleanly.", 3.3f);
            }
        } else if (std::strcmp(locationName, "Canopy Hollow") == 0) {
            if (!IsWestRouteAvailable(&game->tasks)) {
                Game_PostMessage(game, "Canopy Hollow: this canopy shelf remains comm-gated. Restore relay control before running the next investigation here.", 3.3f);
            } else if (game->tasks.westW3Completed) {
                Game_PostMessage(game, "Canopy Hollow: this lead is archived. The west route now stands as a completed three-leg branch.", 3.3f);
            } else if (game->tasks.westW3Started) {
                Game_PostMessage(game, "Canopy Hollow: this lead is active. Complete it and return to base to archive the full west-route chain.", 3.3f);
            } else if (game->tasks.westW2Completed) {
                Game_PostMessage(game, "Canopy Hollow: Survey Break is archived, so this shelf is now the active next lead.", 3.3f);
            } else {
                Game_PostMessage(game, "Canopy Hollow: this is a third-leg shelf. Close Survey Break first, then this lead opens cleanly.", 3.3f);
            }
        } else {
            if (std::strcmp(locationName, "South Collapse") == 0) {
                if (IsSouthRouteAvailable(&game->tasks)) {
                    Game_PostMessage(game, "South Collapse: this is the first south investigation shelf. Record the opening fracture profile here, then move toward Vent Galleries for the next lead.", 3.3f);
                } else {
                    Game_PostMessage(game, "South Collapse: the Subsurface Sink descent is unstable while base power is offline. Restore the Power Bay first, then this becomes the opening south-route survey lane.", 3.3f);
                }
            } else if (std::strcmp(locationName, "Vent Galleries") == 0) {
                if (!IsSouthRouteAvailable(&game->tasks)) {
                    Game_PostMessage(game, "Vent Galleries: this middle stretch remains sealed while base power is unstable. Restore the Power Bay before working here.", 3.3f);
                } else if (game->tasks.southS3Completed) {
                    Game_PostMessage(game, "Vent Galleries: the Service Shaft archive is filed and the full south chain is complete. This lane now supports repeat validation scans.", 3.3f);
                } else if (game->tasks.southS3Started) {
                    Game_PostMessage(game, "Vent Galleries: the Service Shaft follow-up is active. Complete this investigation and return to base to archive the full south chain.", 3.3f);
                } else if (game->tasks.southS2Completed) {
                    Game_PostMessage(game, "Vent Galleries: the vent restoration is archived. Move into Service Shafts to begin the next lead.", 3.3f);
                } else if (game->tasks.southS2Started) {
                    Game_PostMessage(game, "Vent Galleries: the second south-route investigation is active. Finish it and return to base to archive it cleanly.", 3.3f);
                } else if (game->tasks.southS1Completed) {
                    Game_PostMessage(game, "Vent Galleries: the South Collapse archive is filed, so this stretch is now the active south follow-up route.", 3.3f);
                } else {
                    Game_PostMessage(game, "Vent Galleries: southern vent telemetry is unstable. Close the South Collapse investigation first, then this stretch will open cleanly.", 3.3f);
                }
            } else {
                if (!IsSouthRouteAvailable(&game->tasks)) {
                    Game_PostMessage(game, "Service Shafts: this deeper shelf remains power-gated. Restore the Power Bay before running the next investigation here.", 3.3f);
                } else if (game->tasks.southS3Completed) {
                    Game_PostMessage(game, "Service Shafts: this lead is archived. The south route now stands as a completed three-leg branch.", 3.3f);
                } else if (game->tasks.southS3Started) {
                    Game_PostMessage(game, "Service Shafts: this lead is active. Complete it and return to base to archive the full south-route chain.", 3.3f);
                } else if (game->tasks.southS2Completed) {
                    Game_PostMessage(game, "Service Shafts: Vent Galleries is archived, so this shelf is now the active next lead.", 3.3f);
                } else {
                    Game_PostMessage(game, "Service Shafts: this is a third-leg shelf. Close Vent Galleries first, then the next lead opens cleanly.", 3.3f);
                }
            }
        }
        return;
    }

    if (game->tasks.stage < 6 || !IsTrackedRuinsLocation(locationName)) {
        return;
    }
}
