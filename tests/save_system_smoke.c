#include "save_system.h"
#include "persistence_platform.h"
#include "../src/save_legacy_format.h"

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define SETTINGS_MAGIC_V2 "SCLSET2"
#define SAVE_MAGIC_V5 "SCLSAV5"
#define SAVE_NATIVE_MAX_LOGS_LEGACY 6
#define SAVE_TEST_BUFFER_SIZE 8192

typedef struct LegacySettingsFileData {
    char magic[8];
    float masterVolume;
    uint8_t sfxEnabled;
} LegacySettingsFileData;

static void Require(bool condition, const char *message) {
    if (condition) {
        return;
    }

    fprintf(stderr, "save_system_smoke failed: %s\n", message);
    exit(1);
}

static void WriteUInt8ToBuffer(uint8_t *buffer, size_t bufferSize, size_t *offset, uint8_t value) {
    if (buffer == NULL || offset == NULL) {
        return;
    }

    Require(*offset + 1 <= bufferSize, "buffer write should stay within bounds");
    buffer[*offset] = value;
    *offset += 1;
}

static void WriteInt16ToBuffer(uint8_t *buffer, size_t bufferSize, size_t *offset, int16_t value) {
    uint16_t rawValue;

    rawValue = (uint16_t)value;
    Require(*offset + 2 <= bufferSize, "buffer write should stay within bounds");
    buffer[*offset] = (uint8_t)(rawValue & 0xFFu);
    buffer[*offset + 1] = (uint8_t)((rawValue >> 8) & 0xFFu);
    *offset += 2;
}

static void WriteInt32ToBuffer(uint8_t *buffer, size_t bufferSize, size_t *offset, int32_t value) {
    uint32_t rawValue;

    rawValue = (uint32_t)value;
    Require(*offset + 4 <= bufferSize, "buffer write should stay within bounds");
    buffer[*offset] = (uint8_t)(rawValue & 0xFFu);
    buffer[*offset + 1] = (uint8_t)((rawValue >> 8) & 0xFFu);
    buffer[*offset + 2] = (uint8_t)((rawValue >> 16) & 0xFFu);
    buffer[*offset + 3] = (uint8_t)((rawValue >> 24) & 0xFFu);
    *offset += 4;
}

static void WriteFloatToBuffer(uint8_t *buffer, size_t bufferSize, size_t *offset, float value) {
    uint32_t rawValue;

    memcpy(&rawValue, &value, sizeof(rawValue));
    Require(*offset + 4 <= bufferSize, "buffer write should stay within bounds");
    buffer[*offset] = (uint8_t)(rawValue & 0xFFu);
    buffer[*offset + 1] = (uint8_t)((rawValue >> 8) & 0xFFu);
    buffer[*offset + 2] = (uint8_t)((rawValue >> 16) & 0xFFu);
    buffer[*offset + 3] = (uint8_t)((rawValue >> 24) & 0xFFu);
    *offset += 4;
}

