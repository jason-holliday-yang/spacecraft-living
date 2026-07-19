#include "save_system.h"
#include "localization.h"
#include "persistence_platform.h"
#include "save_internal.h"

#include <cctype>
#include <cstdio>
#include <cstdint>
#include <cstring>
#include <string>
#include <vector>

namespace {

struct AccountRecord {
    std::string key;
    std::string displayName;
    std::string passwordHash;
};

constexpr const char *kAccountsMagic = "SCLAUTH1";
constexpr const char *kAccountScoreMagic = "SCLSCR1";
constexpr size_t kAccountsFileMaxSize = 16384;
constexpr size_t kMinUsernameLength = 3;
constexpr size_t kMinPasswordLength = 4;

struct AccountScoreData {
    char magic[8];
    int32_t bestScore;
};

void WriteAuthMessage(char *buffer, size_t bufferSize, const char *message) {
    const char *displayMessage;

    if (buffer == NULL || bufferSize == 0) {
        return;
    }

    buffer[0] = '\0';
    if (message == NULL) {
        return;
    }

    displayMessage = Loc_Translate(message);
    std::snprintf(buffer, bufferSize, "%s", displayMessage);
}

std::string TrimWhitespace(const char *value) {
    std::string text = value == NULL ? "" : value;
    size_t start = 0;
    size_t end = text.size();

    while (start < end && std::isspace(static_cast<unsigned char>(text[start])) != 0) {
        start += 1;
    }
    while (end > start && std::isspace(static_cast<unsigned char>(text[end - 1])) != 0) {
        end -= 1;
    }

    return text.substr(start, end - start);
}

bool IsUsernameCharacterAllowed(char character) {
    unsigned char value = static_cast<unsigned char>(character);
    return std::isalnum(value) != 0 || character == '_' || character == '-';
}

bool ValidateUsername(const char *username,
                      std::string *normalizedUsername,
                      std::string *displayUsername,
                      char *message,
                      size_t messageSize) {
    std::string trimmed;
    std::string normalized;

    trimmed = TrimWhitespace(username);
    if (trimmed.size() < kMinUsernameLength) {
        WriteAuthMessage(message, messageSize, "Username must be at least 3 characters.");
        return false;
    }
    if (trimmed.size() >= SAVE_ACCOUNT_NAME_MAX) {
        WriteAuthMessage(message, messageSize, "Username is too long.");
        return false;
    }

    normalized.reserve(trimmed.size());
    for (char character : trimmed) {
        if (!IsUsernameCharacterAllowed(character)) {
            WriteAuthMessage(message, messageSize, "Username can only use letters, numbers, '_' or '-'.");
            return false;
        }

        normalized.push_back((char)std::tolower(static_cast<unsigned char>(character)));
    }

    if (normalizedUsername != NULL) {
        *normalizedUsername = normalized;
    }
    if (displayUsername != NULL) {
        *displayUsername = trimmed;
    }
    return true;
}

bool ValidatePassword(const char *password, char *message, size_t messageSize) {
    std::string value;

    value = password == NULL ? "" : password;
    if (value.size() < kMinPasswordLength) {
        WriteAuthMessage(message, messageSize, "Password must be at least 4 characters.");
        return false;
    }
    if (value.size() >= SAVE_ACCOUNT_PASSWORD_MAX) {
        WriteAuthMessage(message, messageSize, "Password is too long.");
        return false;
    }

    for (char character : value) {
        unsigned char byte = static_cast<unsigned char>(character);

        if (byte < 32 || byte > 126) {
            WriteAuthMessage(message, messageSize, "Password must use visible ASCII characters.");
            return false;
        }
    }

    return true;
}

std::string HashPassword(const std::string &normalizedUsername, const std::string &password) {
    uint64_t hash = 1469598103934665603ull;
    const std::string seeded = std::string("SpaceCraftLiving::") + normalizedUsername + "::" + password;
    char buffer[24];

    for (unsigned char byte : seeded) {
        hash ^= (uint64_t)byte;
        hash *= 1099511628211ull;
    }

    std::snprintf(buffer, sizeof(buffer), "%016llx", (unsigned long long)hash);
    return buffer;
}

bool LoadAccountRecords(std::vector<AccountRecord> *records) {
    char path[640];
    uint8_t buffer[kAccountsFileMaxSize];
    size_t fileSize;
    size_t bytesRead;
    std::string content;
    size_t cursor;

    if (records == NULL) {
        return false;
    }

    records->clear();
    SaveInternal_GetAccountsPath(path, sizeof(path));
    if (!PersistencePlatform_FileExists(path)) {
        return true;
    }

    fileSize = 0;
    bytesRead = PersistencePlatform_ReadFileAtMost(path, buffer, sizeof(buffer), &fileSize);
    if (bytesRead == 0 || bytesRead != fileSize || fileSize > sizeof(buffer)) {
        return false;
    }

    content.assign(reinterpret_cast<const char *>(buffer), fileSize);
    cursor = 0;

    {
        size_t lineEnd = content.find('\n', cursor);
        std::string header = content.substr(cursor, lineEnd == std::string::npos ? std::string::npos : lineEnd - cursor);

        if (header != kAccountsMagic) {
            return false;
        }

        cursor = lineEnd == std::string::npos ? content.size() : lineEnd + 1;
    }

    while (cursor < content.size()) {
        size_t lineEnd = content.find('\n', cursor);
        std::string line = content.substr(cursor, lineEnd == std::string::npos ? std::string::npos : lineEnd - cursor);

        cursor = lineEnd == std::string::npos ? content.size() : lineEnd + 1;
        if (line.empty()) {
            continue;
        }

        {
            size_t firstPipe = line.find('|');
            size_t secondPipe = firstPipe == std::string::npos ? std::string::npos : line.find('|', firstPipe + 1);

            if (firstPipe == std::string::npos || secondPipe == std::string::npos) {
                return false;
            }

            records->push_back(AccountRecord{
                line.substr(0, firstPipe),
                line.substr(firstPipe + 1, secondPipe - firstPipe - 1),
                line.substr(secondPipe + 1)
            });
        }
    }

    return true;
}

bool SaveAccountRecords(const std::vector<AccountRecord> &records) {
    char path[640];
    std::string content;

    if (!SaveInternal_EnsureBaseDirectory()) {
        return false;
    }

    content.append(kAccountsMagic);
    content.push_back('\n');
    for (const AccountRecord &record : records) {
        content.append(record.key);
        content.push_back('|');
        content.append(record.displayName);
        content.push_back('|');
        content.append(record.passwordHash);
        content.push_back('\n');
    }

    SaveInternal_GetAccountsPath(path, sizeof(path));
    return PersistencePlatform_WriteFileAtomically(path, content.data(), content.size());
}

const AccountRecord *FindAccountRecord(const std::vector<AccountRecord> &records, const std::string &normalizedUsername) {
    for (const AccountRecord &record : records) {
        if (record.key == normalizedUsername) {
            return &record;
        }
    }

    return NULL;
}

bool ActivateAccount(const AccountRecord &record) {
    if (!SaveInternal_SetActiveAccount(record.key.c_str(), record.displayName.c_str())) {
        return false;
    }

    if (!SaveInternal_EnsureActiveAccountDirectory()) {
        SaveInternal_ClearActiveAccount();
        return false;
    }

    SaveInternal_MigrateLegacySaveIfNeeded();
    return true;
}

bool GetActiveAccountScorePath(char *buffer, size_t bufferSize) {
    char accountDirectory[640];

    if (buffer == NULL || bufferSize == 0 || !SaveInternal_GetActiveAccountDirectory(accountDirectory, sizeof(accountDirectory))) {
        return false;
    }

    return std::snprintf(buffer, bufferSize, "%s/account_score.bin", accountDirectory) < (int)bufferSize;
}

bool LoadAccountScore(int *scoreOut) {
    AccountScoreData data;
    char path[640];
    size_t fileSize;
    size_t bytesRead;

    if (scoreOut == NULL || !GetActiveAccountScorePath(path, sizeof(path)) || !PersistencePlatform_FileExists(path)) {
        return false;
    }

    fileSize = 0;
    bytesRead = PersistencePlatform_ReadFileAtMost(path, &data, sizeof(data), &fileSize);
    if (bytesRead != sizeof(data) || fileSize != sizeof(data) || std::memcmp(data.magic, kAccountScoreMagic, 8) != 0) {
        return false;
    }

    if (data.bestScore <= 0) {
        return false;
    }

    *scoreOut = data.bestScore;
    return true;
}

bool SaveAccountScore(int score) {
    AccountScoreData data;
    char path[640];

    if (score <= 0 || !SaveInternal_EnsureActiveAccountDirectory() || !GetActiveAccountScorePath(path, sizeof(path))) {
        return false;
    }

    std::memset(&data, 0, sizeof(data));
    std::memcpy(data.magic, kAccountScoreMagic, 8);
    data.bestScore = score;
    return PersistencePlatform_WriteFileAtomically(path, &data, sizeof(data));
}

std::vector<AccountRecord>::iterator FindAccountRecordIterator(std::vector<AccountRecord> *records, const std::string &normalizedUsername) {
    if (records == NULL) {
        return std::vector<AccountRecord>::iterator();
    }

    for (std::vector<AccountRecord>::iterator it = records->begin(); it != records->end(); ++it) {
        if (it->key == normalizedUsername) {
            return it;
        }
    }

    return records->end();
}

bool DeleteAccountByKey(const std::string &accountKey, char *message, size_t messageSize) {
    std::vector<AccountRecord> records;
    std::vector<AccountRecord> updatedRecords;
    std::vector<AccountRecord>::iterator recordIt;
    char accountDirectory[640];
    std::string displayName;

    if (!LoadAccountRecords(&records)) {
        WriteAuthMessage(message, messageSize, "Couldn't read local account data.");
        return false;
    }

    updatedRecords = records;
    recordIt = FindAccountRecordIterator(&updatedRecords, accountKey);
    if (recordIt == updatedRecords.end()) {
        WriteAuthMessage(message, messageSize, "That account could not be found.");
        return false;
    }

    displayName = recordIt->displayName;
    updatedRecords.erase(recordIt);
    if (!SaveAccountRecords(updatedRecords)) {
        WriteAuthMessage(message, messageSize, "Couldn't remove the account from the local registry.");
        return false;
    }

    if (std::snprintf(accountDirectory, sizeof(accountDirectory), "%s/accounts/%s", SaveSystem_GetBaseDirectory(), accountKey.c_str()) >= (int)sizeof(accountDirectory)
        || !PersistencePlatform_DeleteDirectoryTree(accountDirectory)) {
        SaveAccountRecords(records);
        WriteAuthMessage(message, messageSize, "Couldn't delete this account's save data.");
        return false;
    }

    if (SaveInternal_HasActiveAccount() && accountKey == SaveInternal_GetActiveAccountKey()) {
        SaveInternal_ClearActiveAccount();
    }

    {
        char successMessage[SAVE_AUTH_MESSAGE_MAX];
        const char *format;

        format = Loc_PickLiteral("Account %s deleted. All saves under it were removed.",
                                 "账号 %s 已删除，其下所有存档也已移除。");
        std::snprintf(successMessage, sizeof(successMessage), format, displayName.c_str());
        WriteAuthMessage(message, messageSize, successMessage);
    }
    return true;
}

}  // namespace

