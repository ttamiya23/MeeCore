#include "Timer.h"
#include "TimerDriver.h"
#include "assert.h"

static uint8 initialized = 0;

/* Get time since start up in milliseconds */
STATUS tmr_Initialize(void)
{
    STATUS ret = ERROR;
    if (initialized)
        return ret;

    ret = tmrDriver_Initialize();
    if (ret == SUCCESS)
        initialized = TRUE;
    return ret;
}

/* Get time since start up in milliseconds */
STATUS tmr_GetTimeMs(uint32* ellapsedTimeMs)
{
    STATUS ret = ERROR;
    if (!initialized)
        return ret;

    ret = tmrDriver_GetTimeMs(ellapsedTimeMs);
    return ret;
}

/* Delay timer for time milliseconds */
STATUS tmr_DelayMs(uint32 timeMs)
{
    STATUS ret = ERROR;
    if (!initialized)
        return ret;

    ret = tmrDriver_DelayMs(timeMs);
    return ret;
}
