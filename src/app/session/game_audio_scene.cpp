#include "game_manager_internal.h"

#include <cstring>

static AudioMusicStage GetStageMusicStage(int stage) {
    if (stage <= 1) {
        return AUDIO_MUSIC_STAGE_1_WAKE;
    }

    switch (stage) {
        case 2:
            return AUDIO_MUSIC_STAGE_2_FIRST_STEPS;
        case 3:
            return AUDIO_MUSIC_STAGE_3_WILD;
        case 4:
            return AUDIO_MUSIC_STAGE_4_RISK;
        case 5:
            return AUDIO_MUSIC_STAGE_5_POWER;
        case 6:
            return AUDIO_MUSIC_STAGE_6_RELICS;
        case 7:
        default:
            return AUDIO_MUSIC_STAGE_7_CHOICE;
    }
}

AudioMusicStage Game_SelectMusicStage(const Game *game) {
    MapArea area;
    const char *locationName;

    if (game == NULL) {
        return AUDIO_MUSIC_NONE;
    }

    if (game->flow.state == GAME_STATE_INTRO) {
        return AUDIO_MUSIC_MENU;
    }

    if (game->flow.state == GAME_STATE_OPENING) {
        return GetStageMusicStage(game->runtime.tasks.stage);
    }

    if (game->runtime.tasks.ending != ENDING_NONE || game->flow.state == GAME_STATE_ENDING) {
        return AUDIO_MUSIC_ENDING;
    }

    area = Map_GetAreaAtMap(&game->runtime.map, game->runtime.player.gridX, game->runtime.player.gridY);
    locationName = Map_GetLocationNameAtMap(&game->runtime.map, game->runtime.player.gridX, game->runtime.player.gridY);
    if (area == MAP_AREA_BOSS_ARENA && !game->runtime.tasks.bossDefeated) {
        return AUDIO_MUSIC_BOSS;
    }

    switch (game->runtime.tasks.selectedEndingRoute) {
        case ENDING_HEROIC:
            if (locationName != NULL && std::strcmp(locationName, "Signal Tower Plateau") == 0) {
                return AUDIO_MUSIC_ROUTE_HERO;
            }
            break;
        case ENDING_PEACEFUL:
            if (locationName != NULL && std::strcmp(locationName, "Signal Tower Plateau") == 0) {
                return AUDIO_MUSIC_ROUTE_PEACEFUL;
            }
            break;
        case ENDING_SETTLEMENT:
            if (area == MAP_AREA_BASE) {
                return AUDIO_MUSIC_ROUTE_SETTLEMENT;
            }
            break;
        case ENDING_FAILURE:
        case ENDING_NONE:
        default:
            break;
    }

    return GetStageMusicStage(game->runtime.tasks.stage);
}

AudioScene Game_SelectAudioScene(const Game *game) {
    MapArea area;
    const char *locationName;

    if (game == NULL) {
        return AUDIO_SCENE_NONE;
    }

    if (game->flow.state == GAME_STATE_INTRO || game->flow.state == GAME_STATE_OPENING) {
        return AUDIO_SCENE_MENU;
    }

    if (game->runtime.tasks.ending != ENDING_NONE || game->flow.state == GAME_STATE_ENDING) {
        return AUDIO_SCENE_ENDING;
    }

    area = Map_GetAreaAtMap(&game->runtime.map, game->runtime.player.gridX, game->runtime.player.gridY);
    locationName = Map_GetLocationNameAtMap(&game->runtime.map, game->runtime.player.gridX, game->runtime.player.gridY);

    if (area == MAP_AREA_BOSS_ARENA && !game->runtime.tasks.bossDefeated) {
        return AUDIO_SCENE_BOSS_ARENA;
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
