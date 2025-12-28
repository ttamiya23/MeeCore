#include "unity.h"
#include "mc/system/core.h"
#include "system/fake_system.h"
#include "utils.h" // Needed for assert_helper.h
#include "assert_helper.h"

// Globals
fake_sys_ctx_t ctx;
MC_DEFINE_SYSTEM(sys, fake_sys_driver, ctx);

void setUp()
{
    mc_sys_init(&sys);
}

void test_init_resets_context()
{
    // Artificially set data in x
    ctx.x[0] = 5;
    mc_sys_init(&sys);

    // Should reset to 0
    TEST_ASSERT_EQUAL_INT32(0, ctx.x[0]);
}

void test_write_input_succeeds()
{
    mc_status_t ret = mc_sys_write_input(&sys, 0, 3);
    TEST_ASSERT_EQUAL_INT32(MC_OK, ret);
    TEST_ASSERT_EQUAL_INT32(3, ctx.x[0]);

    ret = mc_sys_write_input(&sys, 1, 8);
    TEST_ASSERT_EQUAL_INT32(MC_OK, ret);
    TEST_ASSERT_EQUAL_INT32(3, ctx.x[0]);
    TEST_ASSERT_EQUAL_INT32(8, ctx.x[1]);
}

void test_read_input_succeeds()
{
    ctx.x[0] = 4;
    ctx.x[1] = 7;
    mc_result_t res = mc_sys_read_input(&sys, 0);
    TEST_ASSERT_TRUE(res.ok);
    TEST_ASSERT_EQUAL_INT32(4, res.value);

    res = mc_sys_read_input(&sys, 1);
    TEST_ASSERT_TRUE(res.ok);
    TEST_ASSERT_EQUAL_INT32(7, res.value);
}

void test_read_output_succeeds()
{
    ctx.y[0] = 5;
    mc_result_t res = mc_sys_read_output(&sys, 0);
    TEST_ASSERT_TRUE(res.ok);
    TEST_ASSERT_EQUAL_INT32(5, res.value);
}

void test_invoke_succeeds()
{
    // Assert that y0 is 0
    TEST_ASSERT_EQUAL_INT32(0, ctx.y[0]);

    mc_status_t ret = mc_sys_invoke(&sys, 0, NULL, 0);
    TEST_ASSERT_EQUAL_INT32(MC_OK, ret);

    // New value should be 1
    TEST_ASSERT_EQUAL_INT32(1, ctx.y[0]);
}

void test_error_gets_propogated()
{
    // Invoking f1 should trigger errors
    int32_t args[1];
    args[0] = MC_ERROR_BUSY;
    mc_status_t ret = mc_sys_invoke(&sys, 1, args, 1);
    TEST_ASSERT_EQUAL_INT32(MC_ERROR_BUSY, ret);
}

void test_invalid_id_returns_error()
{
    mc_status_t ret = mc_sys_write_input(&sys, 3, 3);
    TEST_ASSERT_EQUAL_INT32(MC_ERROR_INVALID_ARGS, ret);

    mc_result_t res = mc_sys_read_input(&sys, 7);
    TEST_ASSERT_FALSE(res.ok);
    TEST_ASSERT_EQUAL_INT32(MC_ERROR_INVALID_ARGS, res.error);

    res = mc_sys_read_output(&sys, 2);
    TEST_ASSERT_FALSE(res.ok);
    TEST_ASSERT_EQUAL_INT32(MC_ERROR_INVALID_ARGS, res.error);

    ret = mc_sys_invoke(&sys, 7, NULL, 0);
    TEST_ASSERT_EQUAL_INT32(MC_ERROR_INVALID_ARGS, ret);
}

void test_unimplemented_methods_return_not_supported()
{
    mc_system_driver_t null_sys_driver = {
        .init = NULL,
        .invoke = NULL,
        .write_input = NULL,
        .read_input = NULL,
        .read_output = NULL,
        .get_function_count = NULL,
        .get_input_count = NULL,
        .get_output_count = NULL};
    mc_system_state_t state;
    mc_system_t null_sys = {
        .driver = &null_sys_driver,
        .ctx = NULL,
        .state = &state};

    mc_sys_init(&null_sys);

    mc_status_t ret = mc_sys_invoke(&null_sys, 0, NULL, 0);
    TEST_ASSERT_EQUAL_INT32(MC_ERROR_NOT_SUPPORTED, ret);
    ret = mc_sys_write_input(&null_sys, 0, 0);
    TEST_ASSERT_EQUAL_INT32(MC_ERROR_NOT_SUPPORTED, ret);
    mc_result_t res = mc_sys_read_input(&null_sys, 0);
    TEST_ASSERT_FALSE(res.ok);
    TEST_ASSERT_EQUAL_INT32(MC_ERROR_NOT_SUPPORTED, res.error);
    res = mc_sys_read_output(&null_sys, 0);
    TEST_ASSERT_FALSE(res.ok);
    TEST_ASSERT_EQUAL_INT32(MC_ERROR_NOT_SUPPORTED, res.error);

    uint8_t val = mc_sys_get_function_count(&null_sys);
    TEST_ASSERT_EQUAL_INT32(0, val);
    val = mc_sys_get_input_count(&null_sys);
    TEST_ASSERT_EQUAL_INT32(0, val);
    val = mc_sys_get_output_count(&null_sys);
    TEST_ASSERT_EQUAL_INT32(0, val);
}

void test_assert_death_if_null_pointer(void)
{
    int32_t val;
    int32_t args[1];
    TEST_ASSERT_DEATH(mc_sys_invoke(NULL, 0, args, 1));
    TEST_ASSERT_DEATH(mc_sys_write_input(NULL, 0, 1));
    TEST_ASSERT_DEATH(mc_sys_read_input(NULL, 0));
    TEST_ASSERT_DEATH(mc_sys_read_output(NULL, 0));
    TEST_ASSERT_DEATH(mc_sys_get_function_count(NULL));
    TEST_ASSERT_DEATH(mc_sys_get_input_count(NULL));
    TEST_ASSERT_DEATH(mc_sys_get_output_count(NULL));
}

void test_assert_death_if_uninitialized(void)
{
    // Force uninitialized state
    sys.state->is_initialized = 0;
    int32_t val;
    int32_t args[1];
    TEST_ASSERT_DEATH(mc_sys_invoke(&sys, 0, args, 1));
    TEST_ASSERT_DEATH(mc_sys_write_input(&sys, 0, 1));
    TEST_ASSERT_DEATH(mc_sys_read_input(&sys, 0));
    TEST_ASSERT_DEATH(mc_sys_read_output(&sys, 0));
    TEST_ASSERT_DEATH(mc_sys_get_function_count(&sys));
    TEST_ASSERT_DEATH(mc_sys_get_input_count(&sys));
    TEST_ASSERT_DEATH(mc_sys_get_output_count(&sys));
}

void test_get_counts_succeeds()
{
    TEST_ASSERT_EQUAL_INT32(2, mc_sys_get_function_count(&sys));
    TEST_ASSERT_EQUAL_INT32(2, mc_sys_get_input_count(&sys));
    TEST_ASSERT_EQUAL_INT32(1, mc_sys_get_output_count(&sys));
}