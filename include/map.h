#ifndef MAP_H
#define MAP_H

#include <stdbool.h>
#include "assets.h"
#include "config.h"
#include "raylib.h"

typedef enum MapArea {
    MAP_AREA_BASE = 0,
    MAP_AREA_FOREST,
    MAP_AREA_SWAMP_OUTER,
    MAP_AREA_SWAMP_DEEP,
    MAP_AREA_RUINS,
    MAP_AREA_UNKNOWN
} MapArea;

typedef enum TileType {
    TILE_VOID = 0,
    TILE_BASE_FLOOR,
    TILE_FOREST_GROUND,
    TILE_SWAMP_GROUND,
    TILE_DEEP_SWAMP_GROUND,
    TILE_RUINS_GROUND,
    TILE_TREE,
    TILE_ROCK,
    TILE_SHIP_CORE,
    TILE_WORKBENCH,
    TILE_AIRLOCK_CONSOLE,
    TILE_AIRLOCK_DOOR,
    TILE_COMM_RELAY,
    TILE_CRASH_CLUE,
    TILE_ENERGY_CONSOLE,
    TILE_SIGNAL_TOWER,
    TILE_MONOLITH,
    TILE_BARRIER_SWAMP,
    TILE_BARRIER_DEEP,
    TILE_BARRIER_RUINS,
    TILE_LOG_SITE
} TileType;

typedef enum HazardType {
    HAZARD_NONE = 0,
    HAZARD_TRIP,
    HAZARD_SWAMP,
    HAZARD_POISON
} HazardType;

typedef struct GameMap {
    TileType tiles[MAP_HEIGHT][MAP_WIDTH];
    bool campPlaced;
    int campX;
    int campY;
} GameMap;

void Map_Init(GameMap *map);
void Map_Draw(const GameMap *map, const AssetBundle *assets, Camera2D camera, int screenWidth, int screenHeight, float elapsedSeconds);

bool Map_IsWithinBounds(int gridX, int gridY);
bool Map_IsWalkable(const GameMap *map, int gridX, int gridY);
bool Map_IsOpaque(const GameMap *map, int gridX, int gridY);
TileType Map_GetTileAt(const GameMap *map, int gridX, int gridY);
HazardType Map_GetHazardAt(const GameMap *map, int gridX, int gridY);
MapArea Map_GetAreaAt(int gridX, int gridY);
float Map_GetMoveStaminaCost(const GameMap *map, int gridX, int gridY);
Vector2 Map_GridToWorld(int gridX, int gridY);
Rectangle Map_GridToRect(int gridX, int gridY);
const char *Map_GetAreaName(MapArea area);
const char *Map_GetRoomNameAt(int gridX, int gridY);

void Map_UnlockSwampOuter(GameMap *map);
void Map_UnlockSwampDeep(GameMap *map);
void Map_UnlockRuins(GameMap *map);
void Map_SetFieldCamp(GameMap *map, int gridX, int gridY);

bool Map_CanCrossWithRope(const GameMap *map, int fromX, int fromY, int toX, int toY);
void Map_CreateRopeBridge(GameMap *map, int gridX, int gridY);

#endif