void SaveSystem_SetDefaultSettings(GameSettings *settings) {
    if (settings == NULL) {
        return;
    }

    settings->masterVolume = 1.0f;
    settings->musicVolume = 1.0f;
    settings->sfxVolume = 1.0f;
    settings->sfxEnabled = true;
    settings->language = GAME_LANGUAGE_EN;
    settings->lastUsername[0] = '\0';
}

bool SaveSystem_LoadSettings(GameSettings *settings) {
    uint8_t buffer[SAVE_INTERNAL_SETTINGS_MAX_FILE_SIZE];
    char path[640];
    size_t fileSize;
    size_t bytesRead;

    if (settings == NULL) {
        return false;
    }

    SaveSystem_SetDefaultSettings(settings);
    SaveInternal_GetSettingsPath(path, sizeof(path));

    fileSize = 0;
    bytesRead = PersistencePlatform_ReadFileAtMost(path, buffer, sizeof(buffer), &fileSize);
    if (bytesRead == 0 || bytesRead != fileSize || fileSize > sizeof(buffer)) {
        return false;
    }

    return SaveInternal_DecodeSettingsBuffer(settings, buffer, fileSize);
}

bool SaveSystem_SaveSettings(const GameSettings *settings) {
    uint8_t buffer[SAVE_INTERNAL_SETTINGS_MAX_FILE_SIZE];
    char path[640];
    size_t bytesWritten;

    if (settings == NULL || !SaveInternal_EnsureBaseDirectory()) {
        return false;
    }

    SaveInternal_GetSettingsPath(path, sizeof(path));
    if (!SaveInternal_EncodeSettingsBuffer(settings, buffer, sizeof(buffer), &bytesWritten)) {
        return false;
    }

    return PersistencePlatform_WriteFileAtomically(path, buffer, bytesWritten);
}

