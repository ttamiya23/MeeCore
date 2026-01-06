#include "unity.h"
#include "mc/system/core.h"
#include "mc/system/modules/analog_vector_system.h"
#include "mc/system/modules/analog_system.h"
#include "mc/device/analog_vector.h"
#include "mc/device/analog.h"
#include "fakes/device/fake_analog_vector.h"
#include "fakes/device/fake_analog.h"
#include "utils.h" // Needed for assert_helper.h
#include "assert_helper.h"

// Globals. We will mock mc_analog functions.
fake_analog_vector_ctx_t dev_ctx;
MC_DEFINE_ANALOG_VECTOR(dev, fake_analog_vector_driver, dev_ctx);
MC_DEFINE_ANALOG_VECTOR_SYSTEM(sys, dev);
mc_analog_vector_system_ctx_t *ctx;

mc_analog_vector_system_config_t custom_config = {
    .x = {.value_name = "myX",
          .target_value_name = "myTargetX"},
    .y = {.value_name = "myY",
          .target_value_name = "myTargetY"},
    .z = {.value_name = "myZ",
          .target_value_name = "myTargetZ"},
};

MC_DEFINE_ANALOG_VECTOR_SYSTEM_WITH_CONFIG(custom_sys, custom_config, dev);

void setUp()
{
    ctx = (mc_analog_vector_system_ctx_t *)sys.ctx;
    mc_analog_vector_init(&dev, /*is_read_only=*/false);
    mc_sys_init(&sys);
}

void test_init_sets_target_value_to_current_value()
{
    dev_ctx.x.value = 14243;
    dev_ctx.y.value = 14244;
    dev_ctx.z.value = 14245;
    mc_sys_init(&sys);
    TEST_ASSERT_EQUAL_INT32(14243, ctx->x.target_value);
    TEST_ASSERT_EQUAL_INT32(14244, ctx->y.target_value);
    TEST_ASSERT_EQUAL_INT32(14245, ctx->z.target_value);
}

void test_write_input_sets_value()
{
    // x0 = 1
    mc_status_t ret = mc_sys_write_input(&sys, 0, 1);
    TEST_ASSERT_EQUAL(MC_OK, ret);
    TEST_ASSERT_EQUAL_INT32(1, ctx->x.target_value);
    TEST_ASSERT_EQUAL_INT32(1, dev_ctx.x.value);
    // x1 = 2
    ret = mc_sys_write_input(&sys, 1, 2);
    TEST_ASSERT_EQUAL(MC_OK, ret);
    TEST_ASSERT_EQUAL_INT32(2, ctx->y.target_value);
    TEST_ASSERT_EQUAL_INT32(2, dev_ctx.y.value);
    // x2 = 3
    ret = mc_sys_write_input(&sys, 2, 3);
    TEST_ASSERT_EQUAL(MC_OK, ret);
    TEST_ASSERT_EQUAL_INT32(3, ctx->z.target_value);
    TEST_ASSERT_EQUAL_INT32(3, dev_ctx.z.value);
}

void test_read_input_gets_target_value()
{
    // Set conflicting target value and actual value
    ctx->x.target_value = 2495;
    dev_ctx.x.value = -1323;
    mc_result_t res = mc_sys_read_input(&sys, 0);
    TEST_ASSERT_TRUE(res.ok);
    TEST_ASSERT_EQUAL_INT32(2495, res.value);

    ctx->y.target_value = 2496;
    dev_ctx.y.value = -1322;
    res = mc_sys_read_input(&sys, 1);
    TEST_ASSERT_TRUE(res.ok);
    TEST_ASSERT_EQUAL_INT32(2496, res.value);

    ctx->z.target_value = 2497;
    dev_ctx.z.value = -1321;
    res = mc_sys_read_input(&sys, 2);
    TEST_ASSERT_TRUE(res.ok);
    TEST_ASSERT_EQUAL_INT32(2497, res.value);
}

void test_read_output_gets_actual_value()
{
    // Set conflicting target state and actual state
    ctx->x.target_value = 2495;
    dev_ctx.x.value = -1323;
    mc_result_t res = mc_sys_read_output(&sys, 0);
    TEST_ASSERT_TRUE(res.ok);
    TEST_ASSERT_EQUAL_INT32(-1323, res.value);

    ctx->y.target_value = 2496;
    dev_ctx.y.value = -1322;
    res = mc_sys_read_output(&sys, 1);
    TEST_ASSERT_TRUE(res.ok);
    TEST_ASSERT_EQUAL_INT32(-1322, res.value);

    ctx->z.target_value = 2497;
    dev_ctx.z.value = -1321;
    res = mc_sys_read_output(&sys, 2);
    TEST_ASSERT_TRUE(res.ok);
    TEST_ASSERT_EQUAL_INT32(-1321, res.value);
}

void test_invoke_function_fails()
{
    mc_status_t ret = mc_sys_invoke(&sys, 0, NULL, 0);
    TEST_ASSERT_EQUAL(MC_ERROR_INVALID_ARGS, ret);
}

