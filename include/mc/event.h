#pragma once

#ifdef __cplusplus
extern "C"
{
#endif

#include "mc/list.h"

// Macro for defining event and callback. Users should always use this.
#define MC_DEFINE_EVENT(NAME) static mc_event_t NAME = {0}

#define MC_DEFINE_CALLBACK(NAME, FUNC, CTX) \
    static mc_callback_t NAME = {           \
        .node = {0},                        \
        .func = FUNC,                       \
        .ctx = &CTX}

    /* Callback function: takes in void pointer pointing to context and event data.
     * ctx: User data stored with the subscription (e.g., "MyObject*")
     * e:   Event data passed when the event fires (e.g., "Keycode")
     */
    typedef void (*mc_callback_func_t)(void *ctx, void *e);

    /* Event struct. */
    typedef struct mc_event_t
    {
        mc_list_t listeners;
    } mc_event_t;

    /* Callback struct. */
    typedef struct mc_callback_t
    {
        mc_node_t node;
        mc_callback_func_t func;
        void *ctx;
    } mc_callback_t;

    /* Initialize event. */
    void mc_event_init(mc_event_t *event);

    /* Initialize callback. */
    void mc_callback_init(mc_callback_t *cb, mc_callback_func_t func, void *ctx);

    /* Register callback to event. */
    void mc_event_register(mc_event_t *event, mc_callback_t *cb);

    /* Unregister callback to event. */
    void mc_event_unregister(mc_event_t *event, mc_callback_t *cb);

    /* Trigger event. */
    void mc_event_trigger(mc_event_t *event, void *event_data);

#ifdef __cplusplus
}
#endif
