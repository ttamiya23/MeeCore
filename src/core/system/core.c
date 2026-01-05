#include "mc/system/core.h"
#include "mc/status.h"
#include "mc/utils.h"

#define CHECK_SYSTEM(sys)                                        \
    do                                                           \
    {                                                            \
        MC_ASSERT(sys != NULL);                                  \
        MC_ASSERT(sys->state->is_initialized == MC_INITIALIZED); \
    } while (0)

void mc_sys_init(const mc_system_t *sys)
{
    MC_ASSERT(sys != NULL);
    MC_ASSERT(sys->driver != NULL);
    MC_ASSERT(sys->state != NULL);

    if (sys->driver->init)
    {
        sys->driver->init(sys->ctx);
    }
    sys->state->is_initialized = MC_INITIALIZED;
}

mc_status_t mc_sys_invoke(const mc_system_t *sys, uint8_t func_id,
                          int32_t *args, uint8_t arg_count)
{
    CHECK_SYSTEM(sys);

    // Only assert if we actually expect arguments
    if (arg_count > 0)
    {
        MC_ASSERT(args != NULL);
    }
    if (!sys->driver->invoke || !sys->driver->get_function_count)
    {
        return MC_ERROR_NOT_SUPPORTED;
    }
    if (func_id >= mc_sys_get_function_count(sys))
    {
        return MC_ERROR_INVALID_ARGS;
    }
    return sys->driver->invoke(sys->ctx, func_id, args, arg_count);
}

mc_status_t mc_sys_write_input(const mc_system_t *sys, uint8_t x_id,
                               int32_t val)
{
    CHECK_SYSTEM(sys);

    if (!sys->driver->write_input || !sys->driver->get_input_count)
    {
        return MC_ERROR_NOT_SUPPORTED;
    }
    if (x_id >= mc_sys_get_input_count(sys))
    {
        return MC_ERROR_INVALID_ARGS;
    }
    return sys->driver->write_input(sys->ctx, x_id, val);
}

mc_result_t mc_sys_read_input(const mc_system_t *sys, uint8_t x_id)
{
    CHECK_SYSTEM(sys);

    if (!sys->driver->read_input || !sys->driver->get_input_count)
    {
        return MC_ERR_VAL(MC_ERROR_NOT_SUPPORTED);
    }
    if (x_id >= mc_sys_get_input_count(sys))
    {
        return MC_ERR_VAL(MC_ERROR_INVALID_ARGS);
    }
    return sys->driver->read_input(sys->ctx, x_id);
}

mc_result_t mc_sys_read_output(const mc_system_t *sys, uint8_t y_id)
{
    CHECK_SYSTEM(sys);

    if (!sys->driver->read_output || !sys->driver->get_output_count)
    {
        return MC_ERR_VAL(MC_ERROR_NOT_SUPPORTED);
    }
    if (y_id >= mc_sys_get_output_count(sys))
    {
        return MC_ERR_VAL(MC_ERROR_INVALID_ARGS);
    }
    return sys->driver->read_output(sys->ctx, y_id);
}

mc_status_t mc_sys_get_alias(const mc_system_t *sys, uint8_t id,
                             mc_sys_cmd_info_t *info)
{
    CHECK_SYSTEM(sys);
    MC_ASSERT(info != NULL);

    if (!sys->driver->get_alias || !sys->driver->get_alias_count)
    {
        return MC_ERROR_NOT_SUPPORTED;
    }
    if (id >= mc_sys_get_alias_count(sys))
    {
        return MC_ERROR_INVALID_ARGS;
    }
    return sys->driver->get_alias(sys->ctx, id, info);
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

uint8_t mc_sys_get_alias_count(const mc_system_t *sys)
{
    CHECK_SYSTEM(sys);

    if (!sys->driver->get_alias_count)
    {
        return 0;
    }
    return sys->driver->get_alias_count(sys->ctx);
}