#ifndef GAME_MANAGER_H
#define GAME_MANAGER_H

#include "c_compat.h"
#include "assets.h"
#include "audio_system.h"
#include "map.h"
#include "minimap.h"
#include "player.h"
#include "save_system.h"
#include "task_system.h"
#include "ui_system.h"

/* Public top-level game state and frame lifecycle entry points. */

SCL_EXTERN_C_BEGIN

typedef enum GameState {
    GAME_STATE_INTRO = 0,
    GAME_STATE_OPENING,
    GAME_STATE_PLAYING,
    GAME_STATE_ENDING
} GameState;

typedef struct Game {
    GameState state;
    GameMap map;
    Player player;
    TaskSystem tasks;
    GameSettings settings;
    AudioManager audio;
    AssetBundle assets;
    Camera2D camera;
    HudMessage hudMessage;
    MiniMap miniMap;
    float elapsedSeconds;
    float openingCutsceneElapsed;
    float hurtSoundCooldown;
    float monsterCueCooldown;
    int bufferedMoveX;
    int bufferedMoveY;
    float inputBufferTimer;
    int heldMoveX;
    int heldMoveY;
    float holdRepeatTimer;
    bool hasSaveFile;
    bool pauseMenuOpen;
    bool settingsOpen;
    bool settingsDirty;
    bool settingsSliderDragging;
    bool backpackOpen;
    bool craftOpen;
    bool mapOpen;
    bool communicatorOpen;
    bool helpOpen;
    bool logReaderOpen;
    bool storySceneOpen;
    StoryScene storyScene;
    bool storySceneShown[STORY_SCENE_COUNT];
    int selectedBackpackItem;
    int selectedCraftIndex;
    bool savePanelOpen;
    SavePanelMode savePanelMode;
    int selectedSaveSlot;
    SaveSlotInfo saveSlots[SAVE_SLOT_COUNT];
    int saveSlotCount;
    bool authenticated;
    bool authHasAccounts;
    AuthScreenMode authMode;
    AuthField authSelectedField;
    bool authPasswordVisible;
    char authUsername[SAVE_ACCOUNT_NAME_MAX];
    char authPassword[SAVE_ACCOUNT_PASSWORD_MAX];
    char authMessage[SAVE_AUTH_MESSAGE_MAX];
    int openingSlideIndex;
    float storySceneElapsed;
    int selectedLogIndex;
    bool showDeathPopup;
    int deathPopupSelection;
    bool settlementConfirmOpen;
    int settlementConfirmSelection;
    bool accountDeleteConfirmOpen;
    int accountDeleteConfirmSelection;
    bool accountDeleteFromAuth;
    char accountDeleteTargetName[SAVE_ACCOUNT_NAME_MAX];
    bool requestClose;
    char lastLocationName[64];
} Game;

void Game_Init(Game *game);
void Game_Update(Game *game, float deltaTime);
void Game_Draw(Game *game);
void Game_Shutdown(Game *game);

SCL_EXTERN_C_END

#endif
