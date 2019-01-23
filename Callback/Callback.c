#include "Callback.h"
#include "Iterator/Iterator.h"

/* Request a callback handle. If no more space available, cbHandle is set to
 * NULL and returns ERROR */
STATUS cb_CreateCallbackHandle(CallbackHandle* cbHandle)
{
    STATUS ret = ERROR;
    ret = iter_CreateIterator((Iterator**)cbHandle);
    return ret;
}

/* Add new callback to cbHandle. If no more space available or cbHandle is
 * NULL, returns ERROR */
STATUS cb_AddCallback(CallbackHandle cbHandle, CallbackFunction cbFunction)
{
    STATUS ret = ERROR;
    if (cbHandle == NULL || cbFunction == NULL)
        return ret;

    ret = iter_AddNode((Iterator*)cbHandle, cbFunction);
    return ret;
}

/* Call all callbacks. If cbHandle is NULL, returns ERROR */
STATUS cb_CallCallbacks(CallbackHandle cbHandle, void* args, uint8 argsLength)
{
    if (cbHandle == NULL)
        return ERROR;

    // Call all CallbackFunctions while nodes exist
    Node* currNode;
    CallbackFunction func;
    iter_GetStart((Iterator*)cbHandle, &currNode);
    while(currNode)
    {
        iter_GetData(currNode, (void**)&func);
        if (func != NULL)
            func(args, argsLength);
        iter_GetNext(currNode, &currNode);
    }

    // Regardless of outcome of callbacks, this method returns success
    return SUCCESS;
}

/* Delete callbackFunction. If callback is NULL or does not belong to cbHandle,
 * returns ERROR */
STATUS cb_DeleteCallback(CallbackHandle cbHandle, CallbackFunction cbFunction)
{
    STATUS ret = ERROR;
    if (cbHandle == NULL || cbFunction == NULL)
        return ret;

    Node* node;
    ret = iter_FindNode((Iterator*)cbHandle, cbFunction, &node);
    if (ret != SUCCESS)
        return ret;

    ret = iter_DeleteNode((Iterator*)cbHandle, &node);
    return ret;
}

/* Delete cbHandle and sets cbHandle to NULL. If cbHandle is NULL, returns
 * ERROR */
STATUS cb_DeleteCallbackHandle(CallbackHandle* cbHandle)
{
    STATUS ret = ERROR;
    if (cbHandle == NULL)
        return ret;

    ret = iter_DeleteIterator((Iterator**)cbHandle);
    return ret;
}
