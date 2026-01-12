#include "device/fake_analog.h"
#include "mc/utils.h"

void fake_analog_init(void *ctx)
{
    fake_analog_ctx_t *analog_ctx = (fake_analog_ctx_t *)ctx;
    analog_ctx->is_initialized = MC_INITIALIZED;
    analog_ctx->value = 0;
    analog_ctx->error = MC_OK;
}

mc_status_t fake_analog_set_value(void *ctx, int32_t value)
{
    fake_analog_ctx_t *analog_ctx = (fake_analog_ctx_t *)ctx;
    analog_ctx->value = value;
    return analog_ctx->error;
}

mc_result_t fake_analog_get_value(void *ctx)
{
    fake_analog_ctx_t *analog_ctx = (fake_analog_ctx_t *)ctx;
    if (analog_ctx->error != MC_OK)
    {
        return MC_ERR_VAL(analog_ctx->error);
    }
    return MC_OK_VAL(analog_ctx->value);
}

const mc_analog_driver_t fake_analog_driver = {
    .init = fake_analog_init,
    .set_value = fake_analog_set_value,
    .get_value = fake_analog_get_value};
