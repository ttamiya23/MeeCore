#pragma once

#ifdef __cplusplus
extern "C"
{
#endif

#include "mc/status.h"
#include "mc/system/core.h"
#include "mc/io.h"

// Macro for cleaner definitions
#define MC_SYS_ENTRY(NAME, SYS) {.name = NAME, .system = &SYS_PTR}

    // Struct for each system.
    typedef struct mc_system_entry_t
    {
        const char *name;
        const mc_system_t *system;
    } mc_system_entry_t;

    typedef struct mc_system_console_t
    {
        // The IO interface used for replying (e.g. UART, USB).
        mc_io_t *io;
        mc_callback_t rx_callback;

        // List of systems.
        const mc_system_entry_t *systems;
        uint8_t system_count;

        // Argument buffer used to parse function arguments.
        int32_t *args_buffer;
        uint8_t max_args_count;

        // Max header count when dumping
        uint8_t max_header_count;
    } mc_system_console_t;

    /* Initialize the console and attach it to an IO stream. */
    void mc_system_console_init(mc_system_console_t *console, mc_io_t *io,
                                const mc_system_entry_t *system_list,
                                uint8_t sys_count, int32_t *args_buffer,
                                uint8_t max_args_count,
                                uint8_t max_header_count);

    /* Dump system info */
    mc_status_t mc_system_console_dump(mc_system_console_t *console,
                                       const mc_system_entry_t *systems,
                                       uint8_t sys_count);

#ifdef __cplusplus
}
#endif
