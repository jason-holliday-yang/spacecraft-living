#include "save_internal.h"

#include <cfloat>

static bool IsFiniteFloat(float value) {
    return value <= FLT_MAX && value >= -FLT_MAX;
}

static int ClampInt(int value, int minValue, int maxValue) {
    if (value < minValue) {
        return minValue;
    }
    if (value > maxValue) {
        return maxValue;
    }
    return value;
}

static float ClampFloatRange(float value, float minValue, float maxValue) {
    if (value < minValue) {
        return minValue;
    }
    if (value > maxValue) {
        return maxValue;
    }
    return value;
}

static float SanitizeFiniteFloat(float value, float fallbackValue, float minValue, float maxValue) {
    if (!IsFiniteFloat(value)) {
        return fallbackValue;
    }

    return ClampFloatRange(value, minValue, maxValue);
}

static bool IsCardinalFacing(int facingX, int facingY) {
    return (facingX == 0 && (facingY == -1 || facingY == 1))
        || (facingY == 0 && (facingX == -1 || facingX == 1));
}

void SaveInternal_SanitizeSnapshot(SaveSnapshot *snapshot) {
    int index;
    float maxHealth;
    float maxStamina;

    if (snapshot == NULL) {
        return;
    }

    if (!Map_IsWithinBounds(snapshot->gridX, snapshot->gridY)) {
        snapshot->gridX = PLAYER_START_X;
        snapshot->gridY = PLAYER_START_Y;
    }

    if (!IsCardinalFacing(snapshot->facingX, snapshot->facingY)) {
        snapshot->facingX = 0;
        snapshot->facingY = 1;
    }

    snapshot->maxHealthBonus = SanitizeFiniteFloat(snapshot->maxHealthBonus, 0.0f, 0.0f, 200.0f);
    maxHealth = BASE_MAX_HEALTH + snapshot->maxHealthBonus;
    snapshot->health = SanitizeFiniteFloat(snapshot->health, INITIAL_HEALTH, 0.0f, maxHealth);
    snapshot->maxStaminaBonus = SanitizeFiniteFloat(snapshot->maxStaminaBonus, 0.0f, 0.0f, 200.0f);
    maxStamina = BASE_MAX_STAMINA + snapshot->maxStaminaBonus;
    snapshot->stamina = SanitizeFiniteFloat(snapshot->stamina, INITIAL_STAMINA, 0.0f, maxStamina);
    snapshot->pressure = SanitizeFiniteFloat(snapshot->pressure, INITIAL_PRESSURE, 0.0f, MAX_PRESSURE);
    snapshot->oxygen = SanitizeFiniteFloat(snapshot->oxygen, INITIAL_OXYGEN, 0.0f, MAX_OXYGEN);
    snapshot->poison = SanitizeFiniteFloat(snapshot->poison, 0.0f, 0.0f, MAX_POISON);
    snapshot->attackBonus = SanitizeFiniteFloat(snapshot->attackBonus, 0.0f, 0.0f, 100.0f);
    snapshot->cycleTimer = SanitizeFiniteFloat(snapshot->cycleTimer, 0.0f, 0.0f, FULL_CYCLE_SECONDS);
    snapshot->elapsedSeconds = SanitizeFiniteFloat(snapshot->elapsedSeconds, 0.0f, 0.0f, 100000000.0f);
    snapshot->deathCount = snapshot->deathCount < 0 ? 0 : snapshot->deathCount;
    snapshot->stage = ClampInt(snapshot->stage, 1, 7);
    snapshot->dayCount = snapshot->dayCount < 0 ? 0 : snapshot->dayCount;
    snapshot->phase = ClampInt(snapshot->phase, DAY_PHASE_DAY, DAY_PHASE_NIGHT);
    snapshot->currentEvent = ClampInt(snapshot->currentEvent, EVENT_HARVEST, EVENT_COUNT - 1);
    snapshot->oxygenRepairLevel = ClampInt(snapshot->oxygenRepairLevel, 0, 2);
    snapshot->commRepairLevel = ClampInt(snapshot->commRepairLevel, 0, 1);
    snapshot->energyRepairLevel = ClampInt(snapshot->energyRepairLevel, 0, 1);
    if (snapshot->westW1Completed) {
        snapshot->westW1Started = true;
    }
    if (snapshot->westW2Completed) {
        snapshot->westW2Started = true;
        snapshot->westW1Started = true;
        snapshot->westW1Completed = true;
    }
    if (snapshot->westW3Completed) {
        snapshot->westW3Started = true;
        snapshot->westW2Started = true;
        snapshot->westW2Completed = true;
        snapshot->westW1Started = true;
        snapshot->westW1Completed = true;
    }
    if (snapshot->westW4Completed) {
        snapshot->westW4Started = true;
        snapshot->westW3Started = true;
        snapshot->westW3Completed = true;
        snapshot->westW2Started = true;
        snapshot->westW2Completed = true;
        snapshot->westW1Started = true;
        snapshot->westW1Completed = true;
    }
    if (snapshot->westW5Completed) {
        snapshot->westW5Started = true;
        snapshot->westW4Started = true;
        snapshot->westW4Completed = true;
        snapshot->westW3Started = true;
        snapshot->westW3Completed = true;
        snapshot->westW2Started = true;
        snapshot->westW2Completed = true;
        snapshot->westW1Started = true;
        snapshot->westW1Completed = true;
    }
    if (snapshot->southS1Completed) {
        snapshot->southS1Started = true;
    }
    if (snapshot->southS2Completed) {
        snapshot->southS2Started = true;
        snapshot->southS1Started = true;
        snapshot->southS1Completed = true;
    }
    if (snapshot->southS3Completed) {
        snapshot->southS3Started = true;
        snapshot->southS2Started = true;
        snapshot->southS2Completed = true;
        snapshot->southS1Started = true;
        snapshot->southS1Completed = true;
    }
    if (snapshot->southS4Completed) {
        snapshot->southS4Started = true;
        snapshot->southS3Started = true;
        snapshot->southS3Completed = true;
        snapshot->southS2Started = true;
        snapshot->southS2Completed = true;
        snapshot->southS1Started = true;
        snapshot->southS1Completed = true;
    }
    if (snapshot->southS5Completed) {
        snapshot->southS5Started = true;
        snapshot->southS4Started = true;
        snapshot->southS4Completed = true;
        snapshot->southS3Started = true;
        snapshot->southS3Completed = true;
        snapshot->southS2Started = true;
        snapshot->southS2Completed = true;
        snapshot->southS1Started = true;
        snapshot->southS1Completed = true;
    }
    snapshot->monolithsLit = ClampInt(snapshot->monolithsLit, 0, 3);
    snapshot->ending = ClampInt(snapshot->ending, ENDING_NONE, ENDING_FAILURE);

    if (!snapshot->campPlaced || !Map_IsWithinBounds(snapshot->campX, snapshot->campY)) {
        snapshot->campPlaced = false;
        snapshot->campX = -1;
        snapshot->campY = -1;
    }

    snapshot->clearedDynamicTileCount = ClampInt(snapshot->clearedDynamicTileCount, 0, SAVE_DYNAMIC_TILE_MAX);
    for (index = 0; index < snapshot->clearedDynamicTileCount; index++) {
        if (!Map_IsWithinBounds(snapshot->clearedDynamicTileX[index], snapshot->clearedDynamicTileY[index])) {
            snapshot->clearedDynamicTileX[index] = PLAYER_START_X;
            snapshot->clearedDynamicTileY[index] = PLAYER_START_Y;
        }
    }

    for (index = 0; index < RESOURCE_COUNT; index++) {
        if (snapshot->resources[index] < 0) {
            snapshot->resources[index] = 0;
        }
    }

    for (index = 0; index < PLAYER_STATUS_COUNT; index++) {
        snapshot->statuses[index].level = ClampInt(snapshot->statuses[index].level, 0, 5);
        snapshot->statuses[index].remainingTime = SanitizeFiniteFloat(snapshot->statuses[index].remainingTime, 0.0f, 0.0f, 100000.0f);
        snapshot->statuses[index].magnitude = SanitizeFiniteFloat(snapshot->statuses[index].magnitude, 0.0f, 0.0f, 1000.0f);
        if (!snapshot->statuses[index].active) {
            snapshot->statuses[index].level = 0;
            snapshot->statuses[index].remainingTime = 0.0f;
            snapshot->statuses[index].magnitude = 0.0f;
        } else if (snapshot->statuses[index].level <= 0) {
            snapshot->statuses[index].level = 1;
        }
    }

    for (index = 0; index < MAX_RESOURCE_NODES; index++) {
        if (snapshot->nodes[index].respawnsRemaining < 0) {
            snapshot->nodes[index].respawnsRemaining = 0;
        }
    }

    for (index = 0; index < MAX_MONSTERS; index++) {
        snapshot->monsters[index].health = SanitizeFiniteFloat(snapshot->monsters[index].health, 0.0f, 0.0f, 500.0f);
        if (!Map_IsWithinBounds(snapshot->monsters[index].gridX, snapshot->monsters[index].gridY)) {
            snapshot->monsters[index].active = false;
            snapshot->monsters[index].gridX = PLAYER_START_X;
            snapshot->monsters[index].gridY = PLAYER_START_Y;
        }
        if (snapshot->monsters[index].health <= 0.0f) {
            snapshot->monsters[index].active = false;
        }
    }
}

bool SaveInternal_IsSnapshotBinaryDataValid(const SaveSnapshot *snapshot) {
    int index;

    if (snapshot == NULL
        || !IsFiniteFloat(snapshot->health)
        || !IsFiniteFloat(snapshot->stamina)
        || !IsFiniteFloat(snapshot->pressure)
        || !IsFiniteFloat(snapshot->oxygen)
        || !IsFiniteFloat(snapshot->poison)
        || !IsFiniteFloat(snapshot->maxHealthBonus)
        || !IsFiniteFloat(snapshot->maxStaminaBonus)
        || !IsFiniteFloat(snapshot->attackBonus)
        || !IsFiniteFloat(snapshot->cycleTimer)
        || !IsFiniteFloat(snapshot->elapsedSeconds)) {
        return false;
    }

    for (index = 0; index < PLAYER_STATUS_COUNT; index++) {
        if (!IsFiniteFloat(snapshot->statuses[index].remainingTime)
            || !IsFiniteFloat(snapshot->statuses[index].magnitude)) {
            return false;
        }
    }

    return true;
}
