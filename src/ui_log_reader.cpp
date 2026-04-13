#include "ui_system.h"

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

    return log->category == SHIP_LOG_MAINLINE ? "Main Archive" : "Supplemental Archive";
}

const char *GetWestRouteArchiveStatus(const TaskSystem *tasks) {
    if (tasks == nullptr) {
        return "Unknown";
    }
    if (tasks->westW5Completed) {
        return "Archived";
    }
    if (tasks->westW5Started) {
        return "Final pass active";
    }
    if (tasks->westW4Completed) {
        return "Last Camp ready";
    }
    if (tasks->westW4Started) {
        return "Echo Basin active";
    }
    if (tasks->westW3Completed) {
        return "Echo Basin archived";
    }
    if (tasks->westW3Started) {
        return "Canopy active";
    }
    if (tasks->westW2Completed) {
        return "Canopy archived";
    }
    if (tasks->westW2Started) {
        return "Survey active";
    }
    if (tasks->westW1Completed) {
        return "Survey archived";
    }
    if (tasks->westW1Started) {
        return "Frontier active";
    }
    if (tasks->stage >= 4 && tasks->commRepairLevel >= 1) {
        return "Ready";
    }
    return "Locked";
}

const char *GetSouthRouteArchiveStatus(const TaskSystem *tasks) {
    if (tasks == nullptr) {
        return "Unknown";
    }
    if (tasks->southS5Completed) {
        return "Archived";
    }
    if (tasks->southS5Started) {
        return "Final pass active";
    }
    if (tasks->southS4Completed) {
        return "Root Vault ready";
    }
    if (tasks->southS4Started) {
        return "Ring active";
    }
    if (tasks->southS3Completed) {
        return "Ring archived";
    }
    if (tasks->southS3Started) {
        return "Shaft active";
    }
    if (tasks->southS2Completed) {
        return "Shaft archived";
    }
    if (tasks->southS2Started) {
        return "Vent active";
    }
    if (tasks->southS1Completed) {
        return "Vent archived";
    }
    if (tasks->southS1Started) {
        return "Threshold active";
    }
    if (tasks->stage >= 5 && tasks->energyRepairLevel >= 1) {
        return "Ready";
    }
    return "Locked";
}

const char *GetWestRouteArchiveNote(const TaskSystem *tasks) {
    if (tasks == nullptr) {
        return "";
    }
    if (tasks->westW5Completed) {
        return "Last Camp testimony is archived. The full west investigation now supports your final ending choice.";
    }
    if (tasks->westW5Started) {
        return "The Last Camp testimony is in reach. Return to base after the final west pass to file the complete record.";
    }
    if (tasks->westW4Completed) {
        return "Echo Basin findings are archived. The west route now points toward the final record at Last Camp.";
    }
    if (tasks->westW4Started) {
        return "Echo Basin reconstruction is active. Return to base after finishing the basin pass.";
    }
    if (tasks->westW3Completed) {
        return "Canopy Hollow is archived. The west trail now has enough evidence to connect people, route choice, and intent.";
    }
    if (tasks->westW3Started) {
        return "Canopy Hollow evidence is active. Bring the handoff record back to base once the pass is complete.";
    }
    if (tasks->westW2Completed) {
        return "Survey Break anchor notes are archived. The route now points deeper into Canopy Hollow.";
    }
    if (tasks->westW2Started) {
        return "Survey Break is active. Return to base after restoring the anchor notes.";
    }
    if (tasks->westW1Completed) {
        return "The frontier signal fragment is archived. Re-enter the west trail to continue at Survey Break.";
    }
    if (tasks->westW1Started) {
        return "The west investigation has begun. Recover the opening signal fragment, then return to base to archive it.";
    }
    if (tasks->stage >= 4 && tasks->commRepairLevel >= 1) {
        return "Comms are restored. Enter West Frontier to begin the west investigation.";
    }
    return "Restore the comm relay first to unlock the west investigation.";
}

const char *GetSouthRouteArchiveNote(const TaskSystem *tasks) {
    if (tasks == nullptr) {
        return "";
    }
    if (tasks->southS5Completed) {
        return "Root Vault findings are archived. The full south facility chain now supports your final ending choice.";
    }
    if (tasks->southS5Started) {
        return "The Root Vault dossier is in reach. Return to base after the final south pass to file it.";
    }
    if (tasks->southS4Completed) {
        return "Purifier Ring controls are archived. The south route now points toward the Root Vault core record.";
    }
    if (tasks->southS4Started) {
        return "Purifier Ring work is active. Return to base after stabilizing the ring controls.";
    }
    if (tasks->southS3Completed) {
        return "Service Shaft sync data is archived. The south route now clearly links ship systems to the buried facility network.";
    }
    if (tasks->southS3Started) {
        return "Service Shaft recovery is active. Bring the sync record back to base once the pass is complete.";
    }
    if (tasks->southS2Completed) {
        return "Vent calibration notes are archived. The route now points deeper into the Service Shafts.";
    }
    if (tasks->southS2Started) {
        return "Vent Galleries are active. Return to base after restoring the calibration handover.";
    }
    if (tasks->southS1Completed) {
        return "The South Collapse outage memo is archived. Re-enter the facility edge to continue into the vents.";
    }
    if (tasks->southS1Started) {
        return "The south facility investigation has begun. Recover the outage memo, then return to base to archive it.";
    }
    if (tasks->stage >= 5 && tasks->energyRepairLevel >= 1) {
        return "Power is restored. Enter South Collapse to begin the south facility investigation.";
    }
    return "Restore the Power Bay first to unlock the south facility investigation.";
}

