#include <gtest/gtest.h>
#include <string>
#include <cstring>
#include "mc_test.h"
extern "C"
{
#include "mc/communication/stream.h"
#include "mc/event.h"
#include "fakes/communication/fake_stream.h"
}

namespace
{
    // Globals for this test suite
    fake_stream_ctx_t ctx;
    MC_DEFINE_STREAM(stream, fake_stream_driver, ctx, 32, 32,
                     MC_STREAM_MODE_TEXT_LINE);

    // Event callback struct
    typedef struct
    {
        int event_fired_count;
        char data[32];
        uint16_t length;
    } my_stream_callback_t;

    // Callback: Captures the event data and fire count
    void on_line_received(void *ctx, void *data)
    {
        my_stream_callback_t *cb = (my_stream_callback_t *)ctx;
        mc_stream_event_data_t *event_data = (mc_stream_event_data_t *)data;
        if (stream.state->mode == MC_STREAM_MODE_TEXT_LINE)
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
    my_stream_callback_t cb;
    MC_DEFINE_CALLBACK(cb_handle, on_line_received, cb);

    // The Test Fixture
    class StreamTest : public MeeCoreTest
    {
    protected:
        void SetUp() override
        {
            MeeCoreTest::SetUp();

            // Reset globals
            cb.event_fired_count = 0;
            std::memset(cb.data, 0, sizeof(cb.data));
            mc_stream_init(&stream);
            mc_stream_set_mode(&stream, MC_STREAM_MODE_TEXT_LINE);
            mc_stream_register_rx_callback(&stream, &cb_handle);
        }
    };

    TEST_F(StreamTest, ReadFiresEvent)
    {
        fake_stream_push_string(&ctx, "Hello\n");
        mc_status_t res = mc_stream_update(&stream);

        EXPECT_EQ(MC_OK, res);
        EXPECT_EQ(1, cb.event_fired_count);
        EXPECT_STREQ("Hello", cb.data);

        // Next message should trigger another event.
        fake_stream_push_string(&ctx, "Good Bye\n");
        res = mc_stream_update(&stream);

        EXPECT_EQ(MC_OK, res);
        EXPECT_EQ(2, cb.event_fired_count);
        EXPECT_STREQ("Good Bye", cb.data);
    }

    TEST_F(StreamTest, ReadHandlesMultipleDelimiters)
    {
        // Send "CMD1\r" (Mac Style) then "CMD2\r\n" (Windows Style)
        fake_stream_push_string(&ctx, "CMD1\r\r\n");

        mc_stream_update(&stream);
        EXPECT_EQ(1, cb.event_fired_count);
        EXPECT_STREQ("CMD1", cb.data);

        // No events should fire after first update.
        mc_stream_update(&stream);
        EXPECT_EQ(1, cb.event_fired_count);
        mc_stream_update(&stream);
        EXPECT_EQ(1, cb.event_fired_count);
    }

    TEST_F(StreamTest, ReadLongMessageSetsOverflow)
    {
        // Buffer is 32 bytes. We send 40 chars + newline.
        // "1234567890123456789012345678901234567890\n"
        fake_stream_push_string(&ctx, "1234567890123456789012345678901234567890\n");

        mc_status_t res = mc_stream_update(&stream);
        EXPECT_EQ(MC_ERROR_NO_RESOURCE, res);
        EXPECT_EQ(MC_STREAM_STATUS_RX_OVERFLOW, mc_stream_get_status(&stream));
        EXPECT_EQ(1, cb.event_fired_count);

        // Check truncation length (Buffer size 32 - 1 for null = 31 chars).
        // Using std::string for partial comparison
        std::string expected = "1234567890123456789012345678901";
        EXPECT_EQ(expected, std::string(cb.data));

        // Next long message should keep overflow status.
        fake_stream_push_string(&ctx, "Supercalifragilisticexpialidocious\n");

        res = mc_stream_update(&stream);
        EXPECT_EQ(MC_ERROR_NO_RESOURCE, res);
        EXPECT_EQ(MC_STREAM_STATUS_RX_OVERFLOW, mc_stream_get_status(&stream));
        EXPECT_EQ(2, cb.event_fired_count);

        expected = "Supercalifragilisticexpialidoci";
        EXPECT_EQ(expected, std::string(cb.data));

        // Next short message should reset overflow.
        fake_stream_push_string(&ctx, "Hello\n");

        res = mc_stream_update(&stream);
        EXPECT_EQ(MC_OK, res);
        EXPECT_EQ(MC_STREAM_STATUS_OK, mc_stream_get_status(&stream));
        EXPECT_EQ(3, cb.event_fired_count);
        EXPECT_STREQ("Hello", cb.data);
    }

    TEST_F(StreamTest, ReadHandlesFragmentation)
    {
        // Send partial packet
        fake_stream_push_string(&ctx, "Hel");
        mc_status_t res = mc_stream_update(&stream);

        EXPECT_EQ(MC_OK, res);
        EXPECT_EQ(0, cb.event_fired_count);

        // Send rest of packet
        fake_stream_push_string(&ctx, "lo\n");
        res = mc_stream_update(&stream);

        EXPECT_EQ(MC_OK, res);
        EXPECT_EQ(1, cb.event_fired_count);
        EXPECT_STREQ("Hello", cb.data);
    }

    TEST_F(StreamTest, ReadHandlesBinaryStreamMode)
    {
        mc_stream_set_mode(&stream, MC_STREAM_MODE_BINARY_STREAM);
        // Send partial packet. Expect event to fire.
        fake_stream_push_string(&ctx, "Hel");
        mc_status_t res = mc_stream_update(&stream);

        EXPECT_EQ(MC_OK, res);
        EXPECT_EQ(1, cb.event_fired_count);
        EXPECT_EQ(3, cb.length);
        EXPECT_EQ(0, std::memcmp("Hel", cb.data, 3));

        // Send rest of packet. Expect another event to fire.
        fake_stream_push_string(&ctx, "lo\n");
        res = mc_stream_update(&stream);

        EXPECT_EQ(MC_OK, res);
        EXPECT_EQ(2, cb.event_fired_count);
        EXPECT_EQ(3, cb.length);
        // Should include newline character
        EXPECT_EQ(0, std::memcmp("lo\n", cb.data, 3));

        // Send null characters. Expect another event to fire.
        fake_stream_push_char_array(&ctx, "\0\0", 2);
        res = mc_stream_update(&stream);

        EXPECT_EQ(MC_OK, res);
        EXPECT_EQ(3, cb.event_fired_count);
        EXPECT_EQ(2, cb.length);
        EXPECT_EQ(0, std::memcmp("\0\0", cb.data, 2));
    }

    TEST_F(StreamTest, WriteSendsBytes)
    {
        char msg[] = {'a', 'd', '-', 0, 3};
        mc_stream_write(&stream, msg, 5);

        // Compare memory content
        EXPECT_EQ(0, std::memcmp(msg, ctx.output_data, 5));
    }

    TEST_F(StreamTest, PrintfSuccess)
    {
        mc_stream_printf(&stream, "Val: %d", 42);
        EXPECT_STREQ("Val: 42", ctx.output_data);
    }

    TEST_F(StreamTest, StreamStatusGetsConvertedToGenericStatus)
    {
        // Simulate a hardware fault. Update should return error too.
        ctx.status = MC_STREAM_STATUS_ERROR;
        EXPECT_EQ(MC_ERROR, mc_stream_update(&stream));

        // Add buffer overflow bit. Return should still be error.
        ctx.status |= MC_STREAM_STATUS_RX_OVERFLOW;
        EXPECT_EQ(MC_ERROR, mc_stream_update(&stream));

        // Set to no response.
        ctx.status = MC_STREAM_STATUS_NO_RESPONSE;
        EXPECT_EQ(MC_ERROR_NO_RESPONSE, mc_stream_update(&stream));

        // Set to busy.
        ctx.status = MC_STREAM_STATUS_HW_BUSY;
        EXPECT_EQ(MC_ERROR_BUSY, mc_stream_update(&stream));

        // Set to overflow.
        ctx.status = MC_STREAM_STATUS_RX_OVERFLOW;
        EXPECT_EQ(MC_ERROR_NO_RESOURCE, mc_stream_update(&stream));

        // Set to OK. Overflow status should be sticky.
        ctx.status = MC_STREAM_STATUS_OK;
        EXPECT_EQ(MC_ERROR_NO_RESOURCE, mc_stream_update(&stream));

        // Receiving newline character should fix overflow status.
        fake_stream_push_string(&ctx, "a\n");
        EXPECT_EQ(MC_OK, mc_stream_update(&stream));
    }

    TEST_F(StreamTest, AssertDeathIfStreamIsNull)
    {

        EXPECT_ANY_THROW(mc_stream_write(NULL, "", 0));
        EXPECT_ANY_THROW(mc_stream_write(&stream, NULL, 0));
        EXPECT_ANY_THROW(mc_stream_printf(NULL, ""));
        EXPECT_ANY_THROW(mc_stream_printf(&stream, NULL));
        EXPECT_ANY_THROW(mc_stream_vprintf(NULL, "", 0));
        EXPECT_ANY_THROW(mc_stream_vprintf(&stream, NULL, 0));
        EXPECT_ANY_THROW(mc_stream_update(NULL));
    }

    TEST_F(StreamTest, AssertDeathIfStreamNotInitialized)
    {
        mc_stream_t new_stream;
        EXPECT_ANY_THROW(mc_stream_write(&new_stream, "", 0));
        EXPECT_ANY_THROW(mc_stream_printf(&new_stream, ""));
        EXPECT_ANY_THROW(mc_stream_vprintf(&new_stream, "", 0));
        EXPECT_ANY_THROW(mc_stream_update(&new_stream));
    }
}
