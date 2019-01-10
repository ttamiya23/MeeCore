#ifndef SYSTEMS_H_
#define SYSTEMS_H_

#include "util.h"
#include "SystemSettings.h"

/* Definition of System struct */
struct System;

/* Function to add a new system with some ID number. Takes in a systemID and a
 * pointer to a function that takes in a pointer to a system and initializes
 * it */
STATUS sys_AddSystem(uint16 sysId, STATUS (*initSys)(System*));

/* Function to set a system to some state */
STATUS sys_SetState(uint16 sysId, int8 state);

/* Function to get the state of a system */
STATUS sys_GetState(uint16 sysId, int8* statePtr);

/* Function to set a target of a system to some value */
STATUS sys_SetTarget(uint16 sysId, uint8 targetNum, float value);

/* Function to get the target of a system */
STATUS sys_GetTarget(uint16 sysId, uint8 targetNum, float* valuePtr);

/* Function to get the value of a system */
STATUS sys_GetValue(uint16 sysId, uint8 valueNum, float* valuePtr);

#endif /* SYSTEMS_H_ */
