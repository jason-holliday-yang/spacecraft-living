#include "save_system.h"
#include "persistence_platform.h"

#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define SAVE_MAGIC_V5 "SCLSAV5"
#define SAVE_NATIVE_MAX_LOGS_LEGACY 6
#define SAVE_TEST_BUFFER_SIZE 8192

static void Require(bool condition, const char *message) {
    if (condition) {
        return;
    }

    fprintf(stderr, "save_status_smoke failed: %s\n", message);
    exit(1);
}

static void WriteUInt8ToBuffer(uint8_t *buffer, size_t bufferSize, size_t *offset, uint8_t value) {
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

static SaveSnapshot BuildStatusSnapshot(void) {
    SaveSnapshot snapshot;

    memset(&snapshot, 0, sizeof(snapshot));
    snapshot.gridX = PLAYER_START_X;
    snapshot.gridY = PLAYER_START_Y;
    snapshot.facingX = 0;
    snapshot.facingY = 1;
    snapshot.health = 56.0f;
    snapshot.stamina = 44.0f;
    snapshot.pressure = 9.0f;
    snapshot.oxygen = 61.0f;
    snapshot.poison = 18.0f;
    snapshot.maxHealthBonus = 10.0f;
    snapshot.maxStaminaBonus = 8.0f;
    snapshot.attackBonus = 3.0f;
    snapshot.hasProtectionSuit = true;
    snapshot.stage = 6;
    snapshot.dayCount = 4;
    snapshot.phase = DAY_PHASE_NIGHT;
    snapshot.currentEvent = EVENT_SPORE_STORM;
    snapshot.statuses[PLAYER_STATUS_POISONED].active = true;
    snapshot.statuses[PLAYER_STATUS_POISONED].level = 2;
    snapshot.statuses[PLAYER_STATUS_POISONED].remainingTime = 22.0f;
    snapshot.statuses[PLAYER_STATUS_POISONED].magnitude = 18.0f;
    snapshot.statuses[PLAYER_STATUS_OXYGEN_LEAK].active = true;
    snapshot.statuses[PLAYER_STATUS_OXYGEN_LEAK].level = 1;
    snapshot.statuses[PLAYER_STATUS_OXYGEN_LEAK].remainingTime = 14.0f;
    snapshot.statuses[PLAYER_STATUS_OXYGEN_LEAK].magnitude = 0.55f;
    snapshot.statuses[PLAYER_STATUS_FILTERED].active = true;
    snapshot.statuses[PLAYER_STATUS_FILTERED].level = 2;
    snapshot.statuses[PLAYER_STATUS_FILTERED].remainingTime = 30.0f;
    snapshot.statuses[PLAYER_STATUS_FILTERED].magnitude = 0.35f;
    snapshot.statuses[PLAYER_STATUS_OXYGEN_RESERVE].active = true;
    snapshot.statuses[PLAYER_STATUS_OXYGEN_RESERVE].level = 1;
    snapshot.statuses[PLAYER_STATUS_OXYGEN_RESERVE].remainingTime = 18.0f;
    snapshot.statuses[PLAYER_STATUS_OXYGEN_RESERVE].magnitude = 8.0f;
    snapshot.statuses[PLAYER_STATUS_CAMP_RECOVERY].active = true;
    snapshot.statuses[PLAYER_STATUS_CAMP_RECOVERY].level = 1;
    snapshot.statuses[PLAYER_STATUS_CAMP_RECOVERY].remainingTime = 20.0f;
    snapshot.statuses[PLAYER_STATUS_CAMP_RECOVERY].magnitude = 4.0f;
    snapshot.communicatorUnlocked = true;
    return snapshot;
}

static void WriteLegacySaveV5(int slotIndex, float stamina, float pressure, float oxygen, float poison, float maxStaminaBonus) {
    uint8_t buffer[SAVE_TEST_BUFFER_SIZE];
    char path[640];
    size_t offset;
    int index;

    memset(buffer, 0, sizeof(buffer));
    offset = 0;
    memcpy(buffer + offset, SAVE_MAGIC_V5, 8);
    offset += 8;
    WriteInt32ToBuffer(buffer, sizeof(buffer), &offset, PLAYER_START_X);
    WriteInt32ToBuffer(buffer, sizeof(buffer), &offset, PLAYER_START_Y);
    WriteInt32ToBuffer(buffer, sizeof(buffer), &offset, 0);
    WriteInt32ToBuffer(buffer, sizeof(buffer), &offset, 1);
    WriteFloatToBuffer(buffer, sizeof(buffer), &offset, stamina);
    WriteFloatToBuffer(buffer, sizeof(buffer), &offset, pressure);
    WriteFloatToBuffer(buffer, sizeof(buffer), &offset, oxygen);
    WriteFloatToBuffer(buffer, sizeof(buffer), &offset, poison);
    WriteFloatToBuffer(buffer, sizeof(buffer), &offset, maxStaminaBonus);
    WriteFloatToBuffer(buffer, sizeof(buffer), &offset, 0.0f);
    WriteInt32ToBuffer(buffer, sizeof(buffer), &offset, 0);
    for (index = 0; index < 7; index++) {
        WriteUInt8ToBuffer(buffer, sizeof(buffer), &offset, 0u);
    }
    for (index = 0; index < RESOURCE_COUNT; index++) {
        WriteInt32ToBuffer(buffer, sizeof(buffer), &offset, 0);
    }
    WriteInt32ToBuffer(buffer, sizeof(buffer), &offset, 4);
    WriteInt32ToBuffer(buffer, sizeof(buffer), &offset, 2);
    WriteInt32ToBuffer(buffer, sizeof(buffer), &offset, DAY_PHASE_DAY);
    WriteInt32ToBuffer(buffer, sizeof(buffer), &offset, EVENT_CLEAR_SKY);
    WriteFloatToBuffer(buffer, sizeof(buffer), &offset, 0.0f);
    WriteFloatToBuffer(buffer, sizeof(buffer), &offset, 40.0f);
    WriteInt32ToBuffer(buffer, sizeof(buffer), &offset, 1);
    WriteInt32ToBuffer(buffer, sizeof(buffer), &offset, 0);
    WriteInt32ToBuffer(buffer, sizeof(buffer), &offset, 0);
    for (index = 0; index < 4; index++) {
        WriteUInt8ToBuffer(buffer, sizeof(buffer), &offset, 0u);
    }
    WriteUInt8ToBuffer(buffer, sizeof(buffer), &offset, 0u);
    WriteUInt8ToBuffer(buffer, sizeof(buffer), &offset, 0u);
    WriteUInt8ToBuffer(buffer, sizeof(buffer), &offset, 0u);
    WriteInt32ToBuffer(buffer, sizeof(buffer), &offset, 0);
    WriteInt32ToBuffer(buffer, sizeof(buffer), &offset, ENDING_NONE);
    WriteUInt8ToBuffer(buffer, sizeof(buffer), &offset, 0u);
    WriteInt32ToBuffer(buffer, sizeof(buffer), &offset, -1);
    WriteInt32ToBuffer(buffer, sizeof(buffer), &offset, -1);
    WriteUInt8ToBuffer(buffer, sizeof(buffer), &offset, 0u);
    WriteInt32ToBuffer(buffer, sizeof(buffer), &offset, 0);
    for (index = 0; index < MAX_RESOURCE_NODES; index++) {
        WriteUInt8ToBuffer(buffer, sizeof(buffer), &offset, 0u);
        WriteInt32ToBuffer(buffer, sizeof(buffer), &offset, 0);
    }
    for (index = 0; index < MAX_MONSTERS; index++) {
        WriteUInt8ToBuffer(buffer, sizeof(buffer), &offset, 0u);
        WriteInt32ToBuffer(buffer, sizeof(buffer), &offset, 0);
        WriteInt32ToBuffer(buffer, sizeof(buffer), &offset, 0);
        WriteFloatToBuffer(buffer, sizeof(buffer), &offset, 0.0f);
        WriteUInt8ToBuffer(buffer, sizeof(buffer), &offset, 0u);
    }
    for (index = 0; index < SAVE_NATIVE_MAX_LOGS_LEGACY; index++) {
        WriteUInt8ToBuffer(buffer, sizeof(buffer), &offset, 0u);
    }
    for (index = 0; index < SAVE_DYNAMIC_TILE_MAX; index++) {
        WriteInt16ToBuffer(buffer, sizeof(buffer), &offset, 0);
        WriteInt16ToBuffer(buffer, sizeof(buffer), &offset, 0);
    }

    SaveSystem_GetSlotPath(slotIndex, path, sizeof(path));
    Require(PersistencePlatform_WriteFileAtomically(path, buffer, offset), "legacy v5 save write should succeed");
}

int main(void) {
    char tempRoot[] = "/tmp/scl_save_status_smoke.XXXXXX";
    char authMessage[SAVE_AUTH_MESSAGE_MAX];
    SaveSnapshot snapshot;
    SaveSnapshot loaded;

    Require(mkdtemp(tempRoot) != NULL, "mkdtemp should succeed");
    Require(setenv("HOME", tempRoot, 1) == 0, "HOME override should succeed");
    Require(SaveSystem_Register("status", "pass1234", authMessage, sizeof(authMessage)),
            "status smoke should authenticate a local account before save checks");

    snapshot = BuildStatusSnapshot();
    memset(&loaded, 0, sizeof(loaded));
    Require(SaveSystem_SaveGame(0, &snapshot), "native status save should succeed");
    Require(SaveSystem_LoadGame(0, &loaded), "native status load should succeed");
    Require(loaded.statuses[PLAYER_STATUS_POISONED].active
                && loaded.statuses[PLAYER_STATUS_OXYGEN_LEAK].active
                && loaded.statuses[PLAYER_STATUS_FILTERED].active
                && loaded.statuses[PLAYER_STATUS_OXYGEN_RESERVE].active
                && loaded.statuses[PLAYER_STATUS_CAMP_RECOVERY].active,
            "native saves should round-trip all major survival statuses");
    Require(loaded.statuses[PLAYER_STATUS_FILTERED].magnitude == snapshot.statuses[PLAYER_STATUS_FILTERED].magnitude,
            "status magnitudes should round-trip through save and load");
    Require(loaded.statuses[PLAYER_STATUS_OXYGEN_RESERVE].remainingTime == snapshot.statuses[PLAYER_STATUS_OXYGEN_RESERVE].remainingTime,
            "status duration should round-trip through save and load");

    WriteLegacySaveV5(1, 22.0f, 77.0f, 0.0f, 88.0f, 14.0f);
    memset(&loaded, 0, sizeof(loaded));
    Require(SaveSystem_LoadGame(1, &loaded), "legacy v5 status load should succeed");
    Require(loaded.health == 22.0f, "legacy v5 should map stamina to health");
    Require(loaded.maxHealthBonus == 14.0f, "legacy v5 should map stamina bonus to max health bonus");
    Require(loaded.pressure == INITIAL_PRESSURE, "legacy v5 should discard old pressure into the new default");
    Require(loaded.statuses[PLAYER_STATUS_POISONED].active
                && loaded.statuses[PLAYER_STATUS_POISONED].level == 3,
            "legacy v5 should derive poisoned status from old poison");
    Require(loaded.statuses[PLAYER_STATUS_LOW_OXYGEN].active
                && loaded.statuses[PLAYER_STATUS_LOW_OXYGEN].level == 2,
            "legacy v5 should derive low oxygen from old oxygen values");
    Require(loaded.statuses[PLAYER_STATUS_SUFFOCATING].active,
            "legacy v5 should derive suffocating when old oxygen is zero");
    Require(loaded.statuses[PLAYER_STATUS_CRITICAL_CONDITION].active,
            "legacy v5 should derive critical condition from mapped low health");

    puts("save_status smoke ok");
    return 0;
}
