#pragma once

#ifdef __cplusplus
extern "C"
{
#endif

#include "mc/status.h"

// Macro for defining a system. Users should always use this.
#define MC_DEFINE_SYSTEM(NAME, DRIVER, CTX)      \
    static mc_system_state_t NAME##_state = {0}; \
                                                 \
    const mc_system_t NAME = {                   \
        .driver = &DRIVER,                       \
        .ctx = (void *)(&CTX),                   \
        .state = &NAME##_state};

    // Enum of different command types
    typedef enum mc_sys_cmd_type_t
    {
        MC_CMD_TYPE_UNKNOWN = 0,
        MC_CMD_TYPE_INPUT,
        MC_CMD_TYPE_OUTPUT,
        MC_CMD_TYPE_FUNC
    } mc_sys_cmd_type_t;

    // System command struct
    typedef struct mc_sys_cmd_info_t
    {
        char *alias;
        mc_sys_cmd_type_t type;
        uint8_t id;
        bool has_preset;    // If true, use the value below.
        int32_t preset_val; // e.g. 1 (for turnOn)
    } mc_sys_cmd_info_t;

    /* System driver struct. */
    typedef struct mc_system_driver_t
    {
        // Initialize context.
        void (*init)(void *ctx);

        // For invoking functions.
        mc_status_t (*invoke)(void *ctx, uint8_t func_id, int32_t *args,
                              uint8_t arg_count);

        // Write value to input.
        mc_status_t (*write_input)(void *ctx, uint8_t x_id, int32_t val);

        // Read input value.
        mc_result_t (*read_input)(void *ctx, uint8_t x_id);

        // Read output value.
        mc_result_t (*read_output)(void *ctx, uint8_t y_id);

        // Get alias command info.
        mc_status_t (*get_alias)(void *ctx, uint8_t id, mc_sys_cmd_info_t *info);

        // Member counts
        uint8_t (*get_function_count)(void *ctx);
        uint8_t (*get_input_count)(void *ctx);
        uint8_t (*get_output_count)(void *ctx);
        uint8_t (*get_alias_count)(void *ctx);
    } mc_system_driver_t;

    /* System state. */
    typedef struct mc_system_state_t
    {
        uint8_t is_initialized;
    } mc_system_state_t;

    /* System struct. */
    typedef struct mc_system_t
    {
        const mc_system_driver_t *driver;
        void *ctx;
        mc_system_state_t *state;
    } mc_system_t;

    /* Initialize system. */
    void mc_sys_init(const mc_system_t *sys);

    /* Call a function. */
    mc_status_t mc_sys_invoke(const mc_system_t *sys, uint8_t func_id,
                              int32_t *args, uint8_t arg_count);

    /* Write an input. */
    mc_status_t mc_sys_write_input(const mc_system_t *sys, uint8_t x_id,
                                   int32_t val);

    /* Read an input. */
    mc_result_t mc_sys_read_input(const mc_system_t *sys, uint8_t x_id);

    /* Read an output. */
    mc_result_t mc_sys_read_output(const mc_system_t *sys, uint8_t y_id);

    /* Get an alias. */
    mc_status_t mc_sys_get_alias(const mc_system_t *sys, uint8_t id,
                                 mc_sys_cmd_info_t *info);

    /* Get function count */
    uint8_t mc_sys_get_function_count(const mc_system_t *sys);

    /* Get input count */
    uint8_t mc_sys_get_input_count(const mc_system_t *sys);

    /* Get output count */
    uint8_t mc_sys_get_output_count(const mc_system_t *sys);

    /* Get alias count */
    uint8_t mc_sys_get_alias_count(const mc_system_t *sys);

#ifdef __cplusplus
}
#endif
