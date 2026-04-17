#include "task_system.h"

#include "recipe_catalog.h"
#include "task_runtime_internal.h"

#include <cmath>

static void DrawTextureAssetCentered(const TextureAsset *asset, Vector2 center, float maxWidth, float maxHeight, Color tint) {
    Rectangle source;
    Rectangle dest;
    float scale;
    float drawWidth;
    float drawHeight;

    if (asset == NULL || !asset->loaded || asset->texture.width <= 0 || asset->texture.height <= 0) {
        return;
    }

    scale = fminf(maxWidth / (float)asset->texture.width, maxHeight / (float)asset->texture.height);
    drawWidth = (float)asset->texture.width * scale;
    drawHeight = (float)asset->texture.height * scale;
    source = Rectangle{0.0f, 0.0f, (float)asset->texture.width, (float)asset->texture.height};
    dest = Rectangle{
        center.x - drawWidth * 0.5f,
        center.y - drawHeight * 0.5f,
        drawWidth,
        drawHeight
    };
    DrawTexturePro(asset->texture, source, dest, Vector2{0.0f, 0.0f}, 0.0f, tint);
}

static const TextureAsset *GetNodeTexture(const AssetBundle *assets, ResourceType type) {
    switch (type) {
        case RESOURCE_WOOD:
            return &assets->nodeWood;
        case RESOURCE_ORE:
            return &assets->nodeOre;
        case RESOURCE_METAL_SCRAP:
            return &assets->nodeMetalScrap;
        case RESOURCE_FRUIT:
            return &assets->nodeFruit;
        case RESOURCE_SPECIAL_FUNGUS:
            return &assets->nodeSpecialFungus;
        case RESOURCE_ENERGY_CORE:
            return &assets->nodeEnergyCore;
        case RESOURCE_GLOW_MOSS:
            return &assets->nodeGlowMoss;
        case RESOURCE_ALIEN_VINE:
            return &assets->nodeAlienVine;
        case RESOURCE_SHELL_FRUIT:
            return &assets->nodeShellFruit;
        case RESOURCE_JUNK_METAL:
            return &assets->nodeJunkMetal;
        case RESOURCE_ENERGY_CRYSTAL:
            return &assets->nodeEnergyCrystal;
        case RESOURCE_CALM_MUSHROOM:
            return &assets->nodeCalmMushroom;
        case RESOURCE_PROTECTIVE_FIBER:
            return &assets->nodeProtectiveFiber;
        case RESOURCE_RELIC_FRAGMENT:
            return &assets->nodeRelicFragment;
        case RESOURCE_BOSS_SCALE:
            return &assets->nodeBossScale;
        case RESOURCE_ALIEN_SLIME:
            return &assets->nodeAlienSlime;
        case RESOURCE_COUNT:
        default:
            return NULL;
    }
}

static void DrawResourceNode(const ResourceNode *node, const AssetBundle *assets, float elapsedSeconds) {
    Rectangle rect;
    Color glow;
    const TextureAsset *texture;

    rect = Map_GridToRect(node->gridX, node->gridY);
    glow = Color{190, 234, 255, (unsigned char)(90 + 40.0f * (sinf(elapsedSeconds * 4.0f) * 0.5f + 0.5f))};
    texture = GetNodeTexture(assets, node->type);

    if (texture != NULL && texture->loaded) {
        DrawTextureAssetCentered(texture,
                                 Vector2{rect.x + rect.width * 0.5f, rect.y + rect.height * 0.5f},
                                 rect.width * 0.9f,
                                 rect.height * 0.9f,
                                 WHITE);
        if (node->type == RESOURCE_ENERGY_CORE || node->type == RESOURCE_ENERGY_CRYSTAL) {
            DrawCircleLines((int)(rect.x + rect.width * 0.5f), (int)(rect.y + rect.height * 0.44f), 15.0f, glow);
        }
        return;
    }

    switch (node->type) {
        case RESOURCE_WOOD:
            DrawRectangle((int)rect.x + 24, (int)rect.y + 18, 16, 32, Color{113, 78, 49, 255});
            break;
        case RESOURCE_METAL_SCRAP:
        case RESOURCE_JUNK_METAL:
            DrawRectangle((int)rect.x + 18, (int)rect.y + 20, 28, 20, Color{145, 152, 164, 255});
            break;
        case RESOURCE_FRUIT:
        case RESOURCE_SHELL_FRUIT:
            DrawCircle((int)(rect.x + 32), (int)(rect.y + 30), 10.0f, Color{221, 173, 89, 255});
            break;
        case RESOURCE_GLOW_MOSS:
        case RESOURCE_CALM_MUSHROOM:
        case RESOURCE_SPECIAL_FUNGUS:
            DrawCircle((int)(rect.x + 32), (int)(rect.y + 32), 12.0f, Color{126, 255, 208, 170});
            break;
        case RESOURCE_ENERGY_CORE:
        case RESOURCE_ENERGY_CRYSTAL:
            DrawCircle((int)(rect.x + 32), (int)(rect.y + 28), 12.0f, Color{110, 214, 255, 190});
            DrawCircleLines((int)(rect.x + 32), (int)(rect.y + 28), 15.0f, glow);
            break;
        case RESOURCE_RELIC_FRAGMENT:
        case RESOURCE_ALIEN_SLIME:
            DrawTriangle(
                Vector2{rect.x + 22.0f, rect.y + 42.0f},
                Vector2{rect.x + 32.0f, rect.y + 18.0f},
                Vector2{rect.x + 44.0f, rect.y + 42.0f},
                Color{184, 194, 211, 255});
            break;
        default:
            DrawCircle((int)(rect.x + 32), (int)(rect.y + 32), 10.0f, glow);
            break;
    }
}

