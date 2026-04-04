#include "game_manager.h"

#include "localization.h"

#include <math.h>
#include <stdio.h>
#include <string.h>

static float ClampFloat(float value, float minValue, float maxValue) {
    if (value < minValue) {
        return minValue;
    }
    if (value > maxValue) {
        return maxValue;
    }
    return value;
}

static float SanitizeDeltaTime(float deltaTime) {
    if (!(deltaTime >= 0.0f)) {
        return 0.0f;
    }
    return ClampFloat(deltaTime, 0.0f, 0.1f);
}

static float TileScale(float value) {
    return value * ((float)TILE_SIZE / 64.0f);
}

static void PostMessage(Game *game, const char *text, float duration) {
    snprintf(game->hudMessage.text, sizeof(game->hudMessage.text), "%s", text);
    game->hudMessage.timer = duration;
}

static void ClearMessage(Game *game) {
    game->hudMessage.text[0] = '\0';
    game->hudMessage.timer = 0.0f;
}

static void DrawObjectiveMarker(const TaskSystem *tasks, const Player *player, float elapsedSeconds) {
    int markerX;
    int markerY;

    if (Tasks_GetObjectiveMarker(tasks, player, &markerX, &markerY)) {
        Vector2 position;
        float outerRadius;

        position = Map_GridToWorld(markerX, markerY);
        outerRadius = TileScale(22.0f) + sinf(elapsedSeconds * 5.0f) * TileScale(2.0f);
        DrawRing(position, TileScale(14.0f), outerRadius, 0.0f, 360.0f, 32, (Color){106, 227, 255, 90});
        DrawCircleLines((int)position.x, (int)position.y, TileScale(16.0f), (Color){176, 238, 255, 140});
    }
}

static void DrawPlayer(const Player *player, const AssetBundle *assets, float elapsedSeconds) {
    if (assets->player.loaded) {
        Rectangle source;
        Rectangle dest;
        float drawSize;

        source = (Rectangle){0.0f, 0.0f, (float)assets->player.texture.width, (float)assets->player.texture.height};
        drawSize = TileScale(44.0f);
        dest = (Rectangle){player->worldPos.x - drawSize * 0.5f, player->worldPos.y - drawSize * 0.5f, drawSize, drawSize};
        DrawTexturePro(assets->player.texture, source, dest, (Vector2){0.0f, 0.0f}, 0.0f, WHITE);
        return;
    }

    DrawCircleV(player->worldPos, TileScale(11.0f), player->crouching ? (Color){155, 209, 236, 255} : (Color){185, 225, 255, 255});
    DrawCircleV((Vector2){player->worldPos.x, player->worldPos.y - TileScale(11.0f)}, TileScale(7.0f), (Color){185, 225, 255, 255});
    DrawEllipse((int)player->worldPos.x, (int)(player->worldPos.y - TileScale(11.0f)), TileScale(5.0f) + sinf(elapsedSeconds * 4.0f) * TileScale(0.4f), TileScale(3.0f), (Color){98, 201, 255, 255});
}

static void ResetGameplayWorld(Game *game) {
    Map_Init(&game->map);
    Player_Init(&game->player);
    Tasks_Init(&game->tasks, &game->map);
    game->elapsedSeconds = 0.0f;
    game->bufferedMoveX = 0;
    game->bufferedMoveY = 0;
    game->inputBufferTimer = 0.0f;
    game->heldMoveX = 0;
    game->heldMoveY = 0;
    game->holdRepeatTimer = 0.0f;
    game->pauseMenuOpen = false;
    game->backpackOpen = false;
    game->craftOpen = false;
    game->communicatorOpen = false;
    game->helpOpen = false;
    game->camera.target = game->player.worldPos;
    game->camera.offset = (Vector2){GetScreenWidth() * 0.5f, GetScreenHeight() * 0.5f};
    game->camera.rotation = 0.0f;
    game->camera.zoom = CAMERA_ZOOM;
    ClearMessage(game);
}

static void StartNewGame(Game *game) {
    ResetGameplayWorld(game);
    game->state = GAME_STATE_PLAYING;
    Audio_SetScene(&game->audio, AUDIO_SCENE_BASE);
    PostMessage(game, "Loxi online. Start by restoring the oxygen system at the ship base.", 4.0f);
}

