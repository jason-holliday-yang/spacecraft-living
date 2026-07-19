#include "game_manager_internal.h"

#include "localization.h"
#include "task_runtime_internal.h"

#include <cstdio>
#include <cstdlib>

static void Game_PopOldestMessage(Game *game) {
    GameMessageNode *oldHead;

    if (game == NULL || game->ui.messageHistory.head == NULL) {
        return;
    }

    oldHead = game->ui.messageHistory.head;
    game->ui.messageHistory.head = oldHead->next;
    if (game->ui.messageHistory.tail == oldHead) {
        game->ui.messageHistory.tail = game->ui.messageHistory.head;
    }
    if (game->ui.messageHistory.count > 0) {
        game->ui.messageHistory.count -= 1;
    }
    std::free(oldHead);
}

static void Game_AppendMessageHistory(Game *game, const char *text, float duration) {
    GameMessageNode *node;

    if (game == NULL || text == NULL || text[0] == '\0') {
        return;
    }

    node = (GameMessageNode *)std::malloc(sizeof(GameMessageNode));
    if (node == NULL) {
        return;
    }

    std::snprintf(node->text, sizeof(node->text), "%s", text);
    node->postedAt = game->runtime.elapsedSeconds;
    node->duration = duration;
    node->next = NULL;

    if (game->ui.messageHistory.tail != NULL) {
        game->ui.messageHistory.tail->next = node;
    } else {
        game->ui.messageHistory.head = node;
    }
    game->ui.messageHistory.tail = node;
    game->ui.messageHistory.count += 1;

    while (game->ui.messageHistory.count > GAME_MESSAGE_HISTORY_LIMIT) {
        Game_PopOldestMessage(game);
    }
}

void Game_PostMessage(Game *game, const char *text, float duration) {
    const char *displayText;

    if (game == NULL || text == NULL) {
        return;
    }

    displayText = Loc_Translate(text);
    TasksRuntime_SanitizeDisplayText(displayText, game->ui.hudMessage.text, sizeof(game->ui.hudMessage.text));
    game->ui.hudMessage.timer = duration + 2.0f;
    Game_AppendMessageHistory(game, game->ui.hudMessage.text, duration);
}

void Game_ClearMessage(Game *game) {
    if (game == NULL) {
        return;
    }

    game->ui.hudMessage.text[0] = '\0';
    game->ui.hudMessage.timer = 0.0f;
}

void Game_ClearMessageHistory(Game *game) {
    if (game == NULL) {
        return;
    }

    while (game->ui.messageHistory.head != NULL) {
        Game_PopOldestMessage(game);
    }
    game->ui.messageHistory.tail = NULL;
    game->ui.messageHistory.count = 0;
}
