#include <gtest/gtest.h>
#include <string>
#include <cstring>
#include "mc_test.h"
extern "C"
{
#include "mc/io.h"
#include "mc/event.h"
#include "fakes/fake_io.h"
}

namespace
{
    // Globals for this test suite
    fake_io_ctx_t ctx;
    MC_DEFINE_IO(io, fake_io_driver, ctx, 32, 32, MC_IO_MODE_TEXT_LINE);

    // Event callback struct
    typedef struct
    {
        int event_fired_count;
        char data[32];
        uint16_t length;
    } my_io_callback_t;

    // Callback: Captures the event data and fire count
    void on_line_received(void *ctx, void *data)
    {
        my_io_callback_t *cb = (my_io_callback_t *)ctx;
        mc_io_event_data_t *event_data = (mc_io_event_data_t *)data;
        if (io.state->mode == MC_IO_MODE_TEXT_LINE)
        {
            strncpy(cb->data, event_data->message, sizeof(cb->data) - 1);
        }
        else
        {
            memcpy(cb->data, event_data->message, event_data->length);
        }
        cb->length = event_data->length;
        cb->event_fired_count++;
    }
    my_io_callback_t cb;
    MC_DEFINE_CALLBACK(cb_handle, on_line_received, cb);

    // The Test Fixture
    class IoTest : public MeeCoreTest
    {
    protected:
        void SetUp() override
        {
            MeeCoreTest::SetUp();

            // Reset globals
            cb.event_fired_count = 0;
            std::memset(cb.data, 0, sizeof(cb.data));
            mc_io_init(&io);
            mc_io_set_mode(&io, MC_IO_MODE_TEXT_LINE);
            mc_io_register_rx_callback(&io, &cb_handle);
        }
    };

    TEST_F(IoTest, ReadFiresEvent)
    {
        fake_io_push_string(&ctx, "Hello\n");
        mc_status_t res = mc_io_update(&io);

        EXPECT_EQ(MC_OK, res);
        EXPECT_EQ(1, cb.event_fired_count);
        EXPECT_STREQ("Hello", cb.data);

        // Next message should trigger another event.
        fake_io_push_string(&ctx, "Good Bye\n");
        res = mc_io_update(&io);

        EXPECT_EQ(MC_OK, res);
        EXPECT_EQ(2, cb.event_fired_count);
        EXPECT_STREQ("Good Bye", cb.data);
    }

    TEST_F(IoTest, ReadHandlesMultipleDelimiters)
    {
        // Send "CMD1\r" (Mac Style) then "CMD2\r\n" (Windows Style)
        fake_io_push_string(&ctx, "CMD1\r\r\n");

        mc_io_update(&io);
        EXPECT_EQ(1, cb.event_fired_count);
        EXPECT_STREQ("CMD1", cb.data);

        // No events should fire after first update.
        mc_io_update(&io);
        EXPECT_EQ(1, cb.event_fired_count);
        mc_io_update(&io);
        EXPECT_EQ(1, cb.event_fired_count);
    }

    TEST_F(IoTest, ReadLongMessageSetsOverflow)
    {
        // Buffer is 32 bytes. We send 40 chars + newline.
        // "1234567890123456789012345678901234567890\n"
        fake_io_push_string(&ctx, "1234567890123456789012345678901234567890\n");

        mc_status_t res = mc_io_update(&io);
        EXPECT_EQ(MC_ERROR_NO_RESOURCE, res);
        EXPECT_EQ(MC_IO_STATUS_RX_OVERFLOW, mc_io_get_status(&io));
        EXPECT_EQ(1, cb.event_fired_count);

        // Check truncation length (Buffer size 32 - 1 for null = 31 chars).
        // Using std::string for partial comparison
        std::string expected = "1234567890123456789012345678901";
        EXPECT_EQ(expected, std::string(cb.data));

        // Next long message should keep overflow status.
        fake_io_push_string(&ctx, "Supercalifragilisticexpialidocious\n");

        res = mc_io_update(&io);
        EXPECT_EQ(MC_ERROR_NO_RESOURCE, res);
        EXPECT_EQ(MC_IO_STATUS_RX_OVERFLOW, mc_io_get_status(&io));
        EXPECT_EQ(2, cb.event_fired_count);

        expected = "Supercalifragilisticexpialidoci";
        EXPECT_EQ(expected, std::string(cb.data));

        // Next short message should reset overflow.
        fake_io_push_string(&ctx, "Hello\n");

        res = mc_io_update(&io);
        EXPECT_EQ(MC_OK, res);
        EXPECT_EQ(MC_IO_STATUS_OK, mc_io_get_status(&io));
        EXPECT_EQ(3, cb.event_fired_count);
        EXPECT_STREQ("Hello", cb.data);
    }

