#include "Callback.h"
#include "Iterator/Iterator.h"
#include "Debug/Debug.h"
#include "assert.h"

/* Request a callback handle */
STATUS cb_CreateCallbackHandle(CallbackHandle* cbHandle)
{
    STATUS ret = ERROR;

    assert(cbHandle != NULL);

    ret = iter_CreateIterator((Iterator**)cbHandle);
    if (ret == SUCCESS)
        dbg_LogInformation("Created new cbHandle[%i]", *cbHandle);

    return ret;
}

/* Add new callback to cbHandle */
STATUS cb_AddCallback(CallbackHandle cbHandle, CallbackFunction cbFunction)
{
    STATUS ret = ERROR;

    assert(cbHandle != NULL);
    assert(cbFunction != NULL);

    ret = iter_AddNode((Iterator*)cbHandle, cbFunction);
    if (ret == SUCCESS)
        dbg_LogInformation("Added cbFunction to cbHandle[%i]", cbHandle);

    return ret;
}

/* Call all callbacks */
STATUS cb_CallCallbacks(CallbackHandle cbHandle, void* args, uint8 argsLength)
{
    assert(cbHandle != NULL);
 
    dbg_LogTrace("Calling all cbFunction for cbHandle[%i]", cbHandle);

    // Call all CallbackFunctions while nodes exist
    Node* currNode;
    CallbackFunction func;
    iter_GetStart((Iterator*)cbHandle, &currNode);
    while(currNode)
    {
        dbg_LogTrace("Calling cbFunction for cbHandle[%i]", cbHandle);

        iter_GetData(currNode, (void**)&func);
        if (func != NULL)
            func(args, argsLength);
        iter_GetNext(currNode, &currNode);
    }

    dbg_LogTrace("Finished all cbFunction for callback handle[%i]",
            cbHandle);

    // Regardless of outcome of callbacks, this method returns success
    return SUCCESS;
}

/* Delete callback. If callback does not belong to cbHandle, returns ERROR */
STATUS cb_DeleteCallback(CallbackHandle cbHandle, CallbackFunction cbFunction)
{
    STATUS ret = ERROR;
    assert(cbHandle != NULL);
    assert(cbFunction != NULL);

    dbg_LogInformation("Deleting cbFunction for cbHandle[%i]", cbHandle);
    Node* node;
    ret = iter_FindNode((Iterator*)cbHandle, cbFunction, &node);
    if (ret != SUCCESS)
        return ret;

    ret = iter_DeleteNode((Iterator*)cbHandle, &node);
    return ret;
}

/* Delete cbHandle and sets cbHandle to NULL */
STATUS cb_DeleteCallbackHandle(CallbackHandle* cbHandle)
{
    STATUS ret = ERROR;
    assert(cbHandle != NULL);
    assert(*cbHandle != NULL);

    dbg_LogInformation("Deleting cbHandle[%i]", *cbHandle);

    ret = iter_DeleteIterator((Iterator**)cbHandle);
    return ret;
}
