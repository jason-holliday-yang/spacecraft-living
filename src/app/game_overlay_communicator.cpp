#include "game_overlay_gameplay_internal.h"

#include "localization.h"
#include "ui_inventory_internal.h"
#include "ui_story_internal.h"
#include "ui_runtime_internal.h"

#include <cmath>
#include <cstdio>

static float ClampUnit(float value) {
    if (value < 0.0f) {
        return 0.0f;
    }
    if (value > 1.0f) {
        return 1.0f;
    }
    return value;
}

void GameOverlay_ResetCommunicatorDetailState(Game *game) {
    if (game == NULL) {
        return;
    }

    game->ui.communicatorLogDetailOpen = false;
    game->ui.communicatorLogDetailVisibility = 0.0f;
    game->ui.communicatorLogDetailScroll = 0.0f;
}

static void ScrollCommunicatorDetail(Game *game, float delta) {
    if (game == NULL) {
        return;
    }

    game->ui.communicatorLogDetailScroll += delta;
    if (game->ui.communicatorLogDetailScroll < 0.0f) {
        game->ui.communicatorLogDetailScroll = 0.0f;
    }
}

static void EnsureCommunicatorSelectionVisible(Game *game, int collectedCount) {
    int visibleCount;

    if (game == NULL) {
        return;
    }

    game->ui.communicatorFirstVisibleLogIndex = UI_ClampCommunicatorFirstVisibleLogIndex(GetScreenWidth(),
                                                                                      GetScreenHeight(),
                                                                                      game->ui.communicatorFirstVisibleLogIndex,
                                                                                      collectedCount);
    if (collectedCount <= 0) {
        return;
    }

    visibleCount = UI_GetCommunicatorVisibleLogCount(GetScreenWidth(), GetScreenHeight());
    if (game->ui.selectedLogIndex < game->ui.communicatorFirstVisibleLogIndex) {
        game->ui.communicatorFirstVisibleLogIndex = game->ui.selectedLogIndex;
    } else if (game->ui.selectedLogIndex >= game->ui.communicatorFirstVisibleLogIndex + visibleCount) {
        game->ui.communicatorFirstVisibleLogIndex = game->ui.selectedLogIndex - visibleCount + 1;
    }

    game->ui.communicatorFirstVisibleLogIndex = UI_ClampCommunicatorFirstVisibleLogIndex(GetScreenWidth(),
                                                                                      GetScreenHeight(),
                                                                                      game->ui.communicatorFirstVisibleLogIndex,
                                                                                      collectedCount);
}

static int CountShownMainStoryScenes(const Game *game) {
    int count = 0;

    if (game == NULL) {
        return 0;
    }

    for (int scene = STORY_SCENE_MAIN_AIR_FOR_ONE_MORE_DAY;
         scene < STORY_SCENE_MAIN_AIR_FOR_ONE_MORE_DAY + STORY_MAIN_SCENE_COUNT;
         ++scene) {
        if (game->flow.storySceneShown[scene]) {
            count++;
        }
    }

    return count;
}

static void EnsureCommunicatorStorySelectionVisible(Game *game, int shownCount) {
    int visibleCount;

    if (game == NULL) {
        return;
    }

    game->ui.communicatorFirstVisibleStorySceneIndex = UI_ClampCommunicatorFirstVisibleLogIndex(GetScreenWidth(),
                                                                                             GetScreenHeight(),
                                                                                             game->ui.communicatorFirstVisibleStorySceneIndex,
                                                                                             shownCount);
    if (shownCount <= 0) {
        return;
    }

    visibleCount = UI_GetCommunicatorVisibleLogCount(GetScreenWidth(), GetScreenHeight());
    if (game->ui.selectedStorySceneIndex < game->ui.communicatorFirstVisibleStorySceneIndex) {
        game->ui.communicatorFirstVisibleStorySceneIndex = game->ui.selectedStorySceneIndex;
    } else if (game->ui.selectedStorySceneIndex >= game->ui.communicatorFirstVisibleStorySceneIndex + visibleCount) {
        game->ui.communicatorFirstVisibleStorySceneIndex = game->ui.selectedStorySceneIndex - visibleCount + 1;
    }

    game->ui.communicatorFirstVisibleStorySceneIndex = UI_ClampCommunicatorFirstVisibleLogIndex(GetScreenWidth(),
                                                                                             GetScreenHeight(),
                                                                                             game->ui.communicatorFirstVisibleStorySceneIndex,
                                                                                             shownCount);
}

