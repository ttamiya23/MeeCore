#ifndef MC_DEBUG_H_
#define MC_DEBUG_H_

#ifdef __cplusplus
extern "C"
{
#endif

#include "mc/common.h"
#include "mc/io.h"

#ifndef MC_DEBUG_LOG_LEVEL
#define MC_DEBUG_LOG_LEVEL 1

#endif

    /* Different levels of debug log */
    typedef enum mc_log_level_t
    {
        MC_LOG_LEVEL_NONE = 0,
        MC_LOG_LEVEL_TRACE = 1,
        MC_LOG_LEVEL_DEBUG = 2,
        MC_LOG_LEVEL_INFORMATION = 3,
        MC_LOG_LEVEL_WARNING = 4,
        MC_LOG_LEVEL_ERROR = 5,
        MC_LOG_LEVEL_CRITICAL = 6,
    } mc_log_level_t;

    // Set the IO stream where logs go.
    void mc_debug_init(mc_io_t *io);

    // Change filter at runtime.
    void mc_debug_set_level(mc_log_level_t level);

    /* Private method for actually writing log message. */
    mc_status_t _mc_log(mc_log_level_t log_level, const char *file, int line,
                        const char *format, ...);

/* Define Log function */
#if MC_DEBUG_LOG_LEVEL > 0 /* MC_LOG_LEVEL_NONE */
#define MC_LOG(log_level, ...) _mc_log(log_level, __FILENAME__, __LINE__, \
                                       __VA_ARGS__)
#else
#define MC_LOG(...)
#endif

/* Define CRITICAL Log function */
#if MC_DEBUG_LOG_LEVEL <= 6 /* MC_LOG_LEVEL_CRITICAL */
#define MC_LOG_CRITICAL(...) MC_LOG(MC_LOG_LEVEL_CRITICAL, __VA_ARGS__)
#else
#define MC_LOG_CRITICAL(...)
#endif

/* Define ERROR Log function */
#if MC_DEBUG_LOG_LEVEL <= 5 /* MC_LOG_LEVEL_ERROR */
#define MC_LOG_ERROR(...) MC_LOG(MC_LOG_LEVEL_ERROR, __VA_ARGS__)
#else
#define MC_LOG_ERROR(...)
#endif

/* Define WARNING Log function */
#if MC_DEBUG_LOG_LEVEL <= 4 /* MC_LOG_LEVEL_WARNING */
#define MC_LOG_WARNING(...) MC_LOG(MC_LOG_LEVEL_WARNING, __VA_ARGS__)
#else
#define MC_LOG_WARNING(...)
#endif

/* Define INFORMATION Log function */
#if MC_DEBUG_LOG_LEVEL <= 3 /* MC_LOG_LEVEL_INFORMATION */
#define MC_LOG_INFORMATION(...) MC_LOG(MC_LOG_LEVEL_INFORMATION, __VA_ARGS__)
#else
#define MC_LOG_INFORMATION(...)
#endif

/* Define DEBUG Log function */
#if MC_DEBUG_LOG_LEVEL <= 2 /* MC_LOG_LEVEL_DEBUG */
#define MC_LOG_DEBUG(...) MC_LOG(MC_LOG_LEVEL_DEBUG, __VA_ARGS__)
#else
#define MC_LOG_DEBUG(...)
#endif

/* Define TRACE Log function */
#if MC_DEBUG_LOG_LEVEL <= 1 /* MC_LOG_LEVEL_TRACE */
#define MC_LOG_TRACE(...) MC_LOG(MC_LOG_LEVEL_TRACE, __VA_ARGS__)
#else
#define MC_LOG_TRACE(...)
#endif

#ifdef __cplusplus
}
#endif

#endif /* MC_DEBUG_H_ */
