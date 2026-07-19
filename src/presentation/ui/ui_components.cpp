#include "ui_components_internal.h"

#include "ui_runtime_internal.h"

#include <algorithm>
#include <cmath>
#include <cstdio>

namespace {

Color WithAlpha(Color color, unsigned char alpha) {
    color.a = alpha;
    return color;
}

Color ActionAccent(UIActionStyle style, const UITheme *theme) {
    switch (style) {
        case UI_ACTION_WARNING:
            return theme->warning;
        case UI_ACTION_DANGER:
            return theme->danger;
        case UI_ACTION_GHOST:
            return theme->textSecondary;
        case UI_ACTION_SECONDARY:
            return theme->signalCyan;
        case UI_ACTION_PRIMARY:
        default:
            return theme->signalBlue;
    }
}

Color ActionTop(UIActionStyle style, bool active) {
    switch (style) {
        case UI_ACTION_WARNING:
            return active ? Color{94, 67, 42, 255} : Color{67, 49, 36, 252};
        case UI_ACTION_DANGER:
            return active ? Color{104, 39, 50, 255} : Color{74, 31, 42, 252};
        case UI_ACTION_GHOST:
            return active ? Color{34, 54, 70, 250} : Color{20, 34, 48, 242};
        case UI_ACTION_SECONDARY:
            return active ? Color{28, 69, 87, 255} : Color{20, 49, 67, 252};
        case UI_ACTION_PRIMARY:
        default:
            return active ? Color{31, 80, 121, 255} : Color{23, 61, 94, 252};
    }
}

}  // namespace

void UIComponents_DrawDiamond(Vector2 center, float radius, Color fill, Color outline) {
    const Vector2 top{center.x, center.y - radius};
    const Vector2 right{center.x + radius, center.y};
    const Vector2 bottom{center.x, center.y + radius};
    const Vector2 left{center.x - radius, center.y};

    DrawTriangle(top, left, bottom, fill);
    DrawTriangle(top, bottom, right, fill);
    DrawLineEx(top, right, 1.2f, outline);
    DrawLineEx(right, bottom, 1.2f, outline);
    DrawLineEx(bottom, left, 1.2f, outline);
    DrawLineEx(left, top, 1.2f, outline);
}

void UIComponents_DrawScrim(int screenWidth, int screenHeight, unsigned char alpha) {
    const UITheme *theme = UITheme_Get();

    DrawRectangle(0, 0, screenWidth, screenHeight, WithAlpha(theme->abyss, alpha));
    DrawRectangleGradientV(0,
                           0,
                           screenWidth,
                           screenHeight,
                           Color{5, 18, 31, (unsigned char)(alpha / 2)},
                           Color{2, 7, 14, (unsigned char)std::min(255, (int)alpha + 24)});
}