static SaveSnapshot BuildSnapshot(void) {
    SaveSnapshot snapshot;

    memset(&snapshot, 0, sizeof(snapshot));
    snapshot.gridX = PLAYER_START_X;
    snapshot.gridY = PLAYER_START_Y;
    snapshot.facingX = 0;
    snapshot.facingY = 1;
    snapshot.health = 68.0f;
    snapshot.stamina = 72.5f;
    snapshot.pressure = 12.0f;
    snapshot.oxygen = 84.0f;
    snapshot.poison = 3.0f;
    snapshot.maxHealthBonus = 12.0f;
    snapshot.maxStaminaBonus = 16.0f;
    snapshot.attackBonus = 5.0f;
    snapshot.deathCount = 2;
    snapshot.crouching = true;
    snapshot.hasGlowStick = true;
    snapshot.hasRope = true;
    snapshot.hasLaserGun = true;
    snapshot.hasProtectionSuit = true;
    snapshot.hasSignalAmplifier = true;
    snapshot.hasFieldCamp = true;
    snapshot.statuses[PLAYER_STATUS_POISONED].active = true;
    snapshot.statuses[PLAYER_STATUS_POISONED].level = 2;
    snapshot.statuses[PLAYER_STATUS_POISONED].remainingTime = 18.0f;
    snapshot.statuses[PLAYER_STATUS_POISONED].magnitude = 4.0f;
    snapshot.resources[RESOURCE_FRUIT] = 4;
    snapshot.resources[RESOURCE_JUNK_METAL] = 7;
    snapshot.stage = 5;
    snapshot.dayCount = 3;
    snapshot.phase = DAY_PHASE_DUSK;
    snapshot.currentEvent = EVENT_CLEAR_SKY;
    snapshot.cycleTimer = 18.0f;
    snapshot.elapsedSeconds = 125.0f;
    snapshot.oxygenRepairLevel = 2;
    snapshot.commRepairLevel = 1;
    snapshot.energyRepairLevel = 1;
    snapshot.crashClueFound = true;
    snapshot.amplifierUnlocked = true;
    snapshot.bossDefeated = false;
    snapshot.signalTowerActivated = false;
    snapshot.monolithActivated[0] = true;
    snapshot.monolithActivated[1] = false;
    snapshot.monolithActivated[2] = true;
    snapshot.monolithsLit = 2;
    snapshot.ending = ENDING_NONE;
    snapshot.campPlaced = true;
    snapshot.campX = PLAYER_START_X + 1;
    snapshot.campY = PLAYER_START_Y;
    snapshot.clearedDynamicTileCount = 2;
    snapshot.clearedDynamicTileX[0] = PLAYER_START_X;
    snapshot.clearedDynamicTileY[0] = PLAYER_START_Y;
    snapshot.clearedDynamicTileX[1] = PLAYER_START_X + 1;
    snapshot.clearedDynamicTileY[1] = PLAYER_START_Y;
    snapshot.nodes[0].active = true;
    snapshot.nodes[0].respawnsRemaining = 1;
    snapshot.monsters[0].active = true;
    snapshot.monsters[0].gridX = PLAYER_START_X + 2;
    snapshot.monsters[0].gridY = PLAYER_START_Y + 1;
    snapshot.monsters[0].health = 14.0f;
    snapshot.monsters[0].phaseTriggered = true;
    snapshot.logs[0].collected = true;
    snapshot.communicatorUnlocked = true;
    return snapshot;
}

static void WriteLegacySettingsV2(float masterVolume, bool sfxEnabled) {
    LegacySettingsFileData settingsData;
    char path[640];

    memset(&settingsData, 0, sizeof(settingsData));
    memcpy(settingsData.magic, SETTINGS_MAGIC_V2, 8);
    settingsData.masterVolume = masterVolume;
    settingsData.sfxEnabled = sfxEnabled ? 1u : 0u;
    snprintf(path, sizeof(path), "%s/settings.bin", SaveSystem_GetBaseDirectory());
    Require(PersistencePlatform_WriteFileAtomically(path, &settingsData, sizeof(settingsData)), "legacy settings write should succeed");
}

