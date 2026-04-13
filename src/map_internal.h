#ifndef MAP_INTERNAL_H
#define MAP_INTERNAL_H

#include "map.h"

/* Internal map helpers shared by layout seeding and rendering slices. */

SCL_EXTERN_C_BEGIN

bool MapInternal_IsShipRoomBounds(int gridX, int gridY);
bool MapInternal_IsCrashSiteVoidBounds(int gridX, int gridY);
bool MapInternal_IsNaturalGroundTile(TileType tile);
void MapInternal_FillGroundRect(GameMap *map, int x, int y, int width, int height, TileType tile);
void MapInternal_FillPropRect(GameMap *map, int x, int y, int width, int height, TileType tile);
void MapInternal_SetPerimeterProp(GameMap *map, int x, int y, TileType tile);
void MapInternal_SetGroundTile(GameMap *map, int x, int y, TileType tile);
void MapInternal_SetPropTile(GameMap *map, int x, int y, TileType tile);
void MapInternal_SetNaturalPropTile(GameMap *map, int x, int y, TileType tile);
void MapInternal_SeedWorldLayout(GameMap *map);
void MapInternal_SeedShipLayout(GameMap *map);
bool MapInternal_GetMultiTilePropBounds(TileType tile,
                                        int gridX,
                                        int gridY,
                                        int *originX,
                                        int *originY,
                                        int *width,
                                        int *height);
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
void MapInternal_DrawPropCell(const GameMap *map,
                              const AssetBundle *assets,
                              int gridX,
                              int gridY,
                              Rectangle rect,
                              float elapsedSeconds);
void MapInternal_DrawHazardCell(HazardType hazard, Rectangle rect, float elapsedSeconds);

SCL_EXTERN_C_END

#endif
