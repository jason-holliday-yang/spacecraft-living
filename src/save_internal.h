#ifndef SAVE_INTERNAL_H
#define SAVE_INTERNAL_H

#include <stddef.h>
#include <stdint.h>

#include "save_system.h"

/* Internal save/settings codec helpers shared by persistence slices. */

#define SAVE_INTERNAL_SETTINGS_MAX_FILE_SIZE 64
#define SAVE_INTERNAL_MAX_SAVE_FILE_SIZE 8192

SCL_EXTERN_C_BEGIN

bool SaveInternal_IsNativeSaveMagic(const char *magic);
bool SaveInternal_DecodeSettingsBuffer(GameSettings *settings, const uint8_t *buffer, size_t fileSize);
bool SaveInternal_EncodeSettingsBuffer(const GameSettings *settings, uint8_t *buffer, size_t bufferSize, size_t *bytesWritten);
bool SaveInternal_DecodeSnapshotBuffer(SaveSnapshot *snapshot, const uint8_t *buffer, size_t fileSize);
bool SaveInternal_EncodeSnapshotBuffer(const SaveSnapshot *snapshot, uint8_t *buffer, size_t bufferSize, size_t *bytesWritten);
bool SaveInternal_LoadSnapshotFromPath(const char *path, SaveSnapshot *snapshot);
bool SaveInternal_EnsureBaseDirectory(void);
bool SaveInternal_EnsureAccountsDirectory(void);
bool SaveInternal_EnsureActiveAccountDirectory(void);
void SaveInternal_GetSettingsPath(char *buffer, size_t bufferSize);
void SaveInternal_GetAccountsPath(char *buffer, size_t bufferSize);
bool SaveInternal_IsValidSlotIndex(int slotIndex);
void SaveInternal_MigrateLegacySaveIfNeeded(void);
bool SaveInternal_IsSnapshotBinaryDataValid(const SaveSnapshot *snapshot);
void SaveInternal_SanitizeSnapshot(SaveSnapshot *snapshot);
bool SaveInternal_HasActiveAccount(void);
bool SaveInternal_SetActiveAccount(const char *accountKey, const char *accountName);
void SaveInternal_ClearActiveAccount(void);
const char *SaveInternal_GetActiveAccountName(void);
const char *SaveInternal_GetActiveAccountKey(void);
bool SaveInternal_GetActiveAccountDirectory(char *buffer, size_t bufferSize);
void SaveInternal_GetLegacyRootSlotPath(int slotIndex, char *buffer, size_t bufferSize);
void SaveInternal_GetLegacyRootSavePath(char *buffer, size_t bufferSize);

SCL_EXTERN_C_END

#endif
