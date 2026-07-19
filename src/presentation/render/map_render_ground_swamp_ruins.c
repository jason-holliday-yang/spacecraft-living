#include "map_render_internal.h"

#include <math.h>
#include <stddef.h>
#include <string.h>

static float TileScale(float value) {
    return value * ((float)TILE_SIZE / 64.0f);
}

static void DrawSwampDetails(int gridX, int gridY, Rectangle rect, bool deepLayer, float elapsedSeconds) {
    Color ooze;
    float pulse;

    pulse = (sinf(elapsedSeconds * 2.0f + (float)(gridX + gridY)) + 1.0f) * 0.5f;
    ooze = deepLayer ? (Color){147, 188, 84, 80 + (int)(pulse * 40.0f)} : (Color){118, 148, 75, 70 + (int)(pulse * 35.0f)};

    DrawEllipse((int)(rect.x + rect.width * 0.34f), (int)(rect.y + rect.height * 0.62f), TileScale(9.0f), TileScale(6.0f), ooze);
    if ((gridX + gridY) % 3 == 0) {
        DrawEllipse((int)(rect.x + rect.width * 0.70f), (int)(rect.y + rect.height * 0.35f), TileScale(6.0f), TileScale(4.0f), (Color){76, 96, 54, 90});
    }
    if ((gridX * 3 + gridY) % 4 == 0) {
        DrawLineEx(
            (Vector2){rect.x + 12.0f, rect.y + 14.0f},
            (Vector2){rect.x + 18.0f, rect.y + 48.0f},
            TileScale(2.0f),
            (Color){83, 110, 61, 120}
        );
    }
}

static void DrawOuterSwampRimDetails(int gridX, int gridY, Rectangle rect, float elapsedSeconds) {
    float pulse;

    pulse = (sinf(elapsedSeconds * 2.1f + (float)(gridX + gridY)) + 1.0f) * 0.5f;
    DrawRectangle((int)rect.x + 14, (int)rect.y + 12, (int)rect.width - 28, 8, (Color){184, 202, 112, 60 + (int)(pulse * 40.0f)});
    DrawRectangle((int)rect.x + 20, (int)rect.y + 28, (int)rect.width - 40, 6, (Color){208, 220, 146, 55});
    if ((gridX + gridY) % 2 == 0) {
        DrawLineEx(
            (Vector2){rect.x + rect.width * 0.30f, rect.y + 14.0f},
            (Vector2){rect.x + rect.width * 0.50f, rect.y + rect.height - 10.0f},
            TileScale(2.0f),
            (Color){218, 226, 164, 85}
        );
    }
}

static void DrawFloodedDetourDetails(int gridX, int gridY, Rectangle rect, float elapsedSeconds) {
    float pulse;

    pulse = (sinf(elapsedSeconds * 1.8f + (float)(gridX * 2 - gridY)) + 1.0f) * 0.5f;
    DrawEllipse((int)(rect.x + rect.width * 0.50f), (int)(rect.y + rect.height * 0.54f), TileScale(14.0f), TileScale(9.0f), (Color){82, 104, 76, 95});
    DrawCircleLines((int)(rect.x + rect.width * 0.50f), (int)(rect.y + rect.height * 0.54f), TileScale(10.0f), (Color){174, 192, 126, 60 + (int)(pulse * 40.0f)});
    if ((gridX + gridY) % 3 == 0) {
        DrawLineEx(
            (Vector2){rect.x + 14.0f, rect.y + 18.0f},
            (Vector2){rect.x + rect.width - 14.0f, rect.y + rect.height - 18.0f},
            TileScale(2.0f),
            (Color){122, 140, 88, 110}
        );
    }
}

static void DrawOuterSwampShelfDetails(int gridX, int gridY, Rectangle rect) {
    if ((gridX + gridY) % 2 == 0) {
        DrawRectangle((int)rect.x + 12, (int)rect.y + 18, (int)rect.width - 24, 8, (Color){120, 145, 78, 95});
        DrawRectangle((int)rect.x + 18, (int)rect.y + 34, (int)rect.width - 36, 6, (Color){146, 170, 94, 75});
    }
}

