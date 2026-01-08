#include "mc/device/analog_vector3.h"
#include "mc/utils.h"

void mc_analog_vector3_init(const mc_analog_vector3_t *dev, bool is_read_only)
{
    MC_ASSERT(dev != NULL);
    mc_analog_init(&dev->x, is_read_only);
    mc_analog_init(&dev->y, is_read_only);
    mc_analog_init(&dev->z, is_read_only);
}

void mc_analog_vector3_set_read_only(const mc_analog_vector3_t *dev, bool enable)
{
    MC_ASSERT(dev != NULL);
    mc_analog_set_read_only(&dev->x, enable);
    mc_analog_set_read_only(&dev->y, enable);
    mc_analog_set_read_only(&dev->z, enable);
}
