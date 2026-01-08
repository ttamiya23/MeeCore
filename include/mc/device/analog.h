#pragma once

#ifdef __cplusplus
extern "C"
{
#endif

#include "mc/status.h"
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
    void mc_analog_init(const mc_analog_t *dev, bool is_read_only);

    // Set the value.
    mc_status_t mc_analog_set_value(const mc_analog_t *dev, int32_t value);

    // Get current value.
    mc_result_t mc_analog_get_value(const mc_analog_t *dev);

    // Set read only bit.
    void mc_analog_set_read_only(const mc_analog_t *dev, bool enable);

#ifdef __cplusplus
}
#endif