static void DrawShipLogPickup(const ShipLog *log, float elapsedSeconds) {
    Rectangle rect;
    Rectangle iconRect;
    Color body;
    Color outline;
    Color pageTint;
    Color accent;
    float pulse;
    float bob;
    float glowRadius;

    rect = Map_GridToRect(log->gridX, log->gridY);
    pulse = sinf(elapsedSeconds * 3.4f) * 0.5f + 0.5f;
    bob = sinf(elapsedSeconds * 2.3f + (float)(log->gridX + log->gridY) * 0.17f) * 1.5f;
    glowRadius = 12.0f + pulse * 4.0f;
    body = Color{42, 56, 76, 240};
    outline = Color{255, 212, 148, (unsigned char)(120 + pulse * 50.0f)};
    pageTint = Color{232, 238, 246, 250};
    accent = log->category == SHIP_LOG_MAINLINE
        ? Color{255, 194, 116, 235}
        : Color{118, 226, 255, 235};
    iconRect = Rectangle{
        rect.x + 18.0f,
        rect.y + 14.0f + bob,
        rect.width - 36.0f,
        rect.height - 26.0f
    };

    DrawCircle((int)(rect.x + rect.width * 0.5f),
               (int)(rect.y + rect.height * 0.48f + bob),
               glowRadius,
               Color{255, 212, 148, (unsigned char)(46 + pulse * 34.0f)});
    DrawRectangleRounded(iconRect, 0.18f, 5, body);
    DrawRectangleRoundedLinesEx(iconRect, 0.18f, 5, 2.0f, outline);
    DrawRectangleRounded(Rectangle{iconRect.x + 9.0f, iconRect.y + 8.0f, iconRect.width - 18.0f, iconRect.height - 16.0f},
                         0.12f,
                         4,
                         pageTint);
    DrawRectangle((int)iconRect.x + 12,
                  (int)iconRect.y + 12,
                  (int)iconRect.width - 24,
                  7,
                  accent);
    DrawRectangle((int)iconRect.x + 12,
                  (int)iconRect.y + 24,
                  (int)iconRect.width - 30,
                  5,
                  Color{126, 144, 166, 225});
    DrawRectangle((int)iconRect.x + 12,
                  (int)iconRect.y + 34,
                  (int)iconRect.width - 22,
                  5,
                  Color{126, 144, 166, 205});
    DrawRectangle((int)iconRect.x + 12,
                  (int)iconRect.y + 44,
                  (int)iconRect.width - 36,
                  5,
                  Color{126, 144, 166, 190});
    DrawCircle((int)(iconRect.x + iconRect.width - 10.0f),
               (int)(iconRect.y + 11.0f),
               3.5f + pulse * 1.2f,
               accent);
}

static const TextureAsset *GetMonsterTexture(const AssetBundle *assets, MonsterType type) {
    switch (type) {
        case MONSTER_THORN_LARVA:
            return &assets->mobThornLarva;
        case MONSTER_WING_BUG:
            return &assets->mobWingBug;
        case MONSTER_RAPTOR:
            return &assets->mobRaptor;
        case MONSTER_SWAMP_STALKER:
            return &assets->mobSwampStalker;
        case MONSTER_SENTINEL_JELLY:
            return &assets->mobSentinelJelly;
        case MONSTER_FOG_WORM:
            return &assets->mobFogWorm;
        case MONSTER_RELIC_GUARD:
            return &assets->mobRelicGuard;
        case MONSTER_FINAL_BOSS:
            return &assets->boss;
        default:
            return NULL;
    }
}

