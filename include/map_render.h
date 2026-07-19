#ifndef MAP_RENDER_H
#define MAP_RENDER_H

#include "c_compat.h"
#include "map.h"

typedef struct AssetBundle AssetBundle;

/* Map drawing and render-space geometry. Runtime queries remain in map.h. */

SCL_EXTERN_C_BEGIN

void Map_Draw(const GameMap *map,
              const AssetBundle *assets,
              Camera2D camera,
              int screenWidth,
              int screenHeight,
              float elapsedSeconds);
Rectangle Map_GridToRect(int gridX, int gridY);

SCL_EXTERN_C_END

#endif
