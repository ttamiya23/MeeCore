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
mc_analog_driver_t *drivers[] = {&fake_analog_vector3_driver.x,
                                 &fake_analog_vector3_driver.y,
                                 &fake_analog_vector3_driver.z};

void setUp()
{
    for (int i = 0; i < 3; i++)
    {
        drivers[i]->init(ctxs[i]);
    }
}

void test_init_resets_context()
{
    for (int i = 0; i < 3; i++)
    {
        // Artificially set context
        ctxs[i]->is_initialized = 0;
        ctxs[i]->value = -234232;
        drivers[i]->init(&ctx);

        // Should reset to defaults
        TEST_ASSERT_EQUAL_INT32(MC_INITIALIZED, ctxs[i]->is_initialized);
        TEST_ASSERT_EQUAL_INT32(0, ctxs[i]->value);
    }
}

void test_set_value_succeeds()
{
    for (int i = 0; i < 3; i++)
    {
        mc_status_t ret = drivers[i]->set_value(&ctx, 100 + i);
        TEST_ASSERT_EQUAL(MC_OK, ret);
    }
    TEST_ASSERT_EQUAL_INT32(100, ctx.x.value);
    TEST_ASSERT_EQUAL_INT32(101, ctx.y.value);
    TEST_ASSERT_EQUAL_INT32(102, ctx.z.value);
}

void test_get_value_succeeds()
{
    for (int i = 0; i < 3; i++)
    {
        ctxs[i]->value = -199 - i;
        mc_result_t res = drivers[i]->get_value(&ctx);
        TEST_ASSERT_TRUE(res.ok);
        TEST_ASSERT_EQUAL_INT32(-199 - i, res.value);
    }
}

void test_error_status_gets_propagated()
{
    mc_status_t error = MC_ERROR_BUSY;
    for (int i = 0; i < 3; i++)
    {
        ctxs[i]->error = error;

        mc_status_t ret = drivers[i]->set_value(&ctx, 324 + i);
        TEST_ASSERT_EQUAL(error, ret);

        mc_result_t res;
        res = drivers[i]->get_value(&ctx);
        TEST_ASSERT_FALSE(res.ok);
        TEST_ASSERT_EQUAL(error, res.error);
    }
}
