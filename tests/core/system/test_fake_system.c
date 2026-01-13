#include "unity.h"
#include "mc/system/core.h"
#include "fakes/system/fake_system.h"
#include "utils.h" // Needed for assert_helper.h
#include "assert_helper.h"

// Globals
fake_sys_ctx_t ctx;

void setUp()
{
    test_assert_init();
    fake_sys_driver.init(&ctx);
}

void test_init_resets_context()
{
    // Artificially set data in x
    ctx.x[0] = 5;
    fake_sys_driver.init(&ctx);

    // Should reset to 0
    TEST_ASSERT_EQUAL_INT32(0, ctx.x[0]);
}

void test_write_input_succeeds()
{
    mc_status_t ret = fake_sys_driver.write_input(&ctx, 0, 3);
    TEST_ASSERT_EQUAL_INT32(MC_OK, ret);
    TEST_ASSERT_EQUAL_INT32(3, ctx.x[0]);

    ret = fake_sys_driver.write_input(&ctx, 1, 8);
    TEST_ASSERT_EQUAL_INT32(MC_OK, ret);
    TEST_ASSERT_EQUAL_INT32(3, ctx.x[0]);
    TEST_ASSERT_EQUAL_INT32(8, ctx.x[1]);
}

void test_read_input_succeeds()
{
    ctx.x[0] = 4;
    ctx.x[1] = 7;
    mc_result_t res = fake_sys_driver.read_input(&ctx, 0);
    TEST_ASSERT_TRUE(res.ok);
    TEST_ASSERT_EQUAL_INT32(4, res.value);

    res = fake_sys_driver.read_input(&ctx, 1);
    TEST_ASSERT_TRUE(res.ok);
    TEST_ASSERT_EQUAL_INT32(7, res.value);
}

void test_read_output_succeeds()
{
    ctx.y[0] = 5;
    mc_result_t res = fake_sys_driver.read_output(&ctx, 0);
    TEST_ASSERT_TRUE(res.ok);
    TEST_ASSERT_EQUAL_INT32(5, res.value);
}

void test_invoke_succeeds()
{
    // Assert that y0 is 0
    TEST_ASSERT_EQUAL_INT32(0, ctx.y[0]);

    mc_status_t ret = fake_sys_driver.invoke(&ctx, 0, NULL, 0);
    TEST_ASSERT_EQUAL_INT32(MC_OK, ret);

    // New value should be 1
    TEST_ASSERT_EQUAL_INT32(1, ctx.y[0]);
}

void test_error_gets_propogated()
{
    // Invoking f1 should trigger errors
    int32_t args[1];
    args[0] = MC_ERROR_BUSY;
    mc_status_t ret = fake_sys_driver.invoke(&ctx, 1, args, 1);
    TEST_ASSERT_EQUAL_INT32(MC_ERROR_BUSY, ret);
}

void test_get_counts_succeeds()
{
    TEST_ASSERT_EQUAL_INT32(2, fake_sys_driver.get_function_count(&ctx));
    TEST_ASSERT_EQUAL_INT32(2, fake_sys_driver.get_input_count(&ctx));
    TEST_ASSERT_EQUAL_INT32(1, fake_sys_driver.get_output_count(&ctx));
    TEST_ASSERT_EQUAL_INT32(4, fake_sys_driver.get_alias_count(&ctx));
}

