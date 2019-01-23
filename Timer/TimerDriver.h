#ifndef TIMERDRIVER_H_
#define TIMERDRIVER_H_ 

#ifdef __cplusplus
extern "C" {
#endif

#include "util.h"

/* Initialize timer */
STATUS tmrDriver_Initialize(void);

/* Get time since start up in milliseconds */
STATUS tmrDriver_GetTimeMs(uint32* ellapsedTimeMs);

/* Delay timer for time milliseconds */
STATUS tmrDriver_DelayMs(uint32 timeMs);

#ifdef __cplusplus
}
#endif

#endif /* TIMERDRIVER_H_ */
