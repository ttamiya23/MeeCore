#ifndef MC_SYSTEM_CORE_H_
#define MC_SYSTEM_CORE_H_

#ifdef __cplusplus
extern "C"
{
#endif

#include "mc/common.h"

// Macro for defining a system. Users should always use this.
#define MC_DEFINE_SYSTEM(NAME, DRIVER, CTX) \
    const mc_system_t NAME = {              \
        .driver = &DRIVER,                  \
        .ctx = (void *)(&CTX),              \
    };

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
        mc_status_t (*read_input)(void *ctx, uint8_t x_id, int32_t *val);

        // Read output value.
        mc_status_t (*read_output)(void *ctx, uint8_t y_id, int32_t *val);

        // Member counts
        uint8_t (*get_function_count)(void *ctx);
        uint8_t (*get_input_count)(void *ctx);
        uint8_t (*get_output_count)(void *ctx);
    } mc_system_driver_t;

    /* System struct. */
    typedef struct mc_system_t
    {
        const mc_system_driver_t *driver;
        void *ctx;
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
    mc_status_t mc_sys_read_input(const mc_system_t *sys, uint8_t x_id,
                                  int32_t *val);

    /* Read an output. */
    mc_status_t mc_sys_read_output(const mc_system_t *sys, uint8_t y_id,
                                   int32_t *val);

    /* Get function count */
    uint8_t mc_sys_get_function_count(const mc_system_t *sys);

    /* Get input count */
    uint8_t mc_sys_get_input_count(const mc_system_t *sys);

    /* Get output count */
    uint8_t mc_sys_get_output_count(const mc_system_t *sys);

#ifdef __cplusplus
}
#endif

#endif /* MC_SYSTEM_CORE_H_ */