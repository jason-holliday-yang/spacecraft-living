#include "map.h"

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static void Require(bool condition, const char *message) {
    if (condition) {
        return;
    }

    fprintf(stderr, "map_layout_smoke failed: %s\n", message);
    exit(1);
}

static bool IsShipRoom(int gridX, int gridY) {
    return (gridX >= SHIP_CORRIDOR_X && gridX < SHIP_CORRIDOR_X + SHIP_CORRIDOR_WIDTH
            && gridY >= SHIP_CORRIDOR_Y && gridY < SHIP_CORRIDOR_Y + SHIP_CORRIDOR_HEIGHT)
        || (gridX >= SHIP_CARGO_HOLD_X && gridX < SHIP_CARGO_HOLD_X + SHIP_CARGO_HOLD_WIDTH
            && gridY >= SHIP_CARGO_HOLD_Y && gridY < SHIP_CARGO_HOLD_Y + SHIP_CARGO_HOLD_HEIGHT)
        || (gridX >= SHIP_CREW_QUARTERS_X && gridX < SHIP_CREW_QUARTERS_X + SHIP_CREW_QUARTERS_WIDTH
            && gridY >= SHIP_CREW_QUARTERS_Y && gridY < SHIP_CREW_QUARTERS_Y + SHIP_CREW_QUARTERS_HEIGHT)
        || (gridX >= SHIP_DIAGNOSTICS_X && gridX < SHIP_DIAGNOSTICS_X + SHIP_DIAGNOSTICS_WIDTH
            && gridY >= SHIP_DIAGNOSTICS_Y && gridY < SHIP_DIAGNOSTICS_Y + SHIP_DIAGNOSTICS_HEIGHT)
        || (gridX >= SHIP_TERMINAL_BAY_X && gridX < SHIP_TERMINAL_BAY_X + SHIP_TERMINAL_BAY_WIDTH
            && gridY >= SHIP_TERMINAL_BAY_Y && gridY < SHIP_TERMINAL_BAY_Y + SHIP_TERMINAL_BAY_HEIGHT)
        || (gridX >= SHIP_LIFE_SUPPORT_X && gridX < SHIP_LIFE_SUPPORT_X + SHIP_LIFE_SUPPORT_WIDTH
            && gridY >= SHIP_LIFE_SUPPORT_Y && gridY < SHIP_LIFE_SUPPORT_Y + SHIP_LIFE_SUPPORT_HEIGHT)
        || (gridX >= SHIP_WORKSHOP_X && gridX < SHIP_WORKSHOP_X + SHIP_WORKSHOP_WIDTH
            && gridY >= SHIP_WORKSHOP_Y && gridY < SHIP_WORKSHOP_Y + SHIP_WORKSHOP_HEIGHT)
        || (gridX >= SHIP_POWER_BAY_X && gridX < SHIP_POWER_BAY_X + SHIP_POWER_BAY_WIDTH
            && gridY >= SHIP_POWER_BAY_Y && gridY < SHIP_POWER_BAY_Y + SHIP_POWER_BAY_HEIGHT)
        || (gridX >= SHIP_AIRLOCK_LINK_X && gridX < SHIP_AIRLOCK_LINK_X + SHIP_AIRLOCK_LINK_WIDTH
            && gridY >= SHIP_AIRLOCK_LINK_Y && gridY < SHIP_AIRLOCK_LINK_Y + SHIP_AIRLOCK_LINK_HEIGHT);
}

static int CountPropTiles(const GameMap *map, TileType tile) {
    int count;
    int gridY;
    int gridX;

    count = 0;
    for (gridY = 0; gridY < MAP_HEIGHT; ++gridY) {
        for (gridX = 0; gridX < MAP_WIDTH; ++gridX) {
            if (map->propTiles[gridY][gridX] == tile) {
                count += 1;
            }
        }
    }

    return count;
}

