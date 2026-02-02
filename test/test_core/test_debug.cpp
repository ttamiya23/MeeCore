#include "mc_test.h"
#include <cstring>
#include <string>

extern "C"
{
#include "mc/debug.h"
#include "mc/communication/stream.h"
#include "mc/time.h"
#include "fakes/communication/fake_stream.h"
#include "fakes/fake_time.h"
}

namespace
{

    // Globals
    fake_stream_ctx_t ctx;
    fake_time_ctx_t time_ctx;
    MC_DEFINE_STREAM(stream, fake_stream_driver, ctx, 1024, 1024, MC_STREAM_MODE_TEXT_LINE);

    class DebugTest : public MeeCoreTest
    {
    protected:
        void SetUp() override
        {
            MeeCoreTest::SetUp();

            // Reset and Initialize
            mc_stream_init(&stream);
            mc_debug_init(&stream);
            mc_debug_set_level(MC_LOG_LEVEL_DEBUG);
            mc_time_init(&fake_time_driver, &time_ctx);
        }
    };

    TEST_F(DebugTest, LogFiltersLowerLevelMessages)
    {
        // Set Level to WARNING (Should ignore INFO/DEBUG)
        mc_debug_set_level(MC_LOG_LEVEL_WARNING);

        // Set time to 1000 ms
        fake_time_set_ms(&time_ctx, 1000);

        // Log an INFO message (Should be ignored)
        MC_LOG_INFORMATION("This should be invisible");
        EXPECT_STREQ("", ctx.output_data);

        // Log a WARN message (Should appear)
        MC_LOG_WARNING("This is a warning %d", 99);

        // Verify output contains specific parts
        EXPECT_TRUE(strstr(ctx.output_data, "[WRN]       1000") != nullptr);
        EXPECT_TRUE(strstr(ctx.output_data, "test_debug.cpp:") != nullptr);
        EXPECT_TRUE(strstr(ctx.output_data, "This is a warning 99") != nullptr);
    }

    TEST_F(DebugTest, LogMacrosExpandCorrectly)
    {
        // This test just ensures the macros expand correctly
        // and don't cause syntax errors or crashes.
        MC_LOG_CRITICAL("Test Critical");
        MC_LOG_ERROR("Test Error");
        MC_LOG_WARNING("Test Warn");
        MC_LOG_INFORMATION("Test Info");
        MC_LOG_DEBUG("Test Debug");
        MC_LOG_TRACE("Test Trace");

        EXPECT_GT(strlen(ctx.output_data), 0);
    }

    TEST_F(DebugTest, LogDoesNothingIfStreamNull)
    {
        // Re-init with NULL
        mc_debug_init(NULL);

        MC_LOG_CRITICAL("Test Critical");
        MC_LOG_ERROR("Test Error");
        MC_LOG_WARNING("Test Warn");
        MC_LOG_INFORMATION("Test Info");
        MC_LOG_DEBUG("Test Debug");
        MC_LOG_TRACE("Test Trace");

        EXPECT_EQ(0, strlen(ctx.output_data));
    }

}