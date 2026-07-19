#include "map_layout_internal.h"

static const int kBunkOrigins[][2] = {
    {45, 44}
};

static const int kStorageLockerOrigins[][2] = {
    {42, 51}
};

static const int kTechTableOrigins[][2] = {
    {45, 58}
};

static void FillPropRectsFromOrigins(GameMap *map,
                                     const int (*origins)[2],
                                     int originCount,
                                     int size,
                                     TileType tile) {
    int index;

    for (index = 0; index < originCount; index++) {
        MapInternal_FillPropRect(map, origins[index][0], origins[index][1], size, size, tile);
    }
}

static void FillVerticalConnector(GameMap *map, int gridX, int startY, int endY) {
    int topY;
    int height;

    if (startY > endY) {
        topY = endY;
        height = startY - endY + 1;
    } else {
        topY = startY;
        height = endY - startY + 1;
    }

    MapInternal_FillGroundRect(map, gridX, topY, SHIP_PASSAGE_WIDTH, height, TILE_BASE_FLOOR);
}

void MapInternal_SeedShipLayout(GameMap *map) {
    MapInternal_FillGroundRect(map, SHIP_CORRIDOR_X, SHIP_CORRIDOR_Y, SHIP_CORRIDOR_WIDTH, SHIP_CORRIDOR_HEIGHT, TILE_BASE_FLOOR);
    MapInternal_FillGroundRect(map, SHIP_CARGO_HOLD_X, SHIP_CARGO_HOLD_Y, SHIP_CARGO_HOLD_WIDTH, SHIP_CARGO_HOLD_HEIGHT, TILE_BASE_FLOOR);
    MapInternal_FillGroundRect(map, SHIP_CREW_QUARTERS_X, SHIP_CREW_QUARTERS_Y, SHIP_CREW_QUARTERS_WIDTH, SHIP_CREW_QUARTERS_HEIGHT, TILE_BASE_FLOOR);
    MapInternal_FillGroundRect(map, SHIP_DIAGNOSTICS_X, SHIP_DIAGNOSTICS_Y, SHIP_DIAGNOSTICS_WIDTH, SHIP_DIAGNOSTICS_HEIGHT, TILE_BASE_FLOOR);
    MapInternal_FillGroundRect(map, SHIP_TERMINAL_BAY_X, SHIP_TERMINAL_BAY_Y, SHIP_TERMINAL_BAY_WIDTH, SHIP_TERMINAL_BAY_HEIGHT, TILE_BASE_FLOOR);
    MapInternal_FillGroundRect(map, SHIP_LIFE_SUPPORT_X, SHIP_LIFE_SUPPORT_Y, SHIP_LIFE_SUPPORT_WIDTH, SHIP_LIFE_SUPPORT_HEIGHT, TILE_BASE_FLOOR);
    MapInternal_FillGroundRect(map, SHIP_WORKSHOP_X, SHIP_WORKSHOP_Y, SHIP_WORKSHOP_WIDTH, SHIP_WORKSHOP_HEIGHT, TILE_BASE_FLOOR);
    MapInternal_FillGroundRect(map, SHIP_POWER_BAY_X, SHIP_POWER_BAY_Y, SHIP_POWER_BAY_WIDTH, SHIP_POWER_BAY_HEIGHT, TILE_BASE_FLOOR);
    MapInternal_FillGroundRect(map, SHIP_AIRLOCK_LINK_X, SHIP_AIRLOCK_LINK_Y, SHIP_AIRLOCK_LINK_WIDTH, SHIP_AIRLOCK_LINK_HEIGHT, TILE_BASE_FLOOR);
    MapInternal_FillGroundRect(map,
                               AIRLOCK_CONSOLE_X - 1,
                               AIRLOCK_CONSOLE_Y,
                               STATION_FOOTPRINT_WIDTH + 1,
                               STATION_FOOTPRINT_HEIGHT,
                               TILE_BASE_FLOOR);
    FillVerticalConnector(map, SHIP_CREW_QUARTERS_X + 1, SHIP_CREW_QUARTERS_Y + SHIP_CREW_QUARTERS_HEIGHT, SHIP_CORRIDOR_Y - 1);
    FillVerticalConnector(map, SHIP_TERMINAL_BAY_X + 1, SHIP_TERMINAL_BAY_Y + SHIP_TERMINAL_BAY_HEIGHT, SHIP_CORRIDOR_Y - 1);
    FillVerticalConnector(map, SHIP_WORKSHOP_X + 1, SHIP_WORKSHOP_Y + SHIP_WORKSHOP_HEIGHT, SHIP_CORRIDOR_Y - 1);
    FillVerticalConnector(map, SHIP_DIAGNOSTICS_X + 1, SHIP_CORRIDOR_Y + SHIP_CORRIDOR_HEIGHT, SHIP_DIAGNOSTICS_Y - 1);
    FillVerticalConnector(map, SHIP_LIFE_SUPPORT_X + 1, SHIP_CORRIDOR_Y + SHIP_CORRIDOR_HEIGHT, SHIP_LIFE_SUPPORT_Y - 1);
    FillVerticalConnector(map, SHIP_POWER_BAY_X + 1, SHIP_CORRIDOR_Y + SHIP_CORRIDOR_HEIGHT, SHIP_POWER_BAY_Y - 1);
    MapInternal_FillPropRect(map, OXYGEN_CONSOLE_X, OXYGEN_CONSOLE_Y, STATION_FOOTPRINT_WIDTH, STATION_FOOTPRINT_HEIGHT, TILE_OXYGEN_CONSOLE);
    MapInternal_FillPropRect(map, LOXI_TERMINAL_X, LOXI_TERMINAL_Y, STATION_FOOTPRINT_WIDTH, STATION_FOOTPRINT_HEIGHT, TILE_LOXI_TERMINAL);
    MapInternal_FillPropRect(map, WORKBENCH_X, WORKBENCH_Y, STATION_FOOTPRINT_WIDTH, STATION_FOOTPRINT_HEIGHT, TILE_WORKBENCH);
    MapInternal_FillPropRect(map, AIRLOCK_CONSOLE_X, AIRLOCK_CONSOLE_Y, STATION_FOOTPRINT_WIDTH, STATION_FOOTPRINT_HEIGHT, TILE_AIRLOCK_CONSOLE);
    MapInternal_FillPropRect(map, ENERGY_CONSOLE_X, ENERGY_CONSOLE_Y, STATION_FOOTPRINT_WIDTH, STATION_FOOTPRINT_HEIGHT, TILE_ENERGY_CONSOLE);

    MapInternal_FillPropRect(map, LOXI_ROOM_DOOR_X, LOXI_ROOM_DOOR_Y, LOXI_ROOM_DOOR_WIDTH, 1, TILE_LOXI_ROOM_DOOR);

    MapInternal_FillGroundRect(map, AIRLOCK_DOOR_X, AIRLOCK_DOOR_TOP_Y, 1, AIRLOCK_DOOR_HEIGHT, TILE_BASE_FLOOR);
    MapInternal_FillPropRect(map, AIRLOCK_DOOR_X, AIRLOCK_DOOR_TOP_Y, 1, AIRLOCK_DOOR_HEIGHT, TILE_AIRLOCK_DOOR);

    FillPropRectsFromOrigins(map,
                             kBunkOrigins,
                             (int)(sizeof(kBunkOrigins) / sizeof(kBunkOrigins[0])),
                             SHIP_FURNITURE_FOOTPRINT_SIZE,
                             TILE_BUNK);
    FillPropRectsFromOrigins(map,
                             kStorageLockerOrigins,
                             (int)(sizeof(kStorageLockerOrigins) / sizeof(kStorageLockerOrigins[0])),
                             SHIP_FURNITURE_FOOTPRINT_SIZE,
                             TILE_STORAGE_LOCKER);
    FillPropRectsFromOrigins(map,
                             kTechTableOrigins,
                             (int)(sizeof(kTechTableOrigins) / sizeof(kTechTableOrigins[0])),
                             SHIP_FURNITURE_FOOTPRINT_SIZE,
                             TILE_TECH_TABLE);
}
