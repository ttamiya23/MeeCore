#include <stdio.h>
#include <string.h>
#include <assert.h>

#include "mc/io.h"
#include "mc/event.h"
#include "mc/list.h"
#include "mock_utils.h" // Needed for assert_helper.h
#include "assert_helper.h"

#define MOCK_RX_SIZE 256
#define MOCK_TX_SIZE 256

typedef struct
{
    // RX Simulation (Computer -> MCU)
    char input_data[MOCK_RX_SIZE];
    int input_head;
    int input_tail;

    // TX Simulation (MCU -> Computer)
    char output_data[MOCK_TX_SIZE];
    int output_index;

    uint8_t status; // For mocking status
} mock_hardware_t;

// Driver: Write (Stores in output buffer)
bool mock_write(void *ctx, char c)
{
    mock_hardware_t *h = (mock_hardware_t *)ctx;
    if (h->output_index < MOCK_TX_SIZE - 1)
    {
        h->output_data[h->output_index++] = c;
        h->output_data[h->output_index] = '\0'; // Keep null terminated for easy printing
    }
    return true;
}

// Driver: Read (Reads from input buffer)
bool mock_read(void *ctx, char *c)
{
    mock_hardware_t *h = (mock_hardware_t *)ctx;
    if (h->input_head != h->input_tail)
    {
        *c = h->input_data[h->input_tail++];
        return true;
    }
    return false;
}

uint8_t mock_get_status(void *ctx)
{
    mock_hardware_t *h = (mock_hardware_t *)ctx;
    return h->status;
}

const mc_io_driver_t mock_driver = {
    .write_char = mock_write,
    .read_char = mock_read,
    .get_status = mock_get_status};

// Event callback mock
typedef struct
{
    int event_fired_count;
    char data[32];
} my_callback;

// Callback: Captures the event data and fire count
void on_line_received(void *ctx, void *data)
{
    my_callback *cb = (my_callback *)ctx;
    strncpy(cb->data, (char *)data, sizeof(cb->data));
    cb->event_fired_count++;
}

// Globals for tests
mock_hardware_t hw;
mc_io_t io;
my_callback cb;
mc_callback_t cb_handle;
char rx_mem[32];
char tx_mem[64];

// Helper: Push string to Mock RX FIFO
void push_string(const char *str)
{
    while (*str)
    {
        hw.input_data[hw.input_head++] = *str++;
    }
}

void setUp()
{
    // Reset Hardware
    hw.input_head = 0;
    hw.input_tail = 0;
    hw.output_index = 0;
    hw.status = MC_IO_STATUS_OK;
    memset(hw.output_data, 0, MOCK_TX_SIZE);

    // Reset Core
    memset(rx_mem, 0, sizeof(rx_mem));
    memset(tx_mem, 0, sizeof(tx_mem));
    cb.event_fired_count = 0;
    memset(cb.data, 0, sizeof(cb.data));

    // Init
    mc_io_init(&io, &mock_driver, &hw, rx_mem, 32, tx_mem, 64);
    mc_callback_init(&cb_handle, on_line_received, &cb);
    mc_io_register_rx_callback(&io, &cb_handle);
}

void test_read_fires_event()
{
    push_string("Hello\n");
    mc_status_t res = mc_io_update(&io);

    TEST_ASSERT_EQUAL_INT8(MC_OK, res);
    TEST_ASSERT_EQUAL_INT(1, cb.event_fired_count);
    TEST_ASSERT_EQUAL_STRING("Hello", cb.data);

    // Next message should trigger another event.
    push_string("Good Bye\n");
    res = mc_io_update(&io);

    TEST_ASSERT_EQUAL_INT8(MC_OK, res);
    TEST_ASSERT_EQUAL_INT(2, cb.event_fired_count);
    TEST_ASSERT_EQUAL_STRING("Good Bye", cb.data);
}