bool SaveSystem_HasRegisteredAccounts(void) {
    std::vector<AccountRecord> records;

    return LoadAccountRecords(&records) && !records.empty();
}

int SaveSystem_ListRegisteredAccounts(char *names, size_t nameStride, int maxNames) {
    std::vector<AccountRecord> records;
    int copyCount;
    int index;

    if (names == NULL || nameStride == 0 || maxNames <= 0 || !LoadAccountRecords(&records)) {
        return 0;
    }

    copyCount = (int)records.size();
    if (copyCount > maxNames) {
        copyCount = maxNames;
    }

    for (index = 0; index < copyCount; ++index) {
        char *destination = names + (size_t)index * nameStride;

        std::snprintf(destination, nameStride, "%s", records[(size_t)index].displayName.c_str());
    }

    return copyCount;
}

bool SaveSystem_IsAccountAuthenticated(void) {
    return SaveInternal_HasActiveAccount();
}

const char *SaveSystem_GetActiveAccountName(void) {
    return SaveInternal_GetActiveAccountName();
}

bool SaveSystem_GetActiveAccountBestScore(int *scoreOut) {
    int bestScore;

    if (scoreOut == NULL || !SaveInternal_HasActiveAccount()) {
        return false;
    }

    bestScore = 0;
    if (!LoadAccountScore(&bestScore)) {
        return false;
    }

    *scoreOut = bestScore;
    return true;
}

