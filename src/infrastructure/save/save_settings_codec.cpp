#include "save_codec_internal.h"

#include <cstring>

typedef struct SettingsFileData {
    char magic[8];
    float masterVolume;
    uint8_t sfxEnabled;
} SettingsFileData;

static bool GameSettings_FromLegacyData(GameSettings *settings, const SettingsFileData *data) {
    if (settings == NULL || data == NULL) {
        return false;
    }

    if (std::memcmp(data->magic, SETTINGS_MAGIC_V2, 7) != 0 || !SaveCodec_IsFiniteFloat(data->masterVolume)) {
        return false;
    }

    settings->masterVolume = data->masterVolume;
    if (settings->masterVolume < 0.0f) {
        settings->masterVolume = 0.0f;
    }
    if (settings->masterVolume > 1.0f) {
        settings->masterVolume = 1.0f;
    }
    settings->musicVolume = 1.0f;
    settings->sfxVolume = data->sfxEnabled != 0 ? 1.0f : 0.0f;
    settings->sfxEnabled = data->sfxEnabled != 0;
    settings->language = GAME_LANGUAGE_EN;
    settings->lastUsername[0] = '\0';
    return true;
}

bool SaveInternal_EncodeSettingsBuffer(const GameSettings *settings, uint8_t *buffer, size_t bufferSize, size_t *bytesWritten) {
    SaveBufferWriter writer;
    float masterVolume;
    float musicVolume;
    float sfxVolume;

    if (settings == NULL || buffer == NULL) {
        return false;
    }

    masterVolume = settings->masterVolume;
    if (!(masterVolume >= 0.0f)) {
        masterVolume = 0.0f;
    }
    if (masterVolume > 1.0f) {
        masterVolume = 1.0f;
    }
    musicVolume = settings->musicVolume;
    if (!(musicVolume >= 0.0f)) {
        musicVolume = 0.0f;
    }
    if (musicVolume > 1.0f) {
        musicVolume = 1.0f;
    }
    sfxVolume = settings->sfxVolume;
    if (!(sfxVolume >= 0.0f)) {
        sfxVolume = 0.0f;
    }
    if (sfxVolume > 1.0f) {
        sfxVolume = 1.0f;
    }

    std::memset(buffer, 0, bufferSize);
    SaveWriter_Init(&writer, buffer, bufferSize);
    SaveWriter_WriteBytes(&writer, SETTINGS_MAGIC_CURRENT, 8);
    SaveWriter_WriteFloat(&writer, masterVolume);
    SaveWriter_WriteFloat(&writer, musicVolume);
    SaveWriter_WriteFloat(&writer, sfxVolume);
    SaveWriter_WriteUInt8(&writer, settings->sfxEnabled ? 1u : 0u);
    SaveWriter_WriteUInt8(&writer, (uint8_t)Loc_NormalizeLanguage((int)settings->language));
    SaveWriter_WriteBytes(&writer, settings->lastUsername, SAVE_ACCOUNT_NAME_MAX);

    if (writer.failed) {
        return false;
    }

    if (bytesWritten != NULL) {
        *bytesWritten = writer.offset;
    }
    return true;
}

