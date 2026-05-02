#ifndef UI_SYSTEM_H
#define UI_SYSTEM_H

#include "c_compat.h"
#include "assets.h"
#include "map.h"
#include "minimap.h"
#include "player.h"
#include "save_system.h"
#include "task_system.h"

/* Public HUD/overlay drawing APIs plus shared UI geometry entry points. */

SCL_EXTERN_C_BEGIN

#define BACKPACK_ENTRY_COUNT 23
#define UI_BACKPACK_SLOT_COLUMNS 4
#define UI_CRAFT_SLOT_COLUMNS 2
#define HUD_SHORTCUT_COUNT 4

typedef struct HudMessage {
    char text[256];
    float timer;
} HudMessage;

typedef enum MainMenuButton {
    MAIN_MENU_BUTTON_START = 0,
    MAIN_MENU_BUTTON_LOAD,
    MAIN_MENU_BUTTON_SETTINGS,
    MAIN_MENU_BUTTON_EXIT,
    MAIN_MENU_BUTTON_COUNT
} MainMenuButton;

typedef enum AccountDeleteConfirmButton {
    ACCOUNT_DELETE_CONFIRM_BUTTON_CANCEL = 0,
    ACCOUNT_DELETE_CONFIRM_BUTTON_DELETE,
    ACCOUNT_DELETE_CONFIRM_BUTTON_COUNT
} AccountDeleteConfirmButton;

typedef enum AuthScreenMode {
    AUTH_SCREEN_MODE_LOGIN = 0,
    AUTH_SCREEN_MODE_REGISTER
} AuthScreenMode;

typedef enum AuthField {
    AUTH_FIELD_USERNAME = 0,
    AUTH_FIELD_PASSWORD,
    AUTH_FIELD_TOGGLE_PASSWORD,
    AUTH_FIELD_DELETE_ACCOUNT,
    AUTH_FIELD_SUBMIT,
    AUTH_FIELD_SWITCH_MODE,
    AUTH_FIELD_EXIT_GAME
} AuthField;

typedef enum PauseMenuButton {
    PAUSE_MENU_BUTTON_CONTINUE = 0,
    PAUSE_MENU_BUTTON_SAVE,
    PAUSE_MENU_BUTTON_LOAD,
    PAUSE_MENU_BUTTON_SETTINGS,
    PAUSE_MENU_BUTTON_MENU,
    PAUSE_MENU_BUTTON_COUNT
} PauseMenuButton;

typedef enum DeathPopupButton {
    DEATH_POPUP_BUTTON_RESTART = 0,
    DEATH_POPUP_BUTTON_LOAD,
    DEATH_POPUP_BUTTON_MENU,
    DEATH_POPUP_BUTTON_COUNT
} DeathPopupButton;

typedef enum SavePanelMode {
    SAVE_PANEL_MODE_LOAD = 0,
    SAVE_PANEL_MODE_SAVE
} SavePanelMode;

typedef enum InfoOverlayTab {
    INFO_OVERLAY_TAB_MAP = 0,
    INFO_OVERLAY_TAB_BACKPACK,
    INFO_OVERLAY_TAB_LOXI,
    INFO_OVERLAY_TAB_SETTINGS,
    INFO_OVERLAY_TAB_COUNT
} InfoOverlayTab;

typedef enum SettlementConfirmButton {
    SETTLEMENT_CONFIRM_BUTTON_HEROIC = 0,
    SETTLEMENT_CONFIRM_BUTTON_PEACEFUL,
    SETTLEMENT_CONFIRM_BUTTON_SETTLEMENT,
    SETTLEMENT_CONFIRM_BUTTON_CANCEL,
    SETTLEMENT_CONFIRM_BUTTON_COUNT
} SettlementConfirmButton;

