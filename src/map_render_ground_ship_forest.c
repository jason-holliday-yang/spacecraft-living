#include "map_internal.h"

#include <math.h>
#include <stddef.h>
#include <string.h>

static float WaveValue(float t, float speed, float amount) {
    return sinf(t * speed) * amount;
}

static float TileScale(float value) {
    return value * ((float)TILE_SIZE / 64.0f);
}

static void DrawShipDeckDetails(int gridX, int gridY, Rectangle rect, float elapsedSeconds) {
    const char *roomName;
    int hatchInset;

    roomName = Map_GetRoomNameAt(gridX, gridY);
    hatchInset = (int)TileScale(6.0f);

    DrawRectangleLinesEx(rect, TileScale(1.0f), (Color){94, 115, 136, 28});

    if (((gridX + gridY) & 1) == 0) {
        DrawRectangle((int)rect.x + 6, (int)rect.y + 6, (int)rect.width - 12, 4, (Color){73, 89, 107, 70});
    }

    if (strcmp(roomName, "Central Corridor") == 0) {
        DrawRectangle((int)rect.x + 8, (int)(rect.y + rect.height * 0.28f), (int)rect.width - 16, 4, (Color){199, 168, 77, 130});
        DrawRectangle((int)rect.x + 8, (int)(rect.y + rect.height * 0.72f), (int)rect.width - 16, 4, (Color){199, 168, 77, 130});
        if ((gridX + gridY) % 2 == 0) {
            DrawRectangle((int)rect.x + 18, (int)rect.y + 12, (int)rect.width - 36, (int)rect.height - 24, (Color){54, 72, 91, 120});
        }
        return;
    }

    if (strcmp(roomName, "Cargo Hold") == 0) {
        if ((gridY + gridX) % 2 == 0) {
            DrawRectangle((int)rect.x + 10, (int)rect.y + 10, (int)rect.width - 20, (int)rect.height - 20, (Color){71, 84, 101, 160});
        }
        if (gridY % 3 == 0) {
            DrawRectangle((int)rect.x + 16, (int)rect.y + 18, (int)rect.width - 32, 12, (Color){117, 129, 144, 180});
        }
        return;
    }

    if (strcmp(roomName, "Crew Quarters") == 0) {
        DrawRectangle((int)rect.x + 12, (int)rect.y + 16, (int)rect.width - 24, (int)rect.height - 30, (Color){108, 118, 132, 155});
        DrawRectangle((int)rect.x + 16, (int)rect.y + 20, (int)rect.width - 32, 8, (Color){200, 208, 214, 155});
        return;
    }

    if (strcmp(roomName, "Diagnostics") == 0) {
        if ((gridX + gridY) % 2 == 0) {
            DrawRectangle((int)rect.x + 14, (int)rect.y + 12, (int)rect.width - 28, (int)rect.height - 24, (Color){86, 103, 120, 165});
        }
        DrawRectangle((int)(rect.x + rect.width * 0.48f) - 3, (int)rect.y + 14, 6, (int)rect.height - 28, (Color){168, 213, 232, 165});
        DrawRectangle((int)rect.x + 14, (int)(rect.y + rect.height * 0.48f) - 3, (int)rect.width - 28, 6, (Color){168, 213, 232, 165});
        return;
    }

    if (strcmp(roomName, "Terminal Bay") == 0) {
        if ((gridX + gridY) % 2 == 0) {
            DrawRectangleRounded((Rectangle){rect.x + 10.0f, rect.y + 12.0f, rect.width - 20.0f, rect.height - 24.0f}, 0.18f, 4, (Color){69, 96, 118, 170});
        }
        DrawCircle((int)(rect.x + rect.width * 0.5f),
                   (int)(rect.y + 18.0f),
                   TileScale(4.0f) + WaveValue(elapsedSeconds + (float)(gridY + gridX), 2.8f, TileScale(0.8f)),
                   (Color){112, 221, 255, 120});
        return;
    }

    if (strcmp(roomName, "Life Support") == 0) {
        DrawRectangle((int)rect.x + 10, (int)rect.y + 18, (int)rect.width - 20, (int)rect.height - 36, (Color){75, 105, 124, 170});
        DrawRectangle((int)rect.x + 14, (int)rect.y + 24, (int)rect.width - 28, 6, (Color){154, 198, 218, 170});
        DrawRectangle((int)rect.x + 14, (int)rect.y + 36, (int)rect.width - 28, 6, (Color){154, 198, 218, 170});
        return;
    }

    if (strcmp(roomName, "Workshop") == 0) {
        if ((gridX + gridY) % 2 == 0) {
            DrawRectangle((int)rect.x + 10, (int)rect.y + 22, (int)rect.width - 20, 12, (Color){128, 95, 63, 180});
        }
        DrawRectangle((int)rect.x + 18, (int)rect.y + 12, (int)rect.width - 36, (int)rect.height - 24, (Color){63, 82, 95, 105});
        return;
    }

    if (strcmp(roomName, "Power Bay") == 0) {
        DrawRectangle((int)rect.x + 12, (int)rect.y + 12, (int)rect.width - 24, (int)rect.height - 24, (Color){88, 82, 68, 155});
        DrawRectangle((int)rect.x + 16, (int)rect.y + 20, (int)rect.width - 32, 8, (Color){214, 178, 96, 165});
        DrawRectangle((int)rect.x + 16, (int)rect.y + 36, (int)rect.width - 32, 6, (Color){156, 126, 72, 150});
        return;
    }

    if (strcmp(roomName, "Airlock Link") == 0) {
        DrawRectangleLinesEx((Rectangle){rect.x + hatchInset, rect.y + hatchInset, rect.width - hatchInset * 2.0f, rect.height - hatchInset * 2.0f}, TileScale(2.0f), (Color){174, 192, 209, 85});
        if ((gridX + gridY) % 2 == 0) {
            DrawTriangle(
                (Vector2){rect.x + 10.0f, rect.y + 46.0f},
                (Vector2){rect.x + 22.0f, rect.y + 18.0f},
                (Vector2){rect.x + 34.0f, rect.y + 46.0f},
                (Color){226, 176, 73, 150}
            );
            DrawTriangle(
                (Vector2){rect.x + 54.0f, rect.y + 18.0f},
                (Vector2){rect.x + 42.0f, rect.y + 46.0f},
                (Vector2){rect.x + 30.0f, rect.y + 18.0f},
                (Color){226, 176, 73, 150}
            );
        }
    }
}

