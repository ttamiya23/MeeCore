#ifndef SYSTEMS_H_
#define SYSTEMS_H_ 

#include "util.h"
#include "SystemSettings.h"

/* System struct */
typedef struct System
{
    const uint8 ID;
    int8 state;
    float targets[SETPOINT_NUM];
    float* values[FEEDBACK_NUM];

    STATUS (*setState(struct System* sys, int8 state));
    STATUS (*setTarget(struct System* sys, uint8 num, float value));
} System;

/* List of all systems */
extern System SystemList[SYSTEM_NUM];

/* Initialize all systems */
STATUS Systems_Initialize();

/* Set state for some system */
STATUS Systems_SetState(uint8 sysnum, int8 value);

/* Set target for some system */
STATUS Systems_SetTarget(uint8 sysnum, uint8 num, float value);

#endif /* SYSTEMS_H_ */