typedef enum StoryScene {
    STORY_SCENE_NONE = 0,
    STORY_SCENE_MAIN_AIR_FOR_ONE_MORE_DAY,
    STORY_SCENE_MAIN_OXYGEN_CYCLE_RESTORED,
    STORY_SCENE_MAIN_LOXI_FULL_SYNC,
    STORY_SCENE_MAIN_AIRLOCK_OPENING,
    STORY_SCENE_MAIN_SIGNAL_ANSWERS_BACK,
    STORY_SCENE_MAIN_CRASH_NOT_ACCIDENT,
    STORY_SCENE_MAIN_DEEP_SCAN_ONLINE,
    STORY_SCENE_MAIN_RELIC_PATTERN_DECODE,
    STORY_SCENE_MAIN_EAST_WRECK_CONFIRMATION,
    STORY_SCENE_MAIN_BASIN_ACCESS_QUALIFICATION,
    STORY_SCENE_MAIN_WEST_ROUTE_CONFIRMED,
    STORY_SCENE_MAIN_SURVEY_BREAK_RELAY,
    STORY_SCENE_MAIN_CANOPY_HANDOFF,
    STORY_SCENE_MAIN_ECHO_BASIN_RECONSTRUCTION,
    STORY_SCENE_MAIN_LAST_CAMP_POSITIONS,
    STORY_SCENE_MAIN_SOUTH_FACILITY_WAKES,
    STORY_SCENE_MAIN_VENT_NETWORK_CALIBRATED,
    STORY_SCENE_MAIN_SERVICE_SHAFT_BACKBONE,
    STORY_SCENE_MAIN_PURIFIER_RING_SEQUENCE,
    STORY_SCENE_MAIN_ROOT_VAULT_TRUTH,
    STORY_SCENE_MAIN_WEST_SOUTH_CORRELATION,
    STORY_SCENE_MAIN_LOXI_CONCLUSION_REWRITE,
    STORY_SCENE_MAIN_MONOLITH_TRUE_ROLE,
    STORY_SCENE_MAIN_NORTH_ROUTE_COMMITMENT,
    STORY_SCENE_MAIN_TOWER_NOT_BUTTON,
    STORY_SCENE_MAIN_THREE_COSTS_REVEALED,
    STORY_SCENE_MAIN_ACTION_DECLARATION,
    STORY_SCENE_MAIN_HEROIC_ROUTE_COMMITMENT,
    STORY_SCENE_MAIN_PEACEFUL_ROUTE_COMMITMENT,
    STORY_SCENE_MAIN_SETTLEMENT_ROUTE_COMMITMENT,
    STORY_SCENE_LOG_THE_CRASH,
    STORY_SCENE_LOG_MISSING_CREW,
    STORY_SCENE_LOG_ALIEN_ECOLOGY,
    STORY_SCENE_LOG_WEST_SIGNAL_FRAGMENT,
    STORY_SCENE_LOG_SURVEY_BREAK_ANCHOR_NOTES,
    STORY_SCENE_LOG_CANOPY_HANDOFF_RECORD,
    STORY_SCENE_LOG_ECHO_BASIN_TOPOLOGY,
    STORY_SCENE_LOG_LAST_CAMP_TESTAMENT,
    STORY_SCENE_LOG_BLACK_BOX_RESIDUE,
    STORY_SCENE_LOG_PURIFIER_OUTAGE_MEMO,
    STORY_SCENE_LOG_VENT_CALIBRATION_HANDOVER,
    STORY_SCENE_LOG_SERVICE_SHAFT_SYNC,
    STORY_SCENE_LOG_PURIFIER_RING_CONTROL_BRIEF,
    STORY_SCENE_LOG_ROOT_VAULT_CORE_DOSSIER,
    STORY_SCENE_COUNT
} StoryScene;