void test_get_alias_succeeds()
{
    mc_sys_cmd_info_t cmd;
    mc_status_t ret;

    ret = mc_sys_get_alias(&sys, 0, &cmd);
    TEST_ASSERT_EQUAL_INT32(MC_OK, ret);
    TEST_ASSERT_EQUAL_STRING("x", cmd.alias);
    TEST_ASSERT_EQUAL_INT32(MC_CMD_TYPE_OUTPUT, cmd.type);
    TEST_ASSERT_EQUAL_INT32(0, cmd.id); // Should be y0
    TEST_ASSERT_FALSE(cmd.has_preset);

    ret = mc_sys_get_alias(&sys, 1, &cmd);
    TEST_ASSERT_EQUAL_INT32(MC_OK, ret);
    TEST_ASSERT_EQUAL_STRING("targetX", cmd.alias);
    TEST_ASSERT_EQUAL_INT32(MC_CMD_TYPE_INPUT, cmd.type);
    TEST_ASSERT_EQUAL_INT32(0, cmd.id); // Should be x0
    TEST_ASSERT_FALSE(cmd.has_preset);

    ret = mc_sys_get_alias(&sys, 2, &cmd);
    TEST_ASSERT_EQUAL_INT32(MC_OK, ret);
    TEST_ASSERT_EQUAL_STRING("y", cmd.alias);
    TEST_ASSERT_EQUAL_INT32(MC_CMD_TYPE_OUTPUT, cmd.type);
    TEST_ASSERT_EQUAL_INT32(1, cmd.id); // Should be y1
    TEST_ASSERT_FALSE(cmd.has_preset);

    ret = mc_sys_get_alias(&sys, 3, &cmd);
    TEST_ASSERT_EQUAL_INT32(MC_OK, ret);
    TEST_ASSERT_EQUAL_STRING("targetY", cmd.alias);
    TEST_ASSERT_EQUAL_INT32(MC_CMD_TYPE_INPUT, cmd.type);
    TEST_ASSERT_EQUAL_INT32(1, cmd.id); // Should be x1
    TEST_ASSERT_FALSE(cmd.has_preset);

    ret = mc_sys_get_alias(&sys, 4, &cmd);
    TEST_ASSERT_EQUAL_INT32(MC_OK, ret);
    TEST_ASSERT_EQUAL_STRING("z", cmd.alias);
    TEST_ASSERT_EQUAL_INT32(MC_CMD_TYPE_OUTPUT, cmd.type);
    TEST_ASSERT_EQUAL_INT32(2, cmd.id); // Should be y2
    TEST_ASSERT_FALSE(cmd.has_preset);

    ret = mc_sys_get_alias(&sys, 5, &cmd);
    TEST_ASSERT_EQUAL_INT32(MC_OK, ret);
    TEST_ASSERT_EQUAL_STRING("targetZ", cmd.alias);
    TEST_ASSERT_EQUAL_INT32(MC_CMD_TYPE_INPUT, cmd.type);
    TEST_ASSERT_EQUAL_INT32(2, cmd.id); // Should be x2
    TEST_ASSERT_FALSE(cmd.has_preset);

    // Invalid ID should fail
    ret = mc_sys_get_alias(&sys, 6, &cmd);
    TEST_ASSERT_EQUAL_INT32(MC_ERROR_INVALID_ARGS, ret);
}

void test_get_alias_with_custom_succeeds()
{
    mc_sys_cmd_info_t cmd;
    mc_status_t ret;

    ret = mc_sys_get_alias(&custom_sys, 0, &cmd);
    TEST_ASSERT_EQUAL_INT32(MC_OK, ret);
    TEST_ASSERT_EQUAL_STRING("myX", cmd.alias);

    ret = mc_sys_get_alias(&custom_sys, 1, &cmd);
    TEST_ASSERT_EQUAL_INT32(MC_OK, ret);
    TEST_ASSERT_EQUAL_STRING("myTargetX", cmd.alias);

    ret = mc_sys_get_alias(&custom_sys, 2, &cmd);
    TEST_ASSERT_EQUAL_INT32(MC_OK, ret);
    TEST_ASSERT_EQUAL_STRING("myY", cmd.alias);

    ret = mc_sys_get_alias(&custom_sys, 3, &cmd);
    TEST_ASSERT_EQUAL_INT32(MC_OK, ret);
    TEST_ASSERT_EQUAL_STRING("myTargetY", cmd.alias);

    ret = mc_sys_get_alias(&custom_sys, 4, &cmd);
    TEST_ASSERT_EQUAL_INT32(MC_OK, ret);
    TEST_ASSERT_EQUAL_STRING("myZ", cmd.alias);

    ret = mc_sys_get_alias(&custom_sys, 5, &cmd);
    TEST_ASSERT_EQUAL_INT32(MC_OK, ret);
    TEST_ASSERT_EQUAL_STRING("myTargetZ", cmd.alias);
}

void test_get_member_count_succeeds()
{
    TEST_ASSERT_EQUAL(0, mc_sys_get_function_count(&sys));
    TEST_ASSERT_EQUAL(3, mc_sys_get_input_count(&sys));
    TEST_ASSERT_EQUAL(3, mc_sys_get_output_count(&sys));
    TEST_ASSERT_EQUAL(6, mc_sys_get_alias_count(&sys));
}