static void DrawForestDetails(int gridX, int gridY, Rectangle rect, float elapsedSeconds) {
    float sway;

    sway = WaveValue(elapsedSeconds + (float)(gridX * 3 + gridY), 1.4f, TileScale(2.0f));

    if ((gridX + gridY) % 4 == 0) {
        DrawCircle((int)(rect.x + rect.width * 0.26f), (int)(rect.y + rect.height * 0.70f), TileScale(5.0f), (Color){43, 95, 64, 100});
        DrawCircle((int)(rect.x + rect.width * 0.62f), (int)(rect.y + rect.height * 0.42f), TileScale(4.0f), (Color){64, 122, 82, 90});
    }
    if ((gridX * 2 + gridY) % 5 == 0) {
        DrawLineEx(
            (Vector2){rect.x + 14.0f + sway * 0.2f, rect.y + 48.0f},
            (Vector2){rect.x + 20.0f + sway, rect.y + 18.0f},
            TileScale(2.0f),
            (Color){87, 140, 91, 110}
        );
    }
    if ((gridX + gridY) % 7 == 0) {
        DrawCircle((int)(rect.x + rect.width * 0.78f), (int)(rect.y + rect.height * 0.72f), TileScale(3.0f), (Color){112, 164, 84, 130});
    }
}

static void DrawWestFrontierDetails(int gridX, int gridY, Rectangle rect, float elapsedSeconds) {
    float pulse;

    pulse = (sinf(elapsedSeconds * 1.9f + (float)(gridX - gridY)) + 1.0f) * 0.5f;

    DrawRectangleLinesEx(rect, TileScale(1.0f), (Color){124, 138, 154, 46});
    if ((gridY + gridX) % 5 == 0) {
        DrawLineEx(
            (Vector2){rect.x + 12.0f, rect.y + 16.0f},
            (Vector2){rect.x + 24.0f, rect.y + 48.0f},
            TileScale(2.0f),
            (Color){128, 112, 88, 150}
        );
        DrawLineEx(
            (Vector2){rect.x + 40.0f, rect.y + 16.0f},
            (Vector2){rect.x + 28.0f, rect.y + 48.0f},
            TileScale(2.0f),
            (Color){128, 112, 88, 150}
        );
    }
    if ((gridX * 2 + gridY) % 6 == 0) {
        DrawCircleLines((int)(rect.x + rect.width * 0.5f),
                        (int)(rect.y + rect.height * 0.5f),
                        TileScale(8.0f) + pulse * TileScale(2.0f),
                        (Color){158, 176, 198, 70 + (int)(pulse * 40.0f)});
    }
}

static void DrawSurveyBreakDetails(int gridX, int gridY, Rectangle rect, float elapsedSeconds) {
    float pulse;

    pulse = (sinf(elapsedSeconds * 2.2f + (float)(gridX + gridY)) + 1.0f) * 0.5f;
    DrawRectangle((int)rect.x + 14, (int)rect.y + 12, (int)rect.width - 28, 10, (Color){132, 118, 92, 140});
    DrawRectangle((int)rect.x + 18, (int)rect.y + 30, (int)rect.width - 36, 8, (Color){168, 178, 184, 70 + (int)(pulse * 44.0f)});
    DrawRectangleLinesEx(rect, TileScale(1.0f), (Color){142, 156, 170, 44});
}

