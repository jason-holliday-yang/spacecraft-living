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

static void DrawBackdrop(int screenWidth, int screenHeight, float elapsedSeconds) {
    int index;

    DrawRectangleGradientV(0, 0, screenWidth, screenHeight, (Color){5, 10, 18, 255}, (Color){11, 23, 37, 255});
    DrawRectangleGradientH(0, screenHeight / 2, screenWidth, screenHeight / 2, (Color){8, 40, 53, 0}, (Color){38, 74, 102, 50});

    for (index = 0; index < 80; index++) {
        float seed;
        float x;
        float y;
        float radius;

        seed = (float)index * 21.0f;
        x = fmodf(seed * 29.0f, (float)screenWidth);
        y = fmodf(seed * 12.0f + elapsedSeconds * (4.0f + (float)(index % 5)), (float)screenHeight);
        radius = 1.0f + (float)(index % 3);
        DrawCircle((int)x, (int)y, radius, (Color){154, 202, 255, (unsigned char)(70 + (index % 4) * 25)});
    }
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