static bool CanReach(const GameMap *map, int startX, int startY, int targetX, int targetY) {
    bool visited[MAP_HEIGHT][MAP_WIDTH];
    int queueX[MAP_WIDTH * MAP_HEIGHT];
    int queueY[MAP_WIDTH * MAP_HEIGHT];
    int head;
    int tail;

    if (!Map_IsWalkable(map, startX, startY) || !Map_IsWalkable(map, targetX, targetY)) {
        return false;
    }

    memset(visited, 0, sizeof(visited));
    head = 0;
    tail = 0;
    visited[startY][startX] = true;
    queueX[tail] = startX;
    queueY[tail] = startY;
    tail += 1;

    while (head < tail) {
        static const int kDirections[4][2] = {
            {1, 0},
            {-1, 0},
            {0, 1},
            {0, -1}
        };
        int currentX;
        int currentY;
        int directionIndex;

        currentX = queueX[head];
        currentY = queueY[head];
        head += 1;

        if (currentX == targetX && currentY == targetY) {
            return true;
        }

        for (directionIndex = 0; directionIndex < 4; directionIndex++) {
            int nextX;
            int nextY;

            nextX = currentX + kDirections[directionIndex][0];
            nextY = currentY + kDirections[directionIndex][1];
            if (!Map_IsWithinBounds(nextX, nextY)
                || visited[nextY][nextX]
                || !Map_IsWalkable(map, nextX, nextY)) {
                continue;
            }

            visited[nextY][nextX] = true;
            queueX[tail] = nextX;
            queueY[tail] = nextY;
            tail += 1;
        }
    }

    return false;
}

static void RequireVoidHullBoundary(const GameMap *map, int gridX, int gridY, const char *message) {
    Require(Map_GetGroundTileAt(map, gridX, gridY) == TILE_VOID, message);
    Require(Map_GetPropTileAt(map, gridX, gridY) == TILE_VOID, message);
    Require(!Map_IsWalkable(map, gridX, gridY), message);
}

static bool IsWithinPlayableWorldEnvelope(int gridX, int gridY) {
    return (gridX >= WORLD_MIN_X && gridX <= WORLD_MAX_X
            && gridY >= WORLD_MIN_Y && gridY <= WORLD_MAX_Y)
        || (gridX >= BOSS_ARENA_X && gridX < BOSS_ARENA_X + BOSS_ARENA_WIDTH
            && gridY >= BOSS_ARENA_Y && gridY < BOSS_ARENA_Y + BOSS_ARENA_HEIGHT);
}

