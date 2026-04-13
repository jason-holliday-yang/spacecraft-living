#include "save_internal.h"
#include "persistence_platform.h"
#include "save_legacy_adapter.h"

#include <cstdint>
#include <cstring>

static bool IsSupportedSaveMagic(const char *magic) {
    return magic != NULL
        && (SaveInternal_IsNativeSaveMagic(magic)
            || SaveLegacyAdapter_IsSupportedMagic(magic));
}

static bool ReadSaveHeader(const char *path, char *magic, size_t magicSize, size_t *fileSizeOut) {
    if (path == NULL || magic == NULL || magicSize < 8) {
        return false;
    }

    std::memset(magic, 0, magicSize);
    return PersistencePlatform_ReadFileAtMost(path, magic, 8, fileSizeOut) >= 8;
}

bool SaveInternal_LoadSnapshotFromPath(const char *path, SaveSnapshot *snapshot) {
    char magic[8];
    size_t fileSize;

    if (path == NULL || snapshot == NULL) {
        return false;
    }

    std::memset(snapshot, 0, sizeof(*snapshot));
    if (!ReadSaveHeader(path, magic, sizeof(magic), &fileSize)) {
        return false;
    }

    if (!IsSupportedSaveMagic(magic)) {
        return false;
    }

    if (SaveInternal_IsNativeSaveMagic(magic)) {
        uint8_t buffer[SAVE_INTERNAL_MAX_SAVE_FILE_SIZE];
        size_t bytesRead;

        bytesRead = PersistencePlatform_ReadFileAtMost(path, buffer, sizeof(buffer), &fileSize);
        if (bytesRead != fileSize || fileSize > sizeof(buffer) || !SaveInternal_DecodeSnapshotBuffer(snapshot, buffer, fileSize)) {
            return false;
        }
    } else {
        if (!SaveLegacyAdapter_LoadSnapshot(path, magic, fileSize, snapshot)) {
            return false;
        }
    }

    if (!SaveInternal_IsSnapshotBinaryDataValid(snapshot)) {
        return false;
    }

    SaveInternal_SanitizeSnapshot(snapshot);
    return true;
}
