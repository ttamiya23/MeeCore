#include <gtest/gtest.h>
#include "mc_test.h"

extern "C"
{
#include "mc/system/core.h"
#include "mc/system/modules/analog_vector3_system.h"
#include "mc/system/modules/analog_system.h"
#include "mc/device/analog_vector3.h"
#include "mc/device/analog.h"
}

namespace
{

    // Globals
    MC_DEFINE_ANALOG_VECTOR3_DATA_OBJECT(dev);
    MC_DEFINE_ANALOG_VECTOR3_SYSTEM(sys, dev);
    mc_analog_vector3_system_ctx_t *ctx;

    mc_analog_vector3_system_config_t custom_config = {
        .x = {.value_name = "myX", .target_value_name = "myTargetX"},
        .y = {.value_name = "myY", .target_value_name = "myTargetY"},
        .z = {.value_name = "myZ", .target_value_name = "myTargetZ"},
    };

    MC_DEFINE_ANALOG_VECTOR3_SYSTEM_WITH_CONFIG(custom_sys, custom_config, dev);

    class AnalogVector3SystemTest : public MeeCoreTest
    {
    protected:
        void SetUp() override
        {
            MeeCoreTest::SetUp();

            // Cast the context pointer from the system struct for easy access in tests
            ctx = (mc_analog_vector3_system_ctx_t *)sys.ctx;

            mc_analog_vector3_init(&dev, /*is_read_only=*/false);
            mc_sys_init(&sys);
            mc_sys_init(&custom_sys);
        }
    };

    TEST_F(AnalogVector3SystemTest, InitSetsTargetValueToCurrentValue)
    {
        dev_ctx.x = 14243;
        dev_ctx.y = 14244;
        dev_ctx.z = 14245;
        mc_sys_init(&sys);
        EXPECT_EQ(14243, ctx->x.target_value);
        EXPECT_EQ(14244, ctx->y.target_value);
        EXPECT_EQ(14245, ctx->z.target_value);
    }

    TEST_F(AnalogVector3SystemTest, WriteInputSetsValue)
    {
        // x0 = 1
        mc_status_t ret = mc_sys_write_input(&sys, 0, 1);
        EXPECT_EQ(MC_OK, ret);
        EXPECT_EQ(1, ctx->x.target_value);
        EXPECT_EQ(1, dev_ctx.x);

        // x1 = 2
        ret = mc_sys_write_input(&sys, 1, 2);
        EXPECT_EQ(MC_OK, ret);
        EXPECT_EQ(2, ctx->y.target_value);
        EXPECT_EQ(2, dev_ctx.y);

        // x2 = 3
        ret = mc_sys_write_input(&sys, 2, 3);
        EXPECT_EQ(MC_OK, ret);
        EXPECT_EQ(3, ctx->z.target_value);
        EXPECT_EQ(3, dev_ctx.z);
    }

    TEST_F(AnalogVector3SystemTest, ReadInputGetsTargetValue)
    {
        // Set conflicting target value and actual value
        ctx->x.target_value = 2495;
        dev_ctx.x = -1323;
        mc_result_t res = mc_sys_read_input(&sys, 0);
        EXPECT_TRUE(res.ok);
        EXPECT_EQ(2495, res.value);

        ctx->y.target_value = 2496;
        dev_ctx.y = -1322;
        res = mc_sys_read_input(&sys, 1);
        EXPECT_TRUE(res.ok);
        EXPECT_EQ(2496, res.value);

        ctx->z.target_value = 2497;
        dev_ctx.z = -1321;
        res = mc_sys_read_input(&sys, 2);
        EXPECT_TRUE(res.ok);
        EXPECT_EQ(2497, res.value);
    }

    TEST_F(AnalogVector3SystemTest, ReadOutputGetsActualValue)
    {
        // Set conflicting target state and actual state
        ctx->x.target_value = 2495;
        dev_ctx.x = -1323;
        mc_result_t res = mc_sys_read_output(&sys, 0);
        EXPECT_TRUE(res.ok);
        EXPECT_EQ(-1323, res.value);

        ctx->y.target_value = 2496;
        dev_ctx.y = -1322;
        res = mc_sys_read_output(&sys, 1);
        EXPECT_TRUE(res.ok);
        EXPECT_EQ(-1322, res.value);

        ctx->z.target_value = 2497;
        dev_ctx.z = -1321;
        res = mc_sys_read_output(&sys, 2);
        EXPECT_TRUE(res.ok);
        EXPECT_EQ(-1321, res.value);
    }

