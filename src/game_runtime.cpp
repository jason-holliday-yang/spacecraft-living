#include "game_manager_internal.h"

#include "task_runtime_internal.h"

#include <cstdio>
#include <cstring>

static bool IsMainStoryScene(StoryScene scene) {
    return scene >= STORY_SCENE_MAIN_AIR_FOR_ONE_MORE_DAY
        && scene < (StoryScene)(STORY_SCENE_MAIN_AIR_FOR_ONE_MORE_DAY + STORY_MAIN_SCENE_COUNT);
}

static AudioCue GetEndingCue(GameEnding ending) {
    switch (ending) {
        case ENDING_PEACEFUL:
            return AUDIO_CUE_ENDING_PEACEFUL;
        case ENDING_SETTLEMENT:
            return AUDIO_CUE_ENDING_SETTLEMENT;
        case ENDING_HEROIC:
            return AUDIO_CUE_ENDING;
        case ENDING_FAILURE:
        case ENDING_NONE:
        default:
            return AUDIO_CUE_WARNING;
    }
}

void Game_EnterEndingState(Game *game) {
    if (game == NULL || game->tasks.ending == ENDING_NONE) {
        return;
    }

    game->state = GAME_STATE_ENDING;
    Audio_SetScene(&game->audio, AUDIO_SCENE_ENDING);
    Audio_SetMusicStage(&game->audio, AUDIO_MUSIC_ENDING);
    Audio_PlayCue(&game->audio, GetEndingCue(game->tasks.ending));
}

void Game_AdvanceWorldClock(Game *game, float deltaTime) {
    if (game == NULL || !(deltaTime > 0.0f)) {
        return;
    }

    game->elapsedSeconds += deltaTime;
    TasksRuntime_UpdateDayCycle(&game->tasks, deltaTime);
}

void Game_MaybePostDayAdvanceMessage(Game *game, int previousDayCount) {
    char message[96];
    const char *formatText;

    if (game == NULL || game->tasks.dayCount == previousDayCount) {
        return;
    }

    formatText = Loc_PickLiteral("Day %d begins.", "第 %d 天开始");
    std::snprintf(message, sizeof(message), formatText, game->tasks.dayCount + 1);
    Game_PostMessage(game, message, 3.0f);
}

bool Game_OpenStoryScene(Game *game, StoryScene scene) {
    if (game == NULL || scene <= STORY_SCENE_NONE || scene >= STORY_SCENE_COUNT) {
        return false;
    }

    if (game->storySceneShown[scene] || game->storySceneOpen) {
        return false;
    }

    game->storySceneShown[scene] = true;
    if (IsMainStoryScene(scene) && game->tasks.shownMainStorySceneCount < STORY_MAIN_SCENE_COUNT) {
        game->tasks.shownMainStorySceneCount += 1;
    }
    game->storySceneOpen = true;
    game->storyScene = scene;
    game->storySceneElapsed = 0.0f;
    return true;
}

void Game_CloseStoryScene(Game *game) {
    if (game == NULL) {
        return;
    }

    game->storySceneOpen = false;
    game->storyScene = STORY_SCENE_NONE;
    game->storySceneElapsed = 0.0f;
    game->narrativeTransitionActive = false;
    game->narrativeTransitionElapsed = 0.0f;
    game->narrativeTransitionAction = NARRATIVE_TRANSITION_NONE;
}
