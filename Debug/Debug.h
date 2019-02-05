#ifndef DEBUG_H_
#define DEBUG_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "util.h"
#include "DebugSettings.h"

/* Different levels of debug log */
#define LOG_LEVEL_NONE        0
#define LOG_LEVEL_TRACE       1
#define LOG_LEVEL_DEBUG       2
#define LOG_LEVEL_INFORMATION 3
#define LOG_LEVEL_WARNING     4
#define LOG_LEVEL_ERROR       5
#define LOG_LEVEL_CRITICAL    6

/* Define Log function */
#if DEBUG_LOG_LEVEL > LOG_LEVEL_NONE

/* Log a debug message with a specified log level */
STATUS dbg_Log(uint8 logLevel, const char* format, ...);

#else
#define dbg_Log(...)
#endif

/* Define CRITICAL Log function */
#if DEBUG_LOG_LEVEL <= LOG_LEVEL_CRITICAL
#define dbg_LogCritical(args...) dbg_Log(LOG_LEVEL_CRITICAL, args)
#else
#define dbg_LogCritical(args...)
#endif

/* Define ERROR Log function */
#if DEBUG_LOG_LEVEL <= LOG_LEVEL_ERROR
#define dbg_LogError(args...) dbg_Log(LOG_LEVEL_ERROR, args)
#else 
#define dbg_LogError(args...)
#endif

/* Define WARNING Log function */
#if DEBUG_LOG_LEVEL <= LOG_LEVEL_WARNING
#define dbg_LogWarning(args...) dbg_Log(LOG_LEVEL_WARNING, args)
#else
#define dbg_LogWarning(args...)
#endif

/* Define INFORMATION Log function */
#if DEBUG_LOG_LEVEL <= LOG_LEVEL_INFORMATION
#define dbg_LogInformation(args...) dbg_Log(LOG_LEVEL_INFORMATION, args)
#else
#define dbg_LogInformation(args...)
#endif

/* Define DEBUG Log function */
#if DEBUG_LOG_LEVEL <= LOG_LEVEL_DEBUG
#define dbg_LogDebug(args...) dbg_Log(LOG_LEVEL_DEBUG, args)
#else
#define dbg_LogDebug(args...)
#endif

/* Define TRACE Log function */
#if DEBUG_LOG_LEVEL <= LOG_LEVEL_TRACE
#define dbg_LogTrace(args...) dbg_Log(LOG_LEVEL_TRACE, args)
#else
#define dbg_LogTrace(args...)
#endif

#ifdef __cplusplus
}
#endif

#endif /* DEBUG_H_ */
