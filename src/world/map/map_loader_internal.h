#ifndef MAP_LOADER_INTERNAL_H
#define MAP_LOADER_INTERNAL_H

#include "map.h"

/* Tiled parsing boundary. Allocation and map lifecycle stay in map_layout.c. */
SCL_EXTERN_C_BEGIN

bool MapInternal_LoadTiledMap(GameMap *map, const char *relativePath);

SCL_EXTERN_C_END

#endif