const char *GetCrossRouteArchiveStatus(const TaskSystem *tasks) {
    if (tasks == nullptr) {
        return "Unknown";
    }
    if (tasks->westW5Completed && tasks->southS5Completed) {
        return "Decision ready";
    }
    if (tasks->westW4Completed && tasks->southS4Completed) {
        return "Strategy ready";
    }
    if (tasks->westW3Completed && tasks->southS2Completed) {
        return "Insight ready";
    }
    if (tasks->westW3Completed || tasks->southS2Completed) {
        return "Partial";
    }
    return "Locked";
}

const char *GetCrossRouteArchiveNote(const TaskSystem *tasks) {
    if (tasks == nullptr) {
        return "";
    }
    if (tasks->westW5Completed && tasks->southS5Completed) {
        return "Both route conclusions are ready. Your ending choice now carries the full crew and facility context.";
    }
    if (tasks->westW4Completed && tasks->southS4Completed) {
        return "Loxi can now synthesize both investigations into a final strategy comparison for the endgame.";
    }
    if (tasks->westW3Completed && tasks->southS2Completed) {
        return "Crew trails and facility records now form one chain. The truth is no longer split across regions.";
    }
    if (tasks->westW3Completed || tasks->southS2Completed) {
        return "The shared truth is only partial. Push both west and south investigations a little farther.";
    }
    return "Shared insight is locked. Advance the west and south investigations to connect the evidence.";
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
        contentPanel.y + 80.0f * scale,
        contentPanel.width,
        contentPanel.height - 230.0f * scale,
    };
    Rectangle routePanel{
        contentPanel.x,
        contentPanel.y + contentPanel.height - 142.0f * scale,
        contentPanel.width,
        142.0f * scale,
    };
    Rectangle storyRect{
        textPanel.x + 15.0f * scale,
        textPanel.y + 15.0f * scale,
        textPanel.width - 30.0f * scale,
        textPanel.height - 56.0f * scale,
    };
    Rectangle rewardRect{
        textPanel.x + 15.0f * scale,
        textPanel.y + textPanel.height - 34.0f * scale,
        textPanel.width - 30.0f * scale,
        20.0f * scale,
    };
    char buffer[128];
    char locationBuffer[128];
    char routeBuffer[192];
    char sanitizedBuffer[256];
    char sanitizedNote[512];
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

    UIRuntime_DrawText(assets, "Ship Log Archive", Vector2{panel.x + 30.0f * scale, panel.y + 22.0f * scale}, 33.0f * scale, WHITE);
    UIRuntime_DrawText(
        assets,
        "Press ESC or L to close",
        Vector2{
            panel.x + panel.width - UIRuntime_MeasureText(assets, "Press ESC or L to close", 17.5f * scale).x - 30.0f * scale,
            panel.y + 28.0f * scale,
        },
        17.5f * scale,
        Color{182, 199, 214, 255}
    );
    std::snprintf(buffer, sizeof(buffer), "Collected %d / %d", collectedCount, tasks->logCount);
    UIRuntime_DrawText(assets, buffer, Vector2{panel.x + 30.0f * scale, panel.y + 60.0f * scale}, 17.0f * scale, Color{194, 224, 255, 255});
    std::snprintf(buffer, sizeof(buffer), "Main Archive %d / %d   Supplemental %d / %d", mainCollectedCount, mainTotalCount, supplementalCollectedCount, supplementalTotalCount);
    UIRuntime_DrawText(assets, buffer, Vector2{panel.x + 230.0f * scale, panel.y + 60.0f * scale}, 15.0f * scale, Color{169, 207, 231, 255});
    UIRuntime_DrawPanel(listPanel, Color{14, 26, 42, 220}, Color{255, 255, 255, 22});
    UIRuntime_DrawText(assets, "Entries", Vector2{listPanel.x + 16.0f * scale, listPanel.y + 14.0f * scale}, 22.0f * scale, WHITE);

    for (int entryIndex = 0; entryIndex < collectedCount; ++entryIndex) {
        const ShipLog *entry = Tasks_GetCollectedLogAt(tasks, entryIndex);
        Rectangle entryRect = UI_GetLogEntryRect(screenWidth, screenHeight, entryIndex);
        bool selected = entryIndex == selectedLog;

        UIRuntime_DrawPanel(
            entryRect,
            selected ? Color{18, 42, 62, 240} : Color{11, 20, 32, 220},
            selected ? Color{255, 214, 154, 190} : Color{105, 168, 222, 50}
        );
        std::snprintf(buffer, sizeof(buffer), "Log %02d", entryIndex + 1);
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
                entry->title,
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
        UIRuntime_DrawWrappedText(assets, log->title, Rectangle{contentPanel.x, contentPanel.y, contentPanel.width, 28.0f * scale}, 22.0f * scale, 22.0f * scale, Color{166, 255, 226, 255});
        std::snprintf(locationBuffer,
                      sizeof(locationBuffer),
                      "%s  |  Location: %s",
                      GetLogCategoryLabel(log),
                      Map_GetLocationNameAt(log->gridX, log->gridY));
        UIRuntime_DrawText(assets, locationBuffer, Vector2{contentPanel.x, contentPanel.y + 40.0f * scale}, 16.0f * scale, Color{194, 224, 255, 255});
        UIRuntime_DrawPanel(textPanel, Color{14, 26, 42, 220}, Color{255, 255, 255, 22});
        UIRuntime_DrawWrappedText(assets, log->storyText, storyRect, 17.0f * scale, 22.0f * scale, Color{220, 235, 248, 255});

        if (log->rewardDesc[0] != '\0') {
            UIRuntime_DrawWrappedText(assets, log->rewardDesc, rewardRect, 15.0f * scale, 18.0f * scale, Color{255, 220, 150, 255});
        }
    } else {
        UIRuntime_DrawText(
            assets,
            "No collected log selected",
            Vector2{contentPanel.x, contentPanel.y + contentPanel.height * 0.5f},
            20.0f * scale,
            Color{180, 180, 180, 255}
        );
    }

    UIRuntime_DrawPanel(routePanel, Color{13, 24, 40, 220}, Color{115, 175, 214, 52});
    UIRuntime_DrawText(assets, "Expedition Summary", Vector2{routePanel.x + 16.0f * scale, routePanel.y + 10.0f * scale}, 20.0f * scale, Color{201, 228, 250, 255});
    std::snprintf(routeBuffer, sizeof(routeBuffer), "West Frontier  %s", GetWestRouteArchiveStatus(tasks));
    TasksRuntime_SanitizeDisplayText(routeBuffer, sanitizedBuffer, sizeof(sanitizedBuffer));
    UIRuntime_DrawText(assets, sanitizedBuffer, Vector2{routePanel.x + 16.0f * scale, routePanel.y + 38.0f * scale}, 15.0f * scale, Color{184, 214, 240, 255});
    TasksRuntime_SanitizeDisplayText(GetWestRouteArchiveNote(tasks), sanitizedNote, sizeof(sanitizedNote));
    UIRuntime_DrawWrappedText(assets, sanitizedNote, Rectangle{routePanel.x + 188.0f * scale, routePanel.y + 34.0f * scale, routePanel.width - 204.0f * scale, 22.0f * scale}, 13.0f * scale, 14.0f * scale, Color{172, 194, 214, 255});
    std::snprintf(routeBuffer, sizeof(routeBuffer), "South Collapse  %s", GetSouthRouteArchiveStatus(tasks));
    TasksRuntime_SanitizeDisplayText(routeBuffer, sanitizedBuffer, sizeof(sanitizedBuffer));
    UIRuntime_DrawText(assets, sanitizedBuffer, Vector2{routePanel.x + 16.0f * scale, routePanel.y + 68.0f * scale}, 15.0f * scale, Color{184, 214, 240, 255});
    TasksRuntime_SanitizeDisplayText(GetSouthRouteArchiveNote(tasks), sanitizedNote, sizeof(sanitizedNote));
    UIRuntime_DrawWrappedText(assets, sanitizedNote, Rectangle{routePanel.x + 188.0f * scale, routePanel.y + 64.0f * scale, routePanel.width - 204.0f * scale, 22.0f * scale}, 13.0f * scale, 14.0f * scale, Color{172, 194, 214, 255});
    std::snprintf(routeBuffer, sizeof(routeBuffer), "Cross Chain  %s", GetCrossRouteArchiveStatus(tasks));
    TasksRuntime_SanitizeDisplayText(routeBuffer, sanitizedBuffer, sizeof(sanitizedBuffer));
    UIRuntime_DrawText(assets, sanitizedBuffer, Vector2{routePanel.x + 16.0f * scale, routePanel.y + 98.0f * scale}, 15.0f * scale, Color{188, 225, 196, 255});
    TasksRuntime_SanitizeDisplayText(GetCrossRouteArchiveNote(tasks), sanitizedNote, sizeof(sanitizedNote));
    UIRuntime_DrawWrappedText(assets, sanitizedNote, Rectangle{routePanel.x + 188.0f * scale, routePanel.y + 94.0f * scale, routePanel.width - 204.0f * scale, 28.0f * scale}, 13.0f * scale, 14.0f * scale, Color{172, 194, 214, 255});
}
