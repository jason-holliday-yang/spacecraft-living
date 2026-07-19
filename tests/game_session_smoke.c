#include "game_manager.h"
#include "task_presentation.h"
#include "game_manager_internal.h"
#include "game_play_internal.h"
#include "game_session_internal.h"
#include "task_runtime_internal.h"
#include "task_survival_internal.h"

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
    Game_LoadLegacyCompatibilityMap(game);
    Game_RefreshSaveSlots(game);
    game->flow.state = GAME_STATE_PLAYING;
}

static void MovePlayerAndUpdate(Game *game, int gridX, int gridY) {
    game->runtime.player.gridX = gridX;
    game->runtime.player.gridY = gridY;
    Player_UpdateWorldPosition(&game->runtime.player);
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
    Require(game.flow.state == GAME_STATE_OPENING, "start new game should enter the opening cutscene state");
    Require(Game_SelectAudioScene(&game) == AUDIO_SCENE_MENU,
            "opening state should always resolve to the menu audio scene");
    Require(Game_SelectMusicStage(&game) == AUDIO_MUSIC_STAGE_1_WAKE,
            "opening cutscene should switch to Stage 1 wake music immediately after starting a new game");
    Require(game.ui.hudMessage.text[0] == '\0', "opening cutscene should not post the gameplay hint early");
    Require(game.runtime.tasks.ending == ENDING_NONE, "start new game should begin without an ending state");
    Require(game.runtime.tasks.objective[0] != '\0', "start new game should initialize the opening objective text");
    Require(game.runtime.player.gridX == PLAYER_RESPAWN_X && game.runtime.player.gridY == PLAYER_RESPAWN_Y,
            "start new game should place the player in the Loxi room");
    Require(Map_IsWalkable(&game.runtime.map, game.runtime.player.gridX, game.runtime.player.gridY), "start new game should place the player on a walkable tile");
    Require(game.runtime.camera.zoom > 0.0f, "start new game should initialize a positive camera zoom");
    Require(game.runtime.camera.target.x == game.runtime.player.worldPos.x && game.runtime.camera.target.y == game.runtime.player.worldPos.y,
            "start new game should center the camera on the player");
    Require(!Map_IsLoxiRoomUnlocked(&game.runtime.map),
            "new game should keep the player sealed in the Loxi room until the first interaction");
    Require(!game.ui.pauseMenuOpen && !game.ui.settingsOpen && !game.ui.infoOverlayOpen && !game.ui.craftOpen
                && !game.ui.helpOpen && !game.ui.logReaderOpen
                && !game.ui.savePanelOpen && !game.ui.showDeathPopup,
            "start new game should clear all overlays and popups");
    {
        Game historyGame;
        int index;

        memset(&historyGame, 0, sizeof(historyGame));
        Game_ResetGameplayWorld(&historyGame);
        historyGame.flow.state = GAME_STATE_PLAYING;
        for (index = 0; index < GAME_MESSAGE_HISTORY_LIMIT + 3; index++) {
            Game_PostMessage(&historyGame, "history smoke message", 1.0f);
        }
        Require(historyGame.ui.messageHistory.count == GAME_MESSAGE_HISTORY_LIMIT,
                "message history linked list should retain only the recent bounded entries");
        Require(historyGame.ui.messageHistory.head != NULL
                    && historyGame.ui.messageHistory.tail != NULL
                    && historyGame.ui.messageHistory.tail->next == NULL,
                "message history linked list should maintain valid head and tail links");
        Require(strstr(historyGame.ui.messageHistory.tail->text, "history smoke message") != NULL,
                "message history linked list should preserve posted message text");
        Game_ClearMessageHistory(&historyGame);
        Require(historyGame.ui.messageHistory.head == NULL
                    && historyGame.ui.messageHistory.tail == NULL
                    && historyGame.ui.messageHistory.count == 0,
                "message history linked list should clear all allocated nodes");
    }

    Game_CompleteOpeningCutscene(&game);
    Require(game.flow.state == GAME_STATE_PLAYING, "completing the opening cutscene should enter playing state");
    Require(Game_SelectAudioScene(&game) == AUDIO_SCENE_BASE,
            "fresh gameplay inside the ship should resolve to the base audio scene");
    Require(strstr(game.ui.hudMessage.text, "Sync the uplink") != NULL
                || strstr(game.ui.hudMessage.text, "Wake in Loxi's cabin") != NULL
                || strstr(game.ui.hudMessage.text, "完成同步") != NULL,
            "entering gameplay should post the opening objective hint");
    Require(game.ui.messageHistory.count == 1
                && game.ui.messageHistory.head == game.ui.messageHistory.tail
                && (strstr(game.ui.messageHistory.head->text, "Sync the uplink") != NULL
                    || strstr(game.ui.messageHistory.head->text, "Wake in Loxi's cabin") != NULL
                    || strstr(game.ui.messageHistory.head->text, "完成同步") != NULL),
            "entering gameplay should append the opening hint to the message history linked list");
    Require(Game_SelectMusicStage(&game) == AUDIO_MUSIC_STAGE_1_WAKE,
            "ship cabin opening should use the Stage 1 wake music");
    Require(Game_LoadLegacyCompatibilityMap(&game),
            "legacy route assertions should continue on the V15 compatibility map");

    game.runtime.tasks.stage = 3;
    game.runtime.player.gridX = EXTERIOR_X(70);
    game.runtime.player.gridY = EXTERIOR_Y(74);
    Require(Game_SelectMusicStage(&game) == AUDIO_MUSIC_STAGE_3_WILD,
            "stage 3 should use the wild-phase music in general crash forest");

    game.runtime.player.gridX = EXTERIOR_X(24);
    game.runtime.player.gridY = EXTERIOR_Y(72);
    Require(Game_SelectMusicStage(&game) == AUDIO_MUSIC_STAGE_3_WILD,
            "stage 3 should keep the same BGM across west-route shelves instead of switching by location");

    game.runtime.tasks.stage = 4;
    game.runtime.player.gridX = EXTERIOR_X(80);
    game.runtime.player.gridY = EXTERIOR_Y(98);
    Require(Game_SelectMusicStage(&game) == AUDIO_MUSIC_STAGE_4_RISK,
            "stage 4 should promote the route pressure music instead of depending on forest-route location tags");

    game.runtime.tasks.stage = 5;
    game.runtime.player.gridX = EXTERIOR_X(111);
    game.runtime.player.gridY = EXTERIOR_Y(48);
    Require(Game_SelectMusicStage(&game) == AUDIO_MUSIC_STAGE_5_POWER,
            "stage 5 should use the power-breakthrough music even in deep swamp objectives");

    game.runtime.tasks.stage = 6;
    game.runtime.player.gridX = EXTERIOR_X(92);
    game.runtime.player.gridY = EXTERIOR_Y(98);
    Require(Game_SelectMusicStage(&game) == AUDIO_MUSIC_STAGE_6_RELICS,
            "stage 6 should use the relic-preparation music across the southern facility route");
    Require(Game_SelectAudioScene(&game) == AUDIO_SCENE_RUINS_FACILITY,
            "southern facility locations should use the dedicated ruins-facility audio scene");

    game.runtime.tasks.stage = 7;
    game.runtime.tasks.selectedEndingRoute = ENDING_NONE;
    game.runtime.tasks.bossDefeated = false;
    game.runtime.player.gridX = SIGNAL_TOWER_X;
    game.runtime.player.gridY = SIGNAL_TOWER_Y + 4;
    Require(strcmp(Map_GetLocationNameAt(game.runtime.player.gridX, game.runtime.player.gridY), "Signal Tower Plateau") == 0,
            "tower smoke coordinate should still land on Signal Tower Plateau");
    Require(Game_SelectMusicStage(&game) == AUDIO_MUSIC_STAGE_7_CHOICE,
            "late tower plateau should keep stage-7 music until the player enters the actual boss arena or locks a route");
    Require(Game_SelectAudioScene(&game) == AUDIO_SCENE_RUINS,
            "tower plateau should stay on the normal ruins scene until the player enters the actual boss arena");

    game.runtime.player.gridX = EXTERIOR_X(48);
    game.runtime.player.gridY = EXTERIOR_Y(12);
    Require(strcmp(Map_GetLocationNameAt(game.runtime.player.gridX, game.runtime.player.gridY), "Monolith Ring") == 0,
            "upper ruins smoke coordinate should still land in Monolith Ring");
    Require(Game_SelectAudioScene(&game) == AUDIO_SCENE_RUINS,
            "Monolith Ring should stay on the standard ruins scene even near the tower-height band");

    game.runtime.player.gridX = BOSS_ARENA_BOSS_X;
    game.runtime.player.gridY = BOSS_ARENA_BOSS_Y;
    Require(Game_SelectMusicStage(&game) == AUDIO_MUSIC_BOSS,
            "boss arena should continue to force boss music regardless of the surrounding stage");
    Require(Game_SelectAudioScene(&game) == AUDIO_SCENE_BOSS_ARENA,
            "only the actual boss arena should resolve to the boss-arena audio scene");

    game.runtime.player.gridX = EXTERIOR_X(70);
    game.runtime.player.gridY = EXTERIOR_Y(74);
    Require(Game_SelectMusicStage(&game) == AUDIO_MUSIC_STAGE_7_CHOICE,
            "stage 7 without a locked route should use the final-choice music away from boss spaces");

    game.runtime.tasks.selectedEndingRoute = ENDING_HEROIC;
    Require(Game_SelectMusicStage(&game) == AUDIO_MUSIC_STAGE_7_CHOICE,
            "heroic route selection should not override stage-7 music away from the real tower plateau");

    game.runtime.player.gridX = SIGNAL_TOWER_X;
    game.runtime.player.gridY = SIGNAL_TOWER_Y + 4;
    Require(Game_SelectMusicStage(&game) == AUDIO_MUSIC_ROUTE_HERO,
            "heroic route selection should override the generic stage-7 music on the real tower plateau");

    game.runtime.tasks.selectedEndingRoute = ENDING_PEACEFUL;
    Require(Game_SelectMusicStage(&game) == AUDIO_MUSIC_ROUTE_PEACEFUL,
            "peaceful route selection should switch to the peaceful-route music stage on the real tower plateau");

    game.runtime.tasks.selectedEndingRoute = ENDING_SETTLEMENT;
    Require(Game_SelectMusicStage(&game) == AUDIO_MUSIC_STAGE_7_CHOICE,
            "settlement route selection should not override stage-7 music away from the base");

    game.runtime.player.gridX = SHIP_CORRIDOR_X + 1;
    game.runtime.player.gridY = SHIP_CORRIDOR_Y + 1;
    Require(Game_SelectMusicStage(&game) == AUDIO_MUSIC_ROUTE_SETTLEMENT,
            "settlement route selection should switch to the settlement-route music stage inside the base");

    game.runtime.tasks.ending = ENDING_SETTLEMENT;
    Require(Game_SelectMusicStage(&game) == AUDIO_MUSIC_ENDING,
            "completed endings should override route and stage music with ending music");
    game.runtime.tasks.ending = ENDING_NONE;

    if (game.services.audio.ready) {
        Audio_SetMusicStage(&game.services.audio, AUDIO_MUSIC_STAGE_4_RISK);
        Audio_Update(&game.services.audio);
        Require(game.services.audio.activeMusicStage == AUDIO_MUSIC_STAGE_4_RISK,
                "audio manager should immediately activate the requested stage music when no prior track is playing");
        Require(game.services.audio.activeMusicVariant == 0,
                "stage-driven music should start from the primary variant before any end-of-track rotation");

        Audio_SetMusicStage(&game.services.audio, AUDIO_MUSIC_ROUTE_SETTLEMENT);
        Audio_Update(&game.services.audio);
        Require(game.services.audio.requestedMusicStage == AUDIO_MUSIC_ROUTE_SETTLEMENT,
                "route music requests should be stored on the dedicated music-stage channel");
    }

    game.runtime.tasks.selectedEndingRoute = ENDING_NONE;
    game.runtime.tasks.stage = 7;
    game.runtime.tasks.bossDefeated = false;
    game.runtime.player.gridX = SIGNAL_TOWER_X;
    game.runtime.player.gridY = SIGNAL_TOWER_Y + 4;
    Player_UpdateWorldPosition(&game.runtime.player);

    {
        SaveSnapshot snapshot;
        Game loadedGame;
        int bossIndex;

        bossIndex = -1;
        for (int index = 0; index < game.runtime.tasks.monsterCount; index++) {
            if (game.runtime.tasks.monsters[index].type == MONSTER_FINAL_BOSS) {
                bossIndex = index;
                break;
            }
        }
        Require(bossIndex >= 0, "session smoke should find the final boss for legacy health migration");
        Game_BuildSaveSnapshot(&game, &snapshot);
        snapshot.monsters[bossIndex].active = true;
        snapshot.monsters[bossIndex].health = 220.0f;
        memset(&loadedGame, 0, sizeof(loadedGame));
        Require(Game_LoadSnapshotIntoSession(&loadedGame, &snapshot),
                "loading a legacy full-health boss snapshot should succeed");
        Require(loadedGame.runtime.tasks.monsters[bossIndex].health == loadedGame.runtime.tasks.monsters[bossIndex].maxHealth,
                "legacy full-health boss saves should migrate to the current full health instead of showing a half bar");
    }

    {
        StoryTriggerSnapshot storyBefore;

        Game_CloseStoryScene(&game);
        game.flow.storySceneShown[STORY_SCENE_LOG_THE_CRASH] = false;
        game.runtime.tasks.logs[0].collected = false;
        GamePlay_CaptureStoryTriggerSnapshot(&game, &storyBefore);
        game.runtime.tasks.logs[0].collected = true;
        GamePlay_TryOpenStorySceneFromSnapshot(&game, &storyBefore);
        Require(game.flow.storySceneOpen, "collecting the first log should open its story scene");
        Require(game.flow.storyScene == STORY_SCENE_LOG_THE_CRASH,
                "the first collected log should map to the first log story scene");
        Game_CloseStoryScene(&game);
    }

    {
        StoryTriggerSnapshot storyBefore;
        const int purifierRingControlBriefIndex =
            STORY_SCENE_LOG_PURIFIER_RING_CONTROL_BRIEF - STORY_SCENE_LOG_THE_CRASH;

        Game_CloseStoryScene(&game);
        game.flow.storySceneShown[STORY_SCENE_LOG_PURIFIER_RING_CONTROL_BRIEF] = false;
        game.runtime.tasks.logs[purifierRingControlBriefIndex].collected = false;
        GamePlay_CaptureStoryTriggerSnapshot(&game, &storyBefore);
        game.runtime.tasks.logs[purifierRingControlBriefIndex].collected = true;
        GamePlay_TryOpenStorySceneFromSnapshot(&game, &storyBefore);
        Require(game.flow.storySceneOpen,
                "collecting the purifier ring control brief should now open the related mainline story scene");
        Require(game.flow.storyScene == STORY_SCENE_MAIN_MONOLITH_TRUE_ROLE,
                "the purifier ring control brief should map to the monolith true role story scene");
        Game_CloseStoryScene(&game);
    }

    {
        StoryTriggerSnapshot storyBefore;

        Game_CloseStoryScene(&game);
        game.flow.storySceneShown[STORY_SCENE_MAIN_THREE_COSTS_REVEALED] = false;
        game.runtime.tasks.stage = 7;
        game.runtime.tasks.westW5Completed = true;
        game.runtime.tasks.southS5Completed = true;
        game.runtime.tasks.endingArchiveReviewed = false;
        GamePlay_CaptureStoryTriggerSnapshot(&game, &storyBefore);
        GamePlay_TryOpenStorySceneFromSnapshot(&game, &storyBefore);
        Require(!game.flow.storySceneOpen,
                "the route-costs story should stay quiet while the archive review step is still incomplete");

        GamePlay_CaptureStoryTriggerSnapshot(&game, &storyBefore);
        game.runtime.tasks.endingArchiveReviewed = true;
        GamePlay_TryOpenStorySceneFromSnapshot(&game, &storyBefore);
        Require(game.flow.storySceneOpen,
                "the route-costs story should open once the final archive review is completed");
        Require(game.flow.storyScene == STORY_SCENE_MAIN_THREE_COSTS_REVEALED,
                "archive review completion should map to the route-costs story scene");
        Game_CloseStoryScene(&game);
    }

    Game_OpenSavePanel(&game, SAVE_PANEL_MODE_SAVE);
    Require(game.ui.savePanelOpen, "opening the save panel should mark it visible");
    Require(game.ui.savePanelMode == SAVE_PANEL_MODE_SAVE, "save panel should remember the requested mode");

    Game_CloseSavePanel(&game);
    Require(!game.ui.savePanelOpen, "closing the save panel should hide it");

    {
        const int startingDayCount = game.runtime.tasks.dayCount;

        TasksRuntime_UpdateDayCycle(&game.runtime.tasks, DAY_COUNT_DURATION_SECONDS - 1.0f);
        Require(game.runtime.tasks.dayCount == startingDayCount,
                "day count should not advance before five real-time minutes have elapsed");
        TasksRuntime_UpdateDayCycle(&game.runtime.tasks, 1.0f);
        Require(game.runtime.tasks.dayCount == startingDayCount + 1,
                "day count should advance exactly once when five real-time minutes are reached");
    }

    {
        Game sleepGame;
        const int startingDayCount = 0;

        PrepareGame(&sleepGame);
        sleepGame.runtime.tasks.elapsedSeconds = DAY_COUNT_DURATION_SECONDS - SLEEP_TIME_ADVANCE_SECONDS * 0.5f;
        sleepGame.runtime.tasks.dayCount = startingDayCount;
        sleepGame.runtime.tasks.cycleTimer = 0.0f;
        Game_AdvanceWorldClock(&sleepGame, SLEEP_TIME_ADVANCE_SECONDS);
        Game_MaybePostDayAdvanceMessage(&sleepGame, startingDayCount);
        Require(sleepGame.runtime.tasks.dayCount == startingDayCount + 1,
                "sleep transition should fast-forward one minute and allow that jump to begin the next day");
        Require(strstr(sleepGame.ui.hudMessage.text, "Day 2 begins.") != NULL
                    || strstr(sleepGame.ui.hudMessage.text, "第 2 天开始") != NULL,
                "crossing into a new day should post a short next-day message");
    }

    game.runtime.tasks.stage = 5;
    game.runtime.tasks.dayCount = 3;
    game.runtime.tasks.westW1Started = true;
    game.runtime.tasks.westW1Completed = true;
    game.runtime.tasks.westW2Started = true;
    game.runtime.tasks.westW2Completed = false;
    game.runtime.tasks.westW3Started = false;
    game.runtime.tasks.westW3Completed = false;
    game.runtime.tasks.westW4Started = false;
    game.runtime.tasks.westW4Completed = false;
    game.runtime.tasks.westW5Started = false;
    game.runtime.tasks.westW5Completed = false;
    game.runtime.tasks.southS1Started = true;
    game.runtime.tasks.southS1Completed = false;
    game.runtime.tasks.southS2Started = false;
    game.runtime.tasks.southS2Completed = false;
    game.runtime.tasks.southS3Started = false;
    game.runtime.tasks.southS3Completed = false;
    game.runtime.tasks.southS4Started = false;
    game.runtime.tasks.southS4Completed = false;
    game.runtime.tasks.southS5Started = false;
    game.runtime.tasks.southS5Completed = false;
    game.runtime.player.gridX = PLAYER_START_X;
    game.runtime.player.gridY = PLAYER_START_Y;
    game.runtime.player.resources[RESOURCE_WOOD] = 5;
    game.runtime.player.resources[RESOURCE_FRUIT] = 2;
    game.runtime.player.deathCount = 1;
    game.runtime.player.hasFieldCamp = true;
    game.runtime.map.campPlaced = true;
    game.runtime.map.campX = PLAYER_START_X + 2;
    game.runtime.map.campY = PLAYER_START_Y - 1;
    Player_UpdateWorldPosition(&game.runtime.player);
    Tasks_UpdateObjective(&game.runtime.tasks, &game.runtime.player);

    game.runtime.player.deathCount = 0;
    Game_OpenSavePanel(&game, SAVE_PANEL_MODE_SAVE);
    Require(game.ui.selectedSaveSlot == 0, "first save should target the first empty slot");
    Game_ActivateSelectedSaveSlot(&game);
    Require(!game.account.hasAccountBestScore,
            "unfinished runs without a recorded death or ending should not backfill a best score into the account panel");
    Require(SaveSystem_DeleteGame(0), "session smoke should be able to delete the temporary no-death save");
    Game_RefreshSaveSlots(&game);

    game.runtime.tasks.stage = 5;
    game.runtime.tasks.dayCount = 3;
    game.runtime.tasks.westW1Started = true;
    game.runtime.tasks.westW1Completed = true;
    game.runtime.tasks.westW2Started = true;
    game.runtime.tasks.westW2Completed = false;
    game.runtime.tasks.southS1Started = true;
    game.runtime.tasks.southS1Completed = false;
    game.runtime.player.resources[RESOURCE_WOOD] = 5;
    game.runtime.player.resources[RESOURCE_FRUIT] = 2;
    game.runtime.player.deathCount = 1;
    memset(game.flow.storySceneShown, 0, sizeof(game.flow.storySceneShown));
    game.flow.storySceneShown[STORY_SCENE_MAIN_AIR_FOR_ONE_MORE_DAY] = true;
    game.flow.storySceneShown[STORY_SCENE_MAIN_MONOLITH_TRUE_ROLE] = true;
    game.flow.storySceneShown[STORY_SCENE_MAIN_SETTLEMENT_ROUTE_COMMITMENT] = true;
    game.runtime.tasks.shownMainStorySceneCount = 3;
    game.runtime.player.hasFieldCamp = true;
    game.runtime.map.campPlaced = true;
    game.runtime.map.campX = PLAYER_START_X + 2;
    game.runtime.map.campY = PLAYER_START_Y - 1;
    Player_UpdateWorldPosition(&game.runtime.player);
    Tasks_UpdateObjective(&game.runtime.tasks, &game.runtime.player);

    Game_OpenSavePanel(&game, SAVE_PANEL_MODE_SAVE);
    Require(game.ui.selectedSaveSlot == 0, "first save should target the first empty slot");
    Game_ActivateSelectedSaveSlot(&game);
    Require(game.account.hasSaveFile, "saving should refresh save-slot availability");
    Require(!game.ui.savePanelOpen, "successful save should close the panel");
    Require(game.account.saveSlots[0].exists, "saved slot should be listed after saving");
    Require(game.account.hasAccountBestScore && game.account.accountBestScore > 0,
            "death-marked legacy saves should backfill the menu best score for the active account");
    Require(strstr(game.ui.hudMessage.text, "Progress saved") != NULL, "save flow should publish success feedback");

    game.runtime.player.gridX = PLAYER_START_X + 17;
    game.runtime.player.gridY = PLAYER_START_Y + 8;
    game.runtime.player.resources[RESOURCE_WOOD] = 0;
    game.runtime.player.resources[RESOURCE_FRUIT] = 0;
    game.runtime.player.deathCount = 0;
    game.runtime.tasks.stage = 1;
    game.runtime.map.campPlaced = false;
    Player_UpdateWorldPosition(&game.runtime.player);

    Game_OpenSavePanel(&game, SAVE_PANEL_MODE_LOAD);
    Require(game.ui.selectedSaveSlot == 0, "load flow should focus the first occupied slot");
    Game_ActivateSelectedSaveSlot(&game);
    Require(game.flow.state == GAME_STATE_PLAYING, "loading a normal save should return to playing state");
    Require(!game.ui.savePanelOpen, "successful load should close the panel");
    Require(game.runtime.player.gridX == PLAYER_START_X && game.runtime.player.gridY == PLAYER_START_Y,
            "load flow should restore the saved player position");
    Require(game.runtime.player.resources[RESOURCE_WOOD] == 5 && game.runtime.player.resources[RESOURCE_FRUIT] == 2,
            "load flow should restore saved resources");
    Require(game.runtime.player.deathCount == 1, "load flow should restore the saved death counter");
    Require(game.runtime.tasks.stage == 5 && game.runtime.tasks.dayCount == 3, "load flow should restore task progression");
    Require(game.runtime.tasks.westW1Started && game.runtime.tasks.westW1Completed,
            "load flow should restore persisted west-route W1 progression");
    Require(game.runtime.tasks.westW2Started && !game.runtime.tasks.westW2Completed,
            "load flow should restore persisted west-route W2 progression");
    Require(!game.runtime.tasks.westW3Started && !game.runtime.tasks.westW3Completed,
            "load flow should restore persisted west-route W3 progression");
    Require(!game.runtime.tasks.westW4Started && !game.runtime.tasks.westW4Completed,
            "load flow should restore persisted west-route W4 progression");
    Require(!game.runtime.tasks.westW5Started && !game.runtime.tasks.westW5Completed,
            "load flow should restore persisted west-route W5 progression");
    Require(game.runtime.tasks.southS1Started && !game.runtime.tasks.southS1Completed,
            "load flow should restore persisted south-route S1 progression");
    Require(!game.runtime.tasks.southS2Started && !game.runtime.tasks.southS2Completed,
            "load flow should restore persisted south-route S2 progression");
    Require(!game.runtime.tasks.southS3Started && !game.runtime.tasks.southS3Completed,
            "load flow should restore persisted south-route S3 progression");
    Require(!game.runtime.tasks.southS4Started && !game.runtime.tasks.southS4Completed,
            "load flow should restore persisted south-route S4 progression");
    Require(!game.runtime.tasks.southS5Started && !game.runtime.tasks.southS5Completed,
            "load flow should restore persisted south-route S5 progression");
    Require(game.flow.storySceneShown[STORY_SCENE_MAIN_AIR_FOR_ONE_MORE_DAY],
            "load flow should restore the opening main-story archive entry");
    Require(game.flow.storySceneShown[STORY_SCENE_MAIN_MONOLITH_TRUE_ROLE],
            "load flow should restore later main-story archive entries");
    Require(game.flow.storySceneShown[STORY_SCENE_MAIN_SETTLEMENT_ROUTE_COMMITMENT],
            "load flow should preserve reserved late-story archive slots");
    shownMainStorySceneCount = 0;
    for (int scene = STORY_SCENE_MAIN_AIR_FOR_ONE_MORE_DAY;
         scene < STORY_SCENE_MAIN_AIR_FOR_ONE_MORE_DAY + STORY_MAIN_SCENE_COUNT;
         scene++) {
        shownMainStorySceneCount += game.flow.storySceneShown[scene] ? 1 : 0;
    }
    Require(game.runtime.tasks.shownMainStorySceneCount == shownMainStorySceneCount,
            "load flow should keep ending-score story counts aligned with the restored archive state");
    Require(game.runtime.map.campPlaced && game.runtime.map.campX == PLAYER_START_X + 2 && game.runtime.map.campY == PLAYER_START_Y - 1,
            "load flow should restore the placed field camp");
    Require(game.runtime.camera.target.x == game.runtime.player.worldPos.x && game.runtime.camera.target.y == game.runtime.player.worldPos.y,
            "load flow should recenter the camera on the restored player position");
    Require(strstr(game.ui.hudMessage.text, "Saved progress loaded.") != NULL, "load flow should publish success feedback");

    game.ui.pauseMenuOpen = true;
    game.ui.infoOverlayOpen = true;
    game.ui.infoOverlayTab = INFO_OVERLAY_TAB_LOXI;
    game.ui.showDeathPopup = true;
    game.services.settings.masterVolume = 0.42f;
    game.services.settings.sfxEnabled = false;
    game.ui.settingsDirty = true;
    Game_ReturnToMenu(&game);
    Require(game.flow.state == GAME_STATE_INTRO, "return to menu should switch back to intro state");
    Require(!game.ui.pauseMenuOpen && !game.ui.infoOverlayOpen, "return to menu should close active overlays");
    Require(!game.ui.showDeathPopup, "return to menu should clear the death popup");
    Require(!game.ui.settingsDirty, "return to menu should save pending settings when possible");
    Require(SaveSystem_LoadSettings(&loadedSettings), "return to menu should persist settings");
    Require(loadedSettings.masterVolume > 0.41f && loadedSettings.masterVolume < 0.43f,
            "return to menu should persist the pending volume setting");
    Require(!loadedSettings.sfxEnabled, "return to menu should persist the pending sfx setting");
    Require(!Game_SaveCurrentGame(&game, -1), "save flow should reject negative slot indexes");
    Require(!Game_SaveCurrentGame(&game, SAVE_SLOT_COUNT), "save flow should reject out-of-range slot indexes");
    Require(!Game_LoadSavedGame(&game, -1), "load flow should reject negative slot indexes");
    Require(!Game_LoadSavedGame(&game, SAVE_SLOT_COUNT), "load flow should reject out-of-range slot indexes");

    game.ui.selectedSaveSlot = -1;
    Game_ActivateSelectedSaveSlot(&game);
    Require(strstr(game.ui.hudMessage.text, "No save file") != NULL, "invalid selected load slot should fail gracefully");

    game.ui.selectedSaveSlot = SAVE_SLOT_COUNT;
    Game_DeleteSelectedSaveSlot(&game);
    Require(strstr(game.ui.hudMessage.text, "No save file") != NULL, "invalid selected delete slot should fail gracefully");

    PrepareGame(&game);
    game.runtime.tasks.stage = 5;
    game.runtime.player.resources[RESOURCE_FRUIT] = 4;
    game.runtime.player.resources[RESOURCE_ENERGY_CORE] = 1;
    game.runtime.player.hasFieldCamp = true;
    game.runtime.map.campPlaced = true;
    game.runtime.map.campX = PLAYER_START_X + 6;
    game.runtime.map.campY = PLAYER_START_Y;
    game.runtime.player.gridX = PLAYER_START_X + 12;
    game.runtime.player.gridY = PLAYER_START_Y - 8;
    game.runtime.player.health = 28.0f;
    game.runtime.player.oxygen = 0.0f;
    game.runtime.player.poison = 50.0f;
    Player_UpdateWorldPosition(&game.runtime.player);

    Game_HandlePlayerDeath(&game);
    Require(game.ui.showDeathPopup, "first death should open the recovery popup");
    Require(game.runtime.player.deathCount == 1, "first death should increment death count");
    Require(game.runtime.player.health == 0.0f, "death handling should clamp health to zero");
    Require(game.runtime.tasks.ending == ENDING_NONE, "death popup flow should not route through the legacy failure ending");
    Require(game.account.hasAccountBestScore && game.account.accountBestScore > 0,
            "a completed death should immediately update the authenticated account's best score");

    Game_HandleDeathRecovery(&game);
    Require(!game.ui.showDeathPopup, "recovery should close the popup");
    Require(game.flow.state == GAME_STATE_PLAYING, "recovery should return to playing state");
    Require(game.runtime.tasks.stage == 1, "restart after death should begin a fresh run from stage 1");
    Require(game.runtime.player.resources[RESOURCE_FRUIT] == 0, "restart after death should clear carried resources");
    Require(game.runtime.player.resources[RESOURCE_ENERGY_CORE] == 0, "restart after death should clear crafted progression resources");
    Require(game.runtime.player.gridX == PLAYER_RESPAWN_X && game.runtime.player.gridY == PLAYER_RESPAWN_Y,
            "restart after death should place the player back in Loxi's room");
    Require(game.runtime.player.health == INITIAL_HEALTH, "restart after death should reset player health");
    Require(game.runtime.player.oxygen == INITIAL_OXYGEN, "restart after death should reset player oxygen");
    Require(game.runtime.player.poison == 0.0f, "restart after death should clear poison");
    Require(game.runtime.camera.zoom > 0.0f
                && game.runtime.camera.target.x == game.runtime.player.worldPos.x
                && game.runtime.camera.target.y == game.runtime.player.worldPos.y,
            "restart after death should rebuild a sane camera state");
    Require(strstr(game.ui.hudMessage.text, "Wake in Loxi's cabin") != NULL
                || strstr(game.ui.hudMessage.text, "先在洛希舱室醒来") != NULL,
            "restart after death should skip the opening recap and go straight to the first gameplay objective");

    PrepareGame(&game);
    game.runtime.tasks.stage = 4;
    game.runtime.player.gridX = PLAYER_START_X + 9;
    game.runtime.player.gridY = PLAYER_START_Y + 3;
    game.runtime.player.oxygen = 0.0f;
    Player_UpdateWorldPosition(&game.runtime.player);

    Game_HandlePlayerDeath(&game);
    Require(game.ui.showDeathPopup, "single-life runs should still route through the death popup");
    Require(game.ui.deathPopupSelection == DEATH_POPUP_BUTTON_RESTART,
            "death popup should default to restart");
    game.ui.deathPopupSelection = DEATH_POPUP_BUTTON_LOAD;
    Game_OpenSavePanel(&game, SAVE_PANEL_MODE_LOAD);
    Require(game.ui.savePanelOpen && game.ui.savePanelMode == SAVE_PANEL_MODE_LOAD,
            "death flow should allow loading directly instead of only restarting");

    PrepareGame(&game);
    game.runtime.tasks.stage = 7;
    game.runtime.player.gridX = SIGNAL_TOWER_X;
    game.runtime.player.gridY = SIGNAL_TOWER_Y + 6;
    Player_UpdateWorldPosition(&game.runtime.player);
    Game_MaybePostNorthRouteTransitionHint(&game);
    Require(game.ui.hudMessage.text[0] == '\0',
            "entering the tower plateau without prep should no longer force a location-entry warning");

    PrepareGame(&game);
    game.runtime.tasks.stage = 7;
    game.runtime.player.hasSignalAmplifier = true;
    game.runtime.player.gridX = SIGNAL_TOWER_X;
    game.runtime.player.gridY = SIGNAL_TOWER_Y + 6;
    Player_UpdateWorldPosition(&game.runtime.player);
    Game_MaybePostNorthRouteTransitionHint(&game);
    Require(game.ui.hudMessage.text[0] == '\0',
            "entering the tower plateau with the amplifier should stay quiet until a real interaction or milestone happens");

    {
        Game reviewGame;

        PrepareGame(&reviewGame);
        reviewGame.runtime.tasks.stage = 7;
        reviewGame.runtime.tasks.oxygenRepairLevel = 2;
        reviewGame.runtime.tasks.commRepairLevel = 1;
        reviewGame.runtime.tasks.energyRepairLevel = 1;
        reviewGame.runtime.tasks.crashClueFound = true;
        reviewGame.runtime.tasks.amplifierUnlocked = true;
        reviewGame.runtime.tasks.westW5Completed = true;
        reviewGame.runtime.tasks.southS5Completed = true;
        reviewGame.runtime.tasks.endingArchiveReviewed = false;
        CollectAllMainlineLogs(&reviewGame.runtime.tasks);
        snprintf(reviewGame.runtime.lastLocationName, sizeof(reviewGame.runtime.lastLocationName), "%s", "Signal Tower Plateau");
        MovePlayerAndUpdate(&reviewGame, SHIP_CORRIDOR_X + 1, SHIP_CORRIDOR_Y + 1);
        Require(reviewGame.ui.hudMessage.text[0] == '\0',
                "returning from the ruins with the full archive but no review should now stay quiet and leave guidance to the objective panel");
    }

    {
        Game endingTransitionGame;

        PrepareGame(&endingTransitionGame);
        endingTransitionGame.runtime.tasks.ending = ENDING_SETTLEMENT;
        Game_BeginScreenTransition(&endingTransitionGame, SCREEN_TRANSITION_ENTER_ENDING, -1);
        Require(endingTransitionGame.flow.state == GAME_STATE_PLAYING,
                "arming the ending fade should not hard-cut into the ending state immediately");
        Require(endingTransitionGame.flow.screenTransitionActive,
                "arming the ending fade should mark the screen transition as active");
        Require(endingTransitionGame.flow.screenTransitionAction == SCREEN_TRANSITION_ENTER_ENDING,
                "ending entry should use the dedicated end-screen transition action");

        Game_EnterEndingState(&endingTransitionGame);
        Require(endingTransitionGame.flow.state == GAME_STATE_ENDING,
                "resolving the ending transition should switch the session into the ending state");
    }

    {
        Game heroicReturnGame;

        PrepareGame(&heroicReturnGame);
        heroicReturnGame.runtime.tasks.stage = 7;
        heroicReturnGame.runtime.tasks.oxygenRepairLevel = 2;
        heroicReturnGame.runtime.tasks.commRepairLevel = 1;
        heroicReturnGame.runtime.tasks.energyRepairLevel = 1;
        heroicReturnGame.runtime.tasks.crashClueFound = true;
        heroicReturnGame.runtime.tasks.amplifierUnlocked = true;
        heroicReturnGame.runtime.tasks.westW5Completed = true;
        heroicReturnGame.runtime.tasks.southS5Completed = true;
        heroicReturnGame.runtime.tasks.endingArchiveReviewed = true;
        heroicReturnGame.runtime.tasks.selectedEndingRoute = ENDING_HEROIC;
        heroicReturnGame.runtime.tasks.bossDefeated = true;
        CollectAllMainlineLogs(&heroicReturnGame.runtime.tasks);
        snprintf(heroicReturnGame.runtime.lastLocationName, sizeof(heroicReturnGame.runtime.lastLocationName), "%s", "Northwest Ruins");
        MovePlayerAndUpdate(&heroicReturnGame, SHIP_CORRIDOR_X + 1, SHIP_CORRIDOR_Y + 1);
        Require(heroicReturnGame.ui.hudMessage.text[0] == '\0',
                "returning from the northwest-ruins guardian fight after locking the heroic route should no longer auto-post a summary");
    }

    PrepareGame(&game);
    game.runtime.tasks.stage = 4;
    game.runtime.tasks.commRepairLevel = 1;
    MovePlayerAndUpdate(&game, EXTERIOR_X(24), EXTERIOR_Y(72));
    Require(game.runtime.tasks.westW1Started,
            "entering West Frontier with relay recovery complete should start W1");
    MovePlayerAndUpdate(&game, WORKBENCH_X, WORKBENCH_Y);
    Require(!game.runtime.tasks.westW1Completed,
            "returning to base from West Frontier without the record should keep W1 open");
    Require(game.ui.hudMessage.text[0] == '\0',
            "missing west evidence should now stay quiet instead of auto-posting a reminder");
    CollectArchiveEvidence(&game.runtime.tasks, 3);
    MovePlayerAndUpdate(&game, EXTERIOR_X(24), EXTERIOR_Y(72));
    MovePlayerAndUpdate(&game, WORKBENCH_X, WORKBENCH_Y);
    Require(game.runtime.tasks.westW1Completed,
            "returning to base from West Frontier with the record should complete W1");

    {
        Game archiveRoomGame;

        PrepareGame(&archiveRoomGame);
        archiveRoomGame.runtime.tasks.stage = 4;
        archiveRoomGame.runtime.tasks.commRepairLevel = 1;
        archiveRoomGame.runtime.tasks.westW1Started = true;
        archiveRoomGame.runtime.tasks.westW1Completed = false;
        CollectArchiveEvidence(&archiveRoomGame.runtime.tasks, 3);
        snprintf(archiveRoomGame.runtime.lastLocationName, sizeof(archiveRoomGame.runtime.lastLocationName), "%s", "West Frontier");
        MovePlayerAndUpdate(&archiveRoomGame, SHIP_CORRIDOR_X + 1, SHIP_CORRIDOR_Y + 1);
        Require(archiveRoomGame.runtime.tasks.westW1Completed,
            "returning through Central Corridor with the record should still count as a valid base archive hand-in");
    }

    {
        Game archiveRoomGame;

        PrepareGame(&archiveRoomGame);
        archiveRoomGame.runtime.tasks.stage = 4;
        archiveRoomGame.runtime.tasks.commRepairLevel = 1;
        archiveRoomGame.runtime.tasks.westW1Completed = true;
        archiveRoomGame.runtime.tasks.westW2Started = true;
        archiveRoomGame.runtime.tasks.westW2Completed = false;
        CollectArchiveEvidence(&archiveRoomGame.runtime.tasks, 4);
        snprintf(archiveRoomGame.runtime.lastLocationName, sizeof(archiveRoomGame.runtime.lastLocationName), "%s", "Survey Break");
        MovePlayerAndUpdate(&archiveRoomGame, EXTERIOR_X(24), EXTERIOR_Y(72));
        MovePlayerAndUpdate(&archiveRoomGame, SHIP_WORKSHOP_X + 1, SHIP_WORKSHOP_Y + 1);
        Require(archiveRoomGame.runtime.tasks.westW2Completed,
                "returning from Survey Break through West Frontier should still archive W2 and unlock Canopy Hollow");
    }

    {
        Game archiveRoomGame;

        PrepareGame(&archiveRoomGame);
        archiveRoomGame.runtime.tasks.stage = 5;
        archiveRoomGame.runtime.tasks.energyRepairLevel = 1;
        archiveRoomGame.runtime.tasks.southS1Started = true;
        archiveRoomGame.runtime.tasks.southS1Completed = false;
        CollectArchiveEvidence(&archiveRoomGame.runtime.tasks, 9);
        snprintf(archiveRoomGame.runtime.lastLocationName, sizeof(archiveRoomGame.runtime.lastLocationName), "%s", "South Collapse");
        MovePlayerAndUpdate(&archiveRoomGame, SHIP_DIAGNOSTICS_X + 1, SHIP_DIAGNOSTICS_Y + 1);
        Require(archiveRoomGame.runtime.tasks.southS1Completed,
            "returning through Diagnostics with the record should still count as a valid base archive hand-in");
    }

    {
        Game archiveRoomGame;

        PrepareGame(&archiveRoomGame);
        archiveRoomGame.runtime.tasks.stage = 5;
        archiveRoomGame.runtime.tasks.energyRepairLevel = 1;
        archiveRoomGame.runtime.tasks.southS1Started = true;
        archiveRoomGame.runtime.tasks.southS1Completed = false;
        CollectArchiveEvidence(&archiveRoomGame.runtime.tasks, 9);
        snprintf(archiveRoomGame.runtime.lastLocationName, sizeof(archiveRoomGame.runtime.lastLocationName), "%s", "South Collapse");
        MovePlayerAndUpdate(&archiveRoomGame, SHIP_CREW_QUARTERS_X + 1, SHIP_CREW_QUARTERS_Y + 1);
        Require(archiveRoomGame.runtime.tasks.southS1Completed,
            "returning through Crew Quarters with the record should still count as a valid base archive hand-in");
    }

    {
        Game archiveRoomGame;

        PrepareGame(&archiveRoomGame);
        archiveRoomGame.runtime.tasks.stage = 5;
        archiveRoomGame.runtime.tasks.energyRepairLevel = 1;
        archiveRoomGame.runtime.tasks.southS1Completed = true;
        archiveRoomGame.runtime.tasks.southS2Started = true;
        archiveRoomGame.runtime.tasks.southS2Completed = false;
        CollectArchiveEvidence(&archiveRoomGame.runtime.tasks, 10);
        snprintf(archiveRoomGame.runtime.lastLocationName, sizeof(archiveRoomGame.runtime.lastLocationName), "%s", "Vent Galleries");
        MovePlayerAndUpdate(&archiveRoomGame, EXTERIOR_X(82), EXTERIOR_Y(96));
        MovePlayerAndUpdate(&archiveRoomGame, SHIP_DIAGNOSTICS_X + 1, SHIP_DIAGNOSTICS_Y + 1);
        Require(archiveRoomGame.runtime.tasks.southS2Completed,
                "returning from Vent Galleries through South Collapse should still archive S2 and unlock Service Shafts");
    }

    MovePlayerAndUpdate(&game, EXTERIOR_X(35), EXTERIOR_Y(76));
    Require(game.runtime.tasks.westW2Started && !game.runtime.tasks.westW2Completed,
            "entering Survey Break after W1 should transition to W2");
    MovePlayerAndUpdate(&game, WORKBENCH_X, WORKBENCH_Y);
    Require(!game.runtime.tasks.westW2Completed,
            "returning to base from Survey Break without the record should keep W2 open");
    CollectArchiveEvidence(&game.runtime.tasks, 4);
    MovePlayerAndUpdate(&game, EXTERIOR_X(35), EXTERIOR_Y(76));
    MovePlayerAndUpdate(&game, WORKBENCH_X, WORKBENCH_Y);
    Require(game.runtime.tasks.westW2Completed,
            "returning to base from Survey Break with the record should archive W2");
    MovePlayerAndUpdate(&game, EXTERIOR_X(41), EXTERIOR_Y(67));
    Require(game.runtime.tasks.westW3Started && !game.runtime.tasks.westW3Completed,
            "entering Canopy Hollow after W2 should transition to W3");
    MovePlayerAndUpdate(&game, WORKBENCH_X, WORKBENCH_Y);
    Require(!game.runtime.tasks.westW3Completed,
            "returning to base from Canopy Hollow without the record should keep W3 open");
    CollectArchiveEvidence(&game.runtime.tasks, 5);
    MovePlayerAndUpdate(&game, EXTERIOR_X(41), EXTERIOR_Y(67));
    MovePlayerAndUpdate(&game, WORKBENCH_X, WORKBENCH_Y);
    Require(game.runtime.tasks.westW3Completed,
            "returning to base from Canopy Hollow with the record should archive W3");
    MovePlayerAndUpdate(&game, EXTERIOR_X(26), EXTERIOR_Y(90));
    Require(game.runtime.tasks.westW4Started && !game.runtime.tasks.westW4Completed,
            "entering Echo Basin after W3 should transition to W4");
    MovePlayerAndUpdate(&game, WORKBENCH_X, WORKBENCH_Y);
    Require(!game.runtime.tasks.westW4Completed,
            "returning to base from Echo Basin without the record should keep W4 open");
    CollectArchiveEvidence(&game.runtime.tasks, 6);
    MovePlayerAndUpdate(&game, EXTERIOR_X(26), EXTERIOR_Y(90));
    MovePlayerAndUpdate(&game, WORKBENCH_X, WORKBENCH_Y);
    Require(game.runtime.tasks.westW4Completed,
            "returning to base from Echo Basin with the record should archive W4");
    MovePlayerAndUpdate(&game, EXTERIOR_X(45), EXTERIOR_Y(84));
    Require(game.runtime.tasks.westW5Started && !game.runtime.tasks.westW5Completed,
            "entering Last Camp after W4 should transition to W5");
    MovePlayerAndUpdate(&game, WORKBENCH_X, WORKBENCH_Y);
    Require(!game.runtime.tasks.westW5Completed,
            "returning to base from Last Camp without the record should keep W5 open");
    CollectArchiveEvidence(&game.runtime.tasks, 7);
    MovePlayerAndUpdate(&game, EXTERIOR_X(45), EXTERIOR_Y(84));
    MovePlayerAndUpdate(&game, WORKBENCH_X, WORKBENCH_Y);
    Require(game.runtime.tasks.westW5Completed,
            "returning to base from Last Camp with the record should archive W5");

    PrepareGame(&game);
    game.runtime.tasks.stage = 5;
    game.runtime.tasks.energyRepairLevel = 1;
    MovePlayerAndUpdate(&game, EXTERIOR_X(82), EXTERIOR_Y(96));
    Require(game.runtime.tasks.southS1Started,
            "entering South Collapse with power restore complete should start S1");
    MovePlayerAndUpdate(&game, WORKBENCH_X, WORKBENCH_Y);
    Require(!game.runtime.tasks.southS1Completed,
            "returning to base from South Collapse without the record should keep S1 open");
    Require(game.ui.hudMessage.text[0] == '\0',
            "missing south evidence should now stay quiet instead of auto-posting a reminder");
    CollectArchiveEvidence(&game.runtime.tasks, 9);
    MovePlayerAndUpdate(&game, EXTERIOR_X(82), EXTERIOR_Y(96));
    MovePlayerAndUpdate(&game, WORKBENCH_X, WORKBENCH_Y);
    Require(game.runtime.tasks.southS1Completed,
            "returning to base from South Collapse with the record should complete S1");
    MovePlayerAndUpdate(&game, EXTERIOR_X(90), EXTERIOR_Y(96));
    Require(game.runtime.tasks.southS2Started && !game.runtime.tasks.southS2Completed,
            "entering Vent Galleries after S1 should transition to S2");
    MovePlayerAndUpdate(&game, WORKBENCH_X, WORKBENCH_Y);
    Require(!game.runtime.tasks.southS2Completed,
            "returning to base from Vent Galleries without the record should keep S2 open");
    CollectArchiveEvidence(&game.runtime.tasks, 10);
    MovePlayerAndUpdate(&game, EXTERIOR_X(90), EXTERIOR_Y(96));
    MovePlayerAndUpdate(&game, WORKBENCH_X, WORKBENCH_Y);
    Require(game.runtime.tasks.southS2Completed,
            "returning to base from Vent Galleries with the record should archive S2");
    MovePlayerAndUpdate(&game, EXTERIOR_X(104), EXTERIOR_Y(96));
    Require(game.runtime.tasks.southS3Started && !game.runtime.tasks.southS3Completed,
            "entering Service Shafts after S2 should transition to S3");
    MovePlayerAndUpdate(&game, WORKBENCH_X, WORKBENCH_Y);
    Require(!game.runtime.tasks.southS3Completed,
            "returning to base from Service Shafts without the record should keep S3 open");
    CollectArchiveEvidence(&game.runtime.tasks, 11);
    MovePlayerAndUpdate(&game, EXTERIOR_X(104), EXTERIOR_Y(96));
    MovePlayerAndUpdate(&game, WORKBENCH_X, WORKBENCH_Y);
    Require(game.runtime.tasks.southS3Completed,
            "returning to base from Service Shafts with the record should archive S3");
    MovePlayerAndUpdate(&game, EXTERIOR_X(112), EXTERIOR_Y(96));
    Require(game.runtime.tasks.southS4Started && !game.runtime.tasks.southS4Completed,
            "entering Purifier Ring after S3 should transition to S4");
    MovePlayerAndUpdate(&game, WORKBENCH_X, WORKBENCH_Y);
    Require(!game.runtime.tasks.southS4Completed,
            "returning to base from Purifier Ring without the record should keep S4 open");
    CollectArchiveEvidence(&game.runtime.tasks, 12);
    MovePlayerAndUpdate(&game, EXTERIOR_X(112), EXTERIOR_Y(96));
    MovePlayerAndUpdate(&game, WORKBENCH_X, WORKBENCH_Y);
    Require(game.runtime.tasks.southS4Completed,
            "returning to base from Purifier Ring with the record should archive S4");
    MovePlayerAndUpdate(&game, EXTERIOR_X(120), EXTERIOR_Y(96));
    Require(game.runtime.tasks.southS5Started && !game.runtime.tasks.southS5Completed,
            "entering Root Vault after S4 should transition to S5");
    MovePlayerAndUpdate(&game, WORKBENCH_X, WORKBENCH_Y);
    Require(!game.runtime.tasks.southS5Completed,
            "returning to base from Root Vault without the record should keep S5 open");
    CollectArchiveEvidence(&game.runtime.tasks, 13);
    MovePlayerAndUpdate(&game, EXTERIOR_X(120), EXTERIOR_Y(96));
    MovePlayerAndUpdate(&game, WORKBENCH_X, WORKBENCH_Y);
    Require(game.runtime.tasks.southS5Completed,
            "returning to base from Root Vault with the record should archive S5");

    PrepareGame(&game);
    game.runtime.tasks.stage = 5;
    game.runtime.player.resources[RESOURCE_ENERGY_CORE] = 1;
    snprintf(game.runtime.lastLocationName, sizeof(game.runtime.lastLocationName), "%s", "Deep Basin");
    game.runtime.player.gridX = SHIP_CORRIDOR_X + 1;
    game.runtime.player.gridY = SHIP_CORRIDOR_Y + 1;
    Player_UpdateWorldPosition(&game.runtime.player);
    Game_MaybePostNorthRouteTransitionHint(&game);
    Require(game.ui.hudMessage.text[0] == '\0'
                || strstr(game.ui.hudMessage.text, "east qualification run is complete") != NULL
                || strstr(game.ui.hudMessage.text, "Energy Core returned") != NULL,
            "returning from the deep east route with the Energy Core should keep base-return summary behavior stable");

    PrepareGame(&game);
    game.runtime.tasks.stage = 6;
    game.runtime.player.resources[RESOURCE_RELIC_FRAGMENT] = 3;
    snprintf(game.runtime.lastLocationName, sizeof(game.runtime.lastLocationName), "%s", "Monolith Ring");
    game.runtime.player.gridX = SHIP_TERMINAL_BAY_X + 1;
    game.runtime.player.gridY = SHIP_TERMINAL_BAY_Y + 1;
    Player_UpdateWorldPosition(&game.runtime.player);
    Game_MaybePostNorthRouteTransitionHint(&game);
    Require(game.ui.hudMessage.text[0] == '\0'
                || strstr(game.ui.hudMessage.text, "fragments") != NULL
                || strstr(game.ui.hudMessage.text, "Relic set ready") != NULL
                || strstr(game.ui.hudMessage.text, "fragment set complete") != NULL,
            "returning from the ruins with fragment set should keep base-return summary behavior stable");

    {
        GameMap dynamicMap = {0};
        MiniMap dynamicMiniMap = {0};
        Player dynamicPlayer;

        Require(Map_LoadTiled(&dynamicMap, "tests/fixtures/dynamic_map.tmj"),
                "session systems should accept a runtime-sized Tiled fixture");
        MiniMap_Init(&dynamicMiniMap, &dynamicMap);
        Require(dynamicMiniMap.width == dynamicMap.width
                    && dynamicMiniMap.height == dynamicMap.height
                    && dynamicMiniMap.explored != NULL,
                "minimap storage should follow runtime map dimensions");
        Player_Init(&dynamicPlayer);
        dynamicPlayer.gridX = 3;
        dynamicPlayer.gridY = 2;
        MiniMap_Update(&dynamicMiniMap, &dynamicPlayer, &dynamicMap);
        Require(dynamicMiniMap.explored[2][3],
                "runtime-sized minimap should reveal the final valid dynamic tile");
        MiniMap_Destroy(&dynamicMiniMap);
        Map_Destroy(&dynamicMap);
    }


    {
        int trip;
        int inventoryBefore;
        float healthBefore;
        float oxygenBefore;
        int stageBefore;

        memset(&game, 0, sizeof(game));
        Game_ResetGameplayWorld(&game);
        Game_RefreshSaveSlots(&game);
        game.flow.state = GAME_STATE_PLAYING;
        game.runtime.tasks.stage = 0;
        game.runtime.player.resources[RESOURCE_ORE] = 7;
        game.runtime.player.health = 73.0f;
        game.runtime.player.oxygen = 61.0f;
        inventoryBefore = game.runtime.player.resources[RESOURCE_ORE];
        healthBefore = game.runtime.player.health;
        oxygenBefore = game.runtime.player.oxygen;
        stageBefore = game.runtime.tasks.stage;
        Require(strcmp(game.runtime.map.mapId, "ship_interior") == 0,
                "new gameplay session should begin on the split ship interior map");

        for (trip = 0; trip < 20; trip++) {
            const MapPortal *portal = Map_GetPortalById(&game.runtime.map, "SHIP_AIRLOCK_EXIT");
            Require(portal != NULL && Game_BeginPortalTransition(&game, portal),
                    "ship portal should begin an independent map transition without a hidden stage gate");
            Require(!Game_BeginPortalTransition(&game, portal),
                    "active transition should reject duplicate portal triggers");
            Require(Game_ResolveMapTransition(&game), "ship-to-surface transition should resolve");
            Require(strcmp(game.runtime.map.mapId, "planet_surface_01") == 0
                        && game.runtime.player.gridX == 83 && game.runtime.player.gridY == 52,
                    "surface transition should place player beside the exterior ship");
            game.flow.screenTransitionActive = false;
            game.flow.screenTransitionAction = SCREEN_TRANSITION_NONE;
            Game_FinishMapTransition(&game);

            portal = Map_GetPortalById(&game.runtime.map, "SHIP_EXTERIOR_ENTRY");
            Require(portal != NULL && Game_BeginPortalTransition(&game, portal),
                    "exterior ship portal should begin return transition");
            Require(Game_ResolveMapTransition(&game), "surface-to-ship transition should resolve");
            Require(strcmp(game.runtime.map.mapId, "ship_interior") == 0
                        && game.runtime.player.gridX == 76 && game.runtime.player.gridY == 52,
                    "return transition should place player near the interior airlock");
            game.flow.screenTransitionActive = false;
            game.flow.screenTransitionAction = SCREEN_TRANSITION_NONE;
            Game_FinishMapTransition(&game);
        }
        Require(game.runtime.player.resources[RESOURCE_ORE] == inventoryBefore
                    && game.runtime.player.health == healthBefore
                    && game.runtime.player.oxygen == oxygenBefore
                    && game.runtime.tasks.stage == stageBefore,
                "20 round trips must preserve inventory, health, oxygen, and task stage");
        Require(game.runtime.tasks.registeredMapCount == 2,
                "round trips must not repeat Tasks_Init or duplicate map entity registration");
    }

    return 0;
}
