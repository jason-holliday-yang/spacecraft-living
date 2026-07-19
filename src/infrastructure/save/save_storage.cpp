#include "save_system.h"
#include "persistence_platform.h"
#include "save_internal.h"

#include <cstdio>
#include <cstring>

namespace {

char g_activeAccountKey[SAVE_ACCOUNT_NAME_MAX];
char g_activeAccountName[SAVE_ACCOUNT_NAME_MAX];

bool CopyString(char *buffer, size_t bufferSize, const char *value) {
    if (buffer == NULL || bufferSize == 0) {
        return false;
    }

    buffer[0] = '\0';
    if (value == NULL) {
        return true;
    }

    return std::snprintf(buffer, bufferSize, "%s", value) < (int)bufferSize;
}

void GetAccountsDirectoryPath(char *buffer, size_t bufferSize) {
    std::snprintf(buffer, bufferSize, "%s/accounts", SaveSystem_GetBaseDirectory());
}

}  // namespace

void SaveInternal_GetLegacyRootSavePath(char *buffer, size_t bufferSize) {
    std::snprintf(buffer, bufferSize, "%s/savegame.bin", SaveSystem_GetBaseDirectory());
}

bool SaveInternal_EnsureBaseDirectory(void) {
    return PersistencePlatform_EnsureDirectoryTree(SaveSystem_GetBaseDirectory());
}

bool SaveInternal_EnsureAccountsDirectory(void) {
    char accountsDirectory[640];

    if (!SaveInternal_EnsureBaseDirectory()) {
        return false;
    }

    GetAccountsDirectoryPath(accountsDirectory, sizeof(accountsDirectory));
    return PersistencePlatform_EnsureDirectoryTree(accountsDirectory);
}

bool SaveInternal_EnsureActiveAccountDirectory(void) {
    char accountDirectory[640];

    if (!SaveInternal_GetActiveAccountDirectory(accountDirectory, sizeof(accountDirectory))) {
        return false;
    }

    return SaveInternal_EnsureAccountsDirectory() && PersistencePlatform_EnsureDirectoryTree(accountDirectory);
}

void SaveInternal_GetSettingsPath(char *buffer, size_t bufferSize) {
    std::snprintf(buffer, bufferSize, "%s/settings.bin", SaveSystem_GetBaseDirectory());
}

void SaveInternal_GetAccountsPath(char *buffer, size_t bufferSize) {
    std::snprintf(buffer, bufferSize, "%s/accounts_registry.txt", SaveSystem_GetBaseDirectory());
}

bool SaveInternal_IsValidSlotIndex(int slotIndex) {
    return slotIndex >= 0 && slotIndex < SAVE_SLOT_COUNT;
}

bool SaveInternal_HasActiveAccount(void) {
    return g_activeAccountKey[0] != '\0';
}

bool SaveInternal_SetActiveAccount(const char *accountKey, const char *accountName) {
    if (accountKey == NULL || accountKey[0] == '\0') {
        return false;
    }

    if (!CopyString(g_activeAccountKey, sizeof(g_activeAccountKey), accountKey)) {
        return false;
    }
    if (!CopyString(g_activeAccountName, sizeof(g_activeAccountName), accountName)) {
        g_activeAccountKey[0] = '\0';
        g_activeAccountName[0] = '\0';
        return false;
    }

    return true;
}

void SaveInternal_ClearActiveAccount(void) {
    g_activeAccountKey[0] = '\0';
    g_activeAccountName[0] = '\0';
}

const char *SaveInternal_GetActiveAccountName(void) {
    return g_activeAccountName;
}

const char *SaveInternal_GetActiveAccountKey(void) {
    return g_activeAccountKey;
}

bool SaveInternal_GetActiveAccountDirectory(char *buffer, size_t bufferSize) {
    char accountsDirectory[640];

    if (buffer == NULL || bufferSize == 0 || !SaveInternal_HasActiveAccount()) {
        return false;
    }

    GetAccountsDirectoryPath(accountsDirectory, sizeof(accountsDirectory));
    return std::snprintf(buffer, bufferSize, "%s/%s", accountsDirectory, g_activeAccountKey) < (int)bufferSize;
}

