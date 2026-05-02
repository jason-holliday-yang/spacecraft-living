#include "game_manager_internal.h"
#include "game_session_internal.h"

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

static void ResolveNarrativeTransition(Game *game) {
    NarrativeTransitionAction action;

    if (game == nullptr) {
        return;
    }

    action = game->narrativeTransitionAction;
    game->narrativeTransitionActive = false;
    game->narrativeTransitionElapsed = 0.0f;
    game->narrativeTransitionAction = NARRATIVE_TRANSITION_NONE;

    switch (action) {
        case NARRATIVE_TRANSITION_OPENING_NEXT_SLIDE:
            game->openingSlideIndex += 1;
            game->openingCutsceneElapsed = 0.0f;
            return;
        case NARRATIVE_TRANSITION_OPENING_COMPLETE:
            Game_CompleteOpeningCutscene(game);
            return;
        case NARRATIVE_TRANSITION_STORY_CLOSE:
            Game_CloseStoryScene(game);
            return;
        case NARRATIVE_TRANSITION_NONE:
        default:
            return;
    }
}

static void ResolveScreenTransition(Game *game) {
    ScreenTransitionAction action;

    if (game == nullptr) {
        return;
    }

    action = game->screenTransitionAction;
    game->screenTransitionResolved = true;

    switch (action) {
        case SCREEN_TRANSITION_LOAD_GAME:
            Game_LoadSavedGame(game, game->screenTransitionSlotIndex);
            return;
        case SCREEN_TRANSITION_RETURN_TO_MENU:
            Game_ReturnToMenu(game);
            return;
        case SCREEN_TRANSITION_LOGOUT_TO_AUTH:
            Game_LogoutToAuthScreen(game);
            return;
        case SCREEN_TRANSITION_TOGGLE_AUTH_MODE:
            Game_ToggleAuthMode(game);
            return;
        case SCREEN_TRANSITION_AUTH_SUCCESS:
            Game_CompleteAuthSuccess(game);
            return;
        case SCREEN_TRANSITION_APPLY_LANGUAGE:
            Game_ApplyLanguage(game, game->pendingLanguage);
            return;
        case SCREEN_TRANSITION_SLEEP_REST: {
            const int previousDayCount = game->tasks.dayCount;

            Game_AdvanceWorldClock(game, SLEEP_TIME_ADVANCE_SECONDS);
            Game_MaybePostDayAdvanceMessage(game, previousDayCount);
            Game_ResetTransientGameplayState(game);
            return;
        }
        case SCREEN_TRANSITION_ENTER_ENDING:
            Game_EnterEndingState(game);
            return;
        case SCREEN_TRANSITION_NONE:
        default:
            return;
    }
}

static void UpdateNarrativePresentation(Game *game, float deltaTime) {
    if (game == nullptr) {
        return;
    }

    if (game->state != GAME_STATE_OPENING && !game->storySceneOpen) {
        game->narrativeTransitionActive = false;
        game->narrativeTransitionElapsed = 0.0f;
        game->narrativeTransitionAction = NARRATIVE_TRANSITION_NONE;
        return;
    }

    if (game->narrativeTransitionActive) {
        game->narrativeTransitionElapsed += deltaTime;
        if (game->narrativeTransitionElapsed >= Game_GetNarrativeTransitionDuration()) {
            ResolveNarrativeTransition(game);
        }
        return;
    }

    if (game->state == GAME_STATE_OPENING) {
        game->openingCutsceneElapsed += deltaTime;
    } else if (game->storySceneOpen) {
        game->storySceneElapsed += deltaTime;
    }
}

static void UpdateScreenTransition(Game *game, float deltaTime) {
    const float totalDuration = Game_GetScreenTransitionDuration(game);
    const float halfDuration = totalDuration * 0.5f;

    if (game == nullptr || !game->screenTransitionActive) {
        return;
    }

    game->screenTransitionElapsed += deltaTime;
    if (!game->screenTransitionResolved && game->screenTransitionElapsed >= halfDuration) {
        ResolveScreenTransition(game);
    }

    if (game->screenTransitionElapsed >= totalDuration) {
        game->screenTransitionActive = false;
        game->screenTransitionResolved = false;
        game->screenTransitionElapsed = 0.0f;
        game->screenTransitionAction = SCREEN_TRANSITION_NONE;
        game->screenTransitionSlotIndex = -1;
    }
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
    Audio_SetMusicStage(&game->audio, AUDIO_MUSIC_MENU);
    game->screenTransitionSlotIndex = -1;
    game->pendingLanguage = game->settings.language;
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

    UpdateNarrativePresentation(game, deltaTime);
    UpdateScreenTransition(game, deltaTime);
    Game_SanitizeGameplayState(game, GetScreenWidth(), GetScreenHeight());

    if (game->screenTransitionActive) {
        return;
    }

    if (Game_UpdateOverlayState(game)) {
        return;
    }

    Game_UpdatePlayingState(game, deltaTime);
    Game_SanitizeGameplayState(game, GetScreenWidth(), GetScreenHeight());
}

void Game_Shutdown(Game *game) {
    Game_ClearMessageHistory(game);
    SaveSystem_SaveSettings(&game->settings);
    Assets_Unload(&game->assets);
    Audio_Shutdown(&game->audio);
}
