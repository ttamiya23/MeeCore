#include "IO.h"
#include "IOSettings.h"
#include "IODriver.h"
#include "assert.h"
#include <stdio.h>

/* Output buffer for storing messages to send */
static char outputBuffer[IO_OUTPUT_BUFFER_SIZE];

/* Input buffer for storing new messages */
static char inputBuffer[IO_INPUT_BUFFER_SIZE];

/* Current length of input buffer */
static uint16 inputLength = 0;

/* Handle for message received callback */
static CallbackHandle msgReceivedHandle;

/* If initialized */
static uint8 initialized = FALSE;

/* Character array of end-of-message characters */
static const char eomCharacters[] = IO_EOM_CHARACTERS;

/* Helper to check if ch is an EOM char */
static uint8 IsEOMChar(char);

/* Initialize IO */
STATUS io_Initialize(void)
{
    STATUS ret = ERROR;
    if (initialized)
        return ret;

    ret = cb_CreateCallbackHandle(&msgReceivedHandle);
    assert(ret == SUCCESS);

    ret = ioDriver_Initialize();
    assert(ret == SUCCESS);

    initialized = TRUE;
    return ret;
}

/* Write string to output like you would with printf() */
STATUS io_WriteString(const char* format, ...)
{
    STATUS ret = ERROR;
    if (!initialized)
        return ret;

    va_list args;
    va_start(args, format);

    ret = io_VariableWriteString(format, args);

    va_end(args);
    return ret;
}

/* Write string to output like you would with vprintf() */
STATUS io_VariableWriteString(const char* format, va_list args)
{
    STATUS ret = ERROR;
    if (!initialized)
        return ret;

    int success = vsnprintf(outputBuffer, IO_OUTPUT_BUFFER_SIZE, format, args);
    if (success < 0)
        return ret;

    // Write each char until NULL charater
    for (uint32 index = 0; index < IO_OUTPUT_BUFFER_SIZE; ++index)
    {
        if (outputBuffer[index] == '\0')
            break;
        ret = ioDriver_WriteChar(outputBuffer[index]);
        if (ret != SUCCESS)
            break;
    }

    return ret;
}

/* Write char to output like you would with putc() */
STATUS io_WriteChar(const char ch)
{
    if (!initialized)
        return ERROR;
    return ioDriver_WriteChar(ch);
}

/* Write n chars. Will send null characters as well */
STATUS io_WriteCharArray(const char* charArray, uint32 n)
{
    STATUS ret = ERROR;
    if (!initialized)
        return ret;

    // Write each char including NULL charater
    for (uint32 index = 0; index < n; ++index)
    {
        ret = ioDriver_WriteChar(charArray[index]);
        if (ret != SUCCESS)
            break;
    }

    return ret;
}

/* Add callback function to message received event */
STATUS io_AddMessageReceivedCallback(CallbackFunction callback)
{
    if (!initialized)
        return ERROR;

    return cb_AddCallback(msgReceivedHandle, callback);
}

/* Delete callback function to message received event */
STATUS io_DeleteMessageReceivedCallback(CallbackFunction callback)
{
    if (!initialized)
        return ERROR;

    return cb_DeleteCallback(msgReceivedHandle, callback);
}

/* Read input. If message is ready, will trigger message received event */
STATUS io_ReadInput()
{
    if (!initialized)
        return ERROR;

    // Initialize variables
    char newChar, overflowed = FALSE;
    uint16 index, endOfInputBuffer;
    static char* newMsgBuffer[IO_NEW_MESSAGES_BUFFER_SIZE];
    uint8 newMsgCount = 0;

    // First message must start from start of input buffer
    newMsgBuffer[0] = inputBuffer;

    // Rare case: Last io_ReadInput() overflowed with a EOM char
    if (inputLength > 0 && inputBuffer[inputLength-1] == '\0')
    {
        newMsgCount++;
        newMsgBuffer[newMsgCount] = inputBuffer + inputLength;
    }

    // Read input until no more new characters
    while (ioDriver_GetChar(&newChar) == SUCCESS)
    {
        // Check if new character is an EOM character
        if (IsEOMChar(newChar))
            newChar = '\0';

        // Overflow check AFTER checking IsEOMChar() but BEFORE adding to new
        // message buffer
        if (inputLength >= IO_INPUT_BUFFER_SIZE)
        {
            overflowed = TRUE;
            break;
        }

        // NOW add new message
        if (newChar == '\0')
        {
            // Add pointer to character AFTER NULL as the NEXT new message
            // (But be sure to boundary check)
            newMsgCount++;
            if (newMsgCount >= IO_NEW_MESSAGES_BUFFER_SIZE)
                break;
            newMsgBuffer[newMsgCount] = inputBuffer + inputLength + 1;
        }

        // Add new character to buffer
        inputBuffer[inputLength] = newChar;
        inputLength++;
    }

    // If no new messages, return error
    if (newMsgCount == 0)
    {
        // No new message AND overflow? Message too long, dispose it
        if (overflowed)
            inputLength = 0;
        return ERROR;
    }

    // Call all callbacks
    cb_CallCallbacks(msgReceivedHandle, newMsgBuffer, newMsgCount);

    // Move extra characters to beginning of input buffer. These characters
    // starts from the beginning of the last new message
    if (newMsgCount < IO_NEW_MESSAGES_BUFFER_SIZE)
    {
        // Get start of next message
        index = newMsgBuffer[newMsgCount] - inputBuffer;
    }
    // If too many new messages are found, iteratively go through each
    // character to find the next NULL, starting from last new message
    else
    {
        index = newMsgBuffer[IO_NEW_MESSAGES_BUFFER_SIZE-1] - inputBuffer;
        while(inputBuffer[index] != '\0')
            index++;
        index++;    // Want character AFTER NULL
    }

    // Start moving the characters over
    endOfInputBuffer = overflowed ? IO_INPUT_BUFFER_SIZE : inputLength;
    for (inputLength = 0; index < endOfInputBuffer; ++index, ++inputLength)
        inputBuffer[inputLength] = inputBuffer[index];

    // Add newChar if overflowed
    if (overflowed)
    {
        inputBuffer[inputLength] = newChar;
        inputLength++;
    }

    return SUCCESS;
}

/* Helper to check if ch is an EOM char */
static uint8 IsEOMChar(char ch)
{
    static const uint8 eomLength = sizeof(eomCharacters);
    for (uint8 index = 0; index < eomLength; ++index)
        if (ch == eomCharacters[index])
            return TRUE;
    return FALSE;
}
