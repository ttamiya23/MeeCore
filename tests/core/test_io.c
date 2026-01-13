#include "unity.h"
#include <stdio.h>
#include <string.h>
#include <assert.h>

#include "mc/io.h"
#include "mc/event.h"
#include "mc/list.h"
#include "fakes/fake_io.h"
#include "utils.h" // Needed for assert_helper.h
#include "assert_helper.h"

// Event callback mock
typedef struct
{
    int event_fired_count;
    char data[32];
} my_callback;

// Globals for tests
fake_io_ctx_t ctx;
mc_callback_t cb_handle;
my_callback cb;

MC_DEFINE_IO(io, fake_io_driver, ctx, 1024, 1024);
MC_DEFINE_IO(small_io, fake_io_driver, ctx, 32, 32);

// Callback: Captures the event data and fire count
void on_line_received(void *ctx, void *data)
{
    my_callback *cb = (my_callback *)ctx;
    strncpy(cb->data, (char *)data, sizeof(cb->data));
    cb->event_fired_count++;
}

void setUp()
{
    test_assert_init();
    // Reset Core
    cb.event_fired_count = 0;
    memset(cb.data, 0, sizeof(cb.data));

    // Init
    fake_io_init(&ctx);
    mc_io_init(&io);
    mc_io_init(&small_io);
    mc_callback_init(&cb_handle, on_line_received, &cb);
    mc_io_register_rx_callback(&io, &cb_handle);
}

void test_read_fires_event()
{
    fake_io_push_string(&ctx, "Hello\n");
    mc_status_t res = mc_io_update(&io);

    TEST_ASSERT_EQUAL_INT8(MC_OK, res);
    TEST_ASSERT_EQUAL_INT(1, cb.event_fired_count);
    TEST_ASSERT_EQUAL_STRING("Hello", cb.data);

    // Next message should trigger another event.
    fake_io_push_string(&ctx, "Good Bye\n");
    res = mc_io_update(&io);

    TEST_ASSERT_EQUAL_INT8(MC_OK, res);
    TEST_ASSERT_EQUAL_INT(2, cb.event_fired_count);
    TEST_ASSERT_EQUAL_STRING("Good Bye", cb.data);
}

void test_read_handles_multiple_delimiters()
{
    // Send "CMD1\r" (Mac Style) then "CMD2\r\n" (Windows Style)
    fake_io_push_string(&ctx, "CMD1\r\r\n");

    mc_io_update(&io);
    TEST_ASSERT_EQUAL_INT(1, cb.event_fired_count);
    TEST_ASSERT_EQUAL_STRING("CMD1", cb.data);

    // No events should fire after first update.
    mc_io_update(&io);
    TEST_ASSERT_EQUAL_INT(1, cb.event_fired_count);
    mc_io_update(&io);
    TEST_ASSERT_EQUAL_INT(1, cb.event_fired_count);
}

void test_read_long_message_sets_overflow()
{
    mc_io_register_rx_callback(&small_io, &cb_handle);

    // Buffer is 32 bytes. We send 40 chars + newline.
    // "1234567890123456789012345678901234567890\n"
    fake_io_push_string(&ctx, "1234567890123456789012345678901234567890\n");

    mc_status_t res = mc_io_update(&small_io);
    TEST_ASSERT_EQUAL_INT8(MC_ERROR_NO_RESOURCE, res);
    TEST_ASSERT_EQUAL_INT8(MC_IO_STATUS_RX_OVERFLOW, mc_io_get_status(&small_io));
    TEST_ASSERT_EQUAL_INT(1, cb.event_fired_count);

    // Check truncation length (Buffer size 32 - 1 for null = 31 chars).
    TEST_ASSERT_EQUAL_STRING_LEN("1234567890123456789012345678901", cb.data, 31);

    // Next long message should keep overflow status.
    fake_io_push_string(&ctx, "Supercalifragilisticexpialidocious\n");

    res = mc_io_update(&small_io);
    TEST_ASSERT_EQUAL_INT8(MC_ERROR_NO_RESOURCE, res);
    TEST_ASSERT_EQUAL_INT8(MC_IO_STATUS_RX_OVERFLOW, mc_io_get_status(&small_io));
    TEST_ASSERT_EQUAL_INT(2, cb.event_fired_count);
    TEST_ASSERT_EQUAL_STRING_LEN("Supercalifragilisticexpialidoci", cb.data, 31);

    // Next short message should reset overflow.
    fake_io_push_string(&ctx, "Hello\n");

    res = mc_io_update(&small_io);
    TEST_ASSERT_EQUAL_INT8(MC_OK, res);
    TEST_ASSERT_EQUAL_INT8(MC_IO_STATUS_OK, mc_io_get_status(&small_io));
    TEST_ASSERT_EQUAL_INT(3, cb.event_fired_count);
    TEST_ASSERT_EQUAL_STRING("Hello", cb.data);
}

