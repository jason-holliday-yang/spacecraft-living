#include "map_internal.h"

#include <math.h>
#include <stddef.h>
#include <string.h>

static float TileScale(float value) {
    return value * ((float)TILE_SIZE / 64.0f);
}

static const TextureAsset *GetShipFloorAsset(const AssetBundle *assets, const char *roomName) {
    if (assets == NULL || roomName == NULL) {
        return NULL;
    }

    if (strcmp(roomName, "Central Corridor") == 0 && assets->tileShipCorridorFloor.loaded) {
        return &assets->tileShipCorridorFloor;
    }
    if (assets->tileShipRoomFloor.loaded) {
        return &assets->tileShipRoomFloor;
    }
    return &assets->tileBaseFloor;
}

static const TextureAsset *GetForestFloorAsset(const AssetBundle *assets, const char *locationName) {
    if (assets == NULL) {
        return NULL;
    }

    if (locationName != NULL
        && strcmp(locationName, "Echo Basin") == 0
        && assets->tileEchoBasinFloor.loaded) {
        return &assets->tileEchoBasinFloor;
    }
    return &assets->tileForestGround;
}

static const TextureAsset *GetSwampFloorAsset(const AssetBundle *assets, const char *locationName, bool deepLayer) {
    if (assets == NULL) {
        return NULL;
    }

    if (deepLayer) {
        if (assets->tileSwampDeepGround.loaded) {
            return &assets->tileSwampDeepGround;
        }
        return &assets->tileSwampGround;
    }

    if (locationName != NULL
        && (strcmp(locationName, "Outer Swamp Rim") == 0
            || strcmp(locationName, "Flooded Detour") == 0)
        && assets->tileSwampOuterGround.loaded) {
        return &assets->tileSwampOuterGround;
    }
    return &assets->tileSwampGround;
}

static const TextureAsset *GetRuinsFloorAsset(const AssetBundle *assets, const char *locationName) {
    if (assets == NULL) {
        return NULL;
    }

    if (locationName != NULL) {
        if (strcmp(locationName, "Ruins Approach") == 0 && assets->tileRuinsApproachFloor.loaded) {
            return &assets->tileRuinsApproachFloor;
        }
        if (strcmp(locationName, "Monolith Ring") == 0 && assets->tileRuinsRingFloor.loaded) {
            return &assets->tileRuinsRingFloor;
        }
        if (strcmp(locationName, "Signal Tower Plateau") == 0 && assets->tileTowerPlateauFloor.loaded) {
            return &assets->tileTowerPlateauFloor;
        }
    }
    return &assets->tileRuinsFloor;
}

static void DrawCrashSiteVoid(Rectangle rect, int gridX, int gridY, float elapsedSeconds) {
    Color base;
    Color accent;
    float pulse;

    pulse = (sinf(elapsedSeconds * 1.7f + (float)(gridX * 2 + gridY)) + 1.0f) * 0.5f;
    base = (Color){18, 22, 28, 255};
    accent = (Color){52, 58, 66, (unsigned char)(90 + pulse * 35.0f)};

    DrawRectangleRec(rect, base);
    DrawRectangle((int)rect.x + 6, (int)rect.y + 6, (int)rect.width - 12, (int)rect.height - 12, (Color){25, 31, 39, 255});
    if (((gridX + gridY) & 1) == 0) {
        DrawCircle((int)(rect.x + rect.width * 0.33f), (int)(rect.y + rect.height * 0.62f), TileScale(7.0f), accent);
        DrawCircle((int)(rect.x + rect.width * 0.68f), (int)(rect.y + rect.height * 0.38f), TileScale(4.5f), (Color){71, 77, 86, 120});
    }
    DrawRectangleLinesEx(rect, TileScale(1.0f), (Color){88, 96, 108, 30});
}

static void DrawShipHullVoid(Rectangle rect, int gridX, int gridY, float elapsedSeconds) {
    float glow;

    glow = 48.0f + (sinf(elapsedSeconds * 2.1f + (float)(gridX - gridY)) + 1.0f) * 18.0f;

    DrawRectangleRec(rect, (Color){11, 16, 24, 255});
    DrawRectangle((int)rect.x + 4, (int)rect.y + 4, (int)rect.width - 8, (int)rect.height - 8, (Color){20, 29, 41, 255});
    DrawRectangleLinesEx(rect, TileScale(1.0f), (Color){110, 136, 162, 65});
    DrawLineEx(
        (Vector2){rect.x + TileScale(10.0f), rect.y + TileScale(18.0f)},
        (Vector2){rect.x + rect.width - TileScale(10.0f), rect.y + TileScale(18.0f)},
        TileScale(2.0f),
        (Color){70, 86, 105, 120}
    );
    DrawLineEx(
        (Vector2){rect.x + TileScale(10.0f), rect.y + rect.height - TileScale(18.0f)},
        (Vector2){rect.x + rect.width - TileScale(10.0f), rect.y + rect.height - TileScale(18.0f)},
        TileScale(2.0f),
        (Color){70, 86, 105, 120}
    );
    if (((gridX + gridY) & 1) == 0) {
        DrawCircle((int)(rect.x + rect.width * 0.5f), (int)(rect.y + rect.height * 0.5f), TileScale(4.0f), (Color){136, 182, 214, (unsigned char)glow});
    }
}

const TextureAsset *MapInternal_GetGroundTextureAsset(const AssetBundle *assets,
                                                      TileType groundTile,
                                                      const char *locationName,
                                                      const char *roomName) {
    switch (groundTile) {
        case TILE_BASE_FLOOR:
            return GetShipFloorAsset(assets, roomName);
        case TILE_FOREST_GROUND:
            return GetForestFloorAsset(assets, locationName);
        case TILE_SWAMP_GROUND:
            return GetSwampFloorAsset(assets, locationName, false);
        case TILE_DEEP_SWAMP_GROUND:
            return GetSwampFloorAsset(assets, locationName, true);
        case TILE_RUINS_GROUND:
            return GetRuinsFloorAsset(assets, locationName);
        case TILE_VOID:
        default:
            return NULL;
    }
}

void MapInternal_DrawVoidGroundBackdrop(int gridX, int gridY, Rectangle rect, float elapsedSeconds) {
    if (MapInternal_IsShipRoomBounds(gridX, gridY)) {
        DrawShipHullVoid(rect, gridX, gridY, elapsedSeconds);
    } else if (MapInternal_IsCrashSiteVoidBounds(gridX, gridY)) {
        DrawCrashSiteVoid(rect, gridX, gridY, elapsedSeconds);
    } else {
        DrawRectangleRec(rect, (Color){5, 9, 15, 255});
    }
}
