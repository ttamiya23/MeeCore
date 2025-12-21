#ifndef MC_DEVICE_DIGITAL_H_
#define MC_DEVICE_DIGITAL_H_

#ifdef __cplusplus
extern "C"
{
#endif

#include "mc/common.h"
#include <stdbool.h>

#define MC_DEFINE_DIGITAL(NAME, DRIVER, CTX) \
    const mc_digital_t NAME = {              \
        .driver = &DRIVER,                   \
        .ctx = &CTX}

    // Driver struct for digital.
    typedef struct mc_digital_driver_t
    {
        void (*init)(void *ctx);
        mc_status_t (*set_state)(void *ctx, bool state);
        mc_status_t (*get_state)(void *ctx, bool *state);

    } mc_digital_driver_t;

    // Digital struct
    typedef struct mc_digital_t
    {
        const mc_digital_driver_t *driver;
        void *ctx;
    } mc_digital_t;

    // Initialize the device.
    static inline void mc_digital_init(const mc_digital_t *dev)
    {
        MC_ASSERT(dev != NULL);
        MC_ASSERT(dev->driver != NULL);
        if (dev->driver->init)
        {
            dev->driver->init(dev->ctx);
        }
    }

    // Set the state.
    static inline mc_status_t mc_digital_set(const mc_digital_t *dev, bool state)
    {
        MC_ASSERT(dev != NULL);
        MC_ASSERT(dev->driver != NULL);
        if (dev->driver->set_state)
        {
            return dev->driver->set_state(dev->ctx, state);
        }
        return MC_ERROR_NOT_SUPPORTED;
    }

    // Get current state.
    static inline mc_status_t mc_digital_get(const mc_digital_t *dev,
                                             bool *state)
    {
        MC_ASSERT(dev != NULL);
        MC_ASSERT(dev->driver != NULL);
        MC_ASSERT(state != NULL);
        if (dev->driver->get_state)
        {
            return dev->driver->get_state(dev->ctx, state);
        }
        return MC_ERROR_NOT_SUPPORTED;
    }

    // Toggle based on current state.
    static inline mc_status_t mc_digital_toggle(const mc_digital_t *dev)
    {
        bool current = false;
        MC_RETURN_IF_ERROR(mc_digital_get(dev, &current));
        return mc_digital_set(dev, !current);
    }

#ifdef __cplusplus
}
#endif

#endif // MC_DEVICE_DIGITAL_H_