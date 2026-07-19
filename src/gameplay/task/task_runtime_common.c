#include "task_runtime_internal.h"

#include "localization.h"

#include <stdio.h>
#include <string.h>

typedef struct DisplayTextReplacement {
    const char *from;
    const char *to;
} DisplayTextReplacement;

static void AppendDisplayText(char *dest, size_t destSize, size_t *destLength, const char *text) {
    size_t copyLength;

    if (dest == NULL || destSize == 0 || destLength == NULL || text == NULL) {
        return;
    }

    if (*destLength >= destSize - 1) {
        dest[destSize - 1] = '\0';
        return;
    }

    copyLength = strlen(text);
    if (copyLength > destSize - 1 - *destLength) {
        copyLength = destSize - 1 - *destLength;
    }

    memcpy(dest + *destLength, text, copyLength);
    *destLength += copyLength;
    dest[*destLength] = '\0';
}

void TasksRuntime_SanitizeDisplayText(const char *source, char *dest, size_t destSize) {
    static const DisplayTextReplacement kReplacements[] = {
        {"West W5", "West final pass"},
        {"West W4", "West fourth pass"},
        {"West W3", "West third pass"},
        {"West W2", "West second pass"},
        {"West W1", "West opening pass"},
        {"South S5", "South final pass"},
        {"South S4", "South fourth pass"},
        {"South S3", "South third pass"},
        {"South S2", "South second pass"},
        {"South S1", "South opening pass"},
        {"Cross X3", "Shared ending context"},
        {"Cross X2", "Strategy synthesis"},
        {"Cross X1", "Shared insight"},
        {"X3-informed", "fully informed"},
        {"X3 context", "Shared ending context"},
        {"X2 context", "Strategy context"},
        {"X1 trace alignment", "shared trace alignment"},
        {"X2 strategy rewrite", "strategy synthesis"},
        {"X2 rewrite", "strategy synthesis"},
        {"X3 complete", "Shared ending context complete"},
        {"X2 complete", "Strategy synthesis complete"},
        {"X1 ready", "Shared insight ready"},
        {"X2 ready", "Strategy synthesis ready"},
        {"X3 ready", "Shared ending context ready"},
        {"W5", "final west pass"},
        {"W4", "fourth west pass"},
        {"W3", "third west pass"},
        {"W2", "second west pass"},
        {"W1", "opening west pass"},
        {"S5", "final south pass"},
        {"S4", "fourth south pass"},
        {"S3", "third south pass"},
        {"S2", "second south pass"},
        {"S1", "opening south pass"},
        {"X3", "shared ending context"},
        {"X2", "strategy synthesis"},
        {"X1", "shared insight"}
    };
    size_t sourceIndex;
    size_t destLength;
    size_t replacementIndex;

    if (dest == NULL || destSize == 0) {
        return;
    }

    if (source == NULL) {
        dest[0] = '\0';
        return;
    }

    dest[0] = '\0';
    sourceIndex = 0;
    destLength = 0;
    while (source[sourceIndex] != '\0' && destLength < destSize - 1) {
        int matched;

        matched = 0;
        for (replacementIndex = 0; replacementIndex < sizeof(kReplacements) / sizeof(kReplacements[0]); replacementIndex++) {
            const DisplayTextReplacement *replacement;
            size_t fromLength;

            replacement = &kReplacements[replacementIndex];
            fromLength = strlen(replacement->from);
            if (strncmp(source + sourceIndex, replacement->from, fromLength) != 0) {
                continue;
            }

            AppendDisplayText(dest, destSize, &destLength, replacement->to);
            sourceIndex += fromLength;
            matched = 1;
            break;
        }

        if (!matched) {
            dest[destLength++] = source[sourceIndex++];
            dest[destLength] = '\0';
        }
    }
}


void TasksRuntime_WriteMessage(char *message, size_t messageSize, const char *text) {
    if (message != NULL && messageSize > 0) {
        TasksRuntime_SanitizeDisplayText(Loc_Translate(text), message, messageSize);
    }
}