void SaveInternal_GetLegacyRootSlotPath(int slotIndex, char *buffer, size_t bufferSize) {
    if (buffer == NULL || bufferSize == 0) {
        return;
    }

    if (!SaveInternal_IsValidSlotIndex(slotIndex)) {
        std::snprintf(buffer, bufferSize, "%s/invalid_legacy_slot.bin", SaveSystem_GetBaseDirectory());
        return;
    }

    std::snprintf(buffer, bufferSize, "%s/save_slot_%02d.bin", SaveSystem_GetBaseDirectory(), slotIndex + 1);
}

void SaveSystem_GetSlotPath(int slotIndex, char *buffer, size_t bufferSize) {
    char accountDirectory[640];

    if (buffer == NULL || bufferSize == 0) {
        return;
    }

    if (!SaveInternal_IsValidSlotIndex(slotIndex)) {
        std::snprintf(buffer, bufferSize, "%s/invalid_slot.bin", SaveSystem_GetBaseDirectory());
        return;
    }

    if (!SaveInternal_GetActiveAccountDirectory(accountDirectory, sizeof(accountDirectory))) {
        std::snprintf(buffer, bufferSize, "%s/no_account_slot_%02d.bin", SaveSystem_GetBaseDirectory(), slotIndex + 1);
        return;
    }

    std::snprintf(buffer, bufferSize, "%s/save_slot_%02d.bin", accountDirectory, slotIndex + 1);
}

void SaveInternal_MigrateLegacySaveIfNeeded(void) {
    char legacyPath[640];
    int slotIndex;

    if (!SaveInternal_HasActiveAccount() || !SaveInternal_EnsureActiveAccountDirectory()) {
        return;
    }

    SaveInternal_GetLegacyRootSavePath(legacyPath, sizeof(legacyPath));
    for (slotIndex = 0; slotIndex < SAVE_SLOT_COUNT; ++slotIndex) {
        char sourcePath[640];
        char targetPath[640];

        SaveInternal_GetLegacyRootSlotPath(slotIndex, sourcePath, sizeof(sourcePath));
        SaveSystem_GetSlotPath(slotIndex, targetPath, sizeof(targetPath));
        if (PersistencePlatform_FileExists(sourcePath) && !PersistencePlatform_FileExists(targetPath)) {
            PersistencePlatform_MoveFile(sourcePath, targetPath);
        }
    }

    SaveInternal_GetLegacyRootSavePath(legacyPath, sizeof(legacyPath));
    if (PersistencePlatform_FileExists(legacyPath)) {
        char firstSlotPath[640];

        SaveSystem_GetSlotPath(0, firstSlotPath, sizeof(firstSlotPath));
        if (!PersistencePlatform_FileExists(firstSlotPath)) {
            PersistencePlatform_MoveFile(legacyPath, firstSlotPath);
        }
    }
}

const char *SaveSystem_GetBaseDirectory(void) {
    static char baseDirectory[512];
    static bool initialized = false;

    if (!initialized) {
        if (!PersistencePlatform_GetAppDataDirectory("SpaceCraftLiving", baseDirectory, sizeof(baseDirectory))) {
            std::snprintf(baseDirectory, sizeof(baseDirectory), "./SpaceCraftLivingData");
        }

        initialized = true;
    }

    return baseDirectory;
}

bool SaveSystem_HasSaveInSlot(int slotIndex) {
    char path[640];

    if (!SaveInternal_HasActiveAccount()) {
        return false;
    }

    SaveInternal_MigrateLegacySaveIfNeeded();
    if (!SaveInternal_IsValidSlotIndex(slotIndex)) {
        return false;
    }

    SaveSystem_GetSlotPath(slotIndex, path, sizeof(path));
    return PersistencePlatform_FileExists(path);
}

bool SaveSystem_HasAnySave(void) {
    int slotIndex;

    if (!SaveInternal_HasActiveAccount()) {
        return false;
    }

    SaveInternal_MigrateLegacySaveIfNeeded();
    for (slotIndex = 0; slotIndex < SAVE_SLOT_COUNT; slotIndex++) {
        if (SaveSystem_HasSaveInSlot(slotIndex)) {
            return true;
        }
    }

    return false;
}
