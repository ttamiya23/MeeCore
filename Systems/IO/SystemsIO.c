#include "SystemsIO.h"
#include "Systems/Systems.h"
#include "Systems/SystemsSettings.h"
#include "IO/IO.h"
#include "Timer/Timer.h"
#include <stdlib.h>

/* Query the system and output result */
STATUS sysio_Query(uint16 sysId)
{
    // Example query
    // ID:3
    // State:1
    // Parameter[0]:0
    // Parameter[1]:42
    // Parameter[2]:523
    // Parameter[3]:1
    // Value[0]:4529
    // Value[1]:0
    // Value[2]:0
    // Value[3]:99
    STATUS ret = ERROR;
    uint8 index;
    int8 state;
    float val;

    // Check if system exists
    ret = sys_GetState(sysId, &state);
    if (ret != SUCCESS)
    {
        io_WriteString("System %i does not exist\r\n", sysId);
        return ret;
    }

    io_WriteString("ID:%i\r\n", sysId);
    io_WriteString("State:%i\r\n", state);
    for (index = 0; index < PARAMETER_NUM; ++index)
    {
        sys_GetParameter(sysId, index, &val);
        io_WriteString("Parameter[%i]:%f\r\n", index, val);
    }
    for (index = 0; index < VALUE_NUM; ++index)
    {
        sys_GetValue(sysId, index, &val);
        io_WriteString("Value[%i]:%f\r\n", index, val);
    }
    return ret;
}

/* Get help from system and output result */
STATUS sysio_Help(uint16 sysId)
{
    STATUS ret = ERROR;

    // Get help string and write to output
    const char** help;
    ret = sys_GetHelp(sysId, help);
    if (ret != SUCCESS)
        return ret;
    ret = io_WriteString(*help);

    return ret;
}

/* Parse input command and make appropriate system call */
STATUS sysio_ParseCommand(char* command)
{
    STATUS ret = ERROR;

    if (!command)
        return ret;

    char* start = command;
    char** end = &command;
    int sysNum = strtol(command, end, 10);

    if (start == *end || sysNum < 0 || sysNum > SYSTEM_NUM)
    {
        ret = sysio_Help(RESERVED_SYS);
        return ret;
    }
}
