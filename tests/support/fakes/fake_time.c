#include "fake_time.h"
#include "mc/utils.h"

static void fake_time_init(void *ctx)
{
    fake_time_ctx_t *t = (fake_time_ctx_t *)ctx;
    t->current_time_ms = 0;
    t->is_initialized = MC_INITIALIZED;
}

static uint32_t fake_time_get_ms(void *ctx)
{
    fake_time_ctx_t *t = (fake_time_ctx_t *)ctx;
    return t->current_time_ms;
}

static void fake_time_delay(void *ctx, uint32_t ms)
{
    fake_time_ctx_t *t = (fake_time_ctx_t *)ctx;
    t->current_time_ms += ms;
}

const mc_time_driver_t fake_time_driver = {
    .init = fake_time_init,
    .get_ms = fake_time_get_ms,
    .delay = fake_time_delay};

void fake_time_set_ms(fake_time_ctx_t *ctx, uint32_t ms)
{
    ctx->current_time_ms = ms;
}