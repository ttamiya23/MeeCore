#ifndef IODRIVER_H_
#define IODRIVER_H_ 

#ifdef __cplusplus
extern "C" {
#endif

#include "util.h"

/* Initialize IO Driver */
STATUS ioDriver_Initialize(void);

/* Write a single char to output */
STATUS ioDriver_WriteChar(const char ch);

/* Get a single char. Returns ERROR if empty */
STATUS ioDriver_GetChar(char* ch);

#ifdef __cplusplus
}
#endif

#endif /* IODRIVER_H_ */
