#pragma once

#ifdef __cplusplus
extern "C"
{
#endif

#include "mc/system/core.h"
#include "mc/device/digital.h"

// Macro for defining digital context. Users should always use this.
#define MC_DEFINE_DIGITAL_SYS_CTX(NAME, DEVICE) \
    static mc_digital_system_ctx_t NAME = {     \
        .device = &DEVICE};

    // Digital system driver.
    extern const mc_system_driver_t mc_digital_sys_driver;

    // Context for this system
    typedef struct mc_digital_system_ctx_t
    {
        const mc_digital_t *device;
        bool target_state;
    } mc_digital_system_ctx_t;

#ifdef __cplusplus
}
#endif
