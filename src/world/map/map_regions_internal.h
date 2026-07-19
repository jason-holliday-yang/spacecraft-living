#ifndef MAP_REGIONS_INTERNAL_H
#define MAP_REGIONS_INTERNAL_H

#include "map.h"

SCL_EXTERN_C_BEGIN

MapArea MapInternal_GetFallbackAreaAt(int gridX, int gridY);
HazardType MapInternal_GetFallbackHazardAt(const GameMap *map, int gridX, int gridY);
void MapInternal_RebuildDerivedLayers(GameMap *map);
void MapInternal_ActivateRuntimeAreaLayer(const GameMap *map);
void MapInternal_ActivateRuntimeRegionLayer(const GameMap *map);

SCL_EXTERN_C_END

#endif
