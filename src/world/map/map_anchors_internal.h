#ifndef MAP_ANCHORS_INTERNAL_H
#define MAP_ANCHORS_INTERNAL_H

#include "map.h"

SCL_EXTERN_C_BEGIN

/* Fixed anchor defaults are only for the explicit legacy fixture. Tiled maps
 * must provide their Anchors object layer.
 */
void MapInternal_RebuildLegacyAnchors(GameMap *map);
void MapInternal_ActivateRuntimeAnchorLayer(const GameMap *map);

SCL_EXTERN_C_END

#endif
