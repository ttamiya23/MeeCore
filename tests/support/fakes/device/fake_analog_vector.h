#pragma once

#include <stdint.h>
#include "mc/device/analog_vector.h"
#include "fake_analog.h"

// Test struct for this test.
typedef struct fake_analog_vector_ctx_t
{
    fake_analog_ctx_t x;
    fake_analog_ctx_t y;
    fake_analog_ctx_t z;
} fake_analog_vector_ctx_t;

extern const mc_analog_vector_driver_t fake_analog_vector_driver;
