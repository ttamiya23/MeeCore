#include "unity.h"
#include "mc/device/digital.h"
#include "device/fake_digital.h"
#include "utils.h" // Needed for assert_helper.h
#include "assert_helper.h"

// Globals
fake_digital_ctx_t ctx;

void setUp()
{
    fake_digital_driver.init(&ctx);
}

void test_init_resets_context()
{
    // Artificially set context
    ctx.is_initialized = 0;
    ctx.state = true;
    fake_digital_driver.init(&ctx);

    // Should reset to defaults
    TEST_ASSERT_EQUAL_INT32(MC_INITIALIZED, ctx.is_initialized);
    TEST_ASSERT_FALSE(ctx.state);
}

void test_set_state_succeeds()
{
    mc_status_t ret = fake_digital_driver.set_state(&ctx, true);
    TEST_ASSERT_EQUAL(MC_OK, ret);
    TEST_ASSERT_TRUE(ctx.state);

    ret = fake_digital_driver.set_state(&ctx, false);
    TEST_ASSERT_EQUAL(MC_OK, ret);
    TEST_ASSERT_FALSE(ctx.state);
}

void test_get_state_succeeds()
{
    ctx.state = true;
    mc_result_t res = fake_digital_driver.get_state(&ctx);
    TEST_ASSERT_TRUE(res.ok);
    TEST_ASSERT_TRUE(res.value);

    ctx.state = false;
    res = fake_digital_driver.get_state(&ctx);
    TEST_ASSERT_TRUE(res.ok);
    TEST_ASSERT_FALSE(res.value);
}

void test_error_status_gets_propagated()
{
    mc_status_t error = MC_ERROR_BUSY;
    ctx.error = error;

    mc_status_t ret = fake_digital_driver.set_state(&ctx, true);
    TEST_ASSERT_EQUAL(error, ret);

    mc_result_t res;
    res = fake_digital_driver.get_state(&ctx);
    TEST_ASSERT_FALSE(res.ok);
    TEST_ASSERT_EQUAL(error, res.error);
}
