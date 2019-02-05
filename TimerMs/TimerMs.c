#include "TimerMs.h"
#include "Drivers/TimerMsDriver.h"
#include "Debug/Debug.h"
#include "assert.h"

static uint8 initialized = 0;

/* Get time since start up in milliseconds */
STATUS tms_Initialize(void)
{
    STATUS ret = ERROR;
    if (initialized)
        return ret;

    dbg_LogInformation("Initializing millisecond timer");

    ret = tmsDriver_Initialize();
    if (ret == SUCCESS)
        initialized = TRUE;
    return ret;
}

/* Get time since start up in milliseconds */
STATUS tms_GetTimeMs(uint32* ellapsedTimeMs)
{
    assert(initialized);
    assert(ellapsedTimeMs != NULL);

    return tmsDriver_GetTimeMs(ellapsedTimeMs);
}

/* Delay timer for time milliseconds */
STATUS tms_DelayMs(uint32 timeMs)
{
    assert(initialized);
    return tmsDriver_DelayMs(timeMs);
}
