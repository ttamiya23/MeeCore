#include "unity.h"
#include <stdio.h>
#include <string.h>
#include "mc/debug.h"
#include "mc/io.h"
#include "mc/event.h"
#include "mc/list.h"
#include "fake_io.h"
#include "utils.h" // Needed for assert_helper.h
#include "assert_helper.h"

// Globals helpers.
mc_io_t io;
fake_io_ctx_t ctx;

void setUp()
{
    fake_io_init(&io, &ctx);
    mc_debug_init(&io);
    mc_debug_set_level(MC_LOG_LEVEL_DEBUG);
}

void test_log_filters_lower_level_messages()
{
    // Set Level to WARNING (Should ignore INFO/DEBUG)
    mc_debug_set_level(MC_LOG_LEVEL_WARNING);

    // Log an INFO message (Should be ignored)
    MC_LOG_INFORMATION("This should be invisible");
    TEST_ASSERT_EQUAL_STRING("", ctx.output_data);

    // Log a WARN message (Should appear)
    MC_LOG_WARNING("This is a warning %d", 99);
    char *res = strstr(ctx.output_data, "[WRN]");
    TEST_ASSERT_NOT_NULL(res);
    res = strstr(ctx.output_data, "test_debug.c");
    TEST_ASSERT_NOT_NULL(res);
    res = strstr(ctx.output_data, "This is a warning 99");
    TEST_ASSERT_NOT_NULL(res);
}

void test_log_macros_expand_correctly()
{
    // This test just ensures the macros expand correctly
    // and don't cause syntax errors.
    MC_LOG_CRITICAL("Test Critical");
    MC_LOG_ERROR("Test Error");
    MC_LOG_WARNING("Test Warn");
    MC_LOG_INFORMATION("Test Info");
    MC_LOG_DEBUG("Test Debug");
    MC_LOG_TRACE("Test Trace");

    TEST_ASSERT(strlen(ctx.output_data) > 0);
}

void test_log_does_nothing_if_io_not_initialized()
{
    // Don't initalize IO handle.
    mc_io_t new_io;
    mc_debug_init(&new_io);

    MC_LOG_CRITICAL("Test Critical");
    MC_LOG_ERROR("Test Error");
    MC_LOG_WARNING("Test Warn");
    MC_LOG_INFORMATION("Test Info");
    MC_LOG_DEBUG("Test Debug");
    MC_LOG_TRACE("Test Trace");

    TEST_ASSERT_EQUAL_INT(0, strlen(ctx.output_data));
}