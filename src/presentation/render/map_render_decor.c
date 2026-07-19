#include "map_render_internal.h"

#include <math.h>

static float DecorScale(float value) {
    return value * ((float)TILE_SIZE / 96.0f);
}

void MapInternal_DrawDecorCell(MapDecorType decor, Rectangle rect, float elapsedSeconds) {
    float pulse;

    switch (decor) {
        case MAP_DECOR_DEBRIS:
            DrawTriangle((Vector2){rect.x + DecorScale(20.0f), rect.y + DecorScale(72.0f)},
                         (Vector2){rect.x + DecorScale(42.0f), rect.y + DecorScale(50.0f)},
                         (Vector2){rect.x + DecorScale(58.0f), rect.y + DecorScale(76.0f)},
                         (Color){104, 112, 122, 150});
            DrawRectangle((int)(rect.x + DecorScale(56.0f)),
                          (int)(rect.y + DecorScale(65.0f)),
                          (int)DecorScale(22.0f),
                          (int)DecorScale(8.0f),
                          (Color){126, 92, 68, 145});
            break;
        case MAP_DECOR_GRASS:
            DrawLineEx((Vector2){rect.x + DecorScale(36.0f), rect.y + DecorScale(78.0f)},
                       (Vector2){rect.x + DecorScale(31.0f), rect.y + DecorScale(58.0f)},
                       DecorScale(3.0f),
                       (Color){88, 148, 94, 155});
            DrawLineEx((Vector2){rect.x + DecorScale(42.0f), rect.y + DecorScale(78.0f)},
                       (Vector2){rect.x + DecorScale(48.0f), rect.y + DecorScale(55.0f)},
                       DecorScale(3.0f),
                       (Color){76, 132, 82, 145});
            break;
        case MAP_DECOR_REEDS:
            DrawLineEx((Vector2){rect.x + DecorScale(32.0f), rect.y + DecorScale(82.0f)},
                       (Vector2){rect.x + DecorScale(29.0f), rect.y + DecorScale(38.0f)},
                       DecorScale(3.0f),
                       (Color){126, 154, 88, 165});
            DrawLineEx((Vector2){rect.x + DecorScale(49.0f), rect.y + DecorScale(82.0f)},
                       (Vector2){rect.x + DecorScale(53.0f), rect.y + DecorScale(31.0f)},
                       DecorScale(3.0f),
                       (Color){112, 140, 76, 155});
            break;
        case MAP_DECOR_RUINS_CRACK:
            DrawLineEx((Vector2){rect.x + DecorScale(22.0f), rect.y + DecorScale(40.0f)},
                       (Vector2){rect.x + DecorScale(45.0f), rect.y + DecorScale(53.0f)},
                       DecorScale(2.0f),
                       (Color){54, 62, 72, 145});
            DrawLineEx((Vector2){rect.x + DecorScale(45.0f), rect.y + DecorScale(53.0f)},
                       (Vector2){rect.x + DecorScale(69.0f), rect.y + DecorScale(45.0f)},
                       DecorScale(2.0f),
                       (Color){54, 62, 72, 145});
            break;
        case MAP_DECOR_WARNING_LIGHT:
            pulse = 0.5f + 0.5f * sinf(elapsedSeconds * 4.5f);
            DrawCircle((int)(rect.x + rect.width * 0.5f),
                       (int)(rect.y + rect.height * 0.5f),
                       DecorScale(7.0f + pulse * 3.0f),
                       (Color){255, 154, 72, (unsigned char)(95 + pulse * 95.0f)});
            break;
        case MAP_DECOR_NONE:
        default:
            break;
    }
}
