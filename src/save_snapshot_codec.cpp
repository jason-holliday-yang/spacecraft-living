#include "save_codec_internal.h"
#include "save_legacy_runtime.h"

#include <cstring>

static bool DecodeSnapshotV5Buffer(SaveSnapshot *snapshot, const uint8_t *buffer, size_t fileSize) {
    SaveBufferReader reader;
    char magic[8];
    int index;

    if (snapshot == NULL || buffer == NULL) {
        return false;
    }

    std::memset(snapshot, 0, sizeof(*snapshot));
    SaveReader_Init(&reader, buffer, fileSize);
    std::memset(magic, 0, sizeof(magic));
    SaveReader_ReadBytes(&reader, magic, sizeof(magic));
    if (reader.failed || std::memcmp(magic, SAVE_MAGIC_V5, 7) != 0) {
        return false;
    }

    snapshot->gridX = SaveReader_ReadInt32(&reader);
    snapshot->gridY = SaveReader_ReadInt32(&reader);
    snapshot->facingX = SaveReader_ReadInt32(&reader);
    snapshot->facingY = SaveReader_ReadInt32(&reader);
    snapshot->stamina = SaveReader_ReadFloat(&reader);
    snapshot->pressure = SaveReader_ReadFloat(&reader);
    snapshot->oxygen = SaveReader_ReadFloat(&reader);
    snapshot->poison = SaveReader_ReadFloat(&reader);
    snapshot->maxStaminaBonus = SaveReader_ReadFloat(&reader);
    snapshot->attackBonus = SaveReader_ReadFloat(&reader);
    snapshot->deathCount = SaveReader_ReadInt32(&reader);
    snapshot->crouching = SaveReader_ReadUInt8(&reader) != 0;
    snapshot->hasGlowStick = SaveReader_ReadUInt8(&reader) != 0;
    snapshot->hasRope = SaveReader_ReadUInt8(&reader) != 0;
    snapshot->hasLaserGun = SaveReader_ReadUInt8(&reader) != 0;
    snapshot->hasProtectionSuit = SaveReader_ReadUInt8(&reader) != 0;
    snapshot->hasSignalAmplifier = SaveReader_ReadUInt8(&reader) != 0;
    snapshot->hasFieldCamp = SaveReader_ReadUInt8(&reader) != 0;

    for (index = 0; index < RESOURCE_COUNT; index++) {
        snapshot->resources[index] = SaveReader_ReadInt32(&reader);
    }

    snapshot->stage = SaveReader_ReadInt32(&reader);
    snapshot->dayCount = SaveReader_ReadInt32(&reader);
    snapshot->phase = SaveReader_ReadInt32(&reader);
    snapshot->currentEvent = SaveReader_ReadInt32(&reader);
    snapshot->cycleTimer = SaveReader_ReadFloat(&reader);
    snapshot->elapsedSeconds = SaveReader_ReadFloat(&reader);
    snapshot->oxygenRepairLevel = SaveReader_ReadInt32(&reader);
    snapshot->commRepairLevel = SaveReader_ReadInt32(&reader);
    snapshot->energyRepairLevel = SaveReader_ReadInt32(&reader);
    snapshot->crashClueFound = SaveReader_ReadUInt8(&reader) != 0;
    snapshot->amplifierUnlocked = SaveReader_ReadUInt8(&reader) != 0;
    snapshot->bossDefeated = SaveReader_ReadUInt8(&reader) != 0;
    snapshot->signalTowerActivated = SaveReader_ReadUInt8(&reader) != 0;
    snapshot->monolithActivated[0] = SaveReader_ReadUInt8(&reader) != 0;
    snapshot->monolithActivated[1] = SaveReader_ReadUInt8(&reader) != 0;
    snapshot->monolithActivated[2] = SaveReader_ReadUInt8(&reader) != 0;
    snapshot->monolithsLit = SaveReader_ReadInt32(&reader);
    snapshot->ending = SaveReader_ReadInt32(&reader);
    snapshot->campPlaced = SaveReader_ReadUInt8(&reader) != 0;
    snapshot->campX = SaveReader_ReadInt32(&reader);
    snapshot->campY = SaveReader_ReadInt32(&reader);
    snapshot->communicatorUnlocked = SaveReader_ReadUInt8(&reader) != 0;
    snapshot->clearedDynamicTileCount = SaveReader_ReadInt32(&reader);

    for (index = 0; index < MAX_RESOURCE_NODES; index++) {
        snapshot->nodes[index].active = SaveReader_ReadUInt8(&reader) != 0;
        snapshot->nodes[index].respawnsRemaining = SaveReader_ReadInt32(&reader);
    }
    for (index = 0; index < MAX_MONSTERS; index++) {
        snapshot->monsters[index].active = SaveReader_ReadUInt8(&reader) != 0;
        snapshot->monsters[index].gridX = SaveReader_ReadInt32(&reader);
        snapshot->monsters[index].gridY = SaveReader_ReadInt32(&reader);
        snapshot->monsters[index].health = SaveReader_ReadFloat(&reader);
        snapshot->monsters[index].phaseTriggered = SaveReader_ReadUInt8(&reader) != 0;
    }
    for (index = 0; index < SAVE_NATIVE_MAX_LOGS_LEGACY; index++) {
        snapshot->logs[index].collected = SaveReader_ReadUInt8(&reader) != 0;
    }
    for (index = 0; index < SAVE_DYNAMIC_TILE_MAX; index++) {
        snapshot->clearedDynamicTileX[index] = SaveReader_ReadInt16(&reader);
        snapshot->clearedDynamicTileY[index] = SaveReader_ReadInt16(&reader);
    }

    SaveLegacy_ApplyDerivedSurvivalFields(snapshot);
    return !reader.failed && reader.offset == fileSize;
}

