#include "game_manager_internal.h"

static constexpr float kNarrativeFadeDuration = 0.22f;
static constexpr float kScreenFadeDuration = 0.34f;
static constexpr float kSleepFadeDuration = 0.56f;
static constexpr float kMapFadeDuration = 1.40f;

static float ClampUnit(float value) {
    if (value < 0.0f) {
        return 0.0f;
    }
    if (value > 1.0f) {
        return 1.0f;
    }
    return value;
}

float Game_GetScreenTransitionDuration(const Game *game) {
    if (game != nullptr && game->flow.screenTransitionAction == SCREEN_TRANSITION_SLEEP_REST) {
        return kSleepFadeDuration;
    }
    if (game != nullptr && game->flow.screenTransitionAction == SCREEN_TRANSITION_MAP_CHANGE) {
        return kMapFadeDuration;
    }

    return kScreenFadeDuration;
}

float Game_GetNarrativeTransitionDuration(void) {
    return kNarrativeFadeDuration;
}

float Game_GetScreenTransitionAlpha(const Game *game) {
    const float totalDuration = Game_GetScreenTransitionDuration(game);
    const float halfDuration = totalDuration * 0.5f;
    float elapsed;

    if (game == nullptr || !game->flow.screenTransitionActive) {
        return 0.0f;
    }

    elapsed = game->flow.screenTransitionElapsed;
    if (elapsed <= halfDuration) {
        return ClampUnit(elapsed / halfDuration);
    }

    return 1.0f - ClampUnit((elapsed - halfDuration) / halfDuration);
}

float Game_GetNarrativeOverlayAlpha(const Game *game) {
    float fadeInAlpha;
    float fadeOutAlpha;
    float elapsed;

    if (game == nullptr) {
        return 0.0f;
    }

    if (game->flow.state == GAME_STATE_OPENING) {
        elapsed = game->flow.openingCutsceneElapsed;
    } else if (game->flow.storySceneOpen) {
        elapsed = game->flow.storySceneElapsed;
    } else {
        return 0.0f;
    }

    fadeInAlpha = 1.0f - ClampUnit(elapsed / Game_GetNarrativeTransitionDuration());
    fadeOutAlpha = game->flow.narrativeTransitionActive
        ? ClampUnit(game->flow.narrativeTransitionElapsed / Game_GetNarrativeTransitionDuration())
        : 0.0f;
    return fadeInAlpha > fadeOutAlpha ? fadeInAlpha : fadeOutAlpha;
}
