#pragma once

#include <stdint.h>
#include "mc/system/core.h"

#define FAKE_SYS_X_COUNT 2
#define FAKE_SYS_Y_COUNT 1
#define FAKE_SYS_F_COUNT 2

// Fake system context.
typedef struct fake_sys_ctx_t
{
    int32_t x[FAKE_SYS_X_COUNT];
    int32_t y[FAKE_SYS_Y_COUNT];
    char *increment_y_name;
    char *x0_name;
    char *y0_name;
} fake_sys_ctx_t;

// Fake system driver. Look at fake_system.c for more details.
extern const mc_system_driver_t fake_sys_driver;

// Functions. Exposing them just for documentation purposes.

// Increment y by 1.
mc_status_t increment_y(fake_sys_ctx_t *data);

// Just returns error.
mc_status_t returns_error(fake_sys_ctx_t *data, int32_t error);
