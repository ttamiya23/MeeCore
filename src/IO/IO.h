#ifndef IO_H_
#define IO_H_ 

#ifdef __cplusplus
extern "C" {
#endif

#include "util.h"
#include "Callback/Callback.h"
#include <stdarg.h>

/* Type of event args for message received event */
typedef char** IOMessageReceivedEventArgs;

/* Initialize IO */
STATUS io_Initialize(void);

/* Write string to output like you would with printf() */
STATUS io_WriteString(const char* format, ...);

/* Write string to output like you would with vprintf() */
STATUS io_VariableWriteString(const char* format, va_list args);

/* Write char to output like you would with putc() */
STATUS io_WriteChar(const char ch);

/* Write n chars. Will send null characters as well */
STATUS io_WriteCharArray(const char* charArray, uint32 n);

/* Add callback function to message received event */
STATUS io_AddMessageReceivedCallback(CallbackFunction callback);

/* Delete callback function to message received event */
STATUS io_DeleteMessageReceivedCallback(CallbackFunction callback);

/* Read input. If message is ready, will trigger message received event */
STATUS io_ReadInput();

#ifdef __cplusplus
}
#endif

#endif /* IO_H_ */
