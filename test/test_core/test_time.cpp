#include <gtest/gtest.h>
#include "mc_test.h"

extern "C"
{
#include "mc/utils.h"
#include "mc/time.h"
#include "fakes/fake_time.h"
}

namespace
{

    // Globals
    fake_time_ctx_t ctx;

    class TimeTest : public MeeCoreTest
    {
    protected:
        void SetUp() override
        {
            MeeCoreTest::SetUp();
            mc_time_init(&fake_time_driver, &ctx);
        }
    };

    TEST_F(TimeTest, InitCallsDriverInit)
    {
        // Artificially break the context to prove init fixes it
        ctx.is_initialized = 0;
        ctx.current_time_ms = 55;

        mc_time_init(&fake_time_driver, &ctx);

        EXPECT_EQ(MC_INITIALIZED, ctx.is_initialized);
        EXPECT_EQ(0, ctx.current_time_ms);
    }

    TEST_F(TimeTest, GetMsDelegatesToDriver)
    {
        ctx.current_time_ms = 4200;

        uint32_t now = mc_time_get_ms();

        EXPECT_EQ(4200, now);
    }

    TEST_F(TimeTest, DelayDelegatesToDriver)
    {
        ctx.current_time_ms = 1000;

        mc_time_delay(500);

        EXPECT_EQ(1500, ctx.current_time_ms);
    }

    TEST_F(TimeTest, AssertDeathIfInitDriverIsNull)
    {
        EXPECT_ANY_THROW(mc_time_init(NULL, &ctx));
    }

    TEST_F(TimeTest, AssertDeathIfDriverMissingFunctions)
    {
        mc_time_driver_t bad_driver = {
            .init = NULL,
            .get_ms = NULL,
            .delay = NULL};

        EXPECT_ANY_THROW(mc_time_init(&bad_driver, &ctx));
    }

}