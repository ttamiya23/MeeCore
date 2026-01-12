#include "unity.h"
#include "mc/device/analog.h"
#include "device/fake_analog.h"
#include "utils.h" // Needed for assert_helper.h
#include "assert_helper.h"

// Globals
fake_analog_ctx_t ctx;
MC_DEFINE_ANALOG(dev, fake_analog_driver, ctx);

void setUp()
{
    mc_analog_init(&dev, /*is_read_only=*/false);
}

void test_init_resets_context()
{
    // Artificially set context
    ctx.is_initialized = 0;
    ctx.value = 194;
    mc_analog_init(&dev, /*is_read_only=*/false);

    // Should reset to defaults
    TEST_ASSERT_EQUAL_INT32(MC_INITIALIZED, dev.config->is_initialized);
    TEST_ASSERT_FALSE(dev.config->is_read_only);
    TEST_ASSERT_EQUAL_INT32(MC_INITIALIZED, ctx.is_initialized);
    TEST_ASSERT_EQUAL_INT32(0, ctx.value);
}

void test_set_value_succeeds()
{
    mc_status_t ret = mc_analog_set_value(&dev, 1553);
    TEST_ASSERT_EQUAL(MC_OK, ret);
    TEST_ASSERT_EQUAL_INT32(1553, ctx.value);
}

void test_get_value_succeeds()
{
    ctx.value = -24445;
    mc_result_t res = mc_analog_get_value(&dev);
    TEST_ASSERT_TRUE(res.ok);
    TEST_ASSERT_EQUAL_INT32(-24445, res.value);
}

void test_read_only_get_value_succeeds()
{
    mc_analog_set_read_only(&dev, true);
    ctx.value = 849922;
    mc_result_t res = mc_analog_get_value(&dev);
    TEST_ASSERT_TRUE(res.ok);
    TEST_ASSERT_EQUAL_INT32(849922, res.value);
}

void test_read_only_set_value_fails()
{
    mc_analog_set_read_only(&dev, true);
    mc_status_t ret = mc_analog_set_value(&dev, 322);
    TEST_ASSERT_EQUAL(MC_ERROR_WRITE_PROTECTED, ret);
}

void test_error_status_gets_propagated()
{
    mc_status_t error = MC_ERROR_BUSY;
    ctx.error = error;

    mc_status_t ret = mc_analog_set_value(&dev, 0);
    TEST_ASSERT_EQUAL(error, ret);

    mc_result_t res;
    res = mc_analog_get_value(&dev);
    TEST_ASSERT_FALSE(res.ok);
    TEST_ASSERT_EQUAL(error, res.error);
}

void test_unimplemented_methods_return_not_supported()
{
    mc_analog_driver_t null_analog_driver = {
        .init = NULL,
        .set_value = NULL,
        .get_value = NULL};
    mc_analog_config_t config;
    mc_analog_t null_dev = {
        .driver = &null_analog_driver,
        .config = &config,
        .ctx = NULL};

    mc_analog_init(&null_dev, /*is_read_only=*/false);

    mc_status_t ret = mc_analog_set_value(&null_dev, 22);
    TEST_ASSERT_EQUAL(MC_ERROR_NOT_SUPPORTED, ret);

    mc_result_t res = mc_analog_get_value(&null_dev);
    TEST_ASSERT_FALSE(res.ok);
    TEST_ASSERT_EQUAL(MC_ERROR_NOT_SUPPORTED, res.error);
}

void test_assert_death_if_null_pointer(void)
{
    mc_analog_t null_dev = {
        .driver = NULL,
        .config = NULL,
        .ctx = NULL};

    TEST_ASSERT_DEATH(mc_analog_set_value(NULL, 13242));
    TEST_ASSERT_DEATH(mc_analog_set_value(&null_dev, -42485));

    TEST_ASSERT_DEATH(mc_analog_get_value(NULL));
    TEST_ASSERT_DEATH(mc_analog_get_value(&null_dev));

    TEST_ASSERT_DEATH(mc_analog_set_read_only(NULL, true));
    TEST_ASSERT_DEATH(mc_analog_set_read_only(&null_dev, true));
}

void test_assert_death_if_uninitialized(void)
{
    mc_analog_config_t config;
    mc_analog_t uninitialized_dev = {
        .driver = &fake_analog_driver,
        .config = &config,
        .ctx = &ctx};

    TEST_ASSERT_DEATH(mc_analog_set_value(&uninitialized_dev, -2));
    TEST_ASSERT_DEATH(mc_analog_get_value(&uninitialized_dev));
    TEST_ASSERT_DEATH(mc_analog_set_read_only(&uninitialized_dev, true));
}