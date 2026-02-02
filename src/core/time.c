#include "mc/time.h"
#include "mc/utils.h"

// Global singleton state
static const mc_time_driver_t *sys_time_driver = NULL;
static void *sys_time_ctx = NULL;
static uint8_t sys_time_initialized = 0;

void mc_time_init(const mc_time_driver_t *driver, void *ctx)
{
    MC_ASSERT(driver != NULL);
    MC_ASSERT(driver->get_ms != NULL);
    MC_ASSERT(driver->delay != NULL);

    sys_time_driver = driver;
    sys_time_ctx = ctx;

    // Initialize the underlying driver if provided
    if (sys_time_driver->init)
    {
        sys_time_driver->init(sys_time_ctx);
    }

    sys_time_initialized = MC_INITIALIZED;
}

uint32_t mc_time_get_ms(void)
{
    return sys_time_initialized == MC_INITIALIZED
               ? sys_time_driver->get_ms(sys_time_ctx)
               : 0;
}

void mc_time_delay(uint32_t ms)
{
    if (sys_time_initialized == MC_INITIALIZED)
    {
        sys_time_driver->delay(sys_time_ctx, ms);
    }
}