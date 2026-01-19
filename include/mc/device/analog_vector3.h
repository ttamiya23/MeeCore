#pragma once

#ifdef __cplusplus
extern "C"
{
#endif

#include "mc/device/analog.h"
#include "mc/utils.h"
#include <stdbool.h>

#define MC_DEFINE_ANALOG_VECTOR3(NAME, DRIVER, CTX)                        \
    mc_analog_config_t NAME##_config = {0};                                \
                                                                           \
    const mc_analog_vector3_t NAME = {                                     \
        .x = {.driver = &DRIVER.x, .ctx = &CTX, .config = &NAME##_config}, \
        .y = {.driver = &DRIVER.y, .ctx = &CTX, .config = &NAME##_config}, \
        .z = {.driver = &DRIVER.z, .ctx = &CTX, .config = &NAME##_config}};

#define MC_DEFINE_ANALOG_VECTOR3_DATA_OBJECT(NAME)               \
    static mc_analog_vector3_data_object_ctx_t NAME##_ctx = {0}; \
    MC_DEFINE_ANALOG_VECTOR3(NAME, mc_analog_vector3_data_object_driver, NAME##_ctx)

    // Analog vector struct
    typedef struct mc_analog_vector3_t
    {
        mc_analog_t x;
        mc_analog_t y;
        mc_analog_t z;
    } mc_analog_vector3_t;

    // Analog vector driver struct
    typedef struct mc_analog_vector3_driver_t
    {
        mc_analog_driver_t x;
        mc_analog_driver_t y;
        mc_analog_driver_t z;
    } mc_analog_vector3_driver_t;

    // Ctx for analog vector3 data object
    typedef struct mc_analog_vector3_data_object_ctx_t
    {
        int32_t x;
        int32_t y;
        int32_t z;
        mc_status_t error;
    } mc_analog_vector3_data_object_ctx_t;

    // Driver for analog vector3 data object
    extern const mc_analog_vector3_driver_t mc_analog_vector3_data_object_driver;

    // Initialize the device.
    void mc_analog_vector3_init(const mc_analog_vector3_t *dev,
                                bool is_read_only);

    // Set vector3.
    mc_status_t mc_analog_vector3_set_value(const mc_analog_vector3_t *dev,
                                            mc_vector3_t value);

    // Get vector3.
    mc_status_t mc_analog_vector3_get_value(const mc_analog_vector3_t *dev,
                                            mc_vector3_t *value);

    // Set read only bit.
    void mc_analog_vector3_set_read_only(const mc_analog_vector3_t *dev,
                                         bool enable);

#ifdef __cplusplus
}
#endif