bool SaveSystem_UpdateActiveAccountBestScore(int score) {
    int existingScore;

    if (!SaveInternal_HasActiveAccount() || score <= 0) {
        return false;
    }

    existingScore = 0;
    if (LoadAccountScore(&existingScore) && existingScore >= score) {
        return true;
    }

    return SaveAccountScore(score);
}

void SaveSystem_Logout(void) {
    SaveInternal_ClearActiveAccount();
}

bool SaveSystem_Login(const char *username, const char *password, char *message, size_t messageSize) {
    std::string normalizedUsername;
    std::string displayUsername;
    std::vector<AccountRecord> records;
    const AccountRecord *record;

    if (!ValidateUsername(username, &normalizedUsername, &displayUsername, message, messageSize)
        || !ValidatePassword(password, message, messageSize)) {
        return false;
    }

    if (!LoadAccountRecords(&records)) {
        WriteAuthMessage(message, messageSize, "Couldn't read local account data.");
        return false;
    }

    record = FindAccountRecord(records, normalizedUsername);
    if (record == NULL) {
        WriteAuthMessage(message, messageSize, "Account not found. Create it first.");
        return false;
    }

    if (record->passwordHash != HashPassword(normalizedUsername, password == NULL ? "" : password)) {
        WriteAuthMessage(message, messageSize, "Incorrect password.");
        return false;
    }

    if (!ActivateAccount(*record)) {
        WriteAuthMessage(message, messageSize, "Couldn't open this account's save folder.");
        return false;
    }

    {
        char successMessage[SAVE_AUTH_MESSAGE_MAX];
        const char *format;

        format = Loc_PickLiteral("Logged in as %s.", "已登录账号：%s。");
        std::snprintf(successMessage, sizeof(successMessage), format, record->displayName.c_str());
        WriteAuthMessage(message, messageSize, successMessage);
    }
    return true;
}

