#include "game_play_internal.h"

typedef struct GameplayFeedbackSnapshot {
    float health;
    float poison;
    bool lowOxygen;
    bool oxygenLeak;
    bool suffocating;
    bool bossTelegraph;
    int bossPhase;
    GameEnding ending;
    int dayCount;
} GameplayFeedbackSnapshot;

static float ClampUnit(float value) {
    if (value < 0.0f) {
        return 0.0f;
    }
    if (value > 1.0f) {
        return 1.0f;
    }
    return value;
}

static Vector2 LerpVector2(Vector2 from, Vector2 to, float t) {
    return (Vector2){
        from.x + (to.x - from.x) * t,
        from.y + (to.y - from.y) * t
    };
}

static bool HasActiveBossTelegraph(const TaskSystem *tasks) {
    int index;

    if (tasks == NULL) {
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

static bool BossStartedTelegraphing(const TaskSystem *tasks, bool hadTelegraphBefore) {
    return !hadTelegraphBefore && HasActiveBossTelegraph(tasks);
}

static int GetActiveBossPhase(const TaskSystem *tasks) {
    int index;

    if (tasks == NULL) {
        return 0;
    }

    for (index = 0; index < tasks->monsterCount; index++) {
        const Monster *monster;

        monster = &tasks->monsters[index];
        if (!monster->active || monster->type != MONSTER_FINAL_BOSS || monster->maxHealth <= 0.0f) {
            continue;
        }

        const float healthPercent = monster->health / monster->maxHealth;
        if (healthPercent > 0.70f) {
            return 1;
        }
        if (healthPercent > 0.35f) {
            return 2;
        }
        return 3;
    }

    return 0;
}

static void GamePlay_UpdateGameplayTimers(Game *game, float deltaTime) {
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

    if (game->laserEffectTimer > 0.0f) {
        game->laserEffectTimer -= deltaTime;
        if (game->laserEffectTimer < 0.0f) {
            game->laserEffectTimer = 0.0f;
        }
    }
}

static void GamePlay_CaptureFeedbackSnapshot(const Game *game, GameplayFeedbackSnapshot *snapshot) {
    snapshot->health = game->player.health;
    snapshot->poison = game->player.poison;
    snapshot->lowOxygen = Player_HasStatus(&game->player, PLAYER_STATUS_LOW_OXYGEN);
    snapshot->oxygenLeak = Player_HasStatus(&game->player, PLAYER_STATUS_OXYGEN_LEAK);
    snapshot->suffocating = Player_HasStatus(&game->player, PLAYER_STATUS_SUFFOCATING);
    snapshot->bossTelegraph = HasActiveBossTelegraph(&game->tasks);
    snapshot->bossPhase = GetActiveBossPhase(&game->tasks);
    snapshot->ending = game->tasks.ending;
    snapshot->dayCount = game->tasks.dayCount;
}

static void GamePlay_UpdateGameplaySimulation(Game *game, float deltaTime, const GameplayFeedbackSnapshot *before) {
    Tasks_Update(&game->tasks, &game->map, &game->player, deltaTime);
    Game_MaybePostDayAdvanceMessage(game, before->dayCount);
    MiniMap_Update(&game->miniMap, &game->player, &game->map);
    Audio_SetScene(&game->audio, Game_SelectAudioScene(game));
    Audio_SetMusicStage(&game->audio, Game_SelectMusicStage(game));
    Game_MaybePostNorthRouteTransitionHint(game);
}

static void GamePlay_UpdateGameplayFeedback(Game *game, const GameplayFeedbackSnapshot *before) {
    static const float kGeneralHurtCueCooldown = 0.42f;
    static const float kSuffocationHurtCueCooldown = 1.2f;
    const bool healthDropped = game->player.health + 0.05f < before->health;
    const bool poisonIncreased = game->player.poison > before->poison + 0.5f;
    const bool lowOxygenStarted = !before->lowOxygen && Player_HasStatus(&game->player, PLAYER_STATUS_LOW_OXYGEN);
    const bool oxygenLeakStarted = !before->oxygenLeak && Player_HasStatus(&game->player, PLAYER_STATUS_OXYGEN_LEAK);
    const bool suffocatingNow = Player_HasStatus(&game->player, PLAYER_STATUS_SUFFOCATING);
    const bool suffocationDamageTriggered = healthDropped && (before->suffocating || suffocatingNow);

    if ((healthDropped || poisonIncreased || lowOxygenStarted || oxygenLeakStarted)
        && game->hurtSoundCooldown <= 0.0f) {
        Audio_PlayCue(&game->audio, AUDIO_CUE_HURT);
        game->hurtSoundCooldown = suffocationDamageTriggered
            ? kSuffocationHurtCueCooldown
            : kGeneralHurtCueCooldown;
    }

    if (BossStartedTelegraphing(&game->tasks, before->bossTelegraph) && game->monsterCueCooldown <= 0.0f) {
        Audio_PlayCue(&game->audio, AUDIO_CUE_MONSTER);
        game->monsterCueCooldown = 1.25f;
    }

    const int bossPhaseNow = GetActiveBossPhase(&game->tasks);
    if (before->bossPhase == 1 && bossPhaseNow == 2) {
        Game_PostMessage(game, Loc_PickLiteral("The guardian falters. Its rhythm breaks, and reinforcements begin to stir around the arena. Hold position and do not let the new pressure scatter you.",
                                               "守卫的步伐开始不稳，节奏出现断口，增援开始在战场四周苏醒。稳住位置，别让新来的压力把你冲散。"), 4.5f);
        Audio_PlayCue(&game->audio, AUDIO_CUE_WARNING);
    } else if (before->bossPhase == 2 && bossPhaseNow == 3) {
        Game_PostMessage(game, Loc_PickLiteral("The guardian is breaking apart. Faster, more desperate chain attacks are coming. Keep oxygen in reserve and finish this on the next real opening.",
                                               "守卫正在崩解。更快、更绝望的连段攻击即将到来。留好氧气，在下一次真正露出破绽时结束它。"), 4.5f);
        Audio_PlayCue(&game->audio, AUDIO_CUE_WARNING);
    } else if (before->bossPhase >= 1 && bossPhaseNow == 0 && game->tasks.bossDefeated) {
        Game_PostMessage(game, Loc_PickLiteral("The guardian collapses. The arena falls silent, and the northwest ruins no longer contest your presence here.",
                                               "守卫轰然倒塌。战场归于沉寂，西北遗迹不再抗拒你的存在。"), 5.0f);
    }
}

static bool GamePlay_UpdateGameplayFlow(Game *game,
                                        const StoryTriggerSnapshot *storyBefore,
                                        const GameplayFeedbackSnapshot *feedbackBefore) {
    GamePlay_TryOpenStorySceneFromSnapshot(game, storyBefore);

    if (game->tasks.ending == ENDING_NONE && game->player.health <= 0.0f) {
        Game_HandlePlayerDeath(game);
        return true;
    }

    if (!game->storySceneOpen && game->tasks.ending != ENDING_NONE) {
        if (feedbackBefore->ending == ENDING_NONE) {
            Game_RecordActiveAccountScore(game);
        }
        if (!game->screenTransitionActive) {
            Game_BeginScreenTransition(game, SCREEN_TRANSITION_ENTER_ENDING, -1);
        }
    }

    return false;
}

static void GamePlay_UpdateCamera(Game *game, float deltaTime) {
    static const float kCameraCatchUpSpeed = 11.0f;
    static const float kCameraSnapDistance = (float)TILE_SIZE * 2.5f;
    const Vector2 desiredTarget = game->player.renderPos;
    const float deltaX = desiredTarget.x - game->camera.target.x;
    const float deltaY = desiredTarget.y - game->camera.target.y;
    const float distanceSquared = deltaX * deltaX + deltaY * deltaY;

    if (distanceSquared >= kCameraSnapDistance * kCameraSnapDistance) {
        game->camera.target = desiredTarget;
    } else {
        const float followT = ClampUnit(deltaTime * kCameraCatchUpSpeed);
        game->camera.target = LerpVector2(game->camera.target, desiredTarget, followT);
    }

    game->camera.offset = (Vector2){GetScreenWidth() * 0.5f, GetScreenHeight() * 0.5f};
}

void Game_UpdatePlayingState(Game *game, float deltaTime) {
    char actionMessage[256];
    StoryTriggerSnapshot storyBefore;
    GameplayFeedbackSnapshot feedbackBefore;

    GamePlay_CaptureStoryTriggerSnapshot(game, &storyBefore);

    if (GamePlay_HandleImmediateInput(game, actionMessage, sizeof(actionMessage))) {
        GamePlay_TryOpenStorySceneFromSnapshot(game, &storyBefore);
        return;
    }

    GamePlay_UpdateGameplayTimers(game, deltaTime);
    GamePlay_UpdateMovement(game);
    GamePlay_CaptureFeedbackSnapshot(game, &feedbackBefore);
    GamePlay_UpdateGameplaySimulation(game, deltaTime, &feedbackBefore);
    GamePlay_UpdateGameplayFeedback(game, &feedbackBefore);

    if (GamePlay_UpdateGameplayFlow(game, &storyBefore, &feedbackBefore)) {
        return;
    }

    GamePlay_UpdateCamera(game, deltaTime);
}
