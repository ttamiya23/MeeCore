#pragma once

#ifdef __cplusplus
extern "C"
{
#endif

#include "mc/system/core.h"
#include "mc/device/digital.h"

// Macro for defining digital context. Users should always use this.
#define MC_DEFINE_DIGITAL_SYSTEM_WITH_CONFIG(NAME, CONFIG, DEVICE) \
    static mc_digital_system_ctx_t NAME##_ctx = {                  \
        .device = &DEVICE,                                         \
        .config = &CONFIG};                                        \
                                                                   \
    MC_DEFINE_SYSTEM(NAME, mc_digital_sys_driver, NAME##_ctx);

// Default constructor.
#define MC_DEFINE_DIGITAL_SYSTEM(NAME, DEVICE) \
    MC_DEFINE_DIGITAL_SYSTEM_WITH_CONFIG(NAME, mc_digital_sys_config, DEVICE);

    // Config for this system
    typedef struct mc_digital_system_config_t
    {
        const char *state_name;
        const char *target_state_name;
        const char *turn_on_name;
        const char *turn_off_name;
        const char *toggle_name;
    } mc_digital_system_config_t;

    // Context for this system
    typedef struct mc_digital_system_ctx_t
    {
        const mc_digital_t *device;
        const mc_digital_system_config_t *config;
        bool target_state;
    } mc_digital_system_ctx_t;

    // Digital system driver.
    extern const mc_system_driver_t mc_digital_sys_driver;

    // Default config for digital systems.
    extern const mc_digital_system_config_t mc_digital_sys_config;

#ifdef __cplusplus
}
#endif
