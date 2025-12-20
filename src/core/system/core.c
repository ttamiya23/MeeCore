#include "mc/system/core.h"
#include "mc/common.h"

#define CHECK_SYSTEM(sys)                                        \
    do                                                           \
    {                                                            \
        MC_ASSERT(sys != NULL);                                  \
        MC_ASSERT(sys->state->is_initialized == MC_INITIALIZED); \
    } while (0)

// Helper: convert mc_sys_status_t to mc_status_t
static mc_status_t convert_status(mc_sys_status_t status)
{
    switch (status)
    {
    case MC_SYS_OK:
        return MC_OK;
    case MC_SYS_ERR:
    case MC_SYS_ERR_HW_ERROR:
        return MC_ERROR;
    case MC_SYS_ERR_INVALID_MEMBER:
    case MC_SYS_ERR_INVALID_ARGS:
        return MC_ERROR_INVALID_ARGS;
    case MC_SYS_ERR_NOT_IMPL:
        return MC_ERROR_NOT_SUPPORTED;
    default:
        return MC_ERROR;
    }
}

void mc_sys_init(const mc_system_t *sys)
{
    MC_ASSERT(sys != NULL);
    MC_ASSERT(sys->driver != NULL);
    MC_ASSERT(sys->state != NULL);

    sys->state->is_initialized = MC_INITIALIZED;
    sys->state->status = MC_SYS_OK;

    if (sys->driver->init)
    {
        sys->driver->init(sys->ctx);
    }
}

mc_status_t mc_sys_invoke(const mc_system_t *sys, uint8_t func_id,
                          int32_t *args, uint8_t arg_count)
{
    CHECK_SYSTEM(sys);
    mc_sys_status_t ret = MC_SYS_OK;

    // Only assert if we actually expect arguments
    if (arg_count > 0)
    {
        MC_ASSERT(args != NULL);
    }

    if (!sys->driver->invoke || !sys->driver->get_function_count)
    {
        ret = MC_SYS_ERR_NOT_IMPL;
    }
    else if (func_id >= mc_sys_get_function_count(sys))
    {
        ret = MC_SYS_ERR_INVALID_MEMBER;
    }
    else
    {
        ret = sys->driver->invoke(sys->ctx, func_id, args,
                                  arg_count);
    }
    sys->state->status = ret;
    return convert_status(ret);
}

mc_status_t mc_sys_write_input(const mc_system_t *sys, uint8_t x_id,
                               int32_t val)
{
    CHECK_SYSTEM(sys);
    mc_sys_status_t ret = MC_SYS_OK;

    if (!sys->driver->write_input || !sys->driver->get_input_count)
    {
        ret = MC_SYS_ERR_NOT_IMPL;
    }
    else if (x_id >= mc_sys_get_input_count(sys))
    {
        ret = MC_SYS_ERR_INVALID_MEMBER;
    }
    else
    {
        ret = sys->driver->write_input(sys->ctx, x_id, val);
    }
    sys->state->status = ret;
    return convert_status(ret);
}

mc_status_t mc_sys_read_input(const mc_system_t *sys, uint8_t x_id,
                              int32_t *val)
{
    CHECK_SYSTEM(sys);
    MC_ASSERT(val != NULL);
    mc_sys_status_t ret = MC_SYS_OK;
    *val = 0;

    if (!sys->driver->read_input || !sys->driver->get_input_count)
    {
        ret = MC_SYS_ERR_NOT_IMPL;
    }
    else if (x_id >= mc_sys_get_input_count(sys))
    {
        ret = MC_SYS_ERR_INVALID_MEMBER;
    }
    else
    {
        ret = sys->driver->read_input(sys->ctx, x_id, val);
    }
    sys->state->status = ret;
    return convert_status(ret);
}

mc_status_t mc_sys_read_output(const mc_system_t *sys, uint8_t y_id,
                               int32_t *val)
{
    CHECK_SYSTEM(sys);
    MC_ASSERT(val != NULL);
    mc_sys_status_t ret = MC_SYS_OK;
    *val = 0;

    if (!sys->driver->read_output || !sys->driver->get_output_count)
    {
        ret = MC_SYS_ERR_NOT_IMPL;
    }
    else if (y_id >= mc_sys_get_output_count(sys))
    {
        ret = MC_SYS_ERR_INVALID_MEMBER;
    }
    else
    {
        ret = sys->driver->read_output(sys->ctx, y_id, val);
    }
    sys->state->status = ret;
    return convert_status(ret);
}

uint8_t mc_sys_get_function_count(const mc_system_t *sys)
{
    CHECK_SYSTEM(sys);

    if (!sys->driver->get_function_count)
    {
        return 0;
    }
    return sys->driver->get_function_count(sys->ctx);
}

uint8_t mc_sys_get_input_count(const mc_system_t *sys)
{
    CHECK_SYSTEM(sys);

    if (!sys->driver->get_input_count)
    {
        return 0;
    }
    return sys->driver->get_input_count(sys->ctx);
}

uint8_t mc_sys_get_output_count(const mc_system_t *sys)
{
    CHECK_SYSTEM(sys);

    if (!sys->driver->get_output_count)
    {
        return 0;
    }
    return sys->driver->get_output_count(sys->ctx);
}

mc_sys_status_t mc_sys_get_status(const mc_system_t *sys)
{
    CHECK_SYSTEM(sys);

    return sys->state->status;
}