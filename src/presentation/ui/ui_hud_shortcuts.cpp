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
                               const char *keyLabel,
                               const TextureAsset *iconTexture,
                               int iconKind,
                               Color primary,
                               Color secondary) {
    float diameter;
    float radius;
    Vector2 center;
    Rectangle iconRect;

    (void)assets;
    (void)label;
    (void)keyLabel;

    diameter = rect.width < rect.height ? rect.width : rect.height;
    radius = diameter * 0.5f;
    center = Vector2{rect.x + rect.width * 0.5f, rect.y + rect.height * 0.5f};
    iconRect = Rectangle{center.x - diameter * 0.30f,
                         center.y - diameter * 0.30f,
                         diameter * 0.60f,
                         diameter * 0.60f};

    DrawCircleV(center, radius, Color{7, 18, 30, 92});
    DrawCircleV(center, radius * 0.92f, Color{10, 22, 36, 228});
    DrawCircleLines((int)center.x, (int)center.y, radius * 0.88f, Color{255, 255, 255, 38});
    DrawCircleLines((int)center.x, (int)center.y, radius * 0.74f, Color{primary.r, primary.g, primary.b, 54});
    UIRuntime_DrawShortcutIcon(iconRect, iconTexture, iconKind, primary, secondary);

    if (keyLabel != nullptr && keyLabel[0] != '\0') {
        Rectangle badgeRect = Rectangle{
            rect.x + rect.width - diameter * 0.36f,
            rect.y + diameter * 0.02f,
            diameter * 0.26f,
            diameter * 0.22f
        };
        Vector2 keySize = UIRuntime_MeasureText(assets, keyLabel, diameter * 0.15f);

        UIRuntime_DrawPanel(badgeRect, Color{22, 39, 58, 245}, Color{255, 214, 154, 120});
        UIRuntime_DrawText(assets,
                           keyLabel,
                           Vector2{badgeRect.x + (badgeRect.width - keySize.x) * 0.5f,
                                   badgeRect.y + (badgeRect.height - keySize.y) * 0.5f - diameter * 0.01f},
                           diameter * 0.15f,
                           Color{255, 236, 214, 255});
    }
}
