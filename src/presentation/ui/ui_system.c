#include "ui_system.h"

#include "localization.h"
#include "recipe_catalog.h"
#include "ui_runtime_internal.h"

#include <math.h>
#include <stdio.h>
#include <string.h>

static float ClampFloat(float value, float minValue, float maxValue) {
    if (value < minValue) {
        return minValue;
    }
    if (value > maxValue) {
        return maxValue;
    }
    return value;
}

static Font GetUIFont(const AssetBundle *assets) {
    return assets->uiFontLoaded ? assets->uiFont : GetFontDefault();
}

static int GetUtf8CharLength(const char *text) {
    unsigned char value;

    if (text == NULL || text[0] == '\0') {
        return 0;
    }

    value = (unsigned char)text[0];
    if ((value & 0x80u) == 0) {
        return 1;
    }
    if ((value & 0xE0u) == 0xC0u) {
        return text[1] != '\0' ? 2 : 1;
    }
    if ((value & 0xF0u) == 0xE0u) {
        if (text[1] != '\0' && text[2] != '\0') {
            return 3;
        }
        return text[1] != '\0' ? 2 : 1;
    }
    if ((value & 0xF8u) == 0xF0u) {
        if (text[1] != '\0' && text[2] != '\0' && text[3] != '\0') {
            return 4;
        }
        if (text[1] != '\0' && text[2] != '\0') {
            return 3;
        }
        return text[1] != '\0' ? 2 : 1;
    }

    return 1;
}

static bool IsAsciiBreakCharacter(const char *text, int charLength) {
    unsigned char value;

    if (text == NULL || charLength != 1) {
        return false;
    }

    value = (unsigned char)text[0];
    return value == ' ' || value == '-' || value == '/' || value == '\t';
}

static float GetUIScale(int screenWidth, int screenHeight) {
    float widthScale;
    float heightScale;

    widthScale = (float)screenWidth / 1440.0f;
    heightScale = (float)screenHeight / 900.0f;
    return ClampFloat(fminf(widthScale, heightScale), 0.84f, 1.25f);
}

static void DrawUIText(const AssetBundle *assets, const char *text, Vector2 position, float fontSize, Color tint) {
    Font font;
    float spacing;

    font = GetUIFont(assets);
    spacing = fontSize * 0.05f;
    DrawTextEx(font, text, position, fontSize, spacing, tint);
}

static Vector2 MeasureUIText(const AssetBundle *assets, const char *text, float fontSize) {
    Font font;
    float spacing;

    font = GetUIFont(assets);
    spacing = fontSize * 0.05f;
    return MeasureTextEx(font, text, fontSize, spacing);
}

static void DrawTextureAssetFitted(const TextureAsset *asset, Rectangle rect, Color tint) {
    Rectangle source;
    Rectangle dest;
    float scale;
    float drawWidth;
    float drawHeight;

    if (asset == NULL || !asset->loaded || asset->texture.width <= 0 || asset->texture.height <= 0) {
        return;
    }

    scale = fminf(rect.width / (float)asset->texture.width, rect.height / (float)asset->texture.height);
    drawWidth = (float)asset->texture.width * scale;
    drawHeight = (float)asset->texture.height * scale;
    source = (Rectangle){0.0f, 0.0f, (float)asset->texture.width, (float)asset->texture.height};
    dest = (Rectangle){
        rect.x + (rect.width - drawWidth) * 0.5f,
        rect.y + (rect.height - drawHeight) * 0.5f,
        drawWidth,
        drawHeight
    };
    DrawTexturePro(asset->texture, source, dest, (Vector2){0.0f, 0.0f}, 0.0f, tint);
}

