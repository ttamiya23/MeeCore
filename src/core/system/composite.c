#include "mc/system/composite.h"
#include "mc/utils.h"

#define CHECK_COMPOSITE(ctx, driver) \
    do                               \
    {                                \
        MC_ASSERT(ctx != NULL);      \
        MC_ASSERT(driver != NULL);   \
    } while (0)

// Helper to get a pointer to the child's context safely
static inline void *get_child_ctx(void *parent_ctx,
                                  const mc_composite_driver_t *driver,
                                  uint8_t index)
{
    // Base address + Offset defined in the macro
    return (void *)((char *)parent_ctx + driver->ctx_offsets[index]);
}

void mc_composite_init(void *ctx, const mc_composite_driver_t *driver)
{
    CHECK_COMPOSITE(ctx, driver);

    for (uint8_t i = 0; i < driver->count; i++)
    {
        const mc_system_driver_t *child_drv = driver->systems[i];
        void *child_ctx = get_child_ctx(ctx, driver, i);

        // Recursively call init if the child driver has it
        if (child_drv && child_drv->init)
        {
            child_drv->init(child_ctx);
        }
    }
}

mc_status_t mc_composite_write_input(void *ctx,
                                     const mc_composite_driver_t *driver,
                                     uint8_t id, int32_t val)
{
    CHECK_COMPOSITE(ctx, driver);

    for (uint8_t i = 0; i < driver->count; i++)
    {
        const mc_system_driver_t *child_drv = driver->systems[i];
        void *child_ctx = get_child_ctx(ctx, driver, i);

        // Ask the child how many inputs it has
        uint8_t local_count = 0;
        if (child_drv->get_input_count)
        {
            local_count = child_drv->get_input_count(child_ctx);
        }

        // Is the ID meant for this child?
        if (id < local_count)
        {
            // Yes! Dispatch to this child using the local ID
            if (child_drv->write_input)
            {
                return child_drv->write_input(child_ctx, id, val);
            }
            else
            {
                return MC_ERROR_NOT_SUPPORTED;
            }
        }

        // No, the ID is for a subsequent child.
        // Subtract this child's count from the ID and continue.
        id -= local_count;
    }

    return MC_ERROR_INVALID_ARGS; // ID exceeded total inputs
}

mc_result_t mc_composite_read_input(void *ctx,
                                    const mc_composite_driver_t *driver,
                                    uint8_t id)
{
    CHECK_COMPOSITE(ctx, driver);

    for (uint8_t i = 0; i < driver->count; i++)
    {
        const mc_system_driver_t *child_drv = driver->systems[i];
        void *child_ctx = get_child_ctx(ctx, driver, i);

        uint8_t local_count = 0;
        if (child_drv->get_input_count)
        {
            local_count = child_drv->get_input_count(child_ctx);
        }

        if (id < local_count)
        {
            if (child_drv->read_input)
            {
                return child_drv->read_input(child_ctx, id);
            }
            else
            {
                return MC_ERR_VAL(MC_ERROR_NOT_SUPPORTED);
            }
        }
        id -= local_count;
    }

    return MC_ERR_VAL(MC_ERROR_INVALID_ARGS);
}

mc_result_t mc_composite_read_output(void *ctx,
                                     const mc_composite_driver_t *driver,
                                     uint8_t id)
{
    CHECK_COMPOSITE(ctx, driver);

    for (uint8_t i = 0; i < driver->count; i++)
    {
        const mc_system_driver_t *child_drv = driver->systems[i];
        void *child_ctx = get_child_ctx(ctx, driver, i);

        uint8_t local_count = 0;
        if (child_drv->get_output_count)
        {
            local_count = child_drv->get_output_count(child_ctx);
        }

        if (id < local_count)
        {
            if (child_drv->read_output)
            {
                return child_drv->read_output(child_ctx, id);
            }
            else
            {
                return MC_ERR_VAL(MC_ERROR_NOT_SUPPORTED);
            }
        }
        id -= local_count;
    }

    return MC_ERR_VAL(MC_ERROR_INVALID_ARGS);
}

