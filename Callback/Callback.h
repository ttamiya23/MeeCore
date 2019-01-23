#ifndef CALLBACK_H_
#define CALLBACK_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "util.h"

/* Definition of Callback handle */
typedef uint8* CallbackHandle;

/* Callback function: takes in void pointer pointing to arguments and length of
 * arguments */
typedef STATUS (*CallbackFunction)(void* args, uint8 argsLength);

/* Request a callback handle. If no more space available, cbHandle is set to
 * NULL and returns ERROR */
STATUS cb_CreateCallbackHandle(CallbackHandle* cbHandle);

/* Add new callback to cbHandle. If no more space available or cbHandle is
 * NULL, returns ERROR */
STATUS cb_AddCallback(CallbackHandle cbHandle, CallbackFunction cbFunction);

/* Call all callbacks. If cbHandle is NULL, returns ERROR */
STATUS cb_CallCallbacks(CallbackHandle cbHandle, void* args, uint8 argsLength);

/* Delete callbackFunction. If callback is NULL or does not belong to cbHandle,
 * returns ERROR */
STATUS cb_DeleteCallback(CallbackHandle cbHandle, CallbackFunction cbFunction);

/* Delete cbHandle and sets cbHandle to NULL. If cbHandle is NULL, returns
 * ERROR */
STATUS cb_DeleteCallbackHandle(CallbackHandle* cbHandle);

#ifdef __cplusplus
}
#endif

#endif /* CALLBACK_H_ */

