#include "mc/system/modules/digital.h"

#define DIGITAL_SYS_X_COUNT 1
#define DIGITAL_SYS_Y_COUNT 1
#define DIGITAL_SYS_F_COUNT 1

void digital_sys_init(void *ctx)
{
    mc_digital_system_ctx_t *digital_ctx = (mc_digital_system_ctx_t *)ctx;
    mc_digital_get(digital_ctx->device, &digital_ctx->target_state);
}

mc_status_t digital_sys_invoke(void *ctx, uint8_t func_id, int32_t *args,
                               uint8_t arg_count)
{
    mc_digital_system_ctx_t *digital_ctx = (mc_digital_system_ctx_t *)ctx;

    // Set target state to !current_state
    bool current_state;
    MC_RETURN_IF_ERROR(mc_digital_get(digital_ctx->device, &current_state));
    digital_ctx->target_state = !current_state;

    return mc_digital_toggle(digital_ctx->device);
}

mc_status_t digital_sys_write_input(void *ctx, uint8_t x_id, int32_t val)
{
    mc_digital_system_ctx_t *digital_ctx = (mc_digital_system_ctx_t *)ctx;
    digital_ctx->target_state = val;
    return mc_digital_set(digital_ctx->device, val);
}

mc_status_t digital_sys_read_input(void *ctx, uint8_t x_id, int32_t *val)
{
    mc_digital_system_ctx_t *digital_ctx = (mc_digital_system_ctx_t *)ctx;
    *val = (int32_t)digital_ctx->target_state;
    return MC_OK;
}

mc_status_t digital_sys_read_output(void *ctx, uint8_t y_id, int32_t *val)
{
    mc_digital_system_ctx_t *digital_ctx = (mc_digital_system_ctx_t *)ctx;
    return mc_digital_get(digital_ctx->device, (bool *)val);
}

uint8_t digital_sys_get_function_count(void *ctx)
{
    return DIGITAL_SYS_F_COUNT;
}

uint8_t digital_sys_get_input_count(void *ctx)
{
    return DIGITAL_SYS_X_COUNT;
}

uint8_t digital_sys_get_output_count(void *ctx)
{
    return DIGITAL_SYS_Y_COUNT;
}

const mc_system_driver_t mc_digital_sys_driver = {
    .init = digital_sys_init,
    .invoke = digital_sys_invoke,
    .write_input = digital_sys_write_input,
    .read_input = digital_sys_read_input,
    .read_output = digital_sys_read_output,
    .get_function_count = digital_sys_get_function_count,
    .get_input_count = digital_sys_get_input_count,
    .get_output_count = digital_sys_get_output_count};