void test_read_handles_multiple_delimiters()
{
    // Send "CMD1\r" (Mac Style) then "CMD2\r\n" (Windows Style)
    push_string("CMD1\r\r\n");

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
    // Buffer is 32 bytes. We send 40 chars + newline.
    // "1234567890123456789012345678901234567890\n"
    push_string("1234567890123456789012345678901234567890\n");

    mc_status_t res = mc_io_update(&io);
    TEST_ASSERT_EQUAL_INT8(MC_ERROR_NO_RESOURCE, res);
    TEST_ASSERT_EQUAL_INT8(MC_IO_STATUS_RX_OVERFLOW, mc_io_get_status(&io));
    TEST_ASSERT_EQUAL_INT(1, cb.event_fired_count);

    // Check truncation length (Buffer size 32 - 1 for null = 31 chars).
    TEST_ASSERT_EQUAL_STRING_LEN("1234567890123456789012345678901", cb.data, 31);

    // Next long message should keep overflow status.
    push_string("Supercalifragilisticexpialidocious\n");

    res = mc_io_update(&io);
    TEST_ASSERT_EQUAL_INT8(MC_ERROR_NO_RESOURCE, res);
    TEST_ASSERT_EQUAL_INT8(MC_IO_STATUS_RX_OVERFLOW, mc_io_get_status(&io));
    TEST_ASSERT_EQUAL_INT(2, cb.event_fired_count);
    TEST_ASSERT_EQUAL_STRING_LEN("Supercalifragilisticexpialidoci", cb.data, 31);

    // Next short message should reset overflow.
    push_string("Hello\n");

    res = mc_io_update(&io);
    TEST_ASSERT_EQUAL_INT8(MC_OK, res);
    TEST_ASSERT_EQUAL_INT8(MC_IO_STATUS_OK, mc_io_get_status(&io));
    TEST_ASSERT_EQUAL_INT(3, cb.event_fired_count);
    TEST_ASSERT_EQUAL_STRING("Hello", cb.data);
}

void test_read_handles_fragmentation()
{
    // Send partial packet
    push_string("Hel");
    mc_status_t res = mc_io_update(&io);

    TEST_ASSERT_EQUAL_INT8(MC_OK, res);
    TEST_ASSERT_EQUAL_INT(0, cb.event_fired_count);

    // Send rest of packet
    push_string("lo\n");
    res = mc_io_update(&io);

    TEST_ASSERT_EQUAL_INT8(MC_OK, res);
    TEST_ASSERT_EQUAL_INT(1, cb.event_fired_count);
    TEST_ASSERT_EQUAL_STRING("Hello", cb.data);
}

void test_write_sends_bytes()
{
    char msg[] = {'a', 'd', '-', 0, 3};
    mc_io_write(&io, msg, 5);

    TEST_ASSERT_EQUAL_CHAR_ARRAY(msg, hw.output_data, 5);
}

void test_printf_success()
{
    mc_io_printf(&io, "Val: %d", 42);

    TEST_ASSERT_EQUAL_STRING("Val: 42", hw.output_data);
}

void test_io_status_gets_coverted_to_generic_status()
{
    // Simulate a hardware fault. Update should return error too.
    hw.status = MC_IO_STATUS_ERROR;
    TEST_ASSERT_EQUAL_INT8(MC_ERROR, mc_io_update(&io));

    // Add buffer overflow bit. Return should still be error.
    hw.status |= MC_IO_STATUS_RX_OVERFLOW;
    TEST_ASSERT_EQUAL_INT8(MC_ERROR, mc_io_update(&io));

    // Set to no resopnse.
    hw.status = MC_IO_STATUS_NO_RESPONSE;
    TEST_ASSERT_EQUAL_INT8(MC_ERROR_NO_RESPONSE, mc_io_update(&io));

    // Set to busy.
    hw.status = MC_IO_STATUS_HW_BUSY;
    TEST_ASSERT_EQUAL_INT8(MC_ERROR_BUSY, mc_io_update(&io));

    // Set to overflow.
    hw.status = MC_IO_STATUS_RX_OVERFLOW;
    TEST_ASSERT_EQUAL_INT8(MC_ERROR_NO_RESOURCE, mc_io_update(&io));

    // Set to OK. Overflow status should be sticky.
    hw.status = MC_IO_STATUS_OK;
    TEST_ASSERT_EQUAL_INT8(MC_ERROR_NO_RESOURCE, mc_io_update(&io));

    // Receiving newline character should fix overflow status.
    push_string("a\n");
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