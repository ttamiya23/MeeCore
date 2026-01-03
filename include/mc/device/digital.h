#pragma once

#ifdef __cplusplus
extern "C"
{
#endif

#include "mc/status.h"
#include "mc/utils.h"
#include <stdbool.h>

#define MC_DEFINE_DIGITAL(NAME, DRIVER, CTX) \
    mc_digital_config_t NAME##_config = {0}; \
                                             \
    const mc_digital_t NAME = {              \
        .driver = &DRIVER,                   \
        .ctx = &CTX,                         \
        .config = &NAME##_config}

    // Driver struct for digital.
    typedef struct mc_digital_driver_t
    {
        void (*init)(void *ctx);
        mc_status_t (*set_state)(void *ctx, bool state);
        mc_result_t (*get_state)(void *ctx);

    } mc_digital_driver_t;

    // Digital config. Contains initialization and read only state.
    typedef struct mc_digital_config_t
    {
        uint8_t is_initialized;
        bool is_read_only;
    } mc_digital_config_t;

    // Digital struct
    typedef struct mc_digital_t
    {
        const mc_digital_driver_t *driver;
        void *ctx;
        mc_digital_config_t *config;
    } mc_digital_t;

    // Initialize the device.
    static inline void mc_digital_init(const mc_digital_t *dev,
                                       bool is_read_only)
    {
        MC_ASSERT(dev != NULL);
        MC_ASSERT(dev->config != NULL);
        MC_ASSERT(dev->driver != NULL);
        if (dev->driver->init)
        {
            dev->driver->init(dev->ctx);
        }
        dev->config->is_initialized = MC_INITIALIZED;
        dev->config->is_read_only = is_read_only;
    }

    // Set the state.
    static inline mc_status_t mc_digital_set_state(const mc_digital_t *dev,
                                                   bool state)
    {
        MC_ASSERT(dev != NULL);
        MC_ASSERT(dev->config != NULL);
        MC_ASSERT(dev->config->is_initialized == MC_INITIALIZED);
        if (dev->config->is_read_only)
        {
            return MC_ERROR_WRITE_PROTECTED;
        }
        if (dev->driver->set_state)
        {
            return dev->driver->set_state(dev->ctx, state);
        }
        return MC_ERROR_NOT_SUPPORTED;
    }

    // Get current state.
    static inline mc_result_t mc_digital_get_state(const mc_digital_t *dev)
    {
        MC_ASSERT(dev != NULL);
        MC_ASSERT(dev->config != NULL);
        MC_ASSERT(dev->config->is_initialized == MC_INITIALIZED);
        if (dev->driver->get_state)
        {
            return dev->driver->get_state(dev->ctx);
        }
        return MC_ERR_VAL(MC_ERROR_NOT_SUPPORTED);
    }

    // Toggle based on current state.
    static inline mc_status_t mc_digital_toggle(const mc_digital_t *dev)
    {
        MC_ASSIGN_OR_RETURN(current, mc_digital_get_state(dev));
        return mc_digital_set_state(dev, !current);
    }

    // Set read only bit.
    static inline void mc_digital_set_read_only(const mc_digital_t *dev,
                                                bool enable)
    {
        MC_ASSERT(dev != NULL);
        MC_ASSERT(dev->config != NULL);
        MC_ASSERT(dev->config->is_initialized == MC_INITIALIZED);
        dev->config->is_read_only = enable;
    }

#ifdef __cplusplus
}
#endif
