#include "unity.h"
#include "fakes/fake_time.h"
#include "mc/utils.h"

fake_time_ctx_t ctx;

void setUp()
{
    // Reset manually or via driver
    fake_time_driver.init(&ctx);
}

void test_init_resets_time()
{
    ctx.current_time_ms = 9999;
    fake_time_driver.init(&ctx);

    TEST_ASSERT_EQUAL_UINT32(0, ctx.current_time_ms);
    TEST_ASSERT_EQUAL_HEX8(MC_INITIALIZED, ctx.is_initialized);
}

void test_get_ms_returns_current_time()
{
    ctx.current_time_ms = 1234;
    uint32_t val = fake_time_driver.get_ms(&ctx);
    TEST_ASSERT_EQUAL_UINT32(1234, val);
}

void test_delay_advances_time()
{
    ctx.current_time_ms = 100;
    fake_time_driver.delay(&ctx, 50);

    TEST_ASSERT_EQUAL_UINT32(150, ctx.current_time_ms);
}

void test_helper_set_ms_succeeds()
{
    fake_time_set_ms(&ctx, 5000);
    TEST_ASSERT_EQUAL_UINT32(5000, ctx.current_time_ms);
}