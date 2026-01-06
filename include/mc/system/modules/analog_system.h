#pragma once

#ifdef __cplusplus
extern "C"
{
#endif

#include "mc/system/core.h"
#include "mc/device/analog.h"

// Macro for defining analog context. Users should always use this.
#define MC_DEFINE_ANALOG_SYSTEM_WITH_CONFIG(NAME, CONFIG, DEVICE) \
    static mc_analog_system_ctx_t NAME##_ctx = {                  \
        .config = &CONFIG,                                        \
        .device = &DEVICE};                                       \
                                                                  \
    MC_DEFINE_SYSTEM(NAME, mc_analog_sys_driver, NAME##_ctx);

// Default constructor.
#define MC_DEFINE_ANALOG_SYSTEM(NAME, DEVICE) \
    MC_DEFINE_ANALOG_SYSTEM_WITH_CONFIG(NAME, mc_analog_sys_config, DEVICE);

    // Config for this system
    typedef struct mc_analog_system_config_t
    {
        const char *value_name;
        const char *target_value_name;
    } mc_analog_system_config_t;

    // Context for this system
    typedef struct mc_analog_system_ctx_t
    {
        const mc_analog_t *device;
        const mc_analog_system_config_t *config;
        int32_t target_value;
    } mc_analog_system_ctx_t;

    // analog system driver.
    extern const mc_system_driver_t mc_analog_sys_driver;

    // Default config for analog systems.
    extern const mc_analog_system_config_t mc_analog_sys_config;

#ifdef __cplusplus
}
#endif