static void DrawDeepGateDetails(int gridX, int gridY, Rectangle rect, float elapsedSeconds) {
    float pulse;

    pulse = (sinf(elapsedSeconds * 2.7f + (float)(gridX - gridY)) + 1.0f) * 0.5f;
    DrawRectangle((int)rect.x + 10, (int)rect.y + 10, (int)rect.width - 20, (int)rect.height - 20, (Color){98, 120, 60, 70});
    DrawRectangleLinesEx((Rectangle){rect.x + 10.0f, rect.y + 10.0f, rect.width - 20.0f, rect.height - 20.0f},
                         TileScale(1.0f),
                         (Color){214, 186, 96, 70 + (int)(pulse * 45.0f)});
    DrawLineEx(
        (Vector2){rect.x + rect.width * 0.28f, rect.y + 14.0f},
        (Vector2){rect.x + rect.width * 0.72f, rect.y + rect.height - 14.0f},
        TileScale(2.0f),
        (Color){204, 174, 88, 95}
    );
}

static void DrawToxicChannelDetails(int gridX, int gridY, Rectangle rect, float elapsedSeconds) {
    float pulse;

    pulse = (sinf(elapsedSeconds * 2.2f + (float)(gridX + gridY)) + 1.0f) * 0.5f;
    DrawRectangle((int)rect.x + 20, (int)rect.y + 8, (int)rect.width - 40, (int)rect.height - 16, (Color){86, 108, 44, 60});
    DrawCircle((int)(rect.x + rect.width * 0.50f), (int)(rect.y + rect.height * 0.50f), TileScale(6.0f) + pulse * TileScale(2.0f), (Color){170, 210, 82, 70});
}

static void DrawDeepBasinDetails(int gridX, int gridY, Rectangle rect, float elapsedSeconds) {
    float pulse;

    pulse = (sinf(elapsedSeconds * 3.1f + (float)(gridX * 2 + gridY)) + 1.0f) * 0.5f;
    DrawEllipse((int)(rect.x + rect.width * 0.50f), (int)(rect.y + rect.height * 0.56f), TileScale(16.0f), TileScale(10.0f), (Color){82, 94, 40, 110});
    DrawCircleLines((int)(rect.x + rect.width * 0.50f), (int)(rect.y + rect.height * 0.56f), TileScale(11.0f), (Color){182, 216, 84, 70 + (int)(pulse * 50.0f)});
    if ((gridX + gridY) % 2 == 0) {
        DrawCircle((int)(rect.x + rect.width * 0.34f), (int)(rect.y + rect.height * 0.36f), TileScale(4.0f), (Color){198, 228, 108, 70});
    }
}

static void DrawRuinsDetails(int gridX, int gridY, Rectangle rect, float elapsedSeconds) {
    float glyphGlow;

    glyphGlow = 70.0f + (sinf(elapsedSeconds * 1.6f + (float)(gridX * 2 - gridY)) + 1.0f) * 18.0f;

    if ((gridX + gridY) % 2 == 0) {
        DrawLineEx(
            (Vector2){rect.x + 10.0f, rect.y + 12.0f},
            (Vector2){rect.x + 26.0f, rect.y + 28.0f},
            TileScale(2.0f),
            (Color){116, 122, 133, 120}
        );
        DrawLineEx(
            (Vector2){rect.x + 26.0f, rect.y + 28.0f},
            (Vector2){rect.x + 48.0f, rect.y + 50.0f},
            TileScale(2.0f),
            (Color){96, 104, 117, 110}
        );
    }
    if ((gridX * 2 + gridY) % 5 == 0) {
        DrawRectangle((int)rect.x + 22, (int)rect.y + 18, 20, 20, (Color){82, 86, 94, 110});
        DrawRectangleLinesEx((Rectangle){rect.x + 22.0f, rect.y + 18.0f, 20.0f, 20.0f}, TileScale(1.0f), (Color){101, 193, 209, (unsigned char)glyphGlow});
    }
}

static void DrawRuinsApproachDetails(int gridX, int gridY, Rectangle rect) {
    if (gridX >= 80 && gridX <= 92) {
        DrawRectangle((int)rect.x + 16, (int)rect.y + 10, (int)rect.width - 32, (int)rect.height - 20, (Color){142, 148, 158, 70});
    }
    if ((gridY & 1) == 0) {
        DrawLineEx(
            (Vector2){rect.x + rect.width * 0.28f, rect.y + 12.0f},
            (Vector2){rect.x + rect.width * 0.50f, rect.y + rect.height - 12.0f},
            TileScale(2.0f),
            (Color){170, 178, 188, 85}
        );
        DrawLineEx(
            (Vector2){rect.x + rect.width * 0.72f, rect.y + 12.0f},
            (Vector2){rect.x + rect.width * 0.50f, rect.y + rect.height - 12.0f},
            TileScale(2.0f),
            (Color){170, 178, 188, 85}
        );
    }
}

