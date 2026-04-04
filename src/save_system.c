#include "save_system.h"

#include <errno.h>
#include <float.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>

#define SETTINGS_MAGIC "SCLSET2"
#define SAVE_MAGIC "SCLSAV2"

typedef struct SettingsFileData {
    char magic[8];
    float masterVolume;
    uint8_t sfxEnabled;
} SettingsFileData;

typedef struct SaveFileRecord {
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
    uint8_t logsCollected[MAX_LOGS];
} SaveFileRecord;

static bool EnsureDirectory(const char *path) {
    struct stat info;

    if (stat(path, &info) == 0) {
        return S_ISDIR(info.st_mode);
    }

    return mkdir(path, 0755) == 0 || errno == EEXIST;
}

static bool EnsureBaseDirectory(void) {
    const char *home;
    char libraryDir[512];
    char supportDir[512];

    home = getenv("HOME");
    if (home == NULL || home[0] == '\0') {
        return false;
    }

    snprintf(libraryDir, sizeof(libraryDir), "%s/Library", home);
    snprintf(supportDir, sizeof(supportDir), "%s/Library/Application Support", home);

    return EnsureDirectory(libraryDir)
        && EnsureDirectory(supportDir)
        && EnsureDirectory(SaveSystem_GetBaseDirectory());
}

static void GetSettingsPath(char *buffer, size_t bufferSize) {
    snprintf(buffer, bufferSize, "%s/settings.bin", SaveSystem_GetBaseDirectory());
}

static void GetSavePath(char *buffer, size_t bufferSize) {
    snprintf(buffer, bufferSize, "%s/savegame.bin", SaveSystem_GetBaseDirectory());
}

static void GetTempPath(char *buffer, size_t bufferSize, const char *targetPath) {
    snprintf(buffer, bufferSize, "%s.tmp", targetPath);
}

static bool IsFiniteFloat(float value) {
    return value <= FLT_MAX && value >= -FLT_MAX;
}

static bool WriteBinaryFileAtomically(const char *path, const void *data, size_t size) {
    FILE *file;
    char tempPath[672];

    GetTempPath(tempPath, sizeof(tempPath), path);
    file = fopen(tempPath, "wb");
    if (file == NULL) {
        return false;
    }

    if (fwrite(data, size, 1, file) != 1) {
        fclose(file);
        remove(tempPath);
        return false;
    }

    if (fflush(file) != 0) {
        fclose(file);
        remove(tempPath);
        return false;
    }

    if (fclose(file) != 0) {
        remove(tempPath);
        return false;
    }

    if (rename(tempPath, path) != 0) {
        remove(tempPath);
        return false;
    }

    return true;
}

const char *SaveSystem_GetBaseDirectory(void) {
    static char baseDirectory[512];
    static bool initialized = false;

    if (!initialized) {
        const char *home;

        home = getenv("HOME");
        if (home != NULL && home[0] != '\0') {
            snprintf(baseDirectory, sizeof(baseDirectory), "%s/Library/Application Support/SpaceCraftLiving", home);
        } else {
            snprintf(baseDirectory, sizeof(baseDirectory), "./SpaceCraftLivingData");
        }

        initialized = true;
    }

    return baseDirectory;
}

void SaveSystem_SetDefaultSettings(GameSettings *settings) {
    settings->masterVolume = 0.80f;
    settings->sfxEnabled = true;
}

bool SaveSystem_LoadSettings(GameSettings *settings) {
    FILE *file;
    SettingsFileData data;
    char path[640];

    SaveSystem_SetDefaultSettings(settings);
    GetSettingsPath(path, sizeof(path));

    file = fopen(path, "rb");
    if (file == NULL) {
        return false;
    }

    if (fread(&data, sizeof(data), 1, file) != 1) {
        fclose(file);
        return false;
    }

    fclose(file);

    if (memcmp(data.magic, SETTINGS_MAGIC, 7) != 0 || !IsFiniteFloat(data.masterVolume)) {
        return false;
    }

    settings->masterVolume = data.masterVolume;
    if (settings->masterVolume < 0.0f) {
        settings->masterVolume = 0.0f;
    }
    if (settings->masterVolume > 1.0f) {
        settings->masterVolume = 1.0f;
    }
    settings->sfxEnabled = data.sfxEnabled != 0;
    return true;
}