static bool DecodeSnapshotV6Buffer(SaveSnapshot *snapshot, const uint8_t *buffer, size_t fileSize) {
    SaveBufferReader reader;
    char magic[8];
    int index;

    if (snapshot == NULL || buffer == NULL) {
        return false;
    }

    std::memset(snapshot, 0, sizeof(*snapshot));
    SaveReader_Init(&reader, buffer, fileSize);
    std::memset(magic, 0, sizeof(magic));
    SaveReader_ReadBytes(&reader, magic, sizeof(magic));
    if (reader.failed || std::memcmp(magic, SAVE_MAGIC_V6, 7) != 0) {
        return false;
    }

    snapshot->gridX = SaveReader_ReadInt32(&reader);
    snapshot->gridY = SaveReader_ReadInt32(&reader);
    snapshot->facingX = SaveReader_ReadInt32(&reader);
    snapshot->facingY = SaveReader_ReadInt32(&reader);
    snapshot->health = SaveReader_ReadFloat(&reader);
    snapshot->stamina = SaveReader_ReadFloat(&reader);
    snapshot->pressure = SaveReader_ReadFloat(&reader);
    snapshot->oxygen = SaveReader_ReadFloat(&reader);
    snapshot->poison = SaveReader_ReadFloat(&reader);
    snapshot->maxHealthBonus = SaveReader_ReadFloat(&reader);
    snapshot->maxStaminaBonus = SaveReader_ReadFloat(&reader);
    snapshot->attackBonus = SaveReader_ReadFloat(&reader);
    snapshot->deathCount = SaveReader_ReadInt32(&reader);
    snapshot->crouching = SaveReader_ReadUInt8(&reader) != 0;
    snapshot->hasGlowStick = SaveReader_ReadUInt8(&reader) != 0;
    snapshot->hasRope = SaveReader_ReadUInt8(&reader) != 0;
    snapshot->hasLaserGun = SaveReader_ReadUInt8(&reader) != 0;
    snapshot->hasProtectionSuit = SaveReader_ReadUInt8(&reader) != 0;
    snapshot->hasSignalAmplifier = SaveReader_ReadUInt8(&reader) != 0;
    snapshot->hasFieldCamp = SaveReader_ReadUInt8(&reader) != 0;

    for (index = 0; index < PLAYER_STATUS_COUNT; index++) {
        snapshot->statuses[index].active = SaveReader_ReadUInt8(&reader) != 0;
        snapshot->statuses[index].level = SaveReader_ReadInt32(&reader);
        snapshot->statuses[index].remainingTime = SaveReader_ReadFloat(&reader);
        snapshot->statuses[index].magnitude = SaveReader_ReadFloat(&reader);
    }

    for (index = 0; index < RESOURCE_COUNT; index++) {
        snapshot->resources[index] = SaveReader_ReadInt32(&reader);
    }

    snapshot->stage = SaveReader_ReadInt32(&reader);
    snapshot->dayCount = SaveReader_ReadInt32(&reader);
    snapshot->phase = SaveReader_ReadInt32(&reader);
    snapshot->currentEvent = SaveReader_ReadInt32(&reader);
    snapshot->cycleTimer = SaveReader_ReadFloat(&reader);
    snapshot->elapsedSeconds = SaveReader_ReadFloat(&reader);
    snapshot->oxygenRepairLevel = SaveReader_ReadInt32(&reader);
    snapshot->commRepairLevel = SaveReader_ReadInt32(&reader);
    snapshot->energyRepairLevel = SaveReader_ReadInt32(&reader);
    snapshot->crashClueFound = SaveReader_ReadUInt8(&reader) != 0;
    snapshot->amplifierUnlocked = SaveReader_ReadUInt8(&reader) != 0;
    snapshot->bossDefeated = SaveReader_ReadUInt8(&reader) != 0;
    snapshot->signalTowerActivated = SaveReader_ReadUInt8(&reader) != 0;
    snapshot->monolithActivated[0] = SaveReader_ReadUInt8(&reader) != 0;
    snapshot->monolithActivated[1] = SaveReader_ReadUInt8(&reader) != 0;
    snapshot->monolithActivated[2] = SaveReader_ReadUInt8(&reader) != 0;
    snapshot->monolithsLit = SaveReader_ReadInt32(&reader);
    snapshot->ending = SaveReader_ReadInt32(&reader);
    snapshot->campPlaced = SaveReader_ReadUInt8(&reader) != 0;
    snapshot->campX = SaveReader_ReadInt32(&reader);
    snapshot->campY = SaveReader_ReadInt32(&reader);
    snapshot->communicatorUnlocked = SaveReader_ReadUInt8(&reader) != 0;
    snapshot->clearedDynamicTileCount = SaveReader_ReadInt32(&reader);

    for (index = 0; index < MAX_RESOURCE_NODES; index++) {
        snapshot->nodes[index].active = SaveReader_ReadUInt8(&reader) != 0;
        snapshot->nodes[index].respawnsRemaining = SaveReader_ReadInt32(&reader);
    }
    for (index = 0; index < MAX_MONSTERS; index++) {
        snapshot->monsters[index].active = SaveReader_ReadUInt8(&reader) != 0;
        snapshot->monsters[index].gridX = SaveReader_ReadInt32(&reader);
        snapshot->monsters[index].gridY = SaveReader_ReadInt32(&reader);
        snapshot->monsters[index].health = SaveReader_ReadFloat(&reader);
        snapshot->monsters[index].phaseTriggered = SaveReader_ReadUInt8(&reader) != 0;
    }
    for (index = 0; index < SAVE_NATIVE_MAX_LOGS_LEGACY; index++) {
        snapshot->logs[index].collected = SaveReader_ReadUInt8(&reader) != 0;
    }
    for (index = 0; index < SAVE_DYNAMIC_TILE_MAX; index++) {
        snapshot->clearedDynamicTileX[index] = SaveReader_ReadInt16(&reader);
        snapshot->clearedDynamicTileY[index] = SaveReader_ReadInt16(&reader);
    }

    return !reader.failed && reader.offset == fileSize;
}

static bool DecodeSnapshotV7Buffer(SaveSnapshot *snapshot, const uint8_t *buffer, size_t fileSize) {
    SaveBufferReader reader;
    char magic[8];
    int index;

    if (snapshot == NULL || buffer == NULL) {
        return false;
    }

    std::memset(snapshot, 0, sizeof(*snapshot));
    SaveReader_Init(&reader, buffer, fileSize);
    std::memset(magic, 0, sizeof(magic));
    SaveReader_ReadBytes(&reader, magic, sizeof(magic));
    if (reader.failed || std::memcmp(magic, SAVE_MAGIC_V7, 7) != 0) {
        return false;
    }

    snapshot->gridX = SaveReader_ReadInt32(&reader);
    snapshot->gridY = SaveReader_ReadInt32(&reader);
    snapshot->facingX = SaveReader_ReadInt32(&reader);
    snapshot->facingY = SaveReader_ReadInt32(&reader);
    snapshot->health = SaveReader_ReadFloat(&reader);
    snapshot->stamina = SaveReader_ReadFloat(&reader);
    snapshot->pressure = SaveReader_ReadFloat(&reader);
    snapshot->oxygen = SaveReader_ReadFloat(&reader);
    snapshot->poison = SaveReader_ReadFloat(&reader);
    snapshot->maxHealthBonus = SaveReader_ReadFloat(&reader);
    snapshot->maxStaminaBonus = SaveReader_ReadFloat(&reader);
    snapshot->attackBonus = SaveReader_ReadFloat(&reader);
    snapshot->deathCount = SaveReader_ReadInt32(&reader);
    snapshot->crouching = SaveReader_ReadUInt8(&reader) != 0;
    snapshot->hasGlowStick = SaveReader_ReadUInt8(&reader) != 0;
    snapshot->hasRope = SaveReader_ReadUInt8(&reader) != 0;
    snapshot->hasLaserGun = SaveReader_ReadUInt8(&reader) != 0;
    snapshot->hasProtectionSuit = SaveReader_ReadUInt8(&reader) != 0;
    snapshot->hasSignalAmplifier = SaveReader_ReadUInt8(&reader) != 0;
    snapshot->hasFieldCamp = SaveReader_ReadUInt8(&reader) != 0;

    for (index = 0; index < PLAYER_STATUS_COUNT; index++) {
        snapshot->statuses[index].active = SaveReader_ReadUInt8(&reader) != 0;
        snapshot->statuses[index].level = SaveReader_ReadInt32(&reader);
        snapshot->statuses[index].remainingTime = SaveReader_ReadFloat(&reader);
        snapshot->statuses[index].magnitude = SaveReader_ReadFloat(&reader);
    }

    for (index = 0; index < RESOURCE_COUNT; index++) {
        snapshot->resources[index] = SaveReader_ReadInt32(&reader);
    }

    snapshot->stage = SaveReader_ReadInt32(&reader);
    snapshot->dayCount = SaveReader_ReadInt32(&reader);
    snapshot->phase = SaveReader_ReadInt32(&reader);
    snapshot->currentEvent = SaveReader_ReadInt32(&reader);
    snapshot->cycleTimer = SaveReader_ReadFloat(&reader);
    snapshot->elapsedSeconds = SaveReader_ReadFloat(&reader);
    snapshot->oxygenRepairLevel = SaveReader_ReadInt32(&reader);
    snapshot->commRepairLevel = SaveReader_ReadInt32(&reader);
    snapshot->energyRepairLevel = SaveReader_ReadInt32(&reader);
    snapshot->crashClueFound = SaveReader_ReadUInt8(&reader) != 0;
    snapshot->amplifierUnlocked = SaveReader_ReadUInt8(&reader) != 0;
    snapshot->bossDefeated = SaveReader_ReadUInt8(&reader) != 0;
    snapshot->signalTowerActivated = SaveReader_ReadUInt8(&reader) != 0;
    snapshot->westW1Started = SaveReader_ReadUInt8(&reader) != 0;
    snapshot->westW1Completed = SaveReader_ReadUInt8(&reader) != 0;
    snapshot->southS1Started = SaveReader_ReadUInt8(&reader) != 0;
    snapshot->southS1Completed = SaveReader_ReadUInt8(&reader) != 0;
    snapshot->monolithActivated[0] = SaveReader_ReadUInt8(&reader) != 0;
    snapshot->monolithActivated[1] = SaveReader_ReadUInt8(&reader) != 0;
    snapshot->monolithActivated[2] = SaveReader_ReadUInt8(&reader) != 0;
    snapshot->monolithsLit = SaveReader_ReadInt32(&reader);
    snapshot->ending = SaveReader_ReadInt32(&reader);
    snapshot->campPlaced = SaveReader_ReadUInt8(&reader) != 0;
    snapshot->campX = SaveReader_ReadInt32(&reader);
    snapshot->campY = SaveReader_ReadInt32(&reader);
    snapshot->communicatorUnlocked = SaveReader_ReadUInt8(&reader) != 0;
    snapshot->clearedDynamicTileCount = SaveReader_ReadInt32(&reader);

    for (index = 0; index < MAX_RESOURCE_NODES; index++) {
        snapshot->nodes[index].active = SaveReader_ReadUInt8(&reader) != 0;
        snapshot->nodes[index].respawnsRemaining = SaveReader_ReadInt32(&reader);
    }
    for (index = 0; index < MAX_MONSTERS; index++) {
        snapshot->monsters[index].active = SaveReader_ReadUInt8(&reader) != 0;
        snapshot->monsters[index].gridX = SaveReader_ReadInt32(&reader);
        snapshot->monsters[index].gridY = SaveReader_ReadInt32(&reader);
        snapshot->monsters[index].health = SaveReader_ReadFloat(&reader);
        snapshot->monsters[index].phaseTriggered = SaveReader_ReadUInt8(&reader) != 0;
    }
    for (index = 0; index < SAVE_NATIVE_MAX_LOGS_LEGACY; index++) {
        snapshot->logs[index].collected = SaveReader_ReadUInt8(&reader) != 0;
    }
    for (index = 0; index < SAVE_DYNAMIC_TILE_MAX; index++) {
        snapshot->clearedDynamicTileX[index] = SaveReader_ReadInt16(&reader);
        snapshot->clearedDynamicTileY[index] = SaveReader_ReadInt16(&reader);
    }

    return !reader.failed && reader.offset == fileSize;
}