static void BuildSnapshotFromGame(const Game *game, SaveSnapshot *snapshot) {
    int index;

    memset(snapshot, 0, sizeof(*snapshot));
    snapshot->gridX = game->player.gridX;
    snapshot->gridY = game->player.gridY;
    snapshot->facingX = game->player.facingX;
    snapshot->facingY = game->player.facingY;
    snapshot->stamina = game->player.stamina;
    snapshot->pressure = game->player.pressure;
    snapshot->oxygen = game->player.oxygen;
    snapshot->poison = game->player.poison;
    snapshot->maxStaminaBonus = game->player.maxStaminaBonus;
    snapshot->attackBonus = game->player.attackBonus;
    snapshot->deathCount = game->player.deathCount;
    snapshot->crouching = game->player.crouching;
    snapshot->hasGlowStick = game->player.hasGlowStick;
    snapshot->hasRope = game->player.hasRope;
    snapshot->hasLaserGun = game->player.hasLaserGun;
    snapshot->hasProtectionSuit = game->player.hasProtectionSuit;
    snapshot->hasSignalAmplifier = game->player.hasSignalAmplifier;
    snapshot->hasFieldCamp = game->player.hasFieldCamp;
    for (index = 0; index < RESOURCE_COUNT; index++) {
        snapshot->resources[index] = game->player.resources[index];
    }

    snapshot->stage = game->tasks.stage;
    snapshot->dayCount = game->tasks.dayCount;
    snapshot->phase = game->tasks.phase;
    snapshot->currentEvent = game->tasks.currentEvent;
    snapshot->cycleTimer = game->tasks.cycleTimer;
    snapshot->elapsedSeconds = game->tasks.elapsedSeconds;
    snapshot->oxygenRepairLevel = game->tasks.oxygenRepairLevel;
    snapshot->commRepairLevel = game->tasks.commRepairLevel;
    snapshot->energyRepairLevel = game->tasks.energyRepairLevel;
    snapshot->crashClueFound = game->tasks.crashClueFound;
    snapshot->amplifierUnlocked = game->tasks.amplifierUnlocked;
    snapshot->bossDefeated = game->tasks.bossDefeated;
    snapshot->signalTowerActivated = game->tasks.signalTowerActivated;
    snapshot->monolithActivated[0] = game->tasks.monolithActivated[0];
    snapshot->monolithActivated[1] = game->tasks.monolithActivated[1];
    snapshot->monolithActivated[2] = game->tasks.monolithActivated[2];
    snapshot->monolithsLit = game->tasks.monolithsLit;
    snapshot->ending = game->tasks.ending;
    snapshot->campPlaced = game->map.campPlaced;
    snapshot->campX = game->map.campX;
    snapshot->campY = game->map.campY;

    for (index = 0; index < game->tasks.nodeCount; index++) {
        snapshot->nodes[index].active = game->tasks.nodes[index].active;
        snapshot->nodes[index].respawnsRemaining = game->tasks.nodes[index].respawnsRemaining;
    }
    for (index = 0; index < game->tasks.monsterCount; index++) {
        snapshot->monsters[index].active = game->tasks.monsters[index].active;
        snapshot->monsters[index].gridX = game->tasks.monsters[index].gridX;
        snapshot->monsters[index].gridY = game->tasks.monsters[index].gridY;
        snapshot->monsters[index].health = game->tasks.monsters[index].health;
        snapshot->monsters[index].phaseTriggered = game->tasks.monsters[index].phaseTriggered;
    }
    for (index = 0; index < game->tasks.logCount; index++) {
        snapshot->logs[index].collected = game->tasks.logs[index].collected;
    }
}

static bool SaveCurrentGame(Game *game) {
    SaveSnapshot snapshot;
    bool saved;

    BuildSnapshotFromGame(game, &snapshot);
    saved = SaveSystem_SaveGame(&snapshot);
    game->hasSaveFile = saved || SaveSystem_HasSave();

    if (saved) {
        PostMessage(game, LOC_UI_SAVE_SUCCESS, 3.0f);
        Audio_PlayCue(&game->audio, AUDIO_CUE_CONFIRM);
    } else {
        PostMessage(game, LOC_UI_SAVE_FAILED, 3.0f);
    }

    return saved;
}

