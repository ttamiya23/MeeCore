#include "unity.h"
#include <stdio.h>
#include <string.h>
#include "mc/debug.h"
#include "mc/io.h"
#include "mc/event.h"
#include "mc/list.h"
#include "utils.h" // Needed for assert_helper.h
#include "assert_helper.h"

// Globals helpers.
mc_io_t io;
char debug_output[1024];
int index = 0;
char tx_buffer[256];
char rx_buffer[1];

bool mock_write(void *ctx, char c)
{
    if (index < 1023)
    {
        debug_output[index++] = c;
        debug_output[index] = '\0';
    }
    return true;
}

bool mock_read(void *ctx, char *c)
{
    return true;
}

uint8_t mock_get_status(void *ctx)
{
    return MC_IO_STATUS_OK;
}

const mc_io_driver_t mock_driver = {
    .write_char = mock_write,
    .read_char = mock_read,
    .get_status = mock_get_status};

void setUp()
{
    memset(debug_output, 0, sizeof(debug_output));
    index = 0;
    mc_io_init(&io, &mock_driver, NULL, rx_buffer, 1, tx_buffer, sizeof(tx_buffer));
    mc_debug_init(&io);
    mc_debug_set_level(MC_LOG_LEVEL_DEBUG);
}

void test_log_filters_lower_level_messages()
{
    // Set Level to WARNING (Should ignore INFO/DEBUG)
    mc_debug_set_level(MC_LOG_LEVEL_WARNING);

    // Log an INFO message (Should be ignored)
    MC_LOG_INFORMATION("This should be invisible");
    TEST_ASSERT_EQUAL_STRING("", debug_output);

    // Log a WARN message (Should appear)
    MC_LOG_WARNING("This is a warning %d", 99);
    char *res = strstr(debug_output, "[WRN]");
    TEST_ASSERT_NOT_NULL(res);
    res = strstr(debug_output, "test_debug.c");
    TEST_ASSERT_NOT_NULL(res);
    res = strstr(debug_output, "This is a warning 99");
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

    TEST_ASSERT(strlen(debug_output) > 0);
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

    TEST_ASSERT_EQUAL_INT(0, strlen(debug_output));
}