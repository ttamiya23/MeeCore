#include "Debug.h"

#ifdef DEBUG_MODE

#include <stdarg.h>
#include <stdio.h>

/* Private global variable of current debug state */
static uint8 currentDebugState = DEBUG_DEFAULT_STATE;

/* Private global variable of current log level */
static LogLevel currentLogLevel = DEBUG_DEFAULT_LOG_LEVEL;

/* Buffer for printing messages */
static char debugBuffer[DEBUG_BUFFER_SIZE];

/* Returns 0 if debug is off and 1 otherwise */
uint8 dbg_GetDebugState(void)
{
    return currentDebugState != OFF;
}

/* Set debug messages on or off. 0 for off, otherwise on */
uint8 dbg_SetDebug(uint8 state)
{
    currentDebugState = state;
    return state;
}

/* Get current log level */
LogLevel dbg_GetLogLevel(void)
{
    return currentLogLevel;
}

/* Set current log level */
LogLevel dbg_SetLogLevel(LogLevel level)
{
    currentLogLevel = level;
    return level;
}

/* Log a debug message with a specified log level */
STATUS dbg_Log(LogLevel level, const char* format, ...)
{
    STATUS ret = ERROR;

    /* Ignore any lower level messages */
    if (level < currentLogLevel)
        return ret;

    va_list args;
    va_start(args, format);

    int success = vsnprintf(debugBuffer, DEBUG_BUFFER_SIZE, format, args);
    if (success > 0)
        ret = printf(debugBuffer);

    va_end(args);

    return ret;
}

#endif /* DEBUG_MODE */
