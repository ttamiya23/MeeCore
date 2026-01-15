#include <gtest/gtest.h>
#include "mc_test.h"

extern "C"
{
#include "mc/utils.h"
#include "fake_time.h"
}

namespace
{

    // Global
    fake_time_ctx_t ctx;

    class FakeTimeTest : public MeeCoreTest
    {
    protected:
        void SetUp() override
        {
            MeeCoreTest::SetUp();
            fake_time_driver.init(&ctx);
        }
    };

    TEST_F(FakeTimeTest, InitResetsTime)
    {
        ctx.current_time_ms = 9999;
        fake_time_driver.init(&ctx);

        EXPECT_EQ(0, ctx.current_time_ms);
        EXPECT_EQ(MC_INITIALIZED, ctx.is_initialized);
    }

    TEST_F(FakeTimeTest, GetMsReturnsCurrentTime)
    {
        ctx.current_time_ms = 1234;
        uint32_t val = fake_time_driver.get_ms(&ctx);
        EXPECT_EQ(1234, val);
    }

    TEST_F(FakeTimeTest, DelayAdvancesTime)
    {
        ctx.current_time_ms = 100;
        fake_time_driver.delay(&ctx, 50);

        EXPECT_EQ(150, ctx.current_time_ms);
    }

    TEST_F(FakeTimeTest, HelperSetMsSucceeds)
    {
        fake_time_set_ms(&ctx, 5000);
        EXPECT_EQ(5000, ctx.current_time_ms);
    }

}