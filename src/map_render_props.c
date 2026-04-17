#include "map_internal.h"

#include <math.h>

static float WaveValue(float t, float speed, float amount) {
    return sinf(t * speed) * amount;
}

static float TileScale(float value) {
    return value * ((float)TILE_SIZE / 64.0f);
}

static bool ResolvePropDrawRect(TileType tile, int gridX, int gridY, Rectangle *rect) {
    int originX;
    int originY;
    int width;
    int height;

    if (!MapInternal_GetMultiTilePropBounds(tile, gridX, gridY, &originX, &originY, &width, &height)) {
        return true;
    }

    if (gridX != originX || gridY != originY) {
        return false;
    }

    *rect = (Rectangle){
        originX * TILE_SIZE,
        originY * TILE_SIZE,
        width * TILE_SIZE,
        height * TILE_SIZE
    };
    return true;
}

static void DrawTileTexture(const TextureAsset *asset, Rectangle rect, Color tint) {
    Rectangle source;

    source = (Rectangle){0.0f, 0.0f, (float)asset->texture.width, (float)asset->texture.height};
    DrawTexturePro(asset->texture, source, rect, (Vector2){0.0f, 0.0f}, 0.0f, tint);
}

static void DrawTree(Rectangle rect, const AssetBundle *assets, float elapsedSeconds) {
    float centerX;
    float sway;

    if (assets->tileTree.loaded) {
        DrawTileTexture(&assets->tileTree, rect, WHITE);
        return;
    }

    centerX = rect.x + rect.width * 0.5f;
    sway = WaveValue(elapsedSeconds, 1.8f, TileScale(2.0f));
    DrawRectangle((int)(centerX - TileScale(4.0f)), (int)(rect.y + TileScale(34.0f)), (int)TileScale(8.0f), (int)TileScale(18.0f), (Color){86, 54, 31, 255});
    DrawCircle((int)(centerX + sway), (int)(rect.y + TileScale(26.0f)), TileScale(17.0f), (Color){38, 92, 69, 255});
    DrawCircle((int)(centerX - TileScale(10.0f) + sway * 0.6f), (int)(rect.y + TileScale(30.0f)), TileScale(12.0f), (Color){56, 122, 90, 255});
    DrawCircle((int)(centerX + TileScale(11.0f) + sway * 0.6f), (int)(rect.y + TileScale(31.0f)), TileScale(11.0f), (Color){56, 122, 90, 255});
}

static void DrawRock(Rectangle rect, const AssetBundle *assets) {
    if (assets->tileRock.loaded) {
        DrawTileTexture(&assets->tileRock, rect, WHITE);
        return;
    }

    DrawRectangleRec(rect, (Color){51, 63, 74, 255});
    DrawTriangle(
        (Vector2){rect.x + rect.width * 0.12f, rect.y + rect.height * 0.82f},
        (Vector2){rect.x + rect.width * 0.46f, rect.y + rect.height * 0.18f},
        (Vector2){rect.x + rect.width * 0.85f, rect.y + rect.height * 0.78f},
        (Color){96, 109, 120, 255}
    );
}

static void DrawBeacon(Rectangle rect, Color base, Color glow, float elapsedSeconds) {
    float radius;

    DrawRectangleRec(rect, base);
    DrawRectangle((int)rect.x + (int)TileScale(20.0f), (int)rect.y + (int)TileScale(8.0f), (int)TileScale(24.0f), (int)TileScale(48.0f), (Color){40, 50, 72, 255});
    radius = TileScale(8.0f) + WaveValue(elapsedSeconds, 4.0f, TileScale(2.0f));
    DrawCircle((int)(rect.x + rect.width * 0.5f), (int)(rect.y + TileScale(20.0f)), radius, glow);
}