void UIComponents_DrawFrame(Rectangle rect, float scale, UIFrameStyle style, bool strongShadow) {
    const UITheme *theme = UITheme_Get();
    const Color accent = style == UI_FRAME_DANGER ? theme->danger : theme->signalBlue;
    const Color secondaryAccent = style == UI_FRAME_DANGER ? theme->warning : theme->signalCyan;
    const Color outerFill = style == UI_FRAME_DANGER ? Color{39, 19, 27, 250} : Color{8, 25, 41, 250};
    const Color innerTop = style == UI_FRAME_DANGER ? Color{45, 23, 31, 250} : theme->surfaceRaised;
    const float inset = style == UI_FRAME_COMPACT ? 6.0f * scale : 9.0f * scale;
    const Rectangle outer{rect.x - 7.0f * scale, rect.y - 7.0f * scale, rect.width + 14.0f * scale, rect.height + 14.0f * scale};
    const Rectangle inner{rect.x + inset, rect.y + inset, rect.width - inset * 2.0f, rect.height - inset * 2.0f};

    if (strongShadow) {
        DrawRectangleRounded(Rectangle{rect.x + 17.0f * scale, rect.y + 20.0f * scale, rect.width, rect.height},
                             0.055f,
                             8,
                             Color{1, 4, 9, 176});
        DrawCircleGradient((int)(rect.x + rect.width * 0.5f),
                           (int)(rect.y + rect.height * 0.52f),
                           rect.width * 0.64f,
                           Color{0, 0, 0, 32},
                           Color{0, 0, 0, 0});
    }

    DrawRectangleRounded(outer, 0.055f, 8, outerFill);
    DrawRectangleRoundedLinesEx(outer, 0.055f, 8, 2.0f * scale, Color{2, 8, 15, 240});
    DrawRectangleRounded(rect, 0.045f, 8, theme->surface);
    DrawRectangleGradientV((int)inner.x,
                           (int)inner.y,
                           (int)inner.width,
                           (int)inner.height,
                           innerTop,
                           Color{6, 17, 29, 250});
    DrawRectangleRoundedLinesEx(rect, 0.045f, 8, 1.8f * scale, WithAlpha(accent, 190));
    DrawRectangleRoundedLinesEx(inner, 0.035f, 8, 1.0f * scale, WithAlpha(secondaryAccent, 58));
    DrawRectangleGradientV((int)(rect.x + 5.0f * scale),
                           (int)(rect.y + 5.0f * scale),
                           (int)(rect.width - 10.0f * scale),
                           (int)(10.0f * scale),
                           WithAlpha(secondaryAccent, 56),
                           WithAlpha(accent, 0));

    UIComponents_DrawDiamond(Vector2{rect.x + 3.0f * scale, rect.y + 26.0f * scale},
                             8.0f * scale,
                             style == UI_FRAME_DANGER ? Color{84, 28, 38, 255} : Color{13, 49, 72, 255},
                             WithAlpha(accent, 205));
    UIComponents_DrawDiamond(Vector2{rect.x + rect.width - 3.0f * scale, rect.y + 26.0f * scale},
                             8.0f * scale,
                             style == UI_FRAME_DANGER ? Color{84, 28, 38, 255} : Color{13, 49, 72, 255},
                             WithAlpha(accent, 205));
    UIComponents_DrawDiamond(Vector2{rect.x + 3.0f * scale, rect.y + rect.height - 26.0f * scale},
                             7.0f * scale,
                             Color{18, 52, 67, 255},
                             WithAlpha(secondaryAccent, 135));
    UIComponents_DrawDiamond(Vector2{rect.x + rect.width - 3.0f * scale, rect.y + rect.height - 26.0f * scale},
                             7.0f * scale,
                             Color{18, 52, 67, 255},
                             WithAlpha(secondaryAccent, 135));
}

void UIComponents_DrawActionButton(const AssetBundle *assets,
                                   Rectangle rect,
                                   const char *label,
                                   bool enabled,
                                   bool keyboardSelected,
                                   UIActionStyle style,
                                   float scale) {
    const UITheme *theme = UITheme_Get();
    const bool hovered = enabled && CheckCollisionPointRec(GetMousePosition(), rect);
    const bool active = hovered || keyboardSelected;
    const Color accent = enabled ? ActionAccent(style, theme) : theme->disabled;
    const Color top = enabled ? ActionTop(style, active) : theme->surfaceDisabled;
    const Color bottom = enabled ? Color{7, 20, 34, 252} : Color{16, 22, 31, 236};
    const Color text = enabled ? theme->iceWhite : WithAlpha(theme->disabled, 230);
    const float markerRadius = rect.height * 0.13f;
    const Vector2 labelSize = UIRuntime_MeasureText(assets, label, rect.height * 0.35f);

    DrawRectangleRounded(Rectangle{rect.x + 7.0f * scale, rect.y + 8.0f * scale, rect.width, rect.height},
                         0.16f,
                         8,
                         Color{1, 4, 9, 165});
    DrawTriangle(Vector2{rect.x - 9.0f * scale, rect.y + rect.height * 0.5f},
                 Vector2{rect.x + 11.0f * scale, rect.y + rect.height - 8.0f * scale},
                 Vector2{rect.x + 11.0f * scale, rect.y + 8.0f * scale},
                 WithAlpha(accent, enabled ? 215 : 110));
    DrawTriangle(Vector2{rect.x + rect.width + 9.0f * scale, rect.y + rect.height * 0.5f},
                 Vector2{rect.x + rect.width - 11.0f * scale, rect.y + 8.0f * scale},
                 Vector2{rect.x + rect.width - 11.0f * scale, rect.y + rect.height - 8.0f * scale},
                 WithAlpha(style == UI_ACTION_DANGER ? theme->danger : theme->signalCyan, enabled ? 170 : 80));

    DrawRectangleRounded(rect, 0.16f, 8, bottom);
    DrawRectangleGradientV((int)(rect.x + 2.0f * scale),
                           (int)(rect.y + 2.0f * scale),
                           (int)(rect.width - 4.0f * scale),
                           (int)(rect.height - 4.0f * scale),
                           top,
                           bottom);
    DrawRectangleRoundedLinesEx(rect, 0.16f, 8, active ? 2.3f * scale : 1.35f * scale, WithAlpha(accent, active ? 245 : 180));
    DrawRectangleRoundedLinesEx(Rectangle{rect.x + 6.0f * scale, rect.y + 6.0f * scale, rect.width - 12.0f * scale, rect.height - 12.0f * scale},
                                0.14f,
                                8,
                                1.0f,
                                WithAlpha(theme->signalCyan, enabled ? 40 : 18));
    DrawRectangleGradientH((int)(rect.x + 16.0f * scale),
                           (int)(rect.y + 7.0f * scale),
                           (int)(rect.width - 32.0f * scale),
                           (int)(2.0f * scale),
                           Color{221, 244, 255, 0},
                           WithAlpha(theme->iceWhite, active ? 140 : 54));

    UIComponents_DrawDiamond(Vector2{rect.x + 29.0f * scale, rect.y + rect.height * 0.5f},
                             markerRadius,
                             enabled ? WithAlpha(accent, 130) : Color{39, 47, 57, 220},
                             WithAlpha(accent, active ? 245 : 185));
    DrawCircle((int)(rect.x + 29.0f * scale),
               (int)(rect.y + rect.height * 0.5f),
               markerRadius * 0.27f,
               enabled ? theme->iceWhite : theme->disabled);

    UIRuntime_DrawText(assets,
                       label,
                       Vector2{rect.x + rect.width * 0.5f - labelSize.x * 0.5f + 7.0f * scale,
                               rect.y + rect.height * 0.5f - labelSize.y * 0.5f - 1.0f * scale},
                       rect.height * 0.35f,
                       text);
}