static int GetWrappedLineLength(const AssetBundle *assets, const char *text, float fontSize, float maxWidth) {
    char buffer[512];
    int byteOffset;
    int lastBreak;

    if (text == NULL || text[0] == '\0') {
        return 0;
    }

    byteOffset = 0;
    lastBreak = -1;
    while (text[byteOffset] != '\0' && text[byteOffset] != '\n') {
        int charLength;
        int copyLength;

        charLength = GetUtf8CharLength(text + byteOffset);
        if (charLength <= 0) {
            break;
        }

        copyLength = byteOffset + charLength;
        if (copyLength >= (int)sizeof(buffer)) {
            break;
        }

        memcpy(buffer, text, (size_t)copyLength);
        buffer[copyLength] = '\0';

        if (MeasureUIText(assets, buffer, fontSize).x > maxWidth) {
            if (lastBreak > 0) {
                return lastBreak;
            }
            return byteOffset > 0 ? byteOffset : charLength;
        }

        if (IsAsciiBreakCharacter(text + byteOffset, charLength)) {
            lastBreak = copyLength;
        }
        byteOffset = copyLength;
    }

    return byteOffset;
}

static void DrawWrappedUIText(const AssetBundle *assets, const char *text, Rectangle rect, float fontSize, float lineSpacing, Color tint) {
    char lineBuffer[512];
    const char *cursor;
    float y;

    if (text == NULL || text[0] == '\0') {
        return;
    }

    cursor = text;
    y = rect.y;

    BeginScissorMode((int)rect.x, (int)rect.y, (int)rect.width, (int)rect.height);
    while (*cursor != '\0' && y + fontSize <= rect.y + rect.height) {
        int lineLength;
        int drawLength;

        while (*cursor == ' ') {
            cursor++;
        }

        if (*cursor == '\n') {
            cursor++;
            y += lineSpacing;
            continue;
        }

        lineLength = GetWrappedLineLength(assets, cursor, fontSize, rect.width);
        if (lineLength <= 0) {
            break;
        }

        drawLength = lineLength;
        while (drawLength > 0 && (cursor[drawLength - 1] == ' ' || cursor[drawLength - 1] == '\t' || cursor[drawLength - 1] == '\r')) {
            drawLength--;
        }

        if (drawLength >= (int)sizeof(lineBuffer)) {
            drawLength = (int)sizeof(lineBuffer) - 1;
        }

        memcpy(lineBuffer, cursor, (size_t)drawLength);
        lineBuffer[drawLength] = '\0';
        DrawUIText(assets, lineBuffer, (Vector2){rect.x, y}, fontSize, tint);

        cursor += lineLength;
        if (*cursor == '\n') {
            cursor++;
        }
        y += lineSpacing;
    }
    EndScissorMode();
}

static int CountWrappedUITextLines(const AssetBundle *assets, const char *text, float fontSize, float maxWidth, int maxLines) {
    const char *cursor;
    int lineCount;

    if (text == NULL || text[0] == '\0') {
        return 0;
    }

    cursor = text;
    lineCount = 0;
    while (*cursor != '\0' && lineCount < maxLines) {
        int lineLength;

        while (*cursor == ' ') {
            cursor++;
        }

        if (*cursor == '\n') {
            cursor++;
            lineCount++;
            continue;
        }

        lineLength = GetWrappedLineLength(assets, cursor, fontSize, maxWidth);
        if (lineLength <= 0) {
            break;
        }

        cursor += lineLength;
        if (*cursor == '\n') {
            cursor++;
        }
        lineCount++;
    }

    return lineCount;
}

static void DrawPanel(Rectangle rect, Color fill, Color outline) {
    DrawRectangleRounded(rect, 0.16f, 8, fill);
    DrawRectangleRoundedLinesEx(rect, 0.16f, 8, 1.2f, outline);
}

float UIRuntime_GetScale(int screenWidth, int screenHeight) {
    return GetUIScale(screenWidth, screenHeight);
}

void UIRuntime_DrawText(const AssetBundle *assets, const char *text, Vector2 position, float fontSize, Color tint) {
    DrawUIText(assets, text, position, fontSize, tint);
}