void test_read_handles_fragmentation()
{
    // Send partial packet
    fake_io_push_string(&ctx, "Hel");
    mc_status_t res = mc_io_update(&io);

    TEST_ASSERT_EQUAL_INT8(MC_OK, res);
    TEST_ASSERT_EQUAL_INT(0, cb.event_fired_count);

    // Send rest of packet
    fake_io_push_string(&ctx, "lo\n");
    res = mc_io_update(&io);

    TEST_ASSERT_EQUAL_INT8(MC_OK, res);
    TEST_ASSERT_EQUAL_INT(1, cb.event_fired_count);
    TEST_ASSERT_EQUAL_STRING("Hello", cb.data);
}

void test_write_sends_bytes()
{
    char msg[] = {'a', 'd', '-', 0, 3};
    mc_io_write(&io, msg, 5);

    TEST_ASSERT_EQUAL_CHAR_ARRAY(msg, ctx.output_data, 5);
}

void test_printf_success()
{
    mc_io_printf(&io, "Val: %d", 42);

    TEST_ASSERT_EQUAL_STRING("Val: 42", ctx.output_data);
}

void test_io_status_gets_coverted_to_generic_status()
{
    // Simulate a hardware fault. Update should return error too.
    ctx.status = MC_IO_STATUS_ERROR;
    TEST_ASSERT_EQUAL_INT8(MC_ERROR, mc_io_update(&io));

    // Add buffer overflow bit. Return should still be error.
    ctx.status |= MC_IO_STATUS_RX_OVERFLOW;
    TEST_ASSERT_EQUAL_INT8(MC_ERROR, mc_io_update(&io));

    // Set to no resopnse.
    ctx.status = MC_IO_STATUS_NO_RESPONSE;
    TEST_ASSERT_EQUAL_INT8(MC_ERROR_NO_RESPONSE, mc_io_update(&io));

    // Set to busy.
    ctx.status = MC_IO_STATUS_HW_BUSY;
    TEST_ASSERT_EQUAL_INT8(MC_ERROR_BUSY, mc_io_update(&io));

    // Set to overflow.
    ctx.status = MC_IO_STATUS_RX_OVERFLOW;
    TEST_ASSERT_EQUAL_INT8(MC_ERROR_NO_RESOURCE, mc_io_update(&io));

    // Set to OK. Overflow status should be sticky.
    ctx.status = MC_IO_STATUS_OK;
    TEST_ASSERT_EQUAL_INT8(MC_ERROR_NO_RESOURCE, mc_io_update(&io));

    // Receiving newline character should fix overflow status.
    fake_io_push_string(&ctx, "a\n");
    TEST_ASSERT_EQUAL_INT8(MC_OK, mc_io_update(&io));
}

void test_assert_death_if_io_is_null(void)
{
    TEST_ASSERT_DEATH(mc_io_update(NULL));
}

void test_assert_death_if_io_not_initialized(void)
{
    mc_io_t new_io;

    TEST_ASSERT_DEATH(mc_io_update(&new_io));
}