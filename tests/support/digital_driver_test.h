#ifndef DIGITAL_DRIVER_TEST_H_
#define DIGITAL_DRIVER_TEST_H_

#include <stdint.h>
#include "mc/device/digital.h"

// Test struct for this test.
typedef struct test_digital_ctx_t
{
    bool state;
    uint8_t is_initialized;
    mc_status_t error;
} test_digital_ctx_t;

extern const mc_digital_driver_t test_digital_driver;

#endif // DIGITAL_DRIVER_TEST_H_