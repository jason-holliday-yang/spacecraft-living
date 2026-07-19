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

    action = game->flow.narrativeTransitionAction;
    game->flow.narrativeTransitionActive = false;
    game->flow.narrativeTransitionElapsed = 0.0f;
    game->flow.narrativeTransitionAction = NARRATIVE_TRANSITION_NONE;

    switch (action) {
        case NARRATIVE_TRANSITION_OPENING_NEXT_SLIDE:
            game->flow.openingSlideIndex += 1;
            game->flow.openingCutsceneElapsed = 0.0f;
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

    action = game->flow.screenTransitionAction;
    game->flow.screenTransitionResolved = true;

    switch (action) {
        case SCREEN_TRANSITION_LOAD_GAME:
            Game_LoadSavedGame(game, game->flow.screenTransitionSlotIndex);
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
            Game_ApplyLanguage(game, game->flow.pendingLanguage);
            return;
        case SCREEN_TRANSITION_SLEEP_REST: {
            const int previousDayCount = game->runtime.tasks.dayCount;

            Game_AdvanceWorldClock(game, SLEEP_TIME_ADVANCE_SECONDS);
            Game_MaybePostDayAdvanceMessage(game, previousDayCount);
            Game_ResetTransientGameplayState(game);
            return;
        }
        case SCREEN_TRANSITION_MAP_CHANGE:
            Game_ResolveMapTransition(game);
            return;
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

    if (game->flow.state != GAME_STATE_OPENING && !game->flow.storySceneOpen) {
        game->flow.narrativeTransitionActive = false;
        game->flow.narrativeTransitionElapsed = 0.0f;
        game->flow.narrativeTransitionAction = NARRATIVE_TRANSITION_NONE;
        return;
    }

    if (game->flow.narrativeTransitionActive) {
        game->flow.narrativeTransitionElapsed += deltaTime;
        if (game->flow.narrativeTransitionElapsed >= Game_GetNarrativeTransitionDuration()) {
            ResolveNarrativeTransition(game);
        }
        return;
    }

    if (game->flow.state == GAME_STATE_OPENING) {
        game->flow.openingCutsceneElapsed += deltaTime;
    } else if (game->flow.storySceneOpen) {
        game->flow.storySceneElapsed += deltaTime;
    }
}

static void UpdateScreenTransition(Game *game, float deltaTime) {
    const float totalDuration = Game_GetScreenTransitionDuration(game);
    const float halfDuration = totalDuration * 0.5f;

    if (game == nullptr || !game->flow.screenTransitionActive) {
        return;
    }

    game->flow.screenTransitionElapsed += deltaTime;
    if (!game->flow.screenTransitionResolved && game->flow.screenTransitionElapsed >= halfDuration) {
        ResolveScreenTransition(game);
    }

    if (game->flow.screenTransitionElapsed >= totalDuration) {
        game->flow.screenTransitionActive = false;
        game->flow.screenTransitionResolved = false;
        game->flow.screenTransitionElapsed = 0.0f;
        game->flow.screenTransitionAction = SCREEN_TRANSITION_NONE;
        game->flow.screenTransitionSlotIndex = -1;
        Game_FinishMapTransition(game);
    }
}

void Game_Init(Game *game) {
    std::memset(game, 0, sizeof(*game));

    SaveSystem_LoadSettings(&game->services.settings);
    Loc_SetLanguage(game->services.settings.language);
    SaveSystem_Logout();
    Audio_Init(&game->services.audio);
    Audio_ApplySettings(&game->services.audio, &game->services.settings);
    Assets_Load(&game->services.assets);

    Game_ResetGameplayWorld(game);
    game->flow.state = GAME_STATE_INTRO;
    game->ui.selectedSaveSlot = 0;
    game->account.authenticated = SaveSystem_IsAccountAuthenticated();
    game->account.authHasAccounts = SaveSystem_HasRegisteredAccounts();
    game->account.authMode = game->account.authHasAccounts ? AUTH_SCREEN_MODE_LOGIN : AUTH_SCREEN_MODE_REGISTER;
    game->account.authSelectedField = AUTH_FIELD_USERNAME;
    game->account.authPasswordVisible = false;
    std::snprintf(game->account.authUsername, sizeof(game->account.authUsername), "%s", game->services.settings.lastUsername);
    Game_RefreshSaveSlots(game);
    Audio_SetScene(&game->services.audio, AUDIO_SCENE_MENU);
    Audio_SetMusicStage(&game->services.audio, AUDIO_MUSIC_MENU);
    game->flow.screenTransitionSlotIndex = -1;
    game->flow.pendingLanguage = game->services.settings.language;
}

void Game_Update(Game *game, float deltaTime) {
    deltaTime = SanitizeDeltaTime(deltaTime);
    Audio_Update(&game->services.audio);
    if (game->runtime.map.loadFailed) {
        return;
    }
    if (game->runtime.hurtSoundCooldown > 0.0f) {
        game->runtime.hurtSoundCooldown -= deltaTime;
        if (game->runtime.hurtSoundCooldown < 0.0f) {
            game->runtime.hurtSoundCooldown = 0.0f;
        }
    }
    if (game->runtime.monsterCueCooldown > 0.0f) {
        game->runtime.monsterCueCooldown -= deltaTime;
        if (game->runtime.monsterCueCooldown < 0.0f) {
            game->runtime.monsterCueCooldown = 0.0f;
        }
    }

    if (game->ui.hudMessage.timer > 0.0f) {
        game->ui.hudMessage.timer -= deltaTime;
        if (game->ui.hudMessage.timer <= 0.0f) {
            Game_ClearMessage(game);
        }
    }

    UpdateNarrativePresentation(game, deltaTime);
    UpdateScreenTransition(game, deltaTime);
    Game_SanitizeGameplayState(game, GetScreenWidth(), GetScreenHeight());

    if (Game_UpdateOverlayState(game)) {
        return;
    }

    if (game->flow.screenTransitionActive) {
        return;
    }

    Game_UpdatePlayingState(game, deltaTime);
    Game_SanitizeGameplayState(game, GetScreenWidth(), GetScreenHeight());
}

void Game_Shutdown(Game *game) {
    Game_ClearMessageHistory(game);
    SaveSystem_SaveSettings(&game->services.settings);
    MiniMap_Destroy(&game->runtime.miniMap);
    MapSceneManager_Destroy(&game->services.mapScene);
    Map_Destroy(&game->runtime.map);
    Assets_Unload(&game->services.assets);
    Audio_Shutdown(&game->services.audio);
}
