#pragma once

#ifdef __cplusplus
extern "C"
{
#endif

#include "mc/system/modules/analog_system.h"

// Macro for defining analog context. Users should always use this.
#define MC_DEFINE_ANALOG_VECTOR3_SYSTEM_WITH_CONFIG(NAME, CONFIG, DEVICE) \
    static mc_analog_vector3_system_ctx_t NAME##_ctx = {                  \
        .x = {.device = &DEVICE.x, .config = &CONFIG.x},                  \
        .y = {.device = &DEVICE.y, .config = &CONFIG.y},                  \
        .z = {.device = &DEVICE.z, .config = &CONFIG.z}};                 \
                                                                          \
    MC_DEFINE_SYSTEM(NAME, mc_analog_vector3_sys_driver, NAME##_ctx);

// Default constructor.
#define MC_DEFINE_ANALOG_VECTOR3_SYSTEM(NAME, DEVICE)                         \
    MC_DEFINE_ANALOG_VECTOR3_SYSTEM_WITH_CONFIG(NAME,                         \
                                                mc_analog_vector3_sys_config, \
                                                DEVICE);

    // Config for this system
    typedef struct mc_analog_vector3_system_config_t
    {
        const mc_analog_system_config_t x;
        const mc_analog_system_config_t y;
        const mc_analog_system_config_t z;
    } mc_analog_vector3_system_config_t;

    // Analog systems.
    typedef struct mc_analog_vector3_system_ctx_t
    {
        mc_analog_system_ctx_t x;
        mc_analog_system_ctx_t y;
        mc_analog_system_ctx_t z;
    } mc_analog_vector3_system_ctx_t;

    // Driver for analog vector system.
    extern const mc_system_driver_t mc_analog_vector3_sys_driver;

    // Default config for analog vector systems.
    extern const mc_analog_vector3_system_config_t mc_analog_vector3_sys_config;

#ifdef __cplusplus
}
#endif
