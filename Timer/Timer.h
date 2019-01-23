#ifndef TIMER_H_
#define TIMER_H_ 

#ifdef __cplusplus
extern "C" {
#endif

#include "util.h"

/* Initialize timer */
STATUS tmr_Initialize(void);

/* Get time since start up in milliseconds */
STATUS tmr_GetTimeMs(uint32* ellapsedTimeMs);

/* Delay timer for time milliseconds */
STATUS tmr_DelayMs(uint32 timeMs);

#ifdef __cplusplus
}
#endif

#endif /* TIMER_H_ */
