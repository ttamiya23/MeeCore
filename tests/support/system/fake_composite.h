#pragma once

#include "mc/system/composite.h"
#include "system/fake_system.h"

// Composite system context.
typedef struct
{
    fake_sys_ctx_t sys1;
    fake_sys_ctx_t sys2;
} fake_composite_ctx_t;

// Composite system combining two test_systems
extern const mc_system_driver_t fake_composite_sys_driver;
