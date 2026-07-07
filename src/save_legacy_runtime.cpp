#include "save_legacy_runtime.h"

#include "config.h"

static int GetLegacyPoisonStatusLevel(float poison) {
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

static void ApplyLegacyDerivedStatuses(SaveSnapshot *snapshot) {
    int index;
    int poisonLevel;

    if (snapshot == NULL) {
        return;
    }

    for (index = 0; index < PLAYER_STATUS_COUNT; index++) {
        snapshot->statuses[index].active = false;
        snapshot->statuses[index].level = 0;
        snapshot->statuses[index].remainingTime = 0.0f;
        snapshot->statuses[index].magnitude = 0.0f;
    }

    poisonLevel = GetLegacyPoisonStatusLevel(snapshot->poison);
    if (poisonLevel > 0) {
        snapshot->statuses[PLAYER_STATUS_POISONED].active = true;
        snapshot->statuses[PLAYER_STATUS_POISONED].level = poisonLevel;
        snapshot->statuses[PLAYER_STATUS_POISONED].magnitude = snapshot->poison;
    }

    if (snapshot->oxygen <= 0.0f) {
        snapshot->statuses[PLAYER_STATUS_LOW_OXYGEN].active = true;
        snapshot->statuses[PLAYER_STATUS_LOW_OXYGEN].level = 2;
        snapshot->statuses[PLAYER_STATUS_LOW_OXYGEN].magnitude = snapshot->oxygen;
        snapshot->statuses[PLAYER_STATUS_SUFFOCATING].active = true;
        snapshot->statuses[PLAYER_STATUS_SUFFOCATING].level = 1;
        snapshot->statuses[PLAYER_STATUS_SUFFOCATING].magnitude = 7.0f;
    } else if (snapshot->oxygen <= 10.0f) {
        snapshot->statuses[PLAYER_STATUS_LOW_OXYGEN].active = true;
        snapshot->statuses[PLAYER_STATUS_LOW_OXYGEN].level = 2;
        snapshot->statuses[PLAYER_STATUS_LOW_OXYGEN].magnitude = snapshot->oxygen;
    } else if (snapshot->oxygen < 30.0f) {
        snapshot->statuses[PLAYER_STATUS_LOW_OXYGEN].active = true;
        snapshot->statuses[PLAYER_STATUS_LOW_OXYGEN].level = 1;
        snapshot->statuses[PLAYER_STATUS_LOW_OXYGEN].magnitude = snapshot->oxygen;
    }

    if (snapshot->health <= 15.0f) {
        snapshot->statuses[PLAYER_STATUS_CRITICAL_CONDITION].active = true;
        snapshot->statuses[PLAYER_STATUS_CRITICAL_CONDITION].level = 2;
        snapshot->statuses[PLAYER_STATUS_CRITICAL_CONDITION].magnitude = snapshot->health;
    } else if (snapshot->health <= 35.0f) {
        snapshot->statuses[PLAYER_STATUS_CRITICAL_CONDITION].active = true;
        snapshot->statuses[PLAYER_STATUS_CRITICAL_CONDITION].level = 1;
        snapshot->statuses[PLAYER_STATUS_CRITICAL_CONDITION].magnitude = snapshot->health;
    }
}

void SaveLegacy_ApplyDerivedSurvivalFields(SaveSnapshot *snapshot) {
    if (snapshot == NULL) {
        return;
    }

    snapshot->health = snapshot->stamina;
    snapshot->maxHealthBonus = snapshot->maxStaminaBonus;
    snapshot->pressure = LEGACY_INITIAL_PRESSURE;
    ApplyLegacyDerivedStatuses(snapshot);
}

void SaveLegacy_SeedFallbackClearedAirlockTiles(SaveSnapshot *snapshot) {
    int index;

    if (snapshot == NULL) {
        return;
    }

    snapshot->clearedDynamicTileCount = 0;
    if (snapshot->stage < 4) {
        return;
    }

    for (index = AIRLOCK_DOOR_TOP_Y;
         index < AIRLOCK_DOOR_TOP_Y + AIRLOCK_DOOR_HEIGHT && snapshot->clearedDynamicTileCount < SAVE_DYNAMIC_TILE_MAX;
         index++) {
        snapshot->clearedDynamicTileX[snapshot->clearedDynamicTileCount] = AIRLOCK_DOOR_X;
        snapshot->clearedDynamicTileY[snapshot->clearedDynamicTileCount] = index;
        snapshot->clearedDynamicTileCount += 1;
    }
}
