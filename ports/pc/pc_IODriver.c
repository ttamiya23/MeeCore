#ifdef PC

#include "IODriver.h"
#include <stdio.h>
#include <conio.h>
#include "assert.h"

/* Initialize IO Driver */
STATUS ioDriver_Initialize(void)
{
    return SUCCESS;
}

/* Write a single char to output */
STATUS ioDriver_WriteChar(const char ch)
{
    STATUS ret = ERROR;
    char success = putchar(ch);
    if (success != EOF)
        ret = SUCCESS;
    return ret;
}

/* Get a single char. Returns ERROR if empty */
STATUS ioDriver_GetChar(char* ch)
{
    STATUS ret = ERROR;
    assert(ch != NULL);

    if (_kbhit())
    {
        *ch = _getch();
        ret = SUCCESS;
    }

    return ret;
}

#endif /* PC */