static bool DecodeSnapshotV8Buffer(SaveSnapshot *snapshot, const uint8_t *buffer, size_t fileSize) {
    SaveBufferReader reader;
    char magic[8];
    int index;

    if (snapshot == NULL || buffer == NULL) {
        return false;
    }

    std::memset(snapshot, 0, sizeof(*snapshot));
    SaveReader_Init(&reader, buffer, fileSize);
    std::memset(magic, 0, sizeof(magic));
    SaveReader_ReadBytes(&reader, magic, sizeof(magic));
    if (reader.failed || std::memcmp(magic, SAVE_MAGIC_V8, 7) != 0) {
        return false;
    }

    snapshot->gridX = SaveReader_ReadInt32(&reader);
    snapshot->gridY = SaveReader_ReadInt32(&reader);
    snapshot->facingX = SaveReader_ReadInt32(&reader);
    snapshot->facingY = SaveReader_ReadInt32(&reader);
    snapshot->health = SaveReader_ReadFloat(&reader);
    snapshot->stamina = SaveReader_ReadFloat(&reader);
    snapshot->pressure = SaveReader_ReadFloat(&reader);
    snapshot->oxygen = SaveReader_ReadFloat(&reader);
    snapshot->poison = SaveReader_ReadFloat(&reader);
    snapshot->maxHealthBonus = SaveReader_ReadFloat(&reader);
    snapshot->maxStaminaBonus = SaveReader_ReadFloat(&reader);
    snapshot->attackBonus = SaveReader_ReadFloat(&reader);
    snapshot->deathCount = SaveReader_ReadInt32(&reader);
    snapshot->crouching = SaveReader_ReadUInt8(&reader) != 0;
    snapshot->hasGlowStick = SaveReader_ReadUInt8(&reader) != 0;
    snapshot->hasRope = SaveReader_ReadUInt8(&reader) != 0;
    snapshot->hasLaserGun = SaveReader_ReadUInt8(&reader) != 0;
    snapshot->hasProtectionSuit = SaveReader_ReadUInt8(&reader) != 0;
    snapshot->hasSignalAmplifier = SaveReader_ReadUInt8(&reader) != 0;
    snapshot->hasFieldCamp = SaveReader_ReadUInt8(&reader) != 0;

    for (index = 0; index < PLAYER_STATUS_COUNT; index++) {
        snapshot->statuses[index].active = SaveReader_ReadUInt8(&reader) != 0;
        snapshot->statuses[index].level = SaveReader_ReadInt32(&reader);
        snapshot->statuses[index].remainingTime = SaveReader_ReadFloat(&reader);
        snapshot->statuses[index].magnitude = SaveReader_ReadFloat(&reader);
    }

    for (index = 0; index < RESOURCE_COUNT; index++) {
        snapshot->resources[index] = SaveReader_ReadInt32(&reader);
    }

    snapshot->stage = SaveReader_ReadInt32(&reader);
    snapshot->dayCount = SaveReader_ReadInt32(&reader);
    snapshot->phase = SaveReader_ReadInt32(&reader);
    snapshot->currentEvent = SaveReader_ReadInt32(&reader);
    snapshot->cycleTimer = SaveReader_ReadFloat(&reader);
    snapshot->elapsedSeconds = SaveReader_ReadFloat(&reader);
    snapshot->oxygenRepairLevel = SaveReader_ReadInt32(&reader);
    snapshot->commRepairLevel = SaveReader_ReadInt32(&reader);
    snapshot->energyRepairLevel = SaveReader_ReadInt32(&reader);
    snapshot->crashClueFound = SaveReader_ReadUInt8(&reader) != 0;
    snapshot->amplifierUnlocked = SaveReader_ReadUInt8(&reader) != 0;
    snapshot->bossDefeated = SaveReader_ReadUInt8(&reader) != 0;
    snapshot->signalTowerActivated = SaveReader_ReadUInt8(&reader) != 0;
    snapshot->westW1Started = SaveReader_ReadUInt8(&reader) != 0;
    snapshot->westW1Completed = SaveReader_ReadUInt8(&reader) != 0;
    snapshot->westW2Started = SaveReader_ReadUInt8(&reader) != 0;
    snapshot->westW2Completed = SaveReader_ReadUInt8(&reader) != 0;
    snapshot->southS1Started = SaveReader_ReadUInt8(&reader) != 0;
    snapshot->southS1Completed = SaveReader_ReadUInt8(&reader) != 0;
    snapshot->southS2Started = SaveReader_ReadUInt8(&reader) != 0;
    snapshot->southS2Completed = SaveReader_ReadUInt8(&reader) != 0;
    snapshot->monolithActivated[0] = SaveReader_ReadUInt8(&reader) != 0;
    snapshot->monolithActivated[1] = SaveReader_ReadUInt8(&reader) != 0;
    snapshot->monolithActivated[2] = SaveReader_ReadUInt8(&reader) != 0;
    snapshot->monolithsLit = SaveReader_ReadInt32(&reader);
    snapshot->ending = SaveReader_ReadInt32(&reader);
    snapshot->campPlaced = SaveReader_ReadUInt8(&reader) != 0;
    snapshot->campX = SaveReader_ReadInt32(&reader);
    snapshot->campY = SaveReader_ReadInt32(&reader);
    snapshot->communicatorUnlocked = SaveReader_ReadUInt8(&reader) != 0;
    snapshot->clearedDynamicTileCount = SaveReader_ReadInt32(&reader);

    for (index = 0; index < MAX_RESOURCE_NODES; index++) {
        snapshot->nodes[index].active = SaveReader_ReadUInt8(&reader) != 0;
        snapshot->nodes[index].respawnsRemaining = SaveReader_ReadInt32(&reader);
    }
    for (index = 0; index < MAX_MONSTERS; index++) {
        snapshot->monsters[index].active = SaveReader_ReadUInt8(&reader) != 0;
        snapshot->monsters[index].gridX = SaveReader_ReadInt32(&reader);
        snapshot->monsters[index].gridY = SaveReader_ReadInt32(&reader);
        snapshot->monsters[index].health = SaveReader_ReadFloat(&reader);
        snapshot->monsters[index].phaseTriggered = SaveReader_ReadUInt8(&reader) != 0;
    }
    for (index = 0; index < SAVE_NATIVE_MAX_LOGS_LEGACY; index++) {
        snapshot->logs[index].collected = SaveReader_ReadUInt8(&reader) != 0;
    }
    for (index = 0; index < SAVE_DYNAMIC_TILE_MAX; index++) {
        snapshot->clearedDynamicTileX[index] = SaveReader_ReadInt16(&reader);
        snapshot->clearedDynamicTileY[index] = SaveReader_ReadInt16(&reader);
    }

    return !reader.failed && reader.offset == fileSize;
}

