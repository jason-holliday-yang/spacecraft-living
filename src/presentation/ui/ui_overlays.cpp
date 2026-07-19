#include "ui_system.h"

#include "localization.h"
#include "ui_runtime_internal.h"
#include "ui_components_internal.h"

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
    const UITheme *theme = UITheme_Get();
    const float scale = UIRuntime_GetScale(screenWidth, screenHeight);
    const Rectangle panel = UI_GetStandardOverlayRect(screenWidth, screenHeight);
    const Rectangle railRect{panel.x + panel.width * 0.5f - 330.0f * scale,
                             panel.y + 7.0f * scale,
                             660.0f * scale,
                             54.0f * scale};

    UIComponents_DrawCard(railRect, false, true, scale, theme->signalBlue);
    for (int tabIndex = 0; tabIndex < INFO_OVERLAY_TAB_COUNT; ++tabIndex) {
        const InfoOverlayTab tab = (InfoOverlayTab)tabIndex;
        const bool selected = tab == selectedTab;
        const bool locked = tab == INFO_OVERLAY_TAB_LOXI && !communicatorUnlocked;
        const Rectangle tabRect = UI_GetInfoOverlayTabRect(screenWidth, screenHeight, tabIndex);
        const char *label = locked
            ? Loc_PickLiteral("SIGNAL LOCKED", "信号未同步")
            : GetInfoOverlayTabLabel(tab);

        UIComponents_DrawCompactButton(assets,
                                       tabRect,
                                       label,
                                       !locked,
                                       selected,
                                       selected ? UI_ACTION_PRIMARY : UI_ACTION_GHOST,
                                       scale);
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
