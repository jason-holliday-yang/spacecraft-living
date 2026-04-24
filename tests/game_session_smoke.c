#include "game_manager.h"
#include "../src/game_manager_internal.h"
#include "../src/game_play_internal.h"
#include "../src/task_runtime_internal.h"

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

static void Require(bool condition, const char *message) {
    if (condition) {
        return;
    }

    fprintf(stderr, "game_session_smoke failed: %s\n", message);
    exit(1);
}

static void PrepareGame(Game *game) {
    memset(game, 0, sizeof(*game));
    Game_ResetGameplayWorld(game);
    Game_RefreshSaveSlots(game);
    game->state = GAME_STATE_PLAYING;
}

static void MovePlayerAndUpdate(Game *game, int gridX, int gridY) {
    game->player.gridX = gridX;
    game->player.gridY = gridY;
    Player_UpdateWorldPosition(&game->player);
    Game_MaybePostNorthRouteTransitionHint(game);
}

static void CollectArchiveEvidence(TaskSystem *tasks, int logIndex) {
    Require(tasks != NULL, "archive evidence helper requires a task system");
    Require(logIndex >= 0 && logIndex < tasks->logCount, "archive evidence helper should target a valid log");
    tasks->logs[logIndex].active = true;
    tasks->logs[logIndex].collected = true;
}

static void CollectAllMainlineLogs(TaskSystem *tasks) {
    int index;

    Require(tasks != NULL, "mainline archive helper requires a task system");
    for (index = 0; index < tasks->logCount; index++) {
        if (tasks->logs[index].category == SHIP_LOG_MAINLINE) {
            tasks->logs[index].active = true;
            tasks->logs[index].collected = true;
        }
    }
}

static void ConfigureIsolatedSaveHome(void) {
    char tempHomeTemplate[] = "/tmp/scl-session-smoke-XXXXXX";
    char *tempHome;

    tempHome = mkdtemp(tempHomeTemplate);
    Require(tempHome != NULL, "smoke test should create an isolated HOME directory");
    Require(setenv("HOME", tempHome, 1) == 0, "smoke test should redirect HOME for save isolation");
    Require(strstr(SaveSystem_GetBaseDirectory(), tempHome) != NULL,
            "save system base directory should be isolated inside the temporary HOME");
}

