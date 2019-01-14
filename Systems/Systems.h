#ifndef SYSTEMS_H_
#define SYSTEMS_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "util.h"
#include "SystemSettings.h"

/* Definition of System struct */
typedef struct System System;

/* Initialization function: takes in a system pointer and initializes it */
typedef STATUS (*InitializeSystem)(System* system);

/* Function to add a new system with some ID number. Takes in a systemID and
 * a InitializeSystem function */
STATUS sys_AddSystem(uint16 sysId, InitializeSystem initSys);

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

#ifdef __cplusplus
}
#endif

#endif /* SYSTEMS_H_ */
