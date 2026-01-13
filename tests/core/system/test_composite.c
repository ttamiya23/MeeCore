#include "unity.h"
#include "mc/system/core.h"
#include "mc/system/composite.h"
#include "fakes/system/fake_system.h"
#include "fakes/system/fake_composite.h"
#include "utils.h" // Needed for assert_helper.h
#include "assert_helper.h"

// Globals
fake_composite_ctx_t ctx;
MC_DEFINE_SYSTEM(sys, fake_composite_sys_driver, ctx);

void setUp()
{
    test_assert_init();
    mc_sys_init(&sys);
}

void test_init_resets_all_contexts()
{
    // Artificially set data in x
    ctx.sys1.x[0] = 5;
    ctx.sys2.x[0] = 5;
    mc_sys_init(&sys);

    // Should reset to 0
    TEST_ASSERT_EQUAL_INT32(0, ctx.sys1.x[0]);
    TEST_ASSERT_EQUAL_INT32(0, ctx.sys2.x[0]);
}

void test_write_input_succeeds()
{
    mc_status_t ret = mc_sys_write_input(&sys, 0, 3);
    TEST_ASSERT_EQUAL_INT32(MC_OK, ret);
    TEST_ASSERT_EQUAL_INT32(3, ctx.sys1.x[0]);

    ret = mc_sys_write_input(&sys, 3, 8);
    TEST_ASSERT_EQUAL_INT32(MC_OK, ret);
    TEST_ASSERT_EQUAL_INT32(3, ctx.sys1.x[0]);
    TEST_ASSERT_EQUAL_INT32(8, ctx.sys2.x[1]);
}

void test_read_input_succeeds()
{
    ctx.sys1.x[0] = 4;
    ctx.sys2.x[0] = 7;
    mc_result_t res = mc_sys_read_input(&sys, 0);
    TEST_ASSERT_TRUE(res.ok);
    TEST_ASSERT_EQUAL_INT32(4, res.value);

    res = mc_sys_read_input(&sys, 2);
    TEST_ASSERT_TRUE(res.ok);
    TEST_ASSERT_EQUAL_INT32(7, res.value);
}

void test_read_output_succeeds()
{
    ctx.sys1.y[0] = 5;
    ctx.sys2.y[0] = 2;
    mc_result_t res = mc_sys_read_output(&sys, 0);
    TEST_ASSERT_TRUE(res.ok);
    TEST_ASSERT_EQUAL_INT32(5, res.value);

    res = mc_sys_read_output(&sys, 1);
    TEST_ASSERT_TRUE(res.ok);
    TEST_ASSERT_EQUAL_INT32(2, res.value);
}

void test_invoke_succeeds()
{
    // Assert that y0 is 0
    TEST_ASSERT_EQUAL_INT32(0, ctx.sys1.y[0]);
    TEST_ASSERT_EQUAL_INT32(0, ctx.sys2.y[0]);

    mc_status_t ret = mc_sys_invoke(&sys, 0, NULL, 0);
    TEST_ASSERT_EQUAL_INT32(MC_OK, ret);
    TEST_ASSERT_EQUAL_INT32(1, ctx.sys1.y[0]);

    ret = mc_sys_invoke(&sys, 2, NULL, 0);
    TEST_ASSERT_EQUAL_INT32(MC_OK, ret);
    TEST_ASSERT_EQUAL_INT32(1, ctx.sys2.y[0]);
}

void test_get_counts_succeeds()
{
    TEST_ASSERT_EQUAL_INT32(4, mc_sys_get_function_count(&sys));
    TEST_ASSERT_EQUAL_INT32(4, mc_sys_get_input_count(&sys));
    TEST_ASSERT_EQUAL_INT32(2, mc_sys_get_output_count(&sys));
    TEST_ASSERT_EQUAL_INT32(8, mc_sys_get_alias_count(&sys));
}

