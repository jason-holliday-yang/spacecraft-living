#include "game_session_internal.h"

#include "localization.h"

#include <cstdio>
#include <cstring>

static int FindFirstOccupiedSlot(const Game *game) {
    int slotIndex;

    for (slotIndex = 0; slotIndex < SAVE_SLOT_COUNT; slotIndex++) {
        if (game->account.saveSlots[slotIndex].exists) {
            return slotIndex;
        }
    }

    return 0;
}

static int FindFirstEmptySlot(const Game *game) {
    int slotIndex;

    for (slotIndex = 0; slotIndex < SAVE_SLOT_COUNT; slotIndex++) {
        if (!game->account.saveSlots[slotIndex].exists) {
            return slotIndex;
        }
    }

    return 0;
}

static bool IsValidSelectedSaveSlot(const Game *game) {
    return game != NULL
        && game->ui.selectedSaveSlot >= 0
        && game->ui.selectedSaveSlot < SAVE_SLOT_COUNT;
}

static void RefreshAuthState(Game *game) {
    if (game == NULL) {
        return;
    }

    game->account.authenticated = SaveSystem_IsAccountAuthenticated();
    game->account.authHasAccounts = SaveSystem_HasRegisteredAccounts();
}

static bool SnapshotShouldPublishBestScore(const SaveSnapshot *snapshot) {
    return snapshot != NULL && (snapshot->deathCount > 0 || snapshot->ending != ENDING_NONE);
}

static int CalculateSnapshotEndingScore(const SaveSnapshot *snapshot) {
    GameMap map{};
    TaskSystem tasks;
    Player player;
    int index;

    if (!SnapshotShouldPublishBestScore(snapshot)) {
        return 0;
    }

    Map_Init(&map);
    Player_Init(&player);
    Tasks_Init(&tasks, &map);

    player.health = snapshot->health;
    player.oxygen = snapshot->oxygen;
    player.deathCount = snapshot->deathCount;
    player.hasSignalAmplifier = snapshot->hasSignalAmplifier;
    for (index = 0; index < RESOURCE_COUNT; index++) {
        player.resources[index] = snapshot->resources[index];
    }

    tasks.stage = snapshot->stage;
    tasks.oxygenRepairLevel = snapshot->oxygenRepairLevel;
    tasks.commRepairLevel = snapshot->commRepairLevel;
    tasks.energyRepairLevel = snapshot->energyRepairLevel;
    tasks.crashClueFound = snapshot->crashClueFound;
    tasks.amplifierUnlocked = snapshot->amplifierUnlocked;
    tasks.signalAmplifierCrafted = snapshot->hasSignalAmplifier;
    tasks.bossDefeated = snapshot->bossDefeated;
    tasks.signalTowerActivated = snapshot->signalTowerActivated;
    tasks.selectedEndingRoute = (GameEnding)snapshot->selectedEndingRoute;
    tasks.endingArchiveReviewed = snapshot->endingArchiveReviewed;
    tasks.westW1Started = snapshot->westW1Started;
    tasks.westW1Completed = snapshot->westW1Completed;
    tasks.westW2Started = snapshot->westW2Started;
    tasks.westW2Completed = snapshot->westW2Completed;
    tasks.westW3Started = snapshot->westW3Started;
    tasks.westW3Completed = snapshot->westW3Completed;
    tasks.westW4Started = snapshot->westW4Started;
    tasks.westW4Completed = snapshot->westW4Completed;
    tasks.westW5Started = snapshot->westW5Started;
    tasks.westW5Completed = snapshot->westW5Completed;
    tasks.southS1Started = snapshot->southS1Started;
    tasks.southS1Completed = snapshot->southS1Completed;
    tasks.southS2Started = snapshot->southS2Started;
    tasks.southS2Completed = snapshot->southS2Completed;
    tasks.southS3Started = snapshot->southS3Started;
    tasks.southS3Completed = snapshot->southS3Completed;
    tasks.southS4Started = snapshot->southS4Started;
    tasks.southS4Completed = snapshot->southS4Completed;
    tasks.southS5Started = snapshot->southS5Started;
    tasks.southS5Completed = snapshot->southS5Completed;
    tasks.monolithActivated[0] = snapshot->monolithActivated[0];
    tasks.monolithActivated[1] = snapshot->monolithActivated[1];
    tasks.monolithActivated[2] = snapshot->monolithActivated[2];
    tasks.monolithsLit = snapshot->monolithsLit;
    for (index = 0; index < STORY_MAIN_SCENE_COUNT; index++) {
        tasks.shownMainStorySceneCount += snapshot->storyMainSceneShown[index] ? 1 : 0;
    }
    tasks.ending = (GameEnding)snapshot->ending;

    for (index = 0; index < tasks.logCount && index < MAX_LOGS; index++) {
        tasks.logs[index].collected = snapshot->logs[index].collected;
    }

    const int score = Tasks_CalculateEndingScore(&tasks, &player);
    Map_Destroy(&map);
    return score;
}

