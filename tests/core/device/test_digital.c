#include "unity.h"
#include "mc/device/digital.h"
#include "device/fake_digital.h"
#include "utils.h" // Needed for assert_helper.h
#include "assert_helper.h"

// Globals
fake_digital_ctx_t ctx;
MC_DEFINE_DIGITAL(dev, fake_digital_driver, ctx);

void setUp()
{
    test_assert_init();
    mc_digital_init(&dev, /*is_read_only=*/false);
}

void test_init_resets_context()
{
    // Artificially set context
    ctx.is_initialized = 0;
    ctx.state = true;
    mc_digital_init(&dev, /*is_read_only=*/false);

    // Should reset to defaults
    TEST_ASSERT_EQUAL_INT32(MC_INITIALIZED, dev.config->is_initialized);
    TEST_ASSERT_FALSE(dev.config->is_read_only);
    TEST_ASSERT_EQUAL_INT32(MC_INITIALIZED, ctx.is_initialized);
    TEST_ASSERT_FALSE(ctx.state);
}

void test_set_state_succeeds()
{
    mc_status_t ret = mc_digital_set_state(&dev, true);
    TEST_ASSERT_EQUAL(MC_OK, ret);
    TEST_ASSERT_TRUE(ctx.state);

    ret = mc_digital_set_state(&dev, false);
    TEST_ASSERT_EQUAL(MC_OK, ret);
    TEST_ASSERT_FALSE(ctx.state);
}

void test_get_state_succeeds()
{
    ctx.state = true;
    mc_result_t res = mc_digital_get_state(&dev);
    TEST_ASSERT_TRUE(res.ok);
    TEST_ASSERT_TRUE(res.value);

    ctx.state = false;
    res = mc_digital_get_state(&dev);
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

void test_read_only_get_state_succeeds()
{
    mc_digital_set_read_only(&dev, true);
    ctx.state = true;
    mc_result_t res = mc_digital_get_state(&dev);
    TEST_ASSERT_TRUE(res.ok);
    TEST_ASSERT_TRUE(res.value);

    ctx.state = false;
    res = mc_digital_get_state(&dev);
    TEST_ASSERT_TRUE(res.ok);
    TEST_ASSERT_FALSE(res.value);
}

void test_read_only_set_state_fails()
{
    mc_digital_set_read_only(&dev, true);
    mc_status_t ret = mc_digital_set_state(&dev, true);
    TEST_ASSERT_EQUAL(MC_ERROR_WRITE_PROTECTED, ret);
}

void test_read_only_toggle_fails()
{
    mc_digital_set_read_only(&dev, true);
    mc_status_t ret = mc_digital_toggle(&dev);
    TEST_ASSERT_EQUAL(MC_ERROR_WRITE_PROTECTED, ret);
}

void test_error_status_gets_propagated()
{
    mc_status_t error = MC_ERROR_BUSY;
    ctx.error = error;

    mc_status_t ret = mc_digital_set_state(&dev, true);
    TEST_ASSERT_EQUAL(error, ret);

    mc_result_t res;
    res = mc_digital_get_state(&dev);
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
    mc_digital_config_t config;
    mc_digital_t null_dev = {
        .driver = &null_digital_driver,
        .config = &config,
        .ctx = NULL};

    mc_digital_init(&null_dev, /*is_read_only=*/false);

    mc_status_t ret = mc_digital_set_state(&null_dev, true);
    TEST_ASSERT_EQUAL(MC_ERROR_NOT_SUPPORTED, ret);

    mc_result_t res = mc_digital_get_state(&null_dev);
    TEST_ASSERT_FALSE(res.ok);
    TEST_ASSERT_EQUAL(MC_ERROR_NOT_SUPPORTED, res.error);

    ret = mc_digital_toggle(&null_dev);
    TEST_ASSERT_EQUAL(MC_ERROR_NOT_SUPPORTED, ret);
}

void test_assert_death_if_null_pointer(void)
{
    mc_digital_t null_dev = {
        .driver = NULL,
        .config = NULL,
        .ctx = NULL};

    TEST_ASSERT_DEATH(mc_digital_set_state(NULL, true));
    TEST_ASSERT_DEATH(mc_digital_set_state(&null_dev, true));

    TEST_ASSERT_DEATH(mc_digital_get_state(NULL));
    TEST_ASSERT_DEATH(mc_digital_get_state(&null_dev));

    TEST_ASSERT_DEATH(mc_digital_toggle(NULL));
    TEST_ASSERT_DEATH(mc_digital_toggle(&null_dev));

    TEST_ASSERT_DEATH(mc_digital_set_read_only(NULL, true));
    TEST_ASSERT_DEATH(mc_digital_set_read_only(&null_dev, true));
}

void test_assert_death_if_uninitialized(void)
{
    mc_digital_config_t config;
    mc_digital_t uninitialized_dev = {
        .driver = &fake_digital_driver,
        .config = &config,
        .ctx = &ctx};

    TEST_ASSERT_DEATH(mc_digital_set_state(&uninitialized_dev, true));
    TEST_ASSERT_DEATH(mc_digital_get_state(&uninitialized_dev));
    TEST_ASSERT_DEATH(mc_digital_toggle(&uninitialized_dev));
    TEST_ASSERT_DEATH(mc_digital_set_read_only(&uninitialized_dev, true));
}