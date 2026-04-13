#include "save_legacy_adapter.h"

#include "save_legacy_format.h"
#include "save_legacy_runtime.h"
#include "persistence_platform.h"

#include <cstddef>
#include <cstdint>
#include <cstring>

template <typename T>
static void PopulateSnapshotSharedState(SaveSnapshot *snapshot, const T *data) {
    int index;

    snapshot->gridX = data->gridX;
    snapshot->gridY = data->gridY;
    snapshot->facingX = data->facingX;
    snapshot->facingY = data->facingY;
    snapshot->stamina = data->stamina;
    snapshot->pressure = data->pressure;
    snapshot->oxygen = data->oxygen;
    snapshot->poison = data->poison;
    snapshot->maxStaminaBonus = data->maxStaminaBonus;
    snapshot->attackBonus = data->attackBonus;
    snapshot->deathCount = data->deathCount;
    snapshot->crouching = data->crouching != 0;
    snapshot->hasGlowStick = data->hasGlowStick != 0;
    snapshot->hasRope = data->hasRope != 0;
    snapshot->hasLaserGun = data->hasLaserGun != 0;
    snapshot->hasProtectionSuit = data->hasProtectionSuit != 0;
    snapshot->hasSignalAmplifier = data->hasSignalAmplifier != 0;
    snapshot->hasFieldCamp = data->hasFieldCamp != 0;
    std::memcpy(snapshot->resources, data->resources, sizeof(data->resources));
    snapshot->stage = data->stage;
    snapshot->dayCount = data->dayCount;
    snapshot->phase = data->phase;
    snapshot->currentEvent = data->currentEvent;
    snapshot->cycleTimer = data->cycleTimer;
    snapshot->elapsedSeconds = data->elapsedSeconds;
    snapshot->oxygenRepairLevel = data->oxygenRepairLevel;
    snapshot->commRepairLevel = data->commRepairLevel;
    snapshot->energyRepairLevel = data->energyRepairLevel;
    snapshot->crashClueFound = data->crashClueFound != 0;
    snapshot->amplifierUnlocked = data->amplifierUnlocked != 0;
    snapshot->bossDefeated = data->bossDefeated != 0;
    snapshot->signalTowerActivated = data->signalTowerActivated != 0;
    snapshot->monolithActivated[0] = data->monolithActivated[0] != 0;
    snapshot->monolithActivated[1] = data->monolithActivated[1] != 0;
    snapshot->monolithActivated[2] = data->monolithActivated[2] != 0;
    snapshot->monolithsLit = data->monolithsLit;
    snapshot->ending = data->ending;
    snapshot->campPlaced = data->campPlaced != 0;
    snapshot->campX = data->campX;
    snapshot->campY = data->campY;

    for (index = 0; index < MAX_RESOURCE_NODES; index++) {
        snapshot->nodes[index].active = data->nodeActive[index] != 0;
        snapshot->nodes[index].respawnsRemaining = data->nodeRespawns[index];
    }

    for (index = 0; index < MAX_MONSTERS; index++) {
        snapshot->monsters[index].active = data->monsterActive[index] != 0;
        snapshot->monsters[index].gridX = data->monsterGridX[index];
        snapshot->monsters[index].gridY = data->monsterGridY[index];
        snapshot->monsters[index].health = data->monsterHealth[index];
        snapshot->monsters[index].phaseTriggered = data->monsterPhaseTriggered[index] != 0;
    }
}

template <size_t LogCount>
static void PopulateSnapshotLogs(SaveSnapshot *snapshot, const uint8_t (&logsCollected)[LogCount]) {
    int index;

    for (index = 0; index < MAX_LOGS && index < (int)LogCount; index++) {
        snapshot->logs[index].collected = logsCollected[index] != 0;
    }
}

