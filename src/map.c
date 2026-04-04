#include "map.h"

#include <math.h>

static void DrawHazardVisuals(HazardType hazard, Rectangle rect, float elapsedSeconds);

static int MaxInt(int a, int b) {
    return a > b ? a : b;
}

static int MinInt(int a, int b) {
    return a < b ? a : b;
}

static float WaveValue(float t, float speed, float amount) {
    return sinf(t * speed) * amount;
}

static float TileScale(float value) {
    return value * ((float)TILE_SIZE / 64.0f);
}

static void FillRect(GameMap *map, int x, int y, int width, int height, TileType tile) {
    int row;
    int column;

    for (row = y; row < y + height; row++) {
        for (column = x; column < x + width; column++) {
            if (Map_IsWithinBounds(column, row)) {
                map->tiles[row][column] = tile;
            }
        }
    }
}

static void DrawTileTexture(const TextureAsset *asset, Rectangle rect, Color tint) {
    Rectangle source;

    source = (Rectangle){0.0f, 0.0f, (float)asset->texture.width, (float)asset->texture.height};
    DrawTexturePro(asset->texture, source, rect, (Vector2){0.0f, 0.0f}, 0.0f, tint);
}

static void DrawGroundTile(const TextureAsset *asset, Rectangle rect, Color a, Color b, bool checker) {
    if (asset->loaded) {
        DrawTileTexture(asset, rect, WHITE);
        return;
    }

    DrawRectangleRec(rect, a);
    if (checker) {
        DrawRectangle((int)rect.x + 4, (int)rect.y + 4, (int)rect.width - 8, (int)rect.height - 8, b);
    }
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

void Map_Init(GameMap *map) {
    int row;
    int column;

    for (row = 0; row < MAP_HEIGHT; row++) {
        for (column = 0; column < MAP_WIDTH; column++) {
            map->tiles[row][column] = TILE_VOID;
        }
    }

    map->campPlaced = false;
    map->campX = -1;
    map->campY = -1;

    /* Ship layout: center hub, three branch cabins, and a long corridor to the right. */
    FillRect(map, 32, 37, 18, 11, TILE_BASE_FLOOR);  /* center hub */
    FillRect(map, 22, 35, 10, 15, TILE_BASE_FLOOR);  /* left cabin */
    FillRect(map, 34, 23, 14, 14, TILE_BASE_FLOOR);  /* upper cabin */
    FillRect(map, 34, 48, 14, 14, TILE_BASE_FLOOR);  /* lower cabin */
    FillRect(map, 50, 40, 14, 5, TILE_BASE_FLOOR);   /* right corridor */

    /* Narrower links between the central cabin and the three side cabins. */
    FillRect(map, 31, 40, 2, 5, TILE_BASE_FLOOR);
    FillRect(map, 38, 36, 5, 1, TILE_BASE_FLOOR);
    FillRect(map, 38, 48, 5, 1, TILE_BASE_FLOOR);

    /* Interior dividers and alcoves for readability without breaking connectivity. */
    FillRect(map, 27, 38, 1, 9, TILE_VOID);
    FillRect(map, 37, 27, 8, 1, TILE_VOID);
    FillRect(map, 37, 57, 8, 1, TILE_VOID);
    FillRect(map, 40, 40, 1, 5, TILE_VOID);
    FillRect(map, 44, 40, 1, 5, TILE_VOID);
    FillRect(map, 56, 40, 1, 5, TILE_VOID);

    /* Door openings in each divider. */
    FillRect(map, 27, 41, 1, 2, TILE_BASE_FLOOR);
    FillRect(map, 39, 27, 2, 1, TILE_BASE_FLOOR);
    FillRect(map, 39, 57, 2, 1, TILE_BASE_FLOOR);
    FillRect(map, 40, 41, 1, 2, TILE_BASE_FLOOR);
    FillRect(map, 44, 42, 1, 2, TILE_BASE_FLOOR);
    FillRect(map, 56, 41, 1, 2, TILE_BASE_FLOOR);

    /* Small obstacle islands to avoid one huge empty chamber. */
    FillRect(map, 24, 38, 2, 3, TILE_VOID);
    FillRect(map, 24, 44, 2, 3, TILE_VOID);
    FillRect(map, 35, 30, 3, 2, TILE_VOID);
    FillRect(map, 43, 30, 3, 2, TILE_VOID);
    FillRect(map, 35, 53, 3, 2, TILE_VOID);
    FillRect(map, 43, 53, 3, 2, TILE_VOID);

    /* Exterior airlock bridge and large separated outdoor regions. */
    FillRect(map, 64, 40, 7, 5, TILE_FOREST_GROUND);
    FillRect(map, 71, 22, 17, 24, TILE_FOREST_GROUND);
    FillRect(map, 70, 46, 14, 20, TILE_FOREST_GROUND);
    FillRect(map, 74, 66, 16, 16, TILE_SWAMP_GROUND);
    FillRect(map, 82, 50, 14, 16, TILE_DEEP_SWAMP_GROUND);
    FillRect(map, 87, 32, 12, 43, TILE_RUINS_GROUND);

    for (column = 73; column <= 86; column += 2) {
        map->tiles[26][column] = TILE_TREE;
        map->tiles[42][column - 1] = TILE_TREE;
        map->tiles[60][column] = TILE_TREE;
    }

    for (row = 28; row <= 62; row += 3) {
        map->tiles[row][76] = TILE_TREE;
        map->tiles[row][80] = TILE_TREE;
    }

    map->tiles[52][74] = TILE_ROCK;
    map->tiles[55][78] = TILE_ROCK;
    map->tiles[58][83] = TILE_ROCK;

    for (row = 69; row <= 79; row += 2) {
        map->tiles[row][79] = TILE_SWAMP_GROUND;
        map->tiles[row][84] = TILE_SWAMP_GROUND;
    }

    for (row = 52; row <= 64; row += 2) {
        map->tiles[row][87] = TILE_DEEP_SWAMP_GROUND;
        map->tiles[row][90] = TILE_DEEP_SWAMP_GROUND;
    }

    map->tiles[SHIP_CORE_Y][SHIP_CORE_X] = TILE_SHIP_CORE;
    map->tiles[WORKBENCH_Y][WORKBENCH_X] = TILE_WORKBENCH;
    map->tiles[AIRLOCK_CONSOLE_Y][AIRLOCK_CONSOLE_X] = TILE_AIRLOCK_CONSOLE;
    map->tiles[COMM_RELAY_Y][COMM_RELAY_X] = TILE_COMM_RELAY;
    map->tiles[CRASH_CLUE_Y][CRASH_CLUE_X] = TILE_CRASH_CLUE;
    map->tiles[ENERGY_CONSOLE_Y][ENERGY_CONSOLE_X] = TILE_ENERGY_CONSOLE;
    map->tiles[SIGNAL_TOWER_Y][SIGNAL_TOWER_X] = TILE_SIGNAL_TOWER;
    map->tiles[MONOLITH_A_Y][MONOLITH_A_X] = TILE_MONOLITH;
    map->tiles[MONOLITH_B_Y][MONOLITH_B_X] = TILE_MONOLITH;
    map->tiles[MONOLITH_C_Y][MONOLITH_C_X] = TILE_MONOLITH;

    map->tiles[AIRLOCK_DOOR_Y][AIRLOCK_DOOR_X] = TILE_AIRLOCK_DOOR;
    map->tiles[73][81] = TILE_BARRIER_DEEP;
    map->tiles[73][82] = TILE_BARRIER_DEEP;
    map->tiles[51][87] = TILE_BARRIER_RUINS;
    map->tiles[52][87] = TILE_BARRIER_RUINS;

    map->tiles[33][33] = TILE_LOG_SITE;
    map->tiles[58][57] = TILE_LOG_SITE;
    map->tiles[52][73] = TILE_LOG_SITE;
    map->tiles[60][89] = TILE_LOG_SITE;
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
            TileType tile;

            rect = Map_GridToRect(column, row);
            tile = map->tiles[row][column];

            switch (tile) {
                case TILE_BASE_FLOOR:
                    DrawGroundTile(&assets->tileBaseFloor, rect, (Color){23, 31, 44, 255}, (Color){36, 46, 60, 255}, true);
                    break;
                case TILE_FOREST_GROUND:
                    DrawGroundTile(&assets->tileForestGround, rect, (Color){24, 54, 47, 255}, (Color){30, 71, 55, 255}, (row + column) % 2 == 0);
                    break;
                case TILE_SWAMP_GROUND:
                    DrawGroundTile(&assets->tileSwampGround, rect, (Color){46, 67, 44, 255}, (Color){67, 92, 54, 255}, true);
                    DrawCircle((int)(rect.x + rect.width * 0.45f), (int)(rect.y + rect.height * 0.55f), TileScale(10.0f), (Color){87, 62, 39, 100});
                    break;
                case TILE_DEEP_SWAMP_GROUND:
                    DrawGroundTile(&assets->tileSwampGround, rect, (Color){40, 48, 42, 255}, (Color){57, 73, 50, 255}, true);
                    DrawCircle((int)(rect.x + rect.width * 0.50f), (int)(rect.y + rect.height * 0.52f), TileScale(12.0f), (Color){133, 179, 66, 80});
                    break;
                case TILE_RUINS_GROUND:
                    DrawGroundTile(&assets->tileRuinsFloor, rect, (Color){73, 76, 83, 255}, (Color){91, 93, 102, 255}, (row + column) % 2 == 0);
                    break;
                case TILE_TREE:
                    DrawGroundTile(&assets->tileForestGround, rect, (Color){21, 50, 44, 255}, (Color){31, 63, 52, 255}, false);
                    DrawTree(rect, assets, elapsedSeconds);
                    break;
                case TILE_ROCK:
                    DrawGroundTile(&assets->tileForestGround, rect, (Color){29, 56, 48, 255}, (Color){34, 66, 54, 255}, false);
                    DrawRock(rect, assets);
                    break;
                case TILE_SHIP_CORE:
                    DrawGroundTile(&assets->tileBaseFloor, rect, (Color){26, 35, 50, 255}, (Color){34, 49, 67, 255}, false);
                    DrawBeacon(rect, (Color){24, 30, 46, 255}, (Color){90, 211, 255, 230}, elapsedSeconds);
                    break;
                case TILE_WORKBENCH:
                    DrawGroundTile(&assets->tileBaseFloor, rect, (Color){26, 35, 50, 255}, (Color){34, 49, 67, 255}, false);
                    if (assets->tileWorkbench.loaded) {
                        DrawTileTexture(&assets->tileWorkbench, rect, WHITE);
                    } else {
                        DrawRectangle((int)rect.x + 8, (int)rect.y + 18, (int)rect.width - 16, 24, (Color){92, 67, 49, 255});
                        DrawRectangle((int)rect.x + 12, (int)rect.y + 12, (int)rect.width - 24, 10, (Color){155, 118, 76, 255});
                    }
                    break;
                case TILE_AIRLOCK_CONSOLE:
                    DrawGroundTile(&assets->tileBaseFloor, rect, (Color){27, 35, 52, 255}, (Color){35, 49, 67, 255}, false);
                    DrawBeacon(rect, (Color){22, 33, 46, 255}, (Color){163, 235, 255, 230}, elapsedSeconds);
                    break;
                case TILE_AIRLOCK_DOOR:
                    DrawGroundTile(&assets->tileBaseFloor, rect, (Color){21, 28, 39, 255}, (Color){32, 43, 58, 255}, false);
                    DrawRectangle((int)rect.x + 8, (int)rect.y + 4, (int)rect.width - 16, (int)rect.height - 8, (Color){127, 148, 166, 255});
                    DrawRectangle((int)rect.x + 28, (int)rect.y + 6, 8, (int)rect.height - 12, (Color){69, 96, 122, 255});
                    DrawRectangleLinesEx(rect, 2.0f, (Color){209, 234, 255, 85});
                    break;
                case TILE_COMM_RELAY:
                    DrawGroundTile(&assets->tileSwampGround, rect, (Color){43, 63, 47, 255}, (Color){54, 79, 57, 255}, false);
                    DrawBeacon(rect, (Color){45, 57, 60, 255}, (Color){97, 243, 218, 230}, elapsedSeconds);
                    break;
                case TILE_CRASH_CLUE:
                    DrawGroundTile(&assets->tileForestGround, rect, (Color){26, 53, 46, 255}, (Color){31, 60, 48, 255}, false);
                    DrawRectangle((int)rect.x + 9, (int)rect.y + 18, (int)rect.width - 18, 22, (Color){89, 95, 109, 255});
                    DrawRectangleLinesEx(rect, 1.0f, (Color){255, 179, 113, 60});
                    break;
                case TILE_ENERGY_CONSOLE:
                    DrawGroundTile(&assets->tileBaseFloor, rect, (Color){26, 35, 50, 255}, (Color){34, 49, 67, 255}, false);
                    DrawBeacon(rect, (Color){32, 35, 52, 255}, (Color){255, 197, 78, 230}, elapsedSeconds);
                    break;
                case TILE_SIGNAL_TOWER:
                    DrawGroundTile(&assets->tileRuinsFloor, rect, (Color){71, 74, 84, 255}, (Color){86, 91, 102, 255}, false);
                    if (assets->tileSignalTower.loaded) {
                        DrawTileTexture(&assets->tileSignalTower, rect, WHITE);
                    } else {
                        DrawRectangle((int)rect.x + 26, (int)rect.y + 8, 12, 48, (Color){180, 205, 228, 255});
                        DrawCircle((int)(rect.x + rect.width * 0.5f), (int)(rect.y + 18), TileScale(6.0f) + WaveValue(elapsedSeconds, 3.0f, TileScale(1.0f)), (Color){104, 231, 255, 180});
                    }
                    break;
                case TILE_MONOLITH:
                    DrawGroundTile(&assets->tileRuinsFloor, rect, (Color){70, 74, 84, 255}, (Color){88, 91, 100, 255}, false);
                    DrawRectangle((int)rect.x + 20, (int)rect.y + 10, 24, 46, (Color){130, 141, 155, 255});
                    break;
                case TILE_BARRIER_SWAMP:
                case TILE_BARRIER_DEEP:
                case TILE_BARRIER_RUINS:
                    DrawRectangleRec(rect, (Color){50, 24, 28, 255});
                    DrawRectangleLinesEx(rect, 2.0f, (Color){232, 130, 96, 120});
                    break;
                case TILE_LOG_SITE:
                    DrawGroundTile(&assets->tileForestGround, rect, (Color){38, 49, 61, 255}, (Color){44, 60, 74, 255}, false);
                    DrawCircle((int)(rect.x + rect.width * 0.5f), (int)(rect.y + rect.height * 0.5f), TileScale(12.0f), (Color){104, 173, 255, 120});
                    break;
                case TILE_VOID:
                default:
                    DrawRectangleRec(rect, (Color){5, 9, 15, 255});
                    break;
            }
            
            HazardType hazard = Map_GetHazardAt(map, column, row);
            if (hazard != HAZARD_NONE) {
                DrawHazardVisuals(hazard, rect, elapsedSeconds);
            }

            if (map->campPlaced && column == map->campX && row == map->campY) {
                DrawRectangle((int)rect.x + 10, (int)rect.y + 26, 44, 24, (Color){122, 89, 60, 255});
                DrawTriangle(
                    (Vector2){rect.x + 16.0f, rect.y + 42.0f},
                    (Vector2){rect.x + 32.0f, rect.y + 14.0f},
                    (Vector2){rect.x + 48.0f, rect.y + 42.0f},
                    (Color){186, 155, 118, 255}
                );
            }
        }
    }
}

