#include "unity.h"
#include "mc/device/analog_vector3.h"
#include "mc/device/analog.h"
#include "device/fake_analog_vector3.h"
#include "device/fake_analog.h"
#include "utils.h" // Needed for assert_helper.h
#include "assert_helper.h"

// Globals
fake_analog_vector3_ctx_t ctx;
fake_analog_ctx_t *ctxs[] = {&ctx.x, &ctx.y, &ctx.z};
MC_DEFINE_ANALOG_VECTOR3(dev, fake_analog_vector3_driver, ctx);
mc_analog_t *devs[] = {&dev.x, &dev.y, &dev.z};

void setUp()
{
    mc_analog_vector3_init(&dev, /*is_read_only=*/false);
}

void test_init_resets_context()
{
    for (int i = 0; i < 3; i++)
    {
        // Artificially set context
        ctxs[i]->is_initialized = 0;
        ctxs[i]->value = 194;
        mc_analog_vector3_init(&dev, /*is_read_only=*/false);

        // Should reset to defaults
        TEST_ASSERT_EQUAL_INT32(MC_INITIALIZED, devs[i]->config->is_initialized);
        TEST_ASSERT_FALSE(devs[i]->config->is_read_only);
        TEST_ASSERT_EQUAL_INT32(MC_INITIALIZED, ctxs[i]->is_initialized);
        TEST_ASSERT_EQUAL_INT32(0, ctxs[i]->value);
    }
}

void test_set_value_succeeds()
{
    mc_vector3_t value = {
        .x = 3,
        .y = 4,
        .z = 5};
    mc_status_t ret = mc_analog_vector3_set_value(&dev, value);
    TEST_ASSERT_EQUAL_INT32(MC_OK, ret);
    TEST_ASSERT_EQUAL_INT32(3, ctx.x.value);
    TEST_ASSERT_EQUAL_INT32(4, ctx.y.value);
    TEST_ASSERT_EQUAL_INT32(5, ctx.z.value);
}

void test_set_value_error_gets_propagated()
{
    mc_vector3_t value = {
        .x = 3,
        .y = 4,
        .z = 5};
    ctx.z.error = MC_ERROR_NO_RESPONSE;
    mc_status_t ret = mc_analog_vector3_set_value(&dev, value);
    TEST_ASSERT_EQUAL_INT32(MC_ERROR_NO_RESPONSE, ret);
}

void test_get_value_succeeds()
{
    ctx.x.value = 9;
    ctx.y.value = 8;
    ctx.z.value = 7;
    mc_vector3_t value;
    mc_status_t ret = mc_analog_vector3_get_value(&dev, &value);
    TEST_ASSERT_EQUAL_INT32(MC_OK, ret);
    TEST_ASSERT_EQUAL_INT32(9, value.x);
    TEST_ASSERT_EQUAL_INT32(8, value.y);
    TEST_ASSERT_EQUAL_INT32(7, value.z);
}

void test_get_value_error_gets_propagated()
{
    ctx.x.value = 9;
    ctx.y.value = 8;
    ctx.z.value = 7;
    mc_vector3_t value;
    ctx.y.error = MC_ERROR_BUSY;
    mc_status_t ret = mc_analog_vector3_get_value(&dev, &value);
    TEST_ASSERT_EQUAL_INT32(MC_ERROR_BUSY, ret);
}

void test_get_value_assert_death_if_null_pointer()
{
    TEST_ASSERT_DEATH(mc_analog_vector3_get_value(&dev, NULL));
}

void test_set_read_only_succeeds()
{
    mc_analog_vector3_set_read_only(&dev, true);
    for (int i = 0; i < 3; i++)
    {
        TEST_ASSERT_TRUE(devs[i]->config->is_read_only);
    }
}