#include "mc/device/digital.h"
#include "mc/utils.h"
#include <stdbool.h>

#define CHECK_DIGITAL(dev)                                        \
    do                                                            \
    {                                                             \
        MC_ASSERT(dev != NULL);                                   \
        MC_ASSERT(dev->config != NULL);                           \
        MC_ASSERT(dev->config->is_initialized == MC_INITIALIZED); \
    } while (0)

void mc_digital_init(const mc_digital_t *dev, bool is_read_only)
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

mc_status_t mc_digital_set_state(const mc_digital_t *dev, bool state)
{
    CHECK_DIGITAL(dev);
    if (dev->config->is_read_only)
    {
        return MC_ERROR_WRITE_PROTECTED;
    }
    if (dev->driver->set_state)
    {
        return dev->driver->set_state(dev->ctx, state);
    }
    return MC_ERROR_NOT_SUPPORTED;
}

mc_result_t mc_digital_get_state(const mc_digital_t *dev)
{
    CHECK_DIGITAL(dev);
    if (dev->driver->get_state)
    {
        return dev->driver->get_state(dev->ctx);
    }
    return MC_ERR_VAL(MC_ERROR_NOT_SUPPORTED);
}

mc_status_t mc_digital_toggle(const mc_digital_t *dev)
{
    MC_ASSIGN_OR_RETURN(current, mc_digital_get_state(dev));
    return mc_digital_set_state(dev, !current);
}

void mc_digital_set_read_only(const mc_digital_t *dev,
                              bool enable)
{
    CHECK_DIGITAL(dev);
    dev->config->is_read_only = enable;
}
