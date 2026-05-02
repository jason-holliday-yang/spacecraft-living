#include "ui_system.h"

#include "localization.h"
#include "task_runtime_internal.h"
#include "ui_runtime_internal.h"

#include <cstdio>

namespace {

int CountLogsByCategory(const TaskSystem *tasks, ShipLogCategory category, bool collectedOnly) {
    int count = 0;

    if (tasks == nullptr) {
        return 0;
    }

    for (int index = 0; index < tasks->logCount; ++index) {
        const ShipLog *log = &tasks->logs[index];

        if (log->category != category) {
            continue;
        }
        if (collectedOnly && (!log->active || !log->collected)) {
            continue;
        }

        count += 1;
    }

    return count;
}

const char *GetLogCategoryLabel(const ShipLog *log) {
    if (log == nullptr) {
        return "";
    }

    return log->category == SHIP_LOG_MAINLINE
        ? Loc_PickLiteral("Main Archive", "主线档案")
        : Loc_PickLiteral("Supplemental Archive", "补充档案");
}

const char *GetWestRouteArchiveStatus(const TaskSystem *tasks) {
    if (tasks == nullptr) {
        return Loc_PickLiteral("Unknown", "未知");
    }
    if (tasks->westW5Completed) {
        return Loc_PickLiteral("Archived", "已归档");
    }
    if (tasks->westW5Started) {
        return Loc_PickLiteral("Final lead active", "最终线索进行中");
    }
    if (tasks->westW4Completed) {
        return Loc_PickLiteral("Last Camp ready", "最后营地就绪");
    }
    if (tasks->westW4Started) {
        return Loc_PickLiteral("Echo Basin underway", "回声盆地进行中");
    }
    if (tasks->westW3Completed) {
        return Loc_PickLiteral("Echo Basin archived", "回声盆地已归档");
    }
    if (tasks->westW3Started) {
        return Loc_PickLiteral("Canopy Hollow underway", "林冠洼地进行中");
    }
    if (tasks->westW2Completed) {
        return Loc_PickLiteral("Canopy archived", "林冠线已归档");
    }
    if (tasks->westW2Started) {
        return Loc_PickLiteral("Survey Break underway", "勘测断点进行中");
    }
    if (tasks->westW1Completed) {
        return Loc_PickLiteral("Survey archived", "勘测记录已归档");
    }
    if (tasks->westW1Started) {
        return Loc_PickLiteral("West Frontier underway", "西部前线进行中");
    }
    if (tasks->stage >= 4 && tasks->commRepairLevel >= 1) {
        return Loc_PickLiteral("Ready", "已就绪");
    }
    return Loc_PickLiteral("Locked", "未解锁");
}

const char *GetSouthRouteArchiveStatus(const TaskSystem *tasks) {
    if (tasks == nullptr) {
        return Loc_PickLiteral("Unknown", "未知");
    }
    if (tasks->southS5Completed) {
        return Loc_PickLiteral("Archived", "已归档");
    }
    if (tasks->southS5Started) {
        return Loc_PickLiteral("Final lead active", "最终线索进行中");
    }
    if (tasks->southS4Completed) {
        return Loc_PickLiteral("Root Vault ready", "根脉核心就绪");
    }
    if (tasks->southS4Started) {
        return Loc_PickLiteral("Purifier Ring underway", "净化环区进行中");
    }
    if (tasks->southS3Completed) {
        return Loc_PickLiteral("Ring archived", "环区记录已归档");
    }
    if (tasks->southS3Started) {
        return Loc_PickLiteral("Service Shafts underway", "维护井道进行中");
    }
    if (tasks->southS2Completed) {
        return Loc_PickLiteral("Shaft archived", "井道记录已归档");
    }
    if (tasks->southS2Started) {
        return Loc_PickLiteral("Vent Galleries underway", "通风廊道进行中");
    }
    if (tasks->southS1Completed) {
        return Loc_PickLiteral("Vent archived", "通风记录已归档");
    }
    if (tasks->southS1Started) {
        return Loc_PickLiteral("South Collapse underway", "南部塌陷区进行中");
    }
    if (tasks->stage >= 5 && tasks->energyRepairLevel >= 1) {
        return Loc_PickLiteral("Ready", "已就绪");
    }
    return Loc_PickLiteral("Locked", "未解锁");
}

const char *GetWestRouteArchiveNote(const TaskSystem *tasks) {
    if (tasks == nullptr) {
        return "";
    }
    if (tasks->westW5Completed) {
        return Loc_PickLiteral("Last Camp testimony is archived. The west route no longer ends in disappearance; it now closes as a full human record that will weigh directly on your final ending choice.", "最后营地证词已归档。西线不再只是失踪线索，而是完整闭合成了一段人的记录；它会直接压进你最终的结局判断里。");
    }
    if (tasks->westW5Started) {
        return Loc_PickLiteral("The Last Camp testimony is within reach. Finish the last west investigation and bring it back to base so the missing crew's final argument can be closed into the record.", "最后营地证词已近在眼前。完成西线最后一段调查后把它带回基地，才能把失踪船员留下的最后争论正式封进档案。");
    }
    if (tasks->westW4Completed) {
        return Loc_PickLiteral("Echo Basin findings are archived. What once sounded like absence now reads like successful relay work, and the west route now points toward the final record at Last Camp.", "回声盆地记录已归档。那些原本像空白的地方，如今已经被读成了一次成功接力；西线路线也因此开始明确指向最后营地的终局记录。");
    }
    if (tasks->westW4Started) {
        return Loc_PickLiteral("Echo Basin reconstruction is underway. Finish the basin investigation, then return to base so Loxi can rebuild the west handoff into something readable.", "回声盆地重建记录正在推进。完成盆地调查后请返回基地，让洛希把西线交接重新拼成一段能读懂的来龙去脉。");
    }
    if (tasks->westW3Completed) {
        return Loc_PickLiteral("Canopy Hollow is archived. The west trail now has enough evidence to connect people, route choice, and intent instead of leaving the missing crew as empty names.", "林冠洼地已归档。西线如今终于有了足够证据，把人员、路线与判断串到一起，而不再只留下几串失踪名字。");
    }
    if (tasks->westW3Started) {
        return Loc_PickLiteral("Canopy Hollow evidence is active. Bring the handoff record back to base once the investigation is complete.", "林冠洼地的证据线正在展开，调查结束后请把交接记录带回基地。");
    }
    if (tasks->westW2Completed) {
        return Loc_PickLiteral("Survey Break anchor notes are archived. The route now points deeper into Canopy Hollow.", "勘测断点锚点笔记已归档，路线现在更深入地指向林冠洼地。");
    }
    if (tasks->westW2Started) {
        return Loc_PickLiteral("Survey Break is active. Return to base after restoring the anchor notes.", "勘测断点调查正在进行，恢复锚点笔记后请返回基地。");
    }
    if (tasks->westW1Completed) {
        return Loc_PickLiteral("The frontier signal fragment is archived. Re-enter the west trail to continue at Survey Break.", "前线信号碎片已归档，重新进入西线即可继续前往勘测断点。");
    }
    if (tasks->westW1Started) {
        return Loc_PickLiteral("The west investigation has begun. Recover the opening signal fragment, then return to base to archive it.", "西线调查已经开始。先取回开端的信号碎片，再回基地完成归档。");
    }
    if (tasks->stage >= 4 && tasks->commRepairLevel >= 1) {
        return Loc_PickLiteral("Comms are restored. Enter West Frontier to begin the west investigation.", "通讯已恢复。进入西部前线，开始西线调查。");
    }
    return Loc_PickLiteral("Restore the comm relay first to unlock the west investigation.", "先修复通讯中继，才能解锁西线调查。");
}

const char *GetSouthRouteArchiveNote(const TaskSystem *tasks) {
    if (tasks == nullptr) {
        return "";
    }
    if (tasks->southS5Completed) {
        return Loc_PickLiteral("Root Vault findings are archived. The south route no longer reads like scattered machinery; it now closes into the truth of one damaged maintenance lattice, and that truth will weigh on your final ending choice.", "根脉核心记录已归档。南线不再像零散设施，而是闭合成了同一张受损维护格网的真相；这份真相会直接压进你最终的结局判断里。");
    }
    if (tasks->southS5Started) {
        return Loc_PickLiteral("The Root Vault dossier is within reach. Finish the last south investigation and bring it back to base so the system-level truth can finally be named outright.", "根脉核心档案已近在眼前。完成南线最后一段调查后请把它带回基地，好让这套系统级真相终于被正式说出口。");
    }
    if (tasks->southS4Completed) {
        return Loc_PickLiteral("Purifier Ring controls are archived. The south route now points beyond raw hazard toward the Root Vault core record where the system can finally explain itself.", "净化环控制记录已归档。南线如今不再只是继续深入危险，而是开始明确指向根脉核心那份能够解释整套系统的关键档案。");
    }
    if (tasks->southS4Started) {
        return Loc_PickLiteral("Purifier Ring work is active. Return to base after stabilizing the ring controls.", "净化环调查正在进行，稳定控制环后请返回基地。");
    }
    if (tasks->southS3Completed) {
        return Loc_PickLiteral("Service Shaft sync data is archived. The south route now clearly links ship systems to the buried facility network.", "维护井道同步记录已归档，南线如今清楚串起了飞船系统与地下设施网络。");
    }
    if (tasks->southS3Started) {
        return Loc_PickLiteral("Service Shaft recovery is active. Bring the sync record back to base once the investigation is complete.", "维护井道回收任务正在进行，调查完成后请把同步记录带回基地。");
    }
    if (tasks->southS2Completed) {
        return Loc_PickLiteral("Vent calibration notes are archived. The route now points deeper into the Service Shafts.", "通风校准记录已归档，路线现在更深入地指向维护井道。");
    }
    if (tasks->southS2Started) {
        return Loc_PickLiteral("Vent Galleries are active. Return to base after restoring the handover record.", "通风廊道调查正在进行，恢复交接记录后请返回基地。");
    }
    if (tasks->southS1Completed) {
        return Loc_PickLiteral("The South Collapse outage memo is archived. Re-enter the facility edge to continue into the vents.", "南部塌陷区停摆备忘已归档，重新进入设施边缘即可继续深入通风区域。");
    }
    if (tasks->southS1Started) {
        return Loc_PickLiteral("The south facility investigation has begun. Recover the outage memo, then return to base to archive it.", "南方设施调查已经开始。先取回停摆备忘，再回基地完成归档。");
    }
    if (tasks->stage >= 5 && tasks->energyRepairLevel >= 1) {
        return Loc_PickLiteral("Power is restored. Enter South Collapse to begin the south facility investigation.", "电力已恢复。进入南部塌陷区，开始南方设施调查。");
    }
    return Loc_PickLiteral("Restore the Power Bay first to unlock the south facility investigation.", "先恢复动力舱，才能解锁南方设施调查。");
}

const char *GetCrossRouteArchiveStatus(const TaskSystem *tasks) {
    if (tasks == nullptr) {
        return Loc_PickLiteral("Unknown", "未知");
    }
    if (tasks->westW5Completed && tasks->southS5Completed) {
        return Loc_PickLiteral("Choice ready", "抉择已就绪");
    }
    if (tasks->westW4Completed && tasks->southS4Completed) {
        return Loc_PickLiteral("Meaning clear", "意义已清晰");
    }
    if (tasks->westW3Completed && tasks->southS2Completed) {
        return Loc_PickLiteral("Linked", "已串联");
    }
    if (tasks->westW3Completed || tasks->southS2Completed) {
        return Loc_PickLiteral("Partial", "部分完成");
    }
    return Loc_PickLiteral("Locked", "未解锁");
}

const char *GetCrossRouteArchiveNote(const TaskSystem *tasks) {
    if (tasks == nullptr) {
        return "";
    }
    if (tasks->westW5Completed && tasks->southS5Completed) {
        return Loc_PickLiteral("Both route conclusions are ready. Crew handoffs and facility truth now sit in the same archive, so your ending choice finally carries the full human and system context together.", "两条路线的结论都已就位。船员接力与设施真相如今已经并进同一套档案，因此你的结局选择终于同时背着完整的人与系统背景。");
    }
    if (tasks->westW4Completed && tasks->southS4Completed) {
        return Loc_PickLiteral("Loxi can now synthesize both investigations into a final strategy comparison. Rescue, stabilization, and settlement are starting to read like different answers to the same corrected truth.", "洛希现在可以把两条调查线整合成终局策略对照。救援、稳定与定居开始不再像分支按钮，而像对同一份纠偏真相的不同回答。");
    }
    if (tasks->westW3Completed && tasks->southS2Completed) {
        return Loc_PickLiteral("Crew trails and facility records now form one chain. The truth is no longer split across regions, only waiting to be pushed far enough to close.", "船员轨迹与设施记录已经连成一条链。真相不再散落在不同区域，只差再往前推一些，就能真正闭合。");
    }
    if (tasks->westW3Completed || tasks->southS2Completed) {
        return Loc_PickLiteral("The shared truth is only partial. Push both west and south investigations a little farther.", "共通真相仍然只是片段，请继续推进西线与南线调查。");
    }
    return Loc_PickLiteral("Shared insight is still locked. Push both investigations farther so the evidence can meet in the middle.", "共通线索尚未解锁，请继续推进两条调查线，让证据在中段汇合。");
}

}  // namespace

