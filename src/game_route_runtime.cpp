#include "game_manager_internal.h"

#include <cstdio>
#include <cstring>

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
            || std::strcmp(locationName, "Cargo Hold") == 0
            || std::strcmp(locationName, "Crew Quarters") == 0
            || std::strcmp(locationName, "Diagnostics") == 0
            || std::strcmp(locationName, "Terminal Bay") == 0
            || std::strcmp(locationName, "Life Support") == 0
            || std::strcmp(locationName, "Workshop") == 0
            || std::strcmp(locationName, "Power Bay") == 0
            || std::strcmp(locationName, "Airlock Link") == 0);
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
        Game_PostMessage(game, Loc_PickLiteral("Base summary: the final west archive has been filed.", "基地简报：西线最终档案已归档。"), 3.4f);
        return;
    }
    if (southFifthCompletedNow) {
        Game_PostMessage(game, Loc_PickLiteral("Base summary: the final south archive has been filed.", "基地简报：南线最终档案已归档。"), 3.4f);
        return;
    }
    if (!x3ReadyBefore && x3ReadyNow) {
        Game_PostMessage(game, Loc_PickLiteral("Base summary: west and south evidence now supports a fully informed rescue-versus-settlement choice.", "基地简报：西线与南线证据现已足够支撑一次完整知情的“救援还是定居”抉择。"), 3.4f);
        return;
    }
    if (westFourthCompletedNow) {
        Game_PostMessage(game, Loc_PickLiteral("Base summary: Echo Basin findings are archived. The west route now points toward Last Camp.", "基地简报：回声盆地记录已归档。西线路线现已继续指向最后营地。"), 3.4f);
        return;
    }
    if (southFourthCompletedNow) {
        Game_PostMessage(game, Loc_PickLiteral("Base summary: Purifier Ring controls are archived. The south route now points toward the Root Vault.", "基地简报：净化环控制记录已归档。南线路线现已继续指向根脉核心。"), 3.4f);
        return;
    }
    if (!x2ReadyBefore && x2ReadyNow) {
        Game_PostMessage(game, Loc_PickLiteral("Base summary: Loxi can now redraw the final choice using both investigations.", "基地简报：洛希现在可以结合两条调查线重新整理最终抉择。"), 3.4f);
        return;
    }
    if (westThirdCompletedNow) {
        Game_PostMessage(game, Loc_PickLiteral("Base summary: Canopy Hollow evidence is archived. The west route now points toward Echo Basin.", "基地简报：林冠洼地记录已归档。西线路线现已继续指向回声盆地。"), 3.4f);
        return;
    }
    if (southThirdCompletedNow) {
        Game_PostMessage(game, Loc_PickLiteral("Base summary: Service Shaft records are archived. The south route now points toward the Purifier Ring.", "基地简报：维护井道记录已归档。南线路线现已继续指向净化环区。"), 3.4f);
        return;
    }
    if (!x1ReadyBefore && x1ReadyNow) {
        Game_PostMessage(game, Loc_PickLiteral("Base summary: west traces and south facility records now align into one investigation.", "基地简报：西线痕迹与南线设施记录现已汇合成同一条调查线。"), 3.4f);
        return;
    }
    if (westSecondCompletedNow) {
        Game_PostMessage(game, Loc_PickLiteral("Base summary: Survey Break anchors are archived. The west trail now points deeper into Canopy Hollow.", "基地简报：勘测断点锚点已归档。西线路线现已继续深入林冠洼地。"), 3.4f);
        return;
    }
    if (southSecondCompletedNow) {
        Game_PostMessage(game, Loc_PickLiteral("Base summary: Vent calibration records are archived. The south route now points deeper into the Service Shafts.", "基地简报：通风校准记录已归档。南线路线现已继续深入维护井道。"), 3.4f);
        return;
    }
    if (westCompletedNow) {
        Game_PostMessage(game, Loc_PickLiteral("Base summary: the opening west signal fragment is archived.", "基地简报：西线开端信号碎片已归档。"), 3.4f);
        return;
    }
    if (southCompletedNow) {
        Game_PostMessage(game, Loc_PickLiteral("Base summary: the South Collapse outage memo is archived.", "基地简报：南部塌陷区停摆备忘已归档。"), 3.4f);
        return;
    }
}
