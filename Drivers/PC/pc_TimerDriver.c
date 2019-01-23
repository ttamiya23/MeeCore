#include "util.h"
#include "Timer/TimerDriver.h"
#include "Debug/Debug.h"
#include <time.h>
#include <stdio.h>

/* Begin time and current time */
static clock_t beginTime, currentTime;

/* Initialize timer */
STATUS tmrDriver_Initialize(void)
{
    STATUS ret = ERROR;

    beginTime = clock();

    ret = SUCCESS;
    return ret;
}

/* Get time since start up in milliseconds */
STATUS tmrDriver_GetTimeMs(uint32* ellapsedTimeMs)
{
    STATUS ret = ERROR;

    currentTime = clock();
    *ellapsedTimeMs = (uint64)(1000*(currentTime - beginTime) / CLOCKS_PER_SEC);

    ret = SUCCESS;
    return ret;
}

/* Delay timer for time milliseconds */
STATUS tmrDriver_DelayMs(uint32 timeMs)
{
    uint32 time;
    tmrDriver_GetTimeMs(&time);
    uint32 end = (uint32)time + timeMs;
    while(time < end)
    {
        tmrDriver_GetTimeMs(&time);
    }
}