void UIComponents_DrawCompactButton(const AssetBundle *assets,
                                    Rectangle rect,
                                    const char *label,
                                    bool enabled,
                                    bool keyboardSelected,
                                    UIActionStyle style,
                                    float scale) {
    const UITheme *theme = UITheme_Get();
    const bool hovered = enabled && CheckCollisionPointRec(GetMousePosition(), rect);
    const bool active = hovered || keyboardSelected;
    const Color accent = enabled ? ActionAccent(style, theme) : theme->disabled;
    const Color top = enabled ? ActionTop(style, active) : theme->surfaceDisabled;
    const Vector2 labelSize = UIRuntime_MeasureText(assets, label, rect.height * 0.36f);

    DrawRectangleRounded(Rectangle{rect.x + 4.0f * scale, rect.y + 5.0f * scale, rect.width, rect.height},
                         0.16f,
                         8,
                         Color{1, 4, 9, 130});
    DrawRectangleRounded(rect, 0.16f, 8, theme->surface);
    DrawRectangleGradientV((int)(rect.x + 2.0f * scale),
                           (int)(rect.y + 2.0f * scale),
                           (int)(rect.width - 4.0f * scale),
                           (int)(rect.height - 4.0f * scale),
                           top,
                           enabled ? Color{7, 20, 34, 252} : theme->surfaceDisabled);
    DrawRectangleRoundedLinesEx(rect,
                                0.16f,
                                8,
                                active ? 2.0f * scale : 1.15f * scale,
                                WithAlpha(accent, active ? 240 : 160));
    DrawLineEx(Vector2{rect.x + 10.0f * scale, rect.y + 6.0f * scale},
               Vector2{rect.x + rect.width - 10.0f * scale, rect.y + 6.0f * scale},
               1.0f,
               WithAlpha(theme->iceWhite, active ? 100 : 38));
    UIRuntime_DrawText(assets,
                       label,
                       Vector2{rect.x + (rect.width - labelSize.x) * 0.5f,
                               rect.y + (rect.height - labelSize.y) * 0.5f - 1.0f * scale},
                       rect.height * 0.36f,
                       enabled ? theme->iceWhite : theme->disabled);
}

void UIComponents_DrawCard(Rectangle rect, bool selected, bool enabled, float scale, Color accent) {
    const UITheme *theme = UITheme_Get();
    const bool hovered = enabled && CheckCollisionPointRec(GetMousePosition(), rect);
    const Color fillTop = enabled
        ? (selected || hovered ? theme->surfaceHover : theme->surfaceRaised)
        : theme->surfaceDisabled;
    const Color outline = enabled
        ? (selected ? accent : (hovered ? WithAlpha(theme->signalCyan, 170) : theme->borderSoft))
        : WithAlpha(theme->disabled, 75);

    DrawRectangleRounded(Rectangle{rect.x + 4.0f * scale, rect.y + 5.0f * scale, rect.width, rect.height},
                         0.08f,
                         8,
                         Color{1, 4, 8, 115});
    DrawRectangleRounded(rect, 0.08f, 8, fillTop);
    DrawRectangleGradientV((int)(rect.x + 3.0f * scale),
                           (int)(rect.y + 3.0f * scale),
                           (int)(rect.width - 6.0f * scale),
                           (int)(rect.height - 6.0f * scale),
                           fillTop,
                           enabled ? theme->surface : theme->surfaceDisabled);
    DrawRectangleRoundedLinesEx(rect, 0.08f, 8, selected ? 2.0f * scale : 1.1f * scale, outline);
    if (selected) {
        DrawRectangle((int)(rect.x + 4.0f * scale),
                      (int)(rect.y + 8.0f * scale),
                      (int)(3.0f * scale),
                      (int)(rect.height - 16.0f * scale),
                      accent);
    }
}

