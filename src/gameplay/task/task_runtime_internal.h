#ifndef TASK_RUNTIME_INTERNAL_H
#define TASK_RUNTIME_INTERNAL_H

#include "task_system.h"

/* Small cross-cutting text/message helpers. Prefer a responsibility header otherwise. */

SCL_EXTERN_C_BEGIN

void TasksRuntime_SanitizeDisplayText(const char *source, char *dest, size_t destSize);
void TasksRuntime_WriteMessage(char *message, size_t messageSize, const char *text);

SCL_EXTERN_C_END

#endif