static void DrawArchiveIcon(Rectangle rect, Color accent, float elapsedSeconds) {
    Rectangle iconRect;
    float pulse;
    float glowRadius;

    pulse = 0.5f + 0.5f * sinf(elapsedSeconds * 3.2f);
    glowRadius = TileScale(11.0f) + pulse * TileScale(3.0f);
    iconRect = (Rectangle){
        rect.x + TileScale(16.0f),
        rect.y + TileScale(14.0f),
        rect.width - TileScale(32.0f),
        rect.height - TileScale(28.0f)
    };

    DrawCircle((int)(rect.x + rect.width * 0.5f),
               (int)(rect.y + rect.height * 0.46f),
               glowRadius,
               (Color){255, 210, 148, (unsigned char)(50 + pulse * 30.0f)});
    DrawRectangleRounded(iconRect, 0.18f, 5, (Color){42, 56, 76, 240});
    DrawRectangleRoundedLinesEx(iconRect, 0.18f, 5, 2.0f, (Color){255, 214, 154, (unsigned char)(95 + pulse * 40.0f)});
    DrawRectangleRounded((Rectangle){iconRect.x + TileScale(8.0f), iconRect.y + TileScale(7.0f), iconRect.width - TileScale(16.0f), iconRect.height - TileScale(14.0f)},
                         0.12f,
                         4,
                         (Color){230, 238, 246, 245});
    DrawRectangle((int)(iconRect.x + TileScale(10.0f)),
                  (int)(iconRect.y + TileScale(11.0f)),
                  (int)(iconRect.width - TileScale(20.0f)),
                  (int)TileScale(7.0f),
                  accent);
    DrawRectangle((int)(iconRect.x + TileScale(10.0f)),
                  (int)(iconRect.y + TileScale(23.0f)),
                  (int)(iconRect.width - TileScale(26.0f)),
                  (int)TileScale(4.0f),
                  (Color){128, 146, 168, 225});
    DrawRectangle((int)(iconRect.x + TileScale(10.0f)),
                  (int)(iconRect.y + TileScale(31.0f)),
                  (int)(iconRect.width - TileScale(18.0f)),
                  (int)TileScale(4.0f),
                  (Color){128, 146, 168, 205});
    DrawRectangle((int)(iconRect.x + TileScale(10.0f)),
                  (int)(iconRect.y + TileScale(39.0f)),
                  (int)(iconRect.width - TileScale(30.0f)),
                  (int)TileScale(4.0f),
                  (Color){128, 146, 168, 190});
    DrawCircle((int)(iconRect.x + iconRect.width - TileScale(8.0f)),
               (int)(iconRect.y + TileScale(11.0f)),
               TileScale(3.0f) + pulse * TileScale(1.0f),
               accent);
}

