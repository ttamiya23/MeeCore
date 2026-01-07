#pragma once

#ifdef __cplusplus
extern "C"
{
#endif

#include <stdint.h>
#include "mc/status.h"

    // Time driver interface
    typedef struct mc_time_driver_t
    {
        // Initialize the hardware timer
        void (*init)(void *ctx);

        // Get current time in milliseconds
        uint32_t (*get_ms)(void *ctx);

        // Blocking delay
        void (*delay)(void *ctx, uint32_t ms);
    } mc_time_driver_t;

    /* Initialize the global system time module. */
    void mc_time_init(const mc_time_driver_t *driver, void *ctx);

    /* Get the current system time in milliseconds. */
    uint32_t mc_time_get_ms();

    /* Blocking delay for a specified number of milliseconds. */
    void mc_time_delay(uint32_t ms);

#ifdef __cplusplus
}
#endif