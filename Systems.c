#include "Systems.h"
#include "SystemDefinition.h"
#include "SystemSettings.h"
#include "stdlib.h"

/* List of all systems */
static struct System systemList[SYSTEM_NUM];

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

/* Macro to check if system with sysId is initialized */
#define IS_INITIALIZED(sysId) initializedSystems & ((sysId - 1) << 1)

/* Macro to add system with sysId as initialized */
#define ADD_INITIALIZED(sysId) initializedSystems | ((sysId - 1) << 1)

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
    STATUS ret;

    uint8 err = sysId >= SYSTEM_NUM                                          ||
                isNewSys && (sysId == RESERVED_SYS || IS_INITIALIZED(sysId)) ||
                !isNewSys && !IS_INITIALIZED(sysId);

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
        ADD_INITIALIZED(sysId);
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
    ret = currSystemPtr -> setState(currSystemPtr, state);

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

/* Function to set a target of a system to some value */
STATUS sys_SetTarget(uint16 sysId, uint8 targetNum, float value)
{
    STATUS ret = CheckSystemId(sysId, FALSE);
    if (ret != SUCCESS)
        return ret;

    System* currSystemPtr = systemList + sysId;
    ret = currSystemPtr -> setTarget(currSystemPtr, targetNum, value);

    return ret;
}

/* Function to get the target of a system */
STATUS sys_GetTarget(uint16 sysId, uint8 targetNum, float* valuePtr)
{
    STATUS ret = CheckSystemId(sysId, FALSE);
    if (ret != SUCCESS || valuePtr == NULL || targetNum >= TARGET_NUM)
        return ret;

    *valuePtr = systemList[sysId].targets[targetNum];

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
