#ifndef MAP_RENDER_INTERNAL_H
#define MAP_RENDER_INTERNAL_H

#include "assets.h"
#include "map_render.h"

/* Internal map render slices; gameplay/layout helpers remain in map_internal.h. */

SCL_EXTERN_C_BEGIN

bool MapInternal_IsShipRoomBounds(int gridX, int gridY);
bool MapInternal_IsCrashSiteVoidBounds(int gridX, int gridY);
const TextureAsset *MapInternal_GetGroundTextureAsset(const AssetBundle *assets,
                                                      TileType groundTile,
                                                      const char *locationName,
                                                      const char *roomName);
void MapInternal_DrawShipGroundDetails(int gridX, int gridY, Rectangle rect, float elapsedSeconds);
void MapInternal_DrawForestGroundDetails(int gridX,
                                         int gridY,
                                         Rectangle rect,
                                         const char *locationName,
                                         float elapsedSeconds);
void MapInternal_DrawSwampGroundDetails(TileType groundTile,
                                        int gridX,
                                        int gridY,
                                        Rectangle rect,
                                        const char *locationName,
                                        float elapsedSeconds);
void MapInternal_DrawRuinsGroundDetails(int gridX,
                                        int gridY,
                                        Rectangle rect,
                                        const char *locationName,
                                        float elapsedSeconds);
void MapInternal_DrawGroundDetailOverlay(TileType groundTile,
                                         int gridX,
                                         int gridY,
                                         Rectangle rect,
                                         const char *locationName,
                                         const char *roomName,
                                         float elapsedSeconds);
void MapInternal_DrawVoidGroundBackdrop(int gridX, int gridY, Rectangle rect, float elapsedSeconds);
void MapInternal_DrawGroundCell(const GameMap *map,
                                const AssetBundle *assets,
                                int gridX,
                                int gridY,
                                Rectangle rect,
                                float elapsedSeconds);
void MapInternal_DrawDecorCell(MapDecorType decor, Rectangle rect, float elapsedSeconds);
void MapInternal_DrawPropCell(const GameMap *map,
                              const AssetBundle *assets,
                              int gridX,
                              int gridY,
                              Rectangle rect,
                              float elapsedSeconds);
void MapInternal_DrawHazardCell(HazardType hazard, Rectangle rect, float elapsedSeconds);

SCL_EXTERN_C_END

#endif
