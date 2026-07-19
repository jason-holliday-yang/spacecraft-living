#ifndef SAVE_SYSTEM_H
#define SAVE_SYSTEM_H

#include <stdbool.h>
#include <stddef.h>
#include "assets.h"
#include "c_compat.h"
#include "config.h"
#include "localization.h"
#include "task_system.h"

/* Public save/settings schemas, slot metadata, and persistence entry points. */

SCL_EXTERN_C_BEGIN

#define SAVE_SLOT_COUNT 16
#define SAVE_SLOT_PATH_MAX 640
#define SAVE_DYNAMIC_TILE_MAX 256
#define SAVE_MAP_STATE_MAX MAX_MAP_CATALOG_ENTRIES
#define SAVE_ACCOUNT_NAME_MAX 32
#define SAVE_ACCOUNT_PASSWORD_MAX 48
#define SAVE_AUTH_MESSAGE_MAX 160
#define SAVE_ACCOUNT_LIST_MAX 128

typedef struct GameSettings {
    float masterVolume;
    float musicVolume;
    float sfxVolume;
    bool sfxEnabled;
    GameLanguage language;
    char lastUsername[SAVE_ACCOUNT_NAME_MAX];
} GameSettings;

typedef struct SavedNodeSnapshot {
    bool active;
    int respawnsRemaining;
} SavedNodeSnapshot;

typedef struct SavedMonsterSnapshot {
    bool active;
    int gridX;
    int gridY;
    float health;
    bool phaseTriggered;
} SavedMonsterSnapshot;

typedef struct SavedLogSnapshot {
    bool collected;
} SavedLogSnapshot;

typedef struct SavedUnlockSnapshot {
    char unlockId[MAP_UNLOCK_ID_MAX];
    bool open;
} SavedUnlockSnapshot;

typedef struct SavedMapStateSnapshot {
    char mapId[MAP_ID_MAX];
    int contentVersion;
    int mapWidth;
    int mapHeight;
    int unlockCount;
    SavedUnlockSnapshot unlocks[MAX_MAP_UNLOCKS];
    int clearedDynamicTileCount;
    int clearedDynamicTileX[SAVE_DYNAMIC_TILE_MAX];
    int clearedDynamicTileY[SAVE_DYNAMIC_TILE_MAX];
    bool campPlaced;
    int campX;
    int campY;
    int nodeCount;
    SavedNodeSnapshot nodes[MAX_RESOURCE_NODES];
    int monsterCount;
    SavedMonsterSnapshot monsters[MAX_MONSTERS];
    int logCount;
    SavedLogSnapshot logs[MAX_LOGS];
} SavedMapStateSnapshot;

typedef struct SavedStatusSnapshot {
    bool active;
    int level;
    float remainingTime;
    float magnitude;
} SavedStatusSnapshot;

typedef struct SaveSnapshot {
    int sourceVersion;
    char currentMapId[MAP_ID_MAX];
    int gridX;
    int gridY;
    int facingX;
    int facingY;
    int mapWidth;
    int mapHeight;
    float health;                 /* canonical player resource */
    float stamina;                /* @deprecated - retained only for backward-compatible save deserialization */
    float pressure;               /* @deprecated - retained only for backward-compatible save deserialization */
    float oxygen;                 /* canonical player resource */
    float poison;                 /* canonical player resource */
    float maxHealthBonus;         /* canonical */
    float maxStaminaBonus;        /* @deprecated - legacy alias for maxHealthBonus */
    float attackBonus;
    int deathCount;
    bool hasGlowStick;
    bool hasRope;
    bool hasLaserGun;
    bool hasProtectionSuit;
    bool hasSignalAmplifier;
    bool hasFieldCamp;
    SavedStatusSnapshot statuses[PLAYER_STATUS_COUNT];
    int resources[RESOURCE_COUNT];
    int stage;
    int dayCount;
    int phase;
    int currentEvent;
    float cycleTimer;
    float elapsedSeconds;
    int oxygenRepairLevel;
    int commRepairLevel;
    int energyRepairLevel;
    bool crashClueFound;
    bool amplifierUnlocked;
    bool bossDefeated;
    bool signalTowerActivated;
    int selectedEndingRoute;
    bool endingArchiveReviewed;
    bool westW1Started;
    bool westW1Completed;
    bool westW2Started;
    bool westW2Completed;
    bool westW3Started;
    bool westW3Completed;
    bool westW4Started;
    bool westW4Completed;
    bool westW5Started;
    bool westW5Completed;
    bool southS1Started;
    bool southS1Completed;
    bool southS2Started;
    bool southS2Completed;
    bool southS3Started;
    bool southS3Completed;
    bool southS4Started;
    bool southS4Completed;
    bool southS5Started;
    bool southS5Completed;
    bool monolithActivated[3];
    int monolithsLit;
    int ending;
    bool campPlaced;
    int campX;
    int campY;
    int clearedDynamicTileCount;
    int clearedDynamicTileX[SAVE_DYNAMIC_TILE_MAX];
    int clearedDynamicTileY[SAVE_DYNAMIC_TILE_MAX];
    SavedNodeSnapshot nodes[MAX_RESOURCE_NODES];
    SavedMonsterSnapshot monsters[MAX_MONSTERS];
    SavedLogSnapshot logs[MAX_LOGS];
    bool storyMainSceneShown[STORY_MAIN_SCENE_COUNT];
    bool communicatorUnlocked;
    int mapStateCount;
    SavedMapStateSnapshot mapStates[SAVE_MAP_STATE_MAX];
} SaveSnapshot;

typedef struct SaveSlotInfo {
    bool exists;
    int slotIndex;
    int stage;
    int dayCount;
    int ending;
    float health;
    float oxygen;
    int deathCount;
    char path[SAVE_SLOT_PATH_MAX];
} SaveSlotInfo;

void SaveSystem_SetDefaultSettings(GameSettings *settings);
bool SaveSystem_LoadSettings(GameSettings *settings);
bool SaveSystem_SaveSettings(const GameSettings *settings);
bool SaveSystem_HasRegisteredAccounts(void);
int SaveSystem_ListRegisteredAccounts(char *names, size_t nameStride, int maxNames);
bool SaveSystem_IsAccountAuthenticated(void);
const char *SaveSystem_GetActiveAccountName(void);
bool SaveSystem_GetActiveAccountBestScore(int *scoreOut);
bool SaveSystem_UpdateActiveAccountBestScore(int score);
void SaveSystem_Logout(void);
bool SaveSystem_Login(const char *username, const char *password, char *message, size_t messageSize);
bool SaveSystem_Register(const char *username, const char *password, char *message, size_t messageSize);
bool SaveSystem_DeleteAccount(const char *username, const char *password, char *message, size_t messageSize);
bool SaveSystem_DeleteActiveAccount(char *message, size_t messageSize);
bool SaveSystem_HasAnySave(void);
bool SaveSystem_HasSaveInSlot(int slotIndex);
bool SaveSystem_LoadGame(int slotIndex, SaveSnapshot *snapshot);
bool SaveSystem_SaveGame(int slotIndex, const SaveSnapshot *snapshot);
bool SaveSystem_DeleteGame(int slotIndex);
void SaveSystem_ListSlots(SaveSlotInfo *slots, int slotCount);
void SaveSystem_GetSlotPath(int slotIndex, char *buffer, size_t bufferSize);
const char *SaveSystem_GetBaseDirectory(void);

SCL_EXTERN_C_END

#endif