static bool DecodeSnapshotV9Buffer(SaveSnapshot *snapshot, const uint8_t *buffer, size_t fileSize) {
    SaveBufferReader reader;
    char magic[8];
    int index;

    if (snapshot == NULL || buffer == NULL) {
        return false;
    }

    std::memset(snapshot, 0, sizeof(*snapshot));
    SaveReader_Init(&reader, buffer, fileSize);
    std::memset(magic, 0, sizeof(magic));
    SaveReader_ReadBytes(&reader, magic, sizeof(magic));
    if (reader.failed || std::memcmp(magic, SAVE_MAGIC_V9, 7) != 0) {
        return false;
    }

    snapshot->gridX = SaveReader_ReadInt32(&reader);
    snapshot->gridY = SaveReader_ReadInt32(&reader);
    snapshot->facingX = SaveReader_ReadInt32(&reader);
    snapshot->facingY = SaveReader_ReadInt32(&reader);
    snapshot->health = SaveReader_ReadFloat(&reader);
    snapshot->stamina = SaveReader_ReadFloat(&reader);
    snapshot->pressure = SaveReader_ReadFloat(&reader);
    snapshot->oxygen = SaveReader_ReadFloat(&reader);
    snapshot->poison = SaveReader_ReadFloat(&reader);
    snapshot->maxHealthBonus = SaveReader_ReadFloat(&reader);
    snapshot->maxStaminaBonus = SaveReader_ReadFloat(&reader);
    snapshot->attackBonus = SaveReader_ReadFloat(&reader);
    snapshot->deathCount = SaveReader_ReadInt32(&reader);
    snapshot->crouching = SaveReader_ReadUInt8(&reader) != 0;
    snapshot->hasGlowStick = SaveReader_ReadUInt8(&reader) != 0;
    snapshot->hasRope = SaveReader_ReadUInt8(&reader) != 0;
    snapshot->hasLaserGun = SaveReader_ReadUInt8(&reader) != 0;
    snapshot->hasProtectionSuit = SaveReader_ReadUInt8(&reader) != 0;
    snapshot->hasSignalAmplifier = SaveReader_ReadUInt8(&reader) != 0;
    snapshot->hasFieldCamp = SaveReader_ReadUInt8(&reader) != 0;

    for (index = 0; index < PLAYER_STATUS_COUNT; index++) {
        snapshot->statuses[index].active = SaveReader_ReadUInt8(&reader) != 0;
        snapshot->statuses[index].level = SaveReader_ReadInt32(&reader);
        snapshot->statuses[index].remainingTime = SaveReader_ReadFloat(&reader);
        snapshot->statuses[index].magnitude = SaveReader_ReadFloat(&reader);
    }

    for (index = 0; index < RESOURCE_COUNT; index++) {
        snapshot->resources[index] = SaveReader_ReadInt32(&reader);
    }

    snapshot->stage = SaveReader_ReadInt32(&reader);
    snapshot->dayCount = SaveReader_ReadInt32(&reader);
    snapshot->phase = SaveReader_ReadInt32(&reader);
    snapshot->currentEvent = SaveReader_ReadInt32(&reader);
    snapshot->cycleTimer = SaveReader_ReadFloat(&reader);
    snapshot->elapsedSeconds = SaveReader_ReadFloat(&reader);
    snapshot->oxygenRepairLevel = SaveReader_ReadInt32(&reader);
    snapshot->commRepairLevel = SaveReader_ReadInt32(&reader);
    snapshot->energyRepairLevel = SaveReader_ReadInt32(&reader);
    snapshot->crashClueFound = SaveReader_ReadUInt8(&reader) != 0;
    snapshot->amplifierUnlocked = SaveReader_ReadUInt8(&reader) != 0;
    snapshot->bossDefeated = SaveReader_ReadUInt8(&reader) != 0;
    snapshot->signalTowerActivated = SaveReader_ReadUInt8(&reader) != 0;
    snapshot->westW1Started = SaveReader_ReadUInt8(&reader) != 0;
    snapshot->westW1Completed = SaveReader_ReadUInt8(&reader) != 0;
    snapshot->westW2Started = SaveReader_ReadUInt8(&reader) != 0;
    snapshot->westW2Completed = SaveReader_ReadUInt8(&reader) != 0;
    snapshot->westW3Started = SaveReader_ReadUInt8(&reader) != 0;
    snapshot->westW3Completed = SaveReader_ReadUInt8(&reader) != 0;
    snapshot->southS1Started = SaveReader_ReadUInt8(&reader) != 0;
    snapshot->southS1Completed = SaveReader_ReadUInt8(&reader) != 0;
    snapshot->southS2Started = SaveReader_ReadUInt8(&reader) != 0;
    snapshot->southS2Completed = SaveReader_ReadUInt8(&reader) != 0;
    snapshot->southS3Started = SaveReader_ReadUInt8(&reader) != 0;
    snapshot->southS3Completed = SaveReader_ReadUInt8(&reader) != 0;
    snapshot->monolithActivated[0] = SaveReader_ReadUInt8(&reader) != 0;
    snapshot->monolithActivated[1] = SaveReader_ReadUInt8(&reader) != 0;
    snapshot->monolithActivated[2] = SaveReader_ReadUInt8(&reader) != 0;
    snapshot->monolithsLit = SaveReader_ReadInt32(&reader);
    snapshot->ending = SaveReader_ReadInt32(&reader);
    snapshot->campPlaced = SaveReader_ReadUInt8(&reader) != 0;
    snapshot->campX = SaveReader_ReadInt32(&reader);
    snapshot->campY = SaveReader_ReadInt32(&reader);
    snapshot->communicatorUnlocked = SaveReader_ReadUInt8(&reader) != 0;
    snapshot->clearedDynamicTileCount = SaveReader_ReadInt32(&reader);

    for (index = 0; index < MAX_RESOURCE_NODES; index++) {
        snapshot->nodes[index].active = SaveReader_ReadUInt8(&reader) != 0;
        snapshot->nodes[index].respawnsRemaining = SaveReader_ReadInt32(&reader);
    }
    for (index = 0; index < MAX_MONSTERS; index++) {
        snapshot->monsters[index].active = SaveReader_ReadUInt8(&reader) != 0;
        snapshot->monsters[index].gridX = SaveReader_ReadInt32(&reader);
        snapshot->monsters[index].gridY = SaveReader_ReadInt32(&reader);
        snapshot->monsters[index].health = SaveReader_ReadFloat(&reader);
        snapshot->monsters[index].phaseTriggered = SaveReader_ReadUInt8(&reader) != 0;
    }
    for (index = 0; index < SAVE_NATIVE_MAX_LOGS_LEGACY; index++) {
        snapshot->logs[index].collected = SaveReader_ReadUInt8(&reader) != 0;
    }
    for (index = 0; index < SAVE_DYNAMIC_TILE_MAX; index++) {
        snapshot->clearedDynamicTileX[index] = SaveReader_ReadInt16(&reader);
        snapshot->clearedDynamicTileY[index] = SaveReader_ReadInt16(&reader);
    }

    return !reader.failed && reader.offset == fileSize;
}

