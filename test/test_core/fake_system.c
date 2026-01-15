#include "fake_system.h"
#include "mc/system/core.h"
#include <string.h>

mc_status_t fake_sys_increment_y(fake_sys_ctx_t *data)
{
    (data->y[0])++;
    return MC_OK;
}

mc_status_t fake_sys_returns_error(fake_sys_ctx_t *data, int32_t error)
{
    return (mc_status_t)error;
}

void fake_sys_init(void *ctx)
{
    fake_sys_ctx_t *data = (fake_sys_ctx_t *)ctx;
    memset(data->x, 0, FAKE_SYS_X_COUNT);
    memset(data->y, 0, FAKE_SYS_Y_COUNT);
    data->increment_y_name = DEFAULT_FAKE_INCREMENT_Y_NAME;
    data->x0_name = (const char *)DEFAULT_FAKE_X0_NAME;
    data->y0_name = (const char *)DEFAULT_FAKE_Y0_NAME;
    data->reset_name = (const char *)DEFAULT_FAKE_RESET_NAME;
}

mc_status_t fake_sys_invoke(void *ctx, uint8_t func_id, int32_t *args,
                            uint8_t arg_count)
{
    switch (func_id)
    {
    case 0:
        return fake_sys_increment_y((fake_sys_ctx_t *)ctx);
    case 1:
        return fake_sys_returns_error((fake_sys_ctx_t *)ctx, args[0]);
    default:
        // This should never happen
        return MC_ERROR;
    }
}

mc_status_t fake_sys_write_input(void *ctx, uint8_t x_id, int32_t val)
{
    fake_sys_ctx_t *data = (fake_sys_ctx_t *)ctx;
    data->x[x_id] = val;
    return MC_OK;
}

mc_result_t fake_sys_read_input(void *ctx, uint8_t x_id)
{
    fake_sys_ctx_t *data = (fake_sys_ctx_t *)ctx;
    return MC_OK_VAL(data->x[x_id]);
}

mc_result_t fake_sys_read_output(void *ctx, uint8_t y_id)
{
    fake_sys_ctx_t *data = (fake_sys_ctx_t *)ctx;
    return MC_OK_VAL(data->y[y_id]);
}

mc_status_t fake_sys_get_alias(void *ctx, uint8_t id, mc_sys_cmd_info_t *info)
{
    fake_sys_ctx_t *data = (fake_sys_ctx_t *)ctx;
    switch (id)
    {
    case 0:
        info->alias = data->increment_y_name;
        info->type = MC_CMD_TYPE_FUNC;
        info->id = 0;
        info->has_preset = false;
        return MC_OK;
    case 1:
        info->alias = data->x0_name;
        info->type = MC_CMD_TYPE_INPUT;
        info->id = 0;
        info->has_preset = false;
        return MC_OK;
    case 2:
        info->alias = data->y0_name;
        info->type = MC_CMD_TYPE_OUTPUT;
        info->id = 0;
        info->has_preset = false;
        return MC_OK;
    case 3:
        info->alias = data->reset_name;
        info->type = MC_CMD_TYPE_INPUT;
        info->id = 0;
        info->has_preset = true;
        info->preset_val = 0;
        return MC_OK;
    default:
        return MC_ERROR_INVALID_ARGS;
    }
}

uint8_t fake_sys_get_function_count(void *ctx)
{
    return FAKE_SYS_F_COUNT;
}

uint8_t fake_sys_get_input_count(void *ctx)
{
    return FAKE_SYS_X_COUNT;
}

uint8_t fake_sys_get_output_count(void *ctx)
{
    return FAKE_SYS_Y_COUNT;
}

uint8_t fake_sys_get_alias_count(void *ctx)
{
    return FAKE_SYS_ALIAS_COUNT;
}

const mc_system_driver_t fake_sys_driver = {
    .init = fake_sys_init,
    .invoke = fake_sys_invoke,
    .write_input = fake_sys_write_input,
    .read_input = fake_sys_read_input,
    .read_output = fake_sys_read_output,
    .get_alias = fake_sys_get_alias,
    .get_function_count = fake_sys_get_function_count,
    .get_input_count = fake_sys_get_input_count,
    .get_output_count = fake_sys_get_output_count,
    .get_alias_count = fake_sys_get_alias_count};
