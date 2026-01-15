#include "mc_test.h"

extern "C"
{
#include "mc/system/core.h"
#include "mc/system/composite.h"
#include "fake_system.h"
#include "fake_composite.h"
}

namespace
{

    // Globals
    fake_composite_ctx_t ctx;
    MC_DEFINE_SYSTEM(sys, fake_composite_sys_driver, ctx);

    class CompositeTest : public MeeCoreTest
    {
    protected:
        void SetUp() override
        {
            MeeCoreTest::SetUp();
            mc_sys_init(&sys);
        }
    };

    TEST_F(CompositeTest, InitResetsAllContexts)
    {
        // Artificially set data in x
        ctx.sys1.x[0] = 5;
        ctx.sys2.x[0] = 5;
        mc_sys_init(&sys);

        // Should reset to 0
        EXPECT_EQ(0, ctx.sys1.x[0]);
        EXPECT_EQ(0, ctx.sys2.x[0]);
    }

    TEST_F(CompositeTest, WriteInputSucceeds)
    {
        mc_status_t ret = mc_sys_write_input(&sys, 0, 3);
        EXPECT_EQ(MC_OK, ret);
        EXPECT_EQ(3, ctx.sys1.x[0]);

        ret = mc_sys_write_input(&sys, 3, 8);
        EXPECT_EQ(MC_OK, ret);
        EXPECT_EQ(3, ctx.sys1.x[0]);
        EXPECT_EQ(8, ctx.sys2.x[1]);
    }

    TEST_F(CompositeTest, ReadInputSucceeds)
    {
        ctx.sys1.x[0] = 4;
        ctx.sys2.x[0] = 7;
        mc_result_t res = mc_sys_read_input(&sys, 0);
        EXPECT_TRUE(res.ok);
        EXPECT_EQ(4, res.value);

        res = mc_sys_read_input(&sys, 2);
        EXPECT_TRUE(res.ok);
        EXPECT_EQ(7, res.value);
    }

    TEST_F(CompositeTest, ReadOutputSucceeds)
    {
        ctx.sys1.y[0] = 5;
        ctx.sys2.y[0] = 2;
        mc_result_t res = mc_sys_read_output(&sys, 0);
        EXPECT_TRUE(res.ok);
        EXPECT_EQ(5, res.value);

        res = mc_sys_read_output(&sys, 1);
        EXPECT_TRUE(res.ok);
        EXPECT_EQ(2, res.value);
    }

    TEST_F(CompositeTest, InvokeSucceeds)
    {
        // Assert that y0 is 0
        EXPECT_EQ(0, ctx.sys1.y[0]);
        EXPECT_EQ(0, ctx.sys2.y[0]);

        mc_status_t ret = mc_sys_invoke(&sys, 0, NULL, 0);
        EXPECT_EQ(MC_OK, ret);
        EXPECT_EQ(1, ctx.sys1.y[0]);

        ret = mc_sys_invoke(&sys, 2, NULL, 0);
        EXPECT_EQ(MC_OK, ret);
        EXPECT_EQ(1, ctx.sys2.y[0]);
    }

    TEST_F(CompositeTest, GetCountsSucceeds)
    {
        EXPECT_EQ(4, mc_sys_get_function_count(&sys));
        EXPECT_EQ(4, mc_sys_get_input_count(&sys));
        EXPECT_EQ(2, mc_sys_get_output_count(&sys));
        EXPECT_EQ(8, mc_sys_get_alias_count(&sys));
    }

    TEST_F(CompositeTest, GetAliasSucceeds)
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
        EXPECT_EQ(MC_OK, ret);
        EXPECT_STREQ("incrementY0", cmd.alias);
        EXPECT_EQ(MC_CMD_TYPE_FUNC, cmd.type);
        EXPECT_EQ(0, cmd.id); // Should be f0
        EXPECT_FALSE(cmd.has_preset);

        ret = mc_sys_get_alias(&sys, 1, &cmd);
        EXPECT_EQ(MC_OK, ret);
        EXPECT_STREQ("input0", cmd.alias);
        EXPECT_EQ(MC_CMD_TYPE_INPUT, cmd.type);
        EXPECT_EQ(0, cmd.id); // Should be x0
        EXPECT_FALSE(cmd.has_preset);

        ret = mc_sys_get_alias(&sys, 2, &cmd);
        EXPECT_EQ(MC_OK, ret);
        EXPECT_STREQ("output0", cmd.alias);
        EXPECT_EQ(MC_CMD_TYPE_OUTPUT, cmd.type);
        EXPECT_EQ(0, cmd.id); // Should be y0
        EXPECT_FALSE(cmd.has_preset);

        ret = mc_sys_get_alias(&sys, 3, &cmd);
        EXPECT_EQ(MC_OK, ret);
        EXPECT_STREQ("reset0", cmd.alias);
        EXPECT_EQ(MC_CMD_TYPE_INPUT, cmd.type);
        EXPECT_EQ(0, cmd.id); // Should be x0
        EXPECT_TRUE(cmd.has_preset);
        EXPECT_EQ(0, cmd.preset_val);

        ret = mc_sys_get_alias(&sys, 4, &cmd);
        EXPECT_EQ(MC_OK, ret);
        EXPECT_STREQ("incrementY1", cmd.alias);
        EXPECT_EQ(MC_CMD_TYPE_FUNC, cmd.type);
        EXPECT_EQ(2, cmd.id); // Should be f2
        EXPECT_FALSE(cmd.has_preset);

        ret = mc_sys_get_alias(&sys, 5, &cmd);
        EXPECT_EQ(MC_OK, ret);
        EXPECT_STREQ("input1", cmd.alias);
        EXPECT_EQ(MC_CMD_TYPE_INPUT, cmd.type);
        EXPECT_EQ(2, cmd.id); // Should be x2
        EXPECT_FALSE(cmd.has_preset);

        ret = mc_sys_get_alias(&sys, 6, &cmd);
        EXPECT_EQ(MC_OK, ret);
        EXPECT_STREQ("output1", cmd.alias);
        EXPECT_EQ(MC_CMD_TYPE_OUTPUT, cmd.type);
        EXPECT_EQ(1, cmd.id); // Should be y1
        EXPECT_FALSE(cmd.has_preset);

        ret = mc_sys_get_alias(&sys, 7, &cmd);
        EXPECT_EQ(MC_OK, ret);
        EXPECT_STREQ("reset1", cmd.alias);
        EXPECT_EQ(MC_CMD_TYPE_INPUT, cmd.type);
        EXPECT_EQ(2, cmd.id); // Should be x2
        EXPECT_TRUE(cmd.has_preset);
        EXPECT_EQ(0, cmd.preset_val);

        // Invalid ID should fail
        ret = mc_sys_get_alias(&sys, 8, &cmd);
        EXPECT_EQ(MC_ERROR_INVALID_ARGS, ret);
    }

}