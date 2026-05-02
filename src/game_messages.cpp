#include "game_manager_internal.h"

#include "localization.h"
#include "task_runtime_internal.h"

#include <cstdio>
#include <cstdlib>

static void Game_PopOldestMessage(Game *game) {
    GameMessageNode *oldHead;

    if (game == NULL || game->messageHistory.head == NULL) {
        return;
    }

    oldHead = game->messageHistory.head;
    game->messageHistory.head = oldHead->next;
    if (game->messageHistory.tail == oldHead) {
        game->messageHistory.tail = game->messageHistory.head;
    }
    if (game->messageHistory.count > 0) {
        game->messageHistory.count -= 1;
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
    node->postedAt = game->elapsedSeconds;
    node->duration = duration;
    node->next = NULL;

    if (game->messageHistory.tail != NULL) {
        game->messageHistory.tail->next = node;
    } else {
        game->messageHistory.head = node;
    }
    game->messageHistory.tail = node;
    game->messageHistory.count += 1;

    while (game->messageHistory.count > GAME_MESSAGE_HISTORY_LIMIT) {
        Game_PopOldestMessage(game);
    }
}

void Game_PostMessage(Game *game, const char *text, float duration) {
    const char *displayText;

    if (game == NULL || text == NULL) {
        return;
    }

    displayText = Loc_Translate(text);
    TasksRuntime_SanitizeDisplayText(displayText, game->hudMessage.text, sizeof(game->hudMessage.text));
    game->hudMessage.timer = duration + 2.0f;
    Game_AppendMessageHistory(game, game->hudMessage.text, duration);
}

void Game_ClearMessage(Game *game) {
    if (game == NULL) {
        return;
    }

    game->hudMessage.text[0] = '\0';
    game->hudMessage.timer = 0.0f;
}

void Game_ClearMessageHistory(Game *game) {
    if (game == NULL) {
        return;
    }

    while (game->messageHistory.head != NULL) {
        Game_PopOldestMessage(game);
    }
    game->messageHistory.tail = NULL;
    game->messageHistory.count = 0;
}
