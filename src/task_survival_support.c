#include "task_runtime_internal.h"

static int AbsIntLocal(int value) {
    return value < 0 ? -value : value;
}

static int DistanceManhattanLocal(int ax, int ay, int bx, int by) {
    return AbsIntLocal(ax - bx) + AbsIntLocal(ay - by);
}

static float GetSuitFilterStrength(const Player *player) {
    return player->hasProtectionSuit ? 0.28f : 0.0f;
}

bool TasksRuntime_IsOuterSwampLowerRoute(const Player *player) {
    return player != NULL && player->gridY >= EXTERIOR_Y(57);
}

bool TasksRuntime_IsDeepSwampCore(const Player *player) {
    return player != NULL && player->gridY >= EXTERIOR_Y(65);
}

bool TasksRuntime_IsTowerPlateau(const Player *player) {
    return player != NULL && player->gridY <= SIGNAL_TOWER_Y + 6;
}

bool TasksRuntime_IsMonolithRing(const Player *player) {
    return player != NULL
        && player->gridY >= EXTERIOR_Y(11)
        && player->gridY < EXTERIOR_Y(21);
}

bool TasksRuntime_IsInSafeRecoveryZone(const GameMap *map, const Player *player, MapArea area) {
    (void)area;

    return map != NULL
        && player != NULL
        && map->campPlaced
        && DistanceManhattanLocal(player->gridX, player->gridY, map->campX, map->campY) <= 1;
}

float TasksRuntime_GetRuinsPreparationFactor(const TaskSystem *tasks, const Player *player) {
    float factor;

    factor = 1.0f;
    if (tasks != NULL && tasks->stage >= 7) {
        factor -= 0.06f * (float)tasks->monolithsLit;
    }
    if (player != NULL
        && player->hasSignalAmplifier
        && (TasksRuntime_IsTowerPlateau(player) || TasksRuntime_IsMonolithRing(player))) {
        factor -= 0.22f;
    }
    if (factor < 0.58f) {
        factor = 0.58f;
    }
    return factor;
}

float TasksRuntime_GetOxygenLeakRate(const TaskSystem *tasks) {
    float rate;
    int pressureCycles;

    rate = 15.0f / FULL_CYCLE_SECONDS;
    if (tasks->oxygenRepairLevel >= 1) {
        rate *= 0.70f;
    }
    if (tasks->oxygenRepairLevel >= 2) {
        rate = 0.0f;
    }

    pressureCycles = tasks->dayCount / 3;
    rate *= 1.0f + 0.08f * pressureCycles;

    if (tasks->currentEvent == EVENT_DEVICE_FAULT) {
        rate *= 1.35f;
    }

    return rate;
}

float TasksRuntime_GetAreaOxygenDrain(MapArea area, const Player *player) {
    const PlayerStatusEffect *filteredEffect;
    float mitigation;
    float rate;

    switch (area) {
        case MAP_AREA_BASE:
            rate = 0.0f;
            break;
        case MAP_AREA_FOREST:
            rate = 0.18f;
            break;
        case MAP_AREA_SWAMP_OUTER:
            rate = TasksRuntime_IsOuterSwampLowerRoute(player) ? 0.56f : 0.42f;
            break;
        case MAP_AREA_SWAMP_DEEP:
            rate = TasksRuntime_IsDeepSwampCore(player) ? 1.08f : 0.88f;
            break;
        case MAP_AREA_RUINS:
            if (TasksRuntime_IsTowerPlateau(player)) {
                rate = 0.92f;
            } else if (TasksRuntime_IsMonolithRing(player)) {
                rate = 0.74f;
            } else {
                rate = 0.58f;
            }
            break;
        case MAP_AREA_BOSS_ARENA:
            rate = 0.54f;
            break;
        case MAP_AREA_UNKNOWN:
        default:
            rate = 0.25f;
            break;
    }

    mitigation = GetSuitFilterStrength(player);
    filteredEffect = Player_GetStatusEffect(player, PLAYER_STATUS_FILTERED);
    if (filteredEffect != NULL && filteredEffect->active && filteredEffect->magnitude > mitigation) {
        mitigation = filteredEffect->magnitude;
    }
    if (mitigation > 0.65f) {
        mitigation = 0.65f;
    }
    if (mitigation > 0.0f) {
        rate *= 1.0f - mitigation;
    }

    return rate;
}

float TasksRuntime_GetPlayerFilterStrength(const Player *player) {
    const PlayerStatusEffect *filteredEffect;
    float filterStrength;

    filterStrength = GetSuitFilterStrength(player);
    filteredEffect = Player_GetStatusEffect(player, PLAYER_STATUS_FILTERED);
    if (filteredEffect != NULL && filteredEffect->active && filteredEffect->magnitude > filterStrength) {
        filterStrength = filteredEffect->magnitude;
    }

    if (filterStrength > 0.65f) {
        filterStrength = 0.65f;
    }
    return filterStrength;
}

int TasksRuntime_GetPoisonStatusLevel(float poison) {
    if (poison >= 70.0f) {
        return 3;
    }
    if (poison >= 35.0f) {
        return 2;
    }
    if (poison > 0.0f) {
        return 1;
    }
    return 0;
}

void TasksRuntime_GetLowOxygenThresholds(MapArea area,
                                         const Player *player,
                                         float *lowOxygenThreshold,
                                         float *criticalOxygenThreshold) {
    *lowOxygenThreshold = 30.0f;
    *criticalOxygenThreshold = 10.0f;

    if (area == MAP_AREA_SWAMP_OUTER) {
        *lowOxygenThreshold = TasksRuntime_IsOuterSwampLowerRoute(player) ? 38.0f : 33.0f;
        *criticalOxygenThreshold = TasksRuntime_IsOuterSwampLowerRoute(player) ? 14.0f : 11.0f;
    } else if (area == MAP_AREA_SWAMP_DEEP) {
        *lowOxygenThreshold = TasksRuntime_IsDeepSwampCore(player) ? 40.0f : 37.0f;
        *criticalOxygenThreshold = TasksRuntime_IsDeepSwampCore(player) ? 15.0f : 13.0f;
    } else if (area == MAP_AREA_RUINS) {
        if (TasksRuntime_IsTowerPlateau(player)) {
            *lowOxygenThreshold = 42.0f;
            *criticalOxygenThreshold = 16.0f;
        } else if (TasksRuntime_IsMonolithRing(player)) {
            *lowOxygenThreshold = 39.0f;
            *criticalOxygenThreshold = 14.0f;
        } else {
            *lowOxygenThreshold = 36.0f;
            *criticalOxygenThreshold = 13.0f;
        }
    } else if (area == MAP_AREA_BOSS_ARENA) {
        *lowOxygenThreshold = 35.0f;
        *criticalOxygenThreshold = 13.0f;
    }
}

void TasksRuntime_ApplyOxygenLeakStatus(Player *player, int level, float duration, float magnitude) {
    if (player == NULL || magnitude <= 0.0f || duration <= 0.0f) {
        return;
    }

    Player_SetStatus(player, PLAYER_STATUS_OXYGEN_LEAK, level, duration, magnitude);
}