Vector2 UIRuntime_MeasureText(const AssetBundle *assets, const char *text, float fontSize) {
    return MeasureUIText(assets, text, fontSize);
}

void UIRuntime_DrawTextureAssetFitted(const TextureAsset *asset, Rectangle rect, Color tint) {
    DrawTextureAssetFitted(asset, rect, tint);
}

void UIRuntime_DrawWrappedText(const AssetBundle *assets, const char *text, Rectangle rect, float fontSize, float lineSpacing, Color tint) {
    DrawWrappedUIText(assets, text, rect, fontSize, lineSpacing, tint);
}

int UIRuntime_CountWrappedTextLines(const AssetBundle *assets, const char *text, float fontSize, float maxWidth, int maxLines) {
    return CountWrappedUITextLines(assets, text, fontSize, maxWidth, maxLines);
}

void UIRuntime_DrawPanel(Rectangle rect, Color fill, Color outline) {
    DrawPanel(rect, fill, outline);
}

static void DrawJaggedHorizon(int screenWidth, int screenHeight, float baseline, float amplitude, float step, float phase, Color color) {
    float x;

    for (x = -step; x < (float)screenWidth + step; x += step) {
        float nextX;
        float y0;
        float y1;

        nextX = x + step;
        y0 = baseline - amplitude * (0.34f + 0.66f * fabsf(sinf(x * 0.013f + phase)));
        y1 = baseline - amplitude * (0.34f + 0.66f * fabsf(sinf(nextX * 0.013f + phase)));
        DrawTriangle((Vector2){x, y0}, (Vector2){x, (float)screenHeight}, (Vector2){nextX, y1}, color);
        DrawTriangle((Vector2){nextX, y1}, (Vector2){x, (float)screenHeight}, (Vector2){nextX, (float)screenHeight}, color);
    }
}

static void DrawAlienSpire(int screenWidth, int screenHeight, float elapsedSeconds) {
    float cx;
    float baseY;
    float topY;
    float pulse;
    Vector2 tip;
    Vector2 leftBase;
    Vector2 rightBase;

    cx = screenWidth * 0.735f;
    baseY = screenHeight * 0.86f;
    topY = screenHeight * 0.17f;
    pulse = 0.5f + 0.5f * sinf(elapsedSeconds * 1.15f);
    tip = (Vector2){cx, topY};
    leftBase = (Vector2){cx - screenWidth * 0.105f, baseY};
    rightBase = (Vector2){cx + screenWidth * 0.095f, baseY};

    DrawCircleGradient((int)cx,
                       (int)(screenHeight * 0.47f),
                       screenHeight * 0.34f,
                       (Color){42, 113, 168, (unsigned char)(54 + pulse * 20.0f)},
                       (Color){7, 18, 32, 0});
    DrawTriangle((Vector2){cx - screenWidth * 0.018f, topY + screenHeight * 0.055f},
                 leftBase,
                 (Vector2){cx + screenWidth * 0.022f, baseY},
                 (Color){9, 24, 39, 255});
    DrawTriangle((Vector2){cx + screenWidth * 0.008f, topY + screenHeight * 0.09f},
                 (Vector2){cx - screenWidth * 0.012f, baseY},
                 rightBase,
                 (Color){14, 34, 51, 255});
    DrawTriangle((Vector2){cx - screenWidth * 0.016f, topY + screenHeight * 0.22f},
                 (Vector2){cx - screenWidth * 0.19f, baseY},
                 (Vector2){cx - screenWidth * 0.035f, baseY},
                 (Color){7, 20, 33, 255});
    DrawTriangle((Vector2){cx + screenWidth * 0.02f, topY + screenHeight * 0.27f},
                 (Vector2){cx + screenWidth * 0.02f, baseY},
                 (Vector2){cx + screenWidth * 0.18f, baseY},
                 (Color){10, 27, 41, 255});

    DrawLineEx(tip,
               (Vector2){cx - screenWidth * 0.058f, baseY},
               3.0f,
               (Color){58, 134, 200, 115});
    DrawLineEx((Vector2){cx + screenWidth * 0.006f, topY + screenHeight * 0.075f},
               (Vector2){cx + screenWidth * 0.058f, baseY},
               2.0f,
               (Color){107, 203, 228, 92});

    DrawCircleGradient((int)cx,
                       (int)(screenHeight * 0.425f),
                       screenHeight * 0.068f,
                       (Color){107, 203, 228, (unsigned char)(86 + pulse * 44.0f)},
                       (Color){32, 104, 164, 0});
    DrawCircle((int)cx, (int)(screenHeight * 0.425f), screenHeight * 0.013f, (Color){221, 244, 255, 235});
    DrawCircleLines((int)cx, (int)(screenHeight * 0.425f), screenHeight * 0.026f, (Color){58, 134, 200, 180});
    DrawCircleLines((int)cx, (int)(screenHeight * 0.425f), screenHeight * 0.044f, (Color){107, 203, 228, 90});

    DrawLineEx((Vector2){cx - screenWidth * 0.018f, screenHeight * 0.51f},
               (Vector2){cx - screenWidth * 0.044f, screenHeight * 0.69f},
               5.0f,
               (Color){58, 134, 200, 64});
    DrawLineEx((Vector2){cx + screenWidth * 0.017f, screenHeight * 0.54f},
               (Vector2){cx + screenWidth * 0.047f, screenHeight * 0.73f},
               4.0f,
               (Color){107, 203, 228, 56});
}

