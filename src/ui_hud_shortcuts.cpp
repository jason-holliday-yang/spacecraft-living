#include "ui_runtime_internal.h"

static void DrawHudShortcutFallbackIcon(Rectangle rect, int iconKind, Color primary, Color secondary) {
    Vector2 center;
    float unit;
    Color glow;

    center = Vector2{rect.x + rect.width * 0.5f, rect.y + rect.height * 0.5f};
    unit = (rect.width < rect.height ? rect.width : rect.height) * 0.18f;
    glow = Color{primary.r, primary.g, primary.b, 38};

    DrawCircleV(center, unit * 1.95f, glow);

    switch (iconKind) {
        case 0:
            DrawTriangle(Vector2{center.x - unit * 1.38f, center.y + unit * 0.86f},
                         Vector2{center.x - unit * 0.34f, center.y - unit * 1.08f},
                         Vector2{center.x + unit * 0.02f, center.y + unit * 0.90f},
                         primary);
            DrawTriangle(Vector2{center.x - unit * 0.14f, center.y + unit * 0.76f},
                         Vector2{center.x + unit * 0.94f, center.y - unit * 1.00f},
                         Vector2{center.x + unit * 1.28f, center.y + unit * 0.78f},
                         secondary);
            DrawLineEx(Vector2{center.x - unit * 0.28f, center.y - unit * 0.72f}, Vector2{center.x - unit * 0.28f, center.y + unit * 0.74f}, unit * 0.16f, Color{218, 238, 248, 210});
            DrawCircleV(Vector2{center.x + unit * 0.60f, center.y - unit * 0.30f}, unit * 0.34f, Color{255, 214, 154, 230});
            DrawCircleV(Vector2{center.x + unit * 0.60f, center.y - unit * 0.30f}, unit * 0.16f, Color{18, 30, 46, 255});
            break;
        case 1:
            DrawRectangleRounded(Rectangle{center.x - unit * 1.14f, center.y - unit * 0.42f, unit * 2.28f, unit * 1.62f}, 0.22f, 8, primary);
            DrawRectangleRounded(Rectangle{center.x - unit * 0.66f, center.y - unit * 1.02f, unit * 1.32f, unit * 0.58f}, 0.50f, 8, secondary);
            DrawLineEx(Vector2{center.x - unit * 0.72f, center.y - unit * 0.34f}, Vector2{center.x + unit * 0.72f, center.y - unit * 0.34f}, unit * 0.14f, Color{218, 238, 248, 190});
            DrawCircleV(Vector2{center.x, center.y + unit * 0.20f}, unit * 0.16f, Color{255, 235, 209, 215});
            break;
        case 2:
        default:
            DrawRectangleRounded(Rectangle{center.x - unit * 1.16f, center.y - unit * 0.82f, unit * 2.32f, unit * 1.76f}, 0.20f, 8, secondary);
            DrawCircleLines((int)center.x, (int)(center.y - unit * 0.14f), unit * 0.82f, primary);
            DrawCircleLines((int)center.x, (int)(center.y - unit * 0.14f), unit * 0.44f, Color{218, 238, 248, 180});
            DrawLineEx(Vector2{center.x - unit * 1.00f, center.y + unit * 0.90f}, Vector2{center.x + unit * 1.00f, center.y + unit * 0.90f}, unit * 0.15f, primary);
            DrawCircleV(Vector2{center.x, center.y - unit * 0.14f}, unit * 0.12f, Color{218, 238, 248, 210});
            DrawLineEx(Vector2{center.x + unit * 0.50f, center.y - unit * 0.68f}, Vector2{center.x + unit * 0.92f, center.y - unit * 1.12f}, unit * 0.10f, Color{255, 214, 154, 200});
            break;
    }
}

void UIRuntime_DrawShortcutIcon(Rectangle rect,
                                const TextureAsset *iconTexture,
                                int iconKind,
                                Color primary,
                                Color secondary) {
    DrawCircleV(Vector2{rect.x + rect.width * 0.5f, rect.y + rect.height * 0.5f},
                (rect.width < rect.height ? rect.width : rect.height) * 0.34f,
                Color{primary.r, primary.g, primary.b, 38});
    if (iconTexture != nullptr && iconTexture->loaded) {
        Rectangle textureRect = Rectangle{rect.x + rect.width * 0.14f,
                                          rect.y + rect.height * 0.14f,
                                          rect.width * 0.72f,
                                          rect.height * 0.72f};
        UIRuntime_DrawTextureAssetFitted(iconTexture, textureRect, WHITE);
        return;
    }

    DrawHudShortcutFallbackIcon(rect, iconKind, primary, secondary);
}

void UIRuntime_DrawHudShortcut(const AssetBundle *assets,
                               Rectangle rect,
                               const char *label,
                               const char *subtitle,
                               const char *keyLabel,
                               const TextureAsset *iconTexture,
                               int iconKind,
                               Color primary,
                               Color secondary) {
    float localScale;
    Rectangle iconRect;
    Rectangle keyRect;
    Vector2 subtitleSize;
    float textX;
    float textTopY;
    float textBottomY;
    float keyFontSize;
    Vector2 keySize;

    localScale = rect.height / 58.0f;
    UIRuntime_DrawPanel(rect, Color{10, 22, 36, 228}, Color{255, 255, 255, 28});
    iconRect = Rectangle{rect.x + 8.0f * localScale, rect.y + 8.0f * localScale, 34.0f * localScale, rect.height - 16.0f * localScale};
    keyRect = Rectangle{rect.x + rect.width - 30.0f * localScale, rect.y + rect.height - 28.0f * localScale, 20.0f * localScale, 18.0f * localScale};
    textX = iconRect.x + iconRect.width + 8.0f * localScale;
    textTopY = rect.y + 10.0f * localScale;
    textBottomY = rect.y + rect.height - 22.0f * localScale;
    keyFontSize = 12.0f * localScale;

    UIRuntime_DrawPanel(iconRect, Color{18, 34, 52, 235}, Color{109, 201, 234, 65});
    UIRuntime_DrawShortcutIcon(iconRect, iconTexture, iconKind, primary, secondary);
    subtitleSize = UIRuntime_MeasureText(assets, subtitle, 10.5f * localScale);
    UIRuntime_DrawText(assets,
                       label,
                       Vector2{textX, textTopY},
                       13.0f * localScale,
                       WHITE);
    UIRuntime_DrawText(assets,
                       subtitle,
                       Vector2{textX, textBottomY - subtitleSize.y * 0.5f},
                       10.5f * localScale,
                       Color{182, 199, 214, 255});
    UIRuntime_DrawPanel(keyRect, Color{33, 56, 80, 245}, Color{255, 214, 154, 85});
    keySize = UIRuntime_MeasureText(assets, keyLabel, keyFontSize);
    UIRuntime_DrawText(assets,
                       keyLabel,
                       Vector2{keyRect.x + (keyRect.width - keySize.x) * 0.5f, keyRect.y + (keyRect.height - keySize.y) * 0.5f - 0.5f * localScale},
                       keyFontSize,
                       Color{255, 234, 206, 255});
}
