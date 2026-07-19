#include "ui_theme_internal.h"

#include <cmath>
#include <cstddef>

static bool TextContainsNonAscii(const char *text) {
    const unsigned char *cursor;

    if (text == NULL) {
        return false;
    }

    cursor = (const unsigned char *)text;
    while (*cursor != '\0') {
        if ((*cursor & 0x80u) != 0) {
            return true;
        }
        cursor++;
    }

    return false;
}

void UITheme_DrawTitleText(const AssetBundle *assets, const char *text, Vector2 center, float fontSize, float scale, Color fill) {
    Font font;
    float spacing;
    float outlineOffset;
    float shadowOffset;
    float highlightOffset;
    Vector2 textSize;
    Vector2 position;
    bool useDefaultFont;
    bool useCjkTitleStyle;

    useDefaultFont = assets == NULL || !assets->uiFontLoaded;
    font = useDefaultFont ? GetFontDefault() : assets->uiFont;
    if (font.texture.id == 0) {
        return;
    }

    useCjkTitleStyle = TextContainsNonAscii(text);

    if (useDefaultFont || useCjkTitleStyle) {
        SetTextureFilter(font.texture, TEXTURE_FILTER_POINT);
    }

    spacing = useCjkTitleStyle ? 0.0f : (useDefaultFont ? std::floor(fontSize * 0.12f) : fontSize * 0.05f);
    outlineOffset = std::round((useCjkTitleStyle ? 2.0f : 3.0f) * scale);
    shadowOffset = std::round((useCjkTitleStyle ? 4.0f : 7.0f) * scale);
    highlightOffset = std::round((useCjkTitleStyle ? 1.0f : 2.0f) * scale);
    textSize = MeasureTextEx(font, text, fontSize, spacing);
    position = Vector2{
        std::round(center.x - textSize.x * 0.5f),
        std::round(center.y - textSize.y * 0.5f)
    };

    DrawTextEx(font,
               text,
               Vector2{position.x + shadowOffset, position.y + shadowOffset},
               fontSize,
               spacing,
               Color{3, 7, 12, (unsigned char)(useCjkTitleStyle ? 150 : 180)});
    if (useCjkTitleStyle) {
        DrawTextEx(font, text, Vector2{position.x, position.y + outlineOffset}, fontSize, spacing, Color{8, 18, 30, 220});
        DrawTextEx(font, text, Vector2{position.x + outlineOffset, position.y}, fontSize, spacing, Color{8, 18, 30, 200});
    } else {
        DrawTextEx(font, text, Vector2{position.x - outlineOffset, position.y}, fontSize, spacing, Color{8, 18, 30, 255});
        DrawTextEx(font, text, Vector2{position.x + outlineOffset, position.y}, fontSize, spacing, Color{8, 18, 30, 255});
        DrawTextEx(font, text, Vector2{position.x, position.y - outlineOffset}, fontSize, spacing, Color{8, 18, 30, 255});
        DrawTextEx(font, text, Vector2{position.x, position.y + outlineOffset}, fontSize, spacing, Color{8, 18, 30, 255});
    }
    DrawTextEx(font, text, position, fontSize, spacing, fill);
    DrawTextEx(font,
               text,
               Vector2{position.x, position.y - highlightOffset},
               fontSize,
               spacing,
               Color{255, 255, 255, (unsigned char)(useCjkTitleStyle ? 76 : 96)});

    if (!useDefaultFont && useCjkTitleStyle) {
        SetTextureFilter(font.texture, TEXTURE_FILTER_BILINEAR);
    }
}


const UITheme *UITheme_Get(void) {
    static const UITheme theme = {
        Color{6, 13, 24, 255},       // abyss
        Color{7, 18, 32, 255},       // backdropTop
        Color{12, 32, 49, 255},      // backdropBottom
        Color{9, 24, 39, 246},       // surface
        Color{16, 40, 61, 248},      // surfaceRaised
        Color{23, 58, 87, 250},      // surfaceHover
        Color{19, 27, 38, 232},      // surfaceDisabled
        Color{58, 134, 200, 205},    // border
        Color{78, 129, 165, 92},     // borderSoft
        Color{58, 134, 200, 255},    // signalBlue
        Color{107, 203, 228, 255},   // signalCyan
        Color{221, 244, 255, 255},   // iceWhite
        Color{229, 239, 246, 255},   // textPrimary
        Color{177, 202, 218, 255},   // textSecondary
        Color{111, 132, 149, 255},   // textMuted
        Color{105, 205, 182, 255},   // positive
        Color{210, 170, 109, 255},   // archiveGold
        Color{211, 140, 79, 255},    // warning
        Color{182, 76, 89, 255},     // danger
        Color{96, 112, 128, 255}     // disabled
    };

    return &theme;
}
