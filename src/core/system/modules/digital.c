#include "mc/system/modules/digital.h"
#include <string.h>

#define DIGITAL_SYS_X_COUNT 1
#define DIGITAL_SYS_Y_COUNT 1
#define DIGITAL_SYS_F_COUNT 1
#define DIGITAL_SYS_ALIAS_COUNT 3
#define DIGITAL_TURN_ON_CMD "turnOn"
#define DIGITAL_TURN_OFF_CMD "turnOff"
#define DIGITAL_TOGGLE_CMD "toggle"

void digital_sys_init(void *ctx)
{
    mc_digital_system_ctx_t *digital_ctx = (mc_digital_system_ctx_t *)ctx;
    digital_ctx->target_state = false;

    mc_result_t current_state = mc_digital_get_state(digital_ctx->device);
    if (current_state.ok)
    {
        digital_ctx->target_state = current_state.value;
    }
}

mc_status_t digital_sys_invoke(void *ctx, uint8_t func_id, int32_t *args,
                               uint8_t arg_count)
{
    mc_digital_system_ctx_t *digital_ctx = (mc_digital_system_ctx_t *)ctx;

    // Set target state to !current_state
    MC_ASSIGN_OR_RETURN(current_state, mc_digital_get_state(digital_ctx->device));
    digital_ctx->target_state = !current_state;

    return mc_digital_toggle(digital_ctx->device);
}

mc_status_t digital_sys_write_input(void *ctx, uint8_t x_id, int32_t val)
{
    mc_digital_system_ctx_t *digital_ctx = (mc_digital_system_ctx_t *)ctx;
    digital_ctx->target_state = val;
    return mc_digital_set_state(digital_ctx->device, val);
}

mc_result_t digital_sys_read_input(void *ctx, uint8_t x_id)
{
    mc_digital_system_ctx_t *digital_ctx = (mc_digital_system_ctx_t *)ctx;
    return MC_OK_VAL(digital_ctx->target_state);
}

mc_result_t digital_sys_read_output(void *ctx, uint8_t y_id)
{
    mc_digital_system_ctx_t *digital_ctx = (mc_digital_system_ctx_t *)ctx;
    return mc_digital_get_state(digital_ctx->device);
}

bool digital_sys_get_alias(void *ctx, uint8_t id, mc_sys_cmd_info_t *info)
{
    switch (id)
    {
    case 0:
        info->alias = DIGITAL_TURN_ON_CMD;
        info->type = MC_CMD_TYPE_INPUT;
        info->id = 0;
        info->has_preset = true;
        info->preset_val = 1;
        return true;
    case 1:
        info->alias = DIGITAL_TURN_OFF_CMD;
        info->type = MC_CMD_TYPE_INPUT;
        info->id = 0;
        info->has_preset = true;
        info->preset_val = 0;
        return true;
    case 2:
        info->alias = DIGITAL_TOGGLE_CMD;
        info->type = MC_CMD_TYPE_FUNC;
        info->id = 0;
        info->has_preset = false;
        return true;
    default:
        return false;
    }
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

uint8_t digital_sys_get_alias_count(void *ctx)
{
    return DIGITAL_SYS_ALIAS_COUNT;
}

const mc_system_driver_t mc_digital_sys_driver = {
    .init = digital_sys_init,
    .invoke = digital_sys_invoke,
    .write_input = digital_sys_write_input,
    .read_input = digital_sys_read_input,
    .read_output = digital_sys_read_output,
    .get_alias = digital_sys_get_alias,
    .get_function_count = digital_sys_get_function_count,
    .get_input_count = digital_sys_get_input_count,
    .get_output_count = digital_sys_get_output_count,
    .get_alias_count = digital_sys_get_alias_count};
