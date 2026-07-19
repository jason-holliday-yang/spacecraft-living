#include "map_render_internal.h"

/* Visual-only compatibility bounds for the old generated single-map fixture.
 * Production maps use Tiled ground/region data; these coordinates do not
 * participate in loading, traversal, anchors, or unlock behavior.
 */
bool MapInternal_IsShipRoomBounds(int gridX, int gridY) {
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

bool MapInternal_IsCrashSiteVoidBounds(int gridX, int gridY) {
    enum {
        LEGACY_CRASH_VOID_X = 39,
        LEGACY_CRASH_VOID_Y = 42,
        LEGACY_CRASH_VOID_WIDTH = 45,
        LEGACY_CRASH_VOID_HEIGHT = 21
    };

    return gridX >= LEGACY_CRASH_VOID_X
        && gridX < LEGACY_CRASH_VOID_X + LEGACY_CRASH_VOID_WIDTH
        && gridY >= LEGACY_CRASH_VOID_Y
        && gridY < LEGACY_CRASH_VOID_Y + LEGACY_CRASH_VOID_HEIGHT;
}
