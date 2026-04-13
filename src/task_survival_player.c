#include "task_runtime_internal.h"

#include "task_content.h"

#include <math.h>

static void SetPlayerStatusActive(Player *player, PlayerStatusType status, bool active, int level, float magnitude) {
    if (active) {
        Player_SetStatus(player, status, level, 0.0f, magnitude);
        return;
    }

    Player_ClearStatus(player, status);
}

void TasksRuntime_UpdatePlayerSurvival(TaskSystem *tasks, GameMap *map, Player *player, float deltaTime) {
    const PlayerStatusEffect *filteredEffect;
    const PlayerStatusEffect *oxygenLeakEffect;
    const PlayerStatusEffect *oxygenReserveEffect;
    const PlayerStatusEffect *campRecoveryEffect;
    MapArea area;
    HazardType hazard;
    float totalOxygenDrain;
    float safeRecoveryAmount;
    float poisonHealthRate;
    float poisonOxygenRate;
    float filterStrength;
    float oxygenReserveStrength;
    float campRecoveryStrength;
    float oxygenLeakStrength;
    float hazardDrain;
    float reserveReduction;
    float poisonMitigation;
    float ruinsPreparationFactor;
    float lowOxygenThreshold;
    float criticalOxygenThreshold;
    bool inSafeRecoveryZone;
    bool filteredActive;
    bool oxygenLeakActive;
    bool oxygenReserveActive;
    bool campRecoveryActive;
    int poisonLevel;
    int lowOxygenLevel;
    int criticalLevel;
    int filteredLevel;
    int oxygenLeakLevel;
    int oxygenReserveLevel;

    player->glowStickTimer = fmaxf(0.0f, player->glowStickTimer - deltaTime);
    player->speedBoostTimer = fmaxf(0.0f, player->speedBoostTimer - deltaTime);
    player->blurPulse += deltaTime;
    Player_UpdateStatuses(player, deltaTime);

    filteredEffect = Player_GetStatusEffect(player, PLAYER_STATUS_FILTERED);
    oxygenLeakEffect = Player_GetStatusEffect(player, PLAYER_STATUS_OXYGEN_LEAK);
    oxygenReserveEffect = Player_GetStatusEffect(player, PLAYER_STATUS_OXYGEN_RESERVE);
    campRecoveryEffect = Player_GetStatusEffect(player, PLAYER_STATUS_CAMP_RECOVERY);
    filteredActive = filteredEffect != NULL && filteredEffect->active;
    oxygenLeakActive = oxygenLeakEffect != NULL && oxygenLeakEffect->active;
    oxygenReserveActive = oxygenReserveEffect != NULL && oxygenReserveEffect->active;
    campRecoveryActive = campRecoveryEffect != NULL && campRecoveryEffect->active;
    filterStrength = TasksRuntime_GetPlayerFilterStrength(player);
    filteredLevel = player->hasProtectionSuit ? 2 : 0;
    if (filteredActive) {
        filterStrength = fmaxf(filterStrength, filteredEffect->magnitude);
        filteredLevel = filteredEffect->level > filteredLevel ? filteredEffect->level : filteredLevel;
    }
    filterStrength = fminf(filterStrength, 0.65f);
    oxygenLeakStrength = oxygenLeakActive ? oxygenLeakEffect->magnitude : 0.0f;
    oxygenLeakLevel = oxygenLeakActive ? oxygenLeakEffect->level : 0;
    oxygenReserveStrength = oxygenReserveActive ? oxygenReserveEffect->magnitude : 0.0f;
    oxygenReserveLevel = oxygenReserveActive ? oxygenReserveEffect->level : 0;
    campRecoveryStrength = campRecoveryActive ? campRecoveryEffect->magnitude : 0.0f;

    area = Map_GetAreaAt(player->gridX, player->gridY);
    hazard = Map_GetHazardAt(map, player->gridX, player->gridY);
    ruinsPreparationFactor = 1.0f;
    if (area == MAP_AREA_RUINS) {
        ruinsPreparationFactor = TasksRuntime_GetRuinsPreparationFactor(tasks, player);
    }
    if (area == MAP_AREA_RUINS) {
        float leakMagnitude;
        float leakDuration;
        int leakLevel;

        if (TasksRuntime_IsTowerPlateau(player)) {
            leakLevel = tasks->stage >= 7 ? 3 : 2;
            leakDuration = tasks->stage >= 7 ? 6.2f : 4.8f;
            leakMagnitude = tasks->stage >= 7 ? 0.86f : 0.44f;
        } else if (TasksRuntime_IsMonolithRing(player)) {
            leakLevel = tasks->stage >= 7 ? 2 : 1;
            leakDuration = tasks->stage >= 7 ? 5.4f : 4.0f;
            leakMagnitude = tasks->stage >= 7 ? 0.72f : 0.36f;
        } else {
            leakLevel = tasks->stage >= 7 ? 2 : 1;
            leakDuration = tasks->stage >= 7 ? 4.8f : 3.5f;
            leakMagnitude = tasks->stage >= 7 ? 0.60f : 0.28f;
        }
        if (player->hasSignalAmplifier && TasksRuntime_IsTowerPlateau(player)) {
            if (leakLevel > 1) {
                leakLevel -= 1;
            }
            leakDuration *= 0.82f;
            leakMagnitude *= 0.68f;
        }
        TasksRuntime_ApplyOxygenLeakStatus(player,
                                           leakLevel,
                                           leakDuration,
                                           leakMagnitude * ruinsPreparationFactor * (1.0f - filterStrength * 0.75f));
        oxygenLeakEffect = Player_GetStatusEffect(player, PLAYER_STATUS_OXYGEN_LEAK);
        oxygenLeakActive = oxygenLeakEffect != NULL && oxygenLeakEffect->active;
        oxygenLeakStrength = oxygenLeakActive ? oxygenLeakEffect->magnitude : 0.0f;
        oxygenLeakLevel = oxygenLeakActive ? oxygenLeakEffect->level : 0;
    }
    totalOxygenDrain = TasksRuntime_GetOxygenLeakRate(tasks)
        + TasksRuntime_GetAreaOxygenDrain(area, player)
        + oxygenLeakStrength;
    if (area == MAP_AREA_RUINS) {
        totalOxygenDrain *= ruinsPreparationFactor;
    }

    if (area == MAP_AREA_FOREST && tasks->phase == DAY_PHASE_NIGHT && player->glowStickTimer <= 0.0f) {
        totalOxygenDrain += 0.10f;
    } else if (area == MAP_AREA_SWAMP_OUTER && tasks->phase != DAY_PHASE_DAY) {
        totalOxygenDrain += TasksRuntime_IsOuterSwampLowerRoute(player) ? 0.14f : 0.08f;
    } else if (area == MAP_AREA_RUINS && tasks->stage >= 7) {
        totalOxygenDrain += (TasksRuntime_IsTowerPlateau(player)
                                ? 0.18f
                                : (TasksRuntime_IsMonolithRing(player) ? 0.12f : 0.08f))
            * ruinsPreparationFactor;
    }

    if (tasks->currentEvent == EVENT_SPORE_STORM) {
        if (area == MAP_AREA_SWAMP_OUTER) {
            totalOxygenDrain += TasksRuntime_IsOuterSwampLowerRoute(player) ? 0.22f : 0.15f;
            Player_AddPoison(player,
                             deltaTime * (TasksRuntime_IsOuterSwampLowerRoute(player) ? 1.4f : 0.9f) * (1.0f - filterStrength));
        } else if (area == MAP_AREA_SWAMP_DEEP) {
            totalOxygenDrain += TasksRuntime_IsDeepSwampCore(player) ? 0.30f : 0.22f;
            Player_AddPoison(player,
                             deltaTime * (TasksRuntime_IsDeepSwampCore(player) ? 2.3f : 1.7f) * (1.0f - filterStrength));
        }
    }

    if (area == MAP_AREA_SWAMP_DEEP && TasksRuntime_IsDeepSwampCore(player) && !player->hasProtectionSuit) {
        Player_AddPoison(player, deltaTime * 0.75f * (1.0f - filterStrength * 0.5f));
    }

    if (hazard == HAZARD_SWAMP) {
        hazardDrain = 0.62f * (1.0f - filterStrength * 0.75f);
        totalOxygenDrain += fmaxf(0.18f, hazardDrain);
        Player_AddPressure(player, deltaTime * 0.25f * (1.0f - filterStrength * 0.40f));
    } else if (hazard == HAZARD_POISON) {
        hazardDrain = 0.82f * (1.0f - filterStrength * 0.85f);
        totalOxygenDrain += fmaxf(0.20f, hazardDrain);
        Player_AddPoison(player, deltaTime * 5.0f * (1.0f - filterStrength));
        Player_AddPressure(player, deltaTime * 0.28f * (1.0f - filterStrength * 0.35f));
    } else if (hazard == HAZARD_TRIP) {
        Player_DamageHealth(player, deltaTime * 0.50f);
    }

    if (oxygenReserveStrength > 0.0f && totalOxygenDrain > 0.0f) {
        reserveReduction = fminf(totalOxygenDrain * 0.55f, oxygenReserveStrength * 0.035f);
        totalOxygenDrain = fmaxf(0.0f, totalOxygenDrain - reserveReduction);
        if (player->oxygen < 45.0f) {
            Player_AddOxygen(player, deltaTime * fminf(1.45f, 0.26f + oxygenReserveStrength * 0.040f));
        }
    }

    Player_DamageOxygen(player, totalOxygenDrain * deltaTime);
    oxygenLeakEffect = Player_GetStatusEffect(player, PLAYER_STATUS_OXYGEN_LEAK);
    oxygenLeakActive = oxygenLeakEffect != NULL && oxygenLeakEffect->active;
    oxygenLeakLevel = oxygenLeakActive ? oxygenLeakEffect->level : 0;
    oxygenLeakStrength = oxygenLeakActive ? oxygenLeakEffect->magnitude : 0.0f;
    SetPlayerStatusActive(player,
                          PLAYER_STATUS_OXYGEN_LEAK,
                          oxygenLeakActive,
                          oxygenLeakLevel > 0 ? oxygenLeakLevel : 1,
                          oxygenLeakStrength);

    poisonLevel = TasksRuntime_GetPoisonStatusLevel(player->poison);
    if (poisonLevel == 1) {
        poisonHealthRate = 0.35f;
        poisonOxygenRate = 0.20f;
    } else if (poisonLevel == 2) {
        poisonHealthRate = 0.75f;
        poisonOxygenRate = 0.45f;
    } else if (poisonLevel >= 3) {
        poisonHealthRate = 1.35f;
        poisonOxygenRate = 0.75f;
    } else {
        poisonHealthRate = 0.0f;
        poisonOxygenRate = 0.0f;
    }

    SetPlayerStatusActive(player,
                          PLAYER_STATUS_POISONED,
                          poisonLevel > 0,
                          poisonLevel,
                          player->poison);
    if (poisonLevel > 0) {
        poisonMitigation = 1.0f - filterStrength * 0.45f;
        Player_DamageHealth(player, poisonHealthRate * poisonMitigation * deltaTime);
        Player_DamageOxygen(player, poisonOxygenRate * poisonMitigation * deltaTime);
    }

    if (player->oxygen <= 0.0f) {
        Player_DamageHealth(player, deltaTime * 7.0f);
        SetPlayerStatusActive(player, PLAYER_STATUS_SUFFOCATING, true, 1, 7.0f);
    } else {
        Player_ClearStatus(player, PLAYER_STATUS_SUFFOCATING);
    }

    TasksRuntime_GetLowOxygenThresholds(area, player, &lowOxygenThreshold, &criticalOxygenThreshold);

    lowOxygenLevel = 0;
    if (player->oxygen <= criticalOxygenThreshold) {
        lowOxygenLevel = 2;
    } else if (player->oxygen < lowOxygenThreshold) {
        lowOxygenLevel = 1;
    }
    SetPlayerStatusActive(player,
                          PLAYER_STATUS_LOW_OXYGEN,
                          lowOxygenLevel > 0,
                          lowOxygenLevel,
                          player->oxygen);

    criticalLevel = 0;
    if (player->health <= 15.0f) {
        criticalLevel = 2;
    } else if (player->health <= 35.0f) {
        criticalLevel = 1;
    }
    SetPlayerStatusActive(player,
                          PLAYER_STATUS_CRITICAL_CONDITION,
                          criticalLevel > 0,
                          criticalLevel,
                          player->health);

    SetPlayerStatusActive(player,
                          PLAYER_STATUS_FILTERED,
                          player->hasProtectionSuit || filteredActive,
                          filteredLevel > 0 ? filteredLevel : 1,
                          filterStrength);

    SetPlayerStatusActive(player,
                          PLAYER_STATUS_OXYGEN_RESERVE,
                          oxygenReserveActive,
                          oxygenReserveLevel > 0 ? oxygenReserveLevel : 1,
                          oxygenReserveStrength);

    inSafeRecoveryZone = TasksRuntime_IsInSafeRecoveryZone(map, player, area);
    if (inSafeRecoveryZone) {
        player->safeRecoveryTimer += deltaTime;
        safeRecoveryAmount = tasks->oxygenRepairLevel >= 2 ? 5.0f : SAFE_RECOVERY_AMOUNT;
        while (player->safeRecoveryTimer >= SAFE_RECOVERY_INTERVAL) {
            Player_RecoverStamina(player, safeRecoveryAmount);
            Player_RecoverHealth(player, area == MAP_AREA_BASE ? 6.0f : 4.0f);
            Player_AddOxygen(player, area == MAP_AREA_BASE ? 12.0f : 8.0f);
            player->safeRecoveryTimer -= SAFE_RECOVERY_INTERVAL;
        }

        if (player->poison > 0.0f) {
            player->poison = fmaxf(0.0f, player->poison - (area == MAP_AREA_BASE ? 4.0f : 2.0f) * deltaTime);
            if (player->poison <= 0.0f) {
                Player_ClearPoison(player);
            }
        }
        Player_SetStatus(player,
                         PLAYER_STATUS_CAMP_RECOVERY,
                         area == MAP_AREA_BASE ? 2 : 1,
                         area == MAP_AREA_BASE ? 18.0f : 45.0f,
                         area == MAP_AREA_BASE ? 6.0f : safeRecoveryAmount);
    } else {
        player->safeRecoveryTimer = 0.0f;
        if (campRecoveryActive) {
            Player_RecoverHealth(player, deltaTime * (0.12f * campRecoveryStrength));
            Player_AddOxygen(player, deltaTime * (0.36f * campRecoveryStrength));
            if (player->poison > 0.0f) {
                player->poison = fmaxf(0.0f, player->poison - deltaTime * (0.25f * campRecoveryStrength));
                if (player->poison <= 0.0f) {
                    Player_ClearPoison(player);
                }
            }
        }
    }

    if (player->health <= 0.0f) {
        player->health = 0.0f;
    }

}
