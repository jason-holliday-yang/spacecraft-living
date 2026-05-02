#include "ui_system.h"

#include "localization.h"
#include "ui_runtime_internal.h"

#include <cmath>
#include <cstring>
#include <cstdio>

namespace {

struct MapContentBounds {
    int minX;
    int minY;
    int maxX;
    int maxY;
};

bool IsNorthwestRuinsFightTile(int gridX, int gridY) {
    return gridX >= BOSS_ARENA_X
        && gridX < BOSS_ARENA_X + BOSS_ARENA_WIDTH
        && gridY >= BOSS_ARENA_Y
        && gridY < BOSS_ARENA_Y + BOSS_ARENA_HEIGHT;
}

bool ShouldRevealNorthwestRuinsFightOnMap(const Player *player, const TaskSystem *tasks) {
    if (player != nullptr && Map_GetAreaAt(player->gridX, player->gridY) == MAP_AREA_BOSS_ARENA) {
        return true;
    }
    if (tasks == nullptr) {
        return false;
    }

    return tasks->bossDefeated
        || tasks->selectedEndingRoute == ENDING_HEROIC
        || tasks->selectedEndingRoute == ENDING_SETTLEMENT;
}

bool ShouldSkipMapTile(int gridX, int gridY, bool revealNorthwestRuinsFight) {
    return !revealNorthwestRuinsFight && IsNorthwestRuinsFightTile(gridX, gridY);
}

bool IsMapTileExplored(const MiniMap *minimap, int gridX, int gridY) {
    if (minimap == nullptr) {
        return true;
    }
    if (!Map_IsWithinBounds(gridX, gridY)) {
        return false;
    }

    return minimap->explored[gridY][gridX];
}

bool IsContextInSameArea(const GameMap *map, int anchorX, int anchorY, int contextX, int contextY) {
    if (map == nullptr || !Map_IsWithinBounds(anchorX, anchorY) || !Map_IsWithinBounds(contextX, contextY)) {
        return false;
    }

    return Map_GetAreaAt(anchorX, anchorY) == Map_GetAreaAt(contextX, contextY);
}

bool IsContextInSameLocation(const GameMap *map, int anchorX, int anchorY, int contextX, int contextY) {
    const char *anchorLocation;
    const char *contextLocation;

    if (map == nullptr || !Map_IsWithinBounds(anchorX, anchorY) || !Map_IsWithinBounds(contextX, contextY)) {
        return false;
    }

    anchorLocation = Map_GetLocationNameAt(anchorX, anchorY);
    contextLocation = Map_GetLocationNameAt(contextX, contextY);
    return anchorLocation != nullptr
        && contextLocation != nullptr
        && std::strcmp(anchorLocation, contextLocation) == 0;
}

bool ShouldShowMapAreaLabel(const MiniMap *minimap,
                            const GameMap *map,
                            int anchorX,
                            int anchorY,
                            const Player *player,
                            bool forceReveal) {
    if (forceReveal || IsMapTileExplored(minimap, anchorX, anchorY)) {
        return true;
    }
    if (player != nullptr && IsContextInSameArea(map, anchorX, anchorY, player->gridX, player->gridY)) {
        return true;
    }
    return false;
}

bool ShouldShowMapLocationLabel(const MiniMap *minimap,
                                const GameMap *map,
                                int anchorX,
                                int anchorY,
                                const Player *player,
                                bool forceReveal) {
    if (forceReveal || IsMapTileExplored(minimap, anchorX, anchorY)) {
        return true;
    }
    if (player != nullptr && IsContextInSameLocation(map, anchorX, anchorY, player->gridX, player->gridY)) {
        return true;
    }
    return false;
}

MapContentBounds GetMapContentBounds(const GameMap *map, bool revealNorthwestRuinsFight) {
    MapContentBounds bounds{MAP_WIDTH - 1, MAP_HEIGHT - 1, 0, 0};
    bool found = false;

    if (map == nullptr) {
        return MapContentBounds{0, 0, MAP_WIDTH - 1, MAP_HEIGHT - 1};
    }

    for (int y = 0; y < MAP_HEIGHT; ++y) {
        for (int x = 0; x < MAP_WIDTH; ++x) {
            if (ShouldSkipMapTile(x, y, revealNorthwestRuinsFight)) {
                continue;
            }
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
            return Color{255, 204, 140, 255};
        case TILE_CRASH_CLUE:
            return Color{255, 190, 126, 255};
        case TILE_VOID:
        default:
            return Color{220, 232, 242, 210};
    }
}

void DrawLegendSwatch(const AssetBundle *assets, Rectangle rect, Color swatch, const char *label, float scale) {
    Rectangle swatchRect = Rectangle{rect.x, rect.y + 4.0f * scale, 14.0f * scale, 14.0f * scale};

    DrawRectangleRounded(swatchRect, 0.28f, 6, swatch);
    UIRuntime_DrawWrappedText(
        assets,
        label,
        Rectangle{rect.x + 22.0f * scale, rect.y, rect.width - 22.0f * scale, rect.height},
        13.8f * scale,
        15.2f * scale,
        Color{214, 226, 238, 255}
    );
}

void DrawReserveFrame(Rectangle rect, Color fill, Color outline) {
    DrawRectangleRec(rect, fill);
    DrawRectangleLinesEx(rect, 2.0f, outline);
}

void DrawMapInfoCard(Rectangle rect, Color fill, Color outline) {
    DrawRectangleRounded(rect, 0.16f, 8, fill);
    DrawRectangleRoundedLinesEx(rect, 0.16f, 8, 1.4f, outline);
}

void DrawArchiveMarker(Rectangle rect, bool mainline, float elapsedSeconds) {
    Rectangle iconRect;
    Color accent;
    float pulse;

    pulse = 0.5f + 0.5f * std::sin(elapsedSeconds * 3.4f + rect.x * 0.01f + rect.y * 0.01f);
    accent = mainline ? Color{255, 198, 118, 245} : Color{118, 226, 255, 245};
    iconRect = Rectangle{
        rect.x + rect.width * 0.12f,
        rect.y + rect.height * 0.08f,
        rect.width * 0.76f,
        rect.height * 0.84f
    };

    DrawRectangleRounded(iconRect, 0.18f, 4, Color{36, 50, 70, 235});
    DrawRectangleRoundedLinesEx(iconRect, 0.18f, 4, 1.2f, Color{255, 224, 180, (unsigned char)(95 + pulse * 40.0f)});
    DrawRectangleRounded(Rectangle{iconRect.x + rect.width * 0.08f,
                                   iconRect.y + rect.height * 0.08f,
                                   iconRect.width - rect.width * 0.16f,
                                   iconRect.height - rect.height * 0.18f},
                         0.12f,
                         3,
                         Color{232, 238, 246, 240});
    DrawRectangle((int)(iconRect.x + rect.width * 0.10f),
                  (int)(iconRect.y + rect.height * 0.10f),
                  (int)(iconRect.width - rect.width * 0.20f),
                  (int)fmaxf(1.0f, rect.height * 0.10f),
                  accent);
    DrawRectangle((int)(iconRect.x + rect.width * 0.10f),
                  (int)(iconRect.y + rect.height * 0.24f),
                  (int)(iconRect.width - rect.width * 0.28f),
                  (int)fmaxf(1.0f, rect.height * 0.06f),
                  Color{126, 144, 166, 220});
    DrawRectangle((int)(iconRect.x + rect.width * 0.10f),
                  (int)(iconRect.y + rect.height * 0.34f),
                  (int)(iconRect.width - rect.width * 0.18f),
                  (int)fmaxf(1.0f, rect.height * 0.06f),
                  Color{126, 144, 166, 200});
    DrawCircle((int)(iconRect.x + iconRect.width - rect.width * 0.10f),
               (int)(iconRect.y + rect.height * 0.12f),
               fmaxf(1.0f, rect.width * 0.06f + pulse * rect.width * 0.02f),
               accent);
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
    float sideGap = 22.0f * scale;
    Rectangle mapPanel{
        panel.x + sideGap,
        panel.y + 96.0f * scale,
        panel.width - sideGap * 2.0f,
        panel.height - 128.0f * scale,
    };
    const bool revealNorthwestRuinsFight = ShouldRevealNorthwestRuinsFightOnMap(player, tasks);
    const bool northwestGuardianPending = tasks != nullptr
        && !tasks->bossDefeated
        && (tasks->selectedEndingRoute == ENDING_HEROIC || tasks->selectedEndingRoute == ENDING_SETTLEMENT);
    MapContentBounds contentBounds = GetMapContentBounds(map, revealNorthwestRuinsFight);
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
    Rectangle legendCard;
    float legendWidth;
    float legendColumnWidth;
    float legendColumnGap;
    float legendLeftX;
    float legendRightX;
    float legendRowY;
    int objectiveX = 0;
    int objectiveY = 0;
    bool hasObjective;

    if (cellSize < 3.0f) {
        cellSize = 3.0f;
    }
    mapWidth = cellSize * contentWidth;
    mapHeight = cellSize * contentHeight;
    originX = mapPanel.x + (mapPanel.width - mapWidth) * 0.5f;
    originY = mapPanel.y + (mapPanel.height - mapHeight) * 0.5f;
    auto TileScreenX = [&](int gridX) { return originX + (gridX - contentBounds.minX) * cellSize; };
    auto TileScreenY = [&](int gridY) { return originY + (gridY - contentBounds.minY) * cellSize; };
    legendWidth = fminf(440.0f * scale, mapPanel.width - 36.0f * scale);
    legendCard = Rectangle{
        mapPanel.x + mapPanel.width - legendWidth - 20.0f * scale,
        mapPanel.y + 18.0f * scale,
        legendWidth,
        224.0f * scale
    };
    legendColumnGap = 18.0f * scale;
    legendColumnWidth = (legendCard.width - 32.0f * scale - legendColumnGap) * 0.5f;
    legendLeftX = legendCard.x + 16.0f * scale;
    legendRightX = legendLeftX + legendColumnWidth + legendColumnGap;
    legendRowY = legendCard.y + 42.0f * scale;
    westReserveRect = Rectangle{
        TileScreenX(EXTERIOR_X(10)),
        TileScreenY(EXTERIOR_Y(30)),
        (EXTERIOR_X(58) - EXTERIOR_X(10)) * cellSize,
        (EXTERIOR_Y(102) - EXTERIOR_Y(30)) * cellSize
    };
    southReserveRect = Rectangle{
        TileScreenX(EXTERIOR_X(62)),
        TileScreenY(EXTERIOR_Y(86)),
        (WORLD_MAX_X - EXTERIOR_X(62) + 1) * cellSize,
        (WORLD_MAX_Y - EXTERIOR_Y(86) + 1) * cellSize
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
    UIRuntime_DrawWrappedText(assets, Loc_PickLiteral("Explore terrain, read route structure, and plan return paths.", "查看地形结构、判断路线，并规划返程。"), Rectangle{panel.x + 28.0f * scale, panel.y + 66.0f * scale, panel.width - 56.0f * scale, 42.0f * scale}, 17.0f * scale, 18.6f * scale, Color{194, 224, 255, 255});
    UIRuntime_DrawPanel(mapPanel, Color{11, 20, 32, 235}, Color{255, 255, 255, 22});

    for (int y = contentBounds.minY; y <= contentBounds.maxY; ++y) {
        for (int x = contentBounds.minX; x <= contentBounds.maxX; ++x) {
            if (ShouldSkipMapTile(x, y, revealNorthwestRuinsFight)) {
                continue;
            }
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

    for (int x = contentBounds.minX; x <= contentBounds.maxX; x += 8) {
        DrawLineEx(Vector2{TileScreenX(x), originY},
                   Vector2{TileScreenX(x), originY + mapHeight},
                   1.0f,
                   Color{146, 170, 194, 18});
    }
    for (int y = contentBounds.minY; y <= contentBounds.maxY; y += 8) {
        DrawLineEx(Vector2{originX, TileScreenY(y)},
                   Vector2{originX + mapWidth, TileScreenY(y)},
                   1.0f,
                   Color{146, 170, 194, 18});
    }

    if (hasObjective && objectiveX >= 0 && objectiveX < MAP_WIDTH && objectiveY >= 0 && objectiveY < MAP_HEIGHT
        && (minimap == nullptr || minimap->explored[objectiveY][objectiveX]
            || (revealNorthwestRuinsFight && IsNorthwestRuinsFightTile(objectiveX, objectiveY)))) {
        const Vector2 objectiveCenter = Vector2{
            TileScreenX(objectiveX) + cellSize * 0.5f,
            TileScreenY(objectiveY) + cellSize * 0.5f
        };
        const float objectiveRadius = fmaxf(cellSize * 0.42f, 3.2f);
        const float objectivePulse = 0.5f + 0.5f * std::sin(GetTime() * 4.0f);

        DrawRing(objectiveCenter,
                 objectiveRadius + cellSize * 0.18f,
                 objectiveRadius + cellSize * 0.32f + objectivePulse * cellSize * 0.12f,
                 0.0f,
                 360.0f,
                 24,
                 Color{255, 214, 154, 92});
        DrawCircleLinesV(objectiveCenter, objectiveRadius, Color{255, 214, 154, 245});
        DrawCircleV(objectiveCenter, fmaxf(cellSize * 0.13f, 1.8f), Color{255, 238, 204, 255});
    }

    if (tasks != nullptr) {
        for (int index = 0; index < tasks->logCount; ++index) {
            const ShipLog *log = &tasks->logs[index];

            if (!log->active || log->collected) {
                continue;
            }
            if (minimap != nullptr && !minimap->explored[log->gridY][log->gridX]) {
                continue;
            }

            DrawArchiveMarker(Rectangle{
                                  TileScreenX(log->gridX),
                                  TileScreenY(log->gridY),
                                  cellSize,
                                  cellSize,
                              },
                              log->category == SHIP_LOG_MAINLINE,
                              GetTime());
        }
    }

    DrawReserveFrame(westReserveRect, Color{86, 96, 110, 24}, Color{146, 162, 180, 78});
    DrawReserveFrame(southReserveRect, Color{110, 78, 68, 24}, Color{176, 134, 98, 84});

    if (revealNorthwestRuinsFight) {
        Rectangle northwestRuinsRect{
            TileScreenX(BOSS_ARENA_X),
            TileScreenY(BOSS_ARENA_Y),
            BOSS_ARENA_WIDTH * cellSize,
            BOSS_ARENA_HEIGHT * cellSize
        };
        DrawRectangleRec(northwestRuinsRect,
                         northwestGuardianPending ? Color{190, 96, 62, 34} : Color{126, 146, 168, 28});
        DrawRectangleLinesEx(northwestRuinsRect,
                             northwestGuardianPending ? 2.5f : 1.6f,
                             northwestGuardianPending ? Color{255, 178, 106, 230} : Color{196, 214, 232, 156});
        UIRuntime_DrawText(assets,
                           Loc_GetLocationNameText("Northwest Ruins"),
                           Vector2{northwestRuinsRect.x + 4.0f * scale, northwestRuinsRect.y - 18.0f * scale},
                           12.0f * scale,
                           northwestGuardianPending ? Color{255, 210, 156, 245} : Color{206, 224, 238, 220});
    }

    {
        const Vector2 playerCenter = Vector2{
            TileScreenX(player->gridX) + cellSize * 0.5f,
            TileScreenY(player->gridY) + cellSize * 0.5f
        };
        const float playerRadius = fmaxf(cellSize * 0.34f, 2.8f);
        const float playerPulse = 0.5f + 0.5f * std::sin(GetTime() * 3.2f);

        DrawRing(playerCenter,
                 playerRadius + cellSize * 0.18f,
                 playerRadius + cellSize * 0.36f + playerPulse * cellSize * 0.10f,
                 0.0f,
                 360.0f,
                 24,
                 Color{88, 255, 180, 86});
        DrawCircleV(playerCenter, playerRadius, Color{88, 255, 180, 255});
        DrawCircleLinesV(playerCenter, playerRadius + 1.0f, Color{220, 255, 238, 220});
    }

    if (ShouldShowMapAreaLabel(minimap, map, EXTERIOR_X(42), EXTERIOR_Y(4), player, false)) {
        UIRuntime_DrawText(assets, Loc_GetAreaNameText("Ruins"), Vector2{TileScreenX(EXTERIOR_X(42)), TileScreenY(EXTERIOR_Y(4))}, 14.0f * scale, Color{226, 233, 240, 245});
    }
    if (ShouldShowMapLocationLabel(minimap, map, EXTERIOR_X(57), EXTERIOR_Y(8), player, false)) {
        UIRuntime_DrawText(assets, Loc_GetLocationNameText("Signal Tower Plateau"), Vector2{TileScreenX(EXTERIOR_X(57)), TileScreenY(EXTERIOR_Y(8))}, 11.5f * scale, Color{200, 227, 242, 225});
    }
    if (ShouldShowMapLocationLabel(minimap, map, EXTERIOR_X(48), EXTERIOR_Y(15), player, false)) {
        UIRuntime_DrawText(assets, Loc_GetLocationNameText("Monolith Ring"), Vector2{TileScreenX(EXTERIOR_X(48)), TileScreenY(EXTERIOR_Y(15))}, 11.5f * scale, Color{200, 227, 242, 225});
    }
    if (ShouldShowMapLocationLabel(minimap, map, EXTERIOR_X(52), EXTERIOR_Y(26), player, false)) {
        UIRuntime_DrawText(assets, Loc_GetLocationNameText("Ruins Approach"), Vector2{TileScreenX(EXTERIOR_X(52)), TileScreenY(EXTERIOR_Y(26))}, 11.5f * scale, Color{200, 227, 242, 225});
    }
    if (ShouldShowMapAreaLabel(minimap, map, 56, 49, player, false)) {
        UIRuntime_DrawText(assets, Loc_GetAreaNameText("Ship Base"), Vector2{TileScreenX(56), TileScreenY(49)}, 14.0f * scale, Color{196, 226, 250, 245});
    }
    if (ShouldShowMapAreaLabel(minimap, map, EXTERIOR_X(98), EXTERIOR_Y(33), player, false)) {
        UIRuntime_DrawText(assets, Loc_GetAreaNameText("Spore Swamp"), Vector2{TileScreenX(EXTERIOR_X(98)), TileScreenY(EXTERIOR_Y(33))}, 14.0f * scale, Color{214, 227, 164, 245});
    }
    if (ShouldShowMapLocationLabel(minimap, map, EXTERIOR_X(96), EXTERIOR_Y(37), player, false)) {
        UIRuntime_DrawText(assets, Loc_GetLocationNameText("Outer Swamp Rim"), Vector2{TileScreenX(EXTERIOR_X(96)), TileScreenY(EXTERIOR_Y(37))}, 11.5f * scale, Color{224, 235, 176, 220});
    }
    if (ShouldShowMapLocationLabel(minimap, map, EXTERIOR_X(100), EXTERIOR_Y(59), player, false)) {
        UIRuntime_DrawText(assets, Loc_GetLocationNameText("Flooded Detour"), Vector2{TileScreenX(EXTERIOR_X(100)), TileScreenY(EXTERIOR_Y(59))}, 11.5f * scale, Color{214, 228, 180, 210});
    }
    if (ShouldShowMapLocationLabel(minimap, map, EXTERIOR_X(107), EXTERIOR_Y(28), player, false)) {
        UIRuntime_DrawText(assets, Loc_GetLocationNameText("Deep Gate"), Vector2{TileScreenX(EXTERIOR_X(107)), TileScreenY(EXTERIOR_Y(28))}, 11.5f * scale, Color{236, 212, 132, 220});
    }
    if (ShouldShowMapLocationLabel(minimap, map, EXTERIOR_X(108), EXTERIOR_Y(43), player, false)) {
        UIRuntime_DrawText(assets, Loc_GetLocationNameText("Deep Basin"), Vector2{TileScreenX(EXTERIOR_X(108)), TileScreenY(EXTERIOR_Y(43))}, 11.5f * scale, Color{224, 212, 136, 220});
    }
    if (ShouldShowMapAreaLabel(minimap, map, EXTERIOR_X(14), EXTERIOR_Y(45), player, false)) {
        UIRuntime_DrawText(assets, Loc_GetAreaNameText("Echo Wilds"), Vector2{TileScreenX(EXTERIOR_X(14)), TileScreenY(EXTERIOR_Y(45))}, 13.0f * scale, Color{208, 196, 184, 220});
    }
    if (ShouldShowMapLocationLabel(minimap, map, EXTERIOR_X(12), EXTERIOR_Y(67), player, false)) {
        UIRuntime_DrawText(assets, Loc_GetLocationNameText("West Frontier"), Vector2{TileScreenX(EXTERIOR_X(12)), TileScreenY(EXTERIOR_Y(67))}, 12.0f * scale, Color{196, 206, 220, 210});
    }
    if (ShouldShowMapLocationLabel(minimap, map, EXTERIOR_X(30), EXTERIOR_Y(76), player, false)) {
        UIRuntime_DrawText(assets, Loc_GetLocationNameText("Survey Break"), Vector2{TileScreenX(EXTERIOR_X(30)), TileScreenY(EXTERIOR_Y(76))}, 11.5f * scale, Color{196, 206, 220, 198});
    }
    if (ShouldShowMapLocationLabel(minimap, map, EXTERIOR_X(34), EXTERIOR_Y(55), player, false)) {
        UIRuntime_DrawText(assets, Loc_GetLocationNameText("Canopy Hollow"), Vector2{TileScreenX(EXTERIOR_X(34)), TileScreenY(EXTERIOR_Y(55))}, 11.0f * scale, Color{196, 206, 220, 188});
    }
    if (ShouldShowMapLocationLabel(minimap, map, EXTERIOR_X(16), EXTERIOR_Y(90), player, false)) {
        UIRuntime_DrawText(assets, Loc_GetLocationNameText("Echo Basin"), Vector2{TileScreenX(EXTERIOR_X(16)), TileScreenY(EXTERIOR_Y(90))}, 10.8f * scale, Color{196, 206, 220, 180});
    }
    if (ShouldShowMapLocationLabel(minimap, map, EXTERIOR_X(40), EXTERIOR_Y(87), player, false)) {
        UIRuntime_DrawText(assets, Loc_GetLocationNameText("Last Camp"), Vector2{TileScreenX(EXTERIOR_X(40)), TileScreenY(EXTERIOR_Y(87))}, 10.8f * scale, Color{196, 206, 220, 174});
    }
    if (ShouldShowMapAreaLabel(minimap, map, EXTERIOR_X(64), EXTERIOR_Y(84), player, false)) {
        UIRuntime_DrawText(assets, Loc_PickLiteral("Subsurface Sink", "地下沉降带"), Vector2{TileScreenX(EXTERIOR_X(64)), TileScreenY(EXTERIOR_Y(84))}, 13.0f * scale, Color{214, 190, 168, 220});
    }
    if (ShouldShowMapLocationLabel(minimap, map, EXTERIOR_X(64), EXTERIOR_Y(92), player, false)) {
        UIRuntime_DrawText(assets, Loc_GetLocationNameText("South Collapse"), Vector2{TileScreenX(EXTERIOR_X(64)), TileScreenY(EXTERIOR_Y(92))}, 12.0f * scale, Color{198, 205, 214, 210});
    }
    if (ShouldShowMapLocationLabel(minimap, map, EXTERIOR_X(82), EXTERIOR_Y(89), player, false)) {
        UIRuntime_DrawText(assets, Loc_GetLocationNameText("Vent Galleries"), Vector2{TileScreenX(EXTERIOR_X(82)), TileScreenY(EXTERIOR_Y(89))}, 11.5f * scale, Color{198, 205, 214, 198});
    }
    if (ShouldShowMapLocationLabel(minimap, map, EXTERIOR_X(96), EXTERIOR_Y(98), player, false)) {
        UIRuntime_DrawText(assets, Loc_GetLocationNameText("Service Shafts"), Vector2{TileScreenX(EXTERIOR_X(96)), TileScreenY(EXTERIOR_Y(98))}, 11.0f * scale, Color{198, 205, 214, 188});
    }
    if (ShouldShowMapLocationLabel(minimap, map, EXTERIOR_X(109), EXTERIOR_Y(91), player, false)) {
        UIRuntime_DrawText(assets, Loc_GetLocationNameText("Purifier Ring"), Vector2{TileScreenX(EXTERIOR_X(109)), TileScreenY(EXTERIOR_Y(91))}, 10.8f * scale, Color{198, 205, 214, 180});
    }
    if (ShouldShowMapLocationLabel(minimap, map, EXTERIOR_X(116), EXTERIOR_Y(101), player, false)) {
        UIRuntime_DrawText(assets, Loc_GetLocationNameText("Root Vault"), Vector2{TileScreenX(EXTERIOR_X(116)), TileScreenY(EXTERIOR_Y(101))}, 10.8f * scale, Color{198, 205, 214, 174});
    }

    DrawMapInfoCard(legendCard, Color{6, 13, 22, 210}, Color{122, 166, 214, 72});
    UIRuntime_DrawText(assets, Loc_PickLiteral("Map Legend", "地图图例"), Vector2{legendCard.x + 16.0f * scale, legendCard.y + 14.0f * scale}, 18.0f * scale, WHITE);
    DrawLegendSwatch(assets, Rectangle{legendLeftX, legendRowY + 0.0f * scale, legendColumnWidth, 22.0f * scale}, Color{88, 255, 180, 255}, Loc_PickLiteral("Player position", "玩家位置"), scale);
    DrawLegendSwatch(assets, Rectangle{legendRightX, legendRowY + 0.0f * scale, legendColumnWidth, 22.0f * scale}, Color{68, 98, 128, 255}, Loc_PickLiteral("Ship base", "飞船基地"), scale);
    DrawLegendSwatch(assets, Rectangle{legendLeftX, legendRowY + 26.0f * scale, legendColumnWidth, 22.0f * scale}, Color{255, 214, 154, 255}, Loc_PickLiteral("Objective marker", "目标标记"), scale);
    DrawLegendSwatch(assets, Rectangle{legendRightX, legendRowY + 26.0f * scale, legendColumnWidth, 22.0f * scale}, Color{58, 114, 78, 255}, Loc_PickLiteral("Forest ground", "森林地表"), scale);
    DrawLegendSwatch(assets, Rectangle{legendLeftX, legendRowY + 52.0f * scale, legendColumnWidth, 22.0f * scale}, Color{255, 198, 118, 255}, Loc_PickLiteral("Main archive", "主线档案"), scale);
    DrawLegendSwatch(assets, Rectangle{legendRightX, legendRowY + 52.0f * scale, legendColumnWidth, 22.0f * scale}, Color{92, 121, 62, 255}, Loc_PickLiteral("Swamp route", "沼泽路线"), scale);
    DrawLegendSwatch(assets, Rectangle{legendLeftX, legendRowY + 78.0f * scale, legendColumnWidth, 22.0f * scale}, Color{118, 226, 255, 255}, Loc_PickLiteral("Supplemental archive", "补充档案"), scale);
    DrawLegendSwatch(assets, Rectangle{legendRightX, legendRowY + 78.0f * scale, legendColumnWidth, 22.0f * scale}, Color{96, 104, 54, 255}, Loc_PickLiteral("Deep swamp", "深沼区域"), scale);
    DrawLegendSwatch(assets, Rectangle{legendLeftX, legendRowY + 104.0f * scale, legendColumnWidth, 22.0f * scale}, Color{14, 18, 26, 240}, Loc_PickLiteral("Unexplored area", "未探索区域"), scale);
    DrawLegendSwatch(assets, Rectangle{legendRightX, legendRowY + 104.0f * scale, legendColumnWidth, 22.0f * scale}, Color{108, 111, 122, 255}, Loc_PickLiteral("Ruins ground", "遗迹地表"), scale);
    DrawLegendSwatch(assets, Rectangle{legendLeftX, legendRowY + 130.0f * scale, legendColumnWidth, 22.0f * scale}, Color{176, 134, 98, 150}, Loc_PickLiteral("Future route frame", "后续路线框"), scale);
    DrawLegendSwatch(assets, Rectangle{legendRightX, legendRowY + 130.0f * scale, legendColumnWidth, 22.0f * scale}, Color{180, 210, 232, 255}, Loc_PickLiteral("Facility node", "设施节点"), scale);
    DrawLegendSwatch(assets, Rectangle{legendLeftX, legendRowY + 156.0f * scale, legendColumnWidth, 22.0f * scale}, Color{196, 214, 232, 190}, Loc_PickLiteral("Revealed key zone", "已揭示关键区"), scale);
}
