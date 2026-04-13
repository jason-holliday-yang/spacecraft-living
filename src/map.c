#include "map.h"
#include "map_internal.h"

#include <math.h>

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
    maxGridX = MinInt(MAP_WIDTH - 1, (int)ceilf((camera.target.x + (float)screenWidth * 0.5f / camera.zoom) / TILE_SIZE) + 2);
    minGridY = MaxInt(0, (int)floorf((camera.target.y - (float)screenHeight * 0.5f / camera.zoom) / TILE_SIZE) - 2);
    maxGridY = MinInt(MAP_HEIGHT - 1, (int)ceilf((camera.target.y + (float)screenHeight * 0.5f / camera.zoom) / TILE_SIZE) + 2);

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
        }
    }

    for (row = minGridY; row <= maxGridY; row++) {
        for (column = minGridX; column <= maxGridX; column++) {
            Rectangle rect;

            rect = Map_GridToRect(column, row);
            MapInternal_DrawPropCell(map, assets, column, row, rect, elapsedSeconds);
        }
    }
}
