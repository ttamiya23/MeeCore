#include "Systems.h"
#include "SystemsSettings.h"
#include "SystemDefinition.h"
#include "Debug/Debug.h"
#include "stdlib.h"
#include "assert.h"

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
STATUS InitializeReservedSystem(void);

/* Private function to check if sysId is a valid ID */
STATUS CheckSystemId(uint16 sysId, uint8 isNewSys);

/* Private Set State function for reserved (does nothing) */
STATUS ReservedSetState(System* sys, int8 state);

/* Private Set Parameter function for reserved (does nothing) */
STATUS ReservedSetParameter(System* sys, uint8 parameterNum, float parameter);

/* Private update function for reserved (does nothing) */
STATUS ReservedUpdate(System* sys);

/* Private function to initialize reserved */
STATUS InitializeReservedSystem(void)
{
    dbg_LogInformation("Initializing reserved system[%i]", RESERVED_SYS);

    System* reservedSystem = systemList + RESERVED_SYS;
    reservedSystem->id = RESERVED_SYS;
    reservedSystem->state = ON;
    reservedSystem->help = systemsHelp;
    reservedSystem->setState = ReservedSetState;
    reservedSystem->setParameter = ReservedSetParameter;
    reservedSystem->update = ReservedUpdate;
    SET_BIT(initializedSystems, RESERVED_SYS);

    return SUCCESS;
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
    // Check if RESERVED_SYS is initialized
    if (!IS_BIT_SET(initializedSystems, RESERVED_SYS))
        InitializeReservedSystem();
        
    uint8 err = sysId >= SYSTEM_NUM                                          ||
                isNewSys && sysId == RESERVED_SYS                            ||
                isNewSys && IS_BIT_SET(initializedSystems, sysId)            ||
                !isNewSys && !IS_BIT_SET(initializedSystems, sysId);
    if (err)
    {
        dbg_LogDebug("System[%i] is not a valid system", sysId);
        return ERROR;
    }
    return SUCCESS;
}

/* Private Set State function for reserved (does nothing) */
STATUS ReservedSetState(System* sys, int8 state)
{
    return SUCCESS;
}

/* Private Set Parameter function for reserved (does nothing) */
STATUS ReservedSetParameter(System* sys, uint8 parameterNum, float parameter)
{
    return SUCCESS;
}

/* Private Set update function for reserved (does nothing) */
STATUS ReservedUpdate(System* sys)
{
    return SUCCESS;
}

/* 
 * Function to add a new system with some ID number. Takes in a systemID and a
 * pointer to a function that takes in a pointer to a system and initializes
 * it
 */
STATUS sys_AddSystem(uint16 sysId, STATUS (*initSys)(System*))
{
    dbg_LogInformation("Adding system %i", sysId);

    STATUS ret = CheckSystemId(sysId, TRUE);
    assert(ret == SUCCESS);

    ret = initSys(systemList + sysId);
    assert(ret == SUCCESS);

    (systemList + sysId) -> id = sysId;
    SET_BIT(initializedSystems, sysId);

    return ret;
}

/* Function to set a system to some state */
STATUS sys_SetState(uint16 sysId, int8 state)
{
    dbg_LogDebug("Setting system[%i] state to %i", sysId, state);

    STATUS ret = CheckSystemId(sysId, FALSE);
    if (ret != SUCCESS)
        return ret;

    System* currSystemPtr = systemList + sysId;
    ret = currSystemPtr->setState(currSystemPtr, state);

    return ret;
}

/* Function to get the state of a system */
STATUS sys_GetState(uint16 sysId, int8* state)
{
    dbg_LogTrace("Getting system[%i] state", sysId);

    assert(state != NULL);

    STATUS ret = CheckSystemId(sysId, FALSE);
    if (ret != SUCCESS)
        return ret;

    *state = systemList[sysId].state;

    ret = SUCCESS;
    return ret;
}

/* Function to set a parameter of a system to some value */
STATUS sys_SetParameter(uint16 sysId, uint8 parameterNum, float parameter)
{
    dbg_LogDebug("Setting system[%i] parameter[%i] to %f", sysId, parameterNum,
        parameter);

    STATUS ret = CheckSystemId(sysId, FALSE);

    if (ret != SUCCESS)
    {
        return ERROR;
    }
    else if (parameterNum >= PARAMETER_NUM)
    {
        dbg_LogDebug("Parameter[%i] is not a valid parameter", parameterNum);
        return ERROR;
    }

    System* currSystemPtr = systemList + sysId;
    ret = currSystemPtr->setParameter(currSystemPtr, parameterNum, parameter);

    return ret;
}

/* Function to get the parameter of a system */
STATUS sys_GetParameter(uint16 sysId, uint8 parameterNum, float* parameter)
{
    dbg_LogTrace("Getting system[%i] parameter[%i]", sysId, parameterNum);

    assert(parameter != NULL);

    STATUS ret = CheckSystemId(sysId, FALSE);
    if (ret != SUCCESS)
    {
        return ERROR;
    }
    else if (parameterNum >= PARAMETER_NUM)
    {
        dbg_LogDebug("Parameter[%i] is not a valid parameter", parameterNum);
        return ERROR;
    }

    *parameter = systemList[sysId].parameters[parameterNum];

    ret = SUCCESS;
    return ret;
}

/* Function to get the value of a system */
STATUS sys_GetValue(uint16 sysId, uint8 valueNum, float* value)
{
    dbg_LogTrace("Getting system[%i] value[%i]", sysId, valueNum);

    assert(value != NULL);

    STATUS ret = CheckSystemId(sysId, FALSE);
    if (ret != SUCCESS || valueNum >= VALUE_NUM)
        return ERROR;

    *value = systemList[sysId].values[valueNum];

    ret = SUCCESS;
    return ret;
}

/* Function to get the description of a system */
STATUS sys_GetHelp(uint16 sysId, const char** help)
{
    dbg_LogTrace("Getting system[%i] help", sysId);

    assert(help != NULL);

    STATUS ret = CheckSystemId(sysId, FALSE);
    if (ret != SUCCESS)
        return ret;

    *help = systemList[sysId].help;

    ret = SUCCESS;
    return ret;
}

/* Function to update all systems. Should be called periodically */
STATUS sys_Update(void)
{
    STATUS ret = ERROR;

    dbg_LogTrace("Updating each system");

    for (uint16 sysId = 0; sysId < SYSTEM_NUM; ++sysId)
    {
        ret = CheckSystemId(sysId, FALSE);
        if (ret != SUCCESS)
            continue;

        dbg_LogTrace("Updating system[%i]", sysId);
        (systemList+sysId)->update(systemList+sysId);
    }

    dbg_LogTrace("Updating finished");

    ret = SUCCESS;
    return ret;
}
