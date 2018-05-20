#include "Systems.h"
#include "stdlib.h"

/* List of all systems */
System SystemList[SYSTEM_NUM];

/* Default set functions */
STATUS setState_Default(System* sys, int8 state)
{
    return ERROR;
}

STATUS setTarget_Default(System* sys, uint8 num, float value)
{
    return ERROR;
}

/* Helper to initialize a system */
void SystemInit(System* sys, uint8 id)
{
    uint8 ret = ERROR;

    sys->ID = id;
    sys->state = OFF;
    uint8 index;
    for (index = 0; index < SETPOINT_NUM; ++index)
        (sys->setpoints)[index] = 0;
    for (index = 0; index < FEEDBACK_NUM; ++index)
        (sys->feedback)[index] = 0;

    sys->setState = setState_Default;
    sys->setTarget = setTarget_Default;

    ret = SUCCESS;
    return ret;
}

/* Initialize all systems */
uint8 initialized = FALSE;
STATUS Systems_Initialize()
{
    uint8 ret = ERROR;

    if (initialized)
        return ret;

    for (uint8 index = 0; index < SYSTEM_NUM; ++index)
        SystemInit(SystemList + index, index)

    initialized = TRUE;
    ret = SUCCESS;
    return ret;
}

/* Set state for some system */
STATUS Systems_SetState(uint8 sysnum, int8 value)
{
    return SystemList[sysnum].setState(SystemList + sysnum, value);
}

/* Set target for some system */
STATUS Systems_SetTarget(uint8 sysnum, uint8 num, float value);
{
    return SystemList[sysnum].setTarget(SystemList + sysnum, num, value);
}
