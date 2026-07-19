#include "game_manager_internal.h"
#include "map_render.h"
#include "task_presentation.h"

#include <cmath>
#include <cstring>

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

static const MapPortal *GetNearbyPortal(const Game *game) {
    if (game == nullptr || game->flow.state != GAME_STATE_PLAYING || game->flow.screenTransitionActive) {
        return nullptr;
    }
    return Map_FindPortalNear(&game->runtime.map, game->runtime.player.gridX, game->runtime.player.gridY);
}

static const char *GetPortalPrompt(const MapPortal *portal) {
    if (portal == nullptr) {
        return "";
    }
    if (std::strcmp(portal->promptKey, "EXIT_SHIP") == 0) {
        return Loc_PickLiteral("F  EXIT SHIP", "F  离开飞船");
    }
    if (std::strcmp(portal->promptKey, "ENTER_SHIP") == 0) {
        return Loc_PickLiteral("F  ENTER SHIP", "F  进入飞船");
    }
    return Loc_PickLiteral("F  USE PORTAL", "F  使用入口");
}

static void DrawNearbyPortalMarker(const Game *game) {
    const MapPortal *portal;
    Vector2 center;
    float pulse;

    portal = GetNearbyPortal(game);
    if (portal == nullptr) {
        return;
    }
    center = Vector2{
        (portal->gridX + portal->width * 0.5f) * TILE_SIZE,
        (portal->gridY + portal->height * 0.5f) * TILE_SIZE
    };
    pulse = 0.5f + 0.5f * std::sin(game->runtime.elapsedSeconds * 5.0f);
    DrawRing(center,
             TileScale(20.0f),
             TileScale(28.0f + pulse * 4.0f),
             0.0f,
             360.0f,
             36,
             Color{92, 225, 255, static_cast<unsigned char>(100 + pulse * 90.0f)});
    DrawCircleLines((int)center.x, (int)center.y, TileScale(18.0f), Color{210, 249, 255, 210});
}

static void DrawPortalInteractionPrompt(const Game *game, int screenWidth, int screenHeight) {
    const MapPortal *portal;
    const char *text;
    Font font;
    float fontSize;
    Vector2 textSize;
    Rectangle panel;

    portal = GetNearbyPortal(game);
    if (portal == nullptr || game->ui.pauseMenuOpen || game->ui.infoOverlayOpen || game->ui.craftOpen || game->ui.helpOpen) {
        return;
    }
    text = GetPortalPrompt(portal);
    font = game->services.assets.uiFontLoaded ? game->services.assets.uiFont : GetFontDefault();
    fontSize = 24.0f;
    textSize = MeasureTextEx(font, text, fontSize, 1.0f);
    panel = Rectangle{
        (screenWidth - textSize.x) * 0.5f - 24.0f,
        screenHeight - 112.0f,
        textSize.x + 48.0f,
        textSize.y + 22.0f
    };
    DrawRectangleRounded(panel, 0.32f, 10, Color{9, 17, 28, 225});
    DrawRectangleRoundedLines(panel, 0.32f, 10, Color{91, 215, 242, 220});
    DrawTextEx(font,
               text,
               Vector2{panel.x + 24.0f, panel.y + 11.0f},
               fontSize,
               1.0f,
               Color{224, 250, 255, 255});
}

