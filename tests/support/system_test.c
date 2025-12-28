#include "CException.h"
#include "system_test.h"
#include "mc/system/core.h"
#include <string.h>

mc_status_t test_sys_increment_y(test_sys_ctx_t *data)
{
    (data->y[0])++;
    return MC_OK;
}

mc_status_t test_sys_returns_error(test_sys_ctx_t *data, int32_t error)
{
    return error;
}

void test_sys_init(void *ctx)
{
    test_sys_ctx_t *data = (test_sys_ctx_t *)ctx;
    memset(data->x, 0, TEST_SYS_X_COUNT);
    memset(data->y, 0, TEST_SYS_Y_COUNT);
}

mc_status_t test_sys_invoke(void *ctx, uint8_t func_id, int32_t *args,
                            uint8_t arg_count)
{
    switch (func_id)
    {
    case 0:
        return test_sys_increment_y(ctx);
    case 1:
        return test_sys_returns_error(ctx, args[0]);
    default:
        Throw(-1); // This should never happen
    }
}

mc_status_t test_sys_write_input(void *ctx, uint8_t x_id, int32_t val)
{
    test_sys_ctx_t *data = (test_sys_ctx_t *)ctx;
    data->x[x_id] = val;
    return MC_OK;
}

mc_result_t test_sys_read_input(void *ctx, uint8_t x_id)
{
    test_sys_ctx_t *data = (test_sys_ctx_t *)ctx;
    return MC_OK_VAL(data->x[x_id]);
}

mc_result_t test_sys_read_output(void *ctx, uint8_t y_id)
{
    test_sys_ctx_t *data = (test_sys_ctx_t *)ctx;
    return MC_OK_VAL(data->y[y_id]);
}

bool test_sys_parse_command(void *ctx, const char *cmd, mc_sys_cmd_info_t *info)
{
    test_sys_ctx_t *data = (test_sys_ctx_t *)ctx;
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

uint8_t test_sys_get_function_count(void *ctx)
{
    return TEST_SYS_F_COUNT;
}

uint8_t test_sys_get_input_count(void *ctx)
{
    return TEST_SYS_X_COUNT;
}

uint8_t test_sys_get_output_count(void *ctx)
{
    return TEST_SYS_Y_COUNT;
}

const mc_system_driver_t test_sys_driver = {
    .init = test_sys_init,
    .invoke = test_sys_invoke,
    .write_input = test_sys_write_input,
    .read_input = test_sys_read_input,
    .read_output = test_sys_read_output,
    .parse_command = test_sys_parse_command,
    .get_function_count = test_sys_get_function_count,
    .get_input_count = test_sys_get_input_count,
    .get_output_count = test_sys_get_output_count};
