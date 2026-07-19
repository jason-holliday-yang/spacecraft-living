#include "map_render_internal.h"

#include <math.h>
#include <stddef.h>

static float TileScale(float value) {
    return value * ((float)TILE_SIZE / 64.0f);
}

static void DrawTileTexture(const TextureAsset *asset, Rectangle rect, Color tint) {
    Rectangle source;

    source = (Rectangle){0.0f, 0.0f, (float)asset->texture.width, (float)asset->texture.height};
    DrawTexturePro(asset->texture, source, rect, (Vector2){0.0f, 0.0f}, 0.0f, tint);
}

static void DrawGroundTile(const TextureAsset *asset, Rectangle rect, Color a, Color b, bool checker) {
    if (asset != NULL && asset->loaded) {
        DrawTileTexture(asset, rect, WHITE);
        return;
    }

    DrawRectangleRec(rect, a);
    if (checker) {
        DrawRectangle((int)rect.x + 4, (int)rect.y + 4, (int)rect.width - 8, (int)rect.height - 8, b);
    }
}

void MapInternal_DrawGroundCell(const GameMap *map,
                                const AssetBundle *assets,
                                int gridX,
                                int gridY,
                                Rectangle rect,
                                float elapsedSeconds) {
    TileType groundTile;
    const char *locationName;
    const char *roomName;
    const TextureAsset *groundAsset;

    groundTile = map->groundTiles[gridY][gridX];
    locationName = Map_GetLocationNameAtMap(map, gridX, gridY);
    roomName = Map_GetRoomNameAtMap(map, gridX, gridY);
    groundAsset = MapInternal_GetGroundTextureAsset(assets, groundTile, locationName, roomName);

    switch (groundTile) {
        case TILE_BASE_FLOOR:
            DrawGroundTile(groundAsset, rect, (Color){23, 31, 44, 255}, (Color){36, 46, 60, 255}, true);
            MapInternal_DrawGroundDetailOverlay(groundTile, gridX, gridY, rect, locationName, roomName, elapsedSeconds);
            break;
        case TILE_FOREST_GROUND:
            DrawGroundTile(groundAsset, rect, (Color){24, 54, 47, 255}, (Color){30, 71, 55, 255}, (gridY + gridX) % 2 == 0);
            MapInternal_DrawGroundDetailOverlay(groundTile, gridX, gridY, rect, locationName, roomName, elapsedSeconds);
            break;
        case TILE_SWAMP_GROUND:
            DrawGroundTile(groundAsset, rect, (Color){46, 67, 44, 255}, (Color){67, 92, 54, 255}, true);
            DrawCircle((int)(rect.x + rect.width * 0.45f), (int)(rect.y + rect.height * 0.55f), TileScale(10.0f), (Color){87, 62, 39, 100});
            MapInternal_DrawGroundDetailOverlay(groundTile, gridX, gridY, rect, locationName, roomName, elapsedSeconds);
            break;
        case TILE_DEEP_SWAMP_GROUND:
            DrawGroundTile(groundAsset, rect, (Color){40, 48, 42, 255}, (Color){57, 73, 50, 255}, true);
            DrawCircle((int)(rect.x + rect.width * 0.50f), (int)(rect.y + rect.height * 0.52f), TileScale(12.0f), (Color){133, 179, 66, 80});
            MapInternal_DrawGroundDetailOverlay(groundTile, gridX, gridY, rect, locationName, roomName, elapsedSeconds);
            break;
        case TILE_RUINS_GROUND:
            DrawGroundTile(groundAsset, rect, (Color){73, 76, 83, 255}, (Color){91, 93, 102, 255}, (gridY + gridX) % 2 == 0);
            MapInternal_DrawGroundDetailOverlay(groundTile, gridX, gridY, rect, locationName, roomName, elapsedSeconds);
            break;
        case TILE_VOID:
        default:
            MapInternal_DrawVoidGroundBackdrop(gridX, gridY, rect, elapsedSeconds);
            break;
    }
}

void MapInternal_DrawHazardCell(HazardType hazard, Rectangle rect, float elapsedSeconds) {
    float pulse;
    int centerX;
    int centerY;

    centerX = (int)(rect.x + rect.width * 0.5f);
    centerY = (int)(rect.y + rect.height * 0.5f);
    pulse = (sinf(elapsedSeconds * 4.0f) + 1.0f) * 0.5f;

    switch (hazard) {
        case HAZARD_TRIP:
            DrawCircleLines(centerX, centerY, TileScale(18.0f), (Color){220, 60, 60, 180});
            DrawCircle(centerX, centerY, TileScale(3.0f), (Color){220, 60, 60, 200 + (int)(pulse * 55.0f)});
            DrawLineEx(
                (Vector2){rect.x + 8.0f, rect.y + 16.0f},
                (Vector2){rect.x + 56.0f, rect.y + 48.0f},
                TileScale(2.0f),
                (Color){180, 40, 40, 160}
            );
            break;
        case HAZARD_SWAMP: {
            int bubbleCount;
            int index;

            bubbleCount = 3 + (int)(pulse * 2.0f);
            for (index = 0; index < bubbleCount; index++) {
                float offsetX;
                float offsetY;
                float bubbleSize;

                offsetX = (sinf(elapsedSeconds * 3.0f + index * 2.0f) + 1.0f) * 0.5f;
                offsetY = (cosf(elapsedSeconds * 2.5f + index * 1.5f) + 1.0f) * 0.5f;
                bubbleSize = TileScale(4.0f + offsetX * 3.0f);

                DrawCircle(
                    centerX + (int)((offsetX - 0.5f) * 20.0f),
                    centerY + (int)((offsetY - 0.5f) * 20.0f),
                    bubbleSize,
                    (Color){133, 179, 66, 120 + (int)(pulse * 80.0f)}
                );
            }
            break;
        }
        case HAZARD_POISON: {
            int index;

            for (index = 0; index < 6; index++) {
                float angle;

                angle = elapsedSeconds * 0.8f + index * (PI / 3.0f);
                DrawCircle(
                    centerX + (int)(cosf(angle) * TileScale(12.0f)),
                    centerY + (int)(sinf(angle) * TileScale(12.0f)),
                    TileScale(3.0f),
                    (Color){150, 60, 180, 140 + (int)(pulse * 70.0f)}
                );
            }
            DrawCircleLines(centerX, centerY, TileScale(16.0f), (Color){180, 80, 200, 100 + (int)(pulse * 100.0f)});
            break;
        }
        default:
            break;
    }
}
