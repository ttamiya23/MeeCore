#include "mc/event.h"
#include "mc/utils.h"

void mc_event_register(mc_event_t *event, mc_callback_t *cb)
{
    MC_ASSERT(event != NULL);
    MC_ASSERT(cb != NULL);
    mc_list_append(&event->listeners, &cb->node);
}

void mc_event_unregister(mc_event_t *event, mc_callback_t *cb)
{
    MC_ASSERT(event != NULL);
    MC_ASSERT(cb != NULL);
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
            cb->func(cb->ctx, event_data);
        }
    }
}