static bool LoadSavedGame(Game *game) {
    SaveSnapshot snapshot;
    int index;

    if (!SaveSystem_LoadGame(&snapshot)) {
        game->hasSaveFile = SaveSystem_HasSave();
        return false;
    }

    ResetGameplayWorld(game);

    game->player.gridX = snapshot.gridX;
    game->player.gridY = snapshot.gridY;
    game->player.facingX = snapshot.facingX;
    game->player.facingY = snapshot.facingY;
    Player_UpdateWorldPosition(&game->player);
    game->player.stamina = snapshot.stamina;
    game->player.pressure = snapshot.pressure;
    game->player.oxygen = snapshot.oxygen;
    game->player.poison = snapshot.poison;
    game->player.maxStaminaBonus = snapshot.maxStaminaBonus;
    game->player.attackBonus = snapshot.attackBonus;
    game->player.deathCount = snapshot.deathCount;
    game->player.crouching = snapshot.crouching;
    game->player.hasGlowStick = snapshot.hasGlowStick;
    game->player.hasRope = snapshot.hasRope;
    game->player.hasLaserGun = snapshot.hasLaserGun;
    game->player.hasProtectionSuit = snapshot.hasProtectionSuit;
    game->player.hasSignalAmplifier = snapshot.hasSignalAmplifier;
    game->player.hasFieldCamp = snapshot.hasFieldCamp;
    for (index = 0; index < RESOURCE_COUNT; index++) {
        game->player.resources[index] = snapshot.resources[index];
    }

    game->tasks.stage = snapshot.stage;
    game->tasks.dayCount = snapshot.dayCount;
    game->tasks.phase = (DayPhase)snapshot.phase;
    game->tasks.currentEvent = (EventType)snapshot.currentEvent;
    game->tasks.cycleTimer = snapshot.cycleTimer;
    game->tasks.elapsedSeconds = snapshot.elapsedSeconds;
    game->tasks.oxygenRepairLevel = snapshot.oxygenRepairLevel;
    game->tasks.commRepairLevel = snapshot.commRepairLevel;
    game->tasks.energyRepairLevel = snapshot.energyRepairLevel;
    game->tasks.crashClueFound = snapshot.crashClueFound;
    game->tasks.amplifierUnlocked = snapshot.amplifierUnlocked;
    game->tasks.communicatorUnlocked = snapshot.oxygenRepairLevel > 0 || snapshot.stage > 1;
    game->tasks.bossDefeated = snapshot.bossDefeated;
    game->tasks.signalTowerActivated = snapshot.signalTowerActivated;
    game->tasks.monolithActivated[0] = snapshot.monolithActivated[0];
    game->tasks.monolithActivated[1] = snapshot.monolithActivated[1];
    game->tasks.monolithActivated[2] = snapshot.monolithActivated[2];
    game->tasks.monolithsLit = snapshot.monolithsLit;
    game->tasks.ending = (GameEnding)snapshot.ending;
    for (index = 0; index < game->tasks.nodeCount; index++) {
        game->tasks.nodes[index].active = snapshot.nodes[index].active;
        game->tasks.nodes[index].respawnsRemaining = snapshot.nodes[index].respawnsRemaining;
    }
    for (index = 0; index < game->tasks.monsterCount; index++) {
        game->tasks.monsters[index].active = snapshot.monsters[index].active;
        game->tasks.monsters[index].gridX = snapshot.monsters[index].gridX;
        game->tasks.monsters[index].gridY = snapshot.monsters[index].gridY;
        game->tasks.monsters[index].health = snapshot.monsters[index].health;
        game->tasks.monsters[index].phaseTriggered = snapshot.monsters[index].phaseTriggered;
    }
    for (index = 0; index < game->tasks.logCount; index++) {
        game->tasks.logs[index].collected = snapshot.logs[index].collected;
    }

    if (game->tasks.stage >= 4) {
        Map_UnlockSwampOuter(&game->map);
    }
    if (game->tasks.stage >= 5) {
        Map_UnlockSwampDeep(&game->map);
    }
    if (game->tasks.stage >= 6) {
        Map_UnlockRuins(&game->map);
    }
    if (snapshot.campPlaced) {
        Map_SetFieldCamp(&game->map, snapshot.campX, snapshot.campY);
    }

    Tasks_UpdateObjective(&game->tasks, &game->player);
    game->state = game->tasks.ending == ENDING_NONE ? GAME_STATE_PLAYING : GAME_STATE_ENDING;
    game->hasSaveFile = true;
        PostMessage(game, "Saved progress loaded.", 3.0f);
    return true;
}

