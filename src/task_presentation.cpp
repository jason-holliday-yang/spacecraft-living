#include "task_system.h"

#include "localization.h"
#include "recipe_catalog.h"
#include "task_runtime_internal.h"

#include <cmath>
#include <cstdio>

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
        case RESOURCE_RECOVERY_RATION:
            return &assets->statusOxygenReserve;
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

static int GetBossPhaseForPresentation(const Monster *boss) {
    const float healthPercent = boss != NULL && boss->maxHealth > 0.0f ? boss->health / boss->maxHealth : 1.0f;

    if (healthPercent > 0.70f) {
        return 1;
    }
    if (healthPercent > 0.35f) {
        return 2;
    }
    return 3;
}

static float GetMonsterHealthRatio(const Monster *monster) {
    float ratio;

    if (monster == NULL || monster->maxHealth <= 0.0f) {
        return 0.0f;
    }

    ratio = monster->health / monster->maxHealth;
    if (ratio < 0.0f) {
        return 0.0f;
    }
    if (ratio > 1.0f) {
        return 1.0f;
    }
    return ratio;
}

static void DrawMonsterHealthBar(Rectangle rect, const Monster *monster, float widthRatio, Color fillColor) {
    const float healthRatio = GetMonsterHealthRatio(monster);
    const float barWidth = rect.width * widthRatio;
    const float barHeight = monster != NULL && monster->type == MONSTER_FINAL_BOSS ? 6.0f : 5.0f;
    const float barX = rect.x + (rect.width - barWidth) * 0.5f;
    const float barY = monster != NULL && monster->type == MONSTER_FINAL_BOSS ? rect.y - 10.0f : rect.y - 8.0f;

    DrawRectangle((int)barX,
                  (int)barY,
                  (int)barWidth,
                  (int)barHeight,
                  Color{33, 20, 22, 230});
    DrawRectangle((int)barX,
                  (int)barY,
                  (int)(barWidth * healthRatio),
                  (int)barHeight,
                  fillColor);
    DrawRectangleLines((int)barX,
                       (int)barY,
                       (int)barWidth,
                       (int)barHeight,
                       Color{255, 245, 220, 80});
}

static const char *GetBossAttackLabel(BossAttackType attack) {
    switch (attack) {
        case BOSS_ATTACK_MELEE:
            return Loc_PickLiteral("Swipe", "挥击");
        case BOSS_ATTACK_CHARGE:
            return Loc_PickLiteral("Line Lock", "直线锁定");
        case BOSS_ATTACK_SPAWN:
        case BOSS_ATTACK_AOE:
        case BOSS_ATTACK_NONE:
        default:
            return "";
    }
}

