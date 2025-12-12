#include "mc/event.h"
#include "mc/list.h"
#include "mc/common.h"
#include "assert.h"

void mc_event_init(mc_event_t *event)
{
    MC_ASSERT(event != NULL);
    mc_list_init(&event->listeners);
}

void mc_callback_init(mc_callback_t *cb, mc_callback_func_t func, void *context)
{
    MC_ASSERT(cb != NULL);
    cb->node.next = NULL;
    cb->node.prev = NULL;
    cb->func = func;
    cb->context = context;
    cb->is_initialized = 1;
}

void mc_event_register(mc_event_t *event, mc_callback_t *cb)
{
    MC_ASSERT(event != NULL);
    MC_ASSERT(cb != NULL);
    MC_ASSERT(cb->is_initialized == 1);
    mc_list_append(&event->listeners, &cb->node);
}

void mc_event_unregister(mc_event_t *event, mc_callback_t *cb)
{
    MC_ASSERT(event != NULL);
    MC_ASSERT(cb != NULL);
    MC_ASSERT(cb->is_initialized == 1);
    mc_list_remove(&event->listeners, &cb->node);
}

void mc_event_trigger(mc_event_t *event, void *event_data)
{
    MC_ASSERT(event != NULL);
    mc_node_t *curr;
    mc_node_t *storage;

    // USE SAFE ITERATION:
    // This allows a callback to call 'mc_event_unregister' on itself
    // without breaking the loop logic.
    MC_LIST_FOR_EACH_SAFE(curr, storage, &event->listeners)
    {

        // Recover the callback struct from the generic node
        mc_callback_t *cb = MC_LIST_ENTRY(curr, mc_callback_t, node);

        // Execute
        if (cb->func)
        {
            cb->func(cb->context, event_data);
        }
    }
}
