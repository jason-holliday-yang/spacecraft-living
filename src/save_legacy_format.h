#ifndef SAVE_LEGACY_FORMAT_H
#define SAVE_LEGACY_FORMAT_H

#include <stdint.h>

#include "save_system.h"

/* Shared historical save layouts used by the legacy adapter and persistence smoke tests. */

#define SAVE_MAGIC_V2 "SCLSAV2"
#define SAVE_MAGIC_V3 "SCLSAV3"
#define SAVE_MAGIC_V4 "SCLSAV4"
#define SAVE_LEGACY_MAX_LOGS_V2 6
#define SAVE_LEGACY_MAX_LOGS_V4 6

typedef struct SaveLegacyRecordV4 {
    char magic[8];
    int32_t gridX;
    int32_t gridY;
    int32_t facingX;
    int32_t facingY;
    float stamina;
    float pressure;
    float oxygen;
    float poison;
    float maxStaminaBonus;
    float attackBonus;
    int32_t deathCount;
    uint8_t crouching;
    uint8_t hasGlowStick;
    uint8_t hasRope;
    uint8_t hasLaserGun;
    uint8_t hasProtectionSuit;
    uint8_t hasSignalAmplifier;
    uint8_t hasFieldCamp;
    int32_t resources[RESOURCE_COUNT];
    int32_t stage;
    int32_t dayCount;
    int32_t phase;
    int32_t currentEvent;
    float cycleTimer;
    float elapsedSeconds;
    int32_t oxygenRepairLevel;
    int32_t commRepairLevel;
    int32_t energyRepairLevel;
    uint8_t crashClueFound;
    uint8_t amplifierUnlocked;
    uint8_t bossDefeated;
    uint8_t signalTowerActivated;
    uint8_t monolithActivated[3];
    int32_t monolithsLit;
    int32_t ending;
    uint8_t campPlaced;
    int32_t campX;
    int32_t campY;
    uint8_t nodeActive[MAX_RESOURCE_NODES];
    int32_t nodeRespawns[MAX_RESOURCE_NODES];
    uint8_t monsterActive[MAX_MONSTERS];
    int32_t monsterGridX[MAX_MONSTERS];
    int32_t monsterGridY[MAX_MONSTERS];
    float monsterHealth[MAX_MONSTERS];
    uint8_t monsterPhaseTriggered[MAX_MONSTERS];
    uint8_t logsCollected[SAVE_LEGACY_MAX_LOGS_V4];
    uint8_t communicatorUnlocked;
    int32_t clearedDynamicTileCount;
    int16_t clearedDynamicTileX[SAVE_DYNAMIC_TILE_MAX];
    int16_t clearedDynamicTileY[SAVE_DYNAMIC_TILE_MAX];
} SaveLegacyRecordV4;

typedef struct SaveLegacyRecordV2 {
    char magic[8];
    int32_t gridX;
    int32_t gridY;
    int32_t facingX;
    int32_t facingY;
    float stamina;
    float pressure;
    float oxygen;
    float poison;
    float maxStaminaBonus;
    float attackBonus;
    int32_t deathCount;
    uint8_t crouching;
    uint8_t hasGlowStick;
    uint8_t hasRope;
    uint8_t hasLaserGun;
    uint8_t hasProtectionSuit;
    uint8_t hasSignalAmplifier;
    uint8_t hasFieldCamp;
    int32_t resources[RESOURCE_COUNT];
    int32_t stage;
    int32_t dayCount;
    int32_t phase;
    int32_t currentEvent;
    float cycleTimer;
    float elapsedSeconds;
    int32_t oxygenRepairLevel;
    int32_t commRepairLevel;
    int32_t energyRepairLevel;
    uint8_t crashClueFound;
    uint8_t amplifierUnlocked;
    uint8_t bossDefeated;
    uint8_t signalTowerActivated;
    uint8_t monolithActivated[3];
    int32_t monolithsLit;
    int32_t ending;
    uint8_t campPlaced;
    int32_t campX;
    int32_t campY;
    uint8_t nodeActive[MAX_RESOURCE_NODES];
    int32_t nodeRespawns[MAX_RESOURCE_NODES];
    uint8_t monsterActive[MAX_MONSTERS];
    int32_t monsterGridX[MAX_MONSTERS];
    int32_t monsterGridY[MAX_MONSTERS];
    float monsterHealth[MAX_MONSTERS];
    uint8_t monsterPhaseTriggered[MAX_MONSTERS];
    uint8_t logsCollected[SAVE_LEGACY_MAX_LOGS_V2];
    uint8_t communicatorUnlocked;
} SaveLegacyRecordV2;

#endif