static void ReturnToMenu(Game *game) {
    game->state = GAME_STATE_INTRO;
    game->pauseMenuOpen = false;
    game->backpackOpen = false;
    game->craftOpen = false;
    game->communicatorOpen = false;
    game->helpOpen = false;
    game->hasSaveFile = SaveSystem_HasSave();
    Audio_SetScene(&game->audio, AUDIO_SCENE_MENU);
}

static bool GetPressedDirection(int *deltaX, int *deltaY) {
    if (IsKeyPressed(KEY_UP) || IsKeyPressed(KEY_W)) {
        *deltaX = 0;
        *deltaY = -1;
        return true;
    }
    if (IsKeyPressed(KEY_DOWN) || IsKeyPressed(KEY_S)) {
        *deltaX = 0;
        *deltaY = 1;
        return true;
    }
    if (IsKeyPressed(KEY_LEFT) || IsKeyPressed(KEY_A)) {
        *deltaX = -1;
        *deltaY = 0;
        return true;
    }
    if (IsKeyPressed(KEY_RIGHT) || IsKeyPressed(KEY_D)) {
        *deltaX = 1;
        *deltaY = 0;
        return true;
    }
    return false;
}

static bool GetHeldDirection(int *deltaX, int *deltaY) {
    if (IsKeyDown(KEY_UP) || IsKeyDown(KEY_W)) {
        *deltaX = 0;
        *deltaY = -1;
        return true;
    }
    if (IsKeyDown(KEY_DOWN) || IsKeyDown(KEY_S)) {
        *deltaX = 0;
        *deltaY = 1;
        return true;
    }
    if (IsKeyDown(KEY_LEFT) || IsKeyDown(KEY_A)) {
        *deltaX = -1;
        *deltaY = 0;
        return true;
    }
    if (IsKeyDown(KEY_RIGHT) || IsKeyDown(KEY_D)) {
        *deltaX = 1;
        *deltaY = 0;
        return true;
    }
    return false;
}

static bool SameDirection(int ax, int ay, int bx, int by) {
    return ax == bx && ay == by;
}

static void HandleMovement(Game *game, int deltaX, int deltaY) {
    bool moved;
    int targetX;
    int targetY;

    if (game->player.isDowned) {
        return;
    }

    targetX = game->player.gridX + deltaX;
    targetY = game->player.gridY + deltaY;

    if (Tasks_IsBlockingActorTile(&game->tasks, targetX, targetY)) {
        PostMessage(game, "A hostile creature is blocking the path.", 2.4f);
        game->player.moveTimer = BLOCKED_MOVE_COOLDOWN;
        return;
    }

    moved = Player_Move(&game->player, &game->map, deltaX, deltaY);
    if (moved) {
        Player_ConsumeStamina(&game->player, Map_GetMoveStaminaCost(&game->map, game->player.gridX, game->player.gridY));
        game->player.moveTimer = Player_GetMoveCooldown(&game->player);
        game->inputBufferTimer = 0.0f;
        game->bufferedMoveX = 0;
        game->bufferedMoveY = 0;
        return;
    }

    PostMessage(game, "That tile cannot be crossed.", 1.6f);
    game->player.moveTimer = BLOCKED_MOVE_COOLDOWN;
}

