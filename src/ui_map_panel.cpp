#include "ui_system.h"

#include "localization.h"
#include "ui_runtime_internal.h"

#include <cmath>
#include <cstdio>

namespace {

struct MapContentBounds {
    int minX;
    int minY;
    int maxX;
    int maxY;
};

MapContentBounds GetMapContentBounds(const GameMap *map) {
    MapContentBounds bounds{MAP_WIDTH - 1, MAP_HEIGHT - 1, 0, 0};
    bool found = false;

    if (map == nullptr) {
        return MapContentBounds{0, 0, MAP_WIDTH - 1, MAP_HEIGHT - 1};
    }

    for (int y = 0; y < MAP_HEIGHT; ++y) {
        for (int x = 0; x < MAP_WIDTH; ++x) {
            if (Map_GetGroundTileAt(map, x, y) == TILE_VOID && Map_GetPropTileAt(map, x, y) == TILE_VOID) {
                continue;
            }

            if (!found) {
                bounds = MapContentBounds{x, y, x, y};
                found = true;
                continue;
            }

            if (x < bounds.minX) {
                bounds.minX = x;
            }
            if (y < bounds.minY) {
                bounds.minY = y;
            }
            if (x > bounds.maxX) {
                bounds.maxX = x;
            }
            if (y > bounds.maxY) {
                bounds.maxY = y;
            }
        }
    }

    if (!found) {
        return MapContentBounds{0, 0, MAP_WIDTH - 1, MAP_HEIGHT - 1};
    }

    return bounds;
}

Color GetGroundTileColor(TileType groundTile) {
    switch (groundTile) {
        case TILE_BASE_FLOOR:
            return Color{68, 98, 128, 255};
        case TILE_FOREST_GROUND:
            return Color{58, 114, 78, 255};
        case TILE_SWAMP_GROUND:
            return Color{92, 121, 62, 255};
        case TILE_DEEP_SWAMP_GROUND:
            return Color{96, 104, 54, 255};
        case TILE_RUINS_GROUND:
            return Color{108, 111, 122, 255};
        case TILE_VOID:
        default:
            return Color{18, 22, 30, 255};
    }
}

Color GetPropTileColor(TileType propTile) {
    switch (propTile) {
        case TILE_TREE:
            return Color{33, 72, 54, 255};
        case TILE_ROCK:
            return Color{103, 116, 128, 255};
        case TILE_TECH_TABLE:
        case TILE_STORAGE_LOCKER:
        case TILE_BUNK:
        case TILE_OXYGEN_CONSOLE:
        case TILE_LOXI_TERMINAL:
        case TILE_WORKBENCH:
        case TILE_AIRLOCK_CONSOLE:
        case TILE_AIRLOCK_DOOR:
        case TILE_ENERGY_CONSOLE:
            return Color{180, 210, 232, 255};
        case TILE_COMM_RELAY:
        case TILE_SIGNAL_TOWER:
            return Color{120, 242, 216, 255};
        case TILE_MONOLITH:
            return Color{206, 220, 232, 255};
        case TILE_BARRIER_SWAMP:
            return Color{134, 164, 96, 255};
        case TILE_BARRIER_DEEP:
            return Color{184, 140, 82, 255};
        case TILE_BARRIER_RUINS:
            return Color{154, 146, 126, 255};
        case TILE_LOG_SITE:
            return Color{104, 173, 255, 255};
        case TILE_CRASH_CLUE:
            return Color{255, 190, 126, 255};
        case TILE_VOID:
        default:
            return Color{220, 232, 242, 210};
    }
}

void DrawLegendSwatch(const AssetBundle *assets, Rectangle rect, Color swatch, const char *label, float scale) {
    Rectangle swatchRect = Rectangle{rect.x, rect.y + 4.0f * scale, 16.0f * scale, 16.0f * scale};

    DrawRectangleRounded(swatchRect, 0.28f, 6, swatch);
    UIRuntime_DrawWrappedText(
        assets,
        label,
        Rectangle{rect.x + 24.0f * scale, rect.y, rect.width - 24.0f * scale, rect.height},
        15.0f * scale,
        16.0f * scale,
        Color{214, 226, 238, 255}
    );
}

void DrawReserveFrame(Rectangle rect, Color fill, Color outline) {
    DrawRectangleRec(rect, fill);
    DrawRectangleLinesEx(rect, 2.0f, outline);
}

}  // namespace

