#include "map_render.h"
#include "map_render_internal.h"

#include <math.h>

#include <string.h>

Rectangle Map_GridToRect(int gridX, int gridY) {
    return (Rectangle){
        gridX * TILE_SIZE,
        gridY * TILE_SIZE,
        TILE_SIZE,
        TILE_SIZE
    };
}

static void DrawLargeMapEntity(const MapEntity *entity, const AssetBundle *assets, float elapsedSeconds) {
    Rectangle bounds;
    Rectangle shadowBounds;
    Color hull;
    Color edge;
    float pulse;

    if (entity == NULL || strcmp(entity->entityType, "ship_exterior") != 0) {
        return;
    }
    bounds = (Rectangle){
        (float)(entity->gridX * TILE_SIZE),
        (float)(entity->gridY * TILE_SIZE),
        (float)(entity->width * TILE_SIZE),
        (float)(entity->height * TILE_SIZE)
    };
    pulse = 0.5f + 0.5f * sinf(elapsedSeconds * 2.2f);
    shadowBounds = (Rectangle){
        bounds.x + bounds.width * 0.035f,
        bounds.y + bounds.height * 0.08f,
        bounds.width * 0.93f,
        bounds.height * 0.88f
    };
    DrawEllipse((int)(shadowBounds.x + shadowBounds.width * 0.5f),
                (int)(shadowBounds.y + shadowBounds.height * 0.72f),
                shadowBounds.width * 0.47f,
                shadowBounds.height * 0.29f,
                (Color){7, 10, 15, 135});
    if (assets != NULL
        && assets->entityPlayerShipExterior.loaded
        && strcmp(entity->textureKey, "ship_exterior_default") == 0) {
        Rectangle source;

        source = (Rectangle){
            0.0f,
            0.0f,
            (float)assets->entityPlayerShipExterior.texture.width,
            (float)assets->entityPlayerShipExterior.texture.height
        };
        DrawTexturePro(assets->entityPlayerShipExterior.texture,
                       source,
                       bounds,
                       (Vector2){0.0f, 0.0f},
                       0.0f,
                       WHITE);
        DrawCircleV((Vector2){bounds.x + bounds.width * 0.82f, bounds.y + bounds.height * 0.61f},
                    (float)TILE_SIZE * (0.24f + pulse * 0.04f),
                    Fade((Color){76, 225, 255, 255}, 0.30f + pulse * 0.18f));
        return;
    }
    hull = (Color){48, 58, 72, 245};
    edge = (Color){118, 195, 222, 220};
    DrawRectangleRounded(bounds, 0.18f, 12, hull);
    DrawRectangleRoundedLines(bounds, 0.18f, 12, edge);
    DrawTriangle((Vector2){bounds.x, bounds.y + bounds.height * 0.28f},
                 (Vector2){bounds.x - bounds.width * 0.16f, bounds.y + bounds.height * 0.5f},
                 (Vector2){bounds.x, bounds.y + bounds.height * 0.72f},
                 (Color){37, 45, 58, 245});
    DrawTriangle((Vector2){bounds.x + bounds.width, bounds.y + bounds.height * 0.28f},
                 (Vector2){bounds.x + bounds.width * 1.16f, bounds.y + bounds.height * 0.5f},
                 (Vector2){bounds.x + bounds.width, bounds.y + bounds.height * 0.72f},
                 (Color){37, 45, 58, 245});
    DrawRectangleRounded((Rectangle){bounds.x + bounds.width * 0.25f,
                                     bounds.y + bounds.height * 0.18f,
                                     bounds.width * 0.50f,
                                     bounds.height * 0.20f},
                         0.35f, 8, (Color){89, 134, 154, 235});
    DrawCircleV((Vector2){bounds.x + bounds.width * 0.82f, bounds.y + bounds.height * 0.50f},
                (float)TILE_SIZE * 0.18f,
                Fade((Color){99, 225, 255, 255}, 0.55f + pulse * 0.35f));
}

static int MaxInt(int a, int b) {
    return a > b ? a : b;
}

static int MinInt(int a, int b) {
    return a < b ? a : b;
}

void Map_Draw(const GameMap *map, const AssetBundle *assets, Camera2D camera, int screenWidth, int screenHeight, float elapsedSeconds) {
    int minGridX;
    int maxGridX;
    int minGridY;
    int maxGridY;
    int row;
    int column;

    minGridX = MaxInt(0, (int)floorf((camera.target.x - (float)screenWidth * 0.5f / camera.zoom) / TILE_SIZE) - 2);
    maxGridX = MinInt(map->width - 1, (int)ceilf((camera.target.x + (float)screenWidth * 0.5f / camera.zoom) / TILE_SIZE) + 2);
    minGridY = MaxInt(0, (int)floorf((camera.target.y - (float)screenHeight * 0.5f / camera.zoom) / TILE_SIZE) - 2);
    maxGridY = MinInt(map->height - 1, (int)ceilf((camera.target.y + (float)screenHeight * 0.5f / camera.zoom) / TILE_SIZE) + 2);

    for (row = minGridY; row <= maxGridY; row++) {
        for (column = minGridX; column <= maxGridX; column++) {
            Rectangle rect;
            HazardType hazard;

            rect = Map_GridToRect(column, row);
            MapInternal_DrawGroundCell(map, assets, column, row, rect, elapsedSeconds);

            hazard = Map_GetHazardAt(map, column, row);
            if (hazard != HAZARD_NONE) {
                MapInternal_DrawHazardCell(hazard, rect, elapsedSeconds);
            }
            MapInternal_DrawDecorCell(Map_GetDecorAt(map, column, row), rect, elapsedSeconds);
        }
    }

    for (row = minGridY; row <= maxGridY; row++) {
        for (column = minGridX; column <= maxGridX; column++) {
            Rectangle rect;

            rect = Map_GridToRect(column, row);
            MapInternal_DrawPropCell(map, assets, column, row, rect, elapsedSeconds);
        }
    }

    for (int entityIndex = 0; entityIndex < map->entityCount; entityIndex++) {
        const MapEntity *entity = &map->entities[entityIndex];
        if (strcmp(entity->drawLayer, "above_props") == 0) {
            DrawLargeMapEntity(entity, assets, elapsedSeconds);
        }
    }
}