static bool IsBossLineTelegraphHorizontal(const Monster *monster) {
    int bossCenterX;
    int bossCenterY;
    int deltaX;
    int deltaY;

    if (monster == NULL) {
        return true;
    }

    bossCenterX = monster->gridX + MONSTER_FOOTPRINT_SIZE / 2;
    bossCenterY = monster->gridY + MONSTER_FOOTPRINT_SIZE / 2;
    deltaX = monster->targetX - bossCenterX;
    deltaY = monster->targetY - bossCenterY;
    if (deltaX < 0) {
        deltaX = -deltaX;
    }
    if (deltaY < 0) {
        deltaY = -deltaY;
    }
    return deltaX >= deltaY;
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
        {
            const int phase = GetBossPhaseForPresentation(monster);
            const char *phaseLabel = phase == 1
                ? Loc_PickLiteral("Phase 1", "一阶段")
                : (phase == 2 ? Loc_PickLiteral("Phase 2", "二阶段") : Loc_PickLiteral("Final Phase", "最终阶段"));
            const char *attackLabel = GetBossAttackLabel(monster->currentAttack);
            Color phaseColor = phase == 1
                ? Color{255, 184, 108, 255}
                : (phase == 2 ? Color{112, 223, 255, 255} : Color{255, 128, 118, 255});

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
            DrawMonsterHealthBar(rect,
                                 monster,
                                 0.70f,
                                 Color{245, 94, 81, 255});
            DrawRectangleRounded(Rectangle{rect.x + rect.width * 0.08f, rect.y - 30.0f, 74.0f, 16.0f}, 0.35f, 6, Fade(phaseColor, 0.18f));
            DrawText(phaseLabel, (int)(rect.x + rect.width * 0.10f), (int)(rect.y - 31.0f), 13, phaseColor);
            if (monster->attackTelegraph > 0.0f && attackLabel[0] != '\0') {
                DrawText(attackLabel, (int)(rect.x + rect.width * 0.61f), (int)(rect.y - 31.0f), 13, Color{255, 234, 182, 255});
            }
            return;
        }
        default:
            break;
    }

    if (texture != NULL && texture->loaded) {
        DrawTextureAssetCentered(texture, pos, rect.width, rect.height, WHITE);
        DrawCircle((int)pos.x,
                   (int)(pos.y - radius * 0.6f),
                   radius * 0.55f + sinf(elapsedSeconds * 4.0f) * 1.2f,
                   Color{255, 255, 255, 28});
        DrawMonsterHealthBar(rect, monster, 0.62f, Color{237, 96, 78, 255});
        return;
    }

    DrawCircleV(pos, radius, body);
    DrawCircle((int)pos.x,
               (int)(pos.y - radius * 0.6f),
               radius * 0.55f + sinf(elapsedSeconds * 4.0f) * 1.2f,
               Color{255, 255, 255, 40});
    DrawMonsterHealthBar(rect, monster, 0.62f, Color{237, 96, 78, 255});
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
            Rectangle lineRect;
            Color lineColor;

            lineColor = Color{255, 184, 108, (unsigned char)(70 + pulse * 72.0f)};
            if (IsBossLineTelegraphHorizontal(monster)) {
                const int lockedY = monster->targetY < BOSS_ARENA_Y + 1
                    ? BOSS_ARENA_Y + 1
                    : (monster->targetY > BOSS_ARENA_Y + BOSS_ARENA_HEIGHT - 2
                        ? BOSS_ARENA_Y + BOSS_ARENA_HEIGHT - 2
                        : monster->targetY);

                lineRect = Rectangle{
                    (float)((BOSS_ARENA_X + 1) * TILE_SIZE),
                    (float)(lockedY * TILE_SIZE + TILE_SIZE / 2 - 5),
                    (float)((BOSS_ARENA_WIDTH - 2) * TILE_SIZE),
                    10.0f + pulse * 4.0f
                };
            } else {
                const int lockedX = monster->targetX < BOSS_ARENA_X + 1
                    ? BOSS_ARENA_X + 1
                    : (monster->targetX > BOSS_ARENA_X + BOSS_ARENA_WIDTH - 2
                        ? BOSS_ARENA_X + BOSS_ARENA_WIDTH - 2
                        : monster->targetX);

                lineRect = Rectangle{
                    (float)(lockedX * TILE_SIZE + TILE_SIZE / 2 - 5),
                    (float)((BOSS_ARENA_Y + 1) * TILE_SIZE),
                    10.0f + pulse * 4.0f,
                    (float)((BOSS_ARENA_HEIGHT - 2) * TILE_SIZE)
                };
            }
            DrawRectangleRounded(lineRect, 0.35f, 6, lineColor);
            break;
        }
        case BOSS_ATTACK_SPAWN:
        case BOSS_ATTACK_AOE:
            break;
        case BOSS_ATTACK_NONE:
        default:
            break;
    }
}

static void DrawMonsterCombatTelegraph(const Monster *monster, float elapsedSeconds) {
    Rectangle rect;
    Vector2 pos;
    float pulse;
    int originX;
    int originY;
    int width;
    int height;
    Color color;

    if (monster == NULL || monster->attackTelegraph <= 0.0f) {
        return;
    }
    if (monster->type == MONSTER_FINAL_BOSS) {
        DrawBossTelegraph(monster, elapsedSeconds);
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
    pulse = sinf(elapsedSeconds * 9.0f) * 0.5f + 0.5f;
    color = Color{255, 158, 118, (unsigned char)(90 + pulse * 60.0f)};

    switch (monster->type) {
        case MONSTER_SWAMP_STALKER:
        case MONSTER_FOG_WORM:
            color = Color{136, 226, 120, (unsigned char)(92 + pulse * 58.0f)};
            break;
        case MONSTER_SENTINEL_JELLY:
        case MONSTER_WING_BUG:
            color = Color{118, 214, 255, (unsigned char)(92 + pulse * 58.0f)};
            break;
        case MONSTER_RELIC_GUARD:
            color = Color{255, 212, 150, (unsigned char)(95 + pulse * 62.0f)};
            break;
        case MONSTER_THORN_LARVA:
        case MONSTER_RAPTOR:
        default:
            break;
    }

    DrawRing(pos,
             rect.width * 0.34f,
             rect.width * 0.50f + pulse * 3.0f,
             0.0f,
             360.0f,
             32,
             color);
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
            DrawMonsterCombatTelegraph(monster, elapsedSeconds);
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