void UI_DrawMapOverlay(const AssetBundle *assets,
                       const MiniMap *minimap,
                       const Player *player,
                       const TaskSystem *tasks,
                       const GameMap *map,
                       int screenWidth,
                       int screenHeight) {
    float scale = UIRuntime_GetScale(screenWidth, screenHeight);
    Rectangle panel = UI_GetStandardOverlayRect(screenWidth, screenHeight);
    Rectangle mapPanel{
        panel.x + 20.0f * scale,
        panel.y + 110.0f * scale,
        620.0f * scale,
        panel.height - 142.0f * scale,
    };
    Rectangle infoPanel{
        panel.x + 662.0f * scale,
        panel.y + 110.0f * scale,
        panel.width - 682.0f * scale,
        panel.height - 142.0f * scale,
    };
    MapContentBounds contentBounds = GetMapContentBounds(map);
    int contentWidth = contentBounds.maxX - contentBounds.minX + 1;
    int contentHeight = contentBounds.maxY - contentBounds.minY + 1;
    float cellSize = std::fmin(mapPanel.width / static_cast<float>(contentWidth),
                               mapPanel.height / static_cast<float>(contentHeight)) * 0.92f;
    float mapWidth;
    float mapHeight;
    float originX;
    float originY;
    Rectangle westReserveRect;
    Rectangle southReserveRect;
    int objectiveX = 0;
    int objectiveY = 0;
    bool hasObjective;
    char buffer[128];

    if (cellSize < 3.0f) {
        cellSize = 3.0f;
    }
    mapWidth = cellSize * contentWidth;
    mapHeight = cellSize * contentHeight;
    originX = mapPanel.x + (mapPanel.width - mapWidth) * 0.5f;
    originY = mapPanel.y + (mapPanel.height - mapHeight) * 0.5f;
    auto TileScreenX = [&](int gridX) { return originX + (gridX - contentBounds.minX) * cellSize; };
    auto TileScreenY = [&](int gridY) { return originY + (gridY - contentBounds.minY) * cellSize; };
    westReserveRect = Rectangle{
        TileScreenX(EXTERIOR_X(22)),
        TileScreenY(EXTERIOR_Y(34)),
        (EXTERIOR_X(49) - EXTERIOR_X(22)) * cellSize,
        (EXTERIOR_Y(89) - EXTERIOR_Y(34)) * cellSize
    };
    southReserveRect = Rectangle{
        TileScreenX(WORLD_MIN_X),
        TileScreenY(EXTERIOR_Y(94)),
        (WORLD_MAX_X - WORLD_MIN_X + 1) * cellSize,
        (WORLD_MAX_Y - EXTERIOR_Y(94) + 1) * cellSize
    };
    hasObjective = Tasks_GetObjectiveMarker(tasks, player, &objectiveX, &objectiveY);

    DrawRectangle(0, 0, screenWidth, screenHeight, Color{5, 9, 16, 198});
    UIRuntime_DrawPanel(panel, Color{8, 18, 30, 245}, Color{124, 166, 214, 75});
    UIRuntime_DrawText(assets, Loc_PickLiteral("Area Map", "区域地图"), Vector2{panel.x + 28.0f * scale, panel.y + 24.0f * scale}, 34.0f * scale, WHITE);
    UIRuntime_DrawText(
        assets,
        Loc_PickLiteral("Press M or ESC to close.", "按 M 或 ESC 关闭。"),
        Vector2{
            panel.x + panel.width - UIRuntime_MeasureText(assets, Loc_PickLiteral("Press M or ESC to close.", "按 M 或 ESC 关闭。"), 17.0f * scale).x - 28.0f * scale,
            panel.y + 30.0f * scale,
        },
        17.0f * scale,
        Color{182, 199, 214, 255}
    );
    UIRuntime_DrawWrappedText(assets, Loc_PickLiteral("Explore, track objectives, and plan return routes.", "探索地图、追踪目标，并规划返程路线。"), Rectangle{panel.x + 28.0f * scale, panel.y + 66.0f * scale, panel.width - 56.0f * scale, 42.0f * scale}, 15.5f * scale, 16.0f * scale, Color{194, 224, 255, 255});
    UIRuntime_DrawPanel(mapPanel, Color{11, 20, 32, 235}, Color{255, 255, 255, 22});
    UIRuntime_DrawPanel(infoPanel, Color{14, 26, 42, 220}, Color{255, 255, 255, 22});

    for (int y = contentBounds.minY; y <= contentBounds.maxY; ++y) {
        for (int x = contentBounds.minX; x <= contentBounds.maxX; ++x) {
            Rectangle tileRect{
                TileScreenX(x),
                TileScreenY(y),
                cellSize,
                cellSize,
            };

            if (minimap != nullptr && !minimap->explored[y][x]) {
                DrawRectangleRec(tileRect, Color{14, 18, 26, 240});
                continue;
            }

            TileType groundTile = Map_GetGroundTileAt(map, x, y);
            TileType propTile = Map_GetPropTileAt(map, x, y);
            DrawRectangleRec(tileRect, GetGroundTileColor(groundTile));
            if (propTile != TILE_VOID) {
                DrawRectangle(static_cast<int>(tileRect.x),
                              static_cast<int>(tileRect.y),
                              static_cast<int>(tileRect.width),
                              static_cast<int>(tileRect.height),
                              GetPropTileColor(propTile));
            }
        }
    }

    if (hasObjective && objectiveX >= 0 && objectiveX < MAP_WIDTH && objectiveY >= 0 && objectiveY < MAP_HEIGHT
        && (minimap == nullptr || minimap->explored[objectiveY][objectiveX])) {
        DrawRectangleLinesEx(
            Rectangle{
                TileScreenX(objectiveX) - 1.0f,
                TileScreenY(objectiveY) - 1.0f,
                cellSize + 2.0f,
                cellSize + 2.0f,
            },
            2.0f,
            Color{255, 214, 154, 220}
        );
    }

    DrawReserveFrame(westReserveRect, Color{86, 96, 110, 24}, Color{146, 162, 180, 78});
    DrawReserveFrame(southReserveRect, Color{110, 78, 68, 24}, Color{176, 134, 98, 84});

    DrawRectangle(static_cast<int>(TileScreenX(player->gridX)),
                  static_cast<int>(TileScreenY(player->gridY)),
                  static_cast<int>(cellSize),
                  static_cast<int>(cellSize),
                  Color{88, 255, 180, 255});

    UIRuntime_DrawText(assets, Loc_GetAreaNameText("Ruins"), Vector2{TileScreenX(EXTERIOR_X(49)), TileScreenY(EXTERIOR_Y(4))}, 14.0f * scale, Color{226, 233, 240, 245});
    UIRuntime_DrawText(assets, Loc_GetLocationNameText("Signal Tower Plateau"), Vector2{TileScreenX(EXTERIOR_X(58)), TileScreenY(EXTERIOR_Y(9))}, 11.5f * scale, Color{200, 227, 242, 225});
    UIRuntime_DrawText(assets, Loc_GetLocationNameText("Monolith Ring"), Vector2{TileScreenX(EXTERIOR_X(54)), TileScreenY(EXTERIOR_Y(15))}, 11.5f * scale, Color{200, 227, 242, 225});
    UIRuntime_DrawText(assets, Loc_GetLocationNameText("Ruins Approach"), Vector2{TileScreenX(EXTERIOR_X(58)), TileScreenY(EXTERIOR_Y(24))}, 11.5f * scale, Color{200, 227, 242, 225});
    UIRuntime_DrawText(assets, Loc_GetAreaNameText("Ship Base"), Vector2{TileScreenX(56), TileScreenY(49)}, 14.0f * scale, Color{196, 226, 250, 245});
    UIRuntime_DrawText(assets, Loc_GetAreaNameText("Spore Swamp"), Vector2{TileScreenX(EXTERIOR_X(98)), TileScreenY(EXTERIOR_Y(45))}, 14.0f * scale, Color{214, 227, 164, 245});
    UIRuntime_DrawText(assets, Loc_GetLocationNameText("Outer Swamp Rim"), Vector2{TileScreenX(EXTERIOR_X(96)), TileScreenY(EXTERIOR_Y(36))}, 11.5f * scale, Color{224, 235, 176, 220});
    UIRuntime_DrawText(assets, Loc_GetLocationNameText("Flooded Detour"), Vector2{TileScreenX(EXTERIOR_X(101)), TileScreenY(EXTERIOR_Y(63))}, 11.5f * scale, Color{214, 228, 180, 210});
    UIRuntime_DrawText(assets, Loc_GetLocationNameText("Deep Gate"), Vector2{TileScreenX(EXTERIOR_X(108)), TileScreenY(EXTERIOR_Y(40))}, 11.5f * scale, Color{236, 212, 132, 220});
    UIRuntime_DrawText(assets, Loc_GetLocationNameText("Deep Basin"), Vector2{TileScreenX(EXTERIOR_X(109)), TileScreenY(EXTERIOR_Y(71))}, 11.5f * scale, Color{224, 212, 136, 220});
    UIRuntime_DrawText(assets, Loc_GetAreaNameText("Echo Wilds"), Vector2{TileScreenX(EXTERIOR_X(18)), TileScreenY(EXTERIOR_Y(54))}, 13.0f * scale, Color{208, 196, 184, 220});
    UIRuntime_DrawText(assets, Loc_GetLocationNameText("West Frontier"), Vector2{TileScreenX(EXTERIOR_X(16)), TileScreenY(EXTERIOR_Y(62))}, 12.0f * scale, Color{196, 206, 220, 210});
    UIRuntime_DrawText(assets, Loc_GetLocationNameText("Survey Break"), Vector2{TileScreenX(EXTERIOR_X(35)), TileScreenY(EXTERIOR_Y(68))}, 11.5f * scale, Color{196, 206, 220, 198});
    UIRuntime_DrawText(assets, Loc_GetLocationNameText("Canopy Hollow"), Vector2{TileScreenX(EXTERIOR_X(42)), TileScreenY(EXTERIOR_Y(60))}, 11.0f * scale, Color{196, 206, 220, 188});
    UIRuntime_DrawText(assets, Loc_GetLocationNameText("Echo Basin"), Vector2{TileScreenX(EXTERIOR_X(47)), TileScreenY(EXTERIOR_Y(72))}, 10.8f * scale, Color{196, 206, 220, 180});
    UIRuntime_DrawText(assets, Loc_GetLocationNameText("Last Camp"), Vector2{TileScreenX(EXTERIOR_X(47)), TileScreenY(EXTERIOR_Y(86))}, 10.8f * scale, Color{196, 206, 220, 174});
    UIRuntime_DrawText(assets, Loc_PickLiteral("Subsurface Sink", "地下沉降带"), Vector2{TileScreenX(EXTERIOR_X(58)), TileScreenY(EXTERIOR_Y(95))}, 13.0f * scale, Color{214, 190, 168, 220});
    UIRuntime_DrawText(assets, Loc_GetLocationNameText("South Collapse"), Vector2{TileScreenX(EXTERIOR_X(61)), TileScreenY(EXTERIOR_Y(98))}, 12.0f * scale, Color{198, 205, 214, 210});
    UIRuntime_DrawText(assets, Loc_GetLocationNameText("Vent Galleries"), Vector2{TileScreenX(EXTERIOR_X(83)), TileScreenY(EXTERIOR_Y(98))}, 11.5f * scale, Color{198, 205, 214, 198});
    UIRuntime_DrawText(assets, Loc_GetLocationNameText("Service Shafts"), Vector2{TileScreenX(EXTERIOR_X(96)), TileScreenY(EXTERIOR_Y(98))}, 11.0f * scale, Color{198, 205, 214, 188});
    UIRuntime_DrawText(assets, Loc_GetLocationNameText("Purifier Ring"), Vector2{TileScreenX(EXTERIOR_X(104)), TileScreenY(EXTERIOR_Y(98))}, 10.8f * scale, Color{198, 205, 214, 180});
    UIRuntime_DrawText(assets, Loc_GetLocationNameText("Root Vault"), Vector2{TileScreenX(EXTERIOR_X(112)), TileScreenY(EXTERIOR_Y(98))}, 10.8f * scale, Color{198, 205, 214, 174});

    UIRuntime_DrawText(assets, Loc_PickLiteral("Legend", "图例"), Vector2{infoPanel.x + 18.0f * scale, infoPanel.y + 18.0f * scale}, 24.0f * scale, WHITE);
    DrawLegendSwatch(assets, Rectangle{infoPanel.x + 18.0f * scale, infoPanel.y + 56.0f * scale, infoPanel.width - 36.0f * scale, 24.0f * scale}, Color{88, 255, 180, 255}, Loc_PickLiteral("Player marker", "玩家位置"), scale);
    DrawLegendSwatch(assets, Rectangle{infoPanel.x + 18.0f * scale, infoPanel.y + 84.0f * scale, infoPanel.width - 36.0f * scale, 24.0f * scale}, Color{255, 214, 154, 255}, Loc_PickLiteral("Objective marker", "目标位置"), scale);
    DrawLegendSwatch(assets, Rectangle{infoPanel.x + 18.0f * scale, infoPanel.y + 126.0f * scale, infoPanel.width - 36.0f * scale, 24.0f * scale}, Color{68, 98, 128, 255}, Loc_PickLiteral("Ship base", "飞船基地"), scale);
    DrawLegendSwatch(assets, Rectangle{infoPanel.x + 18.0f * scale, infoPanel.y + 154.0f * scale, infoPanel.width - 36.0f * scale, 24.0f * scale}, Color{58, 114, 78, 255}, Loc_PickLiteral("Crash forest", "坠毁森林"), scale);
    DrawLegendSwatch(assets, Rectangle{infoPanel.x + 18.0f * scale, infoPanel.y + 182.0f * scale, infoPanel.width - 36.0f * scale, 24.0f * scale}, Color{92, 121, 62, 255}, Loc_PickLiteral("East swamp route", "东侧沼泽路线"), scale);
    DrawLegendSwatch(assets, Rectangle{infoPanel.x + 18.0f * scale, infoPanel.y + 210.0f * scale, infoPanel.width - 36.0f * scale, 24.0f * scale}, Color{108, 111, 122, 255}, Loc_PickLiteral("Ruins", "遗迹"), scale);
    UIRuntime_DrawText(assets, Loc_PickLiteral("Current Objective", "当前目标"), Vector2{infoPanel.x + 18.0f * scale, infoPanel.y + 262.0f * scale}, 21.0f * scale, WHITE);
    UIRuntime_DrawWrappedText(
        assets,
        tasks->objective,
        Rectangle{
            infoPanel.x + 18.0f * scale,
            infoPanel.y + 294.0f * scale,
            infoPanel.width - 36.0f * scale,
            92.0f * scale,
        },
        14.5f * scale,
        17.0f * scale,
        Color{214, 226, 238, 255}
    );
    UIRuntime_DrawPanel(Rectangle{infoPanel.x + 18.0f * scale, infoPanel.y + 396.0f * scale, infoPanel.width - 36.0f * scale, 118.0f * scale}, Color{12, 24, 39, 220}, Color{255, 255, 255, 18});
    std::snprintf(buffer, sizeof(buffer), "%s  %d, %d", Loc_PickLiteral("Position", "坐标"), player->gridX, player->gridY);
    UIRuntime_DrawText(assets, buffer, Vector2{infoPanel.x + 32.0f * scale, infoPanel.y + 414.0f * scale}, 16.0f * scale, Color{194, 224, 255, 255});
    std::snprintf(buffer, sizeof(buffer), "%s  %s", Loc_PickLiteral("Current Area", "当前区域"), Loc_GetAreaNameText(Map_GetAreaName(Map_GetAreaAt(player->gridX, player->gridY))));
    UIRuntime_DrawWrappedText(
        assets,
        buffer,
        Rectangle{infoPanel.x + 32.0f * scale, infoPanel.y + 436.0f * scale, infoPanel.width - 64.0f * scale, 22.0f * scale},
        15.0f * scale,
        18.0f * scale,
        Color{174, 226, 255, 255}
    );
    std::snprintf(buffer, sizeof(buffer), "%s  %s", Loc_PickLiteral("Location", "位置"), Loc_GetLocationNameText(Map_GetLocationNameAt(player->gridX, player->gridY)));
    UIRuntime_DrawWrappedText(
        assets,
        buffer,
        Rectangle{infoPanel.x + 32.0f * scale, infoPanel.y + 458.0f * scale, infoPanel.width - 64.0f * scale, 24.0f * scale},
        15.0f * scale,
        16.0f * scale,
        Color{198, 211, 224, 255}
    );
    UIRuntime_DrawWrappedText(assets,
                              Loc_PickLiteral("West frontier opens after the comm relay. The southern descent opens after the power bay. Ruins prep deepens as you bring back more field evidence.", "通讯中继修复后会开放西部前线；动力舱恢复后会开放南侧下行路线。随着你带回更多现场证据，遗迹推进也会继续加深。"),
                              Rectangle{infoPanel.x + 18.0f * scale, infoPanel.y + 498.0f * scale, infoPanel.width - 36.0f * scale, 92.0f * scale},
                              13.0f * scale,
                              14.5f * scale,
                              Color{182, 199, 214, 255});
}
