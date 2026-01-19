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

// analog vector3 data object methods
static void mc_analog_vector3_data_object_init_x(void *ctx)
{
    mc_analog_vector3_data_object_ctx_t *vector_ctx =
        (mc_analog_vector3_data_object_ctx_t *)ctx;
    vector_ctx->x = 0;
    vector_ctx->error = MC_OK;
}

static void mc_analog_vector3_data_object_init_y(void *ctx)
{
    mc_analog_vector3_data_object_ctx_t *vector_ctx =
        (mc_analog_vector3_data_object_ctx_t *)ctx;
    vector_ctx->y = 0;
    vector_ctx->error = MC_OK;
}

static void mc_analog_vector3_data_object_init_z(void *ctx)
{
    mc_analog_vector3_data_object_ctx_t *vector_ctx =
        (mc_analog_vector3_data_object_ctx_t *)ctx;
    vector_ctx->z = 0;
    vector_ctx->error = MC_OK;
}

static mc_status_t mc_analog_vector3_data_object_set_x(void *ctx, int32_t value)
{
    mc_analog_vector3_data_object_ctx_t *vector_ctx =
        (mc_analog_vector3_data_object_ctx_t *)ctx;
    vector_ctx->x = value;
    return vector_ctx->error;
}

static mc_status_t mc_analog_vector3_data_object_set_y(void *ctx, int32_t value)
{
    mc_analog_vector3_data_object_ctx_t *vector_ctx =
        (mc_analog_vector3_data_object_ctx_t *)ctx;
    vector_ctx->y = value;
    return vector_ctx->error;
}

static mc_status_t mc_analog_vector3_data_object_set_z(void *ctx, int32_t value)
{
    mc_analog_vector3_data_object_ctx_t *vector_ctx =
        (mc_analog_vector3_data_object_ctx_t *)ctx;
    vector_ctx->z = value;
    return vector_ctx->error;
}

static mc_result_t mc_analog_vector3_data_object_get_x(void *ctx)
{
    mc_analog_vector3_data_object_ctx_t *vector_ctx =
        (mc_analog_vector3_data_object_ctx_t *)ctx;
    if (vector_ctx->error != MC_OK)
    {
        return MC_ERR_VAL(vector_ctx->error);
    }
    return MC_OK_VAL(vector_ctx->x);
}

static mc_result_t mc_analog_vector3_data_object_get_y(void *ctx)
{
    mc_analog_vector3_data_object_ctx_t *vector_ctx =
        (mc_analog_vector3_data_object_ctx_t *)ctx;
    if (vector_ctx->error != MC_OK)
    {
        return MC_ERR_VAL(vector_ctx->error);
    }
    return MC_OK_VAL(vector_ctx->y);
}

static mc_result_t mc_analog_vector3_data_object_get_z(void *ctx)
{
    mc_analog_vector3_data_object_ctx_t *vector_ctx =
        (mc_analog_vector3_data_object_ctx_t *)ctx;
    if (vector_ctx->error != MC_OK)
    {
        return MC_ERR_VAL(vector_ctx->error);
    }
    return MC_OK_VAL(vector_ctx->z);
}

const mc_analog_vector3_driver_t mc_analog_vector3_data_object_driver = {
    .x = {.init = mc_analog_vector3_data_object_init_x, .set_value = mc_analog_vector3_data_object_set_x, .get_value = mc_analog_vector3_data_object_get_x},
    .y = {.init = mc_analog_vector3_data_object_init_y, .set_value = mc_analog_vector3_data_object_set_y, .get_value = mc_analog_vector3_data_object_get_y},
    .z = {.init = mc_analog_vector3_data_object_init_z, .set_value = mc_analog_vector3_data_object_set_z, .get_value = mc_analog_vector3_data_object_get_z}};