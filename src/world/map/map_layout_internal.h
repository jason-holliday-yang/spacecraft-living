#ifndef MAP_LAYOUT_INTERNAL_H
#define MAP_LAYOUT_INTERNAL_H

#include "map.h"

/* Legacy fixture seeding and low-level layout mutation. Production map
 * content is loaded from Tiled; these helpers exist only for tests and
 * explicit compatibility fallback.
 */
SCL_EXTERN_C_BEGIN

bool MapInternal_IsNaturalGroundTile(TileType tile);
void MapInternal_FillGroundRect(GameMap *map, int x, int y, int width, int height, TileType tile);
void MapInternal_FillPropRect(GameMap *map, int x, int y, int width, int height, TileType tile);
void MapInternal_SetPerimeterProp(GameMap *map, int x, int y, TileType tile);
void MapInternal_SetGroundTile(GameMap *map, int x, int y, TileType tile);
void MapInternal_SetPropTile(GameMap *map, int x, int y, TileType tile);
void MapInternal_SetNaturalPropTile(GameMap *map, int x, int y, TileType tile);
void MapInternal_SeedWorldLayout(GameMap *map);
void MapInternal_SeedShipLayout(GameMap *map);
bool MapInternal_InitLegacyFixture(GameMap *map);

SCL_EXTERN_C_END

#endif