mc_status_t mc_composite_invoke(void *ctx, const mc_composite_driver_t *driver,
                                uint8_t id, int32_t *args, uint8_t arg_count)
{
    CHECK_COMPOSITE(ctx, driver);

    for (uint8_t i = 0; i < driver->count; i++)
    {
        const mc_system_driver_t *child_drv = driver->systems[i];
        void *child_ctx = get_child_ctx(ctx, driver, i);

        uint8_t local_count = 0;
        if (child_drv->get_function_count)
        {
            local_count = child_drv->get_function_count(child_ctx);
        }

        if (id < local_count)
        {
            if (child_drv->invoke)
            {
                return child_drv->invoke(child_ctx, id, args, arg_count);
            }
            else
            {
                return MC_ERROR_NOT_SUPPORTED;
            }
        }
        id -= local_count;
    }

    return MC_ERROR_INVALID_ARGS;
}

mc_status_t mc_composite_get_alias(
    void *ctx, const mc_composite_driver_t *driver, uint8_t id,
    mc_sys_cmd_info_t *info)
{
    CHECK_COMPOSITE(ctx, driver);
    uint8_t f_offset = 0;
    uint8_t x_offset = 0;
    uint8_t y_offset = 0;

    for (uint8_t i = 0; i < driver->count; i++)
    {
        const mc_system_driver_t *child_drv = driver->systems[i];
        void *child_ctx = get_child_ctx(ctx, driver, i);

        uint8_t local_count = 0;
        if (child_drv->get_alias_count)
        {
            local_count = child_drv->get_alias_count(child_ctx);
        }

        if (id < local_count)
        {
            if (child_drv->get_alias)
            {
                mc_status_t result = child_drv->get_alias(child_ctx, id, info);
                if (info->type == MC_CMD_TYPE_FUNC)
                {
                    info->id += f_offset;
                }
                else if (info->type == MC_CMD_TYPE_INPUT)
                {
                    info->id += x_offset;
                }
                else
                {
                    info->id += y_offset;
                }
                return result;
            }
            else
            {
                return MC_ERROR_NOT_SUPPORTED;
            }
        }
        id -= local_count;

        // Increment offsets
        if (child_drv->get_function_count)
        {
            f_offset += child_drv->get_function_count(child_ctx);
        }
        if (child_drv->get_input_count)
        {
            x_offset += child_drv->get_input_count(child_ctx);
        }
        if (child_drv->get_output_count)
        {
            y_offset += child_drv->get_output_count(child_ctx);
        }
    }

    return MC_ERROR_INVALID_ARGS;
}

// --- Counting Functions ---
uint8_t mc_composite_get_input_count(void *ctx,
                                     const mc_composite_driver_t *driver)
{
    CHECK_COMPOSITE(ctx, driver);

    uint8_t total = 0;
    for (uint8_t i = 0; i < driver->count; i++)
    {
        const mc_system_driver_t *d = driver->systems[i];
        if (d && d->get_input_count)
        {
            total += d->get_input_count(get_child_ctx(ctx, driver, i));
        }
    }
    return total;
}

uint8_t mc_composite_get_output_count(void *ctx,
                                      const mc_composite_driver_t *driver)
{
    CHECK_COMPOSITE(ctx, driver);

    uint8_t total = 0;
    for (uint8_t i = 0; i < driver->count; i++)
    {
        const mc_system_driver_t *d = driver->systems[i];
        if (d && d->get_output_count)
        {
            total += d->get_output_count(get_child_ctx(ctx, driver, i));
        }
    }
    return total;
}

uint8_t mc_composite_get_function_count(void *ctx,
                                        const mc_composite_driver_t *driver)
{
    CHECK_COMPOSITE(ctx, driver);

    uint8_t total = 0;
    for (uint8_t i = 0; i < driver->count; i++)
    {
        const mc_system_driver_t *d = driver->systems[i];
        if (d && d->get_function_count)
        {
            total += d->get_function_count(get_child_ctx(ctx, driver, i));
        }
    }
    return total;
}

uint8_t mc_composite_get_alias_count(void *ctx,
                                     const mc_composite_driver_t *driver)
{
    CHECK_COMPOSITE(ctx, driver);

    uint8_t total = 0;
    for (uint8_t i = 0; i < driver->count; i++)
    {
        const mc_system_driver_t *d = driver->systems[i];
        if (d && d->get_alias_count)
        {
            total += d->get_alias_count(get_child_ctx(ctx, driver, i));
        }
    }
    return total;
}