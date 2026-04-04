#ifndef GAME_MANAGER_H
#define GAME_MANAGER_H

#include "assets.h"
#include "audio_system.h"
#include "map.h"
#include "player.h"
#include "save_system.h"
#include "task_system.h"
#include "ui_system.h"

typedef enum GameState {
    GAME_STATE_INTRO = 0,
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
    float elapsedSeconds;
    int bufferedMoveX;
    int bufferedMoveY;
    float inputBufferTimer;
    int heldMoveX;
    int heldMoveY;
    float holdRepeatTimer;
    bool hasSaveFile;
    bool pauseMenuOpen;
    bool backpackOpen;
    bool craftOpen;
    bool communicatorOpen;
    bool helpOpen;
    bool requestClose;
} Game;

void Game_Init(Game *game);
void Game_Update(Game *game, float deltaTime);
void Game_Draw(Game *game);
void Game_Shutdown(Game *game);

#endif
