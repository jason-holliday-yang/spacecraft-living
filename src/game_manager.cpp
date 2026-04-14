#include "game_manager_internal.h"

#include <cstdio>
#include <cstring>

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

void Game_Init(Game *game) {
    std::memset(game, 0, sizeof(*game));

    SaveSystem_LoadSettings(&game->settings);
    Loc_SetLanguage(game->settings.language);
    SaveSystem_Logout();
    Audio_Init(&game->audio);
    Audio_ApplySettings(&game->audio, &game->settings);
    Assets_Load(&game->assets);

    Game_ResetGameplayWorld(game);
    game->state = GAME_STATE_INTRO;
    game->selectedSaveSlot = 0;
    game->authenticated = SaveSystem_IsAccountAuthenticated();
    game->authHasAccounts = SaveSystem_HasRegisteredAccounts();
    game->authMode = game->authHasAccounts ? AUTH_SCREEN_MODE_LOGIN : AUTH_SCREEN_MODE_REGISTER;
    game->authSelectedField = AUTH_FIELD_USERNAME;
    game->authPasswordVisible = false;
    std::snprintf(game->authUsername, sizeof(game->authUsername), "%s", game->settings.lastUsername);
    Game_RefreshSaveSlots(game);
    Audio_SetScene(&game->audio, AUDIO_SCENE_MENU);
}

void Game_Update(Game *game, float deltaTime) {
    deltaTime = SanitizeDeltaTime(deltaTime);
    Audio_Update(&game->audio);
    if (game->hurtSoundCooldown > 0.0f) {
        game->hurtSoundCooldown -= deltaTime;
        if (game->hurtSoundCooldown < 0.0f) {
            game->hurtSoundCooldown = 0.0f;
        }
    }
    if (game->monsterCueCooldown > 0.0f) {
        game->monsterCueCooldown -= deltaTime;
        if (game->monsterCueCooldown < 0.0f) {
            game->monsterCueCooldown = 0.0f;
        }
    }

    if (game->hudMessage.timer > 0.0f) {
        game->hudMessage.timer -= deltaTime;
        if (game->hudMessage.timer <= 0.0f) {
            Game_ClearMessage(game);
        }
    }

    if (game->state == GAME_STATE_OPENING) {
        game->openingCutsceneElapsed += deltaTime;
    } else if (game->storySceneOpen) {
        game->storySceneElapsed += deltaTime;
    }

    if (Game_UpdateOverlayState(game)) {
        return;
    }

    Game_UpdatePlayingState(game, deltaTime);
}

void Game_Shutdown(Game *game) {
    SaveSystem_SaveSettings(&game->settings);
    Assets_Unload(&game->assets);
    Audio_Shutdown(&game->audio);
}