static void DrawHazardVisuals(HazardType hazard, Rectangle rect, float elapsedSeconds) {
    float pulse;
    int centerX;
    int centerY;
    
    centerX = (int)(rect.x + rect.width * 0.5f);
    centerY = (int)(rect.y + rect.height * 0.5f);
    pulse = (sinf(elapsedSeconds * 4.0f) + 1.0f) * 0.5f;
    
    switch (hazard) {
        case HAZARD_TRIP: {
            DrawCircleLines(centerX, centerY, TileScale(18.0f), (Color){220, 60, 60, 180});
            DrawCircle(centerX, centerY, TileScale(3.0f), (Color){220, 60, 60, 200 + (int)(pulse * 55.0f)});
            
            DrawLineEx(
                (Vector2){rect.x + 8.0f, rect.y + 16.0f},
                (Vector2){rect.x + 56.0f, rect.y + 48.0f},
                TileScale(2.0f),
                (Color){180, 40, 40, 160}
            );
            break;
        }
        case HAZARD_SWAMP: {
            int bubbleCount;
            int i;
            
            bubbleCount = 3 + (int)(pulse * 2.0f);
            for (i = 0; i < bubbleCount; i++) {
                float offsetX;
                float offsetY;
                float bubbleSize;
                
                offsetX = (sinf(elapsedSeconds * 3.0f + i * 2.0f) + 1.0f) * 0.5f;
                offsetY = (cosf(elapsedSeconds * 2.5f + i * 1.5f) + 1.0f) * 0.5f;
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
            int i;
            float angle;
            
            for (i = 0; i < 6; i++) {
                angle = elapsedSeconds * 0.8f + i * (PI / 3.0f);
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

bool Map_IsWithinBounds(int gridX, int gridY) {
    return gridX >= 0 && gridX < MAP_WIDTH && gridY >= 0 && gridY < MAP_HEIGHT;
}

bool Map_IsWalkable(const GameMap *map, int gridX, int gridY) {
    TileType tile;

    if (!Map_IsWithinBounds(gridX, gridY)) {
        return false;
    }

    tile = map->tiles[gridY][gridX];
    return tile != TILE_VOID
        && tile != TILE_TREE
        && tile != TILE_ROCK
        && tile != TILE_AIRLOCK_DOOR
        && tile != TILE_BARRIER_SWAMP
        && tile != TILE_BARRIER_DEEP
        && tile != TILE_BARRIER_RUINS;
}

bool Map_IsOpaque(const GameMap *map, int gridX, int gridY) {
    TileType tile;

    tile = Map_GetTileAt(map, gridX, gridY);
    return tile == TILE_TREE || tile == TILE_ROCK;
}

TileType Map_GetTileAt(const GameMap *map, int gridX, int gridY) {
    if (!Map_IsWithinBounds(gridX, gridY)) {
        return TILE_VOID;
    }
    return map->tiles[gridY][gridX];
}

HazardType Map_GetHazardAt(const GameMap *map, int gridX, int gridY) {
    switch (Map_GetTileAt(map, gridX, gridY)) {
        case TILE_FOREST_GROUND:
            if ((gridX + gridY) % 11 == 0) {
                return HAZARD_TRIP;
            }
            return HAZARD_NONE;
        case TILE_SWAMP_GROUND:
            return HAZARD_SWAMP;
        case TILE_DEEP_SWAMP_GROUND:
            return HAZARD_POISON;
        default:
            return HAZARD_NONE;
    }
}

MapArea Map_GetAreaAt(int gridX, int gridY) {
    if (gridX >= 28 && gridX <= 64 && gridY >= 24 && gridY <= 62) {
        return MAP_AREA_BASE;
    }
    if (gridX >= 64 && gridX <= 88 && gridY >= 22 && gridY <= 66) {
        return MAP_AREA_FOREST;
    }
    if (gridX >= 74 && gridX <= 90 && gridY >= 66 && gridY <= 82) {
        return MAP_AREA_SWAMP_OUTER;
    }
    if (gridX >= 82 && gridX <= 96 && gridY >= 50 && gridY <= 66) {
        return MAP_AREA_SWAMP_DEEP;
    }
    if (gridX >= 87 && gridX <= 99 && gridY >= 32 && gridY <= 75) {
        return MAP_AREA_RUINS;
    }
    return MAP_AREA_UNKNOWN;
}

float Map_GetMoveStaminaCost(const GameMap *map, int gridX, int gridY) {
    switch (Map_GetHazardAt(map, gridX, gridY)) {
        case HAZARD_TRIP:
            return STAMINA_MOVE_COST + 1.0f;
        case HAZARD_SWAMP:
            return STAMINA_MOVE_COST + 1.5f;
        case HAZARD_POISON:
            return STAMINA_MOVE_COST + 2.0f;
        case HAZARD_NONE:
        default:
            return STAMINA_MOVE_COST;
    }
}

Vector2 Map_GridToWorld(int gridX, int gridY) {
    return (Vector2){
        gridX * TILE_SIZE + TILE_SIZE * 0.5f,
        gridY * TILE_SIZE + TILE_SIZE * 0.5f
    };
}

Rectangle Map_GridToRect(int gridX, int gridY) {
    return (Rectangle){
        gridX * TILE_SIZE,
        gridY * TILE_SIZE,
        TILE_SIZE,
        TILE_SIZE
    };
}

const char *Map_GetAreaName(MapArea area) {
    switch (area) {
        case MAP_AREA_BASE:
            return "Ship Base";
        case MAP_AREA_FOREST:
            return "Crash Forest";
        case MAP_AREA_SWAMP_OUTER:
            return "Outer Spore Swamp";
        case MAP_AREA_SWAMP_DEEP:
            return "Deep Spore Swamp";
        case MAP_AREA_RUINS:
            return "Ruins";
        case MAP_AREA_UNKNOWN:
        default:
            return "Unknown Area";
    }
}

const char *Map_GetRoomNameAt(int gridX, int gridY) {
    if (gridX >= 32 && gridX <= 49 && gridY >= 37 && gridY <= 47) {
        return "Central Hub";
    }
    if (gridX >= 22 && gridX <= 31 && gridY >= 35 && gridY <= 49) {
        return "Left Cabin";
    }
    if (gridX >= 34 && gridX <= 47 && gridY >= 23 && gridY <= 36) {
        return "Upper Cabin";
    }
    if (gridX >= 34 && gridX <= 47 && gridY >= 48 && gridY <= 61) {
        return "Lower Cabin";
    }
    if (gridX >= 50 && gridX <= 63 && gridY >= 40 && gridY <= 44) {
        return "Airlock Corridor";
    }
    return Map_GetAreaName(Map_GetAreaAt(gridX, gridY));
}

void Map_UnlockSwampOuter(GameMap *map) {
    map->tiles[AIRLOCK_DOOR_Y][AIRLOCK_DOOR_X] = TILE_FOREST_GROUND;
}

void Map_UnlockSwampDeep(GameMap *map) {
    map->tiles[73][81] = TILE_SWAMP_GROUND;
    map->tiles[73][82] = TILE_SWAMP_GROUND;
}

void Map_UnlockRuins(GameMap *map) {
    map->tiles[51][87] = TILE_RUINS_GROUND;
    map->tiles[52][87] = TILE_RUINS_GROUND;
}

void Map_SetFieldCamp(GameMap *map, int gridX, int gridY) {
    map->campPlaced = true;
    map->campX = gridX;
    map->campY = gridY;
}

bool Map_CanCrossWithRope(const GameMap *map, int fromX, int fromY, int toX, int toY) {
    if (!Map_IsWithinBounds(fromX, fromY) || !Map_IsWithinBounds(toX, toY)) {
        return false;
    }
    
    int dx = toX - fromX;
    int dy = toY - fromY;
    
    if ((dx == 0 && dy == 0)) {
        return false;
    }
    
    if (dx != 0 && dy != 0) {
        return false;
    }
    
    if (dx > 1 || dx < -1 || dy > 1 || dy < -1) {
        return false;
    }
    
    TileType toTile = map->tiles[toY][toX];
    
    if (toTile == TILE_BARRIER_SWAMP || toTile == TILE_BARRIER_DEEP || toTile == TILE_BARRIER_RUINS) {
        return true;
    }
    
    return false;
}

void Map_CreateRopeBridge(GameMap *map, int gridX, int gridY) {
    if (!Map_IsWithinBounds(gridX, gridY)) {
        return;
    }
    
    TileType tile = map->tiles[gridY][gridX];
    if (tile == TILE_BARRIER_SWAMP) {
        map->tiles[gridY][gridX] = TILE_SWAMP_GROUND;
    } else if (tile == TILE_BARRIER_DEEP) {
        map->tiles[gridY][gridX] = TILE_DEEP_SWAMP_GROUND;
    } else if (tile == TILE_BARRIER_RUINS) {
        map->tiles[gridY][gridX] = TILE_RUINS_GROUND;
    }
}
