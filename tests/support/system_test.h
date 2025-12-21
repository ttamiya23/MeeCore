#ifndef SYSTEM_TEST_1_H_
#define SYSTEM_TEST_1_H_

#include <stdint.h>
#include "mc/system/core.h"

#define TEST_SYS_X_COUNT 2
#define TEST_SYS_Y_COUNT 1
#define TEST_SYS_F_COUNT 2

// Add system context.
typedef struct
{
    int32_t x[TEST_SYS_X_COUNT];
    int32_t y[TEST_SYS_Y_COUNT];
} test_sys_ctx_t;

// Test 1 system driver. Look at system_add.c for more details.
extern const mc_system_driver_t test_sys_driver;

// Functions. Exposing them just for documentation purposes.

// Increment y by 1.
mc_sys_status_t increment_y(test_sys_ctx_t *data);

// Just returns error.
mc_sys_status_t returns_error(test_sys_ctx_t *data, int32_t error);

#endif // SYSTEM_TEST_1_H_
