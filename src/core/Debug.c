#include "Debug.h"

#if DEBUG_LOG_LEVEL > LOG_LEVEL_NONE

#include <stdarg.h>
#include "IO.h"

/* Log a debug message with a specified log level */
STATUS dbg_Log(uint8 level, const char* format, ...)
{
    STATUS ret = ERROR;

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

#endif
