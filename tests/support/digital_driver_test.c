#include "digital_driver_test.h"

void digital_driver_test_init(void *ctx)
{
    test_digital_ctx_t *digital_ctx = (test_digital_ctx_t *)ctx;
    digital_ctx->is_initialized = MC_INITIALIZED;
    digital_ctx->state = false;
    digital_ctx->error = MC_OK;
}

mc_status_t digital_driver_test_set_state(void *ctx, bool state)
{
    test_digital_ctx_t *digital_ctx = (test_digital_ctx_t *)ctx;
    digital_ctx->state = state;
    return digital_ctx->error;
}

mc_result_t digital_driver_test_get_state(void *ctx)
{
    test_digital_ctx_t *digital_ctx = (test_digital_ctx_t *)ctx;
    if (digital_ctx->error != MC_OK)
    {
        return MC_ERR_VAL(digital_ctx->error);
    }
    return MC_OK_VAL(digital_ctx->state);
}

const mc_digital_driver_t test_digital_driver = {
    .init = digital_driver_test_init,
    .set_state = digital_driver_test_set_state,
    .get_state = digital_driver_test_get_state};
