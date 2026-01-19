#pragma once

#ifdef __cplusplus
extern "C"
{
#endif

#include "mc/status.h"
#include <stdbool.h>

#define MC_DEFINE_DIGITAL(NAME, DRIVER, CTX)        \
    static mc_digital_config_t NAME##_config = {0}; \
                                                    \
    const mc_digital_t NAME = {                     \
        .driver = &DRIVER,                          \
        .ctx = &CTX,                                \
        .config = &NAME##_config}

#define MC_DEFINE_DIGITAL_DATA_OBJECT(NAME)               \
    static mc_digital_data_object_ctx_t NAME##_ctx = {0}; \
    MC_DEFINE_DIGITAL(NAME, mc_digital_data_object_driver, NAME##_ctx)

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

    // Ctx for digital data object
    typedef struct mc_digital_data_object_ctx_t
    {
        bool state;
        mc_status_t error;
    } mc_digital_data_object_ctx_t;

    // Driver for digital data object
    extern const mc_digital_driver_t mc_digital_data_object_driver;

    // Initialize the device.
    void mc_digital_init(const mc_digital_t *dev, bool is_read_only);

    // Set the state.
    mc_status_t mc_digital_set_state(const mc_digital_t *dev, bool state);

    // Get current state.
    mc_result_t mc_digital_get_state(const mc_digital_t *dev);

    // Toggle based on current state.
    mc_status_t mc_digital_toggle(const mc_digital_t *dev);

    // Set read only bit.
    void mc_digital_set_read_only(const mc_digital_t *dev, bool enable);

#ifdef __cplusplus
}
#endif
