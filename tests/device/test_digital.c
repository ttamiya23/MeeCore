#include "unity.h"
#include "mc/common.h"
#include "mc/device/digital.h"
#include "digital_driver_test.h"
#include "utils.h" // Needed for assert_helper.h
#include "assert_helper.h"

// Globals
test_digital_ctx_t ctx;
MC_DEFINE_DIGITAL(dev, test_digital_driver, ctx);

void setUp()
{
    mc_digital_init(&dev);
}

void test_init_resets_context()
{
    // Artificially set context
    ctx.is_initialized = 0;
    ctx.state = true;
    mc_digital_init(&dev);

    // Should reset to defaults
    TEST_ASSERT_EQUAL_INT32(MC_INITIALIZED, ctx.is_initialized);
    TEST_ASSERT_FALSE(ctx.state);
}

void test_set_state_succeeds()
{
    mc_status_t ret = mc_digital_set(&dev, true);
    TEST_ASSERT_EQUAL(MC_OK, ret);
    TEST_ASSERT_TRUE(ctx.state);

    ret = mc_digital_set(&dev, false);
    TEST_ASSERT_EQUAL(MC_OK, ret);
    TEST_ASSERT_FALSE(ctx.state);
}

void test_get_state_succeeds()
{
    bool state;
    ctx.state = true;
    mc_result_t res = mc_digital_get(&dev);
    TEST_ASSERT_TRUE(res.ok);
    TEST_ASSERT_TRUE(res.value);

    ctx.state = false;
    res = mc_digital_get(&dev);
    TEST_ASSERT_TRUE(res.ok);
    TEST_ASSERT_FALSE(res.value);
}

void test_toggle_succeeds()
{
    mc_status_t ret = mc_digital_toggle(&dev);
    TEST_ASSERT_EQUAL(MC_OK, ret);
    TEST_ASSERT_TRUE(ctx.state);

    ret = mc_digital_toggle(&dev);
    TEST_ASSERT_EQUAL(MC_OK, ret);
    TEST_ASSERT_FALSE(ctx.state);
}

void test_error_status_gets_propagated()
{
    mc_status_t error = MC_ERROR_BUSY;
    ctx.error = error;

    mc_status_t ret = mc_digital_set(&dev, true);
    TEST_ASSERT_EQUAL(error, ret);

    mc_result_t res;
    res = mc_digital_get(&dev);
    TEST_ASSERT_FALSE(res.ok);
    TEST_ASSERT_EQUAL(error, res.error);

    ret = mc_digital_toggle(&dev);
    TEST_ASSERT_EQUAL(error, ret);
}

void test_unimplemented_methods_return_not_supported()
{
    mc_digital_driver_t null_digital_driver = {
        .init = NULL,
        .set_state = NULL,
        .get_state = NULL};
    mc_digital_t null_dev = {
        .driver = &null_digital_driver,
        .ctx = NULL};

    mc_digital_init(&null_dev);

    mc_status_t ret = mc_digital_set(&null_dev, true);
    TEST_ASSERT_EQUAL(MC_ERROR_NOT_SUPPORTED, ret);

    mc_result_t res = mc_digital_get(&null_dev);
    TEST_ASSERT_FALSE(res.ok);
    TEST_ASSERT_EQUAL(MC_ERROR_NOT_SUPPORTED, res.error);

    ret = mc_digital_toggle(&null_dev);
    TEST_ASSERT_EQUAL(MC_ERROR_NOT_SUPPORTED, ret);
}

void test_assert_death_if_null_pointer(void)
{
    mc_digital_t null_dev = {
        .driver = NULL,
        .ctx = NULL};

    TEST_ASSERT_DEATH(mc_digital_set(NULL, true));
    TEST_ASSERT_DEATH(mc_digital_set(&null_dev, true));

    TEST_ASSERT_DEATH(mc_digital_get(NULL));
    TEST_ASSERT_DEATH(mc_digital_get(&null_dev));

    TEST_ASSERT_DEATH(mc_digital_toggle(NULL));
    TEST_ASSERT_DEATH(mc_digital_toggle(&null_dev));
}