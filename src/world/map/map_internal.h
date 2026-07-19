#ifndef MAP_INTERNAL_H
#define MAP_INTERNAL_H

#include "map.h"

/* Small geometry helpers shared by map runtime slices. Keep subsystem
 * declarations in their dedicated internal headers instead of growing this
 * file into a second public map API.
 */
static inline bool MapInternal_IsRectBounds(int gridX,
                                            int gridY,
                                            int x,
                                            int y,
                                            int width,
                                            int height) {
    return width > 0
        && height > 0
        && gridX >= x
        && gridX < x + width
        && gridY >= y
        && gridY < y + height;
}

#endif
