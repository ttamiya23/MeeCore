#include "unity.h"
#include "mc/device/analog.h"
#include "device/fake_analog.h"
#include "utils.h" // Needed for assert_helper.h
#include "assert_helper.h"

// Globals
fake_analog_ctx_t ctx;

void setUp()
{
    fake_analog_driver.init(&ctx);
}

void test_init_resets_context()
{
    // Artificially set context
    ctx.is_initialized = 0;
    ctx.value = -234232;
    fake_analog_driver.init(&ctx);

    // Should reset to defaults
    TEST_ASSERT_EQUAL_INT32(MC_INITIALIZED, ctx.is_initialized);
    TEST_ASSERT_EQUAL_INT32(0, ctx.value);
}

void test_set_value_succeeds()
{
    mc_status_t ret = fake_analog_driver.set_value(&ctx, 100);
    TEST_ASSERT_EQUAL(MC_OK, ret);
    TEST_ASSERT_EQUAL_INT32(100, ctx.value);
}

void test_get_value_succeeds()
{
    ctx.value = -199;
    mc_result_t res = fake_analog_driver.get_value(&ctx);
    TEST_ASSERT_TRUE(res.ok);
    TEST_ASSERT_EQUAL_INT32(-199, res.value);
}

void test_error_status_gets_propagated()
{
    mc_status_t error = MC_ERROR_BUSY;
    ctx.error = error;

    mc_status_t ret = fake_analog_driver.set_value(&ctx, 324);
    TEST_ASSERT_EQUAL(error, ret);

    mc_result_t res;
    res = fake_analog_driver.get_value(&ctx);
    TEST_ASSERT_FALSE(res.ok);
    TEST_ASSERT_EQUAL(error, res.error);
}
