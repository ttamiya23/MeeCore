#include <gtest/gtest.h>
#include "mc_test.h"

extern "C"
{
#include "mc/system/core.h"
#include "mc/system/modules/analog_system.h"
#include "mc/device/analog.h"
}

namespace
{

    // Globals
    MC_DEFINE_ANALOG_DATA_OBJECT(dev);
    MC_DEFINE_ANALOG_SYSTEM(sys, dev);
    mc_analog_system_ctx_t *ctx;

    mc_analog_system_config_t custom_config = {
        .value_name = "myValue",
        .target_value_name = "myTargetValue"};
    MC_DEFINE_ANALOG_SYSTEM_WITH_CONFIG(custom_sys, custom_config, dev);

    class AnalogSystemTest : public MeeCoreTest
    {
    protected:
        void SetUp() override
        {
            MeeCoreTest::SetUp();

            // Cast the context pointer from the system struct for easy access in tests
            ctx = (mc_analog_system_ctx_t *)sys.ctx;

            mc_analog_init(&dev, /*is_read_only=*/false);
            mc_sys_init(&sys);
            mc_sys_init(&custom_sys);
        }
    };

    TEST_F(AnalogSystemTest, InitSetsTargetValueToCurrentValue)
    {
        dev_ctx.value = 14243;
        mc_sys_init(&sys);
        EXPECT_EQ(14243, ctx->target_value);
    }

    TEST_F(AnalogSystemTest, WriteInputSetsValue)
    {
        // x0 = 1
        mc_status_t ret = mc_sys_write_input(&sys, 0, 1);
        EXPECT_EQ(MC_OK, ret);
        EXPECT_EQ(1, ctx->target_value);
        EXPECT_EQ(1, dev_ctx.value);
    }

    TEST_F(AnalogSystemTest, ReadInputGetsTargetValue)
    {
        // Set conflicting target value and actual value
        ctx->target_value = 2495;
        dev_ctx.value = -1323;
        mc_result_t res = mc_sys_read_input(&sys, 0);
        EXPECT_TRUE(res.ok);
        EXPECT_EQ(2495, res.value);
    }

    TEST_F(AnalogSystemTest, ReadOutputGetsActualValue)
    {
        // Set conflicting target state and actual state
        ctx->target_value = 2495;
        dev_ctx.value = -1323;
        mc_result_t res = mc_sys_read_output(&sys, 0);
        EXPECT_TRUE(res.ok);
        EXPECT_EQ(-1323, res.value);
    }

    TEST_F(AnalogSystemTest, InvokeFunctionFails)
    {
        mc_status_t ret = mc_sys_invoke(&sys, 0, NULL, 0);
        EXPECT_EQ(MC_ERROR_NOT_SUPPORTED, ret);
    }

    TEST_F(AnalogSystemTest, GetAliasSucceeds)
    {
        mc_sys_cmd_info_t cmd;
        mc_status_t ret;

        ret = mc_sys_get_alias(&sys, 0, &cmd);
        EXPECT_EQ(MC_OK, ret);
        EXPECT_STREQ("value", cmd.alias);
        EXPECT_EQ(MC_CMD_TYPE_OUTPUT, cmd.type);
        EXPECT_EQ(0, cmd.id); // Should be y0
        EXPECT_FALSE(cmd.has_preset);

        ret = mc_sys_get_alias(&sys, 1, &cmd);
        EXPECT_EQ(MC_OK, ret);
        EXPECT_STREQ("targetValue", cmd.alias);
        EXPECT_EQ(MC_CMD_TYPE_INPUT, cmd.type);
        EXPECT_EQ(0, cmd.id); // Should be x0
        EXPECT_FALSE(cmd.has_preset);

        // Invalid ID should fail
        ret = mc_sys_get_alias(&sys, 2, &cmd);
        EXPECT_EQ(MC_ERROR_INVALID_ARGS, ret);
    }

