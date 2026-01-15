#include <gtest/gtest.h>
#include "mc_test.h"

extern "C"
{
#include "mc/system/core.h"
#include "fakes/system/fake_system.h"
}

namespace
{

    // Globals
    fake_sys_ctx_t ctx;
    MC_DEFINE_SYSTEM(sys, fake_sys_driver, ctx);

    class SystemTest : public MeeCoreTest
    {
    protected:
        void SetUp() override
        {
            MeeCoreTest::SetUp();
            mc_sys_init(&sys);
        }
    };

    TEST_F(SystemTest, InitResetsContext)
    {
        // Artificially set data in x
        ctx.x[0] = 5;
        mc_sys_init(&sys);

        // Should reset to 0
        EXPECT_EQ(0, ctx.x[0]);
    }

    TEST_F(SystemTest, WriteInputSucceeds)
    {
        mc_status_t ret = mc_sys_write_input(&sys, 0, 3);
        EXPECT_EQ(MC_OK, ret);
        EXPECT_EQ(3, ctx.x[0]);

        ret = mc_sys_write_input(&sys, 1, 8);
        EXPECT_EQ(MC_OK, ret);
        EXPECT_EQ(3, ctx.x[0]);
        EXPECT_EQ(8, ctx.x[1]);
    }

    TEST_F(SystemTest, ReadInputSucceeds)
    {
        ctx.x[0] = 4;
        ctx.x[1] = 7;
        mc_result_t res = mc_sys_read_input(&sys, 0);
        EXPECT_TRUE(res.ok);
        EXPECT_EQ(4, res.value);

        res = mc_sys_read_input(&sys, 1);
        EXPECT_TRUE(res.ok);
        EXPECT_EQ(7, res.value);
    }

    TEST_F(SystemTest, ReadOutputSucceeds)
    {
        ctx.y[0] = 5;
        mc_result_t res = mc_sys_read_output(&sys, 0);
        EXPECT_TRUE(res.ok);
        EXPECT_EQ(5, res.value);
    }

    TEST_F(SystemTest, InvokeSucceeds)
    {
        // Assert that y0 is 0
        EXPECT_EQ(0, ctx.y[0]);

        mc_status_t ret = mc_sys_invoke(&sys, 0, NULL, 0);
        EXPECT_EQ(MC_OK, ret);

        // New value should be 1
        EXPECT_EQ(1, ctx.y[0]);
    }

    TEST_F(SystemTest, GetAliasSucceeds)
    {
        mc_sys_cmd_info_t cmd;
        mc_status_t ret = mc_sys_get_alias(&sys, 0, &cmd);
        EXPECT_EQ(MC_OK, ret);
        EXPECT_STREQ("incrementY", cmd.alias);
        EXPECT_EQ(MC_CMD_TYPE_FUNC, cmd.type);
        EXPECT_EQ(0, cmd.id); // Should be f0
        EXPECT_FALSE(cmd.has_preset);
    }

    TEST_F(SystemTest, ErrorGetsPropogated)
    {
        // Invoking f1 should trigger errors
        int32_t args[1];
        args[0] = MC_ERROR_BUSY;
        mc_status_t ret = mc_sys_invoke(&sys, 1, args, 1);
        EXPECT_EQ(MC_ERROR_BUSY, ret);
    }

    TEST_F(SystemTest, InvalidIdReturnsError)
    {
        mc_status_t ret = mc_sys_write_input(&sys, 3, 3);
        EXPECT_EQ(MC_ERROR_INVALID_ARGS, ret);

        mc_result_t res = mc_sys_read_input(&sys, 7);
        EXPECT_FALSE(res.ok);
        EXPECT_EQ(MC_ERROR_INVALID_ARGS, res.error);

        res = mc_sys_read_output(&sys, 2);
        EXPECT_FALSE(res.ok);
        EXPECT_EQ(MC_ERROR_INVALID_ARGS, res.error);

        ret = mc_sys_invoke(&sys, 7, NULL, 0);
        EXPECT_EQ(MC_ERROR_INVALID_ARGS, ret);

        mc_sys_cmd_info_t cmd;
        ret = mc_sys_get_alias(&sys, 9, &cmd);
        EXPECT_EQ(MC_ERROR_INVALID_ARGS, ret);
    }