Rectangle UI_GetMainMenuButtonRect(int screenWidth, int screenHeight, int buttonIndex);
int UI_GetMainMenuButtonIndexAtPoint(int screenWidth, int screenHeight, Vector2 point);
Rectangle UI_GetAuthPanelRect(int screenWidth, int screenHeight);
Rectangle UI_GetAuthInputRect(int screenWidth, int screenHeight, int fieldIndex);
Rectangle UI_GetAuthPasswordToggleRect(int screenWidth, int screenHeight);
Rectangle UI_GetAuthDeleteAccountRect(int screenWidth, int screenHeight);
Rectangle UI_GetAuthSubmitButtonRect(int screenWidth, int screenHeight);
Rectangle UI_GetAuthSwitchModeRect(int screenWidth, int screenHeight);
Rectangle UI_GetAuthExitButtonRect(int screenWidth, int screenHeight);
Rectangle UI_GetPauseMenuPanelRect(int screenWidth, int screenHeight);
Rectangle UI_GetPauseMenuButtonRect(int screenWidth, int screenHeight, int buttonIndex);
Rectangle UI_GetDeathPopupButtonRect(int screenWidth, int screenHeight, int buttonIndex);
Rectangle UI_GetStandardOverlayRect(int screenWidth, int screenHeight);
Rectangle UI_GetSaveSlotRect(int screenWidth, int screenHeight, int slotIndex);
Rectangle UI_GetSavePrimaryButtonRect(int screenWidth, int screenHeight);
Rectangle UI_GetSaveDeleteButtonRect(int screenWidth, int screenHeight);
Rectangle UI_GetBackpackSlotRect(int screenWidth, int screenHeight, int itemIndex);
Rectangle UI_GetCraftSlotRect(int screenWidth, int screenHeight, int itemIndex);
Rectangle UI_GetCraftActionButtonRect(int screenWidth, int screenHeight);
Rectangle UI_GetCommunicatorOverlayRect(int screenWidth, int screenHeight);
Rectangle UI_GetCommunicatorTabRect(int screenWidth, int screenHeight, int tabIndex);
Rectangle UI_GetInfoOverlayTabRect(int screenWidth, int screenHeight, int tabIndex);
Rectangle UI_GetCommunicatorLogListRect(int screenWidth, int screenHeight);
Rectangle UI_GetCommunicatorLogContentRect(int screenWidth, int screenHeight);
Rectangle UI_GetCommunicatorLogImagePanelRect(int screenWidth, int screenHeight);
Rectangle UI_GetCommunicatorLogImageRect(int screenWidth, int screenHeight);
Rectangle UI_GetCommunicatorLogDetailOverlayRect(int screenWidth, int screenHeight);
Rectangle UI_GetCommunicatorVisibleLogEntryRect(int screenWidth, int screenHeight, int visibleIndex);
int UI_GetCommunicatorVisibleLogCount(int screenWidth, int screenHeight);
int UI_ClampCommunicatorFirstVisibleLogIndex(int screenWidth, int screenHeight, int firstVisibleLogIndex, int totalLogCount);
int UI_GetCommunicatorFirstVisibleLogIndex(int screenWidth, int screenHeight, int selectedLogIndex, int totalLogCount);
Rectangle UI_GetLogEntryRect(int screenWidth, int screenHeight, int entryIndex);
Rectangle UI_GetSettingsRowRect(int screenWidth, int screenHeight, int rowIndex);
Rectangle UI_GetSettingsSliderRect(int screenWidth, int screenHeight, int sliderIndex);
Rectangle UI_GetSettingsDecreaseButtonRect(int screenWidth, int screenHeight, int sliderIndex);
Rectangle UI_GetSettingsIncreaseButtonRect(int screenWidth, int screenHeight, int sliderIndex);
Rectangle UI_GetSettingsCloseButtonRect(int screenWidth, int screenHeight);
Rectangle UI_GetSettingsLanguageButtonRect(int screenWidth, int screenHeight, int buttonIndex);
Rectangle UI_GetSettingsAccountButtonRect(int screenWidth, int screenHeight, int buttonIndex);
Rectangle UI_GetHudShortcutRect(int screenWidth, int screenHeight, int shortcutIndex);
Rectangle UI_GetSettlementConfirmPanelRect(int screenWidth, int screenHeight);
Rectangle UI_GetSettlementConfirmButtonRect(int screenWidth, int screenHeight, int buttonIndex, int buttonCount);
Rectangle UI_GetAccountDeleteConfirmPanelRect(int screenWidth, int screenHeight);
Rectangle UI_GetAccountDeleteConfirmButtonRect(int screenWidth, int screenHeight, int buttonIndex);
void UI_DrawHud(const Player *player, const TaskSystem *tasks, const HudMessage *message, const AssetBundle *assets, int screenWidth, int screenHeight);
void UI_DrawAuthScreen(const AssetBundle *assets,
                       AuthScreenMode mode,
                       const char *username,
                       const char *password,
                       bool passwordVisible,
                       AuthField selectedField,
                       const char *message,
                       bool hasAccounts,
                       int screenWidth,
                       int screenHeight,
                       float elapsedSeconds);
