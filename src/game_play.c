#include "game_play_internal.h"

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

static bool BossStartedTelegraphing(const TaskSystem *tasks, bool hadTelegraphBefore) {
    int index;

    if (hadTelegraphBefore) {
        return false;
    }

    for (index = 0; index < tasks->monsterCount; index++) {
        const Monster *monster;

        monster = &tasks->monsters[index];
        if (monster->active && monster->type == MONSTER_FINAL_BOSS && monster->attackTelegraph > 0.0f) {
            return true;
        }
    }

    return false;
}

void Game_UpdatePlayingState(Game *game, float deltaTime) {
    char actionMessage[256];
    StoryTriggerSnapshot storyBefore;
    float healthBeforeUpdate;
    float poisonBeforeUpdate;
    bool hadLowOxygenBefore;
    bool hadOxygenLeakBefore;
    bool hadBossTelegraph;
    GameEnding endingBeforeUpdate;
    int monsterIndex;

    GamePlay_CaptureStoryTriggerSnapshot(game, &storyBefore);

    if (GamePlay_HandleImmediateInput(game, actionMessage, sizeof(actionMessage))) {
        GamePlay_TryOpenStorySceneFromSnapshot(game, &storyBefore);
        return;
    }

    game->elapsedSeconds += deltaTime;

    if (game->player.moveTimer > 0.0f) {
        game->player.moveTimer -= deltaTime;
        if (game->player.moveTimer < 0.0f) {
            game->player.moveTimer = 0.0f;
        }
    }

    Player_UpdateAnimation(&game->player, deltaTime);

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

    GamePlay_UpdateMovement(game);

    healthBeforeUpdate = game->player.health;
    poisonBeforeUpdate = game->player.poison;
    hadLowOxygenBefore = Player_HasStatus(&game->player, PLAYER_STATUS_LOW_OXYGEN);
    hadOxygenLeakBefore = Player_HasStatus(&game->player, PLAYER_STATUS_OXYGEN_LEAK);
    endingBeforeUpdate = game->tasks.ending;
    hadBossTelegraph = false;
    for (monsterIndex = 0; monsterIndex < game->tasks.monsterCount; monsterIndex++) {
        const Monster *monster;

        monster = &game->tasks.monsters[monsterIndex];
        if (monster->active && monster->type == MONSTER_FINAL_BOSS && monster->attackTelegraph > 0.0f) {
            hadBossTelegraph = true;
            break;
        }
    }

    Tasks_Update(&game->tasks, &game->map, &game->player, deltaTime);
    MiniMap_Update(&game->miniMap, &game->player, &game->map);
    Audio_SetScene(&game->audio, Game_SelectAudioScene(game));
    Game_MaybePostNorthRouteTransitionHint(game);

    if ((game->player.health + 0.05f < healthBeforeUpdate
            || game->player.poison > poisonBeforeUpdate + 0.5f
            || (!hadLowOxygenBefore && Player_HasStatus(&game->player, PLAYER_STATUS_LOW_OXYGEN))
            || (!hadOxygenLeakBefore && Player_HasStatus(&game->player, PLAYER_STATUS_OXYGEN_LEAK)))
        && game->hurtSoundCooldown <= 0.0f) {
        Audio_PlayCue(&game->audio, AUDIO_CUE_HURT);
        game->hurtSoundCooldown = 0.42f;
    }

    if (BossStartedTelegraphing(&game->tasks, hadBossTelegraph) && game->monsterCueCooldown <= 0.0f) {
        Audio_PlayCue(&game->audio, AUDIO_CUE_MONSTER);
        game->monsterCueCooldown = 1.25f;
    }

    GamePlay_TryOpenStorySceneFromSnapshot(game, &storyBefore);

    if (game->tasks.ending == ENDING_NONE && game->player.health <= 0.0f) {
        Game_HandlePlayerDeath(game);
        return;
    }

    if (!game->storySceneOpen && game->tasks.ending != ENDING_NONE) {
        if (endingBeforeUpdate == ENDING_NONE) {
            Game_RecordActiveAccountScore(game);
        }
        game->state = GAME_STATE_ENDING;
        Audio_SetScene(&game->audio, AUDIO_SCENE_ENDING);
        Audio_PlayCue(&game->audio, endingBeforeUpdate == ENDING_NONE ? GetEndingCue(game->tasks.ending) : AUDIO_CUE_ENDING);
    }

    game->camera.target = game->player.renderPos;
    game->camera.offset = (Vector2){GetScreenWidth() * 0.5f, GetScreenHeight() * 0.5f};
}