    TEST_F(IoTest, ReadHandlesFragmentation)
    {
        // Send partial packet
        fake_io_push_string(&ctx, "Hel");
        mc_status_t res = mc_io_update(&io);

        EXPECT_EQ(MC_OK, res);
        EXPECT_EQ(0, cb.event_fired_count);

        // Send rest of packet
        fake_io_push_string(&ctx, "lo\n");
        res = mc_io_update(&io);

        EXPECT_EQ(MC_OK, res);
        EXPECT_EQ(1, cb.event_fired_count);
        EXPECT_STREQ("Hello", cb.data);
    }

    TEST_F(IoTest, ReadHandlesBinaryStreamMode)
    {
        mc_io_set_mode(&io, MC_IO_MODE_BINARY_STREAM);
        // Send partial packet. Expect event to fire.
        fake_io_push_string(&ctx, "Hel");
        mc_status_t res = mc_io_update(&io);

        EXPECT_EQ(MC_OK, res);
        EXPECT_EQ(1, cb.event_fired_count);
        EXPECT_EQ(3, cb.length);
        EXPECT_EQ(0, std::memcmp("Hel", cb.data, 3));

        // Send rest of packet. Expect another event to fire.
        fake_io_push_string(&ctx, "lo\n");
        res = mc_io_update(&io);

        EXPECT_EQ(MC_OK, res);
        EXPECT_EQ(2, cb.event_fired_count);
        EXPECT_EQ(3, cb.length);
        // Should include newline character
        EXPECT_EQ(0, std::memcmp("lo\n", cb.data, 3));

        // Send null characters. Expect another event to fire.
        fake_io_push_char_array(&ctx, "\0\0", 2);
        res = mc_io_update(&io);

        EXPECT_EQ(MC_OK, res);
        EXPECT_EQ(3, cb.event_fired_count);
        EXPECT_EQ(2, cb.length);
        EXPECT_EQ(0, std::memcmp("\0\0", cb.data, 2));
    }

    TEST_F(IoTest, WriteSendsBytes)
    {
        char msg[] = {'a', 'd', '-', 0, 3};
        mc_io_write(&io, msg, 5);

        // Compare memory content
        EXPECT_EQ(0, std::memcmp(msg, ctx.output_data, 5));
    }

    TEST_F(IoTest, PrintfSuccess)
    {
        mc_io_printf(&io, "Val: %d", 42);
        EXPECT_STREQ("Val: 42", ctx.output_data);
    }

    TEST_F(IoTest, IoStatusGetsConvertedToGenericStatus)
    {
        // Simulate a hardware fault. Update should return error too.
        ctx.status = MC_IO_STATUS_ERROR;
        EXPECT_EQ(MC_ERROR, mc_io_update(&io));

        // Add buffer overflow bit. Return should still be error.
        ctx.status |= MC_IO_STATUS_RX_OVERFLOW;
        EXPECT_EQ(MC_ERROR, mc_io_update(&io));

        // Set to no response.
        ctx.status = MC_IO_STATUS_NO_RESPONSE;
        EXPECT_EQ(MC_ERROR_NO_RESPONSE, mc_io_update(&io));

        // Set to busy.
        ctx.status = MC_IO_STATUS_HW_BUSY;
        EXPECT_EQ(MC_ERROR_BUSY, mc_io_update(&io));

        // Set to overflow.
        ctx.status = MC_IO_STATUS_RX_OVERFLOW;
        EXPECT_EQ(MC_ERROR_NO_RESOURCE, mc_io_update(&io));

        // Set to OK. Overflow status should be sticky.
        ctx.status = MC_IO_STATUS_OK;
        EXPECT_EQ(MC_ERROR_NO_RESOURCE, mc_io_update(&io));

        // Receiving newline character should fix overflow status.
        fake_io_push_string(&ctx, "a\n");
        EXPECT_EQ(MC_OK, mc_io_update(&io));
    }

    TEST_F(IoTest, AssertDeathIfIoIsNull)
    {

        EXPECT_ANY_THROW(mc_io_write(NULL, "", 0));
        EXPECT_ANY_THROW(mc_io_write(&io, NULL, 0));
        EXPECT_ANY_THROW(mc_io_printf(NULL, ""));
        EXPECT_ANY_THROW(mc_io_printf(&io, NULL));
        EXPECT_ANY_THROW(mc_io_vprintf(NULL, "", 0));
        EXPECT_ANY_THROW(mc_io_vprintf(&io, NULL, 0));
        EXPECT_ANY_THROW(mc_io_update(NULL));
    }

    TEST_F(IoTest, AssertDeathIfIoNotInitialized)
    {
        mc_io_t new_io;
        EXPECT_ANY_THROW(mc_io_write(&new_io, "", 0));
        EXPECT_ANY_THROW(mc_io_printf(&new_io, ""));
        EXPECT_ANY_THROW(mc_io_vprintf(&new_io, "", 0));
        EXPECT_ANY_THROW(mc_io_update(&new_io));
    }
}
