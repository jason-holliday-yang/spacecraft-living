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
    (void)SaveReader_ReadUInt8(&reader);
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
    (void)SaveReader_ReadUInt8(&reader);
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
    (void)SaveReader_ReadUInt8(&reader);
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
    (void)SaveReader_ReadUInt8(&reader);
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
    (void)SaveReader_ReadUInt8(&reader);
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
    if (reader.failed || std::memcmp(magic, SAVE_MAGIC_V10, 8) != 0) {
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
    (void)SaveReader_ReadUInt8(&reader);
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
    if (reader.failed || std::memcmp(magic, SAVE_MAGIC_V11, 8) != 0) {
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
    (void)SaveReader_ReadUInt8(&reader);
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
    if (reader.failed || std::memcmp(magic, SAVE_MAGIC_V12, 8) != 0) {
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
    (void)SaveReader_ReadUInt8(&reader);
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

static bool DecodeSnapshotV13Buffer(SaveSnapshot *snapshot, const uint8_t *buffer, size_t fileSize) {
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
    if (reader.failed || std::memcmp(magic, SAVE_MAGIC_V13, 8) != 0) {
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
    (void)SaveReader_ReadUInt8(&reader);
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
    snapshot->endingArchiveReviewed = SaveReader_ReadUInt8(&reader) != 0;
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

static bool DecodeSnapshotV15Buffer(SaveSnapshot *snapshot, const uint8_t *buffer, size_t fileSize) {
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
    if (reader.failed || std::memcmp(magic, SAVE_MAGIC_V15, 8) != 0) {
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
    (void)SaveReader_ReadUInt8(&reader);
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
    snapshot->endingArchiveReviewed = SaveReader_ReadUInt8(&reader) != 0;
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
    for (index = 0; index < STORY_MAIN_SCENE_COUNT; index++) {
        snapshot->storyMainSceneShown[index] = SaveReader_ReadUInt8(&reader) != 0;
    }
    for (index = 0; index < SAVE_DYNAMIC_TILE_MAX; index++) {
        snapshot->clearedDynamicTileX[index] = SaveReader_ReadInt32(&reader);
        snapshot->clearedDynamicTileY[index] = SaveReader_ReadInt32(&reader);
    }

    snapshot->mapWidth = SaveReader_ReadInt32(&reader);
    snapshot->mapHeight = SaveReader_ReadInt32(&reader);

    return !reader.failed && reader.offset == fileSize;
}


static bool DecodeSnapshotV14Buffer(SaveSnapshot *snapshot, const uint8_t *buffer, size_t fileSize) {
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
    if (reader.failed || std::memcmp(magic, SAVE_MAGIC_V14, 8) != 0) {
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
    (void)SaveReader_ReadUInt8(&reader);
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
    snapshot->endingArchiveReviewed = SaveReader_ReadUInt8(&reader) != 0;
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
    for (index = 0; index < STORY_MAIN_SCENE_COUNT; index++) {
        snapshot->storyMainSceneShown[index] = SaveReader_ReadUInt8(&reader) != 0;
    }
    for (index = 0; index < SAVE_DYNAMIC_TILE_MAX; index++) {
        snapshot->clearedDynamicTileX[index] = SaveReader_ReadInt16(&reader);
        snapshot->clearedDynamicTileY[index] = SaveReader_ReadInt16(&reader);
    }

    return !reader.failed && reader.offset == fileSize;
}

bool SaveInternal_IsNativeSaveMagic(const char *magic) {
    return magic != NULL
        && (std::memcmp(magic, SAVE_MAGIC_CURRENT, 8) == 0
            || std::memcmp(magic, SAVE_MAGIC_V15, 8) == 0
            || std::memcmp(magic, SAVE_MAGIC_V14, 8) == 0
            || std::memcmp(magic, SAVE_MAGIC_V13, 8) == 0
            || std::memcmp(magic, SAVE_MAGIC_V12, 8) == 0
            || std::memcmp(magic, SAVE_MAGIC_V11, 8) == 0
            || std::memcmp(magic, SAVE_MAGIC_V10, 8) == 0
            || std::memcmp(magic, SAVE_MAGIC_V9, 7) == 0
            || std::memcmp(magic, SAVE_MAGIC_V8, 7) == 0
            || std::memcmp(magic, SAVE_MAGIC_V7, 7) == 0
            || std::memcmp(magic, SAVE_MAGIC_V6, 7) == 0
            || std::memcmp(magic, SAVE_MAGIC_V5, 7) == 0);
}

static bool EncodeSnapshotV15Buffer(const SaveSnapshot *snapshot, uint8_t *buffer, size_t bufferSize, size_t *bytesWritten) {
    SaveBufferWriter writer;
    int index;

    if (snapshot == NULL || buffer == NULL) {
        return false;
    }

    std::memset(buffer, 0, bufferSize);
    SaveWriter_Init(&writer, buffer, bufferSize);
    SaveWriter_WriteBytes(&writer, SAVE_MAGIC_V15, 8);
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
    SaveWriter_WriteUInt8(&writer, 0u);
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
    SaveWriter_WriteUInt8(&writer, snapshot->endingArchiveReviewed ? 1u : 0u);
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
    for (index = 0; index < STORY_MAIN_SCENE_COUNT; index++) {
        SaveWriter_WriteUInt8(&writer, snapshot->storyMainSceneShown[index] ? 1u : 0u);
    }
    for (index = 0; index < SAVE_DYNAMIC_TILE_MAX; index++) {
        SaveWriter_WriteInt32(&writer, snapshot->clearedDynamicTileX[index]);
        SaveWriter_WriteInt32(&writer, snapshot->clearedDynamicTileY[index]);
    }
    SaveWriter_WriteInt32(&writer, snapshot->mapWidth);
    SaveWriter_WriteInt32(&writer, snapshot->mapHeight);

    if (writer.failed) {
        return false;
    }

    if (bytesWritten != NULL) {
        *bytesWritten = writer.offset;
    }
    return true;
}

bool SaveInternal_EncodeLegacyV15Buffer(const SaveSnapshot *snapshot, uint8_t *buffer, size_t bufferSize, size_t *bytesWritten) {
    return EncodeSnapshotV15Buffer(snapshot, buffer, bufferSize, bytesWritten);
}

static void WriteFixedString(SaveBufferWriter *writer, const char *value, size_t size) {
    char buffer[MAP_SOURCE_PATH_MAX];
    if (size > sizeof(buffer)) {
        writer->failed = true;
        return;
    }
    std::memset(buffer, 0, size);
    if (value != NULL) {
        std::strncpy(buffer, value, size - 1);
    }
    SaveWriter_WriteBytes(writer, buffer, size);
}

static void ReadFixedString(SaveBufferReader *reader, char *value, size_t size) {
    SaveReader_ReadBytes(reader, value, size);
    if (size > 0) value[size - 1] = '\0';
}

bool SaveInternal_EncodeSnapshotBuffer(const SaveSnapshot *snapshot, uint8_t *buffer, size_t bufferSize, size_t *bytesWritten) {
    SaveBufferWriter writer;
    size_t prefixSize;
    int mapIndex;
    int index;

    if (!EncodeSnapshotV15Buffer(snapshot, buffer, bufferSize, &prefixSize)) return false;
    std::memcpy(buffer, SAVE_MAGIC_V16, 8);
    SaveWriter_Init(&writer, buffer, bufferSize);
    writer.offset = prefixSize;
    WriteFixedString(&writer, snapshot->currentMapId, MAP_ID_MAX);
    SaveWriter_WriteInt32(&writer, snapshot->mapStateCount);
    for (mapIndex = 0; mapIndex < snapshot->mapStateCount && mapIndex < SAVE_MAP_STATE_MAX; mapIndex++) {
        const SavedMapStateSnapshot *state = &snapshot->mapStates[mapIndex];
        WriteFixedString(&writer, state->mapId, MAP_ID_MAX);
        SaveWriter_WriteInt32(&writer, state->contentVersion);
        SaveWriter_WriteInt32(&writer, state->mapWidth);
        SaveWriter_WriteInt32(&writer, state->mapHeight);
        SaveWriter_WriteInt32(&writer, state->unlockCount);
        for (index = 0; index < state->unlockCount && index < MAX_MAP_UNLOCKS; index++) {
            WriteFixedString(&writer, state->unlocks[index].unlockId, MAP_UNLOCK_ID_MAX);
            SaveWriter_WriteUInt8(&writer, state->unlocks[index].open ? 1u : 0u);
        }
        SaveWriter_WriteInt32(&writer, state->clearedDynamicTileCount);
        for (index = 0; index < state->clearedDynamicTileCount && index < SAVE_DYNAMIC_TILE_MAX; index++) {
            SaveWriter_WriteInt32(&writer, state->clearedDynamicTileX[index]);
            SaveWriter_WriteInt32(&writer, state->clearedDynamicTileY[index]);
        }
        SaveWriter_WriteUInt8(&writer, state->campPlaced ? 1u : 0u);
        SaveWriter_WriteInt32(&writer, state->campX);
        SaveWriter_WriteInt32(&writer, state->campY);
        SaveWriter_WriteInt32(&writer, state->nodeCount);
        for (index = 0; index < state->nodeCount && index < MAX_RESOURCE_NODES; index++) {
            SaveWriter_WriteUInt8(&writer, state->nodes[index].active ? 1u : 0u);
            SaveWriter_WriteInt32(&writer, state->nodes[index].respawnsRemaining);
        }
        SaveWriter_WriteInt32(&writer, state->monsterCount);
        for (index = 0; index < state->monsterCount && index < MAX_MONSTERS; index++) {
            SaveWriter_WriteUInt8(&writer, state->monsters[index].active ? 1u : 0u);
            SaveWriter_WriteInt32(&writer, state->monsters[index].gridX);
            SaveWriter_WriteInt32(&writer, state->monsters[index].gridY);
            SaveWriter_WriteFloat(&writer, state->monsters[index].health);
            SaveWriter_WriteUInt8(&writer, state->monsters[index].phaseTriggered ? 1u : 0u);
        }
        SaveWriter_WriteInt32(&writer, state->logCount);
        for (index = 0; index < state->logCount && index < MAX_LOGS; index++) {
            SaveWriter_WriteUInt8(&writer, state->logs[index].collected ? 1u : 0u);
        }
    }
    if (writer.failed || snapshot->mapStateCount < 0 || snapshot->mapStateCount > SAVE_MAP_STATE_MAX) return false;
    if (bytesWritten != NULL) *bytesWritten = writer.offset;
    return true;
}

static bool DecodeSnapshotV16Buffer(SaveSnapshot *snapshot, const uint8_t *buffer, size_t fileSize) {
    uint8_t prefix[SAVE_INTERNAL_MAX_SAVE_FILE_SIZE];
    SaveSnapshot zeroSnapshot;
    size_t prefixSize;
    SaveBufferReader reader;
    int mapIndex;
    int index;

    std::memset(&zeroSnapshot, 0, sizeof(zeroSnapshot));
    if (!EncodeSnapshotV15Buffer(&zeroSnapshot, prefix, sizeof(prefix), &prefixSize)
        || fileSize < prefixSize || prefixSize > sizeof(prefix)) return false;
    std::memcpy(prefix, buffer, prefixSize);
    std::memcpy(prefix, SAVE_MAGIC_V15, 8);
    if (!DecodeSnapshotV15Buffer(snapshot, prefix, prefixSize)) return false;

    SaveReader_Init(&reader, buffer, fileSize);
    reader.offset = prefixSize;
    ReadFixedString(&reader, snapshot->currentMapId, MAP_ID_MAX);
    snapshot->mapStateCount = SaveReader_ReadInt32(&reader);
    if (snapshot->mapStateCount < 0 || snapshot->mapStateCount > SAVE_MAP_STATE_MAX) return false;
    for (mapIndex = 0; mapIndex < snapshot->mapStateCount; mapIndex++) {
        SavedMapStateSnapshot *state = &snapshot->mapStates[mapIndex];
        ReadFixedString(&reader, state->mapId, MAP_ID_MAX);
        state->contentVersion = SaveReader_ReadInt32(&reader);
        state->mapWidth = SaveReader_ReadInt32(&reader);
        state->mapHeight = SaveReader_ReadInt32(&reader);
        state->unlockCount = SaveReader_ReadInt32(&reader);
        if (state->unlockCount < 0 || state->unlockCount > MAX_MAP_UNLOCKS) return false;
        for (index = 0; index < state->unlockCount; index++) {
            ReadFixedString(&reader, state->unlocks[index].unlockId, MAP_UNLOCK_ID_MAX);
            state->unlocks[index].open = SaveReader_ReadUInt8(&reader) != 0;
        }
        state->clearedDynamicTileCount = SaveReader_ReadInt32(&reader);
        if (state->clearedDynamicTileCount < 0 || state->clearedDynamicTileCount > SAVE_DYNAMIC_TILE_MAX) return false;
        for (index = 0; index < state->clearedDynamicTileCount; index++) {
            state->clearedDynamicTileX[index] = SaveReader_ReadInt32(&reader);
            state->clearedDynamicTileY[index] = SaveReader_ReadInt32(&reader);
        }
        state->campPlaced = SaveReader_ReadUInt8(&reader) != 0;
        state->campX = SaveReader_ReadInt32(&reader);
        state->campY = SaveReader_ReadInt32(&reader);
        state->nodeCount = SaveReader_ReadInt32(&reader);
        if (state->nodeCount < 0 || state->nodeCount > MAX_RESOURCE_NODES) return false;
        for (index = 0; index < state->nodeCount; index++) {
            state->nodes[index].active = SaveReader_ReadUInt8(&reader) != 0;
            state->nodes[index].respawnsRemaining = SaveReader_ReadInt32(&reader);
        }
        state->monsterCount = SaveReader_ReadInt32(&reader);
        if (state->monsterCount < 0 || state->monsterCount > MAX_MONSTERS) return false;
        for (index = 0; index < state->monsterCount; index++) {
            state->monsters[index].active = SaveReader_ReadUInt8(&reader) != 0;
            state->monsters[index].gridX = SaveReader_ReadInt32(&reader);
            state->monsters[index].gridY = SaveReader_ReadInt32(&reader);
            state->monsters[index].health = SaveReader_ReadFloat(&reader);
            state->monsters[index].phaseTriggered = SaveReader_ReadUInt8(&reader) != 0;
        }
        state->logCount = SaveReader_ReadInt32(&reader);
        if (state->logCount < 0 || state->logCount > MAX_LOGS) return false;
        for (index = 0; index < state->logCount; index++) state->logs[index].collected = SaveReader_ReadUInt8(&reader) != 0;
    }
    snapshot->sourceVersion = 16;
    return !reader.failed && reader.offset == fileSize;
}

bool SaveInternal_DecodeSnapshotBuffer(SaveSnapshot *snapshot, const uint8_t *buffer, size_t fileSize) {
    SaveBufferReader reader;
    char magic[8];
    bool decoded = false;
    int version = 0;

    if (snapshot == NULL || buffer == NULL) return false;
    SaveReader_Init(&reader, buffer, fileSize);
    std::memset(magic, 0, sizeof(magic));
    SaveReader_ReadBytes(&reader, magic, sizeof(magic));
    if (reader.failed) return false;

    if (std::memcmp(magic, SAVE_MAGIC_V16, 8) == 0) return DecodeSnapshotV16Buffer(snapshot, buffer, fileSize);
    if (std::memcmp(magic, SAVE_MAGIC_V15, 8) == 0) { decoded = DecodeSnapshotV15Buffer(snapshot, buffer, fileSize); version = 15; }
    else if (std::memcmp(magic, SAVE_MAGIC_V14, 8) == 0) { decoded = DecodeSnapshotV14Buffer(snapshot, buffer, fileSize); version = 14; }
    else if (std::memcmp(magic, SAVE_MAGIC_V13, 8) == 0) { decoded = DecodeSnapshotV13Buffer(snapshot, buffer, fileSize); version = 13; }
    else if (std::memcmp(magic, SAVE_MAGIC_V12, 8) == 0) { decoded = DecodeSnapshotV12Buffer(snapshot, buffer, fileSize); version = 12; }
    else if (std::memcmp(magic, SAVE_MAGIC_V11, 8) == 0) { decoded = DecodeSnapshotV11Buffer(snapshot, buffer, fileSize); version = 11; }
    else if (std::memcmp(magic, SAVE_MAGIC_V10, 8) == 0) { decoded = DecodeSnapshotV10Buffer(snapshot, buffer, fileSize); version = 10; }
    else if (std::memcmp(magic, SAVE_MAGIC_V9, 7) == 0) { decoded = DecodeSnapshotV9Buffer(snapshot, buffer, fileSize); version = 9; }
    else if (std::memcmp(magic, SAVE_MAGIC_V8, 7) == 0) { decoded = DecodeSnapshotV8Buffer(snapshot, buffer, fileSize); version = 8; }
    else if (std::memcmp(magic, SAVE_MAGIC_V7, 7) == 0) { decoded = DecodeSnapshotV7Buffer(snapshot, buffer, fileSize); version = 7; }
    else if (std::memcmp(magic, SAVE_MAGIC_V6, 7) == 0) { decoded = DecodeSnapshotV6Buffer(snapshot, buffer, fileSize); version = 6; }
    else if (std::memcmp(magic, SAVE_MAGIC_V5, 7) == 0) { decoded = DecodeSnapshotV5Buffer(snapshot, buffer, fileSize); version = 5; }
    if (decoded) snapshot->sourceVersion = version;
    return decoded;
}
