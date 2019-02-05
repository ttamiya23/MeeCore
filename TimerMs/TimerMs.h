#ifndef TIMERMS_H_
#define TIMERMS_H_ 

#ifdef __cplusplus
extern "C" {
#endif

#include "util.h"

/* Initialize timer */
STATUS tms_Initialize(void);

/* Get time since start up in milliseconds */
STATUS tms_GetTimeMs(uint32* ellapsedTimeMs);

/* Delay timer for time milliseconds */
STATUS tms_DelayMs(uint32 timeMs);

#ifdef __cplusplus
}
#endif

#endif /* TIMERMS_H_ */
