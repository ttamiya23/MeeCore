#include "mc/system/modules/analog_system.h"
#include <string.h>

#define ANALOG_SYS_X_COUNT 1
#define ANALOG_SYS_Y_COUNT 1
#define ANALOG_SYS_F_COUNT 0
#define ANALOG_SYS_ALIAS_COUNT 2
#define ANALOG_SYS_READ_ONLY_X_COUNT 0
#define ANALOG_SYS_READ_ONLY_ALIAS_COUNT 1
#define ANALOG_SYS_VALUE_ALIAS "value"
#define ANALOG_SYS_TARGET_VALUE_ALIAS "targetValue"

const mc_analog_system_config_t mc_analog_sys_config = {
    .value_name = ANALOG_SYS_VALUE_ALIAS,
    .target_value_name = ANALOG_SYS_TARGET_VALUE_ALIAS};

void analog_sys_init(void *ctx)
{
    mc_analog_system_ctx_t *analog_ctx = (mc_analog_system_ctx_t *)ctx;
    mc_result_t current_value = mc_analog_get_value(analog_ctx->device);
    analog_ctx->target_value = current_value.ok ? current_value.value : 0;
}

mc_status_t analog_sys_write_input(void *ctx, uint8_t x_id, int32_t val)
{
    mc_analog_system_ctx_t *analog_ctx = (mc_analog_system_ctx_t *)ctx;
    analog_ctx->target_value = val;
    return mc_analog_set_value(analog_ctx->device, val);
}

mc_result_t analog_sys_read_input(void *ctx, uint8_t x_id)
{
    mc_analog_system_ctx_t *analog_ctx = (mc_analog_system_ctx_t *)ctx;
    return MC_OK_VAL(analog_ctx->target_value);
}

mc_result_t analog_sys_read_output(void *ctx, uint8_t y_id)
{
    mc_analog_system_ctx_t *analog_ctx = (mc_analog_system_ctx_t *)ctx;
    return mc_analog_get_value(analog_ctx->device);
}

mc_status_t analog_sys_get_alias(void *ctx, uint8_t id, mc_sys_cmd_info_t *info)
{
    mc_analog_system_ctx_t *analog_ctx = (mc_analog_system_ctx_t *)ctx;
    switch (id)
    {
    case 0:
        info->alias = analog_ctx->config->value_name;
        info->type = MC_CMD_TYPE_OUTPUT;
        info->id = 0;
        info->has_preset = false;
        return MC_OK;
    case 1:
        info->alias = analog_ctx->config->target_value_name;
        info->type = MC_CMD_TYPE_INPUT;
        info->id = 0;
        info->has_preset = false;
        return MC_OK;
    default:
        return MC_ERROR_INVALID_ARGS;
    }
}

uint8_t analog_sys_get_function_count(void *ctx)
{
    return ANALOG_SYS_F_COUNT;
}

uint8_t analog_sys_get_input_count(void *ctx)
{
    mc_analog_system_ctx_t *analog_ctx = (mc_analog_system_ctx_t *)ctx;
    return analog_ctx->device->config->is_read_only
               ? ANALOG_SYS_READ_ONLY_X_COUNT
               : ANALOG_SYS_X_COUNT;
}

uint8_t analog_sys_get_output_count(void *ctx)
{
    return ANALOG_SYS_Y_COUNT;
}

uint8_t analog_sys_get_alias_count(void *ctx)
{
    mc_analog_system_ctx_t *analog_ctx = (mc_analog_system_ctx_t *)ctx;
    return analog_ctx->device->config->is_read_only
               ? ANALOG_SYS_READ_ONLY_ALIAS_COUNT
               : ANALOG_SYS_ALIAS_COUNT;
}

const mc_system_driver_t mc_analog_sys_driver = {
    .init = analog_sys_init,
    .invoke = NULL,
    .write_input = analog_sys_write_input,
    .read_input = analog_sys_read_input,
    .read_output = analog_sys_read_output,
    .get_alias = analog_sys_get_alias,
    .get_function_count = analog_sys_get_function_count,
    .get_input_count = analog_sys_get_input_count,
    .get_output_count = analog_sys_get_output_count,
    .get_alias_count = analog_sys_get_alias_count};
