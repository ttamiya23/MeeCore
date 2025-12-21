#ifndef SYSTEM_COMPOSITE_H_
#define SYSTEM_COMPOSITE_H_

#include "mc/system/composite.h"
#include "system_test.h"

// Composite system context.
typedef struct
{
    test_sys_ctx_t test_1;
    test_sys_ctx_t test_2;
} composite_sys_ctx_t;

// Composite system combining two test_systems
extern const mc_system_driver_t composite_sys_driver;

#endif // SYSTEM_COMPOSITE_H_