#define POPULATE_LEGACY_COMMON_RECORD(recordPtr, magicValue, snapshotPtr) \
    do { \
        const SaveSnapshot *snapshotLocal = (snapshotPtr); \
        memset((recordPtr), 0, sizeof(*(recordPtr))); \
        memcpy((recordPtr)->magic, (magicValue), 8); \
        (recordPtr)->gridX = snapshotLocal->gridX; \
        (recordPtr)->gridY = snapshotLocal->gridY; \
        (recordPtr)->facingX = snapshotLocal->facingX; \
        (recordPtr)->facingY = snapshotLocal->facingY; \
        (recordPtr)->stamina = snapshotLocal->stamina; \
        (recordPtr)->pressure = snapshotLocal->pressure; \
        (recordPtr)->oxygen = snapshotLocal->oxygen; \
        (recordPtr)->poison = snapshotLocal->poison; \
        (recordPtr)->maxStaminaBonus = snapshotLocal->maxStaminaBonus; \
        (recordPtr)->attackBonus = snapshotLocal->attackBonus; \
        (recordPtr)->deathCount = snapshotLocal->deathCount; \
        (recordPtr)->crouching = snapshotLocal->crouching ? 1u : 0u; \
        (recordPtr)->hasGlowStick = snapshotLocal->hasGlowStick ? 1u : 0u; \
        (recordPtr)->hasRope = snapshotLocal->hasRope ? 1u : 0u; \
        (recordPtr)->hasLaserGun = snapshotLocal->hasLaserGun ? 1u : 0u; \
        (recordPtr)->hasProtectionSuit = snapshotLocal->hasProtectionSuit ? 1u : 0u; \
        (recordPtr)->hasSignalAmplifier = snapshotLocal->hasSignalAmplifier ? 1u : 0u; \
        (recordPtr)->hasFieldCamp = snapshotLocal->hasFieldCamp ? 1u : 0u; \
        memcpy((recordPtr)->resources, snapshotLocal->resources, sizeof((recordPtr)->resources)); \
        (recordPtr)->stage = snapshotLocal->stage; \
        (recordPtr)->dayCount = snapshotLocal->dayCount; \
        (recordPtr)->phase = snapshotLocal->phase; \
        (recordPtr)->currentEvent = snapshotLocal->currentEvent; \
        (recordPtr)->cycleTimer = snapshotLocal->cycleTimer; \
        (recordPtr)->elapsedSeconds = snapshotLocal->elapsedSeconds; \
        (recordPtr)->oxygenRepairLevel = snapshotLocal->oxygenRepairLevel; \
        (recordPtr)->commRepairLevel = snapshotLocal->commRepairLevel; \
        (recordPtr)->energyRepairLevel = snapshotLocal->energyRepairLevel; \
        (recordPtr)->crashClueFound = snapshotLocal->crashClueFound ? 1u : 0u; \
        (recordPtr)->amplifierUnlocked = snapshotLocal->amplifierUnlocked ? 1u : 0u; \
        (recordPtr)->bossDefeated = snapshotLocal->bossDefeated ? 1u : 0u; \
        (recordPtr)->signalTowerActivated = snapshotLocal->signalTowerActivated ? 1u : 0u; \
        (recordPtr)->monolithActivated[0] = snapshotLocal->monolithActivated[0] ? 1u : 0u; \
        (recordPtr)->monolithActivated[1] = snapshotLocal->monolithActivated[1] ? 1u : 0u; \
        (recordPtr)->monolithActivated[2] = snapshotLocal->monolithActivated[2] ? 1u : 0u; \
        (recordPtr)->monolithsLit = snapshotLocal->monolithsLit; \
        (recordPtr)->ending = snapshotLocal->ending; \
        (recordPtr)->campPlaced = snapshotLocal->campPlaced ? 1u : 0u; \
        (recordPtr)->campX = snapshotLocal->campX; \
        (recordPtr)->campY = snapshotLocal->campY; \
        (recordPtr)->nodeActive[0] = snapshotLocal->nodes[0].active ? 1u : 0u; \
        (recordPtr)->nodeRespawns[0] = snapshotLocal->nodes[0].respawnsRemaining; \
        (recordPtr)->monsterActive[0] = snapshotLocal->monsters[0].active ? 1u : 0u; \
        (recordPtr)->monsterGridX[0] = snapshotLocal->monsters[0].gridX; \
        (recordPtr)->monsterGridY[0] = snapshotLocal->monsters[0].gridY; \
        (recordPtr)->monsterHealth[0] = snapshotLocal->monsters[0].health; \
        (recordPtr)->monsterPhaseTriggered[0] = snapshotLocal->monsters[0].phaseTriggered ? 1u : 0u; \
        (recordPtr)->logsCollected[0] = snapshotLocal->logs[0].collected ? 1u : 0u; \
        (recordPtr)->communicatorUnlocked = snapshotLocal->communicatorUnlocked ? 1u : 0u; \
    } while (0)

static void WriteSavePayload(int slotIndex, const void *data, size_t size, const char *message) {
    char path[640];

    SaveSystem_GetSlotPath(slotIndex, path, sizeof(path));
    Require(PersistencePlatform_WriteFileAtomically(path, data, size), message);
}

static void WriteLegacySaveV2(int slotIndex, const SaveSnapshot *snapshot) {
    SaveLegacyRecordV2 record;

    POPULATE_LEGACY_COMMON_RECORD(&record, SAVE_MAGIC_V2, snapshot);
    WriteSavePayload(slotIndex, &record, sizeof(record), "legacy v2 save write should succeed");
}

static void WriteLegacySaveV4(int slotIndex, const SaveSnapshot *snapshot) {
    SaveLegacyRecordV4 record;

    POPULATE_LEGACY_COMMON_RECORD(&record, SAVE_MAGIC_V4, snapshot);
    record.clearedDynamicTileCount = snapshot->clearedDynamicTileCount;
    record.clearedDynamicTileX[0] = (int16_t)snapshot->clearedDynamicTileX[0];
    record.clearedDynamicTileY[0] = (int16_t)snapshot->clearedDynamicTileY[0];
    WriteSavePayload(slotIndex, &record, sizeof(record), "legacy v4 save write should succeed");
}