void GameOverlay_UpdateCommunicator(Game *game, bool *open, KeyboardKey alternateKey) {
    static constexpr float kCommunicatorDetailFadeDuration = 0.16f;
    Vector2 mouse;
    Rectangle contentPanel;
    int collectedCount;
    int shownStoryCount;
    int tabIndex;
    int firstVisibleLog;
    int visibleCount;
    int drawCount;
    int visibleIndex;
    float wheelMove;
    float detailFadeStep;
    float detailScrollStep;
    bool detailVisible;

    detailFadeStep = GetFrameTime() / kCommunicatorDetailFadeDuration;
    detailScrollStep = 80.0f * UIRuntime_GetScale(GetScreenWidth(), GetScreenHeight());
    contentPanel = UI_GetCommunicatorLogContentRect(GetScreenWidth(), GetScreenHeight());
    game->ui.communicatorLogDetailVisibility = ClampUnit(
        game->ui.communicatorLogDetailVisibility + (game->ui.communicatorLogDetailOpen ? detailFadeStep : -detailFadeStep));
    detailVisible = game->ui.communicatorLogDetailOpen || game->ui.communicatorLogDetailVisibility > 0.001f;

    if (detailVisible && (IsKeyPressed(KEY_ESCAPE) || GameOverlay_IsConfirmPressed())) {
        GameOverlay_ResetCommunicatorDetailState(game);
        Audio_PlayCue(&game->services.audio, AUDIO_CUE_CLOSE);
        return;
    }

    if (GameOverlay_TryCloseOverlay(game, open, alternateKey)) {
        GameOverlay_ResetCommunicatorDetailState(game);
        return;
    }

    collectedCount = Tasks_GetCollectedLogCount(&game->runtime.tasks);
    shownStoryCount = CountShownMainStoryScenes(game);
    if (game->ui.communicatorTab < COMMUNICATOR_TAB_TASKS || game->ui.communicatorTab >= COMMUNICATOR_TAB_COUNT) {
        game->ui.communicatorTab = COMMUNICATOR_TAB_TASKS;
    }
    if (game->ui.selectedLogIndex < 0) {
        game->ui.selectedLogIndex = 0;
    }
    if (collectedCount > 0 && game->ui.selectedLogIndex >= collectedCount) {
        game->ui.selectedLogIndex = collectedCount - 1;
    }
    game->ui.communicatorFirstVisibleLogIndex = UI_ClampCommunicatorFirstVisibleLogIndex(GetScreenWidth(),
                                                                                      GetScreenHeight(),
                                                                                      game->ui.communicatorFirstVisibleLogIndex,
                                                                                      collectedCount);
    if (game->ui.selectedStorySceneIndex < 0) {
        game->ui.selectedStorySceneIndex = 0;
    }
    if (shownStoryCount > 0 && game->ui.selectedStorySceneIndex >= shownStoryCount) {
        game->ui.selectedStorySceneIndex = shownStoryCount - 1;
    }
    game->ui.communicatorFirstVisibleStorySceneIndex = UI_ClampCommunicatorFirstVisibleLogIndex(GetScreenWidth(),
                                                                                             GetScreenHeight(),
                                                                                             game->ui.communicatorFirstVisibleStorySceneIndex,
                                                                                             shownStoryCount);

    if (IsKeyPressed(KEY_TAB)
        || IsKeyPressed(KEY_LEFT)
        || IsKeyPressed(KEY_A)
        || IsKeyPressed(KEY_RIGHT)
        || IsKeyPressed(KEY_D)) {
        int nextTab = (int)game->ui.communicatorTab;

        if (IsKeyPressed(KEY_LEFT) || IsKeyPressed(KEY_A)) {
            nextTab = (nextTab + COMMUNICATOR_TAB_COUNT - 1) % COMMUNICATOR_TAB_COUNT;
        } else {
            nextTab = (nextTab + 1) % COMMUNICATOR_TAB_COUNT;
        }

        game->ui.communicatorTab = (CommunicatorTab)nextTab;
        GameOverlay_ResetCommunicatorDetailState(game);
        Audio_PlayCue(&game->services.audio, AUDIO_CUE_CONFIRM);
        return;
    }

    if (game->ui.communicatorTab == COMMUNICATOR_TAB_TASKS) {
        if (GameOverlay_TryGetPrimaryClickPosition(&mouse)) {
            tabIndex = GameOverlay_FindClickedIndexedRect(mouse, COMMUNICATOR_TAB_COUNT, UI_GetCommunicatorTabRect);
            if (tabIndex >= 0) {
                game->ui.communicatorTab = (CommunicatorTab)tabIndex;
                GameOverlay_ResetCommunicatorDetailState(game);
                Audio_PlayCue(&game->services.audio, AUDIO_CUE_CONFIRM);
            }
        }
        return;
    }

    if (detailVisible) {
        if (IsKeyPressed(KEY_UP) || IsKeyPressed(KEY_W)) {
            ScrollCommunicatorDetail(game, -detailScrollStep);
            return;
        }
        if (IsKeyPressed(KEY_DOWN) || IsKeyPressed(KEY_S) || IsKeyPressed(KEY_SPACE)) {
            ScrollCommunicatorDetail(game, detailScrollStep);
            return;
        }
    }

    if (IsKeyPressed(KEY_UP) || IsKeyPressed(KEY_W)) {
        if (game->ui.communicatorTab == COMMUNICATOR_TAB_LOGS && game->ui.selectedLogIndex > 0) {
            game->ui.selectedLogIndex--;
            GameOverlay_ResetCommunicatorDetailState(game);
            EnsureCommunicatorSelectionVisible(game, collectedCount);
            Audio_PlayCue(&game->services.audio, AUDIO_CUE_CONFIRM);
        } else if (game->ui.communicatorTab == COMMUNICATOR_TAB_STORY && game->ui.selectedStorySceneIndex > 0) {
            game->ui.selectedStorySceneIndex--;
            GameOverlay_ResetCommunicatorDetailState(game);
            EnsureCommunicatorStorySelectionVisible(game, shownStoryCount);
            Audio_PlayCue(&game->services.audio, AUDIO_CUE_CONFIRM);
        }
        return;
    }

    if (IsKeyPressed(KEY_DOWN) || IsKeyPressed(KEY_S)) {
        if (game->ui.communicatorTab == COMMUNICATOR_TAB_LOGS && game->ui.selectedLogIndex + 1 < collectedCount) {
            game->ui.selectedLogIndex++;
            GameOverlay_ResetCommunicatorDetailState(game);
            EnsureCommunicatorSelectionVisible(game, collectedCount);
            Audio_PlayCue(&game->services.audio, AUDIO_CUE_CONFIRM);
        } else if (game->ui.communicatorTab == COMMUNICATOR_TAB_STORY && game->ui.selectedStorySceneIndex + 1 < shownStoryCount) {
            game->ui.selectedStorySceneIndex++;
            GameOverlay_ResetCommunicatorDetailState(game);
            EnsureCommunicatorStorySelectionVisible(game, shownStoryCount);
            Audio_PlayCue(&game->services.audio, AUDIO_CUE_CONFIRM);
        }
        return;
    }

    wheelMove = GetMouseWheelMove();
    if (wheelMove != 0.0f) {
        Vector2 mousePosition = GetMousePosition();

        if (detailVisible && CheckCollisionPointRec(mousePosition, contentPanel)) {
            ScrollCommunicatorDetail(game, wheelMove > 0.0f ? -detailScrollStep : detailScrollStep);
            return;
        }

        if (CheckCollisionPointRec(mousePosition, UI_GetCommunicatorLogListRect(GetScreenWidth(), GetScreenHeight()))) {
            if (game->ui.communicatorTab == COMMUNICATOR_TAB_LOGS) {
                int previousFirstVisibleLogIndex = game->ui.communicatorFirstVisibleLogIndex;

                if (wheelMove > 0.0f) {
                    game->ui.communicatorFirstVisibleLogIndex--;
                } else if (wheelMove < 0.0f) {
                    game->ui.communicatorFirstVisibleLogIndex++;
                }

                game->ui.communicatorFirstVisibleLogIndex = UI_ClampCommunicatorFirstVisibleLogIndex(GetScreenWidth(),
                                                                                                  GetScreenHeight(),
                                                                                                  game->ui.communicatorFirstVisibleLogIndex,
                                                                                                  collectedCount);
                if (game->ui.communicatorFirstVisibleLogIndex != previousFirstVisibleLogIndex) {
                    Audio_PlayCue(&game->services.audio, AUDIO_CUE_CONFIRM);
                }
                return;
            }

            if (game->ui.communicatorTab == COMMUNICATOR_TAB_STORY) {
                int previousFirstVisibleStoryIndex = game->ui.communicatorFirstVisibleStorySceneIndex;

                if (wheelMove > 0.0f) {
                    game->ui.communicatorFirstVisibleStorySceneIndex--;
                } else if (wheelMove < 0.0f) {
                    game->ui.communicatorFirstVisibleStorySceneIndex++;
                }

                game->ui.communicatorFirstVisibleStorySceneIndex = UI_ClampCommunicatorFirstVisibleLogIndex(GetScreenWidth(),
                                                                                                         GetScreenHeight(),
                                                                                                         game->ui.communicatorFirstVisibleStorySceneIndex,
                                                                                                         shownStoryCount);
                if (game->ui.communicatorFirstVisibleStorySceneIndex != previousFirstVisibleStoryIndex) {
                    Audio_PlayCue(&game->services.audio, AUDIO_CUE_CONFIRM);
                }
                return;
            }
        }
    }

    if ((GameOverlay_IsConfirmPressed() || IsKeyPressed(KEY_SPACE)) && !detailVisible) {
        game->ui.communicatorLogDetailOpen = true;
        game->ui.communicatorLogDetailScroll = 0.0f;
        Audio_PlayCue(&game->services.audio, AUDIO_CUE_OPEN);
        return;
    }

    if (!GameOverlay_TryGetPrimaryClickPosition(&mouse)) {
        return;
    }

    tabIndex = GameOverlay_FindClickedIndexedRect(mouse, COMMUNICATOR_TAB_COUNT, UI_GetCommunicatorTabRect);
    if (tabIndex >= 0) {
        game->ui.communicatorTab = (CommunicatorTab)tabIndex;
        GameOverlay_ResetCommunicatorDetailState(game);
        Audio_PlayCue(&game->services.audio, AUDIO_CUE_CONFIRM);
        return;
    }

    if (game->ui.communicatorTab == COMMUNICATOR_TAB_LOGS && collectedCount <= 0) {
        return;
    }
    if (game->ui.communicatorTab == COMMUNICATOR_TAB_STORY && shownStoryCount <= 0) {
        return;
    }

    if (detailVisible) {
        GameOverlay_ResetCommunicatorDetailState(game);
        Audio_PlayCue(&game->services.audio, AUDIO_CUE_CLOSE);
        return;
    }

    if (CheckCollisionPointRec(mouse, contentPanel)) {
        game->ui.communicatorLogDetailOpen = true;
        game->ui.communicatorLogDetailScroll = 0.0f;
        Audio_PlayCue(&game->services.audio, AUDIO_CUE_OPEN);
        return;
    }

    firstVisibleLog = game->ui.communicatorTab == COMMUNICATOR_TAB_STORY
        ? game->ui.communicatorFirstVisibleStorySceneIndex
        : game->ui.communicatorFirstVisibleLogIndex;
    visibleCount = UI_GetCommunicatorVisibleLogCount(GetScreenWidth(), GetScreenHeight());
    drawCount = (game->ui.communicatorTab == COMMUNICATOR_TAB_STORY ? shownStoryCount : collectedCount) - firstVisibleLog;
    if (drawCount > visibleCount) {
        drawCount = visibleCount;
    }

    for (visibleIndex = 0; visibleIndex < drawCount; ++visibleIndex) {
        if (CheckCollisionPointRec(mouse, UI_GetCommunicatorVisibleLogEntryRect(GetScreenWidth(), GetScreenHeight(), visibleIndex))) {
            if (game->ui.communicatorTab == COMMUNICATOR_TAB_STORY) {
                game->ui.selectedStorySceneIndex = firstVisibleLog + visibleIndex;
            } else {
                game->ui.selectedLogIndex = firstVisibleLog + visibleIndex;
            }
            GameOverlay_ResetCommunicatorDetailState(game);
            Audio_PlayCue(&game->services.audio, AUDIO_CUE_CONFIRM);
            return;
        }
    }
}
