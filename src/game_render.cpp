#include "game_manager_internal.h"

#include <cmath>

static float ClampUnit(float value) {
    if (value < 0.0f) {
        return 0.0f;
    }
    if (value > 1.0f) {
        return 1.0f;
    }
    return value;
}

static float TileScale(float value) {
    return value * (static_cast<float>(TILE_SIZE) / 64.0f);
}

static void DrawObjectiveMarker(const TaskSystem *tasks, const Player *player, float elapsedSeconds) {
    int markerX;
    int markerY;

    if (Tasks_GetObjectiveMarker(tasks, player, &markerX, &markerY)) {
        Vector2 position;
        float outerRadius;

        position = Map_GridToWorld(markerX, markerY);
        outerRadius = TileScale(22.0f) + std::sin(elapsedSeconds * 5.0f) * TileScale(2.0f);
        DrawRing(position, TileScale(14.0f), outerRadius, 0.0f, 360.0f, 32, Color{106, 227, 255, 90});
        DrawCircleLines((int)position.x, (int)position.y, TileScale(16.0f), Color{176, 238, 255, 140});
    }
}

static void DrawPlayer(const Player *player, const AssetBundle *assets, float elapsedSeconds) {
    if (assets->player.loaded) {
        Rectangle source;
        Rectangle dest;
        float drawWidth;
        float drawHeight;

        if (assets->playerIsSpriteSheet) {
            int rowIndex;
            int frameIndex;
            int frameSlot;
            float sourceWidth;
            float sourceHeight;
            float maxDrawWidth;
            float targetDrawHeight;
            float aspect;

            if (player->facingY > 0) {
                rowIndex = 0;
            } else if (player->facingX < 0) {
                rowIndex = 1;
            } else if (player->facingX > 0) {
                rowIndex = 2;
            } else {
                rowIndex = 3;
            }

            if (Player_IsMoveAnimating(player)) {
                frameIndex = 1 + (static_cast<int>(player->spriteAnimTimer * PLAYER_SPRITE_ANIM_FPS) % (PLAYER_SPRITE_SHEET_COLS - 1));
            } else {
                frameIndex = 0;
            }

            frameSlot = rowIndex * PLAYER_SPRITE_SHEET_COLS + frameIndex;
            source = assets->playerSpriteFrames[frameSlot];
            sourceWidth = source.width;
            sourceHeight = source.height;
            aspect = sourceHeight > 0.0f ? sourceWidth / sourceHeight : 1.0f;
            targetDrawHeight = TileScale(86.0f);
            maxDrawWidth = TileScale(60.0f);
            drawHeight = targetDrawHeight;
            drawWidth = drawHeight * aspect;
            if (drawWidth > maxDrawWidth) {
                drawWidth = maxDrawWidth;
                drawHeight = drawWidth / aspect;
            }
            dest = Rectangle{player->renderPos.x - drawWidth * 0.5f, player->renderPos.y - drawHeight + TileScale(14.0f), drawWidth, drawHeight};
        } else {
            source = Rectangle{0.0f, 0.0f, static_cast<float>(assets->player.texture.width), static_cast<float>(assets->player.texture.height)};
            drawWidth = TileScale(54.0f);
            drawHeight = drawWidth * (static_cast<float>(assets->player.texture.height) / static_cast<float>(assets->player.texture.width));
            dest = Rectangle{player->renderPos.x - drawWidth * 0.5f, player->renderPos.y - drawHeight + TileScale(14.0f), drawWidth, drawHeight};
        }

        DrawTexturePro(assets->player.texture, source, dest, Vector2{0.0f, 0.0f}, 0.0f, WHITE);
        return;
    }

    DrawCircleV(Vector2{player->renderPos.x, player->renderPos.y - TileScale(10.0f)}, TileScale(13.0f), Color{185, 225, 255, 255});
    DrawCircleV(Vector2{player->renderPos.x, player->renderPos.y - TileScale(24.0f)}, TileScale(8.0f), Color{185, 225, 255, 255});
    DrawEllipse((int)player->renderPos.x, (int)(player->renderPos.y - TileScale(24.0f)), TileScale(6.0f) + std::sin(elapsedSeconds * 4.0f) * TileScale(0.5f), TileScale(3.5f), Color{98, 201, 255, 255});
}

