#pragma once

#ifdef __cplusplus
extern "C"
{
#endif

#include "mc/status.h"
#include "mc/utils.h"
#include <stdbool.h>

#define MC_DEFINE_ANALOG(NAME, DRIVER, CTX) \
    mc_analog_config_t NAME##_config = {0}; \
                                            \
    const mc_analog_t NAME = {              \
        .driver = &DRIVER,                  \
        .ctx = &CTX,                        \
        .config = &NAME##_config}

    // Driver struct for analog.
    typedef struct mc_analog_driver_t
    {
        void (*init)(void *ctx);
        mc_status_t (*set_value)(void *ctx, int32_t value);
        mc_result_t (*get_value)(void *ctx);
    } mc_analog_driver_t;

    // Analog config. Contains initialization and read only state.
    typedef struct mc_analog_config_t
    {
        uint8_t is_initialized;
        bool is_read_only;
    } mc_analog_config_t;

    // Analog struct
    typedef struct mc_analog_t
    {
        const mc_analog_driver_t *driver;
        void *ctx;
        mc_analog_config_t *config;
    } mc_analog_t;

    // Initialize the device.
    static inline void mc_analog_init(const mc_analog_t *dev,
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

    // Set the value.
    static inline mc_status_t mc_analog_set_value(const mc_analog_t *dev,
                                                  int32_t value)
    {
        MC_ASSERT(dev != NULL);
        MC_ASSERT(dev->config != NULL);
        MC_ASSERT(dev->config->is_initialized == MC_INITIALIZED);
        if (dev->config->is_read_only)
        {
            return MC_ERROR_WRITE_PROTECTED;
        }
        if (dev->driver->set_value)
        {
            return dev->driver->set_value(dev->ctx, value);
        }
        return MC_ERROR_NOT_SUPPORTED;
    }

    // Get current value.
    static inline mc_result_t mc_analog_get_value(const mc_analog_t *dev)
    {
        MC_ASSERT(dev != NULL);
        MC_ASSERT(dev->config != NULL);
        MC_ASSERT(dev->config->is_initialized == MC_INITIALIZED);
        if (dev->driver->get_value)
        {
            return dev->driver->get_value(dev->ctx);
        }
        return MC_ERR_VAL(MC_ERROR_NOT_SUPPORTED);
    }

    // Set read only bit.
    static inline void mc_analog_set_read_only(const mc_analog_t *dev,
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