static void DrawMonolithRingDetails(int gridX, int gridY, Rectangle rect, float elapsedSeconds) {
    float pulse;

    pulse = (sinf(elapsedSeconds * 2.0f + (float)(gridX + gridY)) + 1.0f) * 0.5f;
    DrawCircleLines((int)(rect.x + rect.width * 0.5f),
                    (int)(rect.y + rect.height * 0.5f),
                    TileScale(18.0f),
                    (Color){132, 204, 220, 55 + (int)(pulse * 50.0f)});
    if ((gridX + gridY) % 3 == 0) {
        DrawCircleLines((int)(rect.x + rect.width * 0.5f),
                        (int)(rect.y + rect.height * 0.5f),
                        TileScale(8.0f),
                        (Color){184, 208, 220, 70});
    }
}

static void DrawTowerPlateauDetails(int gridX, int gridY, Rectangle rect, float elapsedSeconds) {
    float pulse;

    pulse = (sinf(elapsedSeconds * 2.6f + (float)(gridX * 2 - gridY)) + 1.0f) * 0.5f;
    DrawRectangle((int)rect.x + 10, (int)rect.y + 10, (int)rect.width - 20, (int)rect.height - 20, (Color){98, 108, 122, 110});
    DrawRectangleLinesEx((Rectangle){rect.x + 10.0f, rect.y + 10.0f, rect.width - 20.0f, rect.height - 20.0f},
                         TileScale(1.5f),
                         (Color){144, 216, 232, 70 + (int)(pulse * 60.0f)});
    if ((gridX + gridY) % 2 == 0) {
        DrawRectangle((int)rect.x + 18, (int)(rect.y + rect.height * 0.45f), (int)rect.width - 36, 6, (Color){188, 221, 232, 95});
    }
}

static void DrawVentGalleriesDetails(int gridX, int gridY, Rectangle rect, float elapsedSeconds) {
    float pulse;

    pulse = (sinf(elapsedSeconds * 2.0f + (float)(gridX + gridY)) + 1.0f) * 0.5f;
    DrawRectangle((int)rect.x + 14, (int)rect.y + 14, (int)rect.width - 28, (int)rect.height - 28, (Color){104, 94, 88, 110});
    DrawLineEx(
        (Vector2){rect.x + rect.width * 0.32f, rect.y + 12.0f},
        (Vector2){rect.x + rect.width * 0.32f, rect.y + rect.height - 12.0f},
        TileScale(2.0f),
        (Color){188, 160, 118, 70 + (int)(pulse * 40.0f)}
    );
    DrawLineEx(
        (Vector2){rect.x + rect.width * 0.68f, rect.y + 12.0f},
        (Vector2){rect.x + rect.width * 0.68f, rect.y + rect.height - 12.0f},
        TileScale(2.0f),
        (Color){188, 160, 118, 70 + (int)(pulse * 40.0f)}
    );
}

static void DrawServiceShaftsDetails(int gridX, int gridY, Rectangle rect) {
    (void)gridX;
    (void)gridY;

    DrawRectangle((int)rect.x + 20, (int)rect.y + 8, (int)rect.width - 40, (int)rect.height - 16, (Color){88, 84, 80, 110});
    DrawRectangle((int)rect.x + 28, (int)rect.y + 14, 8, (int)rect.height - 28, (Color){146, 168, 178, 90});
    DrawRectangle((int)rect.x + (int)rect.width - 36, (int)rect.y + 14, 8, (int)rect.height - 28, (Color){146, 168, 178, 90});
}

static void DrawPurifierRingDetails(int gridX, int gridY, Rectangle rect, float elapsedSeconds) {
    float pulse;

    pulse = (sinf(elapsedSeconds * 2.8f + (float)(gridX * 2 - gridY)) + 1.0f) * 0.5f;
    DrawCircleLines((int)(rect.x + rect.width * 0.5f),
                    (int)(rect.y + rect.height * 0.5f),
                    TileScale(16.0f),
                    (Color){170, 206, 196, 70 + (int)(pulse * 55.0f)});
    DrawCircle((int)(rect.x + rect.width * 0.5f),
               (int)(rect.y + rect.height * 0.5f),
               TileScale(6.0f),
               (Color){120, 164, 148, 110});
}

