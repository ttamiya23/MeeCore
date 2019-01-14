#ifndef DEBUGSETTINGS_H_
#define DEBUGSETTINGS_H_

#ifdef __cplusplus
extern "C" {
#endif

/* Comment this line to disable debug mode at compile time */
#define DEBUG_MODE

/* The default state the debugger should be in at start up */
#define DEBUG_DEFAULT_STATE ON

/* The default log level the debugger should be in at start up */
#define DEBUG_DEFAULT_LOG_LEVEL LOG_LEVEL_DEBUG

/* Max buffer size for debug messages */
#define DEBUG_BUFFER_SIZE 256

#ifdef __cplusplus
}
#endif

#endif /* DEBUGSETTINGS_H_ */
