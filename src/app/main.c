#include "game_manager.h"
#include "raylib.h"

static int MaxInt(int a, int b) {
    return a > b ? a : b;
}

static int MinInt(int a, int b) {
    return a < b ? a : b;
}

static void ConfigureWindowForMonitor(void) {
    int monitor;
    int monitorWidth;
    int monitorHeight;
    int windowWidth;
    int windowHeight;
    int offsetX;
    int offsetY;

    monitor = GetCurrentMonitor();
    monitorWidth = GetMonitorWidth(monitor);
    monitorHeight = GetMonitorHeight(monitor);

    windowWidth = MaxInt((int)(monitorWidth * WINDOW_MONITOR_FILL), WINDOW_MIN_WIDTH);
    windowHeight = MaxInt((int)(monitorHeight * WINDOW_MONITOR_FILL), WINDOW_MIN_HEIGHT);
    windowWidth = MinInt(windowWidth, MaxInt(monitorWidth - 80, 320));
    windowHeight = MinInt(windowHeight, MaxInt(monitorHeight - 80, 240));
    windowWidth = MaxInt(windowWidth, 320);
    windowHeight = MaxInt(windowHeight, 240);

    SetWindowMinSize(WINDOW_MIN_WIDTH, WINDOW_MIN_HEIGHT);
    SetWindowSize(windowWidth, windowHeight);

    offsetX = (monitorWidth - windowWidth) / 2;
    offsetY = (monitorHeight - windowHeight) / 2;
    SetWindowPosition(offsetX, offsetY);
}

static void EnableDefaultFullscreen(void) {
    int monitor;
    Vector2 monitorPosition;
    int monitorWidth;
    int monitorHeight;

    monitor = GetCurrentMonitor();
    monitorPosition = GetMonitorPosition(monitor);
    monitorWidth = GetMonitorWidth(monitor);
    monitorHeight = GetMonitorHeight(monitor);
    if (IsWindowFullscreen()) {
        ToggleFullscreen();
    }
    ClearWindowState(FLAG_FULLSCREEN_MODE);
    SetWindowPosition((int)monitorPosition.x, (int)monitorPosition.y);
    SetWindowSize(monitorWidth, monitorHeight);
    if (!IsWindowState(FLAG_BORDERLESS_WINDOWED_MODE)) {
        ToggleBorderlessWindowed();
    }
}

int main(void) {
    Game game;

    SetConfigFlags(FLAG_WINDOW_RESIZABLE | FLAG_VSYNC_HINT);
    InitWindow(WINDOW_WIDTH, WINDOW_HEIGHT, WINDOW_TITLE);
    ConfigureWindowForMonitor();
    EnableDefaultFullscreen();
    SetTargetFPS(TARGET_FPS);
    SetExitKey(KEY_NULL);

    Game_Init(&game);

    while (!WindowShouldClose() && !game.flow.requestClose) {
        Game_Update(&game, GetFrameTime());
        Game_Draw(&game);
    }

    Game_Shutdown(&game);
    CloseWindow();
    return 0;
}
