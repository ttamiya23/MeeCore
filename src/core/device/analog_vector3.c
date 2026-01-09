#include "mc/device/analog_vector3.h"
#include "mc/utils.h"

void mc_analog_vector3_init(const mc_analog_vector3_t *dev, bool is_read_only)
{
    MC_ASSERT(dev != NULL);
    mc_analog_init(&dev->x, is_read_only);
    mc_analog_init(&dev->y, is_read_only);
    mc_analog_init(&dev->z, is_read_only);
}

mc_status_t mc_analog_vector3_set_value(const mc_analog_vector3_t *dev,
                                        mc_vector3_t value)
{
    MC_ASSERT(dev != NULL);
    MC_RETURN_IF_ERROR(mc_analog_set_value(&dev->x, value.x));
    MC_RETURN_IF_ERROR(mc_analog_set_value(&dev->y, value.y));
    MC_RETURN_IF_ERROR(mc_analog_set_value(&dev->z, value.z));
    return MC_OK;
}

mc_status_t mc_analog_vector3_get_value(const mc_analog_vector3_t *dev,
                                        mc_vector3_t *value)
{
    MC_ASSERT(dev != NULL);
    MC_ASSERT(value != NULL);
    MC_ASSIGN_OR_RETURN(x, mc_analog_get_value(&dev->x));
    MC_ASSIGN_OR_RETURN(y, mc_analog_get_value(&dev->y));
    MC_ASSIGN_OR_RETURN(z, mc_analog_get_value(&dev->z));
    value->x = x;
    value->y = y;
    value->z = z;
    return MC_OK;
}

void mc_analog_vector3_set_read_only(const mc_analog_vector3_t *dev, bool enable)
{
    MC_ASSERT(dev != NULL);
    mc_analog_set_read_only(&dev->x, enable);
    mc_analog_set_read_only(&dev->y, enable);
    mc_analog_set_read_only(&dev->z, enable);
}
