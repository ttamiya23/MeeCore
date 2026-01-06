#include "unity.h"
#include "mc/system/core.h"
#include "mc/system/modules/analog_system.h"
#include "mc/device/analog.h"
#include "fakes/device/fake_analog.h"
#include "utils.h" // Needed for assert_helper.h
#include "assert_helper.h"

// Globals. We will mock mc_analog functions.
fake_analog_ctx_t dev_ctx;
MC_DEFINE_ANALOG(dev, fake_analog_driver, dev_ctx);
MC_DEFINE_ANALOG_SYSTEM(sys, dev);
mc_analog_system_ctx_t *ctx;

mc_analog_system_config_t custom_config = {
    .value_name = "myValue",
    .target_value_name = "myTargetValue"};

MC_DEFINE_ANALOG_SYSTEM_WITH_CONFIG(custom_sys, custom_config, dev);

void setUp()
{
    ctx = (mc_analog_system_ctx_t *)sys.ctx;
    mc_analog_init(&dev, /*is_read_only=*/false);
    mc_sys_init(&sys);
}

void test_init_sets_target_value_to_current_value()
{
    dev_ctx.value = 14243;
    mc_sys_init(&sys);
    TEST_ASSERT_EQUAL_INT32(14243, ctx->target_value);
}

void test_write_input_sets_value()
{
    // x0 = 1
    mc_status_t ret = mc_sys_write_input(&sys, 0, 1);
    TEST_ASSERT_EQUAL(MC_OK, ret);
    TEST_ASSERT_EQUAL_INT32(1, ctx->target_value);
    TEST_ASSERT_EQUAL_INT32(1, dev_ctx.value);
}

void test_read_input_gets_target_value()
{
    // Set conflicting target value and actual value
    ctx->target_value = 2495;
    dev_ctx.value = -1323;
    mc_result_t res = mc_sys_read_input(&sys, 0);
    TEST_ASSERT_TRUE(res.ok);
    TEST_ASSERT_EQUAL_INT32(2495, res.value);
}

void test_read_output_gets_actual_value()
{
    // Set conflicting target state and actual state
    ctx->target_value = 2495;
    dev_ctx.value = -1323;
    mc_result_t res = mc_sys_read_output(&sys, 0);
    TEST_ASSERT_TRUE(res.ok);
    TEST_ASSERT_EQUAL_INT32(-1323, res.value);
}

void test_invoke_function_fails()
{
    mc_status_t ret = mc_sys_invoke(&sys, 0, NULL, 0);
    TEST_ASSERT_EQUAL(MC_ERROR_NOT_SUPPORTED, ret);
}

void test_get_alias_succeeds()
{
    mc_sys_cmd_info_t cmd;
    mc_status_t ret;

    ret = mc_sys_get_alias(&sys, 0, &cmd);
    TEST_ASSERT_EQUAL_INT32(MC_OK, ret);
    TEST_ASSERT_EQUAL_STRING("value", cmd.alias);
    TEST_ASSERT_EQUAL_INT32(MC_CMD_TYPE_OUTPUT, cmd.type);
    TEST_ASSERT_EQUAL_INT32(0, cmd.id); // Should be y0
    TEST_ASSERT_FALSE(cmd.has_preset);

    ret = mc_sys_get_alias(&sys, 1, &cmd);
    TEST_ASSERT_EQUAL_INT32(MC_OK, ret);
    TEST_ASSERT_EQUAL_STRING("targetValue", cmd.alias);
    TEST_ASSERT_EQUAL_INT32(MC_CMD_TYPE_INPUT, cmd.type);
    TEST_ASSERT_EQUAL_INT32(0, cmd.id); // Should be x0
    TEST_ASSERT_FALSE(cmd.has_preset);

    // Invalid ID should fail
    ret = mc_sys_get_alias(&sys, 2, &cmd);
    TEST_ASSERT_EQUAL_INT32(MC_ERROR_INVALID_ARGS, ret);
}