static void DrawBackdrop(int screenWidth, int screenHeight, float elapsedSeconds) {
    int index;
    float horizon;
    float moonX;
    float moonY;
    float moonRadius;

    DrawRectangleGradientV(0, 0, screenWidth, screenHeight, (Color){5, 13, 25, 255}, (Color){9, 31, 48, 255});
    DrawRectangleGradientH(0, 0, screenWidth, screenHeight, (Color){10, 51, 68, 150}, (Color){24, 49, 88, 90});

    DrawCircleGradient((int)(screenWidth * 0.28f),
                       (int)(screenHeight * 0.30f),
                       screenHeight * 0.42f,
                       (Color){32, 123, 153, 68},
                       (Color){7, 23, 39, 0});
    DrawCircleGradient((int)(screenWidth * 0.74f),
                       (int)(screenHeight * 0.46f),
                       screenHeight * 0.48f,
                       (Color){47, 91, 157, 68},
                       (Color){9, 19, 40, 0});

    moonX = screenWidth * 0.82f;
    moonY = screenHeight * 0.19f;
    moonRadius = screenHeight * 0.095f;
    DrawCircleGradient((int)moonX, (int)moonY, moonRadius * 2.2f, (Color){135, 194, 229, 58}, (Color){39, 79, 130, 0});
    DrawCircle((int)moonX, (int)moonY, moonRadius, (Color){128, 166, 207, 220});
    DrawCircle((int)(moonX - moonRadius * 0.24f), (int)(moonY - moonRadius * 0.18f), moonRadius * 0.88f, (Color){185, 220, 238, 132});
    DrawCircle((int)(moonX + moonRadius * 0.18f), (int)(moonY + moonRadius * 0.08f), moonRadius * 0.14f, (Color){61, 94, 132, 76});
    DrawCircle((int)(moonX - moonRadius * 0.34f), (int)(moonY + moonRadius * 0.26f), moonRadius * 0.10f, (Color){61, 94, 132, 70});

    for (index = 0; index < 96; index++) {
        float seed;
        float x;
        float y;
        float radius;
        unsigned char alpha;

        seed = (float)index * 19.37f;
        x = fmodf(seed * 31.0f, (float)screenWidth);
        y = fmodf(seed * 13.0f + elapsedSeconds * (1.0f + (float)(index % 4)) * 0.45f, screenHeight * 0.70f);
        radius = 0.7f + (float)(index % 3) * 0.55f;
        alpha = (unsigned char)(55 + (index % 5) * 24);
        DrawCircle((int)x, (int)y, radius, index % 7 == 0 ? (Color){210, 170, 109, alpha} : (Color){151, 219, 238, alpha});
    }

    horizon = screenHeight * 0.70f;
    DrawJaggedHorizon(screenWidth, screenHeight, horizon, screenHeight * 0.16f, screenWidth * 0.095f, 0.7f, (Color){24, 38, 56, 255});
    DrawJaggedHorizon(screenWidth, screenHeight, screenHeight * 0.80f, screenHeight * 0.20f, screenWidth * 0.078f, 1.9f, (Color){10, 24, 37, 255});
    DrawAlienSpire(screenWidth, screenHeight, elapsedSeconds);

    DrawRectangleGradientV(0,
                           (int)(screenHeight * 0.68f),
                           screenWidth,
                           (int)(screenHeight * 0.32f),
                           (Color){12, 29, 43, 0},
                           (Color){3, 9, 16, 245});

    for (index = 0; index < 36; ++index) {
        float seed;
        float x;
        float y;
        float radius;
        float drift;

        seed = (float)index * 41.0f;
        drift = fmodf(elapsedSeconds * (3.0f + (float)(index % 4)), 120.0f);
        x = fmodf(seed * 17.0f + drift, (float)screenWidth);
        y = screenHeight * 0.42f + fmodf(seed * 7.0f, screenHeight * 0.50f);
        radius = 1.2f + (float)(index % 4) * 0.55f;
        DrawCircle((int)x, (int)y, radius, index % 3 == 0 ? (Color){210, 170, 109, 60} : (Color){107, 203, 228, 74});
    }

    DrawRectangleGradientH(0, 0, (int)(screenWidth * 0.16f), screenHeight, (Color){3, 5, 10, 190}, (Color){3, 5, 10, 0});
    DrawRectangleGradientH((int)(screenWidth * 0.84f), 0, (int)(screenWidth * 0.16f), screenHeight, (Color){3, 5, 10, 0}, (Color){3, 5, 10, 190});
    DrawRectangleGradientV(0, 0, screenWidth, (int)(screenHeight * 0.12f), (Color){3, 5, 10, 150}, (Color){3, 5, 10, 0});
}