static void DrawCanopyHollowDetails(int gridX, int gridY, Rectangle rect, float elapsedSeconds) {
    float sway;

    sway = WaveValue(elapsedSeconds + (float)(gridX * 2 - gridY), 1.8f, TileScale(3.0f));
    DrawCircle((int)(rect.x + rect.width * 0.34f), (int)(rect.y + rect.height * 0.38f), TileScale(8.0f), (Color){74, 128, 84, 90});
    DrawCircle((int)(rect.x + rect.width * 0.64f), (int)(rect.y + rect.height * 0.64f), TileScale(10.0f), (Color){56, 104, 70, 98});
    DrawLineEx(
        (Vector2){rect.x + 20.0f, rect.y + rect.height - 14.0f},
        (Vector2){rect.x + 22.0f + sway, rect.y + 16.0f},
        TileScale(2.0f),
        (Color){106, 150, 92, 120}
    );
}

static void DrawEchoBasinDetails(int gridX, int gridY, Rectangle rect, float elapsedSeconds) {
    float pulse;

    pulse = (sinf(elapsedSeconds * 2.5f + (float)(gridX * 3 + gridY)) + 1.0f) * 0.5f;
    DrawEllipse((int)(rect.x + rect.width * 0.50f), (int)(rect.y + rect.height * 0.54f), TileScale(14.0f), TileScale(10.0f), (Color){64, 90, 104, 85});
    DrawCircleLines((int)(rect.x + rect.width * 0.50f), (int)(rect.y + rect.height * 0.54f), TileScale(9.0f), (Color){156, 186, 206, 60 + (int)(pulse * 44.0f)});
}

static void DrawLastCampDetails(int gridX, int gridY, Rectangle rect) {
    if ((gridX + gridY) % 2 == 0) {
        DrawTriangle(
            (Vector2){rect.x + 12.0f, rect.y + rect.height - 12.0f},
            (Vector2){rect.x + rect.width * 0.5f, rect.y + 18.0f},
            (Vector2){rect.x + rect.width - 12.0f, rect.y + rect.height - 12.0f},
            (Color){146, 132, 106, 110}
        );
    }
    DrawRectangle((int)rect.x + 18, (int)rect.y + 38, (int)rect.width - 36, 8, (Color){102, 88, 72, 130});
}

static void DrawSouthCollapseDetails(int gridX, int gridY, Rectangle rect, float elapsedSeconds) {
    float pulse;

    pulse = (sinf(elapsedSeconds * 2.3f + (float)(gridX + gridY)) + 1.0f) * 0.5f;

    DrawLineEx(
        (Vector2){rect.x + 8.0f, rect.y + 46.0f},
        (Vector2){rect.x + 24.0f, rect.y + 18.0f},
        TileScale(2.0f),
        (Color){78, 56, 46, 170}
    );
    DrawLineEx(
        (Vector2){rect.x + 24.0f, rect.y + 18.0f},
        (Vector2){rect.x + 54.0f, rect.y + 50.0f},
        TileScale(2.0f),
        (Color){102, 74, 58, 170}
    );
    if ((gridX + gridY) % 3 == 0) {
        DrawRectangle((int)rect.x + 16, (int)rect.y + 10, (int)rect.width - 32, 8, (Color){116, 94, 74, 120});
        DrawRectangle((int)rect.x + 20, (int)rect.y + 22, (int)rect.width - 40, 6, (Color){176, 136, 82, 120 + (int)(pulse * 60.0f)});
    }
    DrawRectangleLinesEx(rect, TileScale(1.0f), (Color){150, 118, 92, 52});
}

void MapInternal_DrawShipGroundDetails(int gridX, int gridY, Rectangle rect, float elapsedSeconds) {
    DrawShipDeckDetails(gridX, gridY, rect, elapsedSeconds);
}

void MapInternal_DrawForestGroundDetails(int gridX,
                                         int gridY,
                                         Rectangle rect,
                                         const char *locationName,
                                         float elapsedSeconds) {
    DrawForestDetails(gridX, gridY, rect, elapsedSeconds);
    if (locationName != NULL && strcmp(locationName, "West Frontier") == 0) {
        DrawWestFrontierDetails(gridX, gridY, rect, elapsedSeconds);
    } else if (locationName != NULL && strcmp(locationName, "Survey Break") == 0) {
        DrawSurveyBreakDetails(gridX, gridY, rect, elapsedSeconds);
    } else if (locationName != NULL && strcmp(locationName, "Canopy Hollow") == 0) {
        DrawCanopyHollowDetails(gridX, gridY, rect, elapsedSeconds);
    } else if (locationName != NULL && strcmp(locationName, "Echo Basin") == 0) {
        DrawEchoBasinDetails(gridX, gridY, rect, elapsedSeconds);
    } else if (locationName != NULL && strcmp(locationName, "Last Camp") == 0) {
        DrawLastCampDetails(gridX, gridY, rect);
    } else if (locationName != NULL && strcmp(locationName, "South Collapse") == 0) {
        DrawSouthCollapseDetails(gridX, gridY, rect, elapsedSeconds);
    }
}