static void RefreshAccountBestScore(Game *game) {
    int bestScore;

    if (game == NULL) {
        return;
    }

    game->account.hasAccountBestScore = false;
    game->account.accountBestScore = 0;
    if (SaveSystem_GetActiveAccountBestScore(&bestScore)) {
        game->account.hasAccountBestScore = true;
        game->account.accountBestScore = bestScore;
    }
}

static void BackfillAccountBestScoreFromSaveSlots(Game *game) {
    int slotIndex;
    int bestScore;

    if (game == NULL || !game->account.authenticated || game->account.hasAccountBestScore) {
        return;
    }

    bestScore = 0;
    for (slotIndex = 0; slotIndex < SAVE_SLOT_COUNT; slotIndex++) {
        SaveSnapshot snapshot;
        int snapshotScore;

        if (!game->account.saveSlots[slotIndex].exists || !SaveSystem_LoadGame(slotIndex, &snapshot)) {
            continue;
        }

        snapshotScore = CalculateSnapshotEndingScore(&snapshot);
        if (snapshotScore > bestScore) {
            bestScore = snapshotScore;
        }
    }

    if (bestScore <= 0 || !SaveSystem_UpdateActiveAccountBestScore(bestScore)) {
        return;
    }

    game->account.hasAccountBestScore = true;
    game->account.accountBestScore = bestScore;
}

void Game_TrySaveSettings(Game *game) {
    if (game == NULL || !game->ui.settingsDirty) {
        return;
    }

    if (SaveSystem_SaveSettings(&game->services.settings)) {
        game->ui.settingsDirty = false;
    }
}

void Game_RefreshSaveSlots(Game *game) {
    int slotIndex;

    if (game == NULL) {
        return;
    }

    RefreshAuthState(game);
    RefreshAccountBestScore(game);
    SaveSystem_ListSlots(game->account.saveSlots, SAVE_SLOT_COUNT);
    game->account.saveSlotCount = 0;
    for (slotIndex = 0; slotIndex < SAVE_SLOT_COUNT; slotIndex++) {
        if (game->account.saveSlots[slotIndex].exists) {
            game->account.saveSlotCount += 1;
        }
    }
    game->account.hasSaveFile = game->account.saveSlotCount > 0;
    if (game->account.hasSaveFile) {
        BackfillAccountBestScoreFromSaveSlots(game);
    }
    if (game->ui.selectedSaveSlot < 0) {
        game->ui.selectedSaveSlot = 0;
    }
    if (game->ui.selectedSaveSlot >= SAVE_SLOT_COUNT) {
        game->ui.selectedSaveSlot = SAVE_SLOT_COUNT - 1;
    }
}

void Game_RecordActiveAccountScore(Game *game) {
    int score;

    if (game == NULL || !game->account.authenticated) {
        return;
    }

    score = Tasks_CalculateEndingScore(&game->runtime.tasks, &game->runtime.player);
    if (score > 0) {
        SaveSystem_UpdateActiveAccountBestScore(score);
    }
    RefreshAccountBestScore(game);
}

void Game_CloseSavePanel(Game *game) {
    game->ui.savePanelOpen = false;
}

void Game_OpenSavePanel(Game *game, SavePanelMode mode) {
    RefreshAuthState(game);
    if (game == NULL || !game->account.authenticated) {
        return;
    }

    Game_RefreshSaveSlots(game);
    game->ui.savePanelMode = mode;
    game->ui.savePanelOpen = true;
    if (mode == SAVE_PANEL_MODE_LOAD) {
        game->ui.selectedSaveSlot = FindFirstOccupiedSlot(game);
    } else {
        game->ui.selectedSaveSlot = FindFirstEmptySlot(game);
    }
}

bool Game_SaveCurrentGame(Game *game, int slotIndex) {
    SaveSnapshot snapshot;
    bool saved;

    RefreshAuthState(game);
    if (game == NULL || !game->account.authenticated || slotIndex < 0 || slotIndex >= SAVE_SLOT_COUNT) {
        return false;
    }

    Game_BuildSaveSnapshot(game, &snapshot);
    saved = SaveSystem_SaveGame(slotIndex, &snapshot);
    Game_RefreshSaveSlots(game);

    if (saved) {
        char message[128];

        std::snprintf(message, sizeof(message), "%s in Slot %02d", LOC_UI_SAVE_SUCCESS, slotIndex + 1);
        Game_PostMessage(game, message, 3.0f);
        Audio_PlayCue(&game->services.audio, AUDIO_CUE_CONFIRM);
    } else {
        Game_PostMessage(game, LOC_UI_SAVE_FAILED, 3.0f);
    }

    return saved;
}