void UIRuntime_DrawBackdrop(int screenWidth, int screenHeight, float elapsedSeconds) {
    DrawBackdrop(screenWidth, screenHeight, elapsedSeconds);
}

static void DrawButton(const AssetBundle *assets, Rectangle rect, const char *label, bool enabled) {
    bool hovered;
    Color fill;
    Color outline;
    Color textColor;
    Vector2 labelSize;

    hovered = enabled && CheckCollisionPointRec(GetMousePosition(), rect);
    fill = enabled
        ? (hovered ? (Color){36, 76, 114, 240} : (Color){18, 40, 66, 235})
        : (Color){28, 32, 44, 220};
    outline = enabled
        ? (hovered ? (Color){165, 230, 255, 150} : (Color){105, 168, 222, 80})
        : (Color){84, 96, 118, 50};
    textColor = enabled ? WHITE : (Color){138, 146, 165, 255};
    labelSize = MeasureUIText(assets, label, rect.height * 0.40f);
    DrawPanel(rect, fill, outline);
    DrawUIText(
        assets,
        label,
        (Vector2){rect.x + rect.width * 0.5f - labelSize.x * 0.5f, rect.y + rect.height * 0.5f - labelSize.y * 0.5f},
        rect.height * 0.40f,
        textColor
    );
}

void UIRuntime_DrawButton(const AssetBundle *assets, Rectangle rect, const char *label, bool enabled) {
    DrawButton(assets, rect, label, enabled);
}
