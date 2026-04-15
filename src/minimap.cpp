#include "minimap.h"

#include "raylib.h"

#include <cstring>

namespace {

Color GetMiniMapTileColor(const GameMap *map, int gridX, int gridY) {
    TileType propTile;
    MapArea area;
    const char *locationName;

    propTile = Map_GetPropTileAt(map, gridX, gridY);
    if (propTile == TILE_SIGNAL_TOWER || propTile == TILE_COMM_RELAY) {
        return Color{109, 243, 221, 240};
    }
    if (propTile == TILE_MONOLITH) {
        return Color{217, 226, 234, 240};
    }
    if (propTile == TILE_AIRLOCK_DOOR || propTile == TILE_BARRIER_DEEP || propTile == TILE_BARRIER_RUINS) {
        return Color{201, 166, 113, 240};
    }
    if (propTile == TILE_TREE) {
        return Color{42, 88, 67, 230};
    }
    if (propTile == TILE_ROCK) {
        return Color{112, 124, 136, 230};
    }

    locationName = Map_GetLocationNameAt(gridX, gridY);
    if (std::strcmp(locationName, "West Frontier") == 0) {
        return Color{88, 96, 112, 226};
    }
    if (std::strcmp(locationName, "Survey Break") == 0) {
        return Color{102, 114, 128, 226};
    }
    if (std::strcmp(locationName, "Canopy Hollow") == 0) {
        return Color{114, 126, 140, 226};
    }
    if (std::strcmp(locationName, "Echo Basin") == 0) {
        return Color{126, 140, 154, 226};
    }
    if (std::strcmp(locationName, "Last Camp") == 0) {
        return Color{140, 154, 166, 226};
    }
    if (std::strcmp(locationName, "South Collapse") == 0) {
        return Color{118, 86, 72, 226};
    }
    if (std::strcmp(locationName, "Vent Galleries") == 0) {
        return Color{134, 96, 80, 226};
    }
    if (std::strcmp(locationName, "Service Shafts") == 0) {
        return Color{148, 108, 92, 226};
    }
    if (std::strcmp(locationName, "Purifier Ring") == 0) {
        return Color{160, 118, 100, 226};
    }
    if (std::strcmp(locationName, "Root Vault") == 0) {
        return Color{174, 130, 112, 226};
    }
    if (std::strcmp(locationName, "Outer Swamp Rim") == 0) {
        return Color{120, 154, 86, 226};
    }
    if (std::strcmp(locationName, "Flooded Detour") == 0) {
        return Color{132, 146, 88, 226};
    }
    if (std::strcmp(locationName, "Deep Gate") == 0) {
        return Color{128, 126, 70, 226};
    }
    if (std::strcmp(locationName, "Deep Basin") == 0) {
        return Color{88, 96, 52, 226};
    }

    area = Map_GetAreaAt(gridX, gridY);
    switch (area) {
        case MAP_AREA_BASE:
            return Color{76, 112, 146, 220};
        case MAP_AREA_FOREST:
            return Color{56, 119, 79, 220};
        case MAP_AREA_SWAMP_OUTER:
            return Color{108, 136, 72, 220};
        case MAP_AREA_SWAMP_DEEP:
            return Color{92, 101, 56, 220};
        case MAP_AREA_RUINS:
            return Color{120, 124, 137, 220};
        case MAP_AREA_BOSS_ARENA:
            return Color{154, 88, 88, 220};
        case MAP_AREA_UNKNOWN:
        default:
            return Color{26, 30, 38, 220};
    }
}

}  // namespace

void MiniMap_Init(MiniMap *minimap) {
    if (!minimap) {
        return;
    }

    std::memset(minimap->explored, 0, sizeof(minimap->explored));
    minimap->isVisible = false;
    minimap->displaySize = MINIMAP_SIZE;
    minimap->rotation = 0.0f;
}

void MiniMap_Update(MiniMap *minimap, const Player *player, const GameMap *map) {
    (void)map;
    int playerRadius;
    int dx;
    int dy;
    int checkX;
    int checkY;

    if (!minimap || !player) {
        return;
    }

    playerRadius = 6;

    for (dx = -playerRadius; dx <= playerRadius; dx++) {
        for (dy = -playerRadius; dy <= playerRadius; dy++) {
            checkX = player->gridX + dx;
            checkY = player->gridY + dy;

            if (Map_IsWithinBounds(checkX, checkY)) {
                minimap->explored[checkY][checkX] = true;
            }
        }
    }

    minimap->rotation += 0.02f;
}

void MiniMap_Draw(const MiniMap *minimap, const Player *player, const GameMap *map, int screenWidth, int screenHeight) {
    int posX;
    int posY;
    int x;
    int y;
    Color playerColor;
    (void)screenHeight;

    if (!minimap || !player) {
        return;
    }
    if (!minimap->isVisible) {
        return;
    }

    posX = screenWidth - MINIMAP_PADDING - MINIMAP_SIZE;
    posY = MINIMAP_PADDING;

    DrawRectangle(posX - 2, posY - 2, MINIMAP_SIZE + 4, MINIMAP_SIZE + 4, Color{0, 0, 0, 180});

    for (y = 0; y < MAP_HEIGHT; y++) {
        for (x = 0; x < MAP_WIDTH; x++) {
            Rectangle rect;

            if (!minimap->explored[y][x]) {
                continue;
            }

            rect.x = posX + x * MINIMAP_TILE_SIZE;
            rect.y = posY + y * MINIMAP_TILE_SIZE;
            rect.width = MINIMAP_TILE_SIZE;
            rect.height = MINIMAP_TILE_SIZE;

            if (x == player->gridX && y == player->gridY) {
                playerColor = Color{0, 255, 0, 255};
                DrawRectangleRec(rect, playerColor);
            } else {
                DrawRectangleRec(rect, GetMiniMapTileColor(map, x, y));
            }
        }
    }

    DrawRectangleLines(posX - 2, posY - 2, MINIMAP_SIZE + 4, MINIMAP_SIZE + 4, Color{200, 200, 200, 255});
}

void MiniMap_Toggle(MiniMap *minimap) {
    if (!minimap) {
        return;
    }

    minimap->isVisible = !minimap->isVisible;
}

bool MiniMap_IsVisible(const MiniMap *minimap) {
    if (!minimap) {
        return false;
    }

    return minimap->isVisible;
}