    TEST_F(SystemTest, GetCountsSucceeds)
    {
        EXPECT_EQ(2, mc_sys_get_function_count(&sys));
        EXPECT_EQ(2, mc_sys_get_input_count(&sys));
        EXPECT_EQ(1, mc_sys_get_output_count(&sys));
        EXPECT_EQ(4, mc_sys_get_alias_count(&sys));
    }

    TEST_F(SystemTest, UnimplementedMethodsReturnNotSupported)
    {
        mc_system_driver_t null_sys_driver = {
            .init = NULL,
            .invoke = NULL,
            .write_input = NULL,
            .read_input = NULL,
            .read_output = NULL,
            .get_alias = NULL,
            .get_function_count = NULL,
            .get_input_count = NULL,
            .get_output_count = NULL,
            .get_alias_count = NULL};
        mc_system_state_t state;
        mc_system_t null_sys = {
            .driver = &null_sys_driver,
            .ctx = NULL,
            .state = &state};

        mc_sys_init(&null_sys);

        mc_status_t ret = mc_sys_invoke(&null_sys, 0, NULL, 0);
        EXPECT_EQ(MC_ERROR_NOT_SUPPORTED, ret);
        ret = mc_sys_write_input(&null_sys, 0, 0);
        EXPECT_EQ(MC_ERROR_NOT_SUPPORTED, ret);
        mc_result_t res = mc_sys_read_input(&null_sys, 0);
        EXPECT_FALSE(res.ok);
        EXPECT_EQ(MC_ERROR_NOT_SUPPORTED, res.error);
        res = mc_sys_read_output(&null_sys, 0);
        EXPECT_FALSE(res.ok);
        EXPECT_EQ(MC_ERROR_NOT_SUPPORTED, res.error);
        mc_sys_cmd_info_t cmd;
        ret = mc_sys_get_alias(&null_sys, 0, &cmd);
        EXPECT_EQ(MC_ERROR_NOT_SUPPORTED, ret);

        uint8_t val = mc_sys_get_function_count(&null_sys);
        EXPECT_EQ(0, val);
        val = mc_sys_get_input_count(&null_sys);
        EXPECT_EQ(0, val);
        val = mc_sys_get_output_count(&null_sys);
        EXPECT_EQ(0, val);
        val = mc_sys_get_alias_count(&null_sys);
        EXPECT_EQ(0, val);
    }

    TEST_F(SystemTest, AssertDeathIfNullPointer)
    {
        int32_t args[1];
        mc_sys_cmd_info_t cmd;
        EXPECT_ANY_THROW(mc_sys_invoke(NULL, 0, args, 1));
        EXPECT_ANY_THROW(mc_sys_write_input(NULL, 0, 1));
        EXPECT_ANY_THROW(mc_sys_read_input(NULL, 0));
        EXPECT_ANY_THROW(mc_sys_read_output(NULL, 0));
        EXPECT_ANY_THROW(mc_sys_get_alias(NULL, 0, &cmd));
        EXPECT_ANY_THROW(mc_sys_get_alias(&sys, 0, NULL));
        EXPECT_ANY_THROW(mc_sys_get_function_count(NULL));
        EXPECT_ANY_THROW(mc_sys_get_input_count(NULL));
        EXPECT_ANY_THROW(mc_sys_get_output_count(NULL));
        EXPECT_ANY_THROW(mc_sys_get_alias_count(NULL));
    }

    TEST_F(SystemTest, AssertDeathIfUninitialized)
    {
        // Force uninitialized state
        sys.state->is_initialized = 0;
        int32_t args[1];
        mc_sys_cmd_info_t cmd;

        EXPECT_ANY_THROW(mc_sys_invoke(&sys, 0, args, 1));
        EXPECT_ANY_THROW(mc_sys_write_input(&sys, 0, 1));
        EXPECT_ANY_THROW(mc_sys_read_input(&sys, 0));
        EXPECT_ANY_THROW(mc_sys_read_output(&sys, 0));
        EXPECT_ANY_THROW(mc_sys_get_alias(&sys, 0, &cmd));
        EXPECT_ANY_THROW(mc_sys_get_function_count(&sys));
        EXPECT_ANY_THROW(mc_sys_get_input_count(&sys));
        EXPECT_ANY_THROW(mc_sys_get_output_count(&sys));
        EXPECT_ANY_THROW(mc_sys_get_alias_count(&sys));
    }
}