    TEST_F(AnalogSystemTest, GetAliasWithCustomSucceeds)
    {
        mc_sys_cmd_info_t cmd;
        mc_status_t ret;

        ret = mc_sys_get_alias(&custom_sys, 0, &cmd);
        EXPECT_EQ(MC_OK, ret);
        EXPECT_STREQ("myValue", cmd.alias);

        ret = mc_sys_get_alias(&custom_sys, 1, &cmd);
        EXPECT_EQ(MC_OK, ret);
        EXPECT_STREQ("myTargetValue", cmd.alias);
    }

    TEST_F(AnalogSystemTest, GetMemberCountSucceeds)
    {
        EXPECT_EQ(0, mc_sys_get_function_count(&sys));
        EXPECT_EQ(1, mc_sys_get_input_count(&sys));
        EXPECT_EQ(1, mc_sys_get_output_count(&sys));
        EXPECT_EQ(2, mc_sys_get_alias_count(&sys));
    }

    TEST_F(AnalogSystemTest, ReadOnlyWriteInputFails)
    {
        mc_analog_set_read_only(&dev, true);
        mc_status_t ret = mc_sys_write_input(&sys, 0, 1);
        EXPECT_EQ(MC_ERROR_INVALID_ARGS, ret);
    }

    TEST_F(AnalogSystemTest, ReadOnlyReadInputFails)
    {
        mc_analog_set_read_only(&dev, true);
        mc_result_t res = mc_sys_read_input(&sys, 0);
        EXPECT_FALSE(res.ok);
        EXPECT_EQ(MC_ERROR_INVALID_ARGS, res.error);
    }

    TEST_F(AnalogSystemTest, ReadOnlyReadOutputSucceeds)
    {
        mc_analog_set_read_only(&dev, true);
        dev_ctx.value = 122;
        mc_result_t res = mc_sys_read_output(&sys, 0);
        EXPECT_TRUE(res.ok);
        EXPECT_EQ(122, res.value);
    }

    TEST_F(AnalogSystemTest, ReadOnlyInvokeFunctionFails)
    {
        mc_analog_set_read_only(&dev, true);
        mc_status_t ret = mc_sys_invoke(&sys, 0, NULL, 0);
        EXPECT_EQ(MC_ERROR_NOT_SUPPORTED, ret);
    }

    TEST_F(AnalogSystemTest, ReadOnlyGetAliasSucceeds)
    {
        mc_analog_set_read_only(&dev, true);
        mc_sys_cmd_info_t cmd;
        mc_status_t ret = mc_sys_get_alias(&sys, 0, &cmd);
        EXPECT_EQ(MC_OK, ret);
        EXPECT_STREQ("value", cmd.alias);
        EXPECT_EQ(MC_CMD_TYPE_OUTPUT, cmd.type);
        EXPECT_EQ(0, cmd.id); // Should be y0
        EXPECT_FALSE(cmd.has_preset);

        // Invalid ID should fail
        ret = mc_sys_get_alias(&sys, 1, &cmd);
        EXPECT_EQ(MC_ERROR_INVALID_ARGS, ret);
    }

    TEST_F(AnalogSystemTest, ReadOnlyGetAliasWithCustomSucceeds)
    {
        mc_analog_set_read_only(&dev, true);
        mc_sys_cmd_info_t cmd;
        mc_status_t ret = mc_sys_get_alias(&custom_sys, 0, &cmd);
        EXPECT_EQ(MC_OK, ret);
        EXPECT_STREQ("myValue", cmd.alias);
    }

    TEST_F(AnalogSystemTest, ReadOnlyGetMemberCountSucceeds)
    {
        mc_analog_set_read_only(&dev, true);
        EXPECT_EQ(0, mc_sys_get_function_count(&sys));
        EXPECT_EQ(0, mc_sys_get_input_count(&sys));
        EXPECT_EQ(1, mc_sys_get_output_count(&sys));
        EXPECT_EQ(1, mc_sys_get_alias_count(&sys));
    }

}