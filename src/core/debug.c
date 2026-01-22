#include <stdarg.h>
#include "mc/debug.h"
#include "mc/utils.h"

static const mc_stream_t *debug_stream = NULL;
static mc_log_level_t current_level = MC_LOG_LEVEL_TRACE;

// Helper: convert log level into string
static const char *get_level_str(mc_log_level_t level)
{
    switch (level)
    {
    case MC_LOG_LEVEL_TRACE:
        return "[TRC]";
    case MC_LOG_LEVEL_DEBUG:
        return "[DBG]";
    case MC_LOG_LEVEL_INFORMATION:
        return "[INF]";
    case MC_LOG_LEVEL_WARNING:
        return "[WRN]";
    case MC_LOG_LEVEL_ERROR:
        return "[ERR]";
    default:
        return "[CRT]";
    }
}

// Set the stream where logs go (e.g., &console)
void mc_debug_init(const mc_stream_t *stream)
{
    debug_stream = stream;
}

// Change filter at runtime
void mc_debug_set_level(mc_log_level_t level)
{
    current_level = level;
}

mc_status_t _mc_log(mc_log_level_t log_level, const char *file, int line,
                    const char *format, ...)
{
    mc_status_t ret = MC_ERROR;
    if (!debug_stream || debug_stream->state->is_initialized != MC_INITIALIZED)
    {
        return ret;
    }
    if (log_level < current_level)
    {
        return ret;
    }
    va_list args;
    va_start(args, format);

    // Eg: "[DBG] debug.c:52 Create example message\r\n"
    ret = mc_stream_printf(debug_stream, "%s %s:%d: ",
                           get_level_str(log_level), file, line);
    mc_stream_vprintf(debug_stream, format, args);
    mc_stream_printf(debug_stream, "\r\n");

    va_end(args);

    return ret;
}