static void UpdateMovement(Game *game) {
    int pressedX;
    int pressedY;
    int heldX;
    int heldY;
    bool hasPressedInput;
    bool hasHeldInput;

    pressedX = 0;
    pressedY = 0;
    heldX = 0;
    heldY = 0;
    hasPressedInput = GetPressedDirection(&pressedX, &pressedY);
    hasHeldInput = GetHeldDirection(&heldX, &heldY);

    if (hasPressedInput) {
        game->bufferedMoveX = pressedX;
        game->bufferedMoveY = pressedY;
        game->inputBufferTimer = INPUT_BUFFER_TIME;
        game->heldMoveX = pressedX;
        game->heldMoveY = pressedY;
        game->holdRepeatTimer = HOLD_REPEAT_INITIAL_DELAY;
    }

    if (!hasHeldInput) {
        game->heldMoveX = 0;
        game->heldMoveY = 0;
        game->holdRepeatTimer = 0.0f;
    } else if (!SameDirection(game->heldMoveX, game->heldMoveY, heldX, heldY)) {
        game->heldMoveX = heldX;
        game->heldMoveY = heldY;
        game->holdRepeatTimer = HOLD_REPEAT_INITIAL_DELAY;
    } else if (!hasPressedInput && game->holdRepeatTimer <= 0.0f) {
        game->bufferedMoveX = heldX;
        game->bufferedMoveY = heldY;
        game->inputBufferTimer = INPUT_BUFFER_TIME;
        game->holdRepeatTimer = HOLD_REPEAT_INTERVAL;
    }

    if (game->player.moveTimer > 0.0f) {
        return;
    }

    if (game->inputBufferTimer > 0.0f && (game->bufferedMoveX != 0 || game->bufferedMoveY != 0)) {
        HandleMovement(game, game->bufferedMoveX, game->bufferedMoveY);
    }
}

static void UpdateAudioScene(Game *game) {
    AudioScene scene;
    MapArea area;

    if (game->state != GAME_STATE_PLAYING) {
        return;
    }

    if (game->tasks.ending != ENDING_NONE) {
        Audio_SetScene(&game->audio, AUDIO_SCENE_ENDING);
        return;
    }

    area = Map_GetAreaAt(game->player.gridX, game->player.gridY);
    if (!game->tasks.bossDefeated && area == MAP_AREA_RUINS && game->tasks.stage >= 7) {
        scene = AUDIO_SCENE_BOSS;
    } else {
        switch (area) {
            case MAP_AREA_BASE:
                scene = AUDIO_SCENE_BASE;
                break;
            case MAP_AREA_FOREST:
                scene = AUDIO_SCENE_FOREST;
                break;
            case MAP_AREA_SWAMP_OUTER:
            case MAP_AREA_SWAMP_DEEP:
                scene = AUDIO_SCENE_SWAMP;
                break;
            case MAP_AREA_RUINS:
                scene = AUDIO_SCENE_RUINS;
                break;
            case MAP_AREA_UNKNOWN:
            default:
                scene = AUDIO_SCENE_BASE;
                break;
        }
    }

    Audio_SetScene(&game->audio, scene);
}

static void UpdateMainMenu(Game *game) {
    Vector2 mouse;
    int buttonIndex;
    bool clickedAnyButton;

    if (IsKeyPressed(KEY_ENTER)) {
        Audio_PlayCue(&game->audio, AUDIO_CUE_CONFIRM);
        StartNewGame(game);
        return;
    }

    if (!IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
        return;
    }

    mouse = GetMousePosition();
    clickedAnyButton = false;
    for (buttonIndex = 0; buttonIndex < MAIN_MENU_BUTTON_COUNT; buttonIndex++) {
        Rectangle rect;

        rect = UI_GetMainMenuButtonRect(GetScreenWidth(), GetScreenHeight(), buttonIndex);
        if (!CheckCollisionPointRec(mouse, rect)) {
            continue;
        }

        clickedAnyButton = true;

        switch (buttonIndex) {
            case MAIN_MENU_BUTTON_START:
                Audio_PlayCue(&game->audio, AUDIO_CUE_CONFIRM);
                StartNewGame(game);
                return;
            case MAIN_MENU_BUTTON_CONTINUE:
                if (game->hasSaveFile) {
                    Audio_PlayCue(&game->audio, AUDIO_CUE_CONFIRM);
                    if (!LoadSavedGame(game)) {
                        PostMessage(game, LOC_UI_SAVE_MISSING, 3.0f);
                        ReturnToMenu(game);
                    }
                }
                return;
            case MAIN_MENU_BUTTON_EXIT:
                game->requestClose = true;
                return;
            default:
                break;
        }
    }

    if (!clickedAnyButton) {
        Audio_PlayCue(&game->audio, AUDIO_CUE_CONFIRM);
        StartNewGame(game);
    }
}

