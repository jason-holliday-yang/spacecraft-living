#include "ui_runtime_internal.h"

static void DrawHudShortcutIcon(Rectangle rect, int iconKind, Color primary, Color secondary) {
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

void UIRuntime_DrawHudShortcut(const AssetBundle *assets,
                               Rectangle rect,
                               const char *label,
                               const char *subtitle,
                               const char *keyLabel,
                               int iconKind,
                               Color primary,
                               Color secondary) {
    float localScale;
    Rectangle iconRect;
    Rectangle keyRect;
    Vector2 labelSize;
    float labelX;
    float labelMinX;
    float labelMaxX;

    localScale = rect.height / 58.0f;
    UIRuntime_DrawPanel(rect, Color{10, 22, 36, 228}, Color{255, 255, 255, 28});
    iconRect = Rectangle{rect.x + 8.0f * localScale, rect.y + 8.0f * localScale, 34.0f * localScale, rect.height - 16.0f * localScale};
    keyRect = Rectangle{rect.x + rect.width - 30.0f * localScale, rect.y + 10.0f * localScale, 20.0f * localScale, 18.0f * localScale};
    UIRuntime_DrawPanel(iconRect, Color{18, 34, 52, 235}, Color{109, 201, 234, 65});
    DrawHudShortcutIcon(iconRect, iconKind, primary, secondary);
    labelSize = UIRuntime_MeasureText(assets, label, 13.5f * localScale);
    labelMinX = iconRect.x + iconRect.width + 8.0f * localScale;
    labelMaxX = keyRect.x - 6.0f * localScale - labelSize.x;
    labelX = labelMinX;
    if (labelMaxX > labelMinX) {
        labelX = labelMinX + (labelMaxX - labelMinX) * 0.5f;
    }
    UIRuntime_DrawText(assets,
                       label,
                       Vector2{labelX, rect.y + rect.height * 0.5f - labelSize.y * 0.5f},
                       13.5f * localScale,
                       WHITE);
    UIRuntime_DrawPanel(keyRect, Color{33, 56, 80, 245}, Color{255, 214, 154, 85});
    UIRuntime_DrawText(assets, keyLabel, Vector2{keyRect.x + 6.0f * localScale, keyRect.y + 2.5f * localScale}, 12.0f * localScale, Color{255, 234, 206, 255});
    (void)subtitle;
}