void test_get_alias_with_custom_succeeds()
{
    mc_sys_cmd_info_t cmd;
    mc_status_t ret;

    ret = mc_sys_get_alias(&custom_sys, 0, &cmd);
    TEST_ASSERT_EQUAL_INT32(MC_OK, ret);
    TEST_ASSERT_EQUAL_STRING("myValue", cmd.alias);

    ret = mc_sys_get_alias(&custom_sys, 1, &cmd);
    TEST_ASSERT_EQUAL_INT32(MC_OK, ret);
    TEST_ASSERT_EQUAL_STRING("myTargetValue", cmd.alias);
}

void test_get_member_count_succeeds()
{
    TEST_ASSERT_EQUAL(0, mc_sys_get_function_count(&sys));
    TEST_ASSERT_EQUAL(1, mc_sys_get_input_count(&sys));
    TEST_ASSERT_EQUAL(1, mc_sys_get_output_count(&sys));
    TEST_ASSERT_EQUAL(2, mc_sys_get_alias_count(&sys));
}

void test_read_only_write_input_fails()
{
    mc_analog_set_read_only(&dev, true);
    mc_status_t ret = mc_sys_write_input(&sys, 0, 1);
    TEST_ASSERT_EQUAL(MC_ERROR_INVALID_ARGS, ret);
}

void test_read_only_read_input_fails()
{
    mc_analog_set_read_only(&dev, true);
    mc_result_t res = mc_sys_read_input(&sys, 0);
    TEST_ASSERT_FALSE(res.ok);
    TEST_ASSERT_EQUAL(MC_ERROR_INVALID_ARGS, res.error);
}

void test_read_only_read_output_succeeds()
{
    mc_analog_set_read_only(&dev, true);
    dev_ctx.value = 122;
    mc_result_t res = mc_sys_read_output(&sys, 0);
    TEST_ASSERT_TRUE(res.ok);
    TEST_ASSERT_EQUAL_INT32(122, res.value);
}

void test_read_only_invoke_function_fails()
{
    mc_analog_set_read_only(&dev, true);
    mc_status_t ret = mc_sys_invoke(&sys, 0, NULL, 0);
    TEST_ASSERT_EQUAL(MC_ERROR_NOT_SUPPORTED, ret);
}

void test_read_only_get_alias_succeeds()
{
    mc_analog_set_read_only(&dev, true);
    mc_sys_cmd_info_t cmd;
    mc_status_t ret = mc_sys_get_alias(&sys, 0, &cmd);
    TEST_ASSERT_EQUAL_INT32(MC_OK, ret);
    TEST_ASSERT_EQUAL_STRING("value", cmd.alias);
    TEST_ASSERT_EQUAL_INT32(MC_CMD_TYPE_OUTPUT, cmd.type);
    TEST_ASSERT_EQUAL_INT32(0, cmd.id); // Should be y0
    TEST_ASSERT_FALSE(cmd.has_preset);

    // Invalid ID should fail
    ret = mc_sys_get_alias(&sys, 1, &cmd);
    TEST_ASSERT_EQUAL_INT32(MC_ERROR_INVALID_ARGS, ret);
}

void test_read_only_get_alias_with_custom_succeeds()
{
    mc_analog_set_read_only(&dev, true);
    mc_sys_cmd_info_t cmd;
    mc_status_t ret = mc_sys_get_alias(&custom_sys, 0, &cmd);
    TEST_ASSERT_EQUAL_INT32(MC_OK, ret);
    TEST_ASSERT_EQUAL_STRING("myValue", cmd.alias);
}

void test_read_only_get_member_count_succeeds()
{
    mc_analog_set_read_only(&dev, true);
    TEST_ASSERT_EQUAL(0, mc_sys_get_function_count(&sys));
    TEST_ASSERT_EQUAL(0, mc_sys_get_input_count(&sys));
    TEST_ASSERT_EQUAL(1, mc_sys_get_output_count(&sys));
    TEST_ASSERT_EQUAL(1, mc_sys_get_alias_count(&sys));
}