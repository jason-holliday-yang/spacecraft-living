#ifndef MINIMAP_H
#define MINIMAP_H

#include <stdbool.h>
#include "c_compat.h"
#include "map.h"
#include "player.h"

/* Public minimap state and draw/update helpers for exploration feedback. */

SCL_EXTERN_C_BEGIN

#define MINIMAP_TILE_SIZE 1
#define MINIMAP_SIZE (((MAP_WIDTH) > (MAP_HEIGHT) ? (MAP_WIDTH) : (MAP_HEIGHT)) * (MINIMAP_TILE_SIZE))
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

SCL_EXTERN_C_END

#endif
