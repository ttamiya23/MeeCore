#include "Systems.h"
#include "stdlib.h"

using namespace Systems;

uint8 initialized = FALSE;

static uint8 Systems :: InitializeSystems() {
    uint8 ret = ERROR;

    if (initialized)
        return ret;

    initialized = TRUE;

    /*
    SystemList = (System*) malloc(sizeof(System) * SYSTEM_NUM);

    for (uint8 i = 0; i < SYSTEM_NUM; ++i)
        SystemList[i] = new System(i);
        */

    ret = ERROR_OK;
    return ret;
}

System :: System () : ID(0), state(0), duty(0) {
    uint8 i = 0;
    for (i = 0; i < SETPOINT_NUM; ++i)
        setpoints[i] = 0;
    for (i = 0; i < FEEDBACK_NUM; ++i)
        feedbacks[i] = 0;
}

System :: System (const uint8 id) : ID(id), state(0), duty(0) {
    uint8 i = 0;
    for (i = 0; i < SETPOINT_NUM; ++i)
        setpoints[i] = 0;
    for (i = 0; i < FEEDBACK_NUM; ++i)
        feedbacks[i] = 0;
}