static bool DecodeSnapshotV10Buffer(SaveSnapshot *snapshot, const uint8_t *buffer, size_t fileSize) {
    SaveBufferReader reader;
    char magic[8];
    int index;

    if (snapshot == NULL || buffer == NULL) {
        return false;
    }

    std::memset(snapshot, 0, sizeof(*snapshot));
    SaveReader_Init(&reader, buffer, fileSize);
    std::memset(magic, 0, sizeof(magic));
    SaveReader_ReadBytes(&reader, magic, sizeof(magic));
    if (reader.failed || std::memcmp(magic, SAVE_MAGIC_V10, 7) != 0) {
        return false;
    }

    snapshot->gridX = SaveReader_ReadInt32(&reader);
    snapshot->gridY = SaveReader_ReadInt32(&reader);
    snapshot->facingX = SaveReader_ReadInt32(&reader);
    snapshot->facingY = SaveReader_ReadInt32(&reader);
    snapshot->health = SaveReader_ReadFloat(&reader);
    snapshot->stamina = SaveReader_ReadFloat(&reader);
    snapshot->pressure = SaveReader_ReadFloat(&reader);
    snapshot->oxygen = SaveReader_ReadFloat(&reader);
    snapshot->poison = SaveReader_ReadFloat(&reader);
    snapshot->maxHealthBonus = SaveReader_ReadFloat(&reader);
    snapshot->maxStaminaBonus = SaveReader_ReadFloat(&reader);
    snapshot->attackBonus = SaveReader_ReadFloat(&reader);
    snapshot->deathCount = SaveReader_ReadInt32(&reader);
    snapshot->crouching = SaveReader_ReadUInt8(&reader) != 0;
    snapshot->hasGlowStick = SaveReader_ReadUInt8(&reader) != 0;
    snapshot->hasRope = SaveReader_ReadUInt8(&reader) != 0;
    snapshot->hasLaserGun = SaveReader_ReadUInt8(&reader) != 0;
    snapshot->hasProtectionSuit = SaveReader_ReadUInt8(&reader) != 0;
    snapshot->hasSignalAmplifier = SaveReader_ReadUInt8(&reader) != 0;
    snapshot->hasFieldCamp = SaveReader_ReadUInt8(&reader) != 0;

    for (index = 0; index < PLAYER_STATUS_COUNT; index++) {
        snapshot->statuses[index].active = SaveReader_ReadUInt8(&reader) != 0;
        snapshot->statuses[index].level = SaveReader_ReadInt32(&reader);
        snapshot->statuses[index].remainingTime = SaveReader_ReadFloat(&reader);
        snapshot->statuses[index].magnitude = SaveReader_ReadFloat(&reader);
    }

    for (index = 0; index < RESOURCE_COUNT; index++) {
        snapshot->resources[index] = SaveReader_ReadInt32(&reader);
    }

    snapshot->stage = SaveReader_ReadInt32(&reader);
    snapshot->dayCount = SaveReader_ReadInt32(&reader);
    snapshot->phase = SaveReader_ReadInt32(&reader);
    snapshot->currentEvent = SaveReader_ReadInt32(&reader);
    snapshot->cycleTimer = SaveReader_ReadFloat(&reader);
    snapshot->elapsedSeconds = SaveReader_ReadFloat(&reader);
    snapshot->oxygenRepairLevel = SaveReader_ReadInt32(&reader);
    snapshot->commRepairLevel = SaveReader_ReadInt32(&reader);
    snapshot->energyRepairLevel = SaveReader_ReadInt32(&reader);
    snapshot->crashClueFound = SaveReader_ReadUInt8(&reader) != 0;
    snapshot->amplifierUnlocked = SaveReader_ReadUInt8(&reader) != 0;
    snapshot->bossDefeated = SaveReader_ReadUInt8(&reader) != 0;
    snapshot->signalTowerActivated = SaveReader_ReadUInt8(&reader) != 0;
    snapshot->westW1Started = SaveReader_ReadUInt8(&reader) != 0;
    snapshot->westW1Completed = SaveReader_ReadUInt8(&reader) != 0;
    snapshot->westW2Started = SaveReader_ReadUInt8(&reader) != 0;
    snapshot->westW2Completed = SaveReader_ReadUInt8(&reader) != 0;
    snapshot->westW3Started = SaveReader_ReadUInt8(&reader) != 0;
    snapshot->westW3Completed = SaveReader_ReadUInt8(&reader) != 0;
    snapshot->westW4Started = SaveReader_ReadUInt8(&reader) != 0;
    snapshot->westW4Completed = SaveReader_ReadUInt8(&reader) != 0;
    snapshot->westW5Started = SaveReader_ReadUInt8(&reader) != 0;
    snapshot->westW5Completed = SaveReader_ReadUInt8(&reader) != 0;
    snapshot->southS1Started = SaveReader_ReadUInt8(&reader) != 0;
    snapshot->southS1Completed = SaveReader_ReadUInt8(&reader) != 0;
    snapshot->southS2Started = SaveReader_ReadUInt8(&reader) != 0;
    snapshot->southS2Completed = SaveReader_ReadUInt8(&reader) != 0;
    snapshot->southS3Started = SaveReader_ReadUInt8(&reader) != 0;
    snapshot->southS3Completed = SaveReader_ReadUInt8(&reader) != 0;
    snapshot->southS4Started = SaveReader_ReadUInt8(&reader) != 0;
    snapshot->southS4Completed = SaveReader_ReadUInt8(&reader) != 0;
    snapshot->southS5Started = SaveReader_ReadUInt8(&reader) != 0;
    snapshot->southS5Completed = SaveReader_ReadUInt8(&reader) != 0;
    snapshot->monolithActivated[0] = SaveReader_ReadUInt8(&reader) != 0;
    snapshot->monolithActivated[1] = SaveReader_ReadUInt8(&reader) != 0;
    snapshot->monolithActivated[2] = SaveReader_ReadUInt8(&reader) != 0;
    snapshot->monolithsLit = SaveReader_ReadInt32(&reader);
    snapshot->ending = SaveReader_ReadInt32(&reader);
    snapshot->campPlaced = SaveReader_ReadUInt8(&reader) != 0;
    snapshot->campX = SaveReader_ReadInt32(&reader);
    snapshot->campY = SaveReader_ReadInt32(&reader);
    snapshot->communicatorUnlocked = SaveReader_ReadUInt8(&reader) != 0;
    snapshot->clearedDynamicTileCount = SaveReader_ReadInt32(&reader);

    for (index = 0; index < MAX_RESOURCE_NODES; index++) {
        snapshot->nodes[index].active = SaveReader_ReadUInt8(&reader) != 0;
        snapshot->nodes[index].respawnsRemaining = SaveReader_ReadInt32(&reader);
    }
    for (index = 0; index < MAX_MONSTERS; index++) {
        snapshot->monsters[index].active = SaveReader_ReadUInt8(&reader) != 0;
        snapshot->monsters[index].gridX = SaveReader_ReadInt32(&reader);
        snapshot->monsters[index].gridY = SaveReader_ReadInt32(&reader);
        snapshot->monsters[index].health = SaveReader_ReadFloat(&reader);
        snapshot->monsters[index].phaseTriggered = SaveReader_ReadUInt8(&reader) != 0;
    }
    for (index = 0; index < SAVE_NATIVE_MAX_LOGS_LEGACY; index++) {
        snapshot->logs[index].collected = SaveReader_ReadUInt8(&reader) != 0;
    }
    for (index = 0; index < SAVE_DYNAMIC_TILE_MAX; index++) {
        snapshot->clearedDynamicTileX[index] = SaveReader_ReadInt16(&reader);
        snapshot->clearedDynamicTileY[index] = SaveReader_ReadInt16(&reader);
    }

    return !reader.failed && reader.offset == fileSize;
}

