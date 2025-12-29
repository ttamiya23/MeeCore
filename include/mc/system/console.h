#pragma once

#ifdef __cplusplus
extern "C"
{
#endif

#include "mc/status.h"
#include "mc/system/core.h"
#include "mc/io.h"

// Macro for cleaner definitions
#define MC_SYS_ENTRY(ID, SYS, NAME) {.id = ID, .system = &SYS, .name = NAME}

    // Struct for each system.
    typedef struct mc_system_entry_t
    {
        const uint8_t id;
        const mc_system_t *system;
        const char *name;
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
        uint8_t args_count;

        // Max header count when dumping
        uint8_t header_count;

        uint8_t is_initialized;
    } mc_system_console_t;

    /* Initialize the console and attach it to an IO stream. */
    void mc_sys_console_init(mc_system_console_t *console, mc_io_t *io,
                             const mc_system_entry_t *system_list,
                             uint8_t sys_count);

    /* Dump system info */
    mc_status_t mc_sys_console_dump(mc_system_console_t *console,
                                    const mc_system_entry_t *systems,
                                    uint8_t sys_count);

    /* Set max header count */
    void mc_sys_console_set_header_count(mc_system_console_t *console,
                                         uint8_t count);

    /* Set args buffer */
    void mc_sys_console_set_args_buffer(mc_system_console_t *console,
                                        int32_t *args_buffer, uint8_t args_len);

#ifdef __cplusplus
}
#endif
