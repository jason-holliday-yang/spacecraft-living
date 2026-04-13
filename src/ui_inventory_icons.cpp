#include "ui_inventory_internal.h"

#include "ui_runtime_internal.h"

static const TextureAsset *GetBackpackEntryTexture(const AssetBundle *assets, int itemIndex) {
    switch (itemIndex) {
        case 0:
            return &assets->nodeWood;
        case 1:
            return &assets->nodeOre;
        case 2:
            return &assets->nodeMetalScrap;
        case 3:
            return &assets->nodeFruit;
        case 4:
            return &assets->nodeSpecialFungus;
        case 5:
            return &assets->nodeEnergyCore;
        case 6:
            return &assets->nodeGlowMoss;
        case 7:
            return &assets->nodeAlienVine;
        case 8:
            return &assets->nodeShellFruit;
        case 9:
            return &assets->nodeJunkMetal;
        case 10:
            return &assets->nodeEnergyCrystal;
        case 11:
            return &assets->nodeCalmMushroom;
        case 12:
            return &assets->nodeProtectiveFiber;
        case 13:
            return &assets->nodeRelicFragment;
        case 14:
            return &assets->nodeBossScale;
        case 15:
            return &assets->nodeAlienSlime;
        case 16:
            return &assets->iconGlowStick;
        case 17:
            return &assets->iconRope;
        case 18:
            return &assets->iconLaserGun;
        case 19:
            return &assets->iconProtectionSuit;
        case 20:
            return &assets->iconSignalAmplifier;
        case 21:
            return &assets->iconFieldCamp;
        default:
            return NULL;
    }
}

