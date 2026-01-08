#pragma once

#ifdef __cplusplus
extern "C"
{
#endif

#include "mc/device/analog.h"
#include <stdbool.h>

#define MC_DEFINE_ANALOG_VECTOR3(NAME, DRIVER, CTX)                        \
    mc_analog_config_t NAME##_config = {0};                                \
                                                                           \
    const mc_analog_vector3_t NAME = {                                     \
        .x = {.driver = &DRIVER.x, .ctx = &CTX, .config = &NAME##_config}, \
        .y = {.driver = &DRIVER.y, .ctx = &CTX, .config = &NAME##_config}, \
        .z = {.driver = &DRIVER.z, .ctx = &CTX, .config = &NAME##_config}};

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

    // Initialize the device.
    void mc_analog_vector3_init(const mc_analog_vector3_t *dev,
                                bool is_read_only);

    // Set read only bit.
    void mc_analog_vector3_set_read_only(const mc_analog_vector3_t *dev,
                                         bool enable);

#ifdef __cplusplus
}
#endif
