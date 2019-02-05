#ifdef PC

#include "util.h"
#include "TimerMs/TimerMsDriver.h"
#include "Debug/Debug.h"
#include <time.h>
#include <stdio.h>

/* Begin time and current time */
static clock_t beginTime, currentTime;

/* Initialize timer */
STATUS tmsDriver_Initialize(void)
{
    beginTime = clock();
    return SUCCESS;
}

/* Get time since start up in milliseconds */
STATUS tmsDriver_GetTimeMs(uint32* ellapsedTimeMs)
{
    currentTime = clock();
    *ellapsedTimeMs = (uint64)(1000*(currentTime - beginTime) / CLOCKS_PER_SEC);
    return SUCCESS;
}

/* Delay timer for time milliseconds */
STATUS tmsDriver_DelayMs(uint32 timeMs)
{
    uint32 time;
    tmsDriver_GetTimeMs(&time);
    uint32 end = (uint32)time + timeMs;
    while(time < end)
    {
        tmsDriver_GetTimeMs(&time);
    }
    return SUCCESS;
}

#endif
