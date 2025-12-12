#ifndef MC_EVENT_H_
#define MC_EVENT_H_

#ifdef __cplusplus
extern "C"
{
#endif

#include "mc/list.h"

    /* Callback function: takes in void pointer pointing to context and event data.
     * context:    User data stored with the subscription (e.g., "MyObject*")
     * event_data: Ephemeral data passed when the event fires (e.g., "Keycode")
     */
    typedef void (*mc_callback_func_t)(void *context, void *event_data);

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
        void *context;
        uint8_t is_initialized;
    } mc_callback_t;

    /* Initialize event. */
    void mc_event_init(mc_event_t *event);

    /* Initialize callback. */
    void mc_callback_init(mc_callback_t *cb, mc_callback_func_t func, void *context);

    /* Register callback to event. */
    void mc_event_register(mc_event_t *event, mc_callback_t *cb);

    /* Unregister callback to event. */
    void mc_event_unregister(mc_event_t *event, mc_callback_t *cb);

    /* Trigger event. */
    void mc_event_trigger(mc_event_t *event, void *event_data);

#ifdef __cplusplus
}
#endif

#endif /* MC_EVENT_H_ */
