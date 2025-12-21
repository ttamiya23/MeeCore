#include "unity.h"
#include "mc/system/core.h"
#include "mc/system/composite.h"
#include "system_test.h"
#include "system_composite.h"
#include "mock_utils.h" // Needed for assert_helper.h
#include "assert_helper.h"

// Globals
composite_sys_ctx_t ctx;
MC_DEFINE_SYSTEM(sys, composite_sys_driver, ctx);

void setUp()
{
    mc_sys_init(&sys);
}

void test_init_resets_all_contexts()
{
    // Artificially set data in x
    ctx.test_1.x[0] = 5;
    ctx.test_2.x[0] = 5;
    mc_sys_init(&sys);

    // Should reset to 0
    TEST_ASSERT_EQUAL_INT32(0, ctx.test_1.x[0]);
    TEST_ASSERT_EQUAL_INT32(0, ctx.test_2.x[0]);
}

void test_write_input_succeeds()
{
    mc_status_t ret = mc_sys_write_input(&sys, 0, 3);
    TEST_ASSERT_EQUAL_INT32(MC_OK, ret);
    TEST_ASSERT_EQUAL_INT32(3, ctx.test_1.x[0]);

    ret = mc_sys_write_input(&sys, 3, 8);
    TEST_ASSERT_EQUAL_INT32(MC_OK, ret);
    TEST_ASSERT_EQUAL_INT32(3, ctx.test_1.x[0]);
    TEST_ASSERT_EQUAL_INT32(8, ctx.test_2.x[1]);
}

void test_read_input_succeeds()
{
    int32_t val;
    ctx.test_1.x[0] = 4;
    ctx.test_2.x[0] = 7;
    mc_status_t ret = mc_sys_read_input(&sys, 0, &val);
    TEST_ASSERT_EQUAL_INT32(MC_OK, ret);
    TEST_ASSERT_EQUAL_INT32(4, val);

    ret = mc_sys_read_input(&sys, 2, &val);
    TEST_ASSERT_EQUAL_INT32(MC_OK, ret);
    TEST_ASSERT_EQUAL_INT32(7, val);
}

void test_read_output_succeeds()
{
    int32_t val;
    ctx.test_1.y[0] = 5;
    ctx.test_2.y[0] = 2;
    mc_status_t ret = mc_sys_read_output(&sys, 0, &val);
    TEST_ASSERT_EQUAL_INT32(MC_OK, ret);
    TEST_ASSERT_EQUAL_INT32(5, val);

    ret = mc_sys_read_output(&sys, 1, &val);
    TEST_ASSERT_EQUAL_INT32(MC_OK, ret);
    TEST_ASSERT_EQUAL_INT32(2, val);
}

void test_invoke_succeeds()
{
    // Assert that y0 is 0
    TEST_ASSERT_EQUAL_INT32(0, ctx.test_1.y[0]);
    TEST_ASSERT_EQUAL_INT32(0, ctx.test_2.y[0]);

    mc_status_t ret = mc_sys_invoke(&sys, 0, NULL, 0);
    TEST_ASSERT_EQUAL_INT32(MC_OK, ret);
    TEST_ASSERT_EQUAL_INT32(1, ctx.test_1.y[0]);

    ret = mc_sys_invoke(&sys, 2, NULL, 0);
    TEST_ASSERT_EQUAL_INT32(MC_OK, ret);
    TEST_ASSERT_EQUAL_INT32(1, ctx.test_2.y[0]);
}
