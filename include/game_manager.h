#ifndef GAME_MANAGER_H
#define GAME_MANAGER_H

#include "c_compat.h"
#include "assets.h"
#include "audio_system.h"
#include "map.h"
#include "map_scene_manager.h"
#include "minimap.h"
#include "player.h"
#include "save_system.h"
#include "task_system.h"
#include "ui_system.h"

/* Public top-level game state and frame lifecycle entry points.
 * Game remains concrete for the shared C/C++ boundary, while its owned state is
 * grouped by responsibility to keep coordinator code explicit.
 */

SCL_EXTERN_C_BEGIN

typedef enum GameState {
    GAME_STATE_INTRO = 0,
    GAME_STATE_OPENING,
    GAME_STATE_PLAYING,
    GAME_STATE_ENDING
} GameState;

typedef enum NarrativeTransitionAction {
    NARRATIVE_TRANSITION_NONE = 0,
    NARRATIVE_TRANSITION_OPENING_NEXT_SLIDE,
    NARRATIVE_TRANSITION_OPENING_COMPLETE,
    NARRATIVE_TRANSITION_STORY_CLOSE
} NarrativeTransitionAction;

typedef enum ScreenTransitionAction {
    SCREEN_TRANSITION_NONE = 0,
    SCREEN_TRANSITION_LOAD_GAME,
    SCREEN_TRANSITION_RETURN_TO_MENU,
    SCREEN_TRANSITION_LOGOUT_TO_AUTH,
    SCREEN_TRANSITION_TOGGLE_AUTH_MODE,
    SCREEN_TRANSITION_AUTH_SUCCESS,
    SCREEN_TRANSITION_APPLY_LANGUAGE,
    SCREEN_TRANSITION_SLEEP_REST,
    SCREEN_TRANSITION_MAP_CHANGE,
    SCREEN_TRANSITION_ENTER_ENDING
} ScreenTransitionAction;

typedef enum CommunicatorTab {
    COMMUNICATOR_TAB_TASKS = 0,
    COMMUNICATOR_TAB_LOGS,
    COMMUNICATOR_TAB_STORY,
    COMMUNICATOR_TAB_COUNT
} CommunicatorTab;

#define GAME_MESSAGE_HISTORY_LIMIT 20

typedef struct GameMessageNode {
    char text[160];
    float postedAt;
    float duration;
    struct GameMessageNode *next;
} GameMessageNode;

typedef struct GameMessageHistory {
    GameMessageNode *head;
    GameMessageNode *tail;
    int count;
} GameMessageHistory;

typedef struct GameRuntimeState {
    GameMap map;
    Player player;
    TaskSystem tasks;
    Camera2D camera;
    MiniMap miniMap;
    float elapsedSeconds;
    float hurtSoundCooldown;
    float monsterCueCooldown;
    float laserEffectTimer;
    bool laserEffectHit;
    Vector2 laserEffectStart;
    Vector2 laserEffectEnd;
    char lastLocationName[64];
} GameRuntimeState;

typedef struct GameUiState {
    HudMessage hudMessage;
    GameMessageHistory messageHistory;
    bool pauseMenuOpen;
    bool settingsOpen;
    bool settingsDirty;
    bool settingsSliderDragging;
    int settingsSliderDragIndex;
    bool infoOverlayOpen;
    InfoOverlayTab infoOverlayTab;
    bool craftOpen;
    bool helpOpen;
    bool logReaderOpen;
    CommunicatorTab communicatorTab;
    int selectedBackpackItem;
    int selectedCraftIndex;
    bool savePanelOpen;
    SavePanelMode savePanelMode;
    int selectedSaveSlot;
    int selectedLogIndex;
    int communicatorFirstVisibleLogIndex;
    int selectedStorySceneIndex;
    int communicatorFirstVisibleStorySceneIndex;
    bool communicatorLogDetailOpen;
    float communicatorLogDetailVisibility;
    float communicatorLogDetailScroll;
    bool showDeathPopup;
    int deathPopupSelection;
    bool settlementConfirmOpen;
    int settlementConfirmSelection;
    bool endingRouteDoubleConfirmOpen;
    int endingRouteDoubleConfirmSelection;
    GameEnding endingRoutePendingConfirm;
    int endingMenuSelection;
} GameUiState;

typedef struct GameInputState {
    int bufferedMoveX;
    int bufferedMoveY;
    float inputBufferTimer;
    int heldMoveX;
    int heldMoveY;
    float holdRepeatTimer;
} GameInputState;

typedef struct GameAccountState {
    bool hasSaveFile;
    SaveSlotInfo saveSlots[SAVE_SLOT_COUNT];
    int saveSlotCount;
    bool hasAccountBestScore;
    int accountBestScore;
    bool authenticated;
    bool authHasAccounts;
    AuthScreenMode authMode;
    AuthField authSelectedField;
    bool authPasswordVisible;
    bool authAccountPickerOpen;
    int authAccountCount;
    int authAccountPickerSelection;
    int authAccountPickerFirstVisible;
    char authAccountNames[SAVE_ACCOUNT_LIST_MAX][SAVE_ACCOUNT_NAME_MAX];
    char authUsername[SAVE_ACCOUNT_NAME_MAX];
    char authPassword[SAVE_ACCOUNT_PASSWORD_MAX];
    char authMessage[SAVE_AUTH_MESSAGE_MAX];
    bool accountDeleteConfirmOpen;
    int accountDeleteConfirmSelection;
    bool accountDeleteFromAuth;
    char accountDeleteTargetName[SAVE_ACCOUNT_NAME_MAX];
} GameAccountState;

typedef struct GameFlowState {
    GameState state;
    float openingCutsceneElapsed;
    bool storySceneOpen;
    StoryScene storyScene;
    bool storySceneShown[STORY_SCENE_COUNT];
    int openingSlideIndex;
    bool openingAwaitingFirstAdvance;
    float storySceneElapsed;
    bool narrativeTransitionActive;
    float narrativeTransitionElapsed;
    NarrativeTransitionAction narrativeTransitionAction;
    bool screenTransitionActive;
    bool screenTransitionResolved;
    float screenTransitionElapsed;
    ScreenTransitionAction screenTransitionAction;
    int screenTransitionSlotIndex;
    GameLanguage pendingLanguage;
    bool requestClose;
} GameFlowState;

typedef struct GameServices {
    GameSettings settings;
    AudioManager audio;
    AssetBundle assets;
    MapSceneManager mapScene;
} GameServices;

typedef struct Game {
    GameRuntimeState runtime;
    GameUiState ui;
    GameInputState input;
    GameAccountState account;
    GameFlowState flow;
    GameServices services;
} Game;

void Game_Init(Game *game);
void Game_Update(Game *game, float deltaTime);
void Game_Draw(Game *game);
void Game_Shutdown(Game *game);
bool Game_BeginPortalTransition(Game *game, const MapPortal *portal);

SCL_EXTERN_C_END

#endif