void test_get_alias_succeeds()
{
    mc_sys_cmd_info_t cmd;
    mc_status_t ret;

    ret = fake_sys_driver.get_alias(&ctx, 0, &cmd);
    TEST_ASSERT_EQUAL_INT32(MC_OK, ret);
    TEST_ASSERT_EQUAL_STRING(DEFAULT_FAKE_INCREMENT_Y_NAME, cmd.alias);
    TEST_ASSERT_EQUAL_INT32(MC_CMD_TYPE_FUNC, cmd.type);
    TEST_ASSERT_EQUAL_INT32(0, cmd.id); // Should be f0
    TEST_ASSERT_FALSE(cmd.has_preset);

    ret = fake_sys_driver.get_alias(&ctx, 1, &cmd);
    TEST_ASSERT_EQUAL_INT32(MC_OK, ret);
    TEST_ASSERT_EQUAL_STRING(DEFAULT_FAKE_X0_NAME, cmd.alias);
    TEST_ASSERT_EQUAL_INT32(MC_CMD_TYPE_INPUT, cmd.type);
    TEST_ASSERT_EQUAL_INT32(0, cmd.id); // Should be x0
    TEST_ASSERT_FALSE(cmd.has_preset);

    ret = fake_sys_driver.get_alias(&ctx, 2, &cmd);
    TEST_ASSERT_EQUAL_INT32(MC_OK, ret);
    TEST_ASSERT_EQUAL_STRING(DEFAULT_FAKE_Y0_NAME, cmd.alias);
    TEST_ASSERT_EQUAL_INT32(MC_CMD_TYPE_OUTPUT, cmd.type);
    TEST_ASSERT_EQUAL_INT32(0, cmd.id); // Should be y0
    TEST_ASSERT_FALSE(cmd.has_preset);

    ret = fake_sys_driver.get_alias(&ctx, 3, &cmd);
    TEST_ASSERT_EQUAL_INT32(MC_OK, ret);
    TEST_ASSERT_EQUAL_STRING(DEFAULT_FAKE_RESET_NAME, cmd.alias);
    TEST_ASSERT_EQUAL_INT32(MC_CMD_TYPE_INPUT, cmd.type);
    TEST_ASSERT_EQUAL_INT32(0, cmd.id); // Should be x0
    TEST_ASSERT_TRUE(cmd.has_preset);
    TEST_ASSERT_EQUAL_INT32(0, cmd.preset_val);

    // Invalid ID should fail
    ret = fake_sys_driver.get_alias(&ctx, 4, &cmd);
    TEST_ASSERT_EQUAL_INT32(MC_ERROR_INVALID_ARGS, ret);
}

void test_get_alias_with_custom_name_succeeds()
{
    mc_sys_cmd_info_t cmd;
    mc_status_t ret;

    const char *new_increment_y_name = "new f0";
    const char *new_x0_name = "new x0";
    const char *new_y0_name = "new y0";
    const char *new_reset_name = "new reset";
    ctx.increment_y_name = new_increment_y_name;
    ctx.x0_name = new_x0_name;
    ctx.y0_name = new_y0_name;
    ctx.reset_name = new_reset_name;

    ret = fake_sys_driver.get_alias(&ctx, 0, &cmd);
    TEST_ASSERT_EQUAL_INT32(MC_OK, ret);
    TEST_ASSERT_EQUAL_STRING(new_increment_y_name, cmd.alias);
    TEST_ASSERT_EQUAL_INT32(MC_CMD_TYPE_FUNC, cmd.type);
    TEST_ASSERT_EQUAL_INT32(0, cmd.id); // Should be f0
    TEST_ASSERT_FALSE(cmd.has_preset);

    ret = fake_sys_driver.get_alias(&ctx, 1, &cmd);
    TEST_ASSERT_EQUAL_INT32(MC_OK, ret);
    TEST_ASSERT_EQUAL_STRING(new_x0_name, cmd.alias);
    TEST_ASSERT_EQUAL_INT32(MC_CMD_TYPE_INPUT, cmd.type);
    TEST_ASSERT_EQUAL_INT32(0, cmd.id); // Should be x0
    TEST_ASSERT_FALSE(cmd.has_preset);

    ret = fake_sys_driver.get_alias(&ctx, 2, &cmd);
    TEST_ASSERT_EQUAL_INT32(MC_OK, ret);
    TEST_ASSERT_EQUAL_STRING(new_y0_name, cmd.alias);
    TEST_ASSERT_EQUAL_INT32(MC_CMD_TYPE_OUTPUT, cmd.type);
    TEST_ASSERT_EQUAL_INT32(0, cmd.id); // Should be y0
    TEST_ASSERT_FALSE(cmd.has_preset);

    ret = fake_sys_driver.get_alias(&ctx, 3, &cmd);
    TEST_ASSERT_EQUAL_INT32(MC_OK, ret);
    TEST_ASSERT_EQUAL_STRING(new_reset_name, cmd.alias);
    TEST_ASSERT_EQUAL_INT32(MC_CMD_TYPE_INPUT, cmd.type);
    TEST_ASSERT_EQUAL_INT32(0, cmd.id); // Should be x0
    TEST_ASSERT_TRUE(cmd.has_preset);
    TEST_ASSERT_EQUAL_INT32(0, cmd.preset_val);
}