static bool DecodeSnapshotV11Buffer(SaveSnapshot *snapshot, const uint8_t *buffer, size_t fileSize) {
    SaveBufferReader reader;
    char magic[8];
    int index;

    if (snapshot == NULL || buffer == NULL) {
        return false;
    }

    std::memset(snapshot, 0, sizeof(*snapshot));
    SaveReader_Init(&reader, buffer, fileSize);
    std::memset(magic, 0, sizeof(magic));
    SaveReader_ReadBytes(&reader, magic, sizeof(magic));
    if (reader.failed || std::memcmp(magic, SAVE_MAGIC_V11, 7) != 0) {
        return false;
    }

    snapshot->gridX = SaveReader_ReadInt32(&reader);
    snapshot->gridY = SaveReader_ReadInt32(&reader);
    snapshot->facingX = SaveReader_ReadInt32(&reader);
    snapshot->facingY = SaveReader_ReadInt32(&reader);
    snapshot->health = SaveReader_ReadFloat(&reader);
    snapshot->stamina = SaveReader_ReadFloat(&reader);
    snapshot->pressure = SaveReader_ReadFloat(&reader);
    snapshot->oxygen = SaveReader_ReadFloat(&reader);
    snapshot->poison = SaveReader_ReadFloat(&reader);
    snapshot->maxHealthBonus = SaveReader_ReadFloat(&reader);
    snapshot->maxStaminaBonus = SaveReader_ReadFloat(&reader);
    snapshot->attackBonus = SaveReader_ReadFloat(&reader);
    snapshot->deathCount = SaveReader_ReadInt32(&reader);
    snapshot->crouching = SaveReader_ReadUInt8(&reader) != 0;
    snapshot->hasGlowStick = SaveReader_ReadUInt8(&reader) != 0;
    snapshot->hasRope = SaveReader_ReadUInt8(&reader) != 0;
    snapshot->hasLaserGun = SaveReader_ReadUInt8(&reader) != 0;
    snapshot->hasProtectionSuit = SaveReader_ReadUInt8(&reader) != 0;
    snapshot->hasSignalAmplifier = SaveReader_ReadUInt8(&reader) != 0;
    snapshot->hasFieldCamp = SaveReader_ReadUInt8(&reader) != 0;

    for (index = 0; index < PLAYER_STATUS_COUNT; index++) {
        snapshot->statuses[index].active = SaveReader_ReadUInt8(&reader) != 0;
        snapshot->statuses[index].level = SaveReader_ReadInt32(&reader);
        snapshot->statuses[index].remainingTime = SaveReader_ReadFloat(&reader);
        snapshot->statuses[index].magnitude = SaveReader_ReadFloat(&reader);
    }

    for (index = 0; index < RESOURCE_COUNT; index++) {
        snapshot->resources[index] = SaveReader_ReadInt32(&reader);
    }

    snapshot->stage = SaveReader_ReadInt32(&reader);
    snapshot->dayCount = SaveReader_ReadInt32(&reader);
    snapshot->phase = SaveReader_ReadInt32(&reader);
    snapshot->currentEvent = SaveReader_ReadInt32(&reader);
    snapshot->cycleTimer = SaveReader_ReadFloat(&reader);
    snapshot->elapsedSeconds = SaveReader_ReadFloat(&reader);
    snapshot->oxygenRepairLevel = SaveReader_ReadInt32(&reader);
    snapshot->commRepairLevel = SaveReader_ReadInt32(&reader);
    snapshot->energyRepairLevel = SaveReader_ReadInt32(&reader);
    snapshot->crashClueFound = SaveReader_ReadUInt8(&reader) != 0;
    snapshot->amplifierUnlocked = SaveReader_ReadUInt8(&reader) != 0;
    snapshot->bossDefeated = SaveReader_ReadUInt8(&reader) != 0;
    snapshot->signalTowerActivated = SaveReader_ReadUInt8(&reader) != 0;
    snapshot->westW1Started = SaveReader_ReadUInt8(&reader) != 0;
    snapshot->westW1Completed = SaveReader_ReadUInt8(&reader) != 0;
    snapshot->westW2Started = SaveReader_ReadUInt8(&reader) != 0;
    snapshot->westW2Completed = SaveReader_ReadUInt8(&reader) != 0;
    snapshot->westW3Started = SaveReader_ReadUInt8(&reader) != 0;
    snapshot->westW3Completed = SaveReader_ReadUInt8(&reader) != 0;
    snapshot->westW4Started = SaveReader_ReadUInt8(&reader) != 0;
    snapshot->westW4Completed = SaveReader_ReadUInt8(&reader) != 0;
    snapshot->westW5Started = SaveReader_ReadUInt8(&reader) != 0;
    snapshot->westW5Completed = SaveReader_ReadUInt8(&reader) != 0;
    snapshot->southS1Started = SaveReader_ReadUInt8(&reader) != 0;
    snapshot->southS1Completed = SaveReader_ReadUInt8(&reader) != 0;
    snapshot->southS2Started = SaveReader_ReadUInt8(&reader) != 0;
    snapshot->southS2Completed = SaveReader_ReadUInt8(&reader) != 0;
    snapshot->southS3Started = SaveReader_ReadUInt8(&reader) != 0;
    snapshot->southS3Completed = SaveReader_ReadUInt8(&reader) != 0;
    snapshot->southS4Started = SaveReader_ReadUInt8(&reader) != 0;
    snapshot->southS4Completed = SaveReader_ReadUInt8(&reader) != 0;
    snapshot->southS5Started = SaveReader_ReadUInt8(&reader) != 0;
    snapshot->southS5Completed = SaveReader_ReadUInt8(&reader) != 0;
    snapshot->monolithActivated[0] = SaveReader_ReadUInt8(&reader) != 0;
    snapshot->monolithActivated[1] = SaveReader_ReadUInt8(&reader) != 0;
    snapshot->monolithActivated[2] = SaveReader_ReadUInt8(&reader) != 0;
    snapshot->monolithsLit = SaveReader_ReadInt32(&reader);
    snapshot->ending = SaveReader_ReadInt32(&reader);
    snapshot->campPlaced = SaveReader_ReadUInt8(&reader) != 0;
    snapshot->campX = SaveReader_ReadInt32(&reader);
    snapshot->campY = SaveReader_ReadInt32(&reader);
    snapshot->communicatorUnlocked = SaveReader_ReadUInt8(&reader) != 0;
    snapshot->clearedDynamicTileCount = SaveReader_ReadInt32(&reader);

    for (index = 0; index < MAX_RESOURCE_NODES; index++) {
        snapshot->nodes[index].active = SaveReader_ReadUInt8(&reader) != 0;
        snapshot->nodes[index].respawnsRemaining = SaveReader_ReadInt32(&reader);
    }
    for (index = 0; index < MAX_MONSTERS; index++) {
        snapshot->monsters[index].active = SaveReader_ReadUInt8(&reader) != 0;
        snapshot->monsters[index].gridX = SaveReader_ReadInt32(&reader);
        snapshot->monsters[index].gridY = SaveReader_ReadInt32(&reader);
        snapshot->monsters[index].health = SaveReader_ReadFloat(&reader);
        snapshot->monsters[index].phaseTriggered = SaveReader_ReadUInt8(&reader) != 0;
    }
    for (index = 0; index < MAX_LOGS; index++) {
        snapshot->logs[index].collected = SaveReader_ReadUInt8(&reader) != 0;
    }
    for (index = 0; index < SAVE_DYNAMIC_TILE_MAX; index++) {
        snapshot->clearedDynamicTileX[index] = SaveReader_ReadInt16(&reader);
        snapshot->clearedDynamicTileY[index] = SaveReader_ReadInt16(&reader);
    }

    return !reader.failed && reader.offset == fileSize;
}