bool SaveInternal_DecodeSettingsBuffer(GameSettings *settings, const uint8_t *buffer, size_t fileSize) {
    SaveBufferReader reader;
    char magic[8];

    if (settings == NULL || buffer == NULL) {
        return false;
    }

    SaveSystem_SetDefaultSettings(settings);
    SaveReader_Init(&reader, buffer, fileSize);
    std::memset(magic, 0, sizeof(magic));
    SaveReader_ReadBytes(&reader, magic, sizeof(magic));
    if (reader.failed) {
        return false;
    }

    if (std::memcmp(magic, SETTINGS_MAGIC_CURRENT, 7) == 0) {
        settings->masterVolume = SaveReader_ReadFloat(&reader);
        settings->musicVolume = SaveReader_ReadFloat(&reader);
        settings->sfxVolume = SaveReader_ReadFloat(&reader);
        settings->sfxEnabled = SaveReader_ReadUInt8(&reader) != 0;
        settings->language = Loc_NormalizeLanguage((int)SaveReader_ReadUInt8(&reader));
        SaveReader_ReadBytes(&reader, settings->lastUsername, SAVE_ACCOUNT_NAME_MAX);
        if (reader.failed
            || reader.offset != fileSize
            || !SaveCodec_IsFiniteFloat(settings->masterVolume)
            || !SaveCodec_IsFiniteFloat(settings->musicVolume)
            || !SaveCodec_IsFiniteFloat(settings->sfxVolume)) {
            return false;
        }

        if (settings->masterVolume < 0.0f) {
            settings->masterVolume = 0.0f;
        }
        if (settings->masterVolume > 1.0f) {
            settings->masterVolume = 1.0f;
        }
        if (settings->musicVolume < 0.0f) {
            settings->musicVolume = 0.0f;
        }
        if (settings->musicVolume > 1.0f) {
            settings->musicVolume = 1.0f;
        }
        if (settings->sfxVolume < 0.0f) {
            settings->sfxVolume = 0.0f;
        }
        if (settings->sfxVolume > 1.0f) {
            settings->sfxVolume = 1.0f;
        }
        settings->lastUsername[SAVE_ACCOUNT_NAME_MAX - 1] = '\0';
        settings->sfxEnabled = settings->sfxEnabled && settings->sfxVolume > 0.001f;
        return true;
    }

    if (std::memcmp(magic, SETTINGS_MAGIC_V5, 7) == 0) {
        SaveBufferReader legacyReader;

        SaveSystem_SetDefaultSettings(settings);
        SaveReader_Init(&legacyReader, buffer, fileSize);
        SaveReader_ReadBytes(&legacyReader, magic, sizeof(magic));
        settings->masterVolume = SaveReader_ReadFloat(&legacyReader);
        settings->sfxEnabled = SaveReader_ReadUInt8(&legacyReader) != 0;
        settings->language = Loc_NormalizeLanguage((int)SaveReader_ReadUInt8(&legacyReader));
        SaveReader_ReadBytes(&legacyReader, settings->lastUsername, SAVE_ACCOUNT_NAME_MAX);
        if (legacyReader.failed || legacyReader.offset != fileSize || !SaveCodec_IsFiniteFloat(settings->masterVolume)) {
            return false;
        }
        if (settings->masterVolume < 0.0f) {
            settings->masterVolume = 0.0f;
        }
        if (settings->masterVolume > 1.0f) {
            settings->masterVolume = 1.0f;
        }
        settings->musicVolume = 1.0f;
        settings->sfxVolume = settings->sfxEnabled ? 1.0f : 0.0f;
        settings->lastUsername[SAVE_ACCOUNT_NAME_MAX - 1] = '\0';
        return true;
    }

    if (std::memcmp(magic, SETTINGS_MAGIC_V3, 7) == 0) {
        SaveBufferReader legacyReader;

        SaveSystem_SetDefaultSettings(settings);
        SaveReader_Init(&legacyReader, buffer, fileSize);
        SaveReader_ReadBytes(&legacyReader, magic, sizeof(magic));
        settings->masterVolume = SaveReader_ReadFloat(&legacyReader);
        settings->sfxEnabled = SaveReader_ReadUInt8(&legacyReader) != 0;
        settings->language = GAME_LANGUAGE_EN;
        if (legacyReader.failed || legacyReader.offset != fileSize || !SaveCodec_IsFiniteFloat(settings->masterVolume)) {
            return false;
        }
        if (settings->masterVolume < 0.0f) {
            settings->masterVolume = 0.0f;
        }
        if (settings->masterVolume > 1.0f) {
            settings->masterVolume = 1.0f;
        }
        settings->musicVolume = 1.0f;
        settings->sfxVolume = settings->sfxEnabled ? 1.0f : 0.0f;
        return true;
    }

    if (std::memcmp(magic, SETTINGS_MAGIC_V2, 7) == 0 && fileSize == sizeof(SettingsFileData)) {
        SettingsFileData legacyData;

        std::memset(&legacyData, 0, sizeof(legacyData));
        std::memcpy(&legacyData, buffer, sizeof(legacyData));
        return GameSettings_FromLegacyData(settings, &legacyData);
    }

    return false;
}