bool SaveSystem_SaveSettings(const GameSettings *settings) {
    SettingsFileData data;
    char path[640];

    if (!EnsureBaseDirectory()) {
        return false;
    }

    memset(&data, 0, sizeof(data));
    memcpy(data.magic, SETTINGS_MAGIC, 7);
    data.masterVolume = settings->masterVolume;
    data.sfxEnabled = settings->sfxEnabled ? 1 : 0;

    GetSettingsPath(path, sizeof(path));
    return WriteBinaryFileAtomically(path, &data, sizeof(data));
}

bool SaveSystem_HasSave(void) {
    FILE *file;
    char path[640];

    GetSavePath(path, sizeof(path));
    file = fopen(path, "rb");
    if (file == NULL) {
        return false;
    }

    fclose(file);
    return true;
}

bool SaveSystem_LoadGame(SaveSnapshot *snapshot) {
    FILE *file;
    SaveFileRecord data;
    char path[640];
    int index;

    memset(snapshot, 0, sizeof(*snapshot));
    GetSavePath(path, sizeof(path));

    file = fopen(path, "rb");
    if (file == NULL) {
        return false;
    }

    if (fread(&data, sizeof(data), 1, file) != 1) {
        fclose(file);
        return false;
    }

    fclose(file);

    if (memcmp(data.magic, SAVE_MAGIC, 7) != 0) {
        return false;
    }

    if (!Map_IsWithinBounds(data.gridX, data.gridY) || !IsFiniteFloat(data.stamina) || !IsFiniteFloat(data.pressure)
        || !IsFiniteFloat(data.oxygen) || !IsFiniteFloat(data.poison) || !IsFiniteFloat(data.maxStaminaBonus)
        || !IsFiniteFloat(data.attackBonus) || !IsFiniteFloat(data.cycleTimer) || !IsFiniteFloat(data.elapsedSeconds)) {
        return false;
    }

    snapshot->gridX = data.gridX;
    snapshot->gridY = data.gridY;
    snapshot->facingX = data.facingX;
    snapshot->facingY = data.facingY;
    snapshot->stamina = data.stamina;
    snapshot->pressure = data.pressure;
    snapshot->oxygen = data.oxygen;
    snapshot->poison = data.poison;
    snapshot->maxStaminaBonus = data.maxStaminaBonus;
    snapshot->attackBonus = data.attackBonus;
    snapshot->deathCount = data.deathCount;
    snapshot->crouching = data.crouching != 0;
    snapshot->hasGlowStick = data.hasGlowStick != 0;
    snapshot->hasRope = data.hasRope != 0;
    snapshot->hasLaserGun = data.hasLaserGun != 0;
    snapshot->hasProtectionSuit = data.hasProtectionSuit != 0;
    snapshot->hasSignalAmplifier = data.hasSignalAmplifier != 0;
    snapshot->hasFieldCamp = data.hasFieldCamp != 0;

    for (index = 0; index < RESOURCE_COUNT; index++) {
        snapshot->resources[index] = data.resources[index];
    }

    snapshot->stage = data.stage;
    snapshot->dayCount = data.dayCount;
    snapshot->phase = data.phase;
    snapshot->currentEvent = data.currentEvent;
    snapshot->cycleTimer = data.cycleTimer;
    snapshot->elapsedSeconds = data.elapsedSeconds;
    snapshot->oxygenRepairLevel = data.oxygenRepairLevel;
    snapshot->commRepairLevel = data.commRepairLevel;
    snapshot->energyRepairLevel = data.energyRepairLevel;
    snapshot->crashClueFound = data.crashClueFound != 0;
    snapshot->amplifierUnlocked = data.amplifierUnlocked != 0;
    snapshot->bossDefeated = data.bossDefeated != 0;
    snapshot->signalTowerActivated = data.signalTowerActivated != 0;
    snapshot->monolithActivated[0] = data.monolithActivated[0] != 0;
    snapshot->monolithActivated[1] = data.monolithActivated[1] != 0;
    snapshot->monolithActivated[2] = data.monolithActivated[2] != 0;
    snapshot->monolithsLit = data.monolithsLit;
    snapshot->ending = data.ending;
    snapshot->campPlaced = data.campPlaced != 0;
    snapshot->campX = data.campX;
    snapshot->campY = data.campY;

    for (index = 0; index < MAX_RESOURCE_NODES; index++) {
        snapshot->nodes[index].active = data.nodeActive[index] != 0;
        snapshot->nodes[index].respawnsRemaining = data.nodeRespawns[index];
    }
    for (index = 0; index < MAX_MONSTERS; index++) {
        snapshot->monsters[index].active = data.monsterActive[index] != 0;
        snapshot->monsters[index].gridX = data.monsterGridX[index];
        snapshot->monsters[index].gridY = data.monsterGridY[index];
        snapshot->monsters[index].health = data.monsterHealth[index];
        snapshot->monsters[index].phaseTriggered = data.monsterPhaseTriggered[index] != 0;
    }
    for (index = 0; index < MAX_LOGS; index++) {
        snapshot->logs[index].collected = data.logsCollected[index] != 0;
    }

    return true;
}