static void WriteTruncatedLegacySaveV4(int slotIndex, const SaveSnapshot *snapshot) {
    SaveLegacyRecordV4 record;
    size_t truncatedSize;

    POPULATE_LEGACY_COMMON_RECORD(&record, SAVE_MAGIC_V4, snapshot);
    truncatedSize = offsetof(SaveLegacyRecordV4, communicatorUnlocked) - 1;
    WriteSavePayload(slotIndex, &record, truncatedSize, "truncated legacy save write should succeed");
}

static void WriteLegacySaveV5(int slotIndex, const SaveSnapshot *snapshot) {
    uint8_t buffer[SAVE_TEST_BUFFER_SIZE];
    size_t offset;
    int index;

    memset(buffer, 0, sizeof(buffer));
    offset = 0;
    memcpy(buffer + offset, SAVE_MAGIC_V5, 8);
    offset += 8;
    WriteInt32ToBuffer(buffer, sizeof(buffer), &offset, snapshot->gridX);
    WriteInt32ToBuffer(buffer, sizeof(buffer), &offset, snapshot->gridY);
    WriteInt32ToBuffer(buffer, sizeof(buffer), &offset, snapshot->facingX);
    WriteInt32ToBuffer(buffer, sizeof(buffer), &offset, snapshot->facingY);
    WriteFloatToBuffer(buffer, sizeof(buffer), &offset, snapshot->stamina);
    WriteFloatToBuffer(buffer, sizeof(buffer), &offset, snapshot->pressure);
    WriteFloatToBuffer(buffer, sizeof(buffer), &offset, snapshot->oxygen);
    WriteFloatToBuffer(buffer, sizeof(buffer), &offset, snapshot->poison);
    WriteFloatToBuffer(buffer, sizeof(buffer), &offset, snapshot->maxStaminaBonus);
    WriteFloatToBuffer(buffer, sizeof(buffer), &offset, snapshot->attackBonus);
    WriteInt32ToBuffer(buffer, sizeof(buffer), &offset, snapshot->deathCount);
    WriteUInt8ToBuffer(buffer, sizeof(buffer), &offset, snapshot->crouching ? 1u : 0u);
    WriteUInt8ToBuffer(buffer, sizeof(buffer), &offset, snapshot->hasGlowStick ? 1u : 0u);
    WriteUInt8ToBuffer(buffer, sizeof(buffer), &offset, snapshot->hasRope ? 1u : 0u);
    WriteUInt8ToBuffer(buffer, sizeof(buffer), &offset, snapshot->hasLaserGun ? 1u : 0u);
    WriteUInt8ToBuffer(buffer, sizeof(buffer), &offset, snapshot->hasProtectionSuit ? 1u : 0u);
    WriteUInt8ToBuffer(buffer, sizeof(buffer), &offset, snapshot->hasSignalAmplifier ? 1u : 0u);
    WriteUInt8ToBuffer(buffer, sizeof(buffer), &offset, snapshot->hasFieldCamp ? 1u : 0u);

    for (index = 0; index < RESOURCE_COUNT; index++) {
        WriteInt32ToBuffer(buffer, sizeof(buffer), &offset, snapshot->resources[index]);
    }

    WriteInt32ToBuffer(buffer, sizeof(buffer), &offset, snapshot->stage);
    WriteInt32ToBuffer(buffer, sizeof(buffer), &offset, snapshot->dayCount);
    WriteInt32ToBuffer(buffer, sizeof(buffer), &offset, snapshot->phase);
    WriteInt32ToBuffer(buffer, sizeof(buffer), &offset, snapshot->currentEvent);
    WriteFloatToBuffer(buffer, sizeof(buffer), &offset, snapshot->cycleTimer);
    WriteFloatToBuffer(buffer, sizeof(buffer), &offset, snapshot->elapsedSeconds);
    WriteInt32ToBuffer(buffer, sizeof(buffer), &offset, snapshot->oxygenRepairLevel);
    WriteInt32ToBuffer(buffer, sizeof(buffer), &offset, snapshot->commRepairLevel);
    WriteInt32ToBuffer(buffer, sizeof(buffer), &offset, snapshot->energyRepairLevel);
    WriteUInt8ToBuffer(buffer, sizeof(buffer), &offset, snapshot->crashClueFound ? 1u : 0u);
    WriteUInt8ToBuffer(buffer, sizeof(buffer), &offset, snapshot->amplifierUnlocked ? 1u : 0u);
    WriteUInt8ToBuffer(buffer, sizeof(buffer), &offset, snapshot->bossDefeated ? 1u : 0u);
    WriteUInt8ToBuffer(buffer, sizeof(buffer), &offset, snapshot->signalTowerActivated ? 1u : 0u);
    WriteUInt8ToBuffer(buffer, sizeof(buffer), &offset, snapshot->monolithActivated[0] ? 1u : 0u);
    WriteUInt8ToBuffer(buffer, sizeof(buffer), &offset, snapshot->monolithActivated[1] ? 1u : 0u);
    WriteUInt8ToBuffer(buffer, sizeof(buffer), &offset, snapshot->monolithActivated[2] ? 1u : 0u);
    WriteInt32ToBuffer(buffer, sizeof(buffer), &offset, snapshot->monolithsLit);
    WriteInt32ToBuffer(buffer, sizeof(buffer), &offset, snapshot->ending);
    WriteUInt8ToBuffer(buffer, sizeof(buffer), &offset, snapshot->campPlaced ? 1u : 0u);
    WriteInt32ToBuffer(buffer, sizeof(buffer), &offset, snapshot->campX);
    WriteInt32ToBuffer(buffer, sizeof(buffer), &offset, snapshot->campY);
    WriteUInt8ToBuffer(buffer, sizeof(buffer), &offset, snapshot->communicatorUnlocked ? 1u : 0u);
    WriteInt32ToBuffer(buffer, sizeof(buffer), &offset, snapshot->clearedDynamicTileCount);

    for (index = 0; index < MAX_RESOURCE_NODES; index++) {
        WriteUInt8ToBuffer(buffer, sizeof(buffer), &offset, snapshot->nodes[index].active ? 1u : 0u);
        WriteInt32ToBuffer(buffer, sizeof(buffer), &offset, snapshot->nodes[index].respawnsRemaining);
    }
    for (index = 0; index < MAX_MONSTERS; index++) {
        WriteUInt8ToBuffer(buffer, sizeof(buffer), &offset, snapshot->monsters[index].active ? 1u : 0u);
        WriteInt32ToBuffer(buffer, sizeof(buffer), &offset, snapshot->monsters[index].gridX);
        WriteInt32ToBuffer(buffer, sizeof(buffer), &offset, snapshot->monsters[index].gridY);
        WriteFloatToBuffer(buffer, sizeof(buffer), &offset, snapshot->monsters[index].health);
        WriteUInt8ToBuffer(buffer, sizeof(buffer), &offset, snapshot->monsters[index].phaseTriggered ? 1u : 0u);
    }
    for (index = 0; index < SAVE_NATIVE_MAX_LOGS_LEGACY; index++) {
        WriteUInt8ToBuffer(buffer, sizeof(buffer), &offset, snapshot->logs[index].collected ? 1u : 0u);
    }
    for (index = 0; index < SAVE_DYNAMIC_TILE_MAX; index++) {
        WriteInt16ToBuffer(buffer, sizeof(buffer), &offset, (int16_t)snapshot->clearedDynamicTileX[index]);
        WriteInt16ToBuffer(buffer, sizeof(buffer), &offset, (int16_t)snapshot->clearedDynamicTileY[index]);
    }

    WriteSavePayload(slotIndex, buffer, offset, "legacy v5 save write should succeed");
}