static bool DecodeSnapshotV12Buffer(SaveSnapshot *snapshot, const uint8_t *buffer, size_t fileSize) {
    SaveBufferReader reader;
    char magic[8];
    int index;

    if (snapshot == NULL || buffer == NULL) {
        return false;
    }

    std::memset(snapshot, 0, sizeof(*snapshot));
    SaveReader_Init(&reader, buffer, fileSize);
    std::memset(magic, 0, sizeof(magic));
    SaveReader_ReadBytes(&reader, magic, sizeof(magic));
    if (reader.failed || std::memcmp(magic, SAVE_MAGIC_V12, 7) != 0) {
        return false;
    }

    snapshot->gridX = SaveReader_ReadInt32(&reader);
    snapshot->gridY = SaveReader_ReadInt32(&reader);
    snapshot->facingX = SaveReader_ReadInt32(&reader);
    snapshot->facingY = SaveReader_ReadInt32(&reader);
    snapshot->health = SaveReader_ReadFloat(&reader);
    snapshot->stamina = SaveReader_ReadFloat(&reader);
    snapshot->pressure = SaveReader_ReadFloat(&reader);
    snapshot->oxygen = SaveReader_ReadFloat(&reader);
    snapshot->poison = SaveReader_ReadFloat(&reader);
    snapshot->maxHealthBonus = SaveReader_ReadFloat(&reader);
    snapshot->maxStaminaBonus = SaveReader_ReadFloat(&reader);
    snapshot->attackBonus = SaveReader_ReadFloat(&reader);
    snapshot->deathCount = SaveReader_ReadInt32(&reader);
    snapshot->crouching = SaveReader_ReadUInt8(&reader) != 0;
    snapshot->hasGlowStick = SaveReader_ReadUInt8(&reader) != 0;
    snapshot->hasRope = SaveReader_ReadUInt8(&reader) != 0;
    snapshot->hasLaserGun = SaveReader_ReadUInt8(&reader) != 0;
    snapshot->hasProtectionSuit = SaveReader_ReadUInt8(&reader) != 0;
    snapshot->hasSignalAmplifier = SaveReader_ReadUInt8(&reader) != 0;
    snapshot->hasFieldCamp = SaveReader_ReadUInt8(&reader) != 0;

    for (index = 0; index < PLAYER_STATUS_COUNT; index++) {
        snapshot->statuses[index].active = SaveReader_ReadUInt8(&reader) != 0;
        snapshot->statuses[index].level = SaveReader_ReadInt32(&reader);
        snapshot->statuses[index].remainingTime = SaveReader_ReadFloat(&reader);
        snapshot->statuses[index].magnitude = SaveReader_ReadFloat(&reader);
    }

    for (index = 0; index < RESOURCE_COUNT; index++) {
        snapshot->resources[index] = SaveReader_ReadInt32(&reader);
    }

    snapshot->stage = SaveReader_ReadInt32(&reader);
    snapshot->dayCount = SaveReader_ReadInt32(&reader);
    snapshot->phase = SaveReader_ReadInt32(&reader);
    snapshot->currentEvent = SaveReader_ReadInt32(&reader);
    snapshot->cycleTimer = SaveReader_ReadFloat(&reader);
    snapshot->elapsedSeconds = SaveReader_ReadFloat(&reader);
    snapshot->oxygenRepairLevel = SaveReader_ReadInt32(&reader);
    snapshot->commRepairLevel = SaveReader_ReadInt32(&reader);
    snapshot->energyRepairLevel = SaveReader_ReadInt32(&reader);
    snapshot->crashClueFound = SaveReader_ReadUInt8(&reader) != 0;
    snapshot->amplifierUnlocked = SaveReader_ReadUInt8(&reader) != 0;
    snapshot->bossDefeated = SaveReader_ReadUInt8(&reader) != 0;
    snapshot->signalTowerActivated = SaveReader_ReadUInt8(&reader) != 0;
    snapshot->selectedEndingRoute = SaveReader_ReadInt32(&reader);
    snapshot->westW1Started = SaveReader_ReadUInt8(&reader) != 0;
    snapshot->westW1Completed = SaveReader_ReadUInt8(&reader) != 0;
    snapshot->westW2Started = SaveReader_ReadUInt8(&reader) != 0;
    snapshot->westW2Completed = SaveReader_ReadUInt8(&reader) != 0;
    snapshot->westW3Started = SaveReader_ReadUInt8(&reader) != 0;
    snapshot->westW3Completed = SaveReader_ReadUInt8(&reader) != 0;
    snapshot->westW4Started = SaveReader_ReadUInt8(&reader) != 0;
    snapshot->westW4Completed = SaveReader_ReadUInt8(&reader) != 0;
    snapshot->westW5Started = SaveReader_ReadUInt8(&reader) != 0;
    snapshot->westW5Completed = SaveReader_ReadUInt8(&reader) != 0;
    snapshot->southS1Started = SaveReader_ReadUInt8(&reader) != 0;
    snapshot->southS1Completed = SaveReader_ReadUInt8(&reader) != 0;
    snapshot->southS2Started = SaveReader_ReadUInt8(&reader) != 0;
    snapshot->southS2Completed = SaveReader_ReadUInt8(&reader) != 0;
    snapshot->southS3Started = SaveReader_ReadUInt8(&reader) != 0;
    snapshot->southS3Completed = SaveReader_ReadUInt8(&reader) != 0;
    snapshot->southS4Started = SaveReader_ReadUInt8(&reader) != 0;
    snapshot->southS4Completed = SaveReader_ReadUInt8(&reader) != 0;
    snapshot->southS5Started = SaveReader_ReadUInt8(&reader) != 0;
    snapshot->southS5Completed = SaveReader_ReadUInt8(&reader) != 0;
    snapshot->monolithActivated[0] = SaveReader_ReadUInt8(&reader) != 0;
    snapshot->monolithActivated[1] = SaveReader_ReadUInt8(&reader) != 0;
    snapshot->monolithActivated[2] = SaveReader_ReadUInt8(&reader) != 0;
    snapshot->monolithsLit = SaveReader_ReadInt32(&reader);
    snapshot->ending = SaveReader_ReadInt32(&reader);
    snapshot->campPlaced = SaveReader_ReadUInt8(&reader) != 0;
    snapshot->campX = SaveReader_ReadInt32(&reader);
    snapshot->campY = SaveReader_ReadInt32(&reader);
    snapshot->communicatorUnlocked = SaveReader_ReadUInt8(&reader) != 0;
    snapshot->clearedDynamicTileCount = SaveReader_ReadInt32(&reader);

    for (index = 0; index < MAX_RESOURCE_NODES; index++) {
        snapshot->nodes[index].active = SaveReader_ReadUInt8(&reader) != 0;
        snapshot->nodes[index].respawnsRemaining = SaveReader_ReadInt32(&reader);
    }
    for (index = 0; index < MAX_MONSTERS; index++) {
        snapshot->monsters[index].active = SaveReader_ReadUInt8(&reader) != 0;
        snapshot->monsters[index].gridX = SaveReader_ReadInt32(&reader);
        snapshot->monsters[index].gridY = SaveReader_ReadInt32(&reader);
        snapshot->monsters[index].health = SaveReader_ReadFloat(&reader);
        snapshot->monsters[index].phaseTriggered = SaveReader_ReadUInt8(&reader) != 0;
    }
    for (index = 0; index < MAX_LOGS; index++) {
        snapshot->logs[index].collected = SaveReader_ReadUInt8(&reader) != 0;
    }
    for (index = 0; index < SAVE_DYNAMIC_TILE_MAX; index++) {
        snapshot->clearedDynamicTileX[index] = SaveReader_ReadInt16(&reader);
        snapshot->clearedDynamicTileY[index] = SaveReader_ReadInt16(&reader);
    }

    return !reader.failed && reader.offset == fileSize;
}

bool SaveInternal_IsNativeSaveMagic(const char *magic) {
    return magic != NULL
        && (std::memcmp(magic, SAVE_MAGIC_CURRENT, 7) == 0
            || std::memcmp(magic, SAVE_MAGIC_V11, 7) == 0
            || std::memcmp(magic, SAVE_MAGIC_V10, 7) == 0
            || std::memcmp(magic, SAVE_MAGIC_V9, 7) == 0
            || std::memcmp(magic, SAVE_MAGIC_V8, 7) == 0
            || std::memcmp(magic, SAVE_MAGIC_V7, 7) == 0
            || std::memcmp(magic, SAVE_MAGIC_V6, 7) == 0
            || std::memcmp(magic, SAVE_MAGIC_V5, 7) == 0);
}

