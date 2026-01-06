#include "device/fake_analog_vector.h"
#include "device/fake_analog.h"
#include "mc/device/analog_vector.h"
#include "mc/device/analog.h"
#include "mc/utils.h"

void fake_analog_init_x(void *ctx)
{
    fake_analog_vector_ctx_t *vector_ctx = (fake_analog_vector_ctx_t *)ctx;
    fake_analog_driver.init(&vector_ctx->x);
}

void fake_analog_init_y(void *ctx)
{
    fake_analog_vector_ctx_t *vector_ctx = (fake_analog_vector_ctx_t *)ctx;
    fake_analog_driver.init(&vector_ctx->y);
}

void fake_analog_init_z(void *ctx)
{
    fake_analog_vector_ctx_t *vector_ctx = (fake_analog_vector_ctx_t *)ctx;
    fake_analog_driver.init(&vector_ctx->z);
}

mc_status_t fake_analog_set_x(void *ctx, int32_t value)
{
    fake_analog_vector_ctx_t *vector_ctx = (fake_analog_vector_ctx_t *)ctx;
    fake_analog_driver.set_value(&vector_ctx->x, value);
}

mc_status_t fake_analog_set_y(void *ctx, int32_t value)
{
    fake_analog_vector_ctx_t *vector_ctx = (fake_analog_vector_ctx_t *)ctx;
    fake_analog_driver.set_value(&vector_ctx->y, value);
}

mc_status_t fake_analog_set_z(void *ctx, int32_t value)
{
    fake_analog_vector_ctx_t *vector_ctx = (fake_analog_vector_ctx_t *)ctx;
    fake_analog_driver.set_value(&vector_ctx->z, value);
}

mc_result_t fake_analog_get_x(void *ctx)
{
    fake_analog_vector_ctx_t *vector_ctx = (fake_analog_vector_ctx_t *)ctx;
    fake_analog_driver.get_value(&vector_ctx->x);
}

mc_result_t fake_analog_get_y(void *ctx)
{
    fake_analog_vector_ctx_t *vector_ctx = (fake_analog_vector_ctx_t *)ctx;
    fake_analog_driver.get_value(&vector_ctx->y);
}

mc_result_t fake_analog_get_z(void *ctx)
{
    fake_analog_vector_ctx_t *vector_ctx = (fake_analog_vector_ctx_t *)ctx;
    fake_analog_driver.get_value(&vector_ctx->z);
}

const mc_analog_vector_driver_t fake_analog_vector_driver = {
    .x = {.init = fake_analog_init_x, .set_value = fake_analog_set_x, .get_value = fake_analog_get_x},
    .y = {.init = fake_analog_init_y, .set_value = fake_analog_set_y, .get_value = fake_analog_get_y},
    .z = {.init = fake_analog_init_z, .set_value = fake_analog_set_z, .get_value = fake_analog_get_z}};