bool Game_LoadSavedGame(Game *game, int slotIndex) {
    SaveSnapshot snapshot;
    bool positionAdjusted;

    RefreshAuthState(game);
    if (game == NULL || !game->account.authenticated || slotIndex < 0 || slotIndex >= SAVE_SLOT_COUNT) {
        return false;
    }

    if (!SaveSystem_LoadGame(slotIndex, &snapshot)) {
        Game_RefreshSaveSlots(game);
        return false;
    }

    Game_TrySaveSettings(game);
    positionAdjusted = Game_LoadSnapshotIntoSession(game, &snapshot);
    Game_RefreshSaveSlots(game);
    Game_PostMessage(game,
                     positionAdjusted
                        ? Loc_PickLiteral("Saved progress loaded. Position adjusted to a safe tile.", "已读取存档进度，并将位置调整到安全地块。")
                        : Loc_PickLiteral("Saved progress loaded.", "已读取存档进度。"),
                     3.2f);
    return true;
}

void Game_MoveSavePanelSelection(Game *game, int delta) {
    int nextSlot;

    if (game == NULL) {
        return;
    }

    if (!IsValidSelectedSaveSlot(game)) {
        game->ui.selectedSaveSlot = 0;
    }

    nextSlot = game->ui.selectedSaveSlot + delta;
    if (nextSlot < 0 || nextSlot >= SAVE_SLOT_COUNT) {
        return;
    }

    game->ui.selectedSaveSlot = nextSlot;
    Audio_PlayCue(&game->services.audio, AUDIO_CUE_OPEN);
}

void Game_ActivateSelectedSaveSlot(Game *game) {
    SaveSlotInfo *slot;

    if (!IsValidSelectedSaveSlot(game)) {
        Game_PostMessage(game, LOC_UI_SAVE_MISSING, 2.5f);
        return;
    }

    slot = &game->account.saveSlots[game->ui.selectedSaveSlot];
    if (game->ui.savePanelMode == SAVE_PANEL_MODE_LOAD) {
        if (!slot->exists) {
            Game_PostMessage(game, LOC_UI_SAVE_MISSING, 2.5f);
            return;
        }

        Audio_PlayCue(&game->services.audio, AUDIO_CUE_CONFIRM);
        if (!Game_LoadSavedGame(game, game->ui.selectedSaveSlot)) {
            Game_PostMessage(game, LOC_UI_SAVE_MISSING, 3.0f);
        }
        return;
    }

    if (slot->exists) {
        Game_PostMessage(game, Loc_PickLiteral("This slot is occupied. Delete it first, or choose an empty slot.", "该槽位已被占用，请先删除存档或选择空槽位。"), 3.2f);
        return;
    }

    Audio_PlayCue(&game->services.audio, AUDIO_CUE_CONFIRM);
    if (Game_SaveCurrentGame(game, game->ui.selectedSaveSlot)) {
        Game_CloseSavePanel(game);
    }
}

void Game_DeleteSelectedSaveSlot(Game *game) {
    SaveSlotInfo *slot;

    if (!IsValidSelectedSaveSlot(game)) {
        Game_PostMessage(game, LOC_UI_SAVE_MISSING, 2.4f);
        return;
    }

    slot = &game->account.saveSlots[game->ui.selectedSaveSlot];
    if (!slot->exists) {
        Game_PostMessage(game, LOC_UI_SAVE_MISSING, 2.4f);
        return;
    }

    if (SaveSystem_DeleteGame(game->ui.selectedSaveSlot)) {
        Game_RefreshSaveSlots(game);
        Game_PostMessage(game, LOC_UI_SAVE_DELETED, 2.8f);
        Audio_PlayCue(&game->services.audio, AUDIO_CUE_CONFIRM);
        if (game->ui.savePanelMode == SAVE_PANEL_MODE_LOAD && game->account.hasSaveFile) {
            game->ui.selectedSaveSlot = FindFirstOccupiedSlot(game);
        } else if (game->ui.savePanelMode == SAVE_PANEL_MODE_SAVE) {
            game->ui.selectedSaveSlot = FindFirstEmptySlot(game);
        }
    } else {
        Game_PostMessage(game, LOC_UI_SAVE_DELETE_FAILED, 2.8f);
    }
}
