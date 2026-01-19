#include <gtest/gtest.h>
#include "mc_test.h"

extern "C"
{
#include "mc/system/core.h"
#include "mc/system/modules/digital_system.h"
#include "mc/device/digital.h"
}

namespace
{
    // Globals
    MC_DEFINE_DIGITAL_DATA_OBJECT(dev);
    MC_DEFINE_DIGITAL_SYSTEM(sys, dev);
    mc_digital_system_ctx_t *ctx;

    mc_digital_system_config_t custom_config = {
        .state_name = "myState",
        .target_state_name = "myTargetState",
        .turn_on_name = "myTurnOn",
        .turn_off_name = "myTurnOff",
        .toggle_name = "myToggle",
    };
    MC_DEFINE_DIGITAL_SYSTEM_WITH_CONFIG(custom_sys, custom_config, dev);

    class DigitalSystemTest : public MeeCoreTest
    {
    protected:
        void SetUp() override
        {
            MeeCoreTest::SetUp();

            // Cast the context pointer from the system struct for easy access in tests
            ctx = (mc_digital_system_ctx_t *)sys.ctx;

            mc_digital_init(&dev, /*is_read_only=*/false);
            mc_sys_init(&sys);
            mc_sys_init(&custom_sys);
        }
    };

    TEST_F(DigitalSystemTest, InitSetsTargetStateToCurrentState)
    {
        EXPECT_FALSE(ctx->target_state);
        dev_ctx.state = true;
        mc_sys_init(&sys);
        EXPECT_TRUE(ctx->target_state);
    }

    TEST_F(DigitalSystemTest, WriteInputSetsState)
    {
        // x0 = 1
        mc_status_t ret = mc_sys_write_input(&sys, 0, 1);
        EXPECT_EQ(MC_OK, ret);
        EXPECT_TRUE(ctx->target_state);
        EXPECT_TRUE(dev_ctx.state);

        // x0 = 0
        ret = mc_sys_write_input(&sys, 0, 0);
        EXPECT_EQ(MC_OK, ret);
        EXPECT_FALSE(ctx->target_state);
        EXPECT_FALSE(dev_ctx.state);
    }

    TEST_F(DigitalSystemTest, ReadInputGetsTargetState)
    {
        // Set conflicting target state and actual state
        ctx->target_state = true;
        dev_ctx.state = false;
        mc_result_t res = mc_sys_read_input(&sys, 0);
        EXPECT_TRUE(res.ok);
        EXPECT_TRUE(res.value);

        ctx->target_state = false;
        dev_ctx.state = true;
        res = mc_sys_read_input(&sys, 0);
        EXPECT_TRUE(res.ok);
        EXPECT_FALSE(res.value);
    }

    TEST_F(DigitalSystemTest, ReadOutputGetsActualState)
    {
        // Set conflicting target state and actual state
        ctx->target_state = true;
        dev_ctx.state = false;
        mc_result_t res = mc_sys_read_output(&sys, 0);
        EXPECT_TRUE(res.ok);
        EXPECT_FALSE(res.value);

        ctx->target_state = false;
        dev_ctx.state = true;
        res = mc_sys_read_output(&sys, 0);
        EXPECT_TRUE(res.ok);
        EXPECT_TRUE(res.value);
    }

    TEST_F(DigitalSystemTest, InvokeFunctionTogglesState)
    {
        ctx->target_state = false;
        dev_ctx.state = false;
        mc_status_t ret = mc_sys_invoke(&sys, 0, NULL, 0);
        EXPECT_EQ(MC_OK, ret);
        EXPECT_TRUE(ctx->target_state);
        EXPECT_TRUE(dev_ctx.state);

        ret = mc_sys_invoke(&sys, 0, NULL, 0);
        EXPECT_EQ(MC_OK, ret);
        EXPECT_FALSE(ctx->target_state);
        EXPECT_FALSE(dev_ctx.state);

        // If, for some reason, target and actual state are not in sync, toggle
        // should force them in sync.
        ctx->target_state = false;
        dev_ctx.state = true;
        ret = mc_sys_invoke(&sys, 0, NULL, 0);
        EXPECT_EQ(MC_OK, ret);
        EXPECT_FALSE(ctx->target_state);
        EXPECT_FALSE(dev_ctx.state);

        ctx->target_state = true;
        dev_ctx.state = false;
        ret = mc_sys_invoke(&sys, 0, NULL, 0);
        EXPECT_EQ(MC_OK, ret);
        EXPECT_TRUE(ctx->target_state);
        EXPECT_TRUE(dev_ctx.state);
    }