static void UpdatePauseMenu(Game *game) {
    Vector2 mouse;
    int buttonIndex;

    if (IsKeyPressed(KEY_ESCAPE)) {
        game->pauseMenuOpen = false;
        Audio_PlayCue(&game->audio, AUDIO_CUE_OPEN);
        return;
    }

    if (!IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
        return;
    }

    mouse = GetMousePosition();
    for (buttonIndex = 0; buttonIndex < PAUSE_MENU_BUTTON_COUNT; buttonIndex++) {
        Rectangle rect;

        rect = UI_GetPauseMenuButtonRect(GetScreenWidth(), GetScreenHeight(), buttonIndex);
        if (!CheckCollisionPointRec(mouse, rect)) {
            continue;
        }

        switch (buttonIndex) {
            case PAUSE_MENU_BUTTON_CONTINUE:
                game->pauseMenuOpen = false;
                Audio_PlayCue(&game->audio, AUDIO_CUE_CONFIRM);
                return;
            case PAUSE_MENU_BUTTON_SAVE:
                SaveCurrentGame(game);
                return;
            case PAUSE_MENU_BUTTON_MENU:
                ReturnToMenu(game);
                return;
            default:
                break;
        }
    }
}

static void UpdateCraftOverlay(Game *game) {
    int recipeIndex;

    if (IsKeyPressed(KEY_Q) || IsKeyPressed(KEY_ESCAPE)) {
        game->craftOpen = false;
        Audio_PlayCue(&game->audio, AUDIO_CUE_OPEN);
        return;
    }

    for (recipeIndex = 0; recipeIndex < 9; recipeIndex++) {
        KeyboardKey key;

        key = (KeyboardKey)(KEY_ONE + recipeIndex);
        if (IsKeyPressed(key) && recipeIndex < Tasks_GetVisibleRecipeCount(&game->tasks)) {
            char message[256];
            RecipeType recipe;

            recipe = Tasks_GetVisibleRecipeAt(&game->tasks, recipeIndex);
            if (Tasks_TryCraft(&game->tasks, &game->map, &game->player, recipe, message, sizeof(message))) {
                Audio_PlayCue(&game->audio, AUDIO_CUE_CRAFT);
                PostMessage(game, message, 3.6f);
            } else {
                PostMessage(game, message, 3.0f);
            }
            return;
        }
    }
}

static void UpdateBackpackOverlay(Game *game) {
    if (IsKeyPressed(KEY_B) || IsKeyPressed(KEY_ESCAPE)) {
        game->backpackOpen = false;
        Audio_PlayCue(&game->audio, AUDIO_CUE_OPEN);
    }
}

static void UpdateCommunicatorOverlay(Game *game) {
    if (IsKeyPressed(KEY_N) || IsKeyPressed(KEY_ESCAPE)) {
        game->communicatorOpen = false;
        Audio_PlayCue(&game->audio, AUDIO_CUE_OPEN);
    }
}

static void UpdateHelpOverlay(Game *game) {
    if (IsKeyPressed(KEY_H) || IsKeyPressed(KEY_ESCAPE)) {
        game->helpOpen = false;
        Audio_PlayCue(&game->audio, AUDIO_CUE_OPEN);
    }
}

void Game_Init(Game *game) {
    memset(game, 0, sizeof(*game));

    SaveSystem_LoadSettings(&game->settings);
    Audio_Init(&game->audio);
    Audio_ApplySettings(&game->audio, &game->settings);
    Assets_Load(&game->assets);

    MiniMap_Init(&game->miniMap);
    ResetGameplayWorld(game);
    game->state = GAME_STATE_INTRO;
    game->hasSaveFile = SaveSystem_HasSave();
    Audio_SetScene(&game->audio, AUDIO_SCENE_MENU);
}

