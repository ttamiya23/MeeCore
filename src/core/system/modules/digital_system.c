#include "mc/system/modules/digital_system.h"
#include <string.h>

#define DIGITAL_SYS_X_COUNT 1
#define DIGITAL_SYS_Y_COUNT 1
#define DIGITAL_SYS_F_COUNT 1
#define DIGITAL_SYS_ALIAS_COUNT 5
#define DIGITAL_SYS_READ_ONLY_X_COUNT 0
#define DIGITAL_SYS_READ_ONLY_Y_COUNT 1
#define DIGITAL_SYS_READ_ONLY_F_COUNT 0
#define DIGITAL_SYS_READ_ONLY_ALIAS_COUNT 1
#define DIGITAL_SYS_STATE_ALIAS "state"
#define DIGITAL_SYS_TARGET_STATE_ALIAS "targetState"
#define DIGITAL_SYS_TURN_ON_ALIAS "turnOn"
#define DIGITAL_SYS_TURN_OFF_ALIAS "turnOff"
#define DIGITAL_SYS_TOGGLE_ALIAS "toggle"

const mc_digital_system_config_t mc_digital_sys_config = {
    .state_name = DIGITAL_SYS_STATE_ALIAS,
    .target_state_name = DIGITAL_SYS_TARGET_STATE_ALIAS,
    .turn_on_name = DIGITAL_SYS_TURN_ON_ALIAS,
    .turn_off_name = DIGITAL_SYS_TURN_OFF_ALIAS,
    .toggle_name = DIGITAL_SYS_TOGGLE_ALIAS};

void digital_sys_init(void *ctx)
{
    mc_digital_system_ctx_t *digital_ctx = (mc_digital_system_ctx_t *)ctx;
    mc_result_t current_state = mc_digital_get_state(digital_ctx->device);
    digital_ctx->target_state = current_state.ok ? current_state.value : false;
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

mc_status_t digital_sys_get_alias(void *ctx, uint8_t id, mc_sys_cmd_info_t *info)
{
    mc_digital_system_ctx_t *digital_ctx = (mc_digital_system_ctx_t *)ctx;
    switch (id)
    {
    case 0:
        info->alias = digital_ctx->config->state_name;
        info->type = MC_CMD_TYPE_OUTPUT;
        info->id = 0;
        info->has_preset = false;
        return MC_OK;
    case 1:
        info->alias = digital_ctx->config->target_state_name;
        info->type = MC_CMD_TYPE_INPUT;
        info->id = 0;
        info->has_preset = false;
        return MC_OK;
    case 2:
        info->alias = digital_ctx->config->turn_on_name;
        info->type = MC_CMD_TYPE_INPUT;
        info->id = 0;
        info->has_preset = true;
        info->preset_val = 1;
        return MC_OK;
    case 3:
        info->alias = digital_ctx->config->turn_off_name;
        info->type = MC_CMD_TYPE_INPUT;
        info->id = 0;
        info->has_preset = true;
        info->preset_val = 0;
        return MC_OK;
    case 4:
        info->alias = digital_ctx->config->toggle_name;
        info->type = MC_CMD_TYPE_FUNC;
        info->id = 0;
        info->has_preset = false;
        return MC_OK;
    default:
        return MC_ERROR_INVALID_ARGS;
    }
}

uint8_t digital_sys_get_function_count(void *ctx)
{
    mc_digital_system_ctx_t *digital_ctx = (mc_digital_system_ctx_t *)ctx;
    return digital_ctx->device->config->is_read_only
               ? DIGITAL_SYS_READ_ONLY_F_COUNT
               : DIGITAL_SYS_F_COUNT;
}

uint8_t digital_sys_get_input_count(void *ctx)
{
    mc_digital_system_ctx_t *digital_ctx = (mc_digital_system_ctx_t *)ctx;
    return digital_ctx->device->config->is_read_only
               ? DIGITAL_SYS_READ_ONLY_X_COUNT
               : DIGITAL_SYS_X_COUNT;
}

uint8_t digital_sys_get_output_count(void *ctx)
{
    mc_digital_system_ctx_t *digital_ctx = (mc_digital_system_ctx_t *)ctx;
    return digital_ctx->device->config->is_read_only
               ? DIGITAL_SYS_READ_ONLY_Y_COUNT
               : DIGITAL_SYS_Y_COUNT;
}

uint8_t digital_sys_get_alias_count(void *ctx)
{
    mc_digital_system_ctx_t *digital_ctx = (mc_digital_system_ctx_t *)ctx;
    return digital_ctx->device->config->is_read_only
               ? DIGITAL_SYS_READ_ONLY_ALIAS_COUNT
               : DIGITAL_SYS_ALIAS_COUNT;
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
