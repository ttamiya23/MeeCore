#ifndef DEVICE_FAKE_DIGITAL_DRIVER_H_
#define DEVICE_FAKE_DIGITAL_DRIVER_H_

#include <stdint.h>
#include "mc/device/digital.h"

// Test struct for this test.
typedef struct fake_digital_ctx_t
{
    bool state;
    uint8_t is_initialized;
    mc_status_t error;
} fake_digital_ctx_t;

extern const mc_digital_driver_t fake_digital_driver;

#endif // DEVICE_FAKE_DIGITAL_DRIVER_H_