void test_get_alias_succeeds()
{
    mc_sys_cmd_info_t cmd;
    ctx.sys1.increment_y_name = "incrementY0";
    ctx.sys1.x0_name = "input0";
    ctx.sys1.y0_name = "output0";
    ctx.sys1.reset_name = "reset0";
    ctx.sys2.increment_y_name = "incrementY1";
    ctx.sys2.x0_name = "input1";
    ctx.sys2.y0_name = "output1";
    ctx.sys2.reset_name = "reset1";

    mc_status_t ret;
    ret = mc_sys_get_alias(&sys, 0, &cmd);
    TEST_ASSERT_EQUAL_INT32(MC_OK, ret);
    TEST_ASSERT_EQUAL_STRING("incrementY0", cmd.alias);
    TEST_ASSERT_EQUAL_INT32(MC_CMD_TYPE_FUNC, cmd.type);
    TEST_ASSERT_EQUAL_INT32(0, cmd.id); // Should be f0
    TEST_ASSERT_FALSE(cmd.has_preset);

    ret = mc_sys_get_alias(&sys, 1, &cmd);
    TEST_ASSERT_EQUAL_INT32(MC_OK, ret);
    TEST_ASSERT_EQUAL_STRING("input0", cmd.alias);
    TEST_ASSERT_EQUAL_INT32(MC_CMD_TYPE_INPUT, cmd.type);
    TEST_ASSERT_EQUAL_INT32(0, cmd.id); // Should be x0
    TEST_ASSERT_FALSE(cmd.has_preset);

    ret = mc_sys_get_alias(&sys, 2, &cmd);
    TEST_ASSERT_EQUAL_INT32(MC_OK, ret);
    TEST_ASSERT_EQUAL_STRING("output0", cmd.alias);
    TEST_ASSERT_EQUAL_INT32(MC_CMD_TYPE_OUTPUT, cmd.type);
    TEST_ASSERT_EQUAL_INT32(0, cmd.id); // Should be y0
    TEST_ASSERT_FALSE(cmd.has_preset);

    ret = mc_sys_get_alias(&sys, 3, &cmd);
    TEST_ASSERT_EQUAL_INT32(MC_OK, ret);
    TEST_ASSERT_EQUAL_STRING("reset0", cmd.alias);
    TEST_ASSERT_EQUAL_INT32(MC_CMD_TYPE_INPUT, cmd.type);
    TEST_ASSERT_EQUAL_INT32(0, cmd.id); // Should be x0
    TEST_ASSERT_TRUE(cmd.has_preset);
    TEST_ASSERT_EQUAL_INT32(0, cmd.preset_val);

    ret = mc_sys_get_alias(&sys, 4, &cmd);
    TEST_ASSERT_EQUAL_INT32(MC_OK, ret);
    TEST_ASSERT_EQUAL_STRING("incrementY1", cmd.alias);
    TEST_ASSERT_EQUAL_INT32(MC_CMD_TYPE_FUNC, cmd.type);
    TEST_ASSERT_EQUAL_INT32(2, cmd.id); // Should be f2
    TEST_ASSERT_FALSE(cmd.has_preset);

    ret = mc_sys_get_alias(&sys, 5, &cmd);
    TEST_ASSERT_EQUAL_INT32(MC_OK, ret);
    TEST_ASSERT_EQUAL_STRING("input1", cmd.alias);
    TEST_ASSERT_EQUAL_INT32(MC_CMD_TYPE_INPUT, cmd.type);
    TEST_ASSERT_EQUAL_INT32(2, cmd.id); // Should be x2
    TEST_ASSERT_FALSE(cmd.has_preset);

    ret = mc_sys_get_alias(&sys, 6, &cmd);
    TEST_ASSERT_EQUAL_INT32(MC_OK, ret);
    TEST_ASSERT_EQUAL_STRING("output1", cmd.alias);
    TEST_ASSERT_EQUAL_INT32(MC_CMD_TYPE_OUTPUT, cmd.type);
    TEST_ASSERT_EQUAL_INT32(1, cmd.id); // Should be y1
    TEST_ASSERT_FALSE(cmd.has_preset);

    ret = mc_sys_get_alias(&sys, 7, &cmd);
    TEST_ASSERT_EQUAL_INT32(MC_OK, ret);
    TEST_ASSERT_EQUAL_STRING("reset1", cmd.alias);
    TEST_ASSERT_EQUAL_INT32(MC_CMD_TYPE_INPUT, cmd.type);
    TEST_ASSERT_EQUAL_INT32(2, cmd.id); // Should be x2
    TEST_ASSERT_TRUE(cmd.has_preset);
    TEST_ASSERT_EQUAL_INT32(0, cmd.preset_val);

    // Invalid ID should fail
    ret = mc_sys_get_alias(&sys, 8, &cmd);
    TEST_ASSERT_EQUAL_INT32(MC_ERROR_INVALID_ARGS, ret);
}