static void DrawObjectiveMarker(const TaskSystem *tasks, const GameMap *map, const Player *player, float elapsedSeconds) {
    int markerX;
    int markerY;

    if (Tasks_GetObjectiveMarkerForMap(tasks, map, player, &markerX, &markerY)) {
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

    if (game == nullptr || game->runtime.laserEffectTimer <= 0.0f) {
        return;
    }

    progress = ClampUnit(game->runtime.laserEffectTimer / kLaserEffectDuration);
    alpha = 255.0f * progress;
    beamWidth = TileScale(7.0f + 5.0f * progress);
    DrawLineEx(game->runtime.laserEffectStart,
               game->runtime.laserEffectEnd,
               beamWidth,
               Color{93, 222, 255, static_cast<unsigned char>(alpha)});
    DrawLineEx(game->runtime.laserEffectStart,
               game->runtime.laserEffectEnd,
               TileScale(2.2f),
               Color{246, 255, 255, static_cast<unsigned char>(210.0f * progress)});

    impact = game->runtime.laserEffectEnd;
    if (game->runtime.laserEffectHit) {
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

    if (game->runtime.map.loadFailed) {
        const char *title = "MAP LOAD ERROR";
        const char *detail = Map_GetLoadError(&game->runtime.map);
        const int titleSize = 34;
        const int detailSize = 20;

        DrawRectangle(0, 0, screenWidth, screenHeight, Color{18, 8, 10, 255});
        DrawText(title,
                 (screenWidth - MeasureText(title, titleSize)) / 2,
                 screenHeight / 2 - 64,
                 titleSize,
                 Color{255, 118, 118, 255});
        DrawText(detail,
                 (screenWidth - MeasureText(detail, detailSize)) / 2,
                 screenHeight / 2,
                 detailSize,
                 Color{238, 214, 214, 255});
        DrawText("The production map must be repaired in maps/world.tmj.",
                 (screenWidth - MeasureText("The production map must be repaired in maps/world.tmj.", 18)) / 2,
                 screenHeight / 2 + 42,
                 18,
                 Color{194, 174, 174, 255});
        EndDrawing();
        return;
    }

    if (game->flow.state == GAME_STATE_INTRO) {
        if (!game->account.authenticated) {
            UI_DrawAuthScreen(&game->services.assets,
                              game->account.authMode,
                              game->account.authUsername,
                              game->account.authPassword,
                              game->account.authPasswordVisible,
                              game->account.authSelectedField,
                              game->account.authMessage,
                              game->account.authHasAccounts,
                              screenWidth,
                              screenHeight,
                              game->runtime.elapsedSeconds);
            if (game->account.authAccountPickerOpen) {
                UI_DrawAuthAccountPickerPopup(&game->services.assets,
                                              game->account.authAccountNames,
                                              game->account.authAccountCount,
                                              game->account.authAccountPickerSelection,
                                              game->account.authAccountPickerFirstVisible,
                                              screenWidth,
                                              screenHeight);
            }
            if (game->account.accountDeleteConfirmOpen) {
                UI_DrawAccountDeleteConfirmPopup(&game->services.assets,
                                                game->account.accountDeleteTargetName,
                                                screenWidth,
                                                screenHeight,
                                                game->account.accountDeleteConfirmSelection);
            }
        } else {
            UI_DrawMainMenu(&game->services.assets,
                            game->account.hasSaveFile,
                            game->account.saveSlotCount,
                            SaveSystem_GetActiveAccountName(),
                            game->account.hasAccountBestScore,
                            game->account.accountBestScore,
                            screenWidth,
                            screenHeight,
                            game->runtime.elapsedSeconds);
            if (game->ui.savePanelOpen) {
                UI_DrawSaveSlotsOverlay(&game->services.assets,
                                        game->account.saveSlots,
                                        SAVE_SLOT_COUNT,
                                        game->ui.savePanelMode,
                                        game->ui.selectedSaveSlot,
                                        SaveSystem_GetActiveAccountName(),
                                        screenWidth,
                                        screenHeight);
            }
            if (game->ui.settingsOpen) {
                UI_DrawSettingsOverlay(&game->services.assets,
                                       &game->services.settings,
                                       SaveSystem_GetActiveAccountName(),
                                       game->account.saveSlotCount,
                                       true,
                                       screenWidth,
                                       screenHeight);
            }
            if (game->account.accountDeleteConfirmOpen) {
                UI_DrawAccountDeleteConfirmPopup(&game->services.assets,
                                                game->account.accountDeleteTargetName,
                                                screenWidth,
                                                screenHeight,
                                                game->account.accountDeleteConfirmSelection);
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

    if (game->flow.state == GAME_STATE_OPENING) {
        if (game->flow.openingAwaitingFirstAdvance) {
            static constexpr float kOpeningInitialBlackHoldDuration = 1.5f;

            UI_DrawOpeningStandby(&game->services.assets,
                                  game->flow.openingCutsceneElapsed >= kOpeningInitialBlackHoldDuration,
                                  screenWidth,
                                  screenHeight,
                                  game->runtime.elapsedSeconds);
        } else {
            UI_DrawOpeningCutscene(&game->services.assets, game->flow.openingSlideIndex, game->flow.openingCutsceneElapsed, screenWidth, screenHeight);
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

    if (game->flow.storySceneOpen) {
        UI_DrawStoryScene(&game->services.assets, game->flow.storyScene, game->flow.storySceneElapsed, screenWidth, screenHeight);
        {
            const float overlayAlpha = Game_GetNarrativeOverlayAlpha(game);
            if (overlayAlpha > 0.0f) {
                DrawRectangle(0, 0, screenWidth, screenHeight, Fade(BLACK, overlayAlpha));
            }
        }
        EndDrawing();
        return;
    }

    BeginMode2D(game->runtime.camera);
    Map_Draw(&game->runtime.map, &game->services.assets, game->runtime.camera, screenWidth, screenHeight, game->runtime.elapsedSeconds);
    DrawNearbyPortalMarker(game);
    DrawObjectiveMarker(&game->runtime.tasks, &game->runtime.map, &game->runtime.player, game->runtime.elapsedSeconds);
    Tasks_DrawWorld(&game->runtime.tasks, &game->services.assets, game->runtime.elapsedSeconds);
    DrawPlayer(&game->runtime.player, &game->services.assets, game->runtime.elapsedSeconds);
    DrawLaserEffect(game);
    EndMode2D();

    if (game->flow.state == GAME_STATE_ENDING) {
        UI_DrawEnding(game->runtime.tasks.ending, &game->runtime.player, &game->runtime.tasks, &game->services.assets, screenWidth, screenHeight, game->runtime.elapsedSeconds, game->ui.endingMenuSelection);
    } else if (game->ui.settlementConfirmOpen) {
        UI_DrawHud(&game->runtime.player, &game->runtime.tasks, &game->ui.hudMessage, &game->services.assets, screenWidth, screenHeight);
        UI_DrawSettlementConfirmPopup(&game->services.assets,
                                      &game->runtime.player,
                                      &game->runtime.tasks,
                                      screenWidth,
                                      screenHeight,
                                      Tasks_GetAvailableEndingCount(&game->runtime.tasks) > 1
                                          ? Tasks_GetAvailableEndingCount(&game->runtime.tasks) + 1
                                          : 2,
                                      game->ui.settlementConfirmSelection);
    } else if (game->ui.endingRouteDoubleConfirmOpen) {
        UI_DrawHud(&game->runtime.player, &game->runtime.tasks, &game->ui.hudMessage, &game->services.assets, screenWidth, screenHeight);
        UI_DrawEndingRouteConfirmPopup(&game->services.assets,
                                       game->ui.endingRoutePendingConfirm,
                                       screenWidth,
                                       screenHeight,
                                       game->ui.endingRouteDoubleConfirmSelection);
    } else if (game->ui.showDeathPopup) {
        UI_DrawDeathPopup(&game->runtime.player, game->account.hasSaveFile, &game->services.assets, screenWidth, screenHeight, game->ui.deathPopupSelection);
    } else if (game->ui.savePanelOpen) {
        UI_DrawSaveSlotsOverlay(&game->services.assets,
                                game->account.saveSlots,
                                SAVE_SLOT_COUNT,
                                game->ui.savePanelMode,
                                game->ui.selectedSaveSlot,
                                SaveSystem_GetActiveAccountName(),
                                screenWidth,
                                screenHeight);
    } else if (game->ui.pauseMenuOpen) {
        UI_DrawPauseMenu(&game->services.assets, screenWidth, screenHeight);
    } else {
        UI_DrawHud(&game->runtime.player, &game->runtime.tasks, &game->ui.hudMessage, &game->services.assets, screenWidth, screenHeight);
        if (game->ui.craftOpen) {
            UI_DrawCraftOverlay(&game->services.assets, &game->runtime.tasks, &game->runtime.player, game->ui.selectedCraftIndex, screenWidth, screenHeight);
        }
        if (game->ui.infoOverlayOpen) {
            UI_DrawInfoOverlay(&game->services.assets,
                               game->ui.infoOverlayTab,
                               &game->services.settings,
                               &game->runtime.miniMap,
                               &game->runtime.player,
                               &game->runtime.tasks,
                               &game->runtime.map,
                               game->flow.storySceneShown,
                               game->account.saveSlotCount,
                               game->ui.selectedBackpackItem,
                               game->ui.communicatorTab,
                               game->ui.selectedLogIndex,
                               game->ui.communicatorFirstVisibleLogIndex,
                               game->ui.selectedStorySceneIndex,
                               game->ui.communicatorFirstVisibleStorySceneIndex,
                               game->ui.communicatorLogDetailVisibility,
                               game->ui.communicatorLogDetailScroll,
                               screenWidth,
                               screenHeight);
        }
        if (game->ui.helpOpen) {
            UI_DrawHelpOverlay(&game->services.assets, screenWidth, screenHeight);
        }
    }

    DrawPortalInteractionPrompt(game, screenWidth, screenHeight);

    {
        const float overlayAlpha = Game_GetScreenTransitionAlpha(game);
        if (overlayAlpha > 0.0f) {
            DrawRectangle(0, 0, screenWidth, screenHeight, Fade(BLACK, overlayAlpha));
            if (game->flow.screenTransitionAction == SCREEN_TRANSITION_MAP_CHANGE && overlayAlpha > 0.72f) {
                const char *title = std::strcmp(game->services.mapScene.transitionId, "enter_ship") == 0
                    ? Loc_PickLiteral("RETURNING TO SHIP", "正在返回飞船")
                    : Loc_PickLiteral("CYCLING OUTER AIRLOCK", "正在执行出舱循环");
                const char *detail = game->services.mapScene.phase == MAP_TRANSITION_ERROR
                    ? MapSceneManager_GetError(&game->services.mapScene)
                    : Loc_PickLiteral("Pressure seal stable // loading destination map", "压力密封稳定 // 正在加载目标地图");
                int titleWidth = MeasureText(title, 28);
                int detailWidth = MeasureText(detail, 18);
                DrawText(title, (screenWidth - titleWidth) / 2, screenHeight / 2 - 22, 28, Fade(RAYWHITE, overlayAlpha));
                DrawText(detail, (screenWidth - detailWidth) / 2, screenHeight / 2 + 18, 18,
                         Fade(game->services.mapScene.phase == MAP_TRANSITION_ERROR ? RED : SKYBLUE, overlayAlpha));
            }
        }
    }

    EndDrawing();
}