    TEST_F(AnalogVector3SystemTest, InvokeFunctionFails)
    {
        mc_status_t ret = mc_sys_invoke(&sys, 0, NULL, 0);
        EXPECT_EQ(MC_ERROR_INVALID_ARGS, ret);
    }

    TEST_F(AnalogVector3SystemTest, GetAliasSucceeds)
    {
        mc_sys_cmd_info_t cmd;
        mc_status_t ret;

        ret = mc_sys_get_alias(&sys, 0, &cmd);
        EXPECT_EQ(MC_OK, ret);
        EXPECT_STREQ("x", cmd.alias);
        EXPECT_EQ(MC_CMD_TYPE_OUTPUT, cmd.type);
        EXPECT_EQ(0, cmd.id); // Should be y0
        EXPECT_FALSE(cmd.has_preset);

        ret = mc_sys_get_alias(&sys, 1, &cmd);
        EXPECT_EQ(MC_OK, ret);
        EXPECT_STREQ("targetX", cmd.alias);
        EXPECT_EQ(MC_CMD_TYPE_INPUT, cmd.type);
        EXPECT_EQ(0, cmd.id); // Should be x0
        EXPECT_FALSE(cmd.has_preset);

        ret = mc_sys_get_alias(&sys, 2, &cmd);
        EXPECT_EQ(MC_OK, ret);
        EXPECT_STREQ("y", cmd.alias);
        EXPECT_EQ(MC_CMD_TYPE_OUTPUT, cmd.type);
        EXPECT_EQ(1, cmd.id); // Should be y1
        EXPECT_FALSE(cmd.has_preset);

        ret = mc_sys_get_alias(&sys, 3, &cmd);
        EXPECT_EQ(MC_OK, ret);
        EXPECT_STREQ("targetY", cmd.alias);
        EXPECT_EQ(MC_CMD_TYPE_INPUT, cmd.type);
        EXPECT_EQ(1, cmd.id); // Should be x1
        EXPECT_FALSE(cmd.has_preset);

        ret = mc_sys_get_alias(&sys, 4, &cmd);
        EXPECT_EQ(MC_OK, ret);
        EXPECT_STREQ("z", cmd.alias);
        EXPECT_EQ(MC_CMD_TYPE_OUTPUT, cmd.type);
        EXPECT_EQ(2, cmd.id); // Should be y2
        EXPECT_FALSE(cmd.has_preset);

        ret = mc_sys_get_alias(&sys, 5, &cmd);
        EXPECT_EQ(MC_OK, ret);
        EXPECT_STREQ("targetZ", cmd.alias);
        EXPECT_EQ(MC_CMD_TYPE_INPUT, cmd.type);
        EXPECT_EQ(2, cmd.id); // Should be x2
        EXPECT_FALSE(cmd.has_preset);

        // Invalid ID should fail
        ret = mc_sys_get_alias(&sys, 6, &cmd);
        EXPECT_EQ(MC_ERROR_INVALID_ARGS, ret);
    }

    TEST_F(AnalogVector3SystemTest, GetAliasWithCustomSucceeds)
    {
        mc_sys_cmd_info_t cmd;
        mc_status_t ret;

        ret = mc_sys_get_alias(&custom_sys, 0, &cmd);
        EXPECT_EQ(MC_OK, ret);
        EXPECT_STREQ("myX", cmd.alias);

        ret = mc_sys_get_alias(&custom_sys, 1, &cmd);
        EXPECT_EQ(MC_OK, ret);
        EXPECT_STREQ("myTargetX", cmd.alias);

        ret = mc_sys_get_alias(&custom_sys, 2, &cmd);
        EXPECT_EQ(MC_OK, ret);
        EXPECT_STREQ("myY", cmd.alias);

        ret = mc_sys_get_alias(&custom_sys, 3, &cmd);
        EXPECT_EQ(MC_OK, ret);
        EXPECT_STREQ("myTargetY", cmd.alias);

        ret = mc_sys_get_alias(&custom_sys, 4, &cmd);
        EXPECT_EQ(MC_OK, ret);
        EXPECT_STREQ("myZ", cmd.alias);

        ret = mc_sys_get_alias(&custom_sys, 5, &cmd);
        EXPECT_EQ(MC_OK, ret);
        EXPECT_STREQ("myTargetZ", cmd.alias);
    }

    TEST_F(AnalogVector3SystemTest, GetMemberCountSucceeds)
    {
        EXPECT_EQ(0, mc_sys_get_function_count(&sys));
        EXPECT_EQ(3, mc_sys_get_input_count(&sys));
        EXPECT_EQ(3, mc_sys_get_output_count(&sys));
        EXPECT_EQ(6, mc_sys_get_alias_count(&sys));
    }

