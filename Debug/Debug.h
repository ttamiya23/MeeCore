#ifndef DEBUG_H_
#define DEBUG_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "util.h"
#include "DebugSettings.h"

/* Different levels of debug log */
typedef enum
{
    LOG_LEVEL_TRACE,
    LOG_LEVEL_DEBUG,
    LOG_LEVEL_INFORMATION,
    LOG_LEVEL_WARNING,
    LOG_LEVEL_ERROR,
    LOG_LEVEL_CRITICAL,
} LogLevel;

#ifdef DEBUG_MODE

/* Returns 0 if debug is off and 1 otherwise */
uint8 dbg_GetDebugState(void);

/* Set debug messages on or off. 0 for off, otherwise on */
uint8 dbg_SetDebug(uint8 state);

/* Get current log level */
LogLevel dbg_GetLogLevel(void);

/* Set current log level */
LogLevel dbg_SetLogLevel(LogLevel level);

/* Log a debug message with a specified log level */
STATUS dbg_Log(LogLevel level, const char* format, ...);

#else   /* Define everything to '0;' */

#define dbg_GetDebugState(...) 0;
#define dbg_SetDebug(...) 0;
#define dbg_GetLogLevel(...) 0;
#define dbg_SetLogLevel(...) 0;
#define dbg_Log(...) 0;

#endif /* DEBUG_MODE */

#define dbg_LogTrace(args...) dbg_Log(LOG_LEVEL_TRACE, args)
#define dbg_LogDebug(args...) dbg_Log(LOG_LEVEL_DEBUG, args)
#define dbg_LogInformation(args...) dbg_Log(LOG_LEVEL_INFORMATION, args)
#define dbg_LogWarning(args...) dbg_Log(LOG_LEVEL_WARNING, args)
#define dbg_LogError(args...) dbg_Log(LOG_LEVEL_ERROR, args)
#define dbg_LogCritical(args...) dbg_Log(LOG_LEVEL_CRITICAL, args)

#ifdef __cplusplus
}
#endif

#endif /* DEBUG_H_ */