bool SaveInternal_EncodeSnapshotBuffer(const SaveSnapshot *snapshot, uint8_t *buffer, size_t bufferSize, size_t *bytesWritten) {
    SaveBufferWriter writer;
    int index;

    if (snapshot == NULL || buffer == NULL) {
        return false;
    }

    std::memset(buffer, 0, bufferSize);
    SaveWriter_Init(&writer, buffer, bufferSize);
    SaveWriter_WriteBytes(&writer, SAVE_MAGIC_V12, 8);
    SaveWriter_WriteInt32(&writer, snapshot->gridX);
    SaveWriter_WriteInt32(&writer, snapshot->gridY);
    SaveWriter_WriteInt32(&writer, snapshot->facingX);
    SaveWriter_WriteInt32(&writer, snapshot->facingY);
    SaveWriter_WriteFloat(&writer, snapshot->health);
    SaveWriter_WriteFloat(&writer, snapshot->stamina);
    SaveWriter_WriteFloat(&writer, snapshot->pressure);
    SaveWriter_WriteFloat(&writer, snapshot->oxygen);
    SaveWriter_WriteFloat(&writer, snapshot->poison);
    SaveWriter_WriteFloat(&writer, snapshot->maxHealthBonus);
    SaveWriter_WriteFloat(&writer, snapshot->maxStaminaBonus);
    SaveWriter_WriteFloat(&writer, snapshot->attackBonus);
    SaveWriter_WriteInt32(&writer, snapshot->deathCount);
    SaveWriter_WriteUInt8(&writer, snapshot->crouching ? 1u : 0u);
    SaveWriter_WriteUInt8(&writer, snapshot->hasGlowStick ? 1u : 0u);
    SaveWriter_WriteUInt8(&writer, snapshot->hasRope ? 1u : 0u);
    SaveWriter_WriteUInt8(&writer, snapshot->hasLaserGun ? 1u : 0u);
    SaveWriter_WriteUInt8(&writer, snapshot->hasProtectionSuit ? 1u : 0u);
    SaveWriter_WriteUInt8(&writer, snapshot->hasSignalAmplifier ? 1u : 0u);
    SaveWriter_WriteUInt8(&writer, snapshot->hasFieldCamp ? 1u : 0u);

    for (index = 0; index < PLAYER_STATUS_COUNT; index++) {
        SaveWriter_WriteUInt8(&writer, snapshot->statuses[index].active ? 1u : 0u);
        SaveWriter_WriteInt32(&writer, snapshot->statuses[index].level);
        SaveWriter_WriteFloat(&writer, snapshot->statuses[index].remainingTime);
        SaveWriter_WriteFloat(&writer, snapshot->statuses[index].magnitude);
    }

    for (index = 0; index < RESOURCE_COUNT; index++) {
        SaveWriter_WriteInt32(&writer, snapshot->resources[index]);
    }

    SaveWriter_WriteInt32(&writer, snapshot->stage);
    SaveWriter_WriteInt32(&writer, snapshot->dayCount);
    SaveWriter_WriteInt32(&writer, snapshot->phase);
    SaveWriter_WriteInt32(&writer, snapshot->currentEvent);
    SaveWriter_WriteFloat(&writer, snapshot->cycleTimer);
    SaveWriter_WriteFloat(&writer, snapshot->elapsedSeconds);
    SaveWriter_WriteInt32(&writer, snapshot->oxygenRepairLevel);
    SaveWriter_WriteInt32(&writer, snapshot->commRepairLevel);
    SaveWriter_WriteInt32(&writer, snapshot->energyRepairLevel);
    SaveWriter_WriteUInt8(&writer, snapshot->crashClueFound ? 1u : 0u);
    SaveWriter_WriteUInt8(&writer, snapshot->amplifierUnlocked ? 1u : 0u);
    SaveWriter_WriteUInt8(&writer, snapshot->bossDefeated ? 1u : 0u);
    SaveWriter_WriteUInt8(&writer, snapshot->signalTowerActivated ? 1u : 0u);
    SaveWriter_WriteInt32(&writer, snapshot->selectedEndingRoute);
    SaveWriter_WriteUInt8(&writer, snapshot->westW1Started ? 1u : 0u);
    SaveWriter_WriteUInt8(&writer, snapshot->westW1Completed ? 1u : 0u);
    SaveWriter_WriteUInt8(&writer, snapshot->westW2Started ? 1u : 0u);
    SaveWriter_WriteUInt8(&writer, snapshot->westW2Completed ? 1u : 0u);
    SaveWriter_WriteUInt8(&writer, snapshot->westW3Started ? 1u : 0u);
    SaveWriter_WriteUInt8(&writer, snapshot->westW3Completed ? 1u : 0u);
    SaveWriter_WriteUInt8(&writer, snapshot->westW4Started ? 1u : 0u);
    SaveWriter_WriteUInt8(&writer, snapshot->westW4Completed ? 1u : 0u);
    SaveWriter_WriteUInt8(&writer, snapshot->westW5Started ? 1u : 0u);
    SaveWriter_WriteUInt8(&writer, snapshot->westW5Completed ? 1u : 0u);
    SaveWriter_WriteUInt8(&writer, snapshot->southS1Started ? 1u : 0u);
    SaveWriter_WriteUInt8(&writer, snapshot->southS1Completed ? 1u : 0u);
    SaveWriter_WriteUInt8(&writer, snapshot->southS2Started ? 1u : 0u);
    SaveWriter_WriteUInt8(&writer, snapshot->southS2Completed ? 1u : 0u);
    SaveWriter_WriteUInt8(&writer, snapshot->southS3Started ? 1u : 0u);
    SaveWriter_WriteUInt8(&writer, snapshot->southS3Completed ? 1u : 0u);
    SaveWriter_WriteUInt8(&writer, snapshot->southS4Started ? 1u : 0u);
    SaveWriter_WriteUInt8(&writer, snapshot->southS4Completed ? 1u : 0u);
    SaveWriter_WriteUInt8(&writer, snapshot->southS5Started ? 1u : 0u);
    SaveWriter_WriteUInt8(&writer, snapshot->southS5Completed ? 1u : 0u);
    SaveWriter_WriteUInt8(&writer, snapshot->monolithActivated[0] ? 1u : 0u);
    SaveWriter_WriteUInt8(&writer, snapshot->monolithActivated[1] ? 1u : 0u);
    SaveWriter_WriteUInt8(&writer, snapshot->monolithActivated[2] ? 1u : 0u);
    SaveWriter_WriteInt32(&writer, snapshot->monolithsLit);
    SaveWriter_WriteInt32(&writer, snapshot->ending);
    SaveWriter_WriteUInt8(&writer, snapshot->campPlaced ? 1u : 0u);
    SaveWriter_WriteInt32(&writer, snapshot->campX);
    SaveWriter_WriteInt32(&writer, snapshot->campY);
    SaveWriter_WriteUInt8(&writer, snapshot->communicatorUnlocked ? 1u : 0u);
    SaveWriter_WriteInt32(&writer, snapshot->clearedDynamicTileCount);

    for (index = 0; index < MAX_RESOURCE_NODES; index++) {
        SaveWriter_WriteUInt8(&writer, snapshot->nodes[index].active ? 1u : 0u);
        SaveWriter_WriteInt32(&writer, snapshot->nodes[index].respawnsRemaining);
    }
    for (index = 0; index < MAX_MONSTERS; index++) {
        SaveWriter_WriteUInt8(&writer, snapshot->monsters[index].active ? 1u : 0u);
        SaveWriter_WriteInt32(&writer, snapshot->monsters[index].gridX);
        SaveWriter_WriteInt32(&writer, snapshot->monsters[index].gridY);
        SaveWriter_WriteFloat(&writer, snapshot->monsters[index].health);
        SaveWriter_WriteUInt8(&writer, snapshot->monsters[index].phaseTriggered ? 1u : 0u);
    }
    for (index = 0; index < MAX_LOGS; index++) {
        SaveWriter_WriteUInt8(&writer, snapshot->logs[index].collected ? 1u : 0u);
    }
    for (index = 0; index < SAVE_DYNAMIC_TILE_MAX; index++) {
        SaveWriter_WriteInt16(&writer, (int16_t)snapshot->clearedDynamicTileX[index]);
        SaveWriter_WriteInt16(&writer, (int16_t)snapshot->clearedDynamicTileY[index]);
    }

    if (writer.failed) {
        return false;
    }

    if (bytesWritten != NULL) {
        *bytesWritten = writer.offset;
    }
    return true;
}

bool SaveInternal_DecodeSnapshotBuffer(SaveSnapshot *snapshot, const uint8_t *buffer, size_t fileSize) {
    SaveBufferReader reader;
    char magic[8];

    if (snapshot == NULL || buffer == NULL) {
        return false;
    }

    SaveReader_Init(&reader, buffer, fileSize);
    std::memset(magic, 0, sizeof(magic));
    SaveReader_ReadBytes(&reader, magic, sizeof(magic));
    if (reader.failed) {
        return false;
    }

    if (std::memcmp(magic, SAVE_MAGIC_V12, 7) == 0) {
        return DecodeSnapshotV12Buffer(snapshot, buffer, fileSize);
    }
    if (std::memcmp(magic, SAVE_MAGIC_V11, 7) == 0) {
        return DecodeSnapshotV11Buffer(snapshot, buffer, fileSize);
    }
    if (std::memcmp(magic, SAVE_MAGIC_V10, 7) == 0) {
        return DecodeSnapshotV10Buffer(snapshot, buffer, fileSize);
    }
    if (std::memcmp(magic, SAVE_MAGIC_V9, 7) == 0) {
        return DecodeSnapshotV9Buffer(snapshot, buffer, fileSize);
    }
    if (std::memcmp(magic, SAVE_MAGIC_V8, 7) == 0) {
        return DecodeSnapshotV8Buffer(snapshot, buffer, fileSize);
    }
    if (std::memcmp(magic, SAVE_MAGIC_V7, 7) == 0) {
        return DecodeSnapshotV7Buffer(snapshot, buffer, fileSize);
    }
    if (std::memcmp(magic, SAVE_MAGIC_V6, 7) == 0) {
        return DecodeSnapshotV6Buffer(snapshot, buffer, fileSize);
    }
    if (std::memcmp(magic, SAVE_MAGIC_V5, 7) == 0) {
        return DecodeSnapshotV5Buffer(snapshot, buffer, fileSize);
    }

    return false;
}