static void DrawMonster(const Monster *monster, const AssetBundle *assets, float elapsedSeconds, int bossDebuffStacks) {
    Rectangle rect;
    Vector2 pos;
    Color body;
    float radius;
    const TextureAsset *texture;
    int originX;
    int originY;
    int width;
    int height;

    TasksRuntime_GetMonsterFootprint(monster, &originX, &originY, &width, &height);
    rect = Rectangle{
        (float)(originX * TILE_SIZE),
        (float)(originY * TILE_SIZE),
        (float)(width * TILE_SIZE),
        (float)(height * TILE_SIZE)
    };
    pos = Vector2{rect.x + rect.width * 0.5f, rect.y + rect.height * 0.5f};
    body = Color{255, 138, 112, 255};
    radius = rect.width * 0.28f;
    texture = GetMonsterTexture(assets, monster->type);

    switch (monster->type) {
        case MONSTER_THORN_LARVA:
            body = Color{197, 110, 81, 255};
            radius = rect.width * 0.22f;
            break;
        case MONSTER_WING_BUG:
            body = Color{160, 217, 132, 255};
            radius = rect.width * 0.20f;
            break;
        case MONSTER_RAPTOR:
            body = Color{211, 121, 90, 255};
            radius = rect.width * 0.30f;
            break;
        case MONSTER_SWAMP_STALKER:
            body = Color{96, 150, 103, 255};
            radius = rect.width * 0.28f;
            break;
        case MONSTER_SENTINEL_JELLY:
            body = Color{101, 207, 242, 255};
            radius = rect.width * 0.30f;
            break;
        case MONSTER_FOG_WORM:
            body = Color{173, 214, 86, 255};
            radius = rect.width * 0.28f;
            break;
        case MONSTER_RELIC_GUARD:
            body = Color{173, 176, 194, 255};
            radius = rect.width * 0.32f;
            break;
        case MONSTER_FINAL_BOSS:
            if (texture != NULL && texture->loaded) {
                DrawTextureAssetCentered(texture, pos, rect.width, rect.height, WHITE);
            } else {
                body = Color{164, 94, 93, 255};
                radius = rect.width * 0.36f;
                DrawCircleV(pos, radius + sinf(elapsedSeconds * 3.0f), Color{255, 163, 131, 45});
            }
            if (bossDebuffStacks > 0) {
                DrawRing(pos,
                         rect.width * 0.42f,
                         rect.width * 0.52f,
                         0.0f,
                         360.0f,
                         32,
                         Color{107, 242, 255, (unsigned char)(50 + bossDebuffStacks * 20)});
            }
            if (!assets->boss.loaded) {
                DrawCircleV(pos, radius, body);
            }
            DrawRectangle((int)(rect.x + rect.width * 0.15f),
                          (int)(rect.y - 10.0f),
                          (int)(rect.width * 0.70f),
                          6,
                          Color{44, 22, 22, 255});
            DrawRectangle((int)(rect.x + rect.width * 0.15f),
                          (int)(rect.y - 10.0f),
                          (int)(rect.width * 0.70f * (monster->health / monster->maxHealth)),
                          6,
                          Color{245, 94, 81, 255});
            return;
        default:
            break;
    }

    if (texture != NULL && texture->loaded) {
        DrawTextureAssetCentered(texture, pos, rect.width, rect.height, WHITE);
        DrawCircle((int)pos.x,
                   (int)(pos.y - radius * 0.6f),
                   radius * 0.55f + sinf(elapsedSeconds * 4.0f) * 1.2f,
                   Color{255, 255, 255, 28});
        return;
    }

    DrawCircleV(pos, radius, body);
    DrawCircle((int)pos.x,
               (int)(pos.y - radius * 0.6f),
               radius * 0.55f + sinf(elapsedSeconds * 4.0f) * 1.2f,
               Color{255, 255, 255, 40});
}

