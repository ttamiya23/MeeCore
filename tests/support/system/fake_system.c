#include "CException.h"
#include "system/fake_system.h"
#include "mc/system/core.h"
#include <string.h>

mc_status_t fake_sys_increment_y(fake_sys_ctx_t *data)
{
    (data->y[0])++;
    return MC_OK;
}

mc_status_t fake_sys_returns_error(fake_sys_ctx_t *data, int32_t error)
{
    return error;
}

void fake_sys_init(void *ctx)
{
    fake_sys_ctx_t *data = (fake_sys_ctx_t *)ctx;
    memset(data->x, 0, FAKE_SYS_X_COUNT);
    memset(data->y, 0, FAKE_SYS_Y_COUNT);
}

mc_status_t fake_sys_invoke(void *ctx, uint8_t func_id, int32_t *args,
                            uint8_t arg_count)
{
    switch (func_id)
    {
    case 0:
        return fake_sys_increment_y(ctx);
    case 1:
        return fake_sys_returns_error(ctx, args[0]);
    default:
        Throw(-1); // This should never happen
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

bool fake_sys_parse_command(void *ctx, const char *cmd, mc_sys_cmd_info_t *info)
{
    fake_sys_ctx_t *data = (fake_sys_ctx_t *)ctx;
    if (strcmp(cmd, data->increment_y_name) == 0)
    {
        info->type = MC_CMD_TYPE_FUNC;
        info->id = 0;
        info->has_preset = false;
        return true;
    }
    else if (strcmp(cmd, data->x0_name) == 0)
    {
        info->type = MC_CMD_TYPE_INPUT;
        info->id = 0;
        info->has_preset = false;
        return true;
    }
    else if (strcmp(cmd, data->y0_name) == 0)
    {
        info->type = MC_CMD_TYPE_OUTPUT;
        info->id = 0;
        info->has_preset = false;
        return true;
    }

    return false;
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

const mc_system_driver_t fake_sys_driver = {
    .init = fake_sys_init,
    .invoke = fake_sys_invoke,
    .write_input = fake_sys_write_input,
    .read_input = fake_sys_read_input,
    .read_output = fake_sys_read_output,
    .parse_command = fake_sys_parse_command,
    .get_function_count = fake_sys_get_function_count,
    .get_input_count = fake_sys_get_input_count,
    .get_output_count = fake_sys_get_output_count};