void test_read_only_write_input_fails()
{
    mc_analog_vector_set_read_only(&dev, true);
    mc_status_t ret = mc_sys_write_input(&sys, 0, 1);
    TEST_ASSERT_EQUAL(MC_ERROR_INVALID_ARGS, ret);
}

void test_read_only_read_input_fails()
{
    mc_analog_vector_set_read_only(&dev, true);
    mc_result_t res = mc_sys_read_input(&sys, 0);
    TEST_ASSERT_FALSE(res.ok);
    TEST_ASSERT_EQUAL(MC_ERROR_INVALID_ARGS, res.error);
}

void test_read_only_read_output_succeeds()
{
    mc_analog_vector_set_read_only(&dev, true);
    dev_ctx.x.value = 122;
    mc_result_t res = mc_sys_read_output(&sys, 0);
    TEST_ASSERT_TRUE(res.ok);
    TEST_ASSERT_EQUAL_INT32(122, res.value);

    dev_ctx.y.value = 123;
    res = mc_sys_read_output(&sys, 1);
    TEST_ASSERT_TRUE(res.ok);
    TEST_ASSERT_EQUAL_INT32(123, res.value);

    dev_ctx.z.value = 124;
    res = mc_sys_read_output(&sys, 2);
    TEST_ASSERT_TRUE(res.ok);
    TEST_ASSERT_EQUAL_INT32(124, res.value);
}

void test_read_only_invoke_function_fails()
{
    mc_analog_vector_set_read_only(&dev, true);
    mc_status_t ret = mc_sys_invoke(&sys, 0, NULL, 0);
    TEST_ASSERT_EQUAL(MC_ERROR_INVALID_ARGS, ret);
}

void test_read_only_get_alias_succeeds()
{
    mc_analog_vector_set_read_only(&dev, true);
    mc_sys_cmd_info_t cmd;
    mc_status_t ret = mc_sys_get_alias(&sys, 0, &cmd);
    TEST_ASSERT_EQUAL_INT32(MC_OK, ret);
    TEST_ASSERT_EQUAL_STRING("x", cmd.alias);
    TEST_ASSERT_EQUAL_INT32(MC_CMD_TYPE_OUTPUT, cmd.type);
    TEST_ASSERT_EQUAL_INT32(0, cmd.id); // Should be y0
    TEST_ASSERT_FALSE(cmd.has_preset);

    ret = mc_sys_get_alias(&sys, 1, &cmd);
    TEST_ASSERT_EQUAL_INT32(MC_OK, ret);
    TEST_ASSERT_EQUAL_STRING("y", cmd.alias);
    TEST_ASSERT_EQUAL_INT32(MC_CMD_TYPE_OUTPUT, cmd.type);
    TEST_ASSERT_EQUAL_INT32(1, cmd.id); // Should be y1
    TEST_ASSERT_FALSE(cmd.has_preset);

    ret = mc_sys_get_alias(&sys, 2, &cmd);
    TEST_ASSERT_EQUAL_INT32(MC_OK, ret);
    TEST_ASSERT_EQUAL_STRING("z", cmd.alias);
    TEST_ASSERT_EQUAL_INT32(MC_CMD_TYPE_OUTPUT, cmd.type);
    TEST_ASSERT_EQUAL_INT32(2, cmd.id); // Should be y2
    TEST_ASSERT_FALSE(cmd.has_preset);

    // Invalid ID should fail
    ret = mc_sys_get_alias(&sys, 3, &cmd);
    TEST_ASSERT_EQUAL_INT32(MC_ERROR_INVALID_ARGS, ret);
}

void test_read_only_get_alias_with_custom_succeeds()
{
    mc_analog_vector_set_read_only(&dev, true);
    mc_sys_cmd_info_t cmd;
    mc_status_t ret = mc_sys_get_alias(&custom_sys, 0, &cmd);
    TEST_ASSERT_EQUAL_INT32(MC_OK, ret);
    TEST_ASSERT_EQUAL_STRING("myX", cmd.alias);

    ret = mc_sys_get_alias(&custom_sys, 1, &cmd);
    TEST_ASSERT_EQUAL_INT32(MC_OK, ret);
    TEST_ASSERT_EQUAL_STRING("myY", cmd.alias);

    ret = mc_sys_get_alias(&custom_sys, 2, &cmd);
    TEST_ASSERT_EQUAL_INT32(MC_OK, ret);
    TEST_ASSERT_EQUAL_STRING("myZ", cmd.alias);
}

void test_read_only_get_member_count_succeeds()
{
    mc_analog_vector_set_read_only(&dev, true);
    TEST_ASSERT_EQUAL(0, mc_sys_get_function_count(&sys));
    TEST_ASSERT_EQUAL(0, mc_sys_get_input_count(&sys));
    TEST_ASSERT_EQUAL(3, mc_sys_get_output_count(&sys));
    TEST_ASSERT_EQUAL(3, mc_sys_get_alias_count(&sys));
}
