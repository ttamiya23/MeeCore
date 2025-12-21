#include "CException.h"
#include "system_test.h"
#include "mc/system/core.h"

mc_sys_status_t increment_y(test_sys_ctx_t *data)
{
    (data->y[0])++;
    return MC_SYS_OK;
}

mc_sys_status_t returns_error(test_sys_ctx_t *data, int32_t error)
{
    return error;
}

void init(void *ctx)
{
    test_sys_ctx_t *data = (test_sys_ctx_t *)ctx;
    memset(data->x, 0, TEST_SYS_X_COUNT);
    memset(data->y, 0, TEST_SYS_Y_COUNT);
}

mc_sys_status_t invoke(void *ctx, uint8_t func_id, int32_t *args,
                       uint8_t arg_count)
{
    switch (func_id)
    {
    case 0:
        return increment_y(ctx);
    case 1:
        return returns_error(ctx, args[0]);
    default:
        Throw(-1); // This should never happen
    }
}

mc_sys_status_t write_input(void *ctx, uint8_t x_id, int32_t val)
{
    test_sys_ctx_t *data = (test_sys_ctx_t *)ctx;
    data->x[x_id] = val;
    return MC_SYS_OK;
}

mc_sys_status_t read_input(void *ctx, uint8_t x_id, int32_t *val)
{
    test_sys_ctx_t *data = (test_sys_ctx_t *)ctx;
    *val = data->x[x_id];
    return MC_SYS_OK;
}

mc_sys_status_t read_output(void *ctx, uint8_t y_id, int32_t *val)
{
    test_sys_ctx_t *data = (test_sys_ctx_t *)ctx;
    *val = data->y[y_id];
    return MC_SYS_OK;
}

uint8_t get_function_count(void *ctx)
{
    return TEST_SYS_F_COUNT;
}

uint8_t get_input_count(void *ctx)
{
    return TEST_SYS_X_COUNT;
}

uint8_t get_output_count(void *ctx)
{
    return TEST_SYS_Y_COUNT;
}

const mc_system_driver_t test_sys_driver = {
    .init = init,
    .invoke = invoke,
    .write_input = write_input,
    .read_input = read_input,
    .read_output = read_output,
    .get_function_count = get_function_count,
    .get_input_count = get_input_count,
    .get_output_count = get_output_count};