    TEST_F(DigitalSystemTest, GetAliasSucceeds)
    {
        mc_sys_cmd_info_t cmd;
        mc_status_t ret;

        ret = mc_sys_get_alias(&sys, 0, &cmd);
        EXPECT_EQ(MC_OK, ret);
        EXPECT_STREQ("state", cmd.alias);
        EXPECT_EQ(MC_CMD_TYPE_OUTPUT, cmd.type);
        EXPECT_EQ(0, cmd.id); // Should be y0
        EXPECT_FALSE(cmd.has_preset);

        ret = mc_sys_get_alias(&sys, 1, &cmd);
        EXPECT_EQ(MC_OK, ret);
        EXPECT_STREQ("targetState", cmd.alias);
        EXPECT_EQ(MC_CMD_TYPE_INPUT, cmd.type);
        EXPECT_EQ(0, cmd.id); // Should be x0
        EXPECT_FALSE(cmd.has_preset);

        ret = mc_sys_get_alias(&sys, 2, &cmd);
        EXPECT_EQ(MC_OK, ret);
        EXPECT_STREQ("turnOn", cmd.alias);
        EXPECT_EQ(MC_CMD_TYPE_INPUT, cmd.type);
        EXPECT_EQ(0, cmd.id); // Should be x0
        EXPECT_TRUE(cmd.has_preset);
        EXPECT_EQ(1, cmd.preset_val);

        ret = mc_sys_get_alias(&sys, 3, &cmd);
        EXPECT_EQ(MC_OK, ret);
        EXPECT_STREQ("turnOff", cmd.alias);
        EXPECT_EQ(MC_CMD_TYPE_INPUT, cmd.type);
        EXPECT_EQ(0, cmd.id); // Should be x0
        EXPECT_TRUE(cmd.has_preset);
        EXPECT_EQ(0, cmd.preset_val);

        ret = mc_sys_get_alias(&sys, 4, &cmd);
        EXPECT_EQ(MC_OK, ret);
        EXPECT_STREQ("toggle", cmd.alias);
        EXPECT_EQ(MC_CMD_TYPE_FUNC, cmd.type);
        EXPECT_EQ(0, cmd.id); // Should be f0
        EXPECT_FALSE(cmd.has_preset);

        // Invalid ID should fail
        ret = mc_sys_get_alias(&sys, 5, &cmd);
        EXPECT_EQ(MC_ERROR_INVALID_ARGS, ret);
    }

    TEST_F(DigitalSystemTest, GetAliasWithCustomSucceeds)
    {
        mc_sys_cmd_info_t cmd;
        mc_status_t ret;

        ret = mc_sys_get_alias(&custom_sys, 0, &cmd);
        EXPECT_EQ(MC_OK, ret);
        EXPECT_STREQ("myState", cmd.alias);

        ret = mc_sys_get_alias(&custom_sys, 1, &cmd);
        EXPECT_EQ(MC_OK, ret);
        EXPECT_STREQ("myTargetState", cmd.alias);

        ret = mc_sys_get_alias(&custom_sys, 2, &cmd);
        EXPECT_EQ(MC_OK, ret);
        EXPECT_STREQ("myTurnOn", cmd.alias);

        ret = mc_sys_get_alias(&custom_sys, 3, &cmd);
        EXPECT_EQ(MC_OK, ret);
        EXPECT_STREQ("myTurnOff", cmd.alias);

        ret = mc_sys_get_alias(&custom_sys, 4, &cmd);
        EXPECT_EQ(MC_OK, ret);
        EXPECT_STREQ("myToggle", cmd.alias);
    }

    TEST_F(DigitalSystemTest, GetMemberCountSucceeds)
    {
        EXPECT_EQ(1, mc_sys_get_function_count(&sys));
        EXPECT_EQ(1, mc_sys_get_input_count(&sys));
        EXPECT_EQ(1, mc_sys_get_output_count(&sys));
        EXPECT_EQ(5, mc_sys_get_alias_count(&sys));
    }

    TEST_F(DigitalSystemTest, ReadOnlyWriteInputFails)
    {
        mc_digital_set_read_only(&dev, true);
        mc_status_t ret = mc_sys_write_input(&sys, 0, 1);
        EXPECT_EQ(MC_ERROR_INVALID_ARGS, ret);
    }

    TEST_F(DigitalSystemTest, ReadOnlyReadInputFails)
    {
        mc_digital_set_read_only(&dev, true);
        mc_result_t res = mc_sys_read_input(&sys, 0);
        EXPECT_FALSE(res.ok);
        EXPECT_EQ(MC_ERROR_INVALID_ARGS, res.error);
    }

    TEST_F(DigitalSystemTest, ReadOnlyReadOutputSucceeds)
    {
        mc_digital_set_read_only(&dev, true);
        dev_ctx.state = false;
        mc_result_t res = mc_sys_read_output(&sys, 0);
        EXPECT_TRUE(res.ok);
        EXPECT_FALSE(res.value);
    }

    TEST_F(DigitalSystemTest, ReadOnlyInvokeFunctionFails)
    {
        mc_digital_set_read_only(&dev, true);
        mc_status_t ret = mc_sys_invoke(&sys, 0, NULL, 0);
        EXPECT_EQ(MC_ERROR_INVALID_ARGS, ret);
    }

    TEST_F(DigitalSystemTest, ReadOnlyGetAliasSucceeds)
    {
        mc_digital_set_read_only(&dev, true);
        mc_sys_cmd_info_t cmd;
        mc_status_t ret = mc_sys_get_alias(&sys, 0, &cmd);
        EXPECT_EQ(MC_OK, ret);
        EXPECT_STREQ("state", cmd.alias);
        EXPECT_EQ(MC_CMD_TYPE_OUTPUT, cmd.type);
        EXPECT_EQ(0, cmd.id); // Should be y0
        EXPECT_FALSE(cmd.has_preset);

        // Invalid ID should fail
        ret = mc_sys_get_alias(&sys, 1, &cmd);
        EXPECT_EQ(MC_ERROR_INVALID_ARGS, ret);
    }

    TEST_F(DigitalSystemTest, ReadOnlyGetAliasWithCustomSucceeds)
    {
        mc_digital_set_read_only(&dev, true);
        mc_sys_cmd_info_t cmd;
        mc_status_t ret = mc_sys_get_alias(&custom_sys, 0, &cmd);
        EXPECT_EQ(MC_OK, ret);
        EXPECT_STREQ("myState", cmd.alias);
    }

    TEST_F(DigitalSystemTest, ReadOnlyGetMemberCountSucceeds)
    {
        mc_digital_set_read_only(&dev, true);
        EXPECT_EQ(0, mc_sys_get_function_count(&sys));
        EXPECT_EQ(0, mc_sys_get_input_count(&sys));
        EXPECT_EQ(1, mc_sys_get_output_count(&sys));
        EXPECT_EQ(1, mc_sys_get_alias_count(&sys));
    }

}