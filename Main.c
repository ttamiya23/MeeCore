#include <stdio.h>
#include "Callback\Callback.h"
#include "Debug\Debug.h"

STATUS callback1(void* args, uint8 argsLength)
{
    char* name = args;
    printf("Hello, %s\n", name);
    dbg_LogInformation("Something is wrong %i\n", 3);
}

STATUS callback2(void* args, uint8 argsLength)
{
    char* name = args;
    printf("Goodbye, %s\n", name);
}

STATUS callback3(void* args, uint8 argsLength)
{
    char* name = args;
    printf("Konnichiwa, %s\n", name);
}

STATUS callback4(void* args, uint8 argsLength)
{
    char* name = args;
    printf("Sayonara, %s\n", name);
}

int main()
{
    CallbackHandle handle1;
    cb_CreateCallbackHandle(&handle1);
    cb_AddCallback(handle1, callback1);
    cb_AddCallback(handle1, callback2);
    cb_AddCallback(handle1, callback3);
    cb_AddCallback(handle1, callback4);

    cb_CallCallbacks(handle1, "Tomoki", 0);
    cb_DeleteCallback(handle1, callback2);
    cb_CallCallbacks(handle1, "Emily", 0);
    cb_DeleteCallback(handle1, callback4);
    cb_CallCallbacks(handle1, "Metan", 0);
    cb_DeleteCallback(handle1, callback1);
    cb_CallCallbacks(handle1, "Chirin", 0);
}