void UI_DrawLogReader(const TaskSystem *tasks, int selectedLog, const AssetBundle *assets, int screenWidth, int screenHeight) {
    float scale = UIRuntime_GetScale(screenWidth, screenHeight);
    Rectangle panel = UI_GetStandardOverlayRect(screenWidth, screenHeight);
    Rectangle listPanel{
        panel.x + 20.0f * scale,
        panel.y + 110.0f * scale,
        282.0f * scale,
        panel.height - 142.0f * scale,
    };
    Rectangle contentPanel{
        panel.x + 326.0f * scale,
        panel.y + 110.0f * scale,
        panel.width - 346.0f * scale,
        panel.height - 142.0f * scale,
    };
    Rectangle textPanel{
        contentPanel.x,
        contentPanel.y + 64.0f * scale,
        contentPanel.width,
        contentPanel.height - 186.0f * scale,
    };
    Rectangle routePanel{
        contentPanel.x,
        contentPanel.y + contentPanel.height - 118.0f * scale,
        contentPanel.width,
        118.0f * scale,
    };
    Rectangle storyRect{
        textPanel.x + 18.0f * scale,
        textPanel.y + 18.0f * scale,
        textPanel.width - 36.0f * scale,
        textPanel.height - 34.0f * scale,
    };
    char buffer[128];
    char locationBuffer[128];
    char routeBuffer[192];
    char sanitizedBuffer[256];
    char sanitizedNote[512];
    const char *bodyText = "";
    int collectedCount = Tasks_GetCollectedLogCount(tasks);
    int mainCollectedCount = CountLogsByCategory(tasks, SHIP_LOG_MAINLINE, true);
    int mainTotalCount = CountLogsByCategory(tasks, SHIP_LOG_MAINLINE, false);
    int supplementalCollectedCount = CountLogsByCategory(tasks, SHIP_LOG_SUPPLEMENTAL, true);
    int supplementalTotalCount = CountLogsByCategory(tasks, SHIP_LOG_SUPPLEMENTAL, false);
    const ShipLog *log = nullptr;

    if (collectedCount > 0) {
        if (selectedLog < 0) {
            selectedLog = 0;
        }
        if (selectedLog >= collectedCount) {
            selectedLog = collectedCount - 1;
        }
    }
    log = Tasks_GetCollectedLogAt(tasks, selectedLog);

    DrawRectangle(0, 0, screenWidth, screenHeight, Color{5, 9, 16, 190});
    UIRuntime_DrawPanel(panel, Color{8, 18, 30, 245}, Color{99, 233, 195, 75});

    UIRuntime_DrawText(assets, Loc_PickLiteral("Ship Log Archive", "飞船日志档案"), Vector2{panel.x + 30.0f * scale, panel.y + 22.0f * scale}, 33.0f * scale, WHITE);
    UIRuntime_DrawText(
        assets,
        Loc_PickLiteral("Press ESC to close", "按 ESC 关闭"),
        Vector2{
            panel.x + panel.width - UIRuntime_MeasureText(assets, Loc_PickLiteral("Press ESC to close", "按 ESC 关闭"), 17.5f * scale).x - 30.0f * scale,
            panel.y + 28.0f * scale,
        },
        17.5f * scale,
        Color{182, 199, 214, 255}
    );
    std::snprintf(buffer, sizeof(buffer), "%s %d / %d", Loc_PickLiteral("Collected", "已收集"), collectedCount, tasks->logCount);
    UIRuntime_DrawText(assets, buffer, Vector2{panel.x + 30.0f * scale, panel.y + 60.0f * scale}, 17.0f * scale, Color{194, 224, 255, 255});
    std::snprintf(buffer, sizeof(buffer), "%s %d / %d   %s %d / %d", Loc_PickLiteral("Main Archive", "主线档案"), mainCollectedCount, mainTotalCount, Loc_PickLiteral("Supplemental", "补充档案"), supplementalCollectedCount, supplementalTotalCount);
    UIRuntime_DrawText(assets, buffer, Vector2{panel.x + 230.0f * scale, panel.y + 60.0f * scale}, 15.0f * scale, Color{169, 207, 231, 255});
    UIRuntime_DrawPanel(listPanel, Color{14, 26, 42, 220}, Color{255, 255, 255, 22});
    UIRuntime_DrawText(assets, Loc_PickLiteral("Entries", "条目"), Vector2{listPanel.x + 16.0f * scale, listPanel.y + 14.0f * scale}, 22.0f * scale, WHITE);

    for (int entryIndex = 0; entryIndex < collectedCount; ++entryIndex) {
        const ShipLog *entry = Tasks_GetCollectedLogAt(tasks, entryIndex);
        Rectangle entryRect = UI_GetLogEntryRect(screenWidth, screenHeight, entryIndex);
        bool selected = entryIndex == selectedLog;

        UIRuntime_DrawPanel(
            entryRect,
            selected ? Color{18, 42, 62, 240} : Color{11, 20, 32, 220},
            selected ? Color{255, 214, 154, 190} : Color{105, 168, 222, 50}
        );
        std::snprintf(buffer, sizeof(buffer), "%s %02d", Loc_PickLiteral("Log", "日志"), entryIndex + 1);
        UIRuntime_DrawText(
            assets,
            buffer,
            Vector2{entryRect.x + 12.0f * scale, entryRect.y + 8.0f * scale},
            13.0f * scale,
            selected ? Color{255, 220, 150, 255} : Color{172, 196, 215, 255}
        );
        if (entry != nullptr) {
            UIRuntime_DrawText(
                assets,
                GetLogCategoryLabel(entry),
                Vector2{entryRect.x + entryRect.width - 116.0f * scale, entryRect.y + 8.0f * scale},
                11.0f * scale,
                selected ? Color{166, 255, 226, 255} : Color{141, 211, 200, 255}
            );
        }
        if (entry != nullptr) {
            UIRuntime_DrawWrappedText(
                assets,
                Tasks_GetLogTitle(entry),
                Rectangle{
                    entryRect.x + 12.0f * scale,
                    entryRect.y + 24.0f * scale,
                    entryRect.width - 24.0f * scale,
                    entryRect.height - 16.0f * scale,
                },
                13.5f * scale,
                15.5f * scale,
                WHITE
            );
        }
    }

    if (log != nullptr) {
        bodyText = Tasks_GetLogDetailText(log);
        if (bodyText[0] == '\0') {
            bodyText = Tasks_GetLogStoryText(log);
        }

        UIRuntime_DrawWrappedText(assets,
                                  Tasks_GetLogTitle(log),
                                  Rectangle{contentPanel.x, contentPanel.y + 2.0f * scale, contentPanel.width, 24.0f * scale},
                                  19.5f * scale,
                                  21.0f * scale,
                                  Color{166, 255, 226, 255});
        std::snprintf(locationBuffer,
                      sizeof(locationBuffer),
                      "%s  |  %s: %s",
                      GetLogCategoryLabel(log),
                      Loc_PickLiteral("Location", "位置"),
                      Loc_GetLocationNameText(Map_GetLocationNameAt(log->gridX, log->gridY)));
        UIRuntime_DrawText(assets, locationBuffer, Vector2{contentPanel.x, contentPanel.y + 30.0f * scale}, 14.8f * scale, Color{194, 224, 255, 255});
        UIRuntime_DrawPanel(textPanel, Color{14, 26, 42, 220}, Color{255, 255, 255, 22});
        UIRuntime_DrawWrappedText(assets, bodyText, storyRect, 18.2f * scale, 24.8f * scale, Color{220, 235, 248, 255});
    } else {
        UIRuntime_DrawText(
            assets,
            Loc_PickLiteral("No collected log selected", "未选择已收集日志"),
            Vector2{contentPanel.x, contentPanel.y + contentPanel.height * 0.5f},
            20.0f * scale,
            Color{180, 180, 180, 255}
        );
    }

    UIRuntime_DrawPanel(routePanel, Color{13, 24, 40, 220}, Color{115, 175, 214, 52});
    UIRuntime_DrawText(assets, Loc_PickLiteral("Expedition Summary", "远征总览"), Vector2{routePanel.x + 16.0f * scale, routePanel.y + 9.0f * scale}, 18.0f * scale, Color{201, 228, 250, 255});
    std::snprintf(routeBuffer, sizeof(routeBuffer), "%s  %s", Loc_GetLocationNameText("West Frontier"), GetWestRouteArchiveStatus(tasks));
    TasksRuntime_SanitizeDisplayText(routeBuffer, sanitizedBuffer, sizeof(sanitizedBuffer));
    UIRuntime_DrawText(assets, sanitizedBuffer, Vector2{routePanel.x + 16.0f * scale, routePanel.y + 34.0f * scale}, 14.2f * scale, Color{184, 214, 240, 255});
    TasksRuntime_SanitizeDisplayText(GetWestRouteArchiveNote(tasks), sanitizedNote, sizeof(sanitizedNote));
    UIRuntime_DrawWrappedText(assets, sanitizedNote, Rectangle{routePanel.x + 176.0f * scale, routePanel.y + 30.0f * scale, routePanel.width - 192.0f * scale, 20.0f * scale}, 12.4f * scale, 13.8f * scale, Color{172, 194, 214, 255});
    std::snprintf(routeBuffer, sizeof(routeBuffer), "%s  %s", Loc_GetLocationNameText("South Collapse"), GetSouthRouteArchiveStatus(tasks));
    TasksRuntime_SanitizeDisplayText(routeBuffer, sanitizedBuffer, sizeof(sanitizedBuffer));
    UIRuntime_DrawText(assets, sanitizedBuffer, Vector2{routePanel.x + 16.0f * scale, routePanel.y + 58.0f * scale}, 14.2f * scale, Color{184, 214, 240, 255});
    TasksRuntime_SanitizeDisplayText(GetSouthRouteArchiveNote(tasks), sanitizedNote, sizeof(sanitizedNote));
    UIRuntime_DrawWrappedText(assets, sanitizedNote, Rectangle{routePanel.x + 176.0f * scale, routePanel.y + 54.0f * scale, routePanel.width - 192.0f * scale, 20.0f * scale}, 12.4f * scale, 13.8f * scale, Color{172, 194, 214, 255});
    std::snprintf(routeBuffer, sizeof(routeBuffer), "%s  %s", Loc_PickLiteral("Shared Truth", "共通真相"), GetCrossRouteArchiveStatus(tasks));
    TasksRuntime_SanitizeDisplayText(routeBuffer, sanitizedBuffer, sizeof(sanitizedBuffer));
    UIRuntime_DrawText(assets, sanitizedBuffer, Vector2{routePanel.x + 16.0f * scale, routePanel.y + 82.0f * scale}, 14.2f * scale, Color{188, 225, 196, 255});
    TasksRuntime_SanitizeDisplayText(GetCrossRouteArchiveNote(tasks), sanitizedNote, sizeof(sanitizedNote));
    UIRuntime_DrawWrappedText(assets, sanitizedNote, Rectangle{routePanel.x + 176.0f * scale, routePanel.y + 78.0f * scale, routePanel.width - 192.0f * scale, 24.0f * scale}, 12.4f * scale, 13.8f * scale, Color{172, 194, 214, 255});
}