bool SaveSystem_Register(const char *username, const char *password, char *message, size_t messageSize) {
    std::string normalizedUsername;
    std::string displayUsername;
    std::vector<AccountRecord> records;
    AccountRecord newRecord;

    if (!ValidateUsername(username, &normalizedUsername, &displayUsername, message, messageSize)
        || !ValidatePassword(password, message, messageSize)) {
        return false;
    }

    if (!LoadAccountRecords(&records)) {
        WriteAuthMessage(message, messageSize, "Couldn't read local account data.");
        return false;
    }

    if (FindAccountRecord(records, normalizedUsername) != NULL) {
        WriteAuthMessage(message, messageSize, "That username already exists. Try signing in.");
        return false;
    }

    newRecord.key = normalizedUsername;
    newRecord.displayName = displayUsername;
    newRecord.passwordHash = HashPassword(normalizedUsername, password == NULL ? "" : password);
    records.push_back(newRecord);

    if (!SaveAccountRecords(records)) {
        WriteAuthMessage(message, messageSize, "Couldn't create the local account.");
        return false;
    }

    if (!ActivateAccount(newRecord)) {
        WriteAuthMessage(message, messageSize, "The account was created, but its save folder could not be opened.");
        return false;
    }

    {
        char successMessage[SAVE_AUTH_MESSAGE_MAX];
        const char *format;

        format = Loc_PickLiteral("Account %s created and signed in.",
                                 "账号 %s 已创建并登录。");
        std::snprintf(successMessage, sizeof(successMessage), format, newRecord.displayName.c_str());
        WriteAuthMessage(message, messageSize, successMessage);
    }
    return true;
}

bool SaveSystem_DeleteAccount(const char *username, const char *password, char *message, size_t messageSize) {
    std::string normalizedUsername;
    std::string displayUsername;
    std::vector<AccountRecord> records;
    const AccountRecord *record;

    if (!ValidateUsername(username, &normalizedUsername, &displayUsername, message, messageSize)
        || !ValidatePassword(password, message, messageSize)) {
        return false;
    }

    if (!LoadAccountRecords(&records)) {
        WriteAuthMessage(message, messageSize, "Couldn't read local account data.");
        return false;
    }

    record = FindAccountRecord(records, normalizedUsername);
    if (record == NULL) {
        WriteAuthMessage(message, messageSize, "That account could not be found.");
        return false;
    }

    if (record->passwordHash != HashPassword(normalizedUsername, password == NULL ? "" : password)) {
        WriteAuthMessage(message, messageSize, "Incorrect password.");
        return false;
    }

    return DeleteAccountByKey(normalizedUsername, message, messageSize);
}

