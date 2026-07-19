#include "map_render_internal.h"

void MapInternal_DrawGroundDetailOverlay(TileType groundTile,
                                         int gridX,
                                         int gridY,
                                         Rectangle rect,
                                         const char *locationName,
                                         const char *roomName,
                                         float elapsedSeconds) {
    switch (groundTile) {
        case TILE_BASE_FLOOR:
            MapInternal_DrawShipGroundDetails(gridX, gridY, rect, elapsedSeconds);
            break;
        case TILE_FOREST_GROUND:
            MapInternal_DrawForestGroundDetails(gridX, gridY, rect, locationName, elapsedSeconds);
            break;
        case TILE_SWAMP_GROUND:
        case TILE_DEEP_SWAMP_GROUND:
            MapInternal_DrawSwampGroundDetails(groundTile, gridX, gridY, rect, locationName, elapsedSeconds);
            break;
        case TILE_RUINS_GROUND:
            MapInternal_DrawRuinsGroundDetails(gridX, gridY, rect, locationName, elapsedSeconds);
            break;
        case TILE_VOID:
        default:
            (void)roomName;
            break;
    }
}

