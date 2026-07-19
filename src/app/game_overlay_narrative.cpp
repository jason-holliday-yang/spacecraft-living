#include "game_overlay_frontend_internal.h"
#include "game_session_internal.h"

#include "localization.h"
#include "ui_runtime_internal.h"

#include <cmath>
#include <cstdio>
#include <cstring>

void GameOverlay_UpdateOpeningCutscene(Game *game) {
    static constexpr float kOpeningInitialBlackHoldDuration = 1.5f;

    if (game->flow.narrativeTransitionActive) {
        return;
    }

    if (IsKeyPressed(KEY_ESCAPE)) {
        Audio_PlayCue(&game->services.audio, AUDIO_CUE_CLOSE);
        game->flow.narrativeTransitionActive = true;
        game->flow.narrativeTransitionElapsed = 0.0f;
        game->flow.narrativeTransitionAction = NARRATIVE_TRANSITION_OPENING_COMPLETE;
        return;
    }

    if (!GameOverlay_IsCutsceneAdvancePressed()) {
        return;
    }

    if (game->flow.openingAwaitingFirstAdvance) {
        if (game->flow.openingCutsceneElapsed < kOpeningInitialBlackHoldDuration) {
            return;
        }

        game->flow.openingAwaitingFirstAdvance = false;
        game->flow.openingCutsceneElapsed = 0.0f;
        Audio_PlayCue(&game->services.audio, AUDIO_CUE_OPEN);
        return;
    }

    if (game->flow.openingSlideIndex + 1 >= INTRO_CUTSCENE_SLIDE_COUNT) {
        Audio_PlayCue(&game->services.audio, AUDIO_CUE_CONFIRM);
        game->flow.narrativeTransitionActive = true;
        game->flow.narrativeTransitionElapsed = 0.0f;
        game->flow.narrativeTransitionAction = NARRATIVE_TRANSITION_OPENING_COMPLETE;
        return;
    }

    Audio_PlayCue(&game->services.audio, AUDIO_CUE_OPEN);
    game->flow.narrativeTransitionActive = true;
    game->flow.narrativeTransitionElapsed = 0.0f;
    game->flow.narrativeTransitionAction = NARRATIVE_TRANSITION_OPENING_NEXT_SLIDE;
}

void GameOverlay_UpdateStoryScene(Game *game) {
    if (game->flow.narrativeTransitionActive) {
        return;
    }

    if (IsKeyPressed(KEY_ESCAPE)) {
        Audio_PlayCue(&game->services.audio, AUDIO_CUE_CLOSE);
        game->flow.narrativeTransitionActive = true;
        game->flow.narrativeTransitionElapsed = 0.0f;
        game->flow.narrativeTransitionAction = NARRATIVE_TRANSITION_STORY_CLOSE;
        return;
    }

    if (!GameOverlay_IsCutsceneAdvancePressed()) {
        return;
    }

    Audio_PlayCue(&game->services.audio, AUDIO_CUE_CONFIRM);
    game->flow.narrativeTransitionActive = true;
    game->flow.narrativeTransitionElapsed = 0.0f;
    game->flow.narrativeTransitionAction = NARRATIVE_TRANSITION_STORY_CLOSE;
}
