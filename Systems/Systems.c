#include "Systems.h"
#include "SystemsSettings.h"
#include "SystemDefinition.h"
#include "stdlib.h"

/* List of all systems */
static System systemList[SYSTEM_NUM];

/* Variable to store which systems are initialized */
static
#if SYSTEM_NUM <= 8
uint8
#elif SYSTEM_NUM <= 16
uint16
#elif SYSTEM_NUM <= 32
uint32
#else
uint64
#endif
initializedSystems = 0;

/* General help */
const char *systemsHelp = \
    "SYSTEM HELP\r\n"
    "ID: ID# of system\r\n"
    "STATE: State of system. 1 for ON, 0 for OFF (typically)\r\n"
    "HELP: Description of system\r\n"
    "PARAMETER: Parameters affecting system's behavior\r\n"
    "VALUE: Feedback values of system\r\n";

/* Private function to initialize reserved */
STATUS InitializeReservedSystem(void)
{
    STATUS ret = ERROR;

    System* reservedSystem = systemList + RESERVED_SYS;
    reservedSystem->id = RESERVED_SYS;
    reservedSystem->state = ON;
    reservedSystem->help = systemsHelp;
    SET_BIT(initializedSystems, RESERVED_SYS);

    ret = SUCCESS;
    return ret;
}

/* 
 * Private function to check if sysId is a valid ID
 * Error exists if any of these are true:
 *      1) sysId is greater than or equal to allowed maximum number
 *      2) is new system and sysId is equal to RESERVED_SYS
 *      3) is new system and sysId is already initialized
 *      4) is not new system and sysId is not initialized
 */
STATUS CheckSystemId(uint16 sysId, uint8 isNewSys)
{
    STATUS ret = ERROR;;

    // Check if RESERVED_SYS is initialized
    if (IS_BIT_SET(initializedSystems, RESERVED_SYS))
        InitializeReservedSystem();
        
    uint8 err = sysId >= SYSTEM_NUM                                          ||
                isNewSys && sysId == RESERVED_SYS                            ||
                isNewSys && IS_BIT_SET(initializedSystems, sysId)            ||
                !isNewSys && !IS_BIT_SET(initializedSystems, sysId);

    ret = err ? ERROR : SUCCESS;
    return ret;
}

/* 
 * Function to add a new system with some ID number. Takes in a systemID and a
 * pointer to a function that takes in a pointer to a system and initializes
 * it
 */
STATUS sys_AddSystem(uint16 sysId, STATUS (*initSys)(System*))
{
    STATUS ret = CheckSystemId(sysId, TRUE);
    if (ret != SUCCESS)
        return ret;

    ret = initSys(systemList + sysId);
    if (ret == SUCCESS)
    {
        (systemList + sysId) -> id = sysId;
        SET_BIT(initializedSystems, sysId);
    }

    return ret;
}

/* Function to set a system to some state */
STATUS sys_SetState(uint16 sysId, int8 state)
{
    STATUS ret = CheckSystemId(sysId, FALSE);
    if (ret != SUCCESS)
        return ret;

    System* currSystemPtr = systemList + sysId;
    ret = currSystemPtr->setState(currSystemPtr, state);

    return ret;
}

/* Function to get the state of a system */
STATUS sys_GetState(uint16 sysId, int8* statePtr)
{
    STATUS ret = CheckSystemId(sysId, FALSE);
    if (ret != SUCCESS || statePtr == NULL)
        return ret;

    *statePtr = systemList[sysId].state;

    ret = SUCCESS;
    return ret;
}

/* Function to set a parameter of a system to some value */
STATUS sys_SetParameter(uint16 sysId, uint8 parameterNum, float parameter)
{
    STATUS ret = CheckSystemId(sysId, FALSE);
    if (ret != SUCCESS || parameterNum >= PARAMETER_NUM)
        return ret;

    System* currSystemPtr = systemList + sysId;
    ret = currSystemPtr->setParameter(currSystemPtr, parameterNum, parameter);

    return ret;
}

/* Function to get the parameter of a system */
STATUS sys_GetParameter(uint16 sysId, uint8 parameterNum, float* parameter)
{
    STATUS ret = CheckSystemId(sysId, FALSE);
    if (ret != SUCCESS || parameter == NULL || parameterNum >= PARAMETER_NUM)
        return ret;

    *parameter = systemList[sysId].parameters[parameterNum];

    ret = SUCCESS;
    return ret;
}

/* Function to get the value of a system */
STATUS sys_GetValue(uint16 sysId, uint8 valueNum, float* valuePtr)
{
    STATUS ret = CheckSystemId(sysId, FALSE);
    if (ret != SUCCESS || valuePtr == NULL || valueNum >= VALUE_NUM)
        return ret;

    *valuePtr = systemList[sysId].values[valueNum];

    ret = SUCCESS;
    return ret;
}

/* Function to get the description of a system */
STATUS sys_GetHelp(uint16 sysId, const char** help)
{
    STATUS ret = CheckSystemId(sysId, FALSE);
    if (ret != SUCCESS || help == NULL)
        return ret;

    *help = systemList[sysId].help;

    ret = SUCCESS;
    return ret;
}