static void DrawBossTelegraph(const Monster *monster, float elapsedSeconds) {
    Rectangle rect;
    Vector2 pos;
    float pulse;
    int originX;
    int originY;
    int width;
    int height;

    if (monster == NULL || monster->type != MONSTER_FINAL_BOSS || monster->attackTelegraph <= 0.0f) {
        return;
    }

    TasksRuntime_GetMonsterFootprint(monster, &originX, &originY, &width, &height);
    rect = Rectangle{
        (float)(originX * TILE_SIZE),
        (float)(originY * TILE_SIZE),
        (float)(width * TILE_SIZE),
        (float)(height * TILE_SIZE)
    };
    pos = Vector2{rect.x + rect.width * 0.5f, rect.y + rect.height * 0.5f};
    pulse = sinf(elapsedSeconds * 8.0f) * 0.5f + 0.5f;

    switch (monster->currentAttack) {
        case BOSS_ATTACK_MELEE:
            DrawRing(pos,
                     rect.width * 0.42f,
                     rect.width * 0.56f + pulse * 4.0f,
                     0.0f,
                     360.0f,
                     36,
                     Color{255, 132, 102, (unsigned char)(95 + pulse * 70.0f)});
            break;
        case BOSS_ATTACK_CHARGE: {
            Vector2 targetPos;

            targetPos = Map_GridToWorld(monster->targetX, monster->targetY);
            DrawLineEx(pos, targetPos, 8.0f + pulse * 3.0f, Color{255, 184, 108, (unsigned char)(95 + pulse * 70.0f)});
            DrawCircleV(targetPos, 12.0f + pulse * 4.0f, Color{255, 184, 108, (unsigned char)(60 + pulse * 45.0f)});
            break;
        }
        case BOSS_ATTACK_SPAWN:
            DrawRing(pos,
                     rect.width * 0.45f,
                     rect.width * 0.70f + pulse * 5.0f,
                     0.0f,
                     360.0f,
                     36,
                     Color{112, 223, 255, (unsigned char)(90 + pulse * 65.0f)});
            break;
        case BOSS_ATTACK_AOE:
            DrawRing(pos,
                     rect.width * 0.72f,
                     rect.width * 0.90f + pulse * 6.0f,
                     0.0f,
                     360.0f,
                     48,
                     Color{255, 208, 118, (unsigned char)(85 + pulse * 70.0f)});
            break;
        case BOSS_ATTACK_NONE:
        default:
            break;
    }
}

void Tasks_DrawWorld(const TaskSystem *tasks, const AssetBundle *assets, float elapsedSeconds) {
    int index;

    for (index = 0; index < tasks->nodeCount; index++) {
        const ResourceNode *node;

        node = &tasks->nodes[index];
        if (node->active) {
            DrawResourceNode(node, assets, elapsedSeconds);
        }
    }

    for (index = 0; index < tasks->logCount; index++) {
        const ShipLog *log;

        log = &tasks->logs[index];
        if (!log->active || log->collected) {
            continue;
        }

        DrawShipLogPickup(log, elapsedSeconds);
    }

    for (index = 0; index < tasks->monsterCount; index++) {
        const Monster *monster;

        monster = &tasks->monsters[index];
        if (monster->active && tasks->stage >= monster->unlockStage) {
            DrawBossTelegraph(monster, elapsedSeconds);
            DrawMonster(monster, assets, elapsedSeconds, tasks->monolithsLit);
        }
    }
}

int Tasks_GetCollectedLogCount(const TaskSystem *tasks) {
    int index;
    int count;

    count = 0;
    for (index = 0; index < tasks->logCount; index++) {
        if (tasks->logs[index].active && tasks->logs[index].collected) {
            count += 1;
        }
    }

    return count;
}

const ShipLog *Tasks_GetCollectedLogAt(const TaskSystem *tasks, int index) {
    int logIndex;
    int visibleIndex;

    visibleIndex = 0;
    for (logIndex = 0; logIndex < tasks->logCount; logIndex++) {
        const ShipLog *log;

        log = &tasks->logs[logIndex];
        if (!log->active || !log->collected) {
            continue;
        }

        if (visibleIndex == index) {
            return log;
        }
        visibleIndex += 1;
    }

    return NULL;
}

int Tasks_GetLogSceneIndex(const TaskSystem *tasks, const ShipLog *log) {
    int logIndex;

    if (tasks == NULL || log == NULL) {
        return -1;
    }

    for (logIndex = 0; logIndex < tasks->logCount; logIndex++) {
        if (&tasks->logs[logIndex] == log) {
            return logIndex;
        }
    }

    return -1;
}

int Tasks_GetVisibleRecipeCount(const TaskSystem *tasks) {
    int count;
    int itemIndex;

    if (tasks == NULL) {
        return 0;
    }

    count = 0;
    for (itemIndex = 0; itemIndex < RecipeCatalog_GetOrderedCount(); itemIndex++) {
        if (Tasks_IsRecipeVisible(tasks, RecipeCatalog_GetOrderedAt(itemIndex))) {
            count += 1;
        }
    }

    return count;
}

RecipeType Tasks_GetVisibleRecipeAt(const TaskSystem *tasks, int index) {
    int visibleIndex;
    int itemIndex;

    if (tasks == NULL || index < 0) {
        return RECIPE_GLOW_STICK;
    }

    visibleIndex = 0;
    for (itemIndex = 0; itemIndex < RecipeCatalog_GetOrderedCount(); itemIndex++) {
        RecipeType recipe;

        recipe = RecipeCatalog_GetOrderedAt(itemIndex);
        if (!Tasks_IsRecipeVisible(tasks, recipe)) {
            continue;
        }

        if (visibleIndex == index) {
            return recipe;
        }
        visibleIndex += 1;
    }

    return RECIPE_GLOW_STICK;
}
