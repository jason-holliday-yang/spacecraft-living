#include "ui_system.h"

#include "localization.h"
#include "ui_runtime_internal.h"

namespace {

InfoOverlayTab ResolveInfoOverlayTab(InfoOverlayTab selectedTab, const TaskSystem *tasks) {
    if (selectedTab < INFO_OVERLAY_TAB_MAP || selectedTab >= INFO_OVERLAY_TAB_COUNT) {
        return INFO_OVERLAY_TAB_MAP;
    }

    if (selectedTab == INFO_OVERLAY_TAB_LOXI && !Tasks_IsCommunicatorUnlocked(tasks)) {
        return INFO_OVERLAY_TAB_MAP;
    }

    return selectedTab;
}

const char *GetInfoOverlayTabLabel(InfoOverlayTab tab) {
    switch (tab) {
        case INFO_OVERLAY_TAB_BACKPACK:
            return Loc_PickLiteral("Backpack", "背包");
        case INFO_OVERLAY_TAB_LOXI:
            return Loc_PickLiteral("Loxi", "洛希");
        case INFO_OVERLAY_TAB_SETTINGS:
            return Loc_PickLiteral("Settings", "设置");
        case INFO_OVERLAY_TAB_MAP:
        default:
            return Loc_PickLiteral("Map", "地图");
    }
}

void DrawInfoOverlayTabs(const AssetBundle *assets,
                         InfoOverlayTab selectedTab,
                         bool communicatorUnlocked,
                         int screenWidth,
                         int screenHeight) {
    float scale;
    Rectangle panel;

    scale = UIRuntime_GetScale(screenWidth, screenHeight);
    panel = UI_GetStandardOverlayRect(screenWidth, screenHeight);

    DrawRectangleRounded(Rectangle{panel.x + panel.width * 0.5f - 254.0f * scale,
                                   panel.y + 10.0f * scale,
                                   508.0f * scale,
                                   48.0f * scale},
                         0.45f,
                         12,
                         Color{7, 15, 27, 210});

    for (int tabIndex = 0; tabIndex < INFO_OVERLAY_TAB_COUNT; ++tabIndex) {
        const InfoOverlayTab tab = (InfoOverlayTab)tabIndex;
        const bool selected = tab == selectedTab;
        const bool locked = tab == INFO_OVERLAY_TAB_LOXI && !communicatorUnlocked;
        const Color fill = locked
            ? Color{12, 20, 31, 208}
            : (selected ? Color{28, 64, 96, 242} : Color{12, 24, 39, 230});
        const Color outline = locked
            ? Color{92, 104, 126, 70}
            : (selected ? Color{255, 214, 154, 220} : Color{99, 221, 194, 75});
        const Color textColor = locked
            ? Color{128, 142, 160, 255}
            : (selected ? WHITE : Color{214, 226, 238, 255});
        const Rectangle tabRect = UI_GetInfoOverlayTabRect(screenWidth, screenHeight, tabIndex);
        const char *label = GetInfoOverlayTabLabel(tab);
        const float fontSize = 17.0f * scale;
        const Vector2 textSize = UIRuntime_MeasureText(assets, label, fontSize);

        UIRuntime_DrawPanel(tabRect, fill, outline);
        UIRuntime_DrawText(assets,
                           label,
                           Vector2{tabRect.x + tabRect.width * 0.5f - textSize.x * 0.5f,
                                   tabRect.y + tabRect.height * 0.5f - textSize.y * 0.5f},
                           fontSize,
                           textColor);
    }
}

}  // namespace

/*
 * Compatibility translation unit kept in place while log/map/save overlays
 * live in ui_log_reader.cpp, ui_map_panel.cpp, and ui_save_slots_panel.cpp.
 */

void UI_DrawInfoOverlay(const AssetBundle *assets,
                        InfoOverlayTab selectedTab,
                        const GameSettings *settings,
                        const MiniMap *minimap,
                        const Player *player,
                        const TaskSystem *tasks,
                        const GameMap *map,
                        const bool *storySceneShown,
                        int saveCount,
                        int selectedBackpackItem,
                        int communicatorTab,
                        int selectedLog,
                        int firstVisibleLog,
                        int selectedStoryScene,
                        int firstVisibleStoryScene,
                        float detailVisibility,
                        float detailScroll,
                        int screenWidth,
                        int screenHeight) {
    const InfoOverlayTab resolvedTab = ResolveInfoOverlayTab(selectedTab, tasks);

    if (resolvedTab == INFO_OVERLAY_TAB_MAP) {
        UI_DrawMapOverlay(assets, minimap, player, tasks, map, screenWidth, screenHeight);
    } else if (resolvedTab == INFO_OVERLAY_TAB_BACKPACK) {
        UI_DrawBackpackOverlay(assets, player, selectedBackpackItem, screenWidth, screenHeight);
    } else if (resolvedTab == INFO_OVERLAY_TAB_SETTINGS) {
        UI_DrawSettingsOverlay(assets,
                               settings,
                               SaveSystem_GetActiveAccountName(),
                               saveCount,
                               false,
                               screenWidth,
                               screenHeight);
    } else {
        UI_DrawCommunicatorOverlay(assets,
                                   tasks,
                                   storySceneShown,
                                   communicatorTab,
                                   selectedLog,
                                   firstVisibleLog,
                                   selectedStoryScene,
                                   firstVisibleStoryScene,
                                   detailVisibility,
                                   detailScroll,
                                   screenWidth,
                                   screenHeight);
    }

    DrawInfoOverlayTabs(assets, resolvedTab, Tasks_IsCommunicatorUnlocked(tasks), screenWidth, screenHeight);
}
