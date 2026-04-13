#include "game_manager_internal.h"

#include <cmath>

static float TileScale(float value) {
    return value * (static_cast<float>(TILE_SIZE) / 64.0f);
}

static bool IsFiniteVector(Vector2 value) {
    return std::isfinite(value.x) && std::isfinite(value.y);
}

static void SanitizeGameplayFrameState(Game *game, int screenWidth, int screenHeight) {
    bool playerTileInvalid;

    playerTileInvalid = !Map_IsWithinBounds(game->player.gridX, game->player.gridY)
        || !Map_IsWalkable(&game->map, game->player.gridX, game->player.gridY);
    if (playerTileInvalid) {
        game->player.gridX = PLAYER_START_X;
        game->player.gridY = PLAYER_START_Y;
        game->player.facingX = 0;
        game->player.facingY = 1;
        Player_UpdateWorldPosition(&game->player);
        MiniMap_Update(&game->miniMap, &game->player, &game->map);
    } else if (!IsFiniteVector(game->player.worldPos)
               || !IsFiniteVector(game->player.renderPos)
               || std::fabs(game->player.worldPos.x - Map_GridToWorld(game->player.gridX, game->player.gridY).x) > 0.5f
               || std::fabs(game->player.worldPos.y - Map_GridToWorld(game->player.gridX, game->player.gridY).y) > 0.5f) {
        Player_UpdateWorldPosition(&game->player);
    }

    if (game->tasks.objective[0] == '\0') {
        Tasks_UpdateObjective(&game->tasks, &game->player);
    }

    if (!std::isfinite(game->camera.zoom) || game->camera.zoom <= 0.0f) {
        game->camera.zoom = CAMERA_ZOOM;
    }
    if (!std::isfinite(game->camera.rotation)) {
        game->camera.rotation = 0.0f;
    }
    if (!IsFiniteVector(game->camera.target)) {
        game->camera.target = game->player.renderPos;
    }
    if (!IsFiniteVector(game->camera.offset)
        || std::fabs(game->camera.offset.x) < 0.5f
        || std::fabs(game->camera.offset.y) < 0.5f) {
        game->camera.offset = Vector2{screenWidth * 0.5f, screenHeight * 0.5f};
    }
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

    DrawCircleV(Vector2{player->renderPos.x, player->renderPos.y - TileScale(10.0f)}, TileScale(13.0f), player->crouching ? Color{155, 209, 236, 255} : Color{185, 225, 255, 255});
    DrawCircleV(Vector2{player->renderPos.x, player->renderPos.y - TileScale(24.0f)}, TileScale(8.0f), Color{185, 225, 255, 255});
    DrawEllipse((int)player->renderPos.x, (int)(player->renderPos.y - TileScale(24.0f)), TileScale(6.0f) + std::sin(elapsedSeconds * 4.0f) * TileScale(0.5f), TileScale(3.5f), Color{98, 201, 255, 255});
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
                UI_DrawSettingsOverlay(&game->assets, &game->settings, screenWidth, screenHeight);
            }
            if (game->accountDeleteConfirmOpen) {
                UI_DrawAccountDeleteConfirmPopup(&game->assets,
                                                game->accountDeleteTargetName,
                                                screenWidth,
                                                screenHeight,
                                                game->accountDeleteConfirmSelection);
            }
        }
        EndDrawing();
        return;
    }

    if (game->state == GAME_STATE_OPENING) {
        UI_DrawOpeningCutscene(&game->assets, game->openingSlideIndex, game->openingCutsceneElapsed, screenWidth, screenHeight);
        EndDrawing();
        return;
    }

    if (game->storySceneOpen) {
        UI_DrawStoryScene(&game->assets, game->storyScene, game->storySceneElapsed, screenWidth, screenHeight);
        EndDrawing();
        return;
    }

    SanitizeGameplayFrameState(game, screenWidth, screenHeight);

    BeginMode2D(game->camera);
    Map_Draw(&game->map, &game->assets, game->camera, screenWidth, screenHeight, game->elapsedSeconds);
    DrawObjectiveMarker(&game->tasks, &game->player, game->elapsedSeconds);
    Tasks_DrawWorld(&game->tasks, &game->assets, game->elapsedSeconds);
    DrawPlayer(&game->player, &game->assets, game->elapsedSeconds);
    EndMode2D();

    if (game->state == GAME_STATE_ENDING) {
        UI_DrawEnding(game->tasks.ending, &game->player, &game->tasks, &game->assets, screenWidth, screenHeight, game->elapsedSeconds);
    } else if (game->settlementConfirmOpen) {
        UI_DrawHud(&game->player, &game->tasks, &game->hudMessage, &game->assets, screenWidth, screenHeight);
        UI_DrawSettlementConfirmPopup(&game->assets, screenWidth, screenHeight, game->settlementConfirmSelection);
    } else if (game->showDeathPopup) {
        UI_DrawDeathPopup(&game->player, &game->assets, screenWidth, screenHeight, game->deathPopupSelection);
    } else if (game->logReaderOpen) {
        UI_DrawLogReader(&game->tasks, game->selectedLogIndex, &game->assets, screenWidth, screenHeight);
    } else if (game->settingsOpen) {
        UI_DrawSettingsOverlay(&game->assets, &game->settings, screenWidth, screenHeight);
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
        if (game->mapOpen) {
            UI_DrawMapOverlay(&game->assets, &game->miniMap, &game->player, &game->tasks, &game->map, screenWidth, screenHeight);
        }
        if (game->backpackOpen) {
            UI_DrawBackpackOverlay(&game->assets, &game->player, game->selectedBackpackItem, screenWidth, screenHeight);
        }
        if (game->communicatorOpen) {
            UI_DrawCommunicatorOverlay(&game->assets, &game->tasks, screenWidth, screenHeight);
        }
        if (game->helpOpen) {
            UI_DrawHelpOverlay(&game->assets, screenWidth, screenHeight);
        }
    }

    EndDrawing();
}