template <typename T>
static void PopulateSnapshotCommunicatorState(SaveSnapshot *snapshot, const T *data, size_t fileSize) {
    if (fileSize >= offsetof(T, communicatorUnlocked) + sizeof(data->communicatorUnlocked)) {
        snapshot->communicatorUnlocked = data->communicatorUnlocked != 0;
    } else {
        snapshot->communicatorUnlocked = data->oxygenRepairLevel > 0 || data->stage > 1;
    }
}

static void PopulateSnapshotClearedDynamicTiles(SaveSnapshot *snapshot, const SaveLegacyRecordV4 *data, size_t fileSize) {
    int index;

    if (fileSize >= sizeof(*data)) {
        snapshot->clearedDynamicTileCount = data->clearedDynamicTileCount;
        for (index = 0; index < SAVE_DYNAMIC_TILE_MAX; index++) {
            snapshot->clearedDynamicTileX[index] = data->clearedDynamicTileX[index];
            snapshot->clearedDynamicTileY[index] = data->clearedDynamicTileY[index];
        }
        return;
    }

    SaveLegacy_SeedFallbackClearedAirlockTiles(snapshot);
}

static void PopulateSnapshotFromCurrentRecord(SaveSnapshot *snapshot, const SaveLegacyRecordV4 *data, size_t fileSize) {
    PopulateSnapshotSharedState(snapshot, data);
    PopulateSnapshotLogs(snapshot, data->logsCollected);
    PopulateSnapshotCommunicatorState(snapshot, data, fileSize);
    PopulateSnapshotClearedDynamicTiles(snapshot, data, fileSize);
    SaveLegacy_ApplyDerivedSurvivalFields(snapshot);
}

static void PopulateSnapshotFromLegacyV2Record(SaveSnapshot *snapshot, const SaveLegacyRecordV2 *data, size_t fileSize) {
    PopulateSnapshotSharedState(snapshot, data);
    PopulateSnapshotLogs(snapshot, data->logsCollected);
    PopulateSnapshotCommunicatorState(snapshot, data, fileSize);
    SaveLegacy_SeedFallbackClearedAirlockTiles(snapshot);
    SaveLegacy_ApplyDerivedSurvivalFields(snapshot);
}

bool SaveLegacyAdapter_IsSupportedMagic(const char *magic) {
    return magic != NULL
        && (std::memcmp(magic, SAVE_MAGIC_V4, 7) == 0
            || std::memcmp(magic, SAVE_MAGIC_V3, 7) == 0
            || std::memcmp(magic, SAVE_MAGIC_V2, 7) == 0);
}

bool SaveLegacyAdapter_LoadSnapshot(const char *path, const char *magic, size_t fileSize, SaveSnapshot *snapshot) {
    SaveLegacyRecordV4 data;
    SaveLegacyRecordV2 legacyData;
    size_t bytesRead;

    if (path == NULL || magic == NULL || snapshot == NULL || !SaveLegacyAdapter_IsSupportedMagic(magic)) {
        return false;
    }

    std::memset(snapshot, 0, sizeof(*snapshot));
    if (std::memcmp(magic, SAVE_MAGIC_V2, 7) == 0) {
        size_t minFileSize;

        minFileSize = offsetof(SaveLegacyRecordV2, communicatorUnlocked);
        if (fileSize < minFileSize || fileSize > sizeof(legacyData)) {
            return false;
        }

        std::memset(&legacyData, 0, sizeof(legacyData));
        bytesRead = PersistencePlatform_ReadFileAtMost(path, &legacyData, sizeof(legacyData), NULL);
        if (bytesRead != fileSize) {
            return false;
        }

        PopulateSnapshotFromLegacyV2Record(snapshot, &legacyData, fileSize);
        return true;
    }

    if (fileSize < offsetof(SaveLegacyRecordV4, communicatorUnlocked) || fileSize > sizeof(data)) {
        return false;
    }

    std::memset(&data, 0, sizeof(data));
    bytesRead = PersistencePlatform_ReadFileAtMost(path, &data, sizeof(data), NULL);
    if (bytesRead != fileSize) {
        return false;
    }

    PopulateSnapshotFromCurrentRecord(snapshot, &data, fileSize);
    return true;
}