bool SaveSystem_DeleteActiveAccount(char *message, size_t messageSize) {
    std::string activeKey;

    if (!SaveInternal_HasActiveAccount()) {
        WriteAuthMessage(message, messageSize, "There is no signed-in account to delete.");
        return false;
    }

    activeKey = SaveInternal_GetActiveAccountKey();
    if (activeKey.empty()) {
        WriteAuthMessage(message, messageSize, "There is no signed-in account to delete.");
        return false;
    }

    return DeleteAccountByKey(activeKey, message, messageSize);
}

static void PopulateSlotInfoFromSnapshot(SaveSlotInfo *slot, const SaveSnapshot *snapshot) {
    if (slot == NULL || snapshot == NULL) {
        return;
    }

    slot->exists = true;
    slot->stage = snapshot->stage;
    slot->dayCount = snapshot->dayCount;
    slot->ending = snapshot->ending;
    slot->health = snapshot->health;
    slot->oxygen = snapshot->oxygen;
    slot->deathCount = snapshot->deathCount;
}

void SaveSystem_ListSlots(SaveSlotInfo *slots, int slotCount) {
    int slotIndex;

    if (slots == NULL || slotCount <= 0) {
        return;
    }

    SaveInternal_MigrateLegacySaveIfNeeded();

    for (slotIndex = 0; slotIndex < slotCount; slotIndex++) {
        SaveSlotInfo *slot;
        SaveSnapshot snapshot;

        slot = &slots[slotIndex];
        std::memset(slot, 0, sizeof(*slot));
        slot->slotIndex = slotIndex;
        SaveSystem_GetSlotPath(slotIndex, slot->path, sizeof(slot->path));

        if (!SaveInternal_HasActiveAccount() || slotIndex >= SAVE_SLOT_COUNT) {
            continue;
        }

        if (!SaveInternal_LoadSnapshotFromPath(slot->path, &snapshot)) {
            continue;
        }
        PopulateSlotInfoFromSnapshot(slot, &snapshot);
    }
}

bool SaveSystem_LoadGame(int slotIndex, SaveSnapshot *snapshot) {
    char path[640];

    if (snapshot == NULL || !SaveInternal_HasActiveAccount()) {
        return false;
    }

    SaveInternal_MigrateLegacySaveIfNeeded();
    if (!SaveInternal_IsValidSlotIndex(slotIndex)) {
        return false;
    }

    SaveSystem_GetSlotPath(slotIndex, path, sizeof(path));
    return SaveInternal_LoadSnapshotFromPath(path, snapshot);
}

bool SaveSystem_SaveGame(int slotIndex, const SaveSnapshot *snapshot) {
    SaveSnapshot sanitizedSnapshot;
    uint8_t buffer[SAVE_INTERNAL_MAX_SAVE_FILE_SIZE];
    char path[640];
    size_t bytesWritten;

    SaveInternal_MigrateLegacySaveIfNeeded();
    if (snapshot == NULL || !SaveInternal_HasActiveAccount() || !SaveInternal_IsValidSlotIndex(slotIndex) || !SaveInternal_EnsureActiveAccountDirectory()) {
        return false;
    }

    sanitizedSnapshot = *snapshot;
    SaveInternal_SanitizeSnapshot(&sanitizedSnapshot);
    if (!SaveInternal_EncodeSnapshotBuffer(&sanitizedSnapshot, buffer, sizeof(buffer), &bytesWritten)) {
        return false;
    }

    SaveSystem_GetSlotPath(slotIndex, path, sizeof(path));
    return PersistencePlatform_WriteFileAtomically(path, buffer, bytesWritten);
}

bool SaveSystem_DeleteGame(int slotIndex) {
    char path[640];

    if (!SaveInternal_HasActiveAccount()) {
        return false;
    }

    SaveInternal_MigrateLegacySaveIfNeeded();
    if (!SaveInternal_IsValidSlotIndex(slotIndex)) {
        return false;
    }

    SaveSystem_GetSlotPath(slotIndex, path, sizeof(path));
    if (!PersistencePlatform_FileExists(path)) {
        return false;
    }

    return PersistencePlatform_DeleteFile(path);
}
