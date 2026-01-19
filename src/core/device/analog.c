#include "mc/device/analog.h"
#include "mc/utils.h"

#define CHECK_ANALOG(dev)                                         \
    do                                                            \
    {                                                             \
        MC_ASSERT(dev != NULL);                                   \
        MC_ASSERT(dev->config != NULL);                           \
        MC_ASSERT(dev->config->is_initialized == MC_INITIALIZED); \
    } while (0)

void mc_analog_init(const mc_analog_t *dev, bool is_read_only)
{
    MC_ASSERT(dev != NULL);
    MC_ASSERT(dev->config != NULL);
    MC_ASSERT(dev->driver != NULL);
    if (dev->driver->init)
    {
        dev->driver->init(dev->ctx);
    }
    dev->config->is_initialized = MC_INITIALIZED;
    dev->config->is_read_only = is_read_only;
}

mc_status_t mc_analog_set_value(const mc_analog_t *dev, int32_t value)
{
    CHECK_ANALOG(dev);
    if (dev->config->is_read_only)
    {
        return MC_ERROR_WRITE_PROTECTED;
    }
    if (dev->driver->set_value)
    {
        return dev->driver->set_value(dev->ctx, value);
    }
    return MC_ERROR_NOT_SUPPORTED;
}

mc_result_t mc_analog_get_value(const mc_analog_t *dev)
{
    CHECK_ANALOG(dev);
    if (dev->driver->get_value)
    {
        return dev->driver->get_value(dev->ctx);
    }
    return MC_ERR_VAL(MC_ERROR_NOT_SUPPORTED);
}

void mc_analog_set_read_only(const mc_analog_t *dev, bool enable)
{
    CHECK_ANALOG(dev);
    dev->config->is_read_only = enable;
}

// analog data object methods
static void mc_analog_data_object_init(void *ctx)
{
    mc_analog_data_object_ctx_t *analog_ctx =
        (mc_analog_data_object_ctx_t *)ctx;
    analog_ctx->value = 0;
    analog_ctx->error = MC_OK;
}

static mc_status_t mc_analog_data_object_set_value(void *ctx, int32_t value)
{
    mc_analog_data_object_ctx_t *analog_ctx =
        (mc_analog_data_object_ctx_t *)ctx;
    analog_ctx->value = value;
    return analog_ctx->error;
}

static mc_result_t mc_analog_data_object_get_value(void *ctx)
{
    mc_analog_data_object_ctx_t *analog_ctx =
        (mc_analog_data_object_ctx_t *)ctx;
    if (analog_ctx->error != MC_OK)
    {
        return MC_ERR_VAL(analog_ctx->error);
    }
    return MC_OK_VAL(analog_ctx->value);
}

const mc_analog_driver_t mc_analog_data_object_driver = {
    .init = mc_analog_data_object_init,
    .set_value = mc_analog_data_object_set_value,
    .get_value = mc_analog_data_object_get_value};