bool SaveSystem_SaveGame(const SaveSnapshot *snapshot) {
    SaveFileRecord data;
    char path[640];
    int index;

    if (!EnsureBaseDirectory()) {
        return false;
    }

    memset(&data, 0, sizeof(data));
    memcpy(data.magic, SAVE_MAGIC, 7);
    data.gridX = snapshot->gridX;
    data.gridY = snapshot->gridY;
    data.facingX = snapshot->facingX;
    data.facingY = snapshot->facingY;
    data.stamina = snapshot->stamina;
    data.pressure = snapshot->pressure;
    data.oxygen = snapshot->oxygen;
    data.poison = snapshot->poison;
    data.maxStaminaBonus = snapshot->maxStaminaBonus;
    data.attackBonus = snapshot->attackBonus;
    data.deathCount = snapshot->deathCount;
    data.crouching = snapshot->crouching ? 1 : 0;
    data.hasGlowStick = snapshot->hasGlowStick ? 1 : 0;
    data.hasRope = snapshot->hasRope ? 1 : 0;
    data.hasLaserGun = snapshot->hasLaserGun ? 1 : 0;
    data.hasProtectionSuit = snapshot->hasProtectionSuit ? 1 : 0;
    data.hasSignalAmplifier = snapshot->hasSignalAmplifier ? 1 : 0;
    data.hasFieldCamp = snapshot->hasFieldCamp ? 1 : 0;

    for (index = 0; index < RESOURCE_COUNT; index++) {
        data.resources[index] = snapshot->resources[index];
    }

    data.stage = snapshot->stage;
    data.dayCount = snapshot->dayCount;
    data.phase = snapshot->phase;
    data.currentEvent = snapshot->currentEvent;
    data.cycleTimer = snapshot->cycleTimer;
    data.elapsedSeconds = snapshot->elapsedSeconds;
    data.oxygenRepairLevel = snapshot->oxygenRepairLevel;
    data.commRepairLevel = snapshot->commRepairLevel;
    data.energyRepairLevel = snapshot->energyRepairLevel;
    data.crashClueFound = snapshot->crashClueFound ? 1 : 0;
    data.amplifierUnlocked = snapshot->amplifierUnlocked ? 1 : 0;
    data.bossDefeated = snapshot->bossDefeated ? 1 : 0;
    data.signalTowerActivated = snapshot->signalTowerActivated ? 1 : 0;
    data.monolithActivated[0] = snapshot->monolithActivated[0] ? 1 : 0;
    data.monolithActivated[1] = snapshot->monolithActivated[1] ? 1 : 0;
    data.monolithActivated[2] = snapshot->monolithActivated[2] ? 1 : 0;
    data.monolithsLit = snapshot->monolithsLit;
    data.ending = snapshot->ending;
    data.campPlaced = snapshot->campPlaced ? 1 : 0;
    data.campX = snapshot->campX;
    data.campY = snapshot->campY;

    for (index = 0; index < MAX_RESOURCE_NODES; index++) {
        data.nodeActive[index] = snapshot->nodes[index].active ? 1 : 0;
        data.nodeRespawns[index] = snapshot->nodes[index].respawnsRemaining;
    }
    for (index = 0; index < MAX_MONSTERS; index++) {
        data.monsterActive[index] = snapshot->monsters[index].active ? 1 : 0;
        data.monsterGridX[index] = snapshot->monsters[index].gridX;
        data.monsterGridY[index] = snapshot->monsters[index].gridY;
        data.monsterHealth[index] = snapshot->monsters[index].health;
        data.monsterPhaseTriggered[index] = snapshot->monsters[index].phaseTriggered ? 1 : 0;
    }
    for (index = 0; index < MAX_LOGS; index++) {
        data.logsCollected[index] = snapshot->logs[index].collected ? 1 : 0;
    }

    GetSavePath(path, sizeof(path));
    return WriteBinaryFileAtomically(path, &data, sizeof(data));
}
