#include "minimap.h"

#include "raylib.h"

#include <math.h>
#include <string.h>

void MiniMap_Init(MiniMap *minimap) {
    if (!minimap) return;
    
    memset(minimap->explored, 0, sizeof(minimap->explored));
    minimap->isVisible = false;
    minimap->displaySize = MINIMAP_SIZE;
    minimap->rotation = 0.0f;
}

void MiniMap_Update(MiniMap *minimap, const Player *player, const GameMap *map) {
    (void)map;
    int playerRadius;
    int dx;
    int dy;
    int checkX;
    int checkY;
    
    if (!minimap || !player) return;
    
    playerRadius = 6;
    
    for (dx = -playerRadius; dx <= playerRadius; dx++) {
        for (dy = -playerRadius; dy <= playerRadius; dy++) {
            checkX = player->gridX + dx;
            checkY = player->gridY + dy;
            
            if (Map_IsWithinBounds(checkX, checkY)) {
                minimap->explored[checkY][checkX] = true;
            }
        }
    }
    
    minimap->rotation += 0.02f;
}

void MiniMap_Draw(const MiniMap *minimap, const Player *player, const GameMap *map, int screenWidth, int screenHeight) {
    int posX;
    int posY;
    int x;
    int y;
    Color tileColor;
    Color playerColor;
    (void)map;
    (void)screenHeight;
    
    if (!minimap || !player) return;
    if (!minimap->isVisible) return;
    
    posX = screenWidth - MINIMAP_PADDING - MINIMAP_SIZE;
    posY = MINIMAP_PADDING;
    
    DrawRectangle(posX - 2, posY - 2, MINIMAP_SIZE + 4, MINIMAP_SIZE + 4, (Color){0, 0, 0, 180});
    
    for (y = 0; y < MAP_HEIGHT; y++) {
        for (x = 0; x < MAP_WIDTH; x++) {
            Rectangle rect;
            
            if (!minimap->explored[y][x]) {
                continue;
            }
            
            rect.x = posX + x * MINIMAP_TILE_SIZE;
            rect.y = posY + y * MINIMAP_TILE_SIZE;
            rect.width = MINIMAP_TILE_SIZE;
            rect.height = MINIMAP_TILE_SIZE;
            
            tileColor = (Color){100, 100, 100, 200};
            
            if (x == player->gridX && y == player->gridY) {
                playerColor = (Color){0, 255, 0, 255};
                DrawRectangleRec(rect, playerColor);
            } else {
                DrawRectangleRec(rect, tileColor);
            }
        }
    }
    
    DrawRectangleLines(posX - 2, posY - 2, MINIMAP_SIZE + 4, MINIMAP_SIZE + 4, (Color){200, 200, 200, 255});
}

void MiniMap_Toggle(MiniMap *minimap) {
    if (!minimap) return;
    
    minimap->isVisible = !minimap->isVisible;
}

bool MiniMap_IsVisible(const MiniMap *minimap) {
    if (!minimap) return false;
    
    return minimap->isVisible;
}
