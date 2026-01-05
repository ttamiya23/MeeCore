#include "unity.h"
#include "mc/system/core.h"
#include "mc/system/modules/digital_system.h"
#include "mc/device/digital.h"
#include "fakes/device/fake_digital.h"
#include "utils.h" // Needed for assert_helper.h
#include "assert_helper.h"

// Globals. We will mock mc_digital functions.
fake_digital_ctx_t dev_ctx;
MC_DEFINE_DIGITAL(dev, fake_digital_driver, dev_ctx);
MC_DEFINE_DIGITAL_SYSTEM(sys, dev);
mc_digital_system_ctx_t *ctx;

void setUp()
{
    ctx = (mc_digital_system_ctx_t *)sys.ctx;
    mc_digital_init(&dev, /*is_read_only=*/false);
    mc_sys_init(&sys);
}

void test_init_sets_target_state_to_current_state()
{
    TEST_ASSERT_FALSE(ctx->target_state);
    dev_ctx.state = true;
    mc_sys_init(&sys);
    TEST_ASSERT_TRUE(ctx->target_state);
}

void test_write_input_sets_state()
{
    // x0 = 1
    mc_status_t ret = mc_sys_write_input(&sys, 0, 1);
    TEST_ASSERT_EQUAL(MC_OK, ret);
    TEST_ASSERT_TRUE(ctx->target_state);
    TEST_ASSERT_TRUE(dev_ctx.state);

    // x0 = 0
    ret = mc_sys_write_input(&sys, 0, 0);
    TEST_ASSERT_EQUAL(MC_OK, ret);
    TEST_ASSERT_FALSE(ctx->target_state);
    TEST_ASSERT_FALSE(dev_ctx.state);
}

void test_read_input_gets_target_state()
{
    // Set conflicting target state and actual state
    ctx->target_state = true;
    dev_ctx.state = false;
    mc_result_t res = mc_sys_read_input(&sys, 0);
    TEST_ASSERT_TRUE(res.ok);
    TEST_ASSERT_TRUE(res.value);

    ctx->target_state = false;
    dev_ctx.state = true;
    res = mc_sys_read_input(&sys, 0);
    TEST_ASSERT_TRUE(res.ok);
    TEST_ASSERT_FALSE(res.value);
}

void test_read_output_gets_actual_state()
{
    // Set conflicting target state and actual state
    ctx->target_state = true;
    dev_ctx.state = false;
    mc_result_t res = mc_sys_read_output(&sys, 0);
    TEST_ASSERT_TRUE(res.ok);
    TEST_ASSERT_FALSE(res.value);

    ctx->target_state = false;
    dev_ctx.state = true;
    res = mc_sys_read_output(&sys, 0);
    TEST_ASSERT_TRUE(res.ok);
    TEST_ASSERT_TRUE(res.value);
}

void test_invoke_function_toggles_state()
{
    ctx->target_state = false;
    dev_ctx.state = false;
    mc_status_t ret = mc_sys_invoke(&sys, 0, NULL, 0);
    TEST_ASSERT_EQUAL(MC_OK, ret);
    TEST_ASSERT_TRUE(ctx->target_state);
    TEST_ASSERT_TRUE(dev_ctx.state);

    ret = mc_sys_invoke(&sys, 0, NULL, 0);
    TEST_ASSERT_EQUAL(MC_OK, ret);
    TEST_ASSERT_FALSE(ctx->target_state);
    TEST_ASSERT_FALSE(dev_ctx.state);

    // If, for some reason, target and actual state are not in sync, toggle
    // should force them in sync.
    ctx->target_state = false;
    dev_ctx.state = true;
    ret = mc_sys_invoke(&sys, 0, NULL, 0);
    TEST_ASSERT_EQUAL(MC_OK, ret);
    TEST_ASSERT_FALSE(ctx->target_state);
    TEST_ASSERT_FALSE(dev_ctx.state);

    ctx->target_state = true;
    dev_ctx.state = false;
    ret = mc_sys_invoke(&sys, 0, NULL, 0);
    TEST_ASSERT_EQUAL(MC_OK, ret);
    TEST_ASSERT_TRUE(ctx->target_state);
    TEST_ASSERT_TRUE(dev_ctx.state);
}

