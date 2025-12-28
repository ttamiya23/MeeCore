#include "device/fake_digital_driver.h"
#include "mc/utils.h"

void fake_digital_driver_init(void *ctx)
{
    fake_digital_ctx_t *digital_ctx = (fake_digital_ctx_t *)ctx;
    digital_ctx->is_initialized = MC_INITIALIZED;
    digital_ctx->state = false;
    digital_ctx->error = MC_OK;
}

mc_status_t fake_digital_driver_set_state(void *ctx, bool state)
{
    fake_digital_ctx_t *digital_ctx = (fake_digital_ctx_t *)ctx;
    digital_ctx->state = state;
    return digital_ctx->error;
}

mc_result_t fake_digital_driver_get_state(void *ctx)
{
    fake_digital_ctx_t *digital_ctx = (fake_digital_ctx_t *)ctx;
    if (digital_ctx->error != MC_OK)
    {
        return MC_ERR_VAL(digital_ctx->error);
    }
    return MC_OK_VAL(digital_ctx->state);
}

const mc_digital_driver_t fake_digital_driver = {
    .init = fake_digital_driver_init,
    .set_state = fake_digital_driver_set_state,
    .get_state = fake_digital_driver_get_state};
