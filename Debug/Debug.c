#include "Debug.h"

#ifdef DEBUG_MODE

#include <stdarg.h>
#include "IO/IO.h"

/* Private global variable of current debug state */
static uint8 currentDebugState = DEBUG_DEFAULT_STATE;

/* Private global variable of current log level */
static LogLevel currentLogLevel = DEBUG_DEFAULT_LOG_LEVEL;

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

    switch (level)
    {
        case LOG_LEVEL_TRACE:
            ret = io_WriteString("TRACE: ");
            break;
        case LOG_LEVEL_DEBUG:
            ret = io_WriteString("DEBUG: ");
            break;
        case LOG_LEVEL_INFORMATION:
            ret = io_WriteString("INFORMATION: ");
            break;
        case LOG_LEVEL_WARNING:
            ret = io_WriteString("WARNING: ");
            break;
        case LOG_LEVEL_ERROR:
            ret = io_WriteString("ERROR: ");
            break;
        default:
            ret = io_WriteString("CRITICAL: ");
            break;
    }

    if (ret != ERROR)
    {
        ret = io_VariableWriteString(format, args) ||
              io_WriteString("\r\n");
    }

    va_end(args);

    return ret;
}

#endif /* DEBUG_MODE */