void UIComponents_DrawBadge(const AssetBundle *assets,
                            Rectangle rect,
                            const char *label,
                            Color accent,
                            bool enabled,
                            float scale) {
    const UITheme *theme = UITheme_Get();
    const Color text = enabled ? theme->iceWhite : theme->disabled;
    const Vector2 size = UIRuntime_MeasureText(assets, label, rect.height * 0.52f);

    DrawRectangleRounded(rect, 0.32f, 8, enabled ? Color{14, 43, 63, 238} : theme->surfaceDisabled);
    DrawRectangleRoundedLinesEx(rect, 0.32f, 8, 1.0f * scale, WithAlpha(accent, enabled ? 155 : 65));
    UIRuntime_DrawText(assets,
                       label,
                       Vector2{rect.x + (rect.width - size.x) * 0.5f, rect.y + (rect.height - size.y) * 0.5f - 1.0f * scale},
                       rect.height * 0.52f,
                       text);
}

void UIComponents_DrawDivider(Rectangle rect, Color accent, float scale) {
    DrawRectangleGradientH((int)rect.x,
                           (int)rect.y,
                           (int)rect.width,
                           (int)std::max(rect.height, 1.0f * scale),
                           Color{accent.r, accent.g, accent.b, 0},
                           Color{accent.r, accent.g, accent.b, 130});
    DrawCircle((int)(rect.x + rect.width), (int)(rect.y + rect.height * 0.5f), 2.2f * scale, WithAlpha(accent, 190));
}

void UIComponents_DrawPanelHeader(const AssetBundle *assets,
                                  Rectangle panel,
                                  const char *eyebrow,
                                  const char *title,
                                  const char *hint,
                                  float scale,
                                  Color accent) {
    const UITheme *theme = UITheme_Get();

    if (eyebrow != nullptr && eyebrow[0] != '\0') {
        UIRuntime_DrawText(assets,
                           eyebrow,
                           Vector2{panel.x + 34.0f * scale, panel.y + 22.0f * scale},
                           12.5f * scale,
                           accent);
    }
    UIRuntime_DrawText(assets,
                       title,
                       Vector2{panel.x + 34.0f * scale, panel.y + 42.0f * scale},
                       32.0f * scale,
                       theme->iceWhite);
    UIComponents_DrawDivider(Rectangle{panel.x + 34.0f * scale,
                                       panel.y + 84.0f * scale,
                                       310.0f * scale,
                                       1.0f * scale},
                             accent,
                             scale);
    if (hint != nullptr && hint[0] != '\0') {
        const Vector2 hintSize = UIRuntime_MeasureText(assets, hint, 14.5f * scale);
        UIRuntime_DrawText(assets,
                           hint,
                           Vector2{panel.x + panel.width - hintSize.x - 32.0f * scale, panel.y + 35.0f * scale},
                           14.5f * scale,
                           theme->textSecondary);
    }
}

void UIComponents_DrawFrontEndButton(const AssetBundle *assets,
                               Rectangle rect,
                               const char *label,
                               bool enabled,
                               bool keyboardSelected,
                               bool primary,
                               float scale) {
    UIComponents_DrawActionButton(assets,
                                  rect,
                                  label,
                                  enabled,
                                  keyboardSelected,
                                  primary ? UI_ACTION_PRIMARY : UI_ACTION_SECONDARY,
                                  scale);
}

static void DrawVolumePreviewBars(Rectangle rect, float volume) {
    const UITheme *theme = UITheme_Get();

    for (int barIndex = 0; barIndex < 12; ++barIndex) {
        const float ratio = (float)(barIndex + 1) / 12.0f;
        const float barWidth = rect.width / 16.0f;
        const float barHeight = rect.height * (0.22f + ratio * 0.78f);
        const float x = rect.x + barIndex * (barWidth + rect.width / 44.0f);
        const float y = rect.y + rect.height - barHeight;
        const bool active = volume >= ratio - (1.0f / 24.0f);

        DrawRectangleRounded(Rectangle{x, y, barWidth, barHeight},
                             0.35f,
                             6,
                             active ? theme->signalCyan : Color{32, 55, 75, 255});
    }
}

