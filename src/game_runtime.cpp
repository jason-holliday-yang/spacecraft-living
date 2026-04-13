#include "game_manager_internal.h"

#include "task_runtime_internal.h"

#include <cstdio>
#include <cstring>

void Game_PostMessage(Game *game, const char *text, float duration) {
    if (game == NULL || text == NULL) {
        return;
    }

    TasksRuntime_SanitizeDisplayText(text, game->hudMessage.text, sizeof(game->hudMessage.text));
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
}

AudioScene Game_SelectAudioScene(const Game *game) {
    MapArea area;

    if (game == NULL) {
        return AUDIO_SCENE_NONE;
    }

    if (game->tasks.ending != ENDING_NONE) {
        return AUDIO_SCENE_ENDING;
    }

    area = Map_GetAreaAt(game->player.gridX, game->player.gridY);
    if (!game->tasks.bossDefeated && area == MAP_AREA_RUINS && game->tasks.stage >= 7) {
        return AUDIO_SCENE_BOSS;
    }

    switch (area) {
        case MAP_AREA_BASE:
            return AUDIO_SCENE_BASE;
        case MAP_AREA_FOREST:
            return AUDIO_SCENE_FOREST;
        case MAP_AREA_SWAMP_OUTER:
        case MAP_AREA_SWAMP_DEEP:
            return AUDIO_SCENE_SWAMP;
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
        && (std::strcmp(locationName, "Command Deck") == 0
            || std::strcmp(locationName, "Workshop") == 0
            || std::strcmp(locationName, "Terminal Bay") == 0
            || std::strcmp(locationName, "Life Support Bay") == 0
            || std::strcmp(locationName, "Cargo Hold") == 0
            || std::strcmp(locationName, "Recovery Zone") == 0
            || std::strcmp(locationName, "Airlock Passage") == 0
            || std::strcmp(locationName, "Power Bay") == 0);
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

    if (IsShipBaseLocation(locationName)
        && previousLocationName != NULL
        && game->tasks.westW5Started
        && !game->tasks.westW5Completed
        && std::strcmp(previousLocationName, "Last Camp") == 0) {
        game->tasks.westW5Completed = true;
        return true;
    }

    if (IsShipBaseLocation(locationName)
        && previousLocationName != NULL
        && game->tasks.westW4Started
        && !game->tasks.westW4Completed
        && std::strcmp(previousLocationName, "Echo Basin") == 0) {
        game->tasks.westW4Completed = true;
        return true;
    }

    if (IsShipBaseLocation(locationName)
        && previousLocationName != NULL
        && game->tasks.westW3Started
        && !game->tasks.westW3Completed
        && std::strcmp(previousLocationName, "Canopy Hollow") == 0) {
        game->tasks.westW3Completed = true;
        return true;
    }

    if (IsShipBaseLocation(locationName)
        && previousLocationName != NULL
        && game->tasks.westW2Started
        && !game->tasks.westW2Completed
        && std::strcmp(previousLocationName, "Survey Break") == 0) {
        game->tasks.westW2Completed = true;
        return true;
    }

    if (IsShipBaseLocation(locationName)
        && previousLocationName != NULL
        && game->tasks.westW1Started
        && !game->tasks.westW1Completed
        && std::strcmp(previousLocationName, "West Frontier") == 0) {
        game->tasks.westW1Completed = true;
        return true;
    }

    if (IsShipBaseLocation(locationName)
        && previousLocationName != NULL
        && game->tasks.southS5Started
        && !game->tasks.southS5Completed
        && std::strcmp(previousLocationName, "Root Vault") == 0) {
        game->tasks.southS5Completed = true;
        return true;
    }

    if (IsShipBaseLocation(locationName)
        && previousLocationName != NULL
        && game->tasks.southS4Started
        && !game->tasks.southS4Completed
        && std::strcmp(previousLocationName, "Purifier Ring") == 0) {
        game->tasks.southS4Completed = true;
        return true;
    }

    if (IsShipBaseLocation(locationName)
        && previousLocationName != NULL
        && game->tasks.southS3Started
        && !game->tasks.southS3Completed
        && std::strcmp(previousLocationName, "Service Shafts") == 0) {
        game->tasks.southS3Completed = true;
        return true;
    }

    if (IsShipBaseLocation(locationName)
        && previousLocationName != NULL
        && game->tasks.southS2Started
        && !game->tasks.southS2Completed
        && std::strcmp(previousLocationName, "Vent Galleries") == 0) {
        game->tasks.southS2Completed = true;
        return true;
    }

    if (IsShipBaseLocation(locationName)
        && previousLocationName != NULL
        && game->tasks.southS1Started
        && !game->tasks.southS1Completed
        && std::strcmp(previousLocationName, "South Collapse") == 0) {
        game->tasks.southS1Completed = true;
        return true;
    }

    return false;
}

static const char *GetBaseReturnSummary(const Game *game, const char *previousLocationName) {
    if (game == NULL || previousLocationName == NULL || Map_GetAreaAt(game->player.gridX, game->player.gridY) != MAP_AREA_BASE) {
        return NULL;
    }

    if (IsTrackedEastLocation(previousLocationName)) {
        if (game->tasks.stage == 3) {
            return "Base summary: east relay run complete. Debrief at Loxi.";
        }
        if (game->tasks.stage == 5 && game->player.resources[RESOURCE_ENERGY_CORE] > 0) {
            return "Base summary: Energy Core returned. Install it in Power Bay.";
        }
        if (game->tasks.stage == 5) {
            return "Base summary: prepare gear before next east deep run.";
        }
        if (game->tasks.stage >= 6) {
            return "Base summary: east complete. Recover and pivot north.";
        }
        return "Base summary: east run complete. Recover and continue.";
    }

    if (IsTrackedRuinsLocation(previousLocationName)) {
        if (game->tasks.stage == 6 && game->player.resources[RESOURCE_RELIC_FRAGMENT] >= 3) {
            return "Base summary: fragment set complete. Sync with Loxi.";
        }
        if (game->tasks.stage == 6) {
            return "Base summary: continue north fragment runs after recovery.";
        }
        if (game->tasks.stage == 7 && game->tasks.bossDefeated) {
            return "Base summary: boss defeated. Choose rescue or settlement.";
        }
        if (game->tasks.stage == 7 && game->player.hasSignalAmplifier) {
            return "Base summary: peaceful route available. Choose rescue or settlement.";
        }
        if (game->tasks.stage == 7 && game->tasks.monolithsLit >= 3) {
            return "Base summary: monolith ring complete. Prepare for final push.";
        }
        if (game->tasks.stage == 7 && game->tasks.monolithsLit > 0) {
            return "Base summary: monolith progress made. Recover before next push.";
        }
        return "Base summary: north is the active endgame route.";
    }

    return NULL;
}

void Game_MaybePostNorthRouteTransitionHint(Game *game) {
    const char *locationName;
    const char *previousLocationName;
    const char *baseReturnSummary;
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
    westCompletedNow = IsShipBaseLocation(locationName)
        && game->tasks.westW1Started
        && !game->tasks.westW1Completed
        && previousLocationName != NULL
        && std::strcmp(previousLocationName, "West Frontier") == 0;
    southCompletedNow = IsShipBaseLocation(locationName)
        && game->tasks.southS1Started
        && !game->tasks.southS1Completed
        && previousLocationName != NULL
        && std::strcmp(previousLocationName, "South Collapse") == 0;
    westSecondCompletedNow = IsShipBaseLocation(locationName)
        && game->tasks.westW2Started
        && !game->tasks.westW2Completed
        && previousLocationName != NULL
        && std::strcmp(previousLocationName, "Survey Break") == 0;
    southSecondCompletedNow = IsShipBaseLocation(locationName)
        && game->tasks.southS2Started
        && !game->tasks.southS2Completed
        && previousLocationName != NULL
        && std::strcmp(previousLocationName, "Vent Galleries") == 0;
    westThirdCompletedNow = IsShipBaseLocation(locationName)
        && game->tasks.westW3Started
        && !game->tasks.westW3Completed
        && previousLocationName != NULL
        && std::strcmp(previousLocationName, "Canopy Hollow") == 0;
    southThirdCompletedNow = IsShipBaseLocation(locationName)
        && game->tasks.southS3Started
        && !game->tasks.southS3Completed
        && previousLocationName != NULL
        && std::strcmp(previousLocationName, "Service Shafts") == 0;
    westFourthCompletedNow = IsShipBaseLocation(locationName)
        && game->tasks.westW4Started
        && !game->tasks.westW4Completed
        && previousLocationName != NULL
        && std::strcmp(previousLocationName, "Echo Basin") == 0;
    southFourthCompletedNow = IsShipBaseLocation(locationName)
        && game->tasks.southS4Started
        && !game->tasks.southS4Completed
        && previousLocationName != NULL
        && std::strcmp(previousLocationName, "Purifier Ring") == 0;
    westFifthCompletedNow = IsShipBaseLocation(locationName)
        && game->tasks.westW5Started
        && !game->tasks.westW5Completed
        && previousLocationName != NULL
        && std::strcmp(previousLocationName, "Last Camp") == 0;
    southFifthCompletedNow = IsShipBaseLocation(locationName)
        && game->tasks.southS5Started
        && !game->tasks.southS5Completed
        && previousLocationName != NULL
        && std::strcmp(previousLocationName, "Root Vault") == 0;
    routeFlagsChanged = TryAdvanceWestSouthRouteFlags(game, locationName, previousLocationName);
    x1ReadyNow = IsCrossX1Ready(&game->tasks);
    x2ReadyNow = IsCrossX2Ready(&game->tasks);
    x3ReadyNow = IsCrossX3Ready(&game->tasks);
    if (routeFlagsChanged) {
        Tasks_UpdateObjective(&game->tasks, &game->player);
    }
    baseReturnSummary = GetBaseReturnSummary(game, previousLocationName);
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
        Game_PostMessage(game, "Base summary: Loxi can now rewrite the final route comparison using both investigations.", 3.4f);
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
    if (baseReturnSummary != NULL) {
        Game_PostMessage(game, baseReturnSummary, 3.4f);
        return;
    }

    if (detailedLocationHintsEnabled && game->tasks.stage >= 3 && game->tasks.stage <= 5 && IsTrackedEastLocation(locationName)) {
        if (std::strcmp(locationName, "Outer Swamp Rim") == 0) {
            if (game->tasks.stage == 3) {
                Game_PostMessage(game, "Outer Swamp Rim: this is the first real out-and-back lane. Repair the relay, learn the oxygen line, then head home before east stops being a controlled lesson.", 3.2f);
            } else if (game->tasks.stage == 4) {
                Game_PostMessage(game, "Outer Swamp Rim: east is no longer just relay ground. Use this shelf as the safer lead-in while you finish gun-and-suit prep for the crash clue.", 3.2f);
            } else {
                Game_PostMessage(game, "Outer Swamp Rim: this is now the last easy shelf before the qualification run. Stabilize Rope, camp, and oxygen support here before you push farther east.", 3.2f);
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
            Game_PostMessage(game, "Deep Basin: this is the full east-route commitment. Bring the Energy Core home from here so the power bay can turn east-route survival into the north-route unlock.", 3.4f);
            return;
        }
    }

    if (detailedLocationHintsEnabled && IsTrackedBoundaryLocation(locationName)) {
        if (std::strcmp(locationName, "West Frontier") == 0) {
            if (IsWestRouteAvailable(&game->tasks)) {
                Game_PostMessage(game, "West Frontier: this is the first west investigation shelf. Capture the opening dead-signal pass here, then move toward Survey Break for the second calibration leg.", 3.3f);
            } else {
                Game_PostMessage(game, "West Frontier: Echo Wilds survey ground is still sealed under comm blackout. Restore the relay first, then this shelf becomes the first west-route investigation lane.", 3.3f);
            }
        } else if (std::strcmp(locationName, "Survey Break") == 0) {
            if (!IsWestRouteAvailable(&game->tasks)) {
                Game_PostMessage(game, "Survey Break: this midline remains unstable under comm blackout. Restore relay control before running west-side calibration here.", 3.3f);
            } else if (game->tasks.westW3Completed) {
                Game_PostMessage(game, "Survey Break: the Canopy Hollow archive is already filed and the full west chain is complete. This lane is now repeat-calibration support only.", 3.3f);
            } else if (game->tasks.westW3Started) {
                Game_PostMessage(game, "Survey Break: the Canopy Hollow follow-up is active. Finish the pass and return to base to archive the full west chain.", 3.3f);
            } else if (game->tasks.westW2Completed) {
                Game_PostMessage(game, "Survey Break: the calibration pair is archived. Move into Canopy Hollow to start the next closure lane.", 3.3f);
            } else if (game->tasks.westW2Started) {
                Game_PostMessage(game, "Survey Break: the second calibration sweep is active. Finish it here and return to base to archive the west-route extension.", 3.3f);
            } else if (game->tasks.westW1Completed) {
                Game_PostMessage(game, "Survey Break: the opening west archive is filed, so this lane is now the active calibration route.", 3.3f);
            } else {
                Game_PostMessage(game, "Survey Break: west midline data is noisy. Close the West Frontier pass first, then this route will open cleanly.", 3.3f);
            }
        } else if (std::strcmp(locationName, "Canopy Hollow") == 0) {
            if (!IsWestRouteAvailable(&game->tasks)) {
                Game_PostMessage(game, "Canopy Hollow: this canopy shelf remains comm-gated. Restore relay control before running the closure pass here.", 3.3f);
            } else if (game->tasks.westW3Completed) {
                Game_PostMessage(game, "Canopy Hollow: the closure pass is archived. The west route now stands as a completed three-leg branch.", 3.3f);
            } else if (game->tasks.westW3Started) {
                Game_PostMessage(game, "Canopy Hollow: the closure pass is active. Complete it and return to base to archive the full west-route chain.", 3.3f);
            } else if (game->tasks.westW2Completed) {
                Game_PostMessage(game, "Canopy Hollow: Survey Break is archived, so this shelf is now the active closure lane.", 3.3f);
            } else {
                Game_PostMessage(game, "Canopy Hollow: this is a third-leg shelf. Close Survey Break first, then this closure lane opens cleanly.", 3.3f);
            }
        } else {
            if (std::strcmp(locationName, "South Collapse") == 0) {
                if (IsSouthRouteAvailable(&game->tasks)) {
                    Game_PostMessage(game, "South Collapse: this is the first south investigation shelf. Log the opening fracture profile here, then move toward Vent Galleries for the second calibration leg.", 3.3f);
                } else {
                    Game_PostMessage(game, "South Collapse: the Subsurface Sink descent is unstable while base power is offline. Restore the Power Bay first, then this becomes the opening south-route survey lane.", 3.3f);
                }
            } else if (std::strcmp(locationName, "Vent Galleries") == 0) {
                if (!IsSouthRouteAvailable(&game->tasks)) {
                    Game_PostMessage(game, "Vent Galleries: this midline remains sealed while base power is unstable. Restore the Power Bay before running vent calibration here.", 3.3f);
                } else if (game->tasks.southS3Completed) {
                    Game_PostMessage(game, "Vent Galleries: the Service Shaft archive is filed and the full south chain is complete. This lane now supports repeat validation scans.", 3.3f);
                } else if (game->tasks.southS3Started) {
                    Game_PostMessage(game, "Vent Galleries: the Service Shaft closure is active. Complete this pass and return to base to archive the full south chain.", 3.3f);
                } else if (game->tasks.southS2Completed) {
                    Game_PostMessage(game, "Vent Galleries: the vent calibration is archived. Move into Service Shafts to begin the next closure pass.", 3.3f);
                } else if (game->tasks.southS2Started) {
                    Game_PostMessage(game, "Vent Galleries: the second south-route pass is active. Finish it and return to base to archive it cleanly.", 3.3f);
                } else if (game->tasks.southS1Completed) {
                    Game_PostMessage(game, "Vent Galleries: the South Collapse archive is filed, so this lane is now the active vent-calibration route.", 3.3f);
                } else {
                    Game_PostMessage(game, "Vent Galleries: southern vent telemetry is unstable. Close the South Collapse pass first, then this lane will open cleanly.", 3.3f);
                }
            } else {
                if (!IsSouthRouteAvailable(&game->tasks)) {
                    Game_PostMessage(game, "Service Shafts: this deeper shelf remains power-gated. Restore the Power Bay before running the closure pass here.", 3.3f);
                } else if (game->tasks.southS3Completed) {
                    Game_PostMessage(game, "Service Shafts: the closure pass is archived. The south route now stands as a completed three-leg branch.", 3.3f);
                } else if (game->tasks.southS3Started) {
                    Game_PostMessage(game, "Service Shafts: the closure pass is active. Complete it and return to base to archive the full south-route chain.", 3.3f);
                } else if (game->tasks.southS2Completed) {
                    Game_PostMessage(game, "Service Shafts: Vent Galleries is archived, so this shelf is now the active closure lane.", 3.3f);
                } else {
                    Game_PostMessage(game, "Service Shafts: this is a third-leg shelf. Close Vent Galleries first, then the closure lane opens cleanly.", 3.3f);
                }
            }
        }
        return;
    }

    if (game->tasks.stage < 6 || !IsTrackedRuinsLocation(locationName)) {
        return;
    }

    if (std::strcmp(locationName, "Ruins Approach") == 0) {
        if (game->tasks.stage >= 7) {
            Game_PostMessage(game, "Ruins Approach: this is still the last low-commitment prep lane. Stabilize oxygen, suit, and fallback before you push farther north.", 3.2f);
        } else {
            Game_PostMessage(game, "Ruins Approach: gather fragments around the ring, then fall back to Loxi before the final route opens.", 3.0f);
        }
        return;
    }

    if (std::strcmp(locationName, "Monolith Ring") == 0) {
        if (game->tasks.monolithsLit >= 3) {
            Game_PostMessage(game, "Monolith Ring: the full resonance is active. This is your cleanest transition point from prep into the final north execution.", 3.2f);
        } else if (game->tasks.monolithsLit == 2) {
            Game_PostMessage(game, "Monolith Ring: only one silent stone remains. Finish it now if you want the cleanest heroic push before the plateau.", 3.2f);
        } else if (game->tasks.monolithsLit == 1) {
            Game_PostMessage(game, "Monolith Ring: the first stone is helping, but the route still wants one more real prep loop before the plateau commit.", 3.0f);
        } else {
            Game_PostMessage(game, "Monolith Ring: this is the prep-to-execution hinge. Light the stones here before treating the plateau like a real final climb.", 3.0f);
        }
        return;
    }

    if (std::strcmp(locationName, "Signal Tower Plateau") == 0) {
        if (game->tasks.bossDefeated) {
            Game_PostMessage(game, "Signal Tower Plateau: the guardian is down. Finish the rescue beacon now, or fall back and return to base for settlement.", 3.4f);
        } else if (game->player.hasSignalAmplifier) {
            Game_PostMessage(game, "Signal Tower Plateau: peaceful route is live. The amplifier is calming the tower systems, but this is still the final oxygen commit.", 3.4f);
        } else if (game->tasks.monolithsLit >= 3) {
            Game_PostMessage(game, "Signal Tower Plateau: the full ring is lit. This is your cleanest heroic window, but the final climb is still a hard commitment.", 3.4f);
        } else if (game->tasks.monolithsLit > 0) {
            Game_PostMessage(game, "Signal Tower Plateau: partial ring prep is helping, but you are entering the final climb early. Retreat if oxygen is not stable.", 3.4f);
        } else {
            Game_PostMessage(game, "Signal Tower Plateau: this is the final climb. Expect heavy oxygen leaks and no cheap recovery beyond this point.", 3.4f);
        }
    }
}