void UIInventory_DrawBackpackIcon(const AssetBundle *assets, int itemIndex, Rectangle rect, Color primary, Color secondary) {
    Vector2 center;
    float unit;
    const TextureAsset *asset;

    asset = GetBackpackEntryTexture(assets, itemIndex);
    if (asset != NULL && asset->loaded) {
        UIRuntime_DrawTextureAssetFitted(asset, rect, WHITE);
        return;
    }

    center = Vector2{rect.x + rect.width * 0.5f, rect.y + rect.height * 0.44f};
    unit = rect.width < rect.height ? rect.width : rect.height;
    unit *= 0.16f;

    switch (itemIndex) {
        case 0:
            DrawRectangleRounded(Rectangle{center.x - unit * 0.55f, center.y - unit * 0.10f, unit * 1.1f, unit * 2.4f}, 0.20f, 6, secondary);
            DrawCircleV(Vector2{center.x, center.y - unit * 0.65f}, unit * 0.95f, primary);
            break;
        case 1:
            DrawTriangle(Vector2{center.x - unit * 1.1f, center.y + unit * 0.8f}, Vector2{center.x - unit * 0.3f, center.y - unit * 1.1f}, Vector2{center.x + unit * 0.4f, center.y + unit * 0.6f}, secondary);
            DrawTriangle(Vector2{center.x - unit * 0.1f, center.y + unit * 1.0f}, Vector2{center.x + unit * 0.8f, center.y - unit * 0.8f}, Vector2{center.x + unit * 1.2f, center.y + unit * 0.7f}, primary);
            break;
        case 2:
            DrawRectanglePro(Rectangle{center.x - unit * 0.95f, center.y - unit * 0.45f, unit * 1.3f, unit * 0.70f}, Vector2{0.0f, 0.0f}, -18.0f, primary);
            DrawRectanglePro(Rectangle{center.x - unit * 0.15f, center.y - unit * 0.10f, unit * 1.2f, unit * 0.75f}, Vector2{0.0f, 0.0f}, 18.0f, secondary);
            break;
        case 3:
            DrawCircleV(Vector2{center.x - unit * 0.45f, center.y}, unit * 0.78f, primary);
            DrawCircleV(Vector2{center.x + unit * 0.45f, center.y}, unit * 0.78f, primary);
            DrawTriangle(Vector2{center.x, center.y - unit * 0.8f}, Vector2{center.x + unit * 0.8f, center.y - unit * 1.2f}, Vector2{center.x + unit * 0.3f, center.y - unit * 0.2f}, secondary);
            break;
        case 4:
        case 11:
            DrawCircleV(Vector2{center.x, center.y - unit * 0.35f}, unit * 0.95f, primary);
            DrawRectangleRounded(Rectangle{center.x - unit * 0.32f, center.y + unit * 0.15f, unit * 0.64f, unit * 1.3f}, 0.3f, 6, secondary);
            break;
        case 5:
            DrawCircleLines((int)center.x, (int)center.y, unit * 1.15f, secondary);
            DrawCircleV(center, unit * 0.70f, primary);
            DrawCircleV(center, unit * 0.20f, secondary);
            break;
        case 6:
            DrawCircleV(Vector2{center.x - unit * 0.55f, center.y + unit * 0.10f}, unit * 0.55f, primary);
            DrawCircleV(Vector2{center.x, center.y - unit * 0.40f}, unit * 0.68f, primary);
            DrawCircleV(Vector2{center.x + unit * 0.60f, center.y + unit * 0.12f}, unit * 0.52f, primary);
            break;
        case 7:
            DrawLineEx(Vector2{center.x - unit * 1.0f, center.y + unit * 0.8f}, Vector2{center.x - unit * 0.2f, center.y - unit * 1.0f}, unit * 0.28f, primary);
            DrawLineEx(Vector2{center.x - unit * 0.2f, center.y - unit * 1.0f}, Vector2{center.x + unit * 0.9f, center.y + unit * 0.9f}, unit * 0.28f, primary);
            DrawLineEx(Vector2{center.x - unit * 0.1f, center.y - unit * 0.3f}, Vector2{center.x + unit * 0.8f, center.y - unit * 0.8f}, unit * 0.20f, secondary);
            break;
        case 8:
            DrawCircleV(center, unit * 1.0f, primary);
            DrawCircleV(center, unit * 0.72f, secondary);
            DrawLineEx(Vector2{center.x - unit * 0.65f, center.y}, Vector2{center.x + unit * 0.65f, center.y}, unit * 0.16f, primary);
            break;
        case 9:
            DrawCircleLines((int)center.x, (int)center.y, unit * 0.95f, primary);
            DrawRectangleRounded(Rectangle{center.x - unit * 0.28f, center.y - unit * 1.0f, unit * 0.56f, unit * 2.0f}, 0.20f, 6, secondary);
            DrawRectangleRounded(Rectangle{center.x - unit * 1.0f, center.y - unit * 0.28f, unit * 2.0f, unit * 0.56f}, 0.20f, 6, secondary);
            break;
        case 10:
        case 13:
        case 14:
            DrawTriangle(Vector2{center.x, center.y - unit * 1.2f}, Vector2{center.x - unit * 0.9f, center.y + unit * 0.3f}, Vector2{center.x + unit * 0.9f, center.y + unit * 0.3f}, primary);
            DrawTriangle(Vector2{center.x, center.y + unit * 1.1f}, Vector2{center.x - unit * 0.8f, center.y + unit * 0.1f}, Vector2{center.x + unit * 0.8f, center.y + unit * 0.1f}, secondary);
            break;
        case 12:
            DrawLineEx(Vector2{center.x - unit * 1.0f, center.y + unit * 0.8f}, Vector2{center.x - unit * 0.3f, center.y - unit * 0.9f}, unit * 0.16f, primary);
            DrawLineEx(Vector2{center.x - unit * 0.3f, center.y - unit * 0.9f}, Vector2{center.x + unit * 0.4f, center.y + unit * 0.8f}, unit * 0.16f, primary);
            DrawLineEx(Vector2{center.x + unit * 0.1f, center.y + unit * 0.8f}, Vector2{center.x + unit * 1.0f, center.y - unit * 0.6f}, unit * 0.16f, secondary);
            break;
        case 15:
            DrawEllipse((int)center.x, (int)center.y + (int)(unit * 0.2f), unit * 1.0f, unit * 0.72f, primary);
            DrawCircleV(Vector2{center.x - unit * 0.35f, center.y - unit * 0.20f}, unit * 0.40f, primary);
            DrawCircleV(Vector2{center.x + unit * 0.38f, center.y - unit * 0.18f}, unit * 0.38f, primary);
            DrawCircleV(Vector2{center.x, center.y - unit * 0.55f}, unit * 0.34f, secondary);
            break;
        case 16:
            DrawRectangleRounded(Rectangle{center.x - unit * 0.32f, center.y - unit * 1.1f, unit * 0.64f, unit * 2.2f}, 0.30f, 8, primary);
            DrawCircleV(Vector2{center.x, center.y - unit * 0.75f}, unit * 0.16f, secondary);
            break;
        case 17:
            DrawCircleLines((int)center.x, (int)center.y, unit * 0.95f, primary);
            DrawCircleLines((int)(center.x + unit * 0.55f), (int)(center.y + unit * 0.10f), unit * 0.72f, primary);
            break;
        case 18:
            DrawRectangleRounded(Rectangle{center.x - unit * 1.0f, center.y - unit * 0.32f, unit * 1.4f, unit * 0.64f}, 0.18f, 6, secondary);
            DrawRectangleRounded(Rectangle{center.x - unit * 0.1f, center.y - unit * 0.55f, unit * 0.9f, unit * 0.44f}, 0.12f, 6, primary);
            DrawCircleV(Vector2{center.x + unit * 0.95f, center.y}, unit * 0.28f, primary);
            break;
        case 19:
            DrawTriangle(Vector2{center.x, center.y - unit * 1.1f}, Vector2{center.x - unit * 0.9f, center.y + unit * 0.9f}, Vector2{center.x + unit * 0.9f, center.y + unit * 0.9f}, primary);
            DrawTriangle(Vector2{center.x, center.y - unit * 0.7f}, Vector2{center.x - unit * 0.52f, center.y + unit * 0.60f}, Vector2{center.x + unit * 0.52f, center.y + unit * 0.60f}, secondary);
            break;
        case 20:
            DrawLineEx(Vector2{center.x, center.y - unit * 1.0f}, Vector2{center.x, center.y + unit * 0.4f}, unit * 0.16f, primary);
            DrawLineEx(Vector2{center.x - unit * 0.8f, center.y + unit * 0.3f}, Vector2{center.x + unit * 0.8f, center.y + unit * 0.3f}, unit * 0.16f, primary);
            DrawCircleLines((int)center.x, (int)(center.y - unit * 0.95f), unit * 0.30f, secondary);
            DrawCircleLines((int)(center.x - unit * 0.7f), (int)(center.y + unit * 0.1f), unit * 0.22f, secondary);
            DrawCircleLines((int)(center.x + unit * 0.7f), (int)(center.y + unit * 0.1f), unit * 0.22f, secondary);
            break;
        case 21:
            DrawTriangle(Vector2{center.x, center.y - unit * 0.9f}, Vector2{center.x - unit * 1.1f, center.y + unit * 0.9f}, Vector2{center.x + unit * 1.1f, center.y + unit * 0.9f}, primary);
            DrawTriangle(Vector2{center.x, center.y - unit * 0.4f}, Vector2{center.x - unit * 0.58f, center.y + unit * 0.55f}, Vector2{center.x + unit * 0.58f, center.y + unit * 0.55f}, secondary);
            break;
        default:
            DrawCircleV(center, unit * 0.85f, primary);
            break;
    }
}