int main(void) {
    Game game;
    GameSettings loadedSettings;
    char authMessage[SAVE_AUTH_MESSAGE_MAX];
    int shownMainStorySceneCount;

    ConfigureIsolatedSaveHome();
    Require(SaveSystem_Register("session", "pass1234", authMessage, sizeof(authMessage)),
            "session smoke should authenticate a local account before save flow checks");

    memset(&game, 0, sizeof(game));
    Game_StartNewGame(&game);
    Require(game.state == GAME_STATE_OPENING, "start new game should enter the opening cutscene state");
    Require(game.hudMessage.text[0] == '\0', "opening cutscene should not post the gameplay hint early");
    Require(game.tasks.ending == ENDING_NONE, "start new game should begin without an ending state");
    Require(game.tasks.objective[0] != '\0', "start new game should initialize the opening objective text");
    Require(game.player.gridX == PLAYER_RESPAWN_X && game.player.gridY == PLAYER_RESPAWN_Y,
            "start new game should place the player in the Loxi room");
    Require(Map_IsWalkable(&game.map, game.player.gridX, game.player.gridY), "start new game should place the player on a walkable tile");
    Require(game.camera.zoom > 0.0f, "start new game should initialize a positive camera zoom");
    Require(game.camera.target.x == game.player.worldPos.x && game.camera.target.y == game.player.worldPos.y,
            "start new game should center the camera on the player");
    Require(!Map_IsLoxiRoomUnlocked(&game.map),
            "new game should keep the player sealed in the Loxi room until the first interaction");
    Require(!game.pauseMenuOpen && !game.settingsOpen && !game.backpackOpen && !game.craftOpen
                && !game.mapOpen && !game.communicatorOpen && !game.helpOpen && !game.logReaderOpen
                && !game.savePanelOpen && !game.showDeathPopup,
            "start new game should clear all overlays and popups");

    Game_CompleteOpeningCutscene(&game);
    Require(game.state == GAME_STATE_PLAYING, "completing the opening cutscene should enter playing state");
    Require(strstr(game.hudMessage.text, "Sync the uplink") != NULL
                || strstr(game.hudMessage.text, "Wake in Loxi's cabin") != NULL
                || strstr(game.hudMessage.text, "完成同步") != NULL,
            "entering gameplay should post the opening objective hint");
    Require(Game_SelectAudioScene(&game) == AUDIO_SCENE_BASE,
            "ship cabin opening should use base music");

    game.player.gridX = EXTERIOR_X(70);
    game.player.gridY = EXTERIOR_Y(74);
    Require(Game_SelectAudioScene(&game) == AUDIO_SCENE_FOREST,
            "general crash forest should keep the standard forest music");

    game.player.gridX = EXTERIOR_X(24);
    game.player.gridY = EXTERIOR_Y(72);
    Require(Game_SelectAudioScene(&game) == AUDIO_SCENE_FOREST_ROUTE,
            "west-route investigation shelves should use the alternate forest music");
    if (game.audio.ready) {
        Audio_SetScene(&game.audio, AUDIO_SCENE_FOREST_ROUTE);
        Audio_Update(&game.audio);
        Require(game.audio.pendingSceneVariant == 1 || game.audio.activeSceneVariant == 1,
                "forest-route audio should start from the route-specific forest track instead of the generic forest loop");
    }

    game.player.gridX = EXTERIOR_X(111);
    game.player.gridY = EXTERIOR_Y(48);
    Require(Game_SelectAudioScene(&game) == AUDIO_SCENE_SWAMP_DEEP,
            "deep swamp objectives should use the deeper swamp music variant");
    if (game.audio.ready) {
        Audio_SetScene(&game.audio, AUDIO_SCENE_SWAMP_DEEP);
        Audio_Update(&game.audio);
        Require(game.audio.pendingSceneVariant == 1 || game.audio.activeSceneVariant == 1,
                "deep-swamp audio should start from the deep-swamp track instead of the shallow-swamp loop");
    }

    game.player.gridX = EXTERIOR_X(80);
    game.player.gridY = EXTERIOR_Y(98);
    Require(Game_SelectAudioScene(&game) == AUDIO_SCENE_FOREST_ROUTE,
            "south-collapse entry shelf should keep the forest-route music while the terrain still reads as forest");

    game.player.gridX = EXTERIOR_X(92);
    game.player.gridY = EXTERIOR_Y(98);
    Require(Game_SelectAudioScene(&game) == AUDIO_SCENE_RUINS_FACILITY,
            "southern facility route should no longer fall back to forest music");
    if (game.audio.ready) {
        Audio_SetScene(&game.audio, AUDIO_SCENE_RUINS_FACILITY);
        Audio_Update(&game.audio);
        Require(game.audio.pendingSceneVariant == 1 || game.audio.activeSceneVariant == 1,
                "southern facility audio should start from the facility track instead of the surface ruins loop");
    }

    game.tasks.stage = 7;
    game.tasks.bossDefeated = false;
    game.player.gridX = SIGNAL_TOWER_X;
    game.player.gridY = SIGNAL_TOWER_Y + 4;
    Require(Game_SelectAudioScene(&game) == AUDIO_SCENE_BOSS,
            "late tower plateau should use the pre-boss tension track");

    game.player.gridX = BOSS_ARENA_BOSS_X;
    game.player.gridY = BOSS_ARENA_BOSS_Y;
    Require(Game_SelectAudioScene(&game) == AUDIO_SCENE_BOSS_ARENA,
            "Northwest Ruins guardian fight should use the dedicated boss music variant");
    if (game.audio.ready) {
        Audio_SetScene(&game.audio, AUDIO_SCENE_BOSS_ARENA);
        Audio_Update(&game.audio);
        Require(game.audio.pendingSceneVariant == 1 || game.audio.activeSceneVariant == 1,
                "guardian-fight audio should start from the boss track instead of the tower pre-boss loop");
    }

    {
        StoryTriggerSnapshot storyBefore;

        Game_CloseStoryScene(&game);
        game.storySceneShown[STORY_SCENE_LOG_THE_CRASH] = false;
        game.tasks.logs[0].collected = false;
        GamePlay_CaptureStoryTriggerSnapshot(&game, &storyBefore);
        game.tasks.logs[0].collected = true;
        GamePlay_TryOpenStorySceneFromSnapshot(&game, &storyBefore);
        Require(game.storySceneOpen, "collecting the first log should open its story scene");
        Require(game.storyScene == STORY_SCENE_LOG_THE_CRASH,
                "the first collected log should map to the first log story scene");
        Game_CloseStoryScene(&game);
    }

    {
        StoryTriggerSnapshot storyBefore;
        const int purifierRingControlBriefIndex =
            STORY_SCENE_LOG_PURIFIER_RING_CONTROL_BRIEF - STORY_SCENE_LOG_THE_CRASH;

        Game_CloseStoryScene(&game);
        game.storySceneShown[STORY_SCENE_LOG_PURIFIER_RING_CONTROL_BRIEF] = false;
        game.tasks.logs[purifierRingControlBriefIndex].collected = false;
        GamePlay_CaptureStoryTriggerSnapshot(&game, &storyBefore);
        game.tasks.logs[purifierRingControlBriefIndex].collected = true;
        GamePlay_TryOpenStorySceneFromSnapshot(&game, &storyBefore);
        Require(game.storySceneOpen,
                "collecting the purifier ring control brief should now open the related mainline story scene");
        Require(game.storyScene == STORY_SCENE_MAIN_MONOLITH_TRUE_ROLE,
                "the purifier ring control brief should map to the monolith true role story scene");
        Game_CloseStoryScene(&game);
    }

    {
        StoryTriggerSnapshot storyBefore;

        Game_CloseStoryScene(&game);
        game.storySceneShown[STORY_SCENE_MAIN_THREE_COSTS_REVEALED] = false;
        game.tasks.stage = 7;
        game.tasks.westW5Completed = true;
        game.tasks.southS5Completed = true;
        game.tasks.endingArchiveReviewed = false;
        GamePlay_CaptureStoryTriggerSnapshot(&game, &storyBefore);
        GamePlay_TryOpenStorySceneFromSnapshot(&game, &storyBefore);
        Require(!game.storySceneOpen,
                "the route-costs story should stay quiet while the archive review step is still incomplete");

        GamePlay_CaptureStoryTriggerSnapshot(&game, &storyBefore);
        game.tasks.endingArchiveReviewed = true;
        GamePlay_TryOpenStorySceneFromSnapshot(&game, &storyBefore);
        Require(game.storySceneOpen,
                "the route-costs story should open once the final archive review is completed");
        Require(game.storyScene == STORY_SCENE_MAIN_THREE_COSTS_REVEALED,
                "archive review completion should map to the route-costs story scene");
        Game_CloseStoryScene(&game);
    }

    Game_OpenSavePanel(&game, SAVE_PANEL_MODE_SAVE);
    Require(game.savePanelOpen, "opening the save panel should mark it visible");
    Require(game.savePanelMode == SAVE_PANEL_MODE_SAVE, "save panel should remember the requested mode");

    Game_CloseSavePanel(&game);
    Require(!game.savePanelOpen, "closing the save panel should hide it");

    {
        const int startingDayCount = game.tasks.dayCount;

        TasksRuntime_UpdateDayCycle(&game.tasks, DAY_COUNT_DURATION_SECONDS - 1.0f);
        Require(game.tasks.dayCount == startingDayCount,
                "day count should not advance before five real-time minutes have elapsed");
        TasksRuntime_UpdateDayCycle(&game.tasks, 1.0f);
        Require(game.tasks.dayCount == startingDayCount + 1,
                "day count should advance exactly once when five real-time minutes are reached");
    }

    {
        Game sleepGame;
        const int startingDayCount = 0;

        PrepareGame(&sleepGame);
        sleepGame.tasks.elapsedSeconds = DAY_COUNT_DURATION_SECONDS - SLEEP_TIME_ADVANCE_SECONDS * 0.5f;
        sleepGame.tasks.dayCount = startingDayCount;
        sleepGame.tasks.cycleTimer = 0.0f;
        Game_AdvanceWorldClock(&sleepGame, SLEEP_TIME_ADVANCE_SECONDS);
        Game_MaybePostDayAdvanceMessage(&sleepGame, startingDayCount);
        Require(sleepGame.tasks.dayCount == startingDayCount + 1,
                "sleep transition should fast-forward one minute and allow that jump to begin the next day");
        Require(strstr(sleepGame.hudMessage.text, "Day 2 begins.") != NULL
                    || strstr(sleepGame.hudMessage.text, "第 2 天开始") != NULL,
                "crossing into a new day should post a short next-day message");
    }

    game.tasks.stage = 5;
    game.tasks.dayCount = 3;
    game.tasks.westW1Started = true;
    game.tasks.westW1Completed = true;
    game.tasks.westW2Started = true;
    game.tasks.westW2Completed = false;
    game.tasks.westW3Started = false;
    game.tasks.westW3Completed = false;
    game.tasks.westW4Started = false;
    game.tasks.westW4Completed = false;
    game.tasks.westW5Started = false;
    game.tasks.westW5Completed = false;
    game.tasks.southS1Started = true;
    game.tasks.southS1Completed = false;
    game.tasks.southS2Started = false;
    game.tasks.southS2Completed = false;
    game.tasks.southS3Started = false;
    game.tasks.southS3Completed = false;
    game.tasks.southS4Started = false;
    game.tasks.southS4Completed = false;
    game.tasks.southS5Started = false;
    game.tasks.southS5Completed = false;
    game.player.gridX = PLAYER_START_X;
    game.player.gridY = PLAYER_START_Y;
    game.player.resources[RESOURCE_WOOD] = 5;
    game.player.resources[RESOURCE_FRUIT] = 2;
    game.player.deathCount = 1;
    game.player.hasFieldCamp = true;
    game.map.campPlaced = true;
    game.map.campX = PLAYER_START_X + 2;
    game.map.campY = PLAYER_START_Y - 1;
    Player_UpdateWorldPosition(&game.player);
    Tasks_UpdateObjective(&game.tasks, &game.player);

    game.player.deathCount = 0;
    Game_OpenSavePanel(&game, SAVE_PANEL_MODE_SAVE);
    Require(game.selectedSaveSlot == 0, "first save should target the first empty slot");
    Game_ActivateSelectedSaveSlot(&game);
    Require(!game.hasAccountBestScore,
            "unfinished runs without a recorded death or ending should not backfill a best score into the account panel");
    Require(SaveSystem_DeleteGame(0), "session smoke should be able to delete the temporary no-death save");
    Game_RefreshSaveSlots(&game);

    game.tasks.stage = 5;
    game.tasks.dayCount = 3;
    game.tasks.westW1Started = true;
    game.tasks.westW1Completed = true;
    game.tasks.westW2Started = true;
    game.tasks.westW2Completed = false;
    game.tasks.southS1Started = true;
    game.tasks.southS1Completed = false;
    game.player.resources[RESOURCE_WOOD] = 5;
    game.player.resources[RESOURCE_FRUIT] = 2;
    game.player.deathCount = 1;
    memset(game.storySceneShown, 0, sizeof(game.storySceneShown));
    game.storySceneShown[STORY_SCENE_MAIN_AIR_FOR_ONE_MORE_DAY] = true;
    game.storySceneShown[STORY_SCENE_MAIN_MONOLITH_TRUE_ROLE] = true;
    game.storySceneShown[STORY_SCENE_MAIN_SETTLEMENT_ROUTE_COMMITMENT] = true;
    game.tasks.shownMainStorySceneCount = 3;
    game.player.hasFieldCamp = true;
    game.map.campPlaced = true;
    game.map.campX = PLAYER_START_X + 2;
    game.map.campY = PLAYER_START_Y - 1;
    Player_UpdateWorldPosition(&game.player);
    Tasks_UpdateObjective(&game.tasks, &game.player);

    Game_OpenSavePanel(&game, SAVE_PANEL_MODE_SAVE);
    Require(game.selectedSaveSlot == 0, "first save should target the first empty slot");
    Game_ActivateSelectedSaveSlot(&game);
    Require(game.hasSaveFile, "saving should refresh save-slot availability");
    Require(!game.savePanelOpen, "successful save should close the panel");
    Require(game.saveSlots[0].exists, "saved slot should be listed after saving");
    Require(game.hasAccountBestScore && game.accountBestScore > 0,
            "death-marked legacy saves should backfill the menu best score for the active account");
    Require(strstr(game.hudMessage.text, "Progress saved") != NULL, "save flow should publish success feedback");

    game.player.gridX = PLAYER_START_X + 17;
    game.player.gridY = PLAYER_START_Y + 8;
    game.player.resources[RESOURCE_WOOD] = 0;
    game.player.resources[RESOURCE_FRUIT] = 0;
    game.player.deathCount = 0;
    game.tasks.stage = 1;
    game.map.campPlaced = false;
    Player_UpdateWorldPosition(&game.player);

    Game_OpenSavePanel(&game, SAVE_PANEL_MODE_LOAD);
    Require(game.selectedSaveSlot == 0, "load flow should focus the first occupied slot");
    Game_ActivateSelectedSaveSlot(&game);
    Require(game.state == GAME_STATE_PLAYING, "loading a normal save should return to playing state");
    Require(!game.savePanelOpen, "successful load should close the panel");
    Require(game.player.gridX == PLAYER_START_X && game.player.gridY == PLAYER_START_Y,
            "load flow should restore the saved player position");
    Require(game.player.resources[RESOURCE_WOOD] == 5 && game.player.resources[RESOURCE_FRUIT] == 2,
            "load flow should restore saved resources");
    Require(game.player.deathCount == 1, "load flow should restore the saved death counter");
    Require(game.tasks.stage == 5 && game.tasks.dayCount == 3, "load flow should restore task progression");
    Require(game.tasks.westW1Started && game.tasks.westW1Completed,
            "load flow should restore persisted west-route W1 progression");
    Require(game.tasks.westW2Started && !game.tasks.westW2Completed,
            "load flow should restore persisted west-route W2 progression");
    Require(!game.tasks.westW3Started && !game.tasks.westW3Completed,
            "load flow should restore persisted west-route W3 progression");
    Require(!game.tasks.westW4Started && !game.tasks.westW4Completed,
            "load flow should restore persisted west-route W4 progression");
    Require(!game.tasks.westW5Started && !game.tasks.westW5Completed,
            "load flow should restore persisted west-route W5 progression");
    Require(game.tasks.southS1Started && !game.tasks.southS1Completed,
            "load flow should restore persisted south-route S1 progression");
    Require(!game.tasks.southS2Started && !game.tasks.southS2Completed,
            "load flow should restore persisted south-route S2 progression");
    Require(!game.tasks.southS3Started && !game.tasks.southS3Completed,
            "load flow should restore persisted south-route S3 progression");
    Require(!game.tasks.southS4Started && !game.tasks.southS4Completed,
            "load flow should restore persisted south-route S4 progression");
    Require(!game.tasks.southS5Started && !game.tasks.southS5Completed,
            "load flow should restore persisted south-route S5 progression");
    Require(game.storySceneShown[STORY_SCENE_MAIN_AIR_FOR_ONE_MORE_DAY],
            "load flow should restore the opening main-story archive entry");
    Require(game.storySceneShown[STORY_SCENE_MAIN_MONOLITH_TRUE_ROLE],
            "load flow should restore later main-story archive entries");
    Require(game.storySceneShown[STORY_SCENE_MAIN_SETTLEMENT_ROUTE_COMMITMENT],
            "load flow should preserve reserved late-story archive slots");
    shownMainStorySceneCount = 0;
    for (int scene = STORY_SCENE_MAIN_AIR_FOR_ONE_MORE_DAY;
         scene < STORY_SCENE_MAIN_AIR_FOR_ONE_MORE_DAY + STORY_MAIN_SCENE_COUNT;
         scene++) {
        shownMainStorySceneCount += game.storySceneShown[scene] ? 1 : 0;
    }
    Require(game.tasks.shownMainStorySceneCount == shownMainStorySceneCount,
            "load flow should keep ending-score story counts aligned with the restored archive state");
    Require(game.map.campPlaced && game.map.campX == PLAYER_START_X + 2 && game.map.campY == PLAYER_START_Y - 1,
            "load flow should restore the placed field camp");
    Require(game.camera.target.x == game.player.worldPos.x && game.camera.target.y == game.player.worldPos.y,
            "load flow should recenter the camera on the restored player position");
    Require(strstr(game.hudMessage.text, "Saved progress loaded.") != NULL, "load flow should publish success feedback");

    game.pauseMenuOpen = true;
    game.backpackOpen = true;
    game.communicatorOpen = true;
    game.showDeathPopup = true;
    game.settings.masterVolume = 0.42f;
    game.settings.sfxEnabled = false;
    game.settingsDirty = true;
    Game_ReturnToMenu(&game);
    Require(game.state == GAME_STATE_INTRO, "return to menu should switch back to intro state");
    Require(!game.pauseMenuOpen && !game.backpackOpen && !game.communicatorOpen, "return to menu should close active overlays");
    Require(!game.showDeathPopup, "return to menu should clear the death popup");
    Require(!game.settingsDirty, "return to menu should save pending settings when possible");
    Require(SaveSystem_LoadSettings(&loadedSettings), "return to menu should persist settings");
    Require(loadedSettings.masterVolume > 0.41f && loadedSettings.masterVolume < 0.43f,
            "return to menu should persist the pending volume setting");
    Require(!loadedSettings.sfxEnabled, "return to menu should persist the pending sfx setting");
    Require(!Game_SaveCurrentGame(&game, -1), "save flow should reject negative slot indexes");
    Require(!Game_SaveCurrentGame(&game, SAVE_SLOT_COUNT), "save flow should reject out-of-range slot indexes");
    Require(!Game_LoadSavedGame(&game, -1), "load flow should reject negative slot indexes");
    Require(!Game_LoadSavedGame(&game, SAVE_SLOT_COUNT), "load flow should reject out-of-range slot indexes");

    game.selectedSaveSlot = -1;
    Game_ActivateSelectedSaveSlot(&game);
    Require(strstr(game.hudMessage.text, "No save file") != NULL, "invalid selected load slot should fail gracefully");

    game.selectedSaveSlot = SAVE_SLOT_COUNT;
    Game_DeleteSelectedSaveSlot(&game);
    Require(strstr(game.hudMessage.text, "No save file") != NULL, "invalid selected delete slot should fail gracefully");

    PrepareGame(&game);
    game.tasks.stage = 5;
    game.player.resources[RESOURCE_FRUIT] = 4;
    game.player.resources[RESOURCE_ENERGY_CORE] = 1;
    game.player.hasFieldCamp = true;
    game.map.campPlaced = true;
    game.map.campX = PLAYER_START_X + 6;
    game.map.campY = PLAYER_START_Y;
    game.player.gridX = PLAYER_START_X + 12;
    game.player.gridY = PLAYER_START_Y - 8;
    game.player.health = 28.0f;
    game.player.oxygen = 0.0f;
    game.player.pressure = 64.0f;
    game.player.poison = 50.0f;
    Player_UpdateWorldPosition(&game.player);

    Game_HandlePlayerDeath(&game);
    Require(game.showDeathPopup, "first death should open the recovery popup");
    Require(game.player.deathCount == 1, "first death should increment death count");
    Require(game.player.health == 0.0f, "death handling should clamp health to zero");
    Require(game.tasks.ending == ENDING_NONE, "death popup flow should not route through the legacy failure ending");
    Require(game.hasAccountBestScore && game.accountBestScore > 0,
            "a completed death should immediately update the authenticated account's best score");

    Game_HandleDeathRecovery(&game);
    Require(!game.showDeathPopup, "recovery should close the popup");
    Require(game.state == GAME_STATE_PLAYING, "recovery should return to playing state");
    Require(game.tasks.stage == 1, "restart after death should begin a fresh run from stage 1");
    Require(game.player.resources[RESOURCE_FRUIT] == 0, "restart after death should clear carried resources");
    Require(game.player.resources[RESOURCE_ENERGY_CORE] == 0, "restart after death should clear crafted progression resources");
    Require(game.player.gridX == PLAYER_RESPAWN_X && game.player.gridY == PLAYER_RESPAWN_Y,
            "restart after death should place the player back in Loxi's room");
    Require(game.player.health == INITIAL_HEALTH, "restart after death should reset player health");
    Require(game.player.stamina == INITIAL_STAMINA, "restart after death should reset player stamina");
    Require(game.player.oxygen == INITIAL_OXYGEN, "restart after death should reset player oxygen");
    Require(game.player.pressure == INITIAL_PRESSURE, "restart after death should reset pressure");
    Require(game.player.poison == 0.0f, "restart after death should clear poison");
    Require(game.camera.zoom > 0.0f
                && game.camera.target.x == game.player.worldPos.x
                && game.camera.target.y == game.player.worldPos.y,
            "restart after death should rebuild a sane camera state");
    Require(strstr(game.hudMessage.text, "Wake in Loxi's cabin") != NULL
                || strstr(game.hudMessage.text, "先在洛希舱室醒来") != NULL,
            "restart after death should skip the opening recap and go straight to the first gameplay objective");

    PrepareGame(&game);
    game.tasks.stage = 4;
    game.player.gridX = PLAYER_START_X + 9;
    game.player.gridY = PLAYER_START_Y + 3;
    game.player.pressure = 45.0f;
    game.player.oxygen = 0.0f;
    Player_UpdateWorldPosition(&game.player);

    Game_HandlePlayerDeath(&game);
    Require(game.showDeathPopup, "single-life runs should still route through the death popup");
    Require(game.deathPopupSelection == DEATH_POPUP_BUTTON_RESTART,
            "death popup should default to restart");
    game.deathPopupSelection = DEATH_POPUP_BUTTON_LOAD;
    Game_OpenSavePanel(&game, SAVE_PANEL_MODE_LOAD);
    Require(game.savePanelOpen && game.savePanelMode == SAVE_PANEL_MODE_LOAD,
            "death flow should allow loading directly instead of only restarting");

    PrepareGame(&game);
    game.tasks.stage = 7;
    game.player.gridX = SIGNAL_TOWER_X;
    game.player.gridY = SIGNAL_TOWER_Y + 6;
    Player_UpdateWorldPosition(&game.player);
    Game_MaybePostNorthRouteTransitionHint(&game);
    Require(game.hudMessage.text[0] == '\0',
            "entering the tower plateau without prep should no longer force a location-entry warning");

    PrepareGame(&game);
    game.tasks.stage = 7;
    game.player.hasSignalAmplifier = true;
    game.player.gridX = SIGNAL_TOWER_X;
    game.player.gridY = SIGNAL_TOWER_Y + 6;
    Player_UpdateWorldPosition(&game.player);
    Game_MaybePostNorthRouteTransitionHint(&game);
    Require(game.hudMessage.text[0] == '\0',
            "entering the tower plateau with the amplifier should stay quiet until a real interaction or milestone happens");

    {
        Game reviewGame;

        PrepareGame(&reviewGame);
        reviewGame.tasks.stage = 7;
        reviewGame.tasks.oxygenRepairLevel = 2;
        reviewGame.tasks.commRepairLevel = 1;
        reviewGame.tasks.energyRepairLevel = 1;
        reviewGame.tasks.crashClueFound = true;
        reviewGame.tasks.amplifierUnlocked = true;
        reviewGame.tasks.westW5Completed = true;
        reviewGame.tasks.southS5Completed = true;
        reviewGame.tasks.endingArchiveReviewed = false;
        CollectAllMainlineLogs(&reviewGame.tasks);
        snprintf(reviewGame.lastLocationName, sizeof(reviewGame.lastLocationName), "%s", "Signal Tower Plateau");
        MovePlayerAndUpdate(&reviewGame, SHIP_CORRIDOR_X + 1, SHIP_CORRIDOR_Y + 1);
        Require(reviewGame.hudMessage.text[0] == '\0',
                "returning from the ruins with the full archive but no review should now stay quiet and leave guidance to the objective panel");
    }

    {
        Game endingTransitionGame;

        PrepareGame(&endingTransitionGame);
        endingTransitionGame.tasks.ending = ENDING_SETTLEMENT;
        Game_BeginScreenTransition(&endingTransitionGame, SCREEN_TRANSITION_ENTER_ENDING, -1);
        Require(endingTransitionGame.state == GAME_STATE_PLAYING,
                "arming the ending fade should not hard-cut into the ending state immediately");
        Require(endingTransitionGame.screenTransitionActive,
                "arming the ending fade should mark the screen transition as active");
        Require(endingTransitionGame.screenTransitionAction == SCREEN_TRANSITION_ENTER_ENDING,
                "ending entry should use the dedicated end-screen transition action");

        Game_EnterEndingState(&endingTransitionGame);
        Require(endingTransitionGame.state == GAME_STATE_ENDING,
                "resolving the ending transition should switch the session into the ending state");
    }

    {
        Game heroicReturnGame;

        PrepareGame(&heroicReturnGame);
        heroicReturnGame.tasks.stage = 7;
        heroicReturnGame.tasks.oxygenRepairLevel = 2;
        heroicReturnGame.tasks.commRepairLevel = 1;
        heroicReturnGame.tasks.energyRepairLevel = 1;
        heroicReturnGame.tasks.crashClueFound = true;
        heroicReturnGame.tasks.amplifierUnlocked = true;
        heroicReturnGame.tasks.westW5Completed = true;
        heroicReturnGame.tasks.southS5Completed = true;
        heroicReturnGame.tasks.endingArchiveReviewed = true;
        heroicReturnGame.tasks.selectedEndingRoute = ENDING_HEROIC;
        heroicReturnGame.tasks.bossDefeated = true;
        CollectAllMainlineLogs(&heroicReturnGame.tasks);
        snprintf(heroicReturnGame.lastLocationName, sizeof(heroicReturnGame.lastLocationName), "%s", "Northwest Ruins");
        MovePlayerAndUpdate(&heroicReturnGame, SHIP_CORRIDOR_X + 1, SHIP_CORRIDOR_Y + 1);
        Require(heroicReturnGame.hudMessage.text[0] == '\0',
                "returning from the northwest-ruins guardian fight after locking the heroic route should no longer auto-post a summary");
    }

    PrepareGame(&game);
    game.tasks.stage = 4;
    game.tasks.commRepairLevel = 1;
    MovePlayerAndUpdate(&game, EXTERIOR_X(24), EXTERIOR_Y(72));
    Require(game.tasks.westW1Started,
            "entering West Frontier with relay recovery complete should start W1");
    MovePlayerAndUpdate(&game, WORKBENCH_X, WORKBENCH_Y);
    Require(!game.tasks.westW1Completed,
            "returning to base from West Frontier without the record should keep W1 open");
    Require(game.hudMessage.text[0] == '\0',
            "missing west evidence should now stay quiet instead of auto-posting a reminder");
    CollectArchiveEvidence(&game.tasks, 3);
    MovePlayerAndUpdate(&game, EXTERIOR_X(24), EXTERIOR_Y(72));
    MovePlayerAndUpdate(&game, WORKBENCH_X, WORKBENCH_Y);
    Require(game.tasks.westW1Completed,
            "returning to base from West Frontier with the record should complete W1");

    {
        Game archiveRoomGame;

        PrepareGame(&archiveRoomGame);
        archiveRoomGame.tasks.stage = 4;
        archiveRoomGame.tasks.commRepairLevel = 1;
        archiveRoomGame.tasks.westW1Started = true;
        archiveRoomGame.tasks.westW1Completed = false;
        CollectArchiveEvidence(&archiveRoomGame.tasks, 3);
        snprintf(archiveRoomGame.lastLocationName, sizeof(archiveRoomGame.lastLocationName), "%s", "West Frontier");
        MovePlayerAndUpdate(&archiveRoomGame, SHIP_CORRIDOR_X + 1, SHIP_CORRIDOR_Y + 1);
        Require(archiveRoomGame.tasks.westW1Completed,
            "returning through Central Corridor with the record should still count as a valid base archive hand-in");
    }

    {
        Game archiveRoomGame;

        PrepareGame(&archiveRoomGame);
        archiveRoomGame.tasks.stage = 4;
        archiveRoomGame.tasks.commRepairLevel = 1;
        archiveRoomGame.tasks.westW1Completed = true;
        archiveRoomGame.tasks.westW2Started = true;
        archiveRoomGame.tasks.westW2Completed = false;
        CollectArchiveEvidence(&archiveRoomGame.tasks, 4);
        snprintf(archiveRoomGame.lastLocationName, sizeof(archiveRoomGame.lastLocationName), "%s", "Survey Break");
        MovePlayerAndUpdate(&archiveRoomGame, EXTERIOR_X(24), EXTERIOR_Y(72));
        MovePlayerAndUpdate(&archiveRoomGame, SHIP_WORKSHOP_X + 1, SHIP_WORKSHOP_Y + 1);
        Require(archiveRoomGame.tasks.westW2Completed,
                "returning from Survey Break through West Frontier should still archive W2 and unlock Canopy Hollow");
    }

    {
        Game archiveRoomGame;

        PrepareGame(&archiveRoomGame);
        archiveRoomGame.tasks.stage = 5;
        archiveRoomGame.tasks.energyRepairLevel = 1;
        archiveRoomGame.tasks.southS1Started = true;
        archiveRoomGame.tasks.southS1Completed = false;
        CollectArchiveEvidence(&archiveRoomGame.tasks, 9);
        snprintf(archiveRoomGame.lastLocationName, sizeof(archiveRoomGame.lastLocationName), "%s", "South Collapse");
        MovePlayerAndUpdate(&archiveRoomGame, SHIP_DIAGNOSTICS_X + 1, SHIP_DIAGNOSTICS_Y + 1);
        Require(archiveRoomGame.tasks.southS1Completed,
            "returning through Diagnostics with the record should still count as a valid base archive hand-in");
    }

    {
        Game archiveRoomGame;

        PrepareGame(&archiveRoomGame);
        archiveRoomGame.tasks.stage = 5;
        archiveRoomGame.tasks.energyRepairLevel = 1;
        archiveRoomGame.tasks.southS1Started = true;
        archiveRoomGame.tasks.southS1Completed = false;
        CollectArchiveEvidence(&archiveRoomGame.tasks, 9);
        snprintf(archiveRoomGame.lastLocationName, sizeof(archiveRoomGame.lastLocationName), "%s", "South Collapse");
        MovePlayerAndUpdate(&archiveRoomGame, SHIP_CREW_QUARTERS_X + 1, SHIP_CREW_QUARTERS_Y + 1);
        Require(archiveRoomGame.tasks.southS1Completed,
            "returning through Crew Quarters with the record should still count as a valid base archive hand-in");
    }

    {
        Game archiveRoomGame;

        PrepareGame(&archiveRoomGame);
        archiveRoomGame.tasks.stage = 5;
        archiveRoomGame.tasks.energyRepairLevel = 1;
        archiveRoomGame.tasks.southS1Completed = true;
        archiveRoomGame.tasks.southS2Started = true;
        archiveRoomGame.tasks.southS2Completed = false;
        CollectArchiveEvidence(&archiveRoomGame.tasks, 10);
        snprintf(archiveRoomGame.lastLocationName, sizeof(archiveRoomGame.lastLocationName), "%s", "Vent Galleries");
        MovePlayerAndUpdate(&archiveRoomGame, EXTERIOR_X(82), EXTERIOR_Y(96));
        MovePlayerAndUpdate(&archiveRoomGame, SHIP_DIAGNOSTICS_X + 1, SHIP_DIAGNOSTICS_Y + 1);
        Require(archiveRoomGame.tasks.southS2Completed,
                "returning from Vent Galleries through South Collapse should still archive S2 and unlock Service Shafts");
    }

    MovePlayerAndUpdate(&game, EXTERIOR_X(35), EXTERIOR_Y(76));
    Require(game.tasks.westW2Started && !game.tasks.westW2Completed,
            "entering Survey Break after W1 should transition to W2");
    MovePlayerAndUpdate(&game, WORKBENCH_X, WORKBENCH_Y);
    Require(!game.tasks.westW2Completed,
            "returning to base from Survey Break without the record should keep W2 open");
    CollectArchiveEvidence(&game.tasks, 4);
    MovePlayerAndUpdate(&game, EXTERIOR_X(35), EXTERIOR_Y(76));
    MovePlayerAndUpdate(&game, WORKBENCH_X, WORKBENCH_Y);
    Require(game.tasks.westW2Completed,
            "returning to base from Survey Break with the record should archive W2");
    MovePlayerAndUpdate(&game, EXTERIOR_X(41), EXTERIOR_Y(67));
    Require(game.tasks.westW3Started && !game.tasks.westW3Completed,
            "entering Canopy Hollow after W2 should transition to W3");
    MovePlayerAndUpdate(&game, WORKBENCH_X, WORKBENCH_Y);
    Require(!game.tasks.westW3Completed,
            "returning to base from Canopy Hollow without the record should keep W3 open");
    CollectArchiveEvidence(&game.tasks, 5);
    MovePlayerAndUpdate(&game, EXTERIOR_X(41), EXTERIOR_Y(67));
    MovePlayerAndUpdate(&game, WORKBENCH_X, WORKBENCH_Y);
    Require(game.tasks.westW3Completed,
            "returning to base from Canopy Hollow with the record should archive W3");
    MovePlayerAndUpdate(&game, EXTERIOR_X(26), EXTERIOR_Y(90));
    Require(game.tasks.westW4Started && !game.tasks.westW4Completed,
            "entering Echo Basin after W3 should transition to W4");
    MovePlayerAndUpdate(&game, WORKBENCH_X, WORKBENCH_Y);
    Require(!game.tasks.westW4Completed,
            "returning to base from Echo Basin without the record should keep W4 open");
    CollectArchiveEvidence(&game.tasks, 6);
    MovePlayerAndUpdate(&game, EXTERIOR_X(26), EXTERIOR_Y(90));
    MovePlayerAndUpdate(&game, WORKBENCH_X, WORKBENCH_Y);
    Require(game.tasks.westW4Completed,
            "returning to base from Echo Basin with the record should archive W4");
    MovePlayerAndUpdate(&game, EXTERIOR_X(45), EXTERIOR_Y(84));
    Require(game.tasks.westW5Started && !game.tasks.westW5Completed,
            "entering Last Camp after W4 should transition to W5");
    MovePlayerAndUpdate(&game, WORKBENCH_X, WORKBENCH_Y);
    Require(!game.tasks.westW5Completed,
            "returning to base from Last Camp without the record should keep W5 open");
    CollectArchiveEvidence(&game.tasks, 7);
    MovePlayerAndUpdate(&game, EXTERIOR_X(45), EXTERIOR_Y(84));
    MovePlayerAndUpdate(&game, WORKBENCH_X, WORKBENCH_Y);
    Require(game.tasks.westW5Completed,
            "returning to base from Last Camp with the record should archive W5");

    PrepareGame(&game);
    game.tasks.stage = 5;
    game.tasks.energyRepairLevel = 1;
    MovePlayerAndUpdate(&game, EXTERIOR_X(82), EXTERIOR_Y(96));
    Require(game.tasks.southS1Started,
            "entering South Collapse with power restore complete should start S1");
    MovePlayerAndUpdate(&game, WORKBENCH_X, WORKBENCH_Y);
    Require(!game.tasks.southS1Completed,
            "returning to base from South Collapse without the record should keep S1 open");
    Require(game.hudMessage.text[0] == '\0',
            "missing south evidence should now stay quiet instead of auto-posting a reminder");
    CollectArchiveEvidence(&game.tasks, 9);
    MovePlayerAndUpdate(&game, EXTERIOR_X(82), EXTERIOR_Y(96));
    MovePlayerAndUpdate(&game, WORKBENCH_X, WORKBENCH_Y);
    Require(game.tasks.southS1Completed,
            "returning to base from South Collapse with the record should complete S1");
    MovePlayerAndUpdate(&game, EXTERIOR_X(90), EXTERIOR_Y(96));
    Require(game.tasks.southS2Started && !game.tasks.southS2Completed,
            "entering Vent Galleries after S1 should transition to S2");
    MovePlayerAndUpdate(&game, WORKBENCH_X, WORKBENCH_Y);
    Require(!game.tasks.southS2Completed,
            "returning to base from Vent Galleries without the record should keep S2 open");
    CollectArchiveEvidence(&game.tasks, 10);
    MovePlayerAndUpdate(&game, EXTERIOR_X(90), EXTERIOR_Y(96));
    MovePlayerAndUpdate(&game, WORKBENCH_X, WORKBENCH_Y);
    Require(game.tasks.southS2Completed,
            "returning to base from Vent Galleries with the record should archive S2");
    MovePlayerAndUpdate(&game, EXTERIOR_X(104), EXTERIOR_Y(96));
    Require(game.tasks.southS3Started && !game.tasks.southS3Completed,
            "entering Service Shafts after S2 should transition to S3");
    MovePlayerAndUpdate(&game, WORKBENCH_X, WORKBENCH_Y);
    Require(!game.tasks.southS3Completed,
            "returning to base from Service Shafts without the record should keep S3 open");
    CollectArchiveEvidence(&game.tasks, 11);
    MovePlayerAndUpdate(&game, EXTERIOR_X(104), EXTERIOR_Y(96));
    MovePlayerAndUpdate(&game, WORKBENCH_X, WORKBENCH_Y);
    Require(game.tasks.southS3Completed,
            "returning to base from Service Shafts with the record should archive S3");
    MovePlayerAndUpdate(&game, EXTERIOR_X(112), EXTERIOR_Y(96));
    Require(game.tasks.southS4Started && !game.tasks.southS4Completed,
            "entering Purifier Ring after S3 should transition to S4");
    MovePlayerAndUpdate(&game, WORKBENCH_X, WORKBENCH_Y);
    Require(!game.tasks.southS4Completed,
            "returning to base from Purifier Ring without the record should keep S4 open");
    CollectArchiveEvidence(&game.tasks, 12);
    MovePlayerAndUpdate(&game, EXTERIOR_X(112), EXTERIOR_Y(96));
    MovePlayerAndUpdate(&game, WORKBENCH_X, WORKBENCH_Y);
    Require(game.tasks.southS4Completed,
            "returning to base from Purifier Ring with the record should archive S4");
    MovePlayerAndUpdate(&game, EXTERIOR_X(120), EXTERIOR_Y(96));
    Require(game.tasks.southS5Started && !game.tasks.southS5Completed,
            "entering Root Vault after S4 should transition to S5");
    MovePlayerAndUpdate(&game, WORKBENCH_X, WORKBENCH_Y);
    Require(!game.tasks.southS5Completed,
            "returning to base from Root Vault without the record should keep S5 open");
    CollectArchiveEvidence(&game.tasks, 13);
    MovePlayerAndUpdate(&game, EXTERIOR_X(120), EXTERIOR_Y(96));
    MovePlayerAndUpdate(&game, WORKBENCH_X, WORKBENCH_Y);
    Require(game.tasks.southS5Completed,
            "returning to base from Root Vault with the record should archive S5");

    PrepareGame(&game);
    game.tasks.stage = 5;
    game.player.resources[RESOURCE_ENERGY_CORE] = 1;
    snprintf(game.lastLocationName, sizeof(game.lastLocationName), "%s", "Deep Basin");
    game.player.gridX = SHIP_CORRIDOR_X + 1;
    game.player.gridY = SHIP_CORRIDOR_Y + 1;
    Player_UpdateWorldPosition(&game.player);
    Game_MaybePostNorthRouteTransitionHint(&game);
    Require(game.hudMessage.text[0] == '\0'
                || strstr(game.hudMessage.text, "east qualification run is complete") != NULL
                || strstr(game.hudMessage.text, "Energy Core returned") != NULL,
            "returning from the deep east route with the Energy Core should keep base-return summary behavior stable");

    PrepareGame(&game);
    game.tasks.stage = 6;
    game.player.resources[RESOURCE_RELIC_FRAGMENT] = 3;
    snprintf(game.lastLocationName, sizeof(game.lastLocationName), "%s", "Monolith Ring");
    game.player.gridX = SHIP_TERMINAL_BAY_X + 1;
    game.player.gridY = SHIP_TERMINAL_BAY_Y + 1;
    Player_UpdateWorldPosition(&game.player);
    Game_MaybePostNorthRouteTransitionHint(&game);
    Require(game.hudMessage.text[0] == '\0'
                || strstr(game.hudMessage.text, "fragments") != NULL
                || strstr(game.hudMessage.text, "Relic set ready") != NULL
                || strstr(game.hudMessage.text, "fragment set complete") != NULL,
            "returning from the ruins with fragment set should keep base-return summary behavior stable");

    return 0;
}