void test_get_alias_succeeds()
{
    mc_sys_cmd_info_t cmd;
    mc_status_t ret;

    ret = mc_sys_get_alias(&sys, 0, &cmd);
    TEST_ASSERT_EQUAL_INT32(MC_OK, ret);
    TEST_ASSERT_EQUAL_STRING("turnOn", cmd.alias);
    TEST_ASSERT_EQUAL_INT32(MC_CMD_TYPE_INPUT, cmd.type);
    TEST_ASSERT_EQUAL_INT32(0, cmd.id); // Should be x0
    TEST_ASSERT_TRUE(cmd.has_preset);
    TEST_ASSERT_EQUAL_INT32(1, cmd.preset_val);

    ret = mc_sys_get_alias(&sys, 1, &cmd);
    TEST_ASSERT_EQUAL_INT32(MC_OK, ret);
    TEST_ASSERT_EQUAL_STRING("turnOff", cmd.alias);
    TEST_ASSERT_EQUAL_INT32(MC_CMD_TYPE_INPUT, cmd.type);
    TEST_ASSERT_EQUAL_INT32(0, cmd.id); // Should be x0
    TEST_ASSERT_TRUE(cmd.has_preset);
    TEST_ASSERT_EQUAL_INT32(0, cmd.preset_val);

    ret = mc_sys_get_alias(&sys, 2, &cmd);
    TEST_ASSERT_EQUAL_INT32(MC_OK, ret);
    TEST_ASSERT_EQUAL_STRING("toggle", cmd.alias);
    TEST_ASSERT_EQUAL_INT32(MC_CMD_TYPE_FUNC, cmd.type);
    TEST_ASSERT_EQUAL_INT32(0, cmd.id); // Should be f0
    TEST_ASSERT_FALSE(cmd.has_preset);

    // Invalid ID should fail
    ret = mc_sys_get_alias(&sys, 3, &cmd);
    TEST_ASSERT_EQUAL_INT32(MC_ERROR_INVALID_ARGS, ret);
}

void test_get_member_count_succeeds()
{
    TEST_ASSERT_EQUAL(1, mc_sys_get_function_count(&sys));
    TEST_ASSERT_EQUAL(1, mc_sys_get_input_count(&sys));
    TEST_ASSERT_EQUAL(1, mc_sys_get_output_count(&sys));
    TEST_ASSERT_EQUAL(3, mc_sys_get_alias_count(&sys));
}

void test_read_only_write_input_fails()
{
    mc_digital_set_read_only(&dev, true);
    mc_status_t ret = mc_sys_write_input(&sys, 0, 1);
    TEST_ASSERT_EQUAL(MC_ERROR_INVALID_ARGS, ret);
}

void test_read_only_read_input_fails()
{
    mc_digital_set_read_only(&dev, true);
    mc_result_t res = mc_sys_read_input(&sys, 0);
    TEST_ASSERT_FALSE(res.ok);
    TEST_ASSERT_EQUAL(MC_ERROR_INVALID_ARGS, res.error);
}

void test_read_only_read_output_succeeds()
{
    mc_digital_set_read_only(&dev, true);
    dev_ctx.state = false;
    mc_result_t res = mc_sys_read_output(&sys, 0);
    TEST_ASSERT_TRUE(res.ok);
    TEST_ASSERT_FALSE(res.value);
}

void test_read_only_invoke_function_fails()
{
    mc_digital_set_read_only(&dev, true);
    mc_status_t ret = mc_sys_invoke(&sys, 0, NULL, 0);
    TEST_ASSERT_EQUAL(MC_ERROR_INVALID_ARGS, ret);
}

void test_read_only_get_alias_fails()
{
    mc_digital_set_read_only(&dev, true);
    mc_sys_cmd_info_t cmd;
    mc_status_t ret = mc_sys_get_alias(&sys, 0, &cmd);
    TEST_ASSERT_EQUAL_INT32(MC_ERROR_INVALID_ARGS, ret);
}

void test_read_only_get_member_count_succeeds()
{
    mc_digital_set_read_only(&dev, true);
    TEST_ASSERT_EQUAL(0, mc_sys_get_function_count(&sys));
    TEST_ASSERT_EQUAL(0, mc_sys_get_input_count(&sys));
    TEST_ASSERT_EQUAL(1, mc_sys_get_output_count(&sys));
    TEST_ASSERT_EQUAL(0, mc_sys_get_alias_count(&sys));
}