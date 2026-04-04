#ifndef SAVE_SYSTEM_H
#define SAVE_SYSTEM_H

#include <stdbool.h>
#include "task_system.h"

typedef struct GameSettings {
    float masterVolume;
    bool sfxEnabled;
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

typedef struct SaveSnapshot {
    int gridX;
    int gridY;
    int facingX;
    int facingY;
    float stamina;
    float pressure;
    float oxygen;
    float poison;
    float maxStaminaBonus;
    float attackBonus;
    int deathCount;
    bool crouching;
    bool hasGlowStick;
    bool hasRope;
    bool hasLaserGun;
    bool hasProtectionSuit;
    bool hasSignalAmplifier;
    bool hasFieldCamp;
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
    bool monolithActivated[3];
    int monolithsLit;
    int ending;
    bool campPlaced;
    int campX;
    int campY;
    SavedNodeSnapshot nodes[MAX_RESOURCE_NODES];
    SavedMonsterSnapshot monsters[MAX_MONSTERS];
    SavedLogSnapshot logs[MAX_LOGS];
} SaveSnapshot;

void SaveSystem_SetDefaultSettings(GameSettings *settings);
bool SaveSystem_LoadSettings(GameSettings *settings);
bool SaveSystem_SaveSettings(const GameSettings *settings);
bool SaveSystem_HasSave(void);
bool SaveSystem_LoadGame(SaveSnapshot *snapshot);
bool SaveSystem_SaveGame(const SaveSnapshot *snapshot);
const char *SaveSystem_GetBaseDirectory(void);

#endif
