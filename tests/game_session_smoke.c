#include "game_manager.h"
#include "../src/game_manager_internal.h"
#include "../src/game_play_internal.h"

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

    ConfigureIsolatedSaveHome();
    Require(SaveSystem_Register("session", "pass1234", authMessage, sizeof(authMessage)),
            "session smoke should authenticate a local account before save flow checks");

    memset(&game, 0, sizeof(game));
    Game_StartNewGame(&game);
    Require(game.state == GAME_STATE_OPENING, "start new game should enter the opening cutscene state");
    Require(game.hudMessage.text[0] == '\0', "opening cutscene should not post the gameplay hint early");
    Require(game.tasks.ending == ENDING_NONE, "start new game should begin without an ending state");
    Require(game.tasks.objective[0] != '\0', "start new game should initialize the opening objective text");
    Require(Map_IsWalkable(&game.map, game.player.gridX, game.player.gridY), "start new game should place the player on a walkable tile");
    Require(game.camera.zoom > 0.0f, "start new game should initialize a positive camera zoom");
    Require(game.camera.target.x == game.player.worldPos.x && game.camera.target.y == game.player.worldPos.y,
            "start new game should center the camera on the player");
    Require(!game.pauseMenuOpen && !game.settingsOpen && !game.backpackOpen && !game.craftOpen
                && !game.mapOpen && !game.communicatorOpen && !game.helpOpen && !game.logReaderOpen
                && !game.savePanelOpen && !game.showDeathPopup,
            "start new game should clear all overlays and popups");

    Game_CompleteOpeningCutscene(&game);
    Require(game.state == GAME_STATE_PLAYING, "completing the opening cutscene should enter playing state");
    Require(strstr(game.hudMessage.text, "restore the oxygen console") != NULL,
            "entering gameplay should post the opening objective hint");

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

    Game_OpenSavePanel(&game, SAVE_PANEL_MODE_SAVE);
    Require(game.savePanelOpen, "opening the save panel should mark it visible");
    Require(game.savePanelMode == SAVE_PANEL_MODE_SAVE, "save panel should remember the requested mode");

    Game_CloseSavePanel(&game);
    Require(!game.savePanelOpen, "closing the save panel should hide it");

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

    Game_OpenSavePanel(&game, SAVE_PANEL_MODE_SAVE);
    Require(game.selectedSaveSlot == 0, "first save should target the first empty slot");
    Game_ActivateSelectedSaveSlot(&game);
    Require(game.hasSaveFile, "saving should refresh save-slot availability");
    Require(!game.savePanelOpen, "successful save should close the panel");
    Require(game.saveSlots[0].exists, "saved slot should be listed after saving");
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
    Require(game.tasks.ending == ENDING_NONE, "first death should not immediately fail the run");

    Game_HandleDeathRecovery(&game);
    Require(!game.showDeathPopup, "recovery should close the popup");
    Require(game.state == GAME_STATE_PLAYING, "recovery should return to playing state");
    Require(game.tasks.stage == 5, "recovery should preserve stage progress");
    Require(game.player.resources[RESOURCE_FRUIT] == 4, "recovery should preserve collected resources");
    Require(game.player.resources[RESOURCE_ENERGY_CORE] == 1, "recovery should preserve crafted progression resources");
    Require(game.player.gridX == game.map.campX && game.player.gridY == game.map.campY, "recovery should prefer the placed field camp");
    Require(game.player.health > 28.0f, "recovery should restore health in the new survival model");
    Require(game.player.stamina > 0.0f, "recovery should restore some stamina");
    Require(game.player.oxygen > 0.0f, "recovery should restore oxygen");
    Require(game.player.pressure == INITIAL_PRESSURE, "recovery should normalize retired pressure state");
    Require(game.player.poison < 50.0f, "recovery should reduce poison");
    Require(game.camera.zoom > 0.0f
                && game.camera.target.x == game.player.worldPos.x
                && game.camera.target.y == game.player.worldPos.y,
            "recovery should rebuild a sane camera state");
    Require(strstr(game.hudMessage.text, "field camp") != NULL, "recovery should explain when the field camp is used");

    Game_HandlePlayerDeath(&game);
    Require(game.showDeathPopup, "second death should still allow recovery");
    Require(game.player.deathCount == 2, "second death should increment death count");

    Game_HandleDeathRecovery(&game);
    Require(strstr(game.hudMessage.text, "one more collapse") != NULL, "second recovery should warn about the final remaining attempt");

    game.map.campPlaced = false;
    game.player.gridX = PLAYER_START_X + 10;
    game.player.gridY = PLAYER_START_Y + 5;
    Player_UpdateWorldPosition(&game.player);
    game.player.oxygen = 0.0f;
    Game_HandlePlayerDeath(&game);
    Require(!game.showDeathPopup, "third death should skip the recovery popup");
    Require(game.player.deathCount == 3, "third death should increment death count");
    Require(game.tasks.ending == ENDING_FAILURE, "third death should trigger the failure ending");
    Require(game.state == GAME_STATE_ENDING, "third death should transition to ending state");

    PrepareGame(&game);
    game.tasks.stage = 4;
    game.player.gridX = PLAYER_START_X + 9;
    game.player.gridY = PLAYER_START_Y + 3;
    game.player.pressure = 45.0f;
    game.player.oxygen = 0.0f;
    Player_UpdateWorldPosition(&game.player);

    Game_HandlePlayerDeath(&game);
    Require(game.showDeathPopup, "base fallback run should still open the recovery popup");
    Game_HandleDeathRecovery(&game);
    Require(game.player.gridX == PLAYER_START_X && game.player.gridY == PLAYER_START_Y, "recovery should fall back to base when no field camp is placed");
    Require(strstr(game.hudMessage.text, "returned you to base") != NULL, "base fallback should explain the recovery destination");

    PrepareGame(&game);
    game.tasks.stage = 7;
    game.player.gridX = SIGNAL_TOWER_X;
    game.player.gridY = SIGNAL_TOWER_Y + 6;
    Player_UpdateWorldPosition(&game.player);
    Game_MaybePostNorthRouteTransitionHint(&game);
    Require(strstr(game.hudMessage.text, "final climb") != NULL,
            "entering the tower plateau without prep should post a commitment warning");

    PrepareGame(&game);
    game.tasks.stage = 7;
    game.player.hasSignalAmplifier = true;
    game.player.gridX = SIGNAL_TOWER_X;
    game.player.gridY = SIGNAL_TOWER_Y + 6;
    Player_UpdateWorldPosition(&game.player);
    Game_MaybePostNorthRouteTransitionHint(&game);
    Require(strstr(game.hudMessage.text, "peaceful route is live") != NULL,
            "entering the tower plateau with the amplifier should post the peaceful-route commitment hint");

    PrepareGame(&game);
    game.tasks.stage = 4;
    game.tasks.commRepairLevel = 1;
    MovePlayerAndUpdate(&game, EXTERIOR_X(24), EXTERIOR_Y(72));
    Require(game.tasks.westW1Started,
            "entering West Frontier with relay recovery complete should start W1");
    MovePlayerAndUpdate(&game, WORKBENCH_X, WORKBENCH_Y);
    Require(!game.tasks.westW1Completed,
            "returning to base from West Frontier without the record should keep W1 open");
    Require(strstr(game.hudMessage.text, "West Frontier record") != NULL,
            "missing west evidence should explain which record still needs to be recovered");
    CollectArchiveEvidence(&game.tasks, 3);
    MovePlayerAndUpdate(&game, EXTERIOR_X(24), EXTERIOR_Y(72));
    MovePlayerAndUpdate(&game, WORKBENCH_X, WORKBENCH_Y);
    Require(game.tasks.westW1Completed,
            "returning to base from West Frontier with the record should complete W1");
    MovePlayerAndUpdate(&game, EXTERIOR_X(34), EXTERIOR_Y(68));
    Require(game.tasks.westW2Started && !game.tasks.westW2Completed,
            "entering Survey Break after W1 should transition to W2");
    MovePlayerAndUpdate(&game, WORKBENCH_X, WORKBENCH_Y);
    Require(!game.tasks.westW2Completed,
            "returning to base from Survey Break without the record should keep W2 open");
    CollectArchiveEvidence(&game.tasks, 4);
    MovePlayerAndUpdate(&game, EXTERIOR_X(34), EXTERIOR_Y(68));
    MovePlayerAndUpdate(&game, WORKBENCH_X, WORKBENCH_Y);
    Require(game.tasks.westW2Completed,
            "returning to base from Survey Break with the record should archive W2");
    MovePlayerAndUpdate(&game, EXTERIOR_X(44), EXTERIOR_Y(68));
    Require(game.tasks.westW3Started && !game.tasks.westW3Completed,
            "entering Canopy Hollow after W2 should transition to W3");
    MovePlayerAndUpdate(&game, WORKBENCH_X, WORKBENCH_Y);
    Require(!game.tasks.westW3Completed,
            "returning to base from Canopy Hollow without the record should keep W3 open");
    CollectArchiveEvidence(&game.tasks, 5);
    MovePlayerAndUpdate(&game, EXTERIOR_X(44), EXTERIOR_Y(68));
    MovePlayerAndUpdate(&game, WORKBENCH_X, WORKBENCH_Y);
    Require(game.tasks.westW3Completed,
            "returning to base from Canopy Hollow with the record should archive W3");
    MovePlayerAndUpdate(&game, EXTERIOR_X(48), EXTERIOR_Y(78));
    Require(game.tasks.westW4Started && !game.tasks.westW4Completed,
            "entering Echo Basin after W3 should transition to W4");
    MovePlayerAndUpdate(&game, WORKBENCH_X, WORKBENCH_Y);
    Require(!game.tasks.westW4Completed,
            "returning to base from Echo Basin without the record should keep W4 open");
    CollectArchiveEvidence(&game.tasks, 6);
    MovePlayerAndUpdate(&game, EXTERIOR_X(48), EXTERIOR_Y(78));
    MovePlayerAndUpdate(&game, WORKBENCH_X, WORKBENCH_Y);
    Require(game.tasks.westW4Completed,
            "returning to base from Echo Basin with the record should archive W4");
    MovePlayerAndUpdate(&game, EXTERIOR_X(48), EXTERIOR_Y(88));
    Require(game.tasks.westW5Started && !game.tasks.westW5Completed,
            "entering Last Camp after W4 should transition to W5");
    MovePlayerAndUpdate(&game, WORKBENCH_X, WORKBENCH_Y);
    Require(!game.tasks.westW5Completed,
            "returning to base from Last Camp without the record should keep W5 open");
    CollectArchiveEvidence(&game.tasks, 7);
    MovePlayerAndUpdate(&game, EXTERIOR_X(48), EXTERIOR_Y(88));
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
    Require(strstr(game.hudMessage.text, "South Collapse record") != NULL,
            "missing south evidence should explain which record still needs to be recovered");
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
