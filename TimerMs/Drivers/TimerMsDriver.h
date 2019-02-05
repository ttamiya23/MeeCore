#ifndef TIMERMSDRIVER_H_
#define TIMERMSDRIVER_H_ 

#ifdef __cplusplus
extern "C" {
#endif

#include "util.h"

/* Initialize timer */
STATUS tmsDriver_Initialize(void);

/* Get time since start up in milliseconds */
STATUS tmsDriver_GetTimeMs(uint32* ellapsedTimeMs);

/* Delay timer for time milliseconds */
STATUS tmsDriver_DelayMs(uint32 timeMs);

#ifdef __cplusplus
}
#endif

#endif /* TIMERMSDRIVER_H_ */