int main(void) {
    char tempRoot[] = "/tmp/scl_save_smoke.XXXXXX";
    char authMessage[SAVE_AUTH_MESSAGE_MAX];
    char deletedAccountPath[640];
    char gammaAccountPath[640];
    GameSettings settings;
    GameSettings loadedSettings;
    SaveSnapshot saved;
    SaveSnapshot loaded;
    SaveSnapshot legacySource;
    SaveSnapshot secondAccountSaved;
    SaveSlotInfo slots[SAVE_SLOT_COUNT];

    Require(mkdtemp(tempRoot) != NULL, "mkdtemp should succeed");
    Require(setenv("HOME", tempRoot, 1) == 0, "HOME override should succeed");

    saved = BuildSnapshot();
    memset(authMessage, 0, sizeof(authMessage));
    Require(!SaveSystem_LoadSettings(NULL), "settings load should reject null");
    Require(!SaveSystem_SaveSettings(NULL), "settings save should reject null");
    Require(!SaveSystem_LoadGame(0, NULL), "load should reject null snapshot");
    Require(!SaveSystem_SaveGame(0, NULL), "save should reject null snapshot");
    Require(!SaveSystem_IsAccountAuthenticated(), "save tests should start logged out");
    Require(!SaveSystem_HasAnySave(), "logged out state should not expose save slots");
    Require(!SaveSystem_SaveGame(0, &saved), "saving should require an authenticated account");
    Require(SaveSystem_Register("alpha", "pass1234", authMessage, sizeof(authMessage)), "register should create the first local account");
    Require(SaveSystem_IsAccountAuthenticated(), "register should sign the first account in");
    Require(strcmp(SaveSystem_GetActiveAccountName(), "alpha") == 0, "registered account name should become active");
    Require(!SaveSystem_Register("alpha", "pass9999", authMessage, sizeof(authMessage)), "register should reject duplicate usernames");
    Require(strstr(authMessage, "already exists") != NULL, "duplicate username should return an already-exists message");
    Require(!SaveSystem_Register("ALPHA", "pass9999", authMessage, sizeof(authMessage)), "register should treat usernames as case-insensitive");
    Require(!SaveSystem_Register("  alpha  ", "pass9999", authMessage, sizeof(authMessage)), "register should reject duplicates after trimming whitespace");
    SaveSystem_SetDefaultSettings(&settings);
    settings.masterVolume = 0.37f;
    settings.sfxEnabled = false;
    settings.language = GAME_LANGUAGE_ZH_CN;
    snprintf(settings.lastUsername, sizeof(settings.lastUsername), "%s", "alpha");
    memset(&loadedSettings, 0, sizeof(loadedSettings));
    memset(&loaded, 0, sizeof(loaded));
    memset(slots, 0, sizeof(slots));

    Require(SaveSystem_SaveSettings(&settings), "settings save should succeed");
    Require(SaveSystem_LoadSettings(&loadedSettings), "settings load should succeed");
    Require(loadedSettings.masterVolume > 0.36f && loadedSettings.masterVolume < 0.38f, "settings volume should round-trip");
    Require(!loadedSettings.sfxEnabled, "settings sfx flag should round-trip");
    Require(loadedSettings.language == GAME_LANGUAGE_ZH_CN, "settings language should round-trip");
    Require(strcmp(loadedSettings.lastUsername, "alpha") == 0, "settings should remember the last username");

    WriteLegacySettingsV2(1.25f, true);
    memset(&loadedSettings, 0, sizeof(loadedSettings));
    Require(SaveSystem_LoadSettings(&loadedSettings), "legacy settings load should succeed");
    Require(loadedSettings.masterVolume == 1.0f, "legacy settings volume should clamp");
    Require(loadedSettings.sfxEnabled, "legacy settings sfx flag should load");
    Require(loadedSettings.language == GAME_LANGUAGE_EN, "legacy settings should default language to english");
    Require(loadedSettings.lastUsername[0] == '\0', "legacy settings should default remembered username to empty");

    Require(SaveSystem_SaveGame(0, &saved), "save should succeed");
    Require(SaveSystem_HasAnySave(), "save discovery should report at least one slot");
    Require(SaveSystem_HasSaveInSlot(0), "slot 0 should exist after save");

    SaveSystem_ListSlots(slots, SAVE_SLOT_COUNT);
    Require(slots[0].exists, "slot listing should include slot 0");
    Require(slots[0].stage == saved.stage, "slot listing should preserve stage");
    Require(slots[0].dayCount == saved.dayCount, "slot listing should preserve day count");
    Require(slots[0].deathCount == saved.deathCount, "slot listing should preserve death count");
    Require(slots[0].health == saved.health, "slot listing should preserve health");

    Require(SaveSystem_LoadGame(0, &loaded), "load should succeed");
    Require(loaded.health == saved.health, "health should round-trip");
    Require(loaded.maxHealthBonus == saved.maxHealthBonus, "max health bonus should round-trip");
    Require(loaded.statuses[PLAYER_STATUS_POISONED].active, "status active flag should round-trip");
    Require(loaded.statuses[PLAYER_STATUS_POISONED].level == saved.statuses[PLAYER_STATUS_POISONED].level, "status level should round-trip");
    Require(loaded.oxygenRepairLevel == 2, "oxygen repair level 2 should round-trip");
    Require(loaded.communicatorUnlocked, "communicator state should round-trip");
    Require(loaded.clearedDynamicTileCount == saved.clearedDynamicTileCount, "dynamic tile count should round-trip");
    Require(loaded.resources[RESOURCE_JUNK_METAL] == saved.resources[RESOURCE_JUNK_METAL], "resource inventory should round-trip");
    Require(loaded.logs[0].collected == saved.logs[0].collected, "log collection should round-trip");

    SaveSystem_Logout();
    Require(!SaveSystem_IsAccountAuthenticated(), "logout should clear the active account");
    Require(!SaveSystem_HasAnySave(), "logged out state should hide existing saves");
    Require(SaveSystem_Register("beta", "beta5678", authMessage, sizeof(authMessage)), "register should create a second local account");
    Require(!SaveSystem_HasAnySave(), "a different account should start with its own empty save list");

    secondAccountSaved = saved;
    secondAccountSaved.stage = 6;
    secondAccountSaved.dayCount = 5;
    secondAccountSaved.health = 41.0f;
    secondAccountSaved.oxygen = 58.0f;
    Require(SaveSystem_SaveGame(0, &secondAccountSaved), "second account should be able to save into its own slot");

    SaveSystem_Logout();
    Require(SaveSystem_Login("alpha", "pass1234", authMessage, sizeof(authMessage)), "login should restore the first account");
    memset(&loaded, 0, sizeof(loaded));
    Require(SaveSystem_LoadGame(0, &loaded), "first account should still load its own slot");
    Require(loaded.stage == saved.stage && loaded.dayCount == saved.dayCount, "first account save should stay isolated from other accounts");
    Require(loaded.health == saved.health && loaded.oxygen == saved.oxygen, "first account vitals should not be overwritten by another account");

    SaveSystem_Logout();
    Require(SaveSystem_Login("beta", "beta5678", authMessage, sizeof(authMessage)), "login should switch to the second account");
    memset(&loaded, 0, sizeof(loaded));
    Require(SaveSystem_LoadGame(0, &loaded), "second account should load its own slot");
    Require(loaded.stage == secondAccountSaved.stage && loaded.dayCount == secondAccountSaved.dayCount,
            "second account should see only its own saved progress");
    Require(loaded.health == secondAccountSaved.health && loaded.oxygen == secondAccountSaved.oxygen,
            "second account vitals should round-trip independently");
    snprintf(deletedAccountPath, sizeof(deletedAccountPath), "%s/accounts/beta", SaveSystem_GetBaseDirectory());
    Require(PersistencePlatform_FileExists(deletedAccountPath), "second account directory should exist before deletion");
    Require(SaveSystem_DeleteActiveAccount(authMessage, sizeof(authMessage)), "active account deletion should succeed");
    Require(!SaveSystem_IsAccountAuthenticated(), "deleting the active account should sign it out");
    Require(!PersistencePlatform_FileExists(deletedAccountPath), "deleting an account should remove its save directory");
    Require(!SaveSystem_Login("beta", "beta5678", authMessage, sizeof(authMessage)), "deleted account should no longer be able to log in");
    Require(SaveSystem_HasRegisteredAccounts(), "other accounts should remain registered after deleting one");

    SaveSystem_Logout();
    Require(SaveSystem_Register("gamma", "gamma000", authMessage, sizeof(authMessage)), "register should create a third local account");
    snprintf(gammaAccountPath, sizeof(gammaAccountPath), "%s/accounts/gamma", SaveSystem_GetBaseDirectory());
    Require(PersistencePlatform_FileExists(gammaAccountPath), "third account directory should exist before logged-out deletion");
    SaveSystem_Logout();
    Require(!SaveSystem_DeleteAccount("gamma", "wrongpass", authMessage, sizeof(authMessage)), "delete account should reject a wrong password");
    Require(PersistencePlatform_FileExists(gammaAccountPath), "wrong-password deletion should leave the account directory intact");
    Require(!SaveSystem_IsAccountAuthenticated(), "logged-out deletion attempt should keep the auth state logged out");
    Require(SaveSystem_DeleteAccount("gamma", "gamma000", authMessage, sizeof(authMessage)), "delete account should succeed with matching credentials");
    Require(!PersistencePlatform_FileExists(gammaAccountPath), "credential-based deletion should remove the target account directory");
    Require(!SaveSystem_Login("gamma", "gamma000", authMessage, sizeof(authMessage)), "deleted logged-out account should no longer be able to log in");
    Require(!SaveSystem_IsAccountAuthenticated(), "deleting a logged-out account should not silently sign one in");

    Require(SaveSystem_Login("alpha", "pass1234", authMessage, sizeof(authMessage)), "legacy compatibility checks should continue under the first account");

    legacySource = saved;
    legacySource.stamina = 27.0f;
    legacySource.pressure = 83.0f;
    legacySource.oxygen = 7.0f;
    legacySource.poison = 52.0f;
    WriteLegacySaveV2(1, &legacySource);
    memset(&loaded, 0, sizeof(loaded));
    Require(SaveSystem_LoadGame(1, &loaded), "legacy v2 load should succeed");
    Require(loaded.stage == legacySource.stage, "legacy v2 should preserve stage");
    Require(loaded.health == legacySource.stamina, "legacy v2 should map stamina to health");
    Require(loaded.pressure == INITIAL_PRESSURE, "legacy v2 should discard old pressure into the new default recovery strategy");
    Require(loaded.statuses[PLAYER_STATUS_POISONED].active && loaded.statuses[PLAYER_STATUS_POISONED].level == 2,
            "legacy v2 should translate old poison into poisoned status");
    Require(loaded.statuses[PLAYER_STATUS_LOW_OXYGEN].active && loaded.statuses[PLAYER_STATUS_LOW_OXYGEN].level == 2,
            "legacy v2 should translate low oxygen into the warning status");
    Require(loaded.statuses[PLAYER_STATUS_CRITICAL_CONDITION].active,
            "legacy v2 should derive critical condition from low mapped health");
    Require(loaded.communicatorUnlocked == legacySource.communicatorUnlocked, "legacy v2 communicator should round-trip");

    legacySource.stamina = 62.0f;
    legacySource.pressure = 71.0f;
    legacySource.oxygen = 0.0f;
    legacySource.poison = 12.0f;
    WriteLegacySaveV4(2, &legacySource);
    memset(&loaded, 0, sizeof(loaded));
    Require(SaveSystem_LoadGame(2, &loaded), "legacy v4 load should succeed");
    Require(loaded.clearedDynamicTileCount == legacySource.clearedDynamicTileCount, "legacy v4 dynamic tile count should round-trip");
    Require(loaded.clearedDynamicTileX[0] == legacySource.clearedDynamicTileX[0], "legacy v4 dynamic tile x should round-trip");
    Require(loaded.health == legacySource.stamina, "legacy v4 should map stamina to health");
    Require(loaded.pressure == INITIAL_PRESSURE, "legacy v4 should discard old pressure into the default state");
    Require(loaded.statuses[PLAYER_STATUS_POISONED].active && loaded.statuses[PLAYER_STATUS_POISONED].level == 1,
            "legacy v4 should translate old poison into poisoned status");
    Require(loaded.statuses[PLAYER_STATUS_SUFFOCATING].active,
            "legacy v4 should translate zero oxygen into suffocating status");

    legacySource.stamina = 48.0f;
    legacySource.pressure = 66.0f;
    legacySource.oxygen = 18.0f;
    legacySource.poison = 0.0f;
    WriteLegacySaveV5(4, &legacySource);
    memset(&loaded, 0, sizeof(loaded));
    Require(SaveSystem_LoadGame(4, &loaded), "legacy v5 load should succeed");
    Require(loaded.health == legacySource.stamina, "legacy v5 should map stamina to health");
    Require(loaded.maxHealthBonus == legacySource.maxStaminaBonus, "legacy v5 should map stamina bonus to max health bonus");
    Require(loaded.pressure == INITIAL_PRESSURE, "legacy v5 should discard old pressure into the default state");
    Require(loaded.statuses[PLAYER_STATUS_LOW_OXYGEN].active && loaded.statuses[PLAYER_STATUS_LOW_OXYGEN].level == 1,
            "legacy v5 should derive low oxygen warning from the stored oxygen value");

    WriteTruncatedLegacySaveV4(3, &saved);
    memset(&loaded, 0, sizeof(loaded));
    Require(!SaveSystem_LoadGame(3, &loaded), "truncated legacy v4 should be rejected");
    memset(slots, 0, sizeof(slots));
    SaveSystem_ListSlots(slots, SAVE_SLOT_COUNT);
    Require(!slots[3].exists, "truncated legacy slot should not appear in listings");

    Require(SaveSystem_DeleteGame(0), "delete should succeed");
    Require(!SaveSystem_HasSaveInSlot(0), "slot 0 should be gone after delete");

    puts("save_system smoke ok");
    return 0;
}
