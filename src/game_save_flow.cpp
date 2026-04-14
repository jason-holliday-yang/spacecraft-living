#include "game_session_internal.h"

#include "localization.h"

#include <cstdio>

static int FindFirstOccupiedSlot(const Game *game) {
    int slotIndex;

    for (slotIndex = 0; slotIndex < SAVE_SLOT_COUNT; slotIndex++) {
        if (game->saveSlots[slotIndex].exists) {
            return slotIndex;
        }
    }

    return 0;
}

static int FindFirstEmptySlot(const Game *game) {
    int slotIndex;

    for (slotIndex = 0; slotIndex < SAVE_SLOT_COUNT; slotIndex++) {
        if (!game->saveSlots[slotIndex].exists) {
            return slotIndex;
        }
    }

    return 0;
}

static bool IsValidSelectedSaveSlot(const Game *game) {
    return game != NULL
        && game->selectedSaveSlot >= 0
        && game->selectedSaveSlot < SAVE_SLOT_COUNT;
}

static void RefreshAuthState(Game *game) {
    if (game == NULL) {
        return;
    }

    game->authenticated = SaveSystem_IsAccountAuthenticated();
    game->authHasAccounts = SaveSystem_HasRegisteredAccounts();
}

void Game_TrySaveSettings(Game *game) {
    if (game == NULL || !game->settingsDirty) {
        return;
    }

    if (SaveSystem_SaveSettings(&game->settings)) {
        game->settingsDirty = false;
    }
}

void Game_RefreshSaveSlots(Game *game) {
    int slotIndex;

    if (game == NULL) {
        return;
    }

    RefreshAuthState(game);
    SaveSystem_ListSlots(game->saveSlots, SAVE_SLOT_COUNT);
    game->saveSlotCount = 0;
    for (slotIndex = 0; slotIndex < SAVE_SLOT_COUNT; slotIndex++) {
        if (game->saveSlots[slotIndex].exists) {
            game->saveSlotCount += 1;
        }
    }
    game->hasSaveFile = game->saveSlotCount > 0;
    if (game->selectedSaveSlot < 0) {
        game->selectedSaveSlot = 0;
    }
    if (game->selectedSaveSlot >= SAVE_SLOT_COUNT) {
        game->selectedSaveSlot = SAVE_SLOT_COUNT - 1;
    }
}

void Game_CloseSavePanel(Game *game) {
    game->savePanelOpen = false;
}

void Game_OpenSavePanel(Game *game, SavePanelMode mode) {
    RefreshAuthState(game);
    if (game == NULL || !game->authenticated) {
        return;
    }

    Game_RefreshSaveSlots(game);
    game->savePanelMode = mode;
    game->savePanelOpen = true;
    if (mode == SAVE_PANEL_MODE_LOAD) {
        game->selectedSaveSlot = FindFirstOccupiedSlot(game);
    } else {
        game->selectedSaveSlot = FindFirstEmptySlot(game);
    }
}

bool Game_SaveCurrentGame(Game *game, int slotIndex) {
    SaveSnapshot snapshot;
    bool saved;

    RefreshAuthState(game);
    if (game == NULL || !game->authenticated || slotIndex < 0 || slotIndex >= SAVE_SLOT_COUNT) {
        return false;
    }

    Game_BuildSaveSnapshot(game, &snapshot);
    saved = SaveSystem_SaveGame(slotIndex, &snapshot);
    Game_RefreshSaveSlots(game);

    if (saved) {
        char message[128];

        std::snprintf(message, sizeof(message), "%s in Slot %02d", LOC_UI_SAVE_SUCCESS, slotIndex + 1);
        Game_PostMessage(game, message, 3.0f);
        Audio_PlayCue(&game->audio, AUDIO_CUE_CONFIRM);
    } else {
        Game_PostMessage(game, LOC_UI_SAVE_FAILED, 3.0f);
    }

    return saved;
}

bool Game_LoadSavedGame(Game *game, int slotIndex) {
    SaveSnapshot snapshot;
    bool positionAdjusted;

    RefreshAuthState(game);
    if (game == NULL || !game->authenticated || slotIndex < 0 || slotIndex >= SAVE_SLOT_COUNT) {
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
        game->selectedSaveSlot = 0;
    }

    nextSlot = game->selectedSaveSlot + delta;
    if (nextSlot < 0 || nextSlot >= SAVE_SLOT_COUNT) {
        return;
    }

    game->selectedSaveSlot = nextSlot;
    Audio_PlayCue(&game->audio, AUDIO_CUE_OPEN);
}

void Game_ActivateSelectedSaveSlot(Game *game) {
    SaveSlotInfo *slot;

    if (!IsValidSelectedSaveSlot(game)) {
        Game_PostMessage(game, LOC_UI_SAVE_MISSING, 2.5f);
        return;
    }

    slot = &game->saveSlots[game->selectedSaveSlot];
    if (game->savePanelMode == SAVE_PANEL_MODE_LOAD) {
        if (!slot->exists) {
            Game_PostMessage(game, LOC_UI_SAVE_MISSING, 2.5f);
            return;
        }

        Audio_PlayCue(&game->audio, AUDIO_CUE_CONFIRM);
        if (!Game_LoadSavedGame(game, game->selectedSaveSlot)) {
            Game_PostMessage(game, LOC_UI_SAVE_MISSING, 3.0f);
        }
        return;
    }

    if (slot->exists) {
        Game_PostMessage(game, Loc_PickLiteral("This slot is occupied. Delete it first, or choose an empty slot.", "该槽位已被占用，请先删除存档或选择空槽位。"), 3.2f);
        return;
    }

    Audio_PlayCue(&game->audio, AUDIO_CUE_CONFIRM);
    if (Game_SaveCurrentGame(game, game->selectedSaveSlot)) {
        Game_CloseSavePanel(game);
    }
}

void Game_DeleteSelectedSaveSlot(Game *game) {
    SaveSlotInfo *slot;

    if (!IsValidSelectedSaveSlot(game)) {
        Game_PostMessage(game, LOC_UI_SAVE_MISSING, 2.4f);
        return;
    }

    slot = &game->saveSlots[game->selectedSaveSlot];
    if (!slot->exists) {
        Game_PostMessage(game, LOC_UI_SAVE_MISSING, 2.4f);
        return;
    }

    if (SaveSystem_DeleteGame(game->selectedSaveSlot)) {
        Game_RefreshSaveSlots(game);
        Game_PostMessage(game, LOC_UI_SAVE_DELETED, 2.8f);
        Audio_PlayCue(&game->audio, AUDIO_CUE_CONFIRM);
        if (game->savePanelMode == SAVE_PANEL_MODE_LOAD && game->hasSaveFile) {
            game->selectedSaveSlot = FindFirstOccupiedSlot(game);
        } else if (game->savePanelMode == SAVE_PANEL_MODE_SAVE) {
            game->selectedSaveSlot = FindFirstEmptySlot(game);
        }
    } else {
        Game_PostMessage(game, LOC_UI_SAVE_DELETE_FAILED, 2.8f);
    }
}
