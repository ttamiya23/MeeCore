#include "unity.h"
#include "mc/system/core.h"
#include "mc/system/composite.h"
#include "system_test.h"
#include "system_composite.h"
#include "utils.h" // Needed for assert_helper.h
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
    ctx.test_1.x[0] = 4;
    ctx.test_2.x[0] = 7;
    mc_result_t res = mc_sys_read_input(&sys, 0);
    TEST_ASSERT_TRUE(res.ok);
    TEST_ASSERT_EQUAL_INT32(4, res.value);

    res = mc_sys_read_input(&sys, 2);
    TEST_ASSERT_TRUE(res.ok);
    TEST_ASSERT_EQUAL_INT32(7, res.value);
}

void test_read_output_succeeds()
{
    ctx.test_1.y[0] = 5;
    ctx.test_2.y[0] = 2;
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
    TEST_ASSERT_EQUAL_INT32(0, ctx.test_1.y[0]);
    TEST_ASSERT_EQUAL_INT32(0, ctx.test_2.y[0]);

    mc_status_t ret = mc_sys_invoke(&sys, 0, NULL, 0);
    TEST_ASSERT_EQUAL_INT32(MC_OK, ret);
    TEST_ASSERT_EQUAL_INT32(1, ctx.test_1.y[0]);

    ret = mc_sys_invoke(&sys, 2, NULL, 0);
    TEST_ASSERT_EQUAL_INT32(MC_OK, ret);
    TEST_ASSERT_EQUAL_INT32(1, ctx.test_2.y[0]);
}

void test_get_counts_succeeds()
{
    TEST_ASSERT_EQUAL_INT32(4, mc_sys_get_function_count(&sys));
    TEST_ASSERT_EQUAL_INT32(4, mc_sys_get_input_count(&sys));
    TEST_ASSERT_EQUAL_INT32(2, mc_sys_get_output_count(&sys));
}

void test_parse_command_succeeds()
{
    mc_sys_cmd_info_t cmd;
    ctx.test_1.increment_y_name = "incrementY0";
    ctx.test_2.increment_y_name = "incrementY1";
    ctx.test_1.x0_name = "input0";
    ctx.test_2.x0_name = "input1";
    ctx.test_1.y0_name = "output0";
    ctx.test_2.y0_name = "output1";

    TEST_ASSERT_TRUE(sys.driver->parse_command(&ctx, "incrementY0", &cmd));
    TEST_ASSERT_EQUAL_INT32(MC_CMD_TYPE_FUNC, cmd.type);
    TEST_ASSERT_EQUAL_INT32(0, cmd.id); // Should be f0
    TEST_ASSERT_FALSE(cmd.has_preset);

    TEST_ASSERT_TRUE(sys.driver->parse_command(&ctx, "incrementY1", &cmd));
    TEST_ASSERT_EQUAL_INT32(MC_CMD_TYPE_FUNC, cmd.type);
    TEST_ASSERT_EQUAL_INT32(2, cmd.id); // Should be f2
    TEST_ASSERT_FALSE(cmd.has_preset);

    TEST_ASSERT_TRUE(sys.driver->parse_command(&ctx, "input0", &cmd));
    TEST_ASSERT_EQUAL_INT32(MC_CMD_TYPE_INPUT, cmd.type);
    TEST_ASSERT_EQUAL_INT32(0, cmd.id); // Should be x0
    TEST_ASSERT_FALSE(cmd.has_preset);

    TEST_ASSERT_TRUE(sys.driver->parse_command(&ctx, "input1", &cmd));
    TEST_ASSERT_EQUAL_INT32(MC_CMD_TYPE_INPUT, cmd.type);
    TEST_ASSERT_EQUAL_INT32(2, cmd.id); // Should be x2
    TEST_ASSERT_FALSE(cmd.has_preset);

    TEST_ASSERT_TRUE(sys.driver->parse_command(&ctx, "output0", &cmd));
    TEST_ASSERT_EQUAL_INT32(MC_CMD_TYPE_OUTPUT, cmd.type);
    TEST_ASSERT_EQUAL_INT32(0, cmd.id); // Should be y0
    TEST_ASSERT_FALSE(cmd.has_preset);

    TEST_ASSERT_TRUE(sys.driver->parse_command(&ctx, "output1", &cmd));
    TEST_ASSERT_EQUAL_INT32(MC_CMD_TYPE_OUTPUT, cmd.type);
    TEST_ASSERT_EQUAL_INT32(1, cmd.id); // Should be y1
    TEST_ASSERT_FALSE(cmd.has_preset);
}