static float GetSettingsVolumeValue(const GameSettings *settings, int sliderIndex) {
    if (settings == NULL) {
        return 0.0f;
    }

    switch (sliderIndex) {
        case 0:
            return settings->masterVolume;
        case 1:
            return settings->musicVolume;
        case 2:
            return settings->sfxVolume;
        default:
            return 0.0f;
    }
}

void UIComponents_DrawSettingsSliderRow(const AssetBundle *assets,
                                  const GameSettings *settings,
                                  int sliderIndex,
                                  const char *title,
                                  const char *description,
                                  int screenWidth,
                                  int screenHeight) {
    const UITheme *theme = UITheme_Get();
    const float scale = UIRuntime_GetScale(screenWidth, screenHeight);
    const Rectangle rowRect = UI_GetSettingsRowRect(screenWidth, screenHeight, sliderIndex);
    const Rectangle sliderRect = UI_GetSettingsSliderRect(screenWidth, screenHeight, sliderIndex);
    const Rectangle decreaseRect = UI_GetSettingsDecreaseButtonRect(screenWidth, screenHeight, sliderIndex);
    const Rectangle increaseRect = UI_GetSettingsIncreaseButtonRect(screenWidth, screenHeight, sliderIndex);
    const float value = GetSettingsVolumeValue(settings, sliderIndex);
    const float filledWidth = sliderRect.width * value;
    const float handleCenter = sliderRect.x + std::max(10.0f * scale, std::min(sliderRect.width - 10.0f * scale, filledWidth));
    const Rectangle handleRect{
        handleCenter - 10.0f * scale,
        sliderRect.y - 7.0f * scale,
        20.0f * scale,
        sliderRect.height + 14.0f * scale
    };
    const Rectangle previewRect{
        rowRect.x + 190.0f * scale,
        rowRect.y + 18.0f * scale,
        42.0f * scale,
        36.0f * scale
    };
    char valueBuffer[16];

    UIComponents_DrawCard(rowRect, false, true, scale, theme->signalBlue);
    UIRuntime_DrawText(assets,
                       title,
                       Vector2{rowRect.x + 22.0f * scale, rowRect.y + 13.0f * scale},
                       21.0f * scale,
                       theme->iceWhite);
    UIRuntime_DrawWrappedText(assets,
                              description,
                              Rectangle{rowRect.x + 22.0f * scale, rowRect.y + 40.0f * scale, 220.0f * scale, 20.0f * scale},
                              12.0f * scale,
                              13.0f * scale,
                              theme->textSecondary);
    DrawVolumePreviewBars(previewRect, value);

    DrawRectangleRounded(sliderRect, 0.42f, 8, Color{5, 19, 32, 245});
    DrawRectangleRoundedLinesEx(sliderRect, 0.42f, 8, 1.0f, Color{58, 134, 200, 100});
    if (filledWidth > 6.0f * scale) {
        DrawRectangleRounded(Rectangle{sliderRect.x + 3.0f * scale,
                                       sliderRect.y + 3.0f * scale,
                                       filledWidth - 6.0f * scale,
                                       sliderRect.height - 6.0f * scale},
                             0.38f,
                             8,
                             theme->signalCyan);
    }
    DrawCircleGradient((int)(handleRect.x + handleRect.width * 0.5f),
                       (int)(handleRect.y + handleRect.height * 0.5f),
                       22.0f * scale,
                       Color{107, 203, 228, 72},
                       Color{58, 134, 200, 0});
    DrawRectangleRounded(handleRect, 0.38f, 8, theme->iceWhite);
    DrawRectangleRoundedLinesEx(handleRect, 0.38f, 8, 1.0f, theme->signalBlue);

    UIComponents_DrawCompactButton(assets, decreaseRect, "-", true, false, UI_ACTION_GHOST, scale);
    UIComponents_DrawCompactButton(assets, increaseRect, "+", true, false, UI_ACTION_GHOST, scale);

    std::snprintf(valueBuffer, sizeof(valueBuffer), "%d%%", (int)std::round(value * 100.0f));
    UIRuntime_DrawText(assets,
                       valueBuffer,
                       Vector2{rowRect.x + rowRect.width - 105.0f * scale, rowRect.y + 25.0f * scale},
                       18.0f * scale,
                       theme->signalCyan);
}
