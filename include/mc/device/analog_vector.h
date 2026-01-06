#pragma once

#ifdef __cplusplus
extern "C"
{
#endif

#include "mc/status.h"
#include "mc/utils.h"
#include "mc/device/analog.h"
#include <stdbool.h>

#define MC_DEFINE_ANALOG_VECTOR(NAME, DRIVER, CTX)                         \
    mc_analog_config_t NAME##_config = {0};                                \
                                                                           \
    const mc_analog_vector_t NAME = {                                      \
        .x = {.driver = &DRIVER.x, .ctx = &CTX, .config = &NAME##_config}, \
        .y = {.driver = &DRIVER.y, .ctx = &CTX, .config = &NAME##_config}, \
        .z = {.driver = &DRIVER.z, .ctx = &CTX, .config = &NAME##_config}};

    // Analog vector struct
    typedef struct mc_analog_vector_t
    {
        mc_analog_t x;
        mc_analog_t y;
        mc_analog_t z;
    } mc_analog_vector_t;

    // Analog vector driver struct
    typedef struct mc_analog_vector_driver_t
    {
        mc_analog_driver_t x;
        mc_analog_driver_t y;
        mc_analog_driver_t z;
    } mc_analog_vector_driver_t;

    // Initialize the device.
    static inline void mc_analog_vector_init(const mc_analog_vector_t *dev,
                                             bool is_read_only)
    {
        MC_ASSERT(dev != NULL);
        mc_analog_init(&dev->x, is_read_only);
        mc_analog_init(&dev->y, is_read_only);
        mc_analog_init(&dev->z, is_read_only);
    }

    // Set read only bit.
    static inline void mc_analog_vector_set_read_only(
        const mc_analog_vector_t *dev, bool enable)
    {
        MC_ASSERT(dev != NULL);
        mc_analog_set_read_only(&dev->x, enable);
        mc_analog_set_read_only(&dev->y, enable);
        mc_analog_set_read_only(&dev->z, enable);
    }

#ifdef __cplusplus
}
#endif