int main(void) {
    GameMap map;
    int gridY;
    int gridX;
    int offsetX;
    int offsetY;

    Map_Init(&map);

    for (gridY = 0; gridY < MAP_HEIGHT; ++gridY) {
        for (gridX = 0; gridX < MAP_WIDTH; ++gridX) {
            TileType groundTile;
            TileType propTile;

            groundTile = map.groundTiles[gridY][gridX];
            propTile = map.propTiles[gridY][gridX];

            Require(!(groundTile == TILE_VOID && propTile != TILE_VOID), "props should not be placed on void tiles");
            Require(!(groundTile == TILE_BASE_FLOOR && (propTile == TILE_TREE || propTile == TILE_ROCK)),
                    "natural props should not appear on ship floor");
            Require(!(IsShipRoom(gridX, gridY) && (propTile == TILE_TREE || propTile == TILE_ROCK)),
                    "ship rooms should not contain trees or rocks");
            Require((groundTile == TILE_VOID && propTile == TILE_VOID) || IsWithinPlayableWorldEnvelope(gridX, gridY),
                    "map content should not leak outside the playable world envelope");
        }
    }

    Require(Map_GetGroundTileAt(&map, PLAYER_START_X, PLAYER_START_Y) == TILE_BASE_FLOOR,
            "player start should remain on ship floor");
    Require(Map_IsWalkable(&map, PLAYER_RESPAWN_X, PLAYER_RESPAWN_Y),
            "player respawn should stay on a walkable tile inside the Loxi room");
    Require(SHIP_CREW_QUARTERS_X == SHIP_DIAGNOSTICS_X
                && SHIP_TERMINAL_BAY_X == SHIP_LIFE_SUPPORT_X
                && SHIP_WORKSHOP_X == SHIP_POWER_BAY_X,
            "ship cabin columns should stay symmetric above and below the corridor");
    Require(SHIP_CREW_QUARTERS_WIDTH == SHIP_DIAGNOSTICS_WIDTH
                && SHIP_TERMINAL_BAY_WIDTH == SHIP_LIFE_SUPPORT_WIDTH
                && SHIP_WORKSHOP_WIDTH == SHIP_POWER_BAY_WIDTH,
            "symmetric cabin pairs should keep matching widths");
    Require(SHIP_WORKSHOP_WIDTH == 5
                && SHIP_POWER_BAY_WIDTH == 5
                && SHIP_AIRLOCK_LINK_WIDTH == 5
                && SHIP_AIRLOCK_LINK_HEIGHT == 5,
            "right-side cabin stack should use the intended five-tile symmetric module");
    Require(SHIP_CORRIDOR_Y - (SHIP_CREW_QUARTERS_Y + SHIP_CREW_QUARTERS_HEIGHT)
                == SHIP_DIAGNOSTICS_Y - (SHIP_CORRIDOR_Y + SHIP_CORRIDOR_HEIGHT),
            "upper and lower cabin offsets should stay symmetric around the corridor");
    Require(SHIP_AIRLOCK_LINK_Y - (SHIP_WORKSHOP_Y + SHIP_WORKSHOP_HEIGHT)
                == SHIP_POWER_BAY_Y - (SHIP_AIRLOCK_LINK_Y + SHIP_AIRLOCK_LINK_HEIGHT),
            "right-side cabins should stay symmetric around the airlock hall");
    Require(CanReach(&map, PLAYER_START_X, PLAYER_START_Y, SHIP_CARGO_HOLD_X + SHIP_CARGO_HOLD_WIDTH - 1, AIRLOCK_DOOR_Y),
            "central corridor should stay connected to the cargo hold");
    Require(CanReach(&map, PLAYER_START_X, PLAYER_START_Y, SHIP_CREW_QUARTERS_X + SHIP_CREW_QUARTERS_WIDTH - 1, SHIP_CREW_QUARTERS_Y + SHIP_CREW_QUARTERS_HEIGHT - 1),
            "central corridor should connect into crew quarters");
    Require(!CanReach(&map, PLAYER_START_X, PLAYER_START_Y, PLAYER_RESPAWN_X, PLAYER_RESPAWN_Y),
            "central corridor should not reach the Loxi room before the uplink door is removed");
    Require(!CanReach(&map, PLAYER_RESPAWN_X, PLAYER_RESPAWN_Y, SHIP_CORRIDOR_X + 10, SHIP_CORRIDOR_Y + 1),
            "the player should not be able to leave the Loxi room before the first terminal sync");
    Require(CanReach(&map, PLAYER_START_X, PLAYER_START_Y, SHIP_WORKSHOP_X, SHIP_WORKSHOP_Y + SHIP_WORKSHOP_HEIGHT - 1),
            "central corridor should connect into the workshop");
    Require(CanReach(&map, PLAYER_START_X, PLAYER_START_Y, SHIP_DIAGNOSTICS_X + SHIP_DIAGNOSTICS_WIDTH - 1, SHIP_DIAGNOSTICS_Y),
            "central corridor should connect into diagnostics");
    Require(CanReach(&map, PLAYER_START_X, PLAYER_START_Y, SHIP_LIFE_SUPPORT_X, SHIP_LIFE_SUPPORT_Y),
            "central corridor should connect into life support");
    Require(CanReach(&map, PLAYER_START_X, PLAYER_START_Y, SHIP_POWER_BAY_X, SHIP_POWER_BAY_Y),
            "central corridor should connect into the power bay");
    for (gridY = SHIP_WORKSHOP_Y + SHIP_WORKSHOP_HEIGHT; gridY < SHIP_CORRIDOR_Y; ++gridY) {
        Require(Map_GetGroundTileAt(&map, SHIP_WORKSHOP_X + 1, gridY) == TILE_BASE_FLOOR
                    && Map_GetGroundTileAt(&map, SHIP_WORKSHOP_X + 2, gridY) == TILE_BASE_FLOOR
                    && Map_GetGroundTileAt(&map, SHIP_WORKSHOP_X + 3, gridY) == TILE_BASE_FLOOR,
                "right upper connector should stay centered under the workshop");
        Require(Map_GetGroundTileAt(&map, SHIP_WORKSHOP_X, gridY) == TILE_VOID,
                "right upper connector should not drift left of the workshop centerline");
    }
    for (gridY = SHIP_CORRIDOR_Y + SHIP_CORRIDOR_HEIGHT; gridY < SHIP_POWER_BAY_Y; ++gridY) {
        Require(Map_GetGroundTileAt(&map, SHIP_POWER_BAY_X + 1, gridY) == TILE_BASE_FLOOR
                    && Map_GetGroundTileAt(&map, SHIP_POWER_BAY_X + 2, gridY) == TILE_BASE_FLOOR
                    && Map_GetGroundTileAt(&map, SHIP_POWER_BAY_X + 3, gridY) == TILE_BASE_FLOOR,
                "right lower connector should stay centered above the power bay");
        Require(Map_GetGroundTileAt(&map, SHIP_POWER_BAY_X, gridY) == TILE_VOID,
                "right lower connector should not drift left of the power-bay centerline");
    }
    RequireVoidHullBoundary(&map, SHIP_CARGO_HOLD_X - 1, AIRLOCK_DOOR_Y, "left hull should stay sealed");
    RequireVoidHullBoundary(&map, PLAYER_START_X, SHIP_CREW_QUARTERS_Y - 1, "top hull should stay sealed");
    RequireVoidHullBoundary(&map, PLAYER_START_X, SHIP_POWER_BAY_Y + SHIP_POWER_BAY_HEIGHT, "bottom hull should stay sealed");
    RequireVoidHullBoundary(&map, AIRLOCK_DOOR_X, AIRLOCK_DOOR_TOP_Y - 1, "airlock should not leak above the door");
    RequireVoidHullBoundary(&map, AIRLOCK_DOOR_X, AIRLOCK_DOOR_TOP_Y + AIRLOCK_DOOR_HEIGHT, "airlock should not leak below the door");
    Require(Map_GetGroundTileAt(&map, SHIP_CARGO_HOLD_X - 1, AIRLOCK_DOOR_Y) != TILE_BASE_FLOOR,
            "left side should stay sealed off instead of connecting cargo directly to ship-floor continuity");
    Require(!Map_IsWalkable(&map, SHIP_CARGO_HOLD_X - 1, AIRLOCK_DOOR_Y),
            "left side should not be walkable beside the cargo hold");
    Require(Map_GetGroundTileAt(&map, SHIP_CARGO_HOLD_X - 4, AIRLOCK_DOOR_Y) != TILE_BASE_FLOOR,
            "left side should not expose an additional ship-floor breach");
    Require(Map_GetGroundTileAt(&map, AIRLOCK_DOOR_X + AIRLOCK_EXIT_WIDTH, AIRLOCK_DOOR_Y) != TILE_VOID,
            "right side should still open into the exterior approach");
    Require(!CanReach(&map, PLAYER_START_X, PLAYER_START_Y, AIRLOCK_DOOR_X + 1, AIRLOCK_DOOR_Y),
            "ship interior should not reach the exterior before the airlock unlocks");
    for (offsetY = 0; offsetY < AIRLOCK_DOOR_HEIGHT; offsetY++) {
        Require(Map_GetPropTileAt(&map, AIRLOCK_DOOR_X, AIRLOCK_DOOR_TOP_Y + offsetY) == TILE_AIRLOCK_DOOR,
                "airlock door footprint should remain initialized across its full height before unlocking");
    }
    for (offsetX = 0; offsetX < LOXI_ROOM_DOOR_WIDTH; offsetX++) {
        Require(Map_GetPropTileAt(&map, LOXI_ROOM_DOOR_X + offsetX, LOXI_ROOM_DOOR_Y) == TILE_LOXI_ROOM_DOOR,
                "Loxi room door should seal the terminal cabin before the first sync");
    }
    Map_UnlockLoxiRoom(&map);
    Require(Map_IsLoxiRoomUnlocked(&map),
            "unlocking the Loxi room should clear the cabin door");
    Require(CanReach(&map, PLAYER_RESPAWN_X, PLAYER_RESPAWN_Y, SHIP_CORRIDOR_X + 10, SHIP_CORRIDOR_Y + 1),
            "once unlocked, the Loxi room should reconnect to the central corridor");
    Require(CanReach(&map, PLAYER_START_X, PLAYER_START_Y, SHIP_TERMINAL_BAY_X, SHIP_TERMINAL_BAY_Y + SHIP_TERMINAL_BAY_HEIGHT - 1),
            "central corridor should reconnect into the terminal bay after the cabin door unlocks");
    Map_UnlockSwampOuter(&map);
    Require(CanReach(&map, PLAYER_START_X, PLAYER_START_Y, AIRLOCK_DOOR_X + 1, AIRLOCK_DOOR_Y),
            "unlocking the airlock should connect the ship interior to the exterior approach");
    for (offsetX = 1; offsetX <= AIRLOCK_EXIT_WIDTH; offsetX++) {
        Require(Map_IsWalkable(&map, AIRLOCK_DOOR_X + offsetX, AIRLOCK_DOOR_Y),
                "exterior approach should keep a full five-tile connection band");
    }
    Require(CanReach(&map, PLAYER_START_X, PLAYER_START_Y, EXTERIOR_X(98), EXTERIOR_Y(48)),
            "airlock exit should reconnect into the outer-swamp route");
    Require(CanReach(&map, PLAYER_START_X, PLAYER_START_Y, EXTERIOR_X(64), EXTERIOR_Y(27)),
            "airlock exit should reconnect into the ruins approach");
    Require(CanReach(&map, PLAYER_START_X, PLAYER_START_Y, EXTERIOR_X(34), EXTERIOR_Y(78)),
            "airlock exit should reconnect into the western forest route");
    Require(strcmp(Map_GetLocationNameAt(EXTERIOR_X(53), EXTERIOR_Y(74)), "Echo Basin") == 0,
            "echo basin should now occupy a broad west-line basin instead of a thin edge pocket");
    Require(strcmp(Map_GetLocationNameAt(EXTERIOR_X(108), EXTERIOR_Y(98)), "Purifier Ring") == 0,
            "purifier ring should claim a slightly wider south-deck footprint after the late-map polish");
    for (offsetY = 0; offsetY < STATION_FOOTPRINT_HEIGHT; offsetY++) {
        for (offsetX = 0; offsetX < STATION_FOOTPRINT_WIDTH; offsetX++) {
            Require(Map_GetPropTileAt(&map, WORKBENCH_X + offsetX, WORKBENCH_Y + offsetY) == TILE_WORKBENCH,
                    "workbench footprint should remain intact");
            Require(Map_GetPropTileAt(&map, OXYGEN_CONSOLE_X + offsetX, OXYGEN_CONSOLE_Y + offsetY) == TILE_OXYGEN_CONSOLE,
                    "oxygen console footprint should remain intact");
            Require(Map_GetPropTileAt(&map, LOXI_TERMINAL_X + offsetX, LOXI_TERMINAL_Y + offsetY) == TILE_LOXI_TERMINAL,
                    "Loxi terminal footprint should remain intact");
            Require(Map_GetPropTileAt(&map, ENERGY_CONSOLE_X + offsetX, ENERGY_CONSOLE_Y + offsetY) == TILE_ENERGY_CONSOLE,
                    "energy console footprint should remain intact");
            Require(Map_GetPropTileAt(&map, AIRLOCK_CONSOLE_X + offsetX, AIRLOCK_CONSOLE_Y + offsetY) == TILE_AIRLOCK_CONSOLE,
                    "airlock console footprint should remain centered and intact");
        }
    }

    for (offsetY = 0; offsetY < AIRLOCK_DOOR_HEIGHT; offsetY++) {
        Require(Map_GetPropTileAt(&map, AIRLOCK_DOOR_X, AIRLOCK_DOOR_TOP_Y + offsetY) == TILE_VOID,
                "unlocking the airlock should clear the door footprint across its full height");
        Require(Map_GetGroundTileAt(&map, AIRLOCK_DOOR_X + AIRLOCK_EXIT_WIDTH, AIRLOCK_DOOR_TOP_Y + offsetY) != TILE_VOID,
                "right-side breach should keep a full-height exterior route");
        for (offsetX = 1; offsetX <= AIRLOCK_EXIT_WIDTH; offsetX++) {
            Require(Map_GetPropTileAt(&map, AIRLOCK_DOOR_X + offsetX, AIRLOCK_DOOR_TOP_Y + offsetY) == TILE_VOID,
                    "right-side breach should not be blocked across the full five-tile opening");
        }
    }

    for (offsetY = 0; offsetY < WORLD_INTERACTIVE_FOOTPRINT_SIZE; offsetY++) {
        for (offsetX = 0; offsetX < WORLD_INTERACTIVE_FOOTPRINT_SIZE; offsetX++) {
            Require(Map_GetPropTileAt(&map, COMM_RELAY_X + offsetX, COMM_RELAY_Y + offsetY) == TILE_COMM_RELAY,
                    "comm relay footprint should remain square and intact");
            Require(Map_GetPropTileAt(&map, SIGNAL_TOWER_X + offsetX, SIGNAL_TOWER_Y + offsetY) == TILE_SIGNAL_TOWER,
                    "signal tower footprint should remain square and intact");
            Require(Map_GetPropTileAt(&map, MONOLITH_A_X + offsetX, MONOLITH_A_Y + offsetY) == TILE_MONOLITH,
                    "monolith A footprint should remain square and intact");
            Require(Map_GetPropTileAt(&map, MONOLITH_B_X + offsetX, MONOLITH_B_Y + offsetY) == TILE_MONOLITH,
                    "monolith B footprint should remain square and intact");
            Require(Map_GetPropTileAt(&map, MONOLITH_C_X + offsetX, MONOLITH_C_Y + offsetY) == TILE_MONOLITH,
                    "monolith C footprint should remain square and intact");
        }
    }

    for (offsetY = 0; offsetY < CRASH_CLUE_FOOTPRINT_SIZE; offsetY++) {
        for (offsetX = 0; offsetX < CRASH_CLUE_FOOTPRINT_SIZE; offsetX++) {
            Require(Map_GetPropTileAt(&map, CRASH_CLUE_X + offsetX, CRASH_CLUE_Y + offsetY) == TILE_CRASH_CLUE,
                    "crash clue footprint should remain square and intact");
        }
    }

    Require(Map_GetGroundTileAt(&map, EXTERIOR_X(64), EXTERIOR_Y(27)) == TILE_RUINS_GROUND,
            "north ruins approach should stay on ruins ground");
    Require(Map_GetGroundTileAt(&map, EXTERIOR_X(64), EXTERIOR_Y(16)) == TILE_RUINS_GROUND,
            "monolith ring should stay on ruins ground");
    Require(Map_GetGroundTileAt(&map, EXTERIOR_X(64), EXTERIOR_Y(10)) == TILE_RUINS_GROUND,
            "tower plateau should stay on ruins ground");
    Require(Map_IsWalkable(&map, EXTERIOR_X(64), EXTERIOR_Y(27)),
            "ruins approach centerline should remain walkable");
    Require(Map_IsWalkable(&map, EXTERIOR_X(64), EXTERIOR_Y(17)),
            "monolith ring centerline should remain walkable");
    Require(Map_IsWalkable(&map, SIGNAL_TOWER_X - 1, SIGNAL_TOWER_Y),
            "tower ascent centerline should remain walkable");
    Require(Map_GetGroundTileAt(&map, EXTERIOR_X(98), EXTERIOR_Y(48)) == TILE_SWAMP_GROUND,
            "outer swamp rim should stay on swamp ground");
    Require(Map_GetGroundTileAt(&map, EXTERIOR_X(106), EXTERIOR_Y(70)) == TILE_SWAMP_GROUND,
            "flooded detour should stay on outer-swamp ground");
    Require(Map_GetGroundTileAt(&map, EXTERIOR_X(112), EXTERIOR_Y(56)) == TILE_DEEP_SWAMP_GROUND,
            "deep gate shelf should stay on deep-swamp ground");
    Require(Map_GetGroundTileAt(&map, EXTERIOR_X(114), EXTERIOR_Y(72)) == TILE_DEEP_SWAMP_GROUND,
            "deep basin should stay on deep-swamp ground");
    Require(Map_IsWalkable(&map, EXTERIOR_X(98), EXTERIOR_Y(48)),
            "outer swamp rim teaching shelf should remain walkable");
    Require(Map_IsWalkable(&map, EXTERIOR_X(106), EXTERIOR_Y(70)),
            "flooded detour route should remain walkable");
    Require(Map_IsWalkable(&map, EXTERIOR_X(111), EXTERIOR_Y(56)),
            "deep gate objective shelf should remain walkable");
    Require(Map_IsWalkable(&map, EXTERIOR_X(114), EXTERIOR_Y(72)),
            "deep basin route should remain walkable");
    Require(Map_GetPropTileAt(&map, SWAMP_GATE_X, EXTERIOR_Y(52)) == TILE_VOID
                && Map_GetPropTileAt(&map, SWAMP_GATE_X + 1, EXTERIOR_Y(52)) == TILE_VOID,
            "deep-swamp gate should no longer use a decorative full-height barrier that can be bypassed");
    Require(Map_GetPropTileAt(&map, RUINS_GATE_X, EXTERIOR_Y(27)) == TILE_VOID
                && Map_GetPropTileAt(&map, RUINS_GATE_X + 1, EXTERIOR_Y(27)) == TILE_VOID,
            "ruins gate should no longer use a decorative full-height barrier that can be bypassed");
    Require(Map_GetPropTileAt(&map, ROPE_BARRIER_B_X, ROPE_BARRIER_B_Y) == TILE_BARRIER_SWAMP,
            "rope shortcut barrier should remain present before interaction");
    for (gridY = RUINS_MAIN_Y; gridY < RUINS_MAIN_Y + RUINS_MAIN_HEIGHT; ++gridY) {
        for (gridX = RUINS_MAIN_X; gridX < RUINS_MAIN_X + RUINS_MAIN_WIDTH; ++gridX) {
            TileType groundTile = Map_GetGroundTileAt(&map, gridX, gridY);
            Require(groundTile != TILE_SWAMP_GROUND && groundTile != TILE_DEEP_SWAMP_GROUND,
                    "ruins mainline should not contain swamp ground contamination");
        }
    }
    for (gridY = RUINS_APPROACH_Y; gridY < RUINS_APPROACH_Y + RUINS_APPROACH_HEIGHT; ++gridY) {
        for (gridX = RUINS_APPROACH_X; gridX < RUINS_APPROACH_X + RUINS_APPROACH_WIDTH; ++gridX) {
            TileType groundTile = Map_GetGroundTileAt(&map, gridX, gridY);
            Require(groundTile != TILE_SWAMP_GROUND && groundTile != TILE_DEEP_SWAMP_GROUND,
                    "ruins approach should not contain swamp ground contamination");
        }
    }

    Require(CountPropTiles(&map, TILE_STORAGE_LOCKER) == SHIP_FURNITURE_FOOTPRINT_SIZE * SHIP_FURNITURE_FOOTPRINT_SIZE,
            "cargo hold should keep exactly one storage-locker focal prop");
    Require(CountPropTiles(&map, TILE_BUNK) == SHIP_FURNITURE_FOOTPRINT_SIZE * SHIP_FURNITURE_FOOTPRINT_SIZE,
            "crew quarters should keep exactly one bunk focal prop");
    Require(CountPropTiles(&map, TILE_TECH_TABLE) == SHIP_FURNITURE_FOOTPRINT_SIZE * SHIP_FURNITURE_FOOTPRINT_SIZE,
            "diagnostics should keep exactly one tech-table focal prop");
    Require(CountPropTiles(&map, TILE_LOG_SITE) == 0,
            "legacy archive marker props should be removed so only real collectible logs remain");

    puts("map_layout smoke ok");
    return 0;
}