void MapInternal_DrawPropCell(const GameMap *map,
                              const AssetBundle *assets,
                              int gridX,
                              int gridY,
                              Rectangle rect,
                              float elapsedSeconds) {
    TileType propTile;

    propTile = map->propTiles[gridY][gridX];

    switch (propTile) {
        case TILE_TREE:
            DrawTree(rect, assets, elapsedSeconds);
            break;
        case TILE_ROCK:
            DrawRock(rect, assets);
            break;
        case TILE_TECH_TABLE:
            if (!ResolvePropDrawRect(propTile, gridX, gridY, &rect)) {
                break;
            }
            if (assets->tileTechTable.loaded) {
                DrawTileTexture(&assets->tileTechTable, rect, WHITE);
            } else {
                DrawRectangleRounded((Rectangle){rect.x + 8.0f, rect.y + 16.0f, rect.width - 16.0f, rect.height - 24.0f}, 0.12f, 3, (Color){65, 81, 100, 235});
                DrawRectangle((int)rect.x + 12, (int)rect.y + 20, (int)rect.width - 24, 10, (Color){138, 190, 220, 210});
                DrawRectangle((int)rect.x + 16, (int)rect.y + 36, 8, (int)rect.height - 44, (Color){87, 101, 120, 255});
                DrawRectangle((int)rect.x + (int)rect.width - 24, (int)rect.y + 36, 8, (int)rect.height - 44, (Color){87, 101, 120, 255});
            }
            break;
        case TILE_STORAGE_LOCKER:
            if (!ResolvePropDrawRect(propTile, gridX, gridY, &rect)) {
                break;
            }
            if (assets->tileStorageLocker.loaded) {
                DrawTileTexture(&assets->tileStorageLocker, rect, WHITE);
            } else {
                DrawRectangle((int)rect.x + 14, (int)rect.y + 8, (int)rect.width - 28, (int)rect.height - 16, (Color){78, 92, 109, 245});
                DrawRectangle((int)rect.x + 18, (int)rect.y + 12, (int)rect.width - 36, (int)rect.height - 24, (Color){108, 124, 144, 235});
                DrawCircle((int)(rect.x + rect.width * 0.70f), (int)(rect.y + rect.height * 0.50f), TileScale(2.2f), (Color){225, 200, 120, 235});
            }
            break;
        case TILE_BUNK:
            if (!ResolvePropDrawRect(propTile, gridX, gridY, &rect)) {
                break;
            }
            if (assets->tileBunk.loaded) {
                DrawTileTexture(&assets->tileBunk, rect, WHITE);
            } else {
                DrawRectangleRounded((Rectangle){rect.x + 7.0f, rect.y + 16.0f, rect.width - 14.0f, rect.height - 22.0f}, 0.10f, 2, (Color){89, 100, 118, 220});
                DrawRectangle((int)rect.x + 10, (int)rect.y + 20, (int)rect.width - 20, (int)rect.height - 30, (Color){132, 146, 162, 220});
                DrawRectangle((int)rect.x + 12, (int)rect.y + 22, 14, 10, (Color){205, 217, 223, 235});
            }
            break;
        case TILE_OXYGEN_CONSOLE:
            if (!ResolvePropDrawRect(propTile, gridX, gridY, &rect)) {
                break;
            }
            if (assets->tileOxygenConsole.loaded) {
                DrawTileTexture(&assets->tileOxygenConsole, rect, WHITE);
            } else {
                DrawBeacon(rect, (Color){24, 34, 47, 255}, (Color){103, 232, 255, 230}, elapsedSeconds);
                DrawRectangle((int)rect.x + 22, (int)rect.y + 78, (int)rect.width - 44, 10, (Color){154, 198, 218, 220});
                DrawRectangle((int)rect.x + 30, (int)rect.y + 92, (int)rect.width - 60, 8, (Color){82, 124, 145, 220});
            }
            break;
        case TILE_LOXI_TERMINAL:
            if (!ResolvePropDrawRect(propTile, gridX, gridY, &rect)) {
                break;
            }
            if (assets->loxiTerminal.loaded) {
                DrawTileTexture(&assets->loxiTerminal, rect, WHITE);
            } else {
                DrawRectangle((int)rect.x + 24, (int)rect.y + 16, (int)rect.width - 48, 76, (Color){55, 67, 90, 255});
                DrawRectangle((int)rect.x + 30, (int)rect.y + 22, (int)rect.width - 60, 36, (Color){120, 221, 238, 220});
                DrawRectangle((int)rect.x + 36, (int)rect.y + 66, (int)rect.width - 72, 18, (Color){78, 118, 139, 255});
            }
            DrawCircle((int)(rect.x + rect.width * 0.5f), (int)(rect.y + 22.0f), TileScale(10.0f) + WaveValue(elapsedSeconds, 3.8f, TileScale(1.8f)), (Color){129, 246, 238, 185});
            break;
        case TILE_WORKBENCH:
            if (!ResolvePropDrawRect(propTile, gridX, gridY, &rect)) {
                break;
            }
            if (assets->tileWorkbench.loaded) {
                DrawTileTexture(&assets->tileWorkbench, rect, WHITE);
            } else {
                DrawRectangle((int)rect.x + 14, (int)rect.y + 42, (int)rect.width - 28, 38, (Color){92, 67, 49, 255});
                DrawRectangle((int)rect.x + 18, (int)rect.y + 24, (int)rect.width - 36, 16, (Color){155, 118, 76, 255});
                DrawRectangle((int)rect.x + 28, (int)rect.y + 32, 18, 44, (Color){83, 96, 112, 220});
                DrawRectangle((int)rect.x + (int)rect.width - 46, (int)rect.y + 36, 16, 30, (Color){214, 184, 118, 215});
            }
            break;
        case TILE_AIRLOCK_CONSOLE:
            if (!ResolvePropDrawRect(propTile, gridX, gridY, &rect)) {
                break;
            }
            if (assets->tileAirlockConsole.loaded) {
                DrawTileTexture(&assets->tileAirlockConsole, rect, WHITE);
            } else {
                DrawBeacon(rect, (Color){22, 33, 46, 255}, (Color){163, 235, 255, 230}, elapsedSeconds);
            }
            break;
        case TILE_AIRLOCK_DOOR:
            if (!ResolvePropDrawRect(propTile, gridX, gridY, &rect)) {
                break;
            }
            if (assets->tileAirlockDoor.loaded) {
                DrawTileTexture(&assets->tileAirlockDoor, rect, WHITE);
            } else {
                DrawRectangle((int)rect.x + 8, (int)rect.y + 4, (int)rect.width - 16, (int)rect.height - 8, (Color){127, 148, 166, 255});
                DrawRectangle((int)rect.x + 28, (int)rect.y + 6, 8, (int)rect.height - 12, (Color){69, 96, 122, 255});
                DrawRectangleLinesEx(rect, 2.0f, (Color){209, 234, 255, 85});
            }
            break;
        case TILE_LOXI_ROOM_DOOR:
            DrawRectangleRounded((Rectangle){rect.x + 4.0f, rect.y + 10.0f, rect.width - 8.0f, rect.height - 20.0f}, 0.18f, 3, (Color){88, 115, 138, 255});
            DrawRectangle((int)rect.x + 10, (int)rect.y + 18, (int)rect.width - 20, (int)rect.height - 36, (Color){53, 82, 103, 255});
            DrawRectangleLinesEx(rect, 2.0f, (Color){124, 214, 226, 72});
            break;
        case TILE_COMM_RELAY:
            if (!ResolvePropDrawRect(propTile, gridX, gridY, &rect)) {
                break;
            }
            if (assets->tileCommRelay.loaded) {
                DrawTileTexture(&assets->tileCommRelay, rect, WHITE);
            } else {
                DrawBeacon(rect, (Color){45, 57, 60, 255}, (Color){97, 243, 218, 230}, elapsedSeconds);
            }
            break;
        case TILE_CRASH_CLUE:
            if (!ResolvePropDrawRect(propTile, gridX, gridY, &rect)) {
                break;
            }
            if (assets->tileCrashClue.loaded) {
                DrawTileTexture(&assets->tileCrashClue, rect, WHITE);
            } else {
                DrawRectangle((int)rect.x + (int)TileScale(12.0f),
                              (int)rect.y + (int)TileScale(46.0f),
                              (int)rect.width - (int)TileScale(28.0f),
                              (int)TileScale(34.0f),
                              (Color){74, 80, 95, 255});
                DrawRectangle((int)rect.x + (int)TileScale(40.0f),
                              (int)rect.y + (int)TileScale(26.0f),
                              (int)TileScale(66.0f),
                              (int)TileScale(22.0f),
                              (Color){96, 103, 118, 250});
                DrawRectangle((int)rect.x + (int)TileScale(82.0f),
                              (int)rect.y + (int)TileScale(58.0f),
                              (int)TileScale(70.0f),
                              (int)TileScale(18.0f),
                              (Color){108, 88, 78, 235});
                DrawRectangle((int)rect.x + (int)TileScale(30.0f),
                              (int)rect.y + (int)TileScale(82.0f),
                              (int)TileScale(28.0f),
                              (int)TileScale(20.0f),
                              (Color){118, 97, 74, 230});
                DrawCircle((int)(rect.x + rect.width * 0.70f),
                           (int)(rect.y + rect.height * 0.35f),
                           TileScale(9.0f) + WaveValue(elapsedSeconds, 3.5f, TileScale(1.0f)),
                           (Color){255, 145, 108, 110});
                DrawRectangleLinesEx(rect, 2.0f, (Color){255, 179, 113, 90});
            }
            break;
        case TILE_ENERGY_CONSOLE:
            if (!ResolvePropDrawRect(propTile, gridX, gridY, &rect)) {
                break;
            }
            if (assets->tileEnergyConsole.loaded) {
                DrawTileTexture(&assets->tileEnergyConsole, rect, WHITE);
            } else {
                DrawBeacon(rect, (Color){32, 35, 52, 255}, (Color){255, 197, 78, 230}, elapsedSeconds);
            }
            break;
        case TILE_SIGNAL_TOWER:
            if (!ResolvePropDrawRect(propTile, gridX, gridY, &rect)) {
                break;
            }
            if (assets->tileSignalTower.loaded) {
                DrawTileTexture(&assets->tileSignalTower, rect, WHITE);
            } else {
                DrawRectangle((int)rect.x + 26, (int)rect.y + 8, 12, 48, (Color){180, 205, 228, 255});
                DrawCircle((int)(rect.x + rect.width * 0.5f), (int)(rect.y + 18), TileScale(6.0f) + WaveValue(elapsedSeconds, 3.0f, TileScale(1.0f)), (Color){104, 231, 255, 180});
            }
            break;
        case TILE_MONOLITH:
            if (!ResolvePropDrawRect(propTile, gridX, gridY, &rect)) {
                break;
            }
            if (assets->tileMonolith.loaded) {
                DrawTileTexture(&assets->tileMonolith, rect, WHITE);
            } else {
                DrawRectangle((int)rect.x + 20, (int)rect.y + 10, 24, 46, (Color){130, 141, 155, 255});
            }
            break;
        case TILE_BARRIER_SWAMP:
            if (assets->tileBarrierSwamp.loaded) {
                DrawTileTexture(&assets->tileBarrierSwamp, rect, WHITE);
                break;
            }
            DrawRectangleRec(rect, (Color){56, 74, 42, 235});
            DrawRectangle((int)rect.x + 10, (int)rect.y + 14, (int)rect.width - 20, (int)rect.height - 28, (Color){90, 118, 58, 220});
            DrawRectangleLinesEx(rect, 2.0f, (Color){168, 198, 116, 105});
            break;
        case TILE_BARRIER_DEEP:
            if (assets->tileBarrierDeep.loaded) {
                DrawTileTexture(&assets->tileBarrierDeep, rect, WHITE);
                break;
            }
            DrawRectangleRec(rect, (Color){70, 54, 30, 235});
            DrawRectangle((int)rect.x + 10, (int)rect.y + 14, (int)rect.width - 20, (int)rect.height - 28, (Color){118, 92, 44, 220});
            DrawRectangleLinesEx(rect, 2.0f, (Color){222, 174, 96, 120});
            break;
        case TILE_BARRIER_RUINS:
            if (assets->tileBarrierRuins.loaded) {
                DrawTileTexture(&assets->tileBarrierRuins, rect, WHITE);
                break;
            }
            DrawRectangleRec(rect, (Color){60, 50, 50, 240});
            DrawRectangle((int)rect.x + 10, (int)rect.y + 14, (int)rect.width - 20, (int)rect.height - 28, (Color){110, 96, 86, 220});
            DrawRectangle((int)rect.x + 18, (int)rect.y + 18, (int)rect.width - 36, 8, (Color){140, 128, 114, 140});
            DrawRectangleLinesEx(rect, 2.0f, (Color){168, 154, 132, 120});
            break;
        case TILE_LOG_SITE:
            DrawArchiveIcon(rect, (Color){118, 226, 255, 235}, elapsedSeconds);
            break;
        case TILE_VOID:
        default:
            break;
    }

    if (map->campPlaced && gridX == map->campX && gridY == map->campY) {
        DrawRectangle((int)rect.x + 10, (int)rect.y + 26, 44, 24, (Color){122, 89, 60, 255});
        DrawTriangle(
            (Vector2){rect.x + 16.0f, rect.y + 42.0f},
            (Vector2){rect.x + 32.0f, rect.y + 14.0f},
            (Vector2){rect.x + 48.0f, rect.y + 42.0f},
            (Color){186, 155, 118, 255}
        );
    }
}