void Game_Update(Game *game, float deltaTime) {
    char actionMessage[256];

    deltaTime = SanitizeDeltaTime(deltaTime);
    Audio_Update(&game->audio);

    if (game->hudMessage.timer > 0.0f) {
        game->hudMessage.timer -= deltaTime;
        if (game->hudMessage.timer <= 0.0f) {
            ClearMessage(game);
        }
    }

    if (game->state == GAME_STATE_INTRO) {
        UpdateMainMenu(game);
        return;
    }

    if (game->state == GAME_STATE_ENDING) {
        if (IsKeyPressed(KEY_ESCAPE)) {
            game->requestClose = true;
        }
        return;
    }

    if (game->pauseMenuOpen) {
        UpdatePauseMenu(game);
        return;
    }

    if (game->craftOpen) {
        UpdateCraftOverlay(game);
        return;
    }

    if (game->backpackOpen) {
        UpdateBackpackOverlay(game);
        return;
    }

    if (game->communicatorOpen) {
        UpdateCommunicatorOverlay(game);
        return;
    }

    if (game->helpOpen) {
        UpdateHelpOverlay(game);
        return;
    }

    if (IsKeyPressed(KEY_ESCAPE)) {
        game->pauseMenuOpen = true;
        Audio_PlayCue(&game->audio, AUDIO_CUE_OPEN);
        return;
    }
    if (IsKeyPressed(KEY_Q)) {
        game->craftOpen = true;
        Audio_PlayCue(&game->audio, AUDIO_CUE_OPEN);
        return;
    }
    if (IsKeyPressed(KEY_B)) {
        game->backpackOpen = true;
        Audio_PlayCue(&game->audio, AUDIO_CUE_OPEN);
        return;
    }
    if (IsKeyPressed(KEY_N)) {
        if (Tasks_IsCommunicatorUnlocked(&game->tasks)) {
            game->communicatorOpen = true;
            Audio_PlayCue(&game->audio, AUDIO_CUE_OPEN);
        } else {
            PostMessage(game, "Loxi link is offline. Sync with the ship core first, then press N.", 2.8f);
        }
        return;
    }
    if (IsKeyPressed(KEY_H)) {
        game->helpOpen = true;
        Audio_PlayCue(&game->audio, AUDIO_CUE_OPEN);
        return;
    }
    if (IsKeyPressed(KEY_M)) {
        MiniMap_Toggle(&game->miniMap);
        Audio_PlayCue(&game->audio, AUDIO_CUE_OPEN);
        return;
    }
    if (IsKeyPressed(KEY_C)) {
        game->player.crouching = !game->player.crouching;
        PostMessage(game, game->player.crouching ? "Stealth crouch enabled." : "Stealth crouch disabled.", 2.0f);
    }

    game->elapsedSeconds += deltaTime;

    if (game->player.moveTimer > 0.0f) {
        game->player.moveTimer -= deltaTime;
        if (game->player.moveTimer < 0.0f) {
            game->player.moveTimer = 0.0f;
        }
    }

    if (game->inputBufferTimer > 0.0f) {
        game->inputBufferTimer -= deltaTime;
        if (game->inputBufferTimer <= 0.0f) {
            game->inputBufferTimer = 0.0f;
            game->bufferedMoveX = 0;
            game->bufferedMoveY = 0;
        }
    }

    if (game->holdRepeatTimer > 0.0f) {
        game->holdRepeatTimer -= deltaTime;
        if (game->holdRepeatTimer < 0.0f) {
            game->holdRepeatTimer = 0.0f;
        }
    }

    UpdateMovement(game);

    if (IsKeyPressed(KEY_E)) {
        if (Tasks_HandleInteraction(&game->tasks, &game->map, &game->player, actionMessage, sizeof(actionMessage))) {
            Audio_PlayCue(&game->audio, AUDIO_CUE_REPAIR);
            PostMessage(game, actionMessage, 4.0f);
        } else {
            PostMessage(game, actionMessage, 2.6f);
        }
    }

    if (IsKeyPressed(KEY_SPACE)) {
        if (!game->player.isDowned) {
            if (Tasks_HandleAttack(&game->tasks, &game->map, &game->player, actionMessage, sizeof(actionMessage))) {
                Audio_PlayCue(&game->audio, game->player.hasLaserGun ? AUDIO_CUE_LASER : AUDIO_CUE_CONFIRM);
                PostMessage(game, actionMessage, 2.8f);
            } else {
                PostMessage(game, actionMessage, 2.2f);
            }
        } else {
            PostMessage(game, "You are downed! Use food or medicine to recover.", 2.0f);
        }
    }

    if (IsKeyPressed(KEY_Z)) {
        if (Player_UseQuickConsumable(&game->player, CONSUMABLE_FOOD, actionMessage, (int)sizeof(actionMessage))) {
            PostMessage(game, actionMessage, 2.8f);
        } else {
            if (game->player.isDowned) {
                PostMessage(game, "No food available! You need to eat to recover.", 2.0f);
            } else {
                PostMessage(game, actionMessage, 2.4f);
            }
        }
    }

    if (IsKeyPressed(KEY_X)) {
        if (Player_UseQuickConsumable(&game->player, CONSUMABLE_CALM, actionMessage, (int)sizeof(actionMessage))) {
            PostMessage(game, actionMessage, 2.8f);
        } else {
            if (game->player.isDowned) {
                PostMessage(game, "No medicine available! You need treatment to recover.", 2.0f);
            } else {
                PostMessage(game, actionMessage, 2.4f);
            }
        }
    }

    bool wasDowned = game->player.isDowned;
    
    Tasks_Update(&game->tasks, &game->map, &game->player, deltaTime);
    MiniMap_Update(&game->miniMap, &game->player, &game->map);
    UpdateAudioScene(game);

    if (wasDowned && !game->player.isDowned && game->player.stamina <= 0.0f) {
        game->showDeathPopup = true;
    }
    
    if (game->showDeathPopup && IsKeyPressed(KEY_ENTER)) {
        game->showDeathPopup = false;
    }

    if (game->tasks.ending != ENDING_NONE) {
        game->state = GAME_STATE_ENDING;
        Audio_SetScene(&game->audio, AUDIO_SCENE_ENDING);
        Audio_PlayCue(&game->audio, AUDIO_CUE_ENDING);
    }

    game->camera.target = game->player.worldPos;
    game->camera.offset = (Vector2){GetScreenWidth() * 0.5f, GetScreenHeight() * 0.5f};
}