    TEST_F(AnalogVector3SystemTest, ReadOnlyWriteInputFails)
    {
        mc_analog_vector3_set_read_only(&dev, true);
        mc_status_t ret = mc_sys_write_input(&sys, 0, 1);
        EXPECT_EQ(MC_ERROR_INVALID_ARGS, ret);
    }

    TEST_F(AnalogVector3SystemTest, ReadOnlyReadInputFails)
    {
        mc_analog_vector3_set_read_only(&dev, true);
        mc_result_t res = mc_sys_read_input(&sys, 0);
        EXPECT_FALSE(res.ok);
        EXPECT_EQ(MC_ERROR_INVALID_ARGS, res.error);
    }

    TEST_F(AnalogVector3SystemTest, ReadOnlyReadOutputSucceeds)
    {
        mc_analog_vector3_set_read_only(&dev, true);
        dev_ctx.x = 122;
        mc_result_t res = mc_sys_read_output(&sys, 0);
        EXPECT_TRUE(res.ok);
        EXPECT_EQ(122, res.value);

        dev_ctx.y = 123;
        res = mc_sys_read_output(&sys, 1);
        EXPECT_TRUE(res.ok);
        EXPECT_EQ(123, res.value);

        dev_ctx.z = 124;
        res = mc_sys_read_output(&sys, 2);
        EXPECT_TRUE(res.ok);
        EXPECT_EQ(124, res.value);
    }

    TEST_F(AnalogVector3SystemTest, ReadOnlyInvokeFunctionFails)
    {
        mc_analog_vector3_set_read_only(&dev, true);
        mc_status_t ret = mc_sys_invoke(&sys, 0, NULL, 0);
        EXPECT_EQ(MC_ERROR_INVALID_ARGS, ret);
    }

    TEST_F(AnalogVector3SystemTest, ReadOnlyGetAliasSucceeds)
    {
        mc_analog_vector3_set_read_only(&dev, true);
        mc_sys_cmd_info_t cmd;
        mc_status_t ret = mc_sys_get_alias(&sys, 0, &cmd);
        EXPECT_EQ(MC_OK, ret);
        EXPECT_STREQ("x", cmd.alias);
        EXPECT_EQ(MC_CMD_TYPE_OUTPUT, cmd.type);
        EXPECT_EQ(0, cmd.id); // Should be y0
        EXPECT_FALSE(cmd.has_preset);

        ret = mc_sys_get_alias(&sys, 1, &cmd);
        EXPECT_EQ(MC_OK, ret);
        EXPECT_STREQ("y", cmd.alias);
        EXPECT_EQ(MC_CMD_TYPE_OUTPUT, cmd.type);
        EXPECT_EQ(1, cmd.id); // Should be y1
        EXPECT_FALSE(cmd.has_preset);

        ret = mc_sys_get_alias(&sys, 2, &cmd);
        EXPECT_EQ(MC_OK, ret);
        EXPECT_STREQ("z", cmd.alias);
        EXPECT_EQ(MC_CMD_TYPE_OUTPUT, cmd.type);
        EXPECT_EQ(2, cmd.id); // Should be y2
        EXPECT_FALSE(cmd.has_preset);

        // Invalid ID should fail
        ret = mc_sys_get_alias(&sys, 3, &cmd);
        EXPECT_EQ(MC_ERROR_INVALID_ARGS, ret);
    }

    TEST_F(AnalogVector3SystemTest, ReadOnlyGetAliasWithCustomSucceeds)
    {
        mc_analog_vector3_set_read_only(&dev, true);
        mc_sys_cmd_info_t cmd;
        mc_status_t ret = mc_sys_get_alias(&custom_sys, 0, &cmd);
        EXPECT_EQ(MC_OK, ret);
        EXPECT_STREQ("myX", cmd.alias);

        ret = mc_sys_get_alias(&custom_sys, 1, &cmd);
        EXPECT_EQ(MC_OK, ret);
        EXPECT_STREQ("myY", cmd.alias);

        ret = mc_sys_get_alias(&custom_sys, 2, &cmd);
        EXPECT_EQ(MC_OK, ret);
        EXPECT_STREQ("myZ", cmd.alias);
    }

    TEST_F(AnalogVector3SystemTest, ReadOnlyGetMemberCountSucceeds)
    {
        mc_analog_vector3_set_read_only(&dev, true);
        EXPECT_EQ(0, mc_sys_get_function_count(&sys));
        EXPECT_EQ(0, mc_sys_get_input_count(&sys));
        EXPECT_EQ(3, mc_sys_get_output_count(&sys));
        EXPECT_EQ(3, mc_sys_get_alias_count(&sys));
    }

}