static void DrawRootVaultDetails(int gridX, int gridY, Rectangle rect, float elapsedSeconds) {
    float pulse;

    pulse = (sinf(elapsedSeconds * 2.4f + (float)(gridX + gridY * 2)) + 1.0f) * 0.5f;
    DrawRectangle((int)rect.x + 10, (int)rect.y + 16, (int)rect.width - 20, (int)rect.height - 32, (Color){72, 62, 54, 118});
    DrawRectangleLinesEx((Rectangle){rect.x + 10.0f, rect.y + 16.0f, rect.width - 20.0f, rect.height - 32.0f},
                         TileScale(1.0f),
                         (Color){184, 156, 120, 70 + (int)(pulse * 48.0f)});
    DrawLineEx(
        (Vector2){rect.x + 14.0f, rect.y + rect.height - 18.0f},
        (Vector2){rect.x + rect.width - 14.0f, rect.y + 18.0f},
        TileScale(2.0f),
        (Color){114, 152, 102, 95}
    );
}

void MapInternal_DrawSwampGroundDetails(TileType groundTile,
                                        int gridX,
                                        int gridY,
                                        Rectangle rect,
                                        const char *locationName,
                                        float elapsedSeconds) {
    if (groundTile == TILE_DEEP_SWAMP_GROUND) {
        DrawSwampDetails(gridX, gridY, rect, true, elapsedSeconds);
        if (locationName != NULL && strcmp(locationName, "Deep Gate") == 0) {
            DrawDeepGateDetails(gridX, gridY, rect, elapsedSeconds);
            if (gridY > 63) {
                DrawToxicChannelDetails(gridX, gridY, rect, elapsedSeconds);
            }
        } else if (locationName != NULL && strcmp(locationName, "Deep Basin") == 0) {
            DrawDeepBasinDetails(gridX, gridY, rect, elapsedSeconds);
        }
        return;
    }

    DrawSwampDetails(gridX, gridY, rect, false, elapsedSeconds);
    if (locationName != NULL && strcmp(locationName, "Outer Swamp Rim") == 0) {
        DrawOuterSwampRimDetails(gridX, gridY, rect, elapsedSeconds);
    } else if (locationName != NULL && strcmp(locationName, "Flooded Detour") == 0) {
        DrawFloodedDetourDetails(gridX, gridY, rect, elapsedSeconds);
        if (gridX < 124) {
            DrawOuterSwampShelfDetails(gridX, gridY, rect);
        }
    }
}

void MapInternal_DrawRuinsGroundDetails(int gridX,
                                        int gridY,
                                        Rectangle rect,
                                        const char *locationName,
                                        float elapsedSeconds) {
    if (locationName != NULL && strcmp(locationName, "Vent Galleries") == 0) {
        DrawVentGalleriesDetails(gridX, gridY, rect, elapsedSeconds);
        return;
    }
    if (locationName != NULL && strcmp(locationName, "Service Shafts") == 0) {
        DrawServiceShaftsDetails(gridX, gridY, rect);
        return;
    }
    if (locationName != NULL && strcmp(locationName, "Purifier Ring") == 0) {
        DrawPurifierRingDetails(gridX, gridY, rect, elapsedSeconds);
        return;
    }
    if (locationName != NULL && strcmp(locationName, "Root Vault") == 0) {
        DrawRootVaultDetails(gridX, gridY, rect, elapsedSeconds);
        return;
    }

    DrawRuinsDetails(gridX, gridY, rect, elapsedSeconds);
    if (locationName != NULL && strcmp(locationName, "Ruins Approach") == 0) {
        DrawRuinsApproachDetails(gridX, gridY, rect);
    } else if (locationName != NULL && strcmp(locationName, "Monolith Ring") == 0) {
        DrawMonolithRingDetails(gridX, gridY, rect, elapsedSeconds);
    } else if (locationName != NULL && strcmp(locationName, "Signal Tower Plateau") == 0) {
        DrawTowerPlateauDetails(gridX, gridY, rect, elapsedSeconds);
    }
}
