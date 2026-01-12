#pragma once

#include <stdint.h>
#include "mc/device/analog.h"

// Test struct for this test.
typedef struct fake_analog_ctx_t
{
    int32_t value;
    uint8_t is_initialized;
    mc_status_t error;
} fake_analog_ctx_t;

extern const mc_analog_driver_t fake_analog_driver;
