#include "unity.h"
#include "mc/device/analog_vector.h"
#include "mc/device/analog.h"
#include "device/fake_analog_vector.h"
#include "device/fake_analog.h"
#include "utils.h" // Needed for assert_helper.h
#include "assert_helper.h"

// Globals
fake_analog_vector_ctx_t ctx;
fake_analog_ctx_t *ctxs[] = {&ctx.x, &ctx.y, &ctx.z};
MC_DEFINE_ANALOG_VECTOR(dev, fake_analog_vector_driver, ctx);
mc_analog_t *devs[] = {&dev.x, &dev.y, &dev.z};

void setUp()
{
    mc_analog_vector_init(&dev, /*is_read_only=*/false);
}

void test_init_resets_context()
{
    for (int i = 0; i < 3; i++)
    {
        // Artificially set context
        ctxs[i]->is_initialized = 0;
        ctxs[i]->value = 194;
        mc_analog_vector_init(&dev, /*is_read_only=*/false);

        // Should reset to defaults
        TEST_ASSERT_EQUAL_INT32(MC_INITIALIZED, devs[i]->config->is_initialized);
        TEST_ASSERT_FALSE(devs[i]->config->is_read_only);
        TEST_ASSERT_EQUAL_INT32(MC_INITIALIZED, ctxs[i]->is_initialized);
        TEST_ASSERT_EQUAL_INT32(0, ctxs[i]->value);
    }
}

void test_set_read_only_succeeds()
{
    mc_analog_vector_set_read_only(&dev, true);
    for (int i = 0; i < 3; i++)
    {
        TEST_ASSERT_TRUE(devs[i]->config->is_read_only);
    }
}