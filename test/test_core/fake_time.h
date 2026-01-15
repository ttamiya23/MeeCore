#pragma once

#include <stdint.h>
#include "mc/time.h"

// Context for the fake time driver
typedef struct fake_time_ctx_t
{
    uint32_t current_time_ms;
    uint8_t is_initialized;
} fake_time_ctx_t;

// The fake driver instance
extern const mc_time_driver_t fake_time_driver;

// Helper to manually set the time (e.g. for testing timeouts)
void fake_time_set_ms(fake_time_ctx_t *ctx, uint32_t ms);