static void DrawLaserEffect(const Game *game) {
    static constexpr float kLaserEffectDuration = 0.18f;
    float progress;
    float alpha;
    float beamWidth;
    Vector2 impact;

    if (game == nullptr || game->laserEffectTimer <= 0.0f) {
        return;
    }

    progress = ClampUnit(game->laserEffectTimer / kLaserEffectDuration);
    alpha = 255.0f * progress;
    beamWidth = TileScale(7.0f + 5.0f * progress);
    DrawLineEx(game->laserEffectStart,
               game->laserEffectEnd,
               beamWidth,
               Color{93, 222, 255, static_cast<unsigned char>(alpha)});
    DrawLineEx(game->laserEffectStart,
               game->laserEffectEnd,
               TileScale(2.2f),
               Color{246, 255, 255, static_cast<unsigned char>(210.0f * progress)});

    impact = game->laserEffectEnd;
    if (game->laserEffectHit) {
        DrawRing(impact,
                 TileScale(5.0f + 8.0f * (1.0f - progress)),
                 TileScale(15.0f + 12.0f * (1.0f - progress)),
                 0.0f,
                 360.0f,
                 28,
                 Color{255, 238, 162, static_cast<unsigned char>(170.0f * progress)});
        DrawCircleV(impact, TileScale(4.0f + 5.0f * progress), Color{255, 255, 255, static_cast<unsigned char>(180.0f * progress)});
    }
}