void UI_DrawMainMenu(const AssetBundle *assets,
                     bool hasSave,
                     int saveCount,
                     const char *accountName,
                     bool hasBestScore,
                     int bestScore,
                     int screenWidth,
                     int screenHeight,
                     float elapsedSeconds);
void UI_DrawOpeningStandby(const AssetBundle *assets, bool showContinueHint, int screenWidth, int screenHeight, float elapsedSeconds);
void UI_DrawOpeningCutscene(const AssetBundle *assets, int slideIndex, float slideElapsed, int screenWidth, int screenHeight);
void UI_DrawStoryScene(const AssetBundle *assets, StoryScene scene, float sceneElapsed, int screenWidth, int screenHeight);
void UI_DrawPauseMenu(const AssetBundle *assets, int screenWidth, int screenHeight);
void UI_DrawSettingsOverlay(const AssetBundle *assets,
                            const GameSettings *settings,
                            const char *accountName,
                            int saveCount,
                            bool accountActionsEnabled,
                            int screenWidth,
                            int screenHeight);
void UI_DrawInfoOverlay(const AssetBundle *assets,
                        InfoOverlayTab selectedTab,
                        const GameSettings *settings,
                        const MiniMap *minimap,
                        const Player *player,
                        const TaskSystem *tasks,
                        const GameMap *map,
                        const bool *storySceneShown,
                        int saveCount,
                        int selectedBackpackItem,
                        int communicatorTab,
                        int selectedLog,
                        int firstVisibleLog,
                        int selectedStoryScene,
                        int firstVisibleStoryScene,
                        float detailVisibility,
                        float detailScroll,
                        int screenWidth,
                        int screenHeight);
void UI_DrawBackpackOverlay(const AssetBundle *assets, const Player *player, int selectedItem, int screenWidth, int screenHeight);
void UI_DrawCraftOverlay(const AssetBundle *assets, const TaskSystem *tasks, const Player *player, int selectedRecipe, int screenWidth, int screenHeight);
void UI_DrawCommunicatorOverlay(const AssetBundle *assets,
                                const TaskSystem *tasks,
                                const bool *storySceneShown,
                                int selectedTab,
                                int selectedLog,
                                int firstVisibleLog,
                                int selectedStoryScene,
                                int firstVisibleStoryScene,
                                float detailVisibility,
                                float detailScroll,
                                int screenWidth,
                                int screenHeight);
void UI_DrawHelpOverlay(const AssetBundle *assets, int screenWidth, int screenHeight);
void UI_DrawEnding(GameEnding ending, const Player *player, const TaskSystem *tasks, const AssetBundle *assets, int screenWidth, int screenHeight, float elapsedSeconds);
void UI_DrawDeathPopup(const Player *player, bool hasSave, const AssetBundle *assets, int screenWidth, int screenHeight, int selectedButton);
void UI_DrawSettlementConfirmPopup(const AssetBundle *assets,
                                   const Player *player,
                                   const TaskSystem *tasks,
                                   int screenWidth,
                                   int screenHeight,
                                   int buttonCount,
                                   int selectedButton);
void UI_DrawAccountDeleteConfirmPopup(const AssetBundle *assets,
                                      const char *accountName,
                                      int screenWidth,
                                      int screenHeight,
                                      int selectedButton);
void UI_DrawLogReader(const TaskSystem *tasks, int selectedLog, const AssetBundle *assets, int screenWidth, int screenHeight);
void UI_DrawMapOverlay(const AssetBundle *assets, const MiniMap *minimap, const Player *player, const TaskSystem *tasks, const GameMap *map, int screenWidth, int screenHeight);
void UI_DrawSaveSlotsOverlay(const AssetBundle *assets,
                             const SaveSlotInfo *slots,
                             int slotCount,
                             SavePanelMode mode,
                             int selectedSlot,
                             const char *accountName,
                             int screenWidth,
                             int screenHeight);

SCL_EXTERN_C_END

#endif
