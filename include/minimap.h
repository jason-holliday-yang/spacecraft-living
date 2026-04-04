#ifndef MINIMAP_H
#define MINIMAP_H

#include <stdbool.h>
#include "map.h"
#include "player.h"

#define MINIMAP_SIZE 160
#define MINIMAP_TILE_SIZE 4
#define MINIMAP_PADDING 10

typedef struct MiniMap {
    bool explored[MAP_HEIGHT][MAP_WIDTH];
    bool isVisible;
    int displaySize;
    float rotation;
} MiniMap;

void MiniMap_Init(MiniMap *minimap);
void MiniMap_Update(MiniMap *minimap, const Player *player, const GameMap *map);
void MiniMap_Draw(const MiniMap *minimap, const Player *player, const GameMap *map, int screenWidth, int screenHeight);
void MiniMap_Toggle(MiniMap *minimap);
bool MiniMap_IsVisible(const MiniMap *minimap);

#endif