static const TextureAsset *GetRecipeTexture(const AssetBundle *assets, RecipeType recipe) {
    switch (recipe) {
        case RECIPE_GLOW_STICK:
            return &assets->iconGlowStick;
        case RECIPE_ROPE:
            return &assets->iconRope;
        case RECIPE_REINFORCED_METAL:
            return &assets->iconReinforcedMetal;
        case RECIPE_LASER_GUN:
            return &assets->iconLaserGun;
        case RECIPE_PROTECTION_SUIT:
            return &assets->iconProtectionSuit;
        case RECIPE_SIGNAL_AMPLIFIER:
            return &assets->iconSignalAmplifier;
        case RECIPE_FIELD_CAMP:
            return &assets->iconFieldCamp;
        case RECIPE_COUNT:
        default:
            return NULL;
    }
}

void UIInventory_DrawRecipeIcon(const AssetBundle *assets, RecipeType recipe, Rectangle rect, Color primary, Color secondary) {
    const TextureAsset *asset;

    asset = GetRecipeTexture(assets, recipe);
    if (asset != NULL && asset->loaded) {
        UIRuntime_DrawTextureAssetFitted(asset, rect, WHITE);
        return;
    }

    switch (recipe) {
        case RECIPE_GLOW_STICK:
            UIInventory_DrawBackpackIcon(assets, 16, rect, primary, secondary);
            break;
        case RECIPE_ROPE:
            UIInventory_DrawBackpackIcon(assets, 17, rect, primary, secondary);
            break;
        case RECIPE_REINFORCED_METAL: {
            Vector2 center;
            float unit;

            center = Vector2{rect.x + rect.width * 0.5f, rect.y + rect.height * 0.48f};
            unit = rect.width < rect.height ? rect.width : rect.height;
            unit *= 0.18f;
            DrawRectanglePro(Rectangle{center.x - unit * 1.2f, center.y - unit * 0.5f, unit * 1.55f, unit * 0.82f}, Vector2{0.0f, 0.0f}, -15.0f, primary);
            DrawRectanglePro(Rectangle{center.x - unit * 0.20f, center.y - unit * 0.15f, unit * 1.45f, unit * 0.82f}, Vector2{0.0f, 0.0f}, 15.0f, secondary);
            DrawRectangle((int)(center.x - unit * 0.30f), (int)(center.y - unit * 1.0f), (int)(unit * 0.6f), (int)(unit * 2.0f), Color{220, 232, 242, 150});
            break;
        }
        case RECIPE_LASER_GUN:
            UIInventory_DrawBackpackIcon(assets, 18, rect, primary, secondary);
            break;
        case RECIPE_PROTECTION_SUIT:
            UIInventory_DrawBackpackIcon(assets, 19, rect, primary, secondary);
            break;
        case RECIPE_SIGNAL_AMPLIFIER:
            UIInventory_DrawBackpackIcon(assets, 20, rect, primary, secondary);
            break;
        case RECIPE_FIELD_CAMP:
            UIInventory_DrawBackpackIcon(assets, 21, rect, primary, secondary);
            break;
        case RECIPE_COUNT:
        default:
            DrawCircleV(Vector2{rect.x + rect.width * 0.5f, rect.y + rect.height * 0.5f}, rect.width * 0.2f, primary);
            break;
    }
}
