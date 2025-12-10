#include "SystemsIO.h"
#include "SystemsIOSettings.h"
#include "Systems.h"
#include "SystemsSettings.h"
#include "IO.h"
#include "assert.h"
#include <string.h>
#include <stdlib.h>

/* Command function */
typedef STATUS (*CommandFunction)(uint16 sysId, char** args, uint8 argsLength);

/* Command struct for parsing the command */
#pragma pack(1)
typedef struct Command
{
    const char* name;
    CommandFunction function;
} Command;

/* Private function for getting/setting state */
static STATUS StateCommand(uint16 sysId, char** args, uint8 argsLength);

/* Private function for getting/setting parameter */
static STATUS ParameterCommand(uint16 sysId, char** args, uint8 argsLength);

/* Private function for getting value */
static STATUS ValueCommand(uint16 sysId, char** args, uint8 argsLength);

/* Private function for getting help */
static STATUS HelpCommand(uint16 sysId, char** args, uint8 argsLength);

/* Private function for getting system query */
static STATUS QueryCommand(uint16 sysId, char** args, uint8 argsLength);

const Command commandList[] =
{
    {"state", StateCommand},
    {"parameter", ParameterCommand},
    {"value", ValueCommand},
    {"help", HelpCommand},
    {"query", QueryCommand}
};

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
    const char* help;
    ret = sys_GetHelp(sysId, &help);
    if (ret == SUCCESS)
        io_WriteString("%s\r\n", help);
    else
        io_WriteString("HELP NOT FOUND\r\n");

    return ret;
}

/* Parse input command and make appropriate system call */
STATUS sysio_ParseCommand(char* commandString)
{
    STATUS ret = ERROR;

    assert(commandString != NULL);

    // Get system ID
    char *commandName, *args[SYSIO_ARGS_BUFFER_SIZE], *start, *end;
    int sysId = strtol(commandString, &end, 10);
    if (commandString == end || end == NULL || sysId < 0 || sysId > SYSTEM_NUM)
        goto PrintHelp;

    // Get command name
    commandName = strtok(end, SYSIO_DELIMITERS);
    if (commandName == NULL)
        goto PrintHelp;

    // Find the corresponding command
    static const uint8 commandNum = sizeof(commandList) / sizeof(Command);
    CommandFunction function;
    uint8 index;
    for (index = 0; index < commandNum; ++index)
    {

        if (strcmp(commandList[index].name, commandName) == 0)
        {
            function = commandList[index].function;
            break;
        }
    }
    if (index == commandNum)    // Couldn't find corresponding function
        goto PrintHelp;
    
    // Get rest of the arguments
    uint8 argsLength;
    start = strtok(NULL, SYSIO_DELIMITERS);
    for (argsLength = 0; start != NULL &&
            argsLength < SYSIO_ARGS_BUFFER_SIZE; ++argsLength)
    {
        args[argsLength] = start;
        start = strtok(NULL, SYSIO_DELIMITERS);
    }

    ret = function(sysId, args, argsLength);
    return ret;

PrintHelp:
    ret = sysio_Help(sysId);
    if (ret != SUCCESS)
        sysio_Help(RESERVED_SYS);
    return ERROR;
}

/* Callback function for message received event */
STATUS sysio_MessageReceivedCallback(void* args, uint8 argsLength)
{
    IOMessageReceivedEventArgs messages = args;

    assert(messages != NULL && *messages != NULL);

    for (uint8 index = 0; index < argsLength; ++index)
    {
        io_WriteString("%s\r\n", messages[index]);
        sysio_ParseCommand(messages[index]);
        io_WriteString("\r\n", messages[index]);
    }

    return SUCCESS;
}

/* Private function for getting/setting state */
static STATUS StateCommand(uint16 sysId, char** args, uint8 argsLength)
{
    STATUS ret = ERROR;
    assert(args != NULL);

    int8 state;
    if (argsLength > 0)
    {
        char* end;
        state = strtol(args[0], &end, 10);

        // GetState if failed to parse args, SetState if it worked
        if (args[0] == end)
            ret = sys_GetState(sysId, &state);
        else
            ret = sys_SetState(sysId, state);
    }
    else
    {
        ret = sys_GetState(sysId, &state);
    }

    io_WriteString("ID:%i\r\nState:%i\r\nError:0x%08x\r\n", sysId, state, ret);
    return ret;
}

/* Private function for getting/setting parameter */
static STATUS ParameterCommand(uint16 sysId, char** args, uint8 argsLength)
{
    STATUS ret = ERROR;

    char* end;
    float parameter;
    uint8 paramNum = 0;

    // Get param num
    if (argsLength > 0)
        paramNum = strtoul(args[0], &end, 10);

    if (argsLength > 1)
    {
        // Get next parameter
        parameter = strtof(args[1], &end);

        // GetParameter if failed to parse, SetParameter if it worked
        if (args[1] == end)
            ret = sys_GetParameter(sysId, paramNum, &parameter);
        else
            ret = sys_SetParameter(sysId, paramNum, parameter);
    }
    else
    {
        ret = sys_GetParameter(sysId, paramNum, &parameter);
    }

    io_WriteString("ID:%i\r\nParameter[%i]:%f\r\nError:0x%08x\r\n", sysId,
            paramNum, parameter, ret);

    return ret;
}

/* Private function for getting value */
static STATUS ValueCommand(uint16 sysId, char** args, uint8 argsLength)
{
    char* end;
    uint8 valueNum = 0;
    if (argsLength > 0)
        valueNum = strtoul(args[0], &end, 10);

    // Get value
    float value;
    sys_GetValue(sysId, valueNum, &value);

    io_WriteString("ID:%i\r\nValue[%i]:%f\r\n", sysId, valueNum, value);

    return SUCCESS;
}

/* Private function for getting help */
static STATUS HelpCommand(uint16 sysId, char** args, uint8 argsLength)
{
    return sysio_Help(sysId);
}

/* Private function for getting system query */
static STATUS QueryCommand(uint16 sysId, char** args, uint8 argsLength)
{
    return sysio_Query(sysId);
}