void Game_Draw(Game *game) {
    int screenWidth;
    int screenHeight;

    screenWidth = GetScreenWidth();
    screenHeight = GetScreenHeight();

    BeginDrawing();
    ClearBackground(BLACK);

    if (game->state == GAME_STATE_INTRO) {
        if (!game->authenticated) {
            UI_DrawAuthScreen(&game->assets,
                              game->authMode,
                              game->authUsername,
                              game->authPassword,
                              game->authPasswordVisible,
                              game->authSelectedField,
                              game->authMessage,
                              game->authHasAccounts,
                              screenWidth,
                              screenHeight,
                              game->elapsedSeconds);
            if (game->accountDeleteConfirmOpen) {
                UI_DrawAccountDeleteConfirmPopup(&game->assets,
                                                game->accountDeleteTargetName,
                                                screenWidth,
                                                screenHeight,
                                                game->accountDeleteConfirmSelection);
            }
        } else {
            UI_DrawMainMenu(&game->assets,
                            game->hasSaveFile,
                            game->saveSlotCount,
                            SaveSystem_GetActiveAccountName(),
                            game->hasAccountBestScore,
                            game->accountBestScore,
                            screenWidth,
                            screenHeight,
                            game->elapsedSeconds);
            if (game->savePanelOpen) {
                UI_DrawSaveSlotsOverlay(&game->assets,
                                        game->saveSlots,
                                        SAVE_SLOT_COUNT,
                                        game->savePanelMode,
                                        game->selectedSaveSlot,
                                        SaveSystem_GetActiveAccountName(),
                                        screenWidth,
                                        screenHeight);
            }
            if (game->settingsOpen) {
                UI_DrawSettingsOverlay(&game->assets,
                                       &game->settings,
                                       SaveSystem_GetActiveAccountName(),
                                       game->saveSlotCount,
                                       true,
                                       screenWidth,
                                       screenHeight);
            }
            if (game->accountDeleteConfirmOpen) {
                UI_DrawAccountDeleteConfirmPopup(&game->assets,
                                                game->accountDeleteTargetName,
                                                screenWidth,
                                                screenHeight,
                                                game->accountDeleteConfirmSelection);
            }
        }
        {
            const float overlayAlpha = Game_GetScreenTransitionAlpha(game);
            if (overlayAlpha > 0.0f) {
                DrawRectangle(0, 0, screenWidth, screenHeight, Fade(BLACK, overlayAlpha));
            }
        }
        EndDrawing();
        return;
    }

    if (game->state == GAME_STATE_OPENING) {
        if (game->openingAwaitingFirstAdvance) {
            static constexpr float kOpeningInitialBlackHoldDuration = 1.5f;

            UI_DrawOpeningStandby(&game->assets,
                                  game->openingCutsceneElapsed >= kOpeningInitialBlackHoldDuration,
                                  screenWidth,
                                  screenHeight,
                                  game->elapsedSeconds);
        } else {
            UI_DrawOpeningCutscene(&game->assets, game->openingSlideIndex, game->openingCutsceneElapsed, screenWidth, screenHeight);
        }
        {
            const float overlayAlpha = Game_GetNarrativeOverlayAlpha(game);
            if (overlayAlpha > 0.0f) {
                DrawRectangle(0, 0, screenWidth, screenHeight, Fade(BLACK, overlayAlpha));
            }
        }
        EndDrawing();
        return;
    }

    if (game->storySceneOpen) {
        UI_DrawStoryScene(&game->assets, game->storyScene, game->storySceneElapsed, screenWidth, screenHeight);
        {
            const float overlayAlpha = Game_GetNarrativeOverlayAlpha(game);
            if (overlayAlpha > 0.0f) {
                DrawRectangle(0, 0, screenWidth, screenHeight, Fade(BLACK, overlayAlpha));
            }
        }
        EndDrawing();
        return;
    }

    BeginMode2D(game->camera);
    Map_Draw(&game->map, &game->assets, game->camera, screenWidth, screenHeight, game->elapsedSeconds);
    DrawObjectiveMarker(&game->tasks, &game->player, game->elapsedSeconds);
    Tasks_DrawWorld(&game->tasks, &game->assets, game->elapsedSeconds);
    DrawPlayer(&game->player, &game->assets, game->elapsedSeconds);
    DrawLaserEffect(game);
    EndMode2D();

    if (game->state == GAME_STATE_ENDING) {
        UI_DrawEnding(game->tasks.ending, &game->player, &game->tasks, &game->assets, screenWidth, screenHeight, game->elapsedSeconds, game->endingMenuSelection);
    } else if (game->settlementConfirmOpen) {
        UI_DrawHud(&game->player, &game->tasks, &game->hudMessage, &game->assets, screenWidth, screenHeight);
        UI_DrawSettlementConfirmPopup(&game->assets,
                                      &game->player,
                                      &game->tasks,
                                      screenWidth,
                                      screenHeight,
                                      Tasks_GetAvailableEndingCount(&game->tasks) > 1
                                          ? Tasks_GetAvailableEndingCount(&game->tasks) + 1
                                          : 2,
                                      game->settlementConfirmSelection);
    } else if (game->endingRouteDoubleConfirmOpen) {
        UI_DrawHud(&game->player, &game->tasks, &game->hudMessage, &game->assets, screenWidth, screenHeight);
        UI_DrawEndingRouteConfirmPopup(&game->assets,
                                       game->endingRoutePendingConfirm,
                                       screenWidth,
                                       screenHeight,
                                       game->endingRouteDoubleConfirmSelection);
    } else if (game->showDeathPopup) {
        UI_DrawDeathPopup(&game->player, game->hasSaveFile, &game->assets, screenWidth, screenHeight, game->deathPopupSelection);
    } else if (game->savePanelOpen) {
        UI_DrawSaveSlotsOverlay(&game->assets,
                                game->saveSlots,
                                SAVE_SLOT_COUNT,
                                game->savePanelMode,
                                game->selectedSaveSlot,
                                SaveSystem_GetActiveAccountName(),
                                screenWidth,
                                screenHeight);
    } else if (game->pauseMenuOpen) {
        UI_DrawPauseMenu(&game->assets, screenWidth, screenHeight);
    } else {
        UI_DrawHud(&game->player, &game->tasks, &game->hudMessage, &game->assets, screenWidth, screenHeight);
        if (game->craftOpen) {
            UI_DrawCraftOverlay(&game->assets, &game->tasks, &game->player, game->selectedCraftIndex, screenWidth, screenHeight);
        }
        if (game->infoOverlayOpen) {
            UI_DrawInfoOverlay(&game->assets,
                               game->infoOverlayTab,
                               &game->settings,
                               &game->miniMap,
                               &game->player,
                               &game->tasks,
                               &game->map,
                               game->storySceneShown,
                               game->saveSlotCount,
                               game->selectedBackpackItem,
                               game->communicatorTab,
                               game->selectedLogIndex,
                               game->communicatorFirstVisibleLogIndex,
                               game->selectedStorySceneIndex,
                               game->communicatorFirstVisibleStorySceneIndex,
                               game->communicatorLogDetailVisibility,
                               game->communicatorLogDetailScroll,
                               screenWidth,
                               screenHeight);
        }
        if (game->helpOpen) {
            UI_DrawHelpOverlay(&game->assets, screenWidth, screenHeight);
        }
    }

    {
        const float overlayAlpha = Game_GetScreenTransitionAlpha(game);
        if (overlayAlpha > 0.0f) {
            DrawRectangle(0, 0, screenWidth, screenHeight, Fade(BLACK, overlayAlpha));
        }
    }

    EndDrawing();
}