void Game_Draw(Game *game) {
    int screenWidth;
    int screenHeight;

    screenWidth = GetScreenWidth();
    screenHeight = GetScreenHeight();

    BeginDrawing();
    ClearBackground(BLACK);

    if (game->state == GAME_STATE_INTRO) {
        UI_DrawMainMenu(&game->assets, game->hasSaveFile, screenWidth, screenHeight, game->elapsedSeconds);
        EndDrawing();
        return;
    }

    BeginMode2D(game->camera);
    Map_Draw(&game->map, &game->assets, game->camera, screenWidth, screenHeight, game->elapsedSeconds);
    DrawObjectiveMarker(&game->tasks, &game->player, game->elapsedSeconds);
    Tasks_DrawWorld(&game->tasks, &game->assets, game->elapsedSeconds);
    DrawPlayer(&game->player, &game->assets, game->elapsedSeconds);
    EndMode2D();

    if (game->state == GAME_STATE_ENDING) {
        UI_DrawEnding(game->tasks.ending, &game->player, &game->tasks, &game->assets, screenWidth, screenHeight, game->elapsedSeconds);
    } else if (game->showDeathPopup) {
        UI_DrawDeathPopup(&game->player, &game->assets, screenWidth, screenHeight);
    } else {
        UI_DrawHud(&game->player, &game->tasks, &game->hudMessage, &game->assets, screenWidth, screenHeight);
        MiniMap_Draw(&game->miniMap, &game->player, &game->map, screenWidth, screenHeight);
        
        if (game->player.isDowned) {
            UI_DrawDownedOverlay(&game->player, &game->assets, screenWidth, screenHeight);
        }
        
        if (game->pauseMenuOpen) {
            UI_DrawPauseMenu(&game->assets, screenWidth, screenHeight);
        }
        if (game->craftOpen) {
            UI_DrawCraftOverlay(&game->assets, &game->tasks, screenWidth, screenHeight);
        }
        if (game->backpackOpen) {
            UI_DrawBackpackOverlay(&game->assets, &game->player, screenWidth, screenHeight);
        }
        if (game->communicatorOpen) {
            UI_DrawCommunicatorOverlay(&game->assets, &game->tasks, screenWidth, screenHeight);
        }
        if (game->helpOpen) {
            UI_DrawHelpOverlay(&game->assets, screenWidth, screenHeight);
        }
    }

    EndDrawing();
}

void Game_Shutdown(Game *game) {
    SaveSystem_SaveSettings(&game->settings);
    Assets_Unload(&game->assets);
    Audio_Shutdown(&game->audio);
}
