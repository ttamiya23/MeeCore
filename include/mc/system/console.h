#pragma once

#ifdef __cplusplus
extern "C"
{
#endif

#include "mc/status.h"
#include "mc/system/core.h"
#include "mc/io.h"

// Defaults
#define SYS_CONSOLE_DEFAULT_HEADER_COUNT 5
#define SYS_CONSOLE_DEFAULT_ARGS_COUNT 8

// Macros for cleaner definitions
#define MC_SYS_ENTRY(ID, SYS, NAME) {.id = ID, .system = &SYS, .name = NAME}

#define MC_DEFINE_SYSTEM_CONSOLE(NAME, IO, SYSTEMS, SYS_COUNT, ARGS_COUNT, \
                                 HEADER_COUNT)                             \
    static int32_t NAME##_args[ARGS_COUNT];                                \
    static const mc_system_console_config_t NAME##_config =                \
        {                                                                  \
            .systems = SYSTEMS,                                            \
            .system_count = SYS_COUNT,                                     \
            .args_buffer = NAME##_args,                                    \
            .args_count = ARGS_COUNT,                                      \
            .header_count = HEADER_COUNT};                                 \
    static mc_system_console_state_t NAME##_state = {0};                   \
    static const mc_system_console_t NAME =                                \
        {                                                                  \
            .io = &IO,                                                     \
            .config = &NAME##_config,                                      \
            .state = &NAME##_state}

    // Struct for each system.
    typedef struct mc_system_entry_t
    {
        const uint8_t id;
        const mc_system_t *system;
        const char *name;
    } mc_system_entry_t;

    // Struct for console config
    typedef struct mc_system_console_config_t
    {
        // List of systems.
        const mc_system_entry_t *systems;
        uint8_t system_count;

        // Argument buffer used to parse function arguments.
        int32_t *args_buffer;
        uint8_t args_count;

        // Max header count when dumping
        uint8_t header_count;
    } mc_system_console_config_t;

    // Struct for console state
    typedef struct mc_system_console_state_t
    {
        mc_callback_t rx_callback;
        uint8_t is_initialized;
    } mc_system_console_state_t;

    // Console struct.
    typedef struct mc_system_console_t
    {
        // The IO interface used for replying (e.g. UART, USB).
        const mc_io_t *io;
        const mc_system_console_config_t *config;
        mc_system_console_state_t *state;
    } mc_system_console_t;

    /* Initialize the console and attach it to an IO stream. */
    void mc_sys_console_init(const mc_system_console_t *console);

    /* Dump system info */
    mc_status_t mc_sys_console_dump(const mc_system_console_t *console,
                                    const mc_system_entry_t *systems,
                                    uint8_t sys_count);

#ifdef __cplusplus
}
#endif
