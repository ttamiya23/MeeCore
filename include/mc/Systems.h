S_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "util.h"

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
STATUS sys_GetState(uint16 sysId, int8* state);

/* Function to set a parameter of a system to some value */
STATUS sys_SetParameter(uint16 sysId, uint8 parameterNum, float parameter);

/* Function to get the parameter of a system */
STATUS sys_GetParameter(uint16 sysId, uint8 parameterNum, float* parameter);

/* Function to get the value of a system */
STATUS sys_GetValue(uint16 sysId, uint8 valueNum, float* value);

/* Function to get the description of a system */
STATUS sys_GetHelp(uint16 sysId, const char** help);

/* Function to update all systems. Should be called periodically */
STATUS sys_Update(void);

#ifdef __cplusplus
}
#endif

#endif /* SYSTEMS_H_ */
