#include <gtest/gtest.h>
#include "mc_test.h"

extern "C"
{
#include "mc/system/core.h"
#include "fake_system.h"
}

namespace
{
    // Globals
    fake_sys_ctx_t ctx;

    class FakeSystemTest : public MeeCoreTest
    {
    protected:
        void SetUp() override
        {
            MeeCoreTest::SetUp();
            fake_sys_driver.init(&ctx);
        }
    };

    TEST_F(FakeSystemTest, InitResetsContext)
    {
        // Artificially set data in x
        ctx.x[0] = 5;
        fake_sys_driver.init(&ctx);

        // Should reset to 0
        EXPECT_EQ(0, ctx.x[0]);
    }

    TEST_F(FakeSystemTest, WriteInputSucceeds)
    {
        mc_status_t ret = fake_sys_driver.write_input(&ctx, 0, 3);
        EXPECT_EQ(MC_OK, ret);
        EXPECT_EQ(3, ctx.x[0]);

        ret = fake_sys_driver.write_input(&ctx, 1, 8);
        EXPECT_EQ(MC_OK, ret);
        EXPECT_EQ(3, ctx.x[0]);
        EXPECT_EQ(8, ctx.x[1]);
    }

    TEST_F(FakeSystemTest, ReadInputSucceeds)
    {
        ctx.x[0] = 4;
        ctx.x[1] = 7;
        mc_result_t res = fake_sys_driver.read_input(&ctx, 0);
        EXPECT_TRUE(res.ok);
        EXPECT_EQ(4, res.value);

        res = fake_sys_driver.read_input(&ctx, 1);
        EXPECT_TRUE(res.ok);
        EXPECT_EQ(7, res.value);
    }

    TEST_F(FakeSystemTest, ReadOutputSucceeds)
    {
        ctx.y[0] = 5;
        mc_result_t res = fake_sys_driver.read_output(&ctx, 0);
        EXPECT_TRUE(res.ok);
        EXPECT_EQ(5, res.value);
    }

    TEST_F(FakeSystemTest, InvokeSucceeds)
    {
        // Assert that y0 is 0
        EXPECT_EQ(0, ctx.y[0]);

        mc_status_t ret = fake_sys_driver.invoke(&ctx, 0, NULL, 0);
        EXPECT_EQ(MC_OK, ret);

        // New value should be 1
        EXPECT_EQ(1, ctx.y[0]);
    }

    TEST_F(FakeSystemTest, ErrorGetsPropogated)
    {
        // Invoking f1 should trigger errors
        int32_t args[1];
        args[0] = MC_ERROR_BUSY;
        mc_status_t ret = fake_sys_driver.invoke(&ctx, 1, args, 1);
        EXPECT_EQ(MC_ERROR_BUSY, ret);
    }

    TEST_F(FakeSystemTest, GetCountsSucceeds)
    {
        EXPECT_EQ(2, fake_sys_driver.get_function_count(&ctx));
        EXPECT_EQ(2, fake_sys_driver.get_input_count(&ctx));
        EXPECT_EQ(1, fake_sys_driver.get_output_count(&ctx));
        EXPECT_EQ(4, fake_sys_driver.get_alias_count(&ctx));
    }

    TEST_F(FakeSystemTest, GetAliasSucceeds)
    {
        mc_sys_cmd_info_t cmd;
        mc_status_t ret;

        ret = fake_sys_driver.get_alias(&ctx, 0, &cmd);
        EXPECT_EQ(MC_OK, ret);
        EXPECT_STREQ(DEFAULT_FAKE_INCREMENT_Y_NAME, cmd.alias);
        EXPECT_EQ(MC_CMD_TYPE_FUNC, cmd.type);
        EXPECT_EQ(0, cmd.id); // Should be f0
        EXPECT_FALSE(cmd.has_preset);

        ret = fake_sys_driver.get_alias(&ctx, 1, &cmd);
        EXPECT_EQ(MC_OK, ret);
        EXPECT_STREQ(DEFAULT_FAKE_X0_NAME, cmd.alias);
        EXPECT_EQ(MC_CMD_TYPE_INPUT, cmd.type);
        EXPECT_EQ(0, cmd.id); // Should be x0
        EXPECT_FALSE(cmd.has_preset);

        ret = fake_sys_driver.get_alias(&ctx, 2, &cmd);
        EXPECT_EQ(MC_OK, ret);
        EXPECT_STREQ(DEFAULT_FAKE_Y0_NAME, cmd.alias);
        EXPECT_EQ(MC_CMD_TYPE_OUTPUT, cmd.type);
        EXPECT_EQ(0, cmd.id); // Should be y0
        EXPECT_FALSE(cmd.has_preset);

        ret = fake_sys_driver.get_alias(&ctx, 3, &cmd);
        EXPECT_EQ(MC_OK, ret);
        EXPECT_STREQ(DEFAULT_FAKE_RESET_NAME, cmd.alias);
        EXPECT_EQ(MC_CMD_TYPE_INPUT, cmd.type);
        EXPECT_EQ(0, cmd.id); // Should be x0
        EXPECT_TRUE(cmd.has_preset);
        EXPECT_EQ(0, cmd.preset_val);

        // Invalid ID should fail
        ret = fake_sys_driver.get_alias(&ctx, 4, &cmd);
        EXPECT_EQ(MC_ERROR_INVALID_ARGS, ret);
    }

    TEST_F(FakeSystemTest, GetAliasWithCustomNameSucceeds)
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
        EXPECT_EQ(MC_OK, ret);
        EXPECT_STREQ(new_increment_y_name, cmd.alias);
        EXPECT_EQ(MC_CMD_TYPE_FUNC, cmd.type);
        EXPECT_EQ(0, cmd.id); // Should be f0
        EXPECT_FALSE(cmd.has_preset);

        ret = fake_sys_driver.get_alias(&ctx, 1, &cmd);
        EXPECT_EQ(MC_OK, ret);
        EXPECT_STREQ(new_x0_name, cmd.alias);
        EXPECT_EQ(MC_CMD_TYPE_INPUT, cmd.type);
        EXPECT_EQ(0, cmd.id); // Should be x0
        EXPECT_FALSE(cmd.has_preset);

        ret = fake_sys_driver.get_alias(&ctx, 2, &cmd);
        EXPECT_EQ(MC_OK, ret);
        EXPECT_STREQ(new_y0_name, cmd.alias);
        EXPECT_EQ(MC_CMD_TYPE_OUTPUT, cmd.type);
        EXPECT_EQ(0, cmd.id); // Should be y0
        EXPECT_FALSE(cmd.has_preset);

        ret = fake_sys_driver.get_alias(&ctx, 3, &cmd);
        EXPECT_EQ(MC_OK, ret);
        EXPECT_STREQ(new_reset_name, cmd.alias);
        EXPECT_EQ(MC_CMD_TYPE_INPUT, cmd.type);
        EXPECT_EQ(0, cmd.id); // Should be x0
        EXPECT_TRUE(cmd.has_preset);
        EXPECT_EQ(0, cmd.preset_val);
    }
}