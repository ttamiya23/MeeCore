#include <gtest/gtest.h>
#include <string>
#include <cstring>
#include "mc_test.h"
extern "C"
{
#include "fakes/fake_stream.h"
}

namespace
{
    // Globals for this test suite
    fake_stream_ctx_t ctx;

    // The Test Fixture
    class FakeStreamTest : public MeeCoreTest
    {
    protected:
        void SetUp() override
        {
            MeeCoreTest::SetUp();
            fake_stream_driver.init(&ctx);
        }
    };

    TEST_F(FakeStreamTest, InitResetsData)
    {
        EXPECT_EQ(0, ctx.input_head);
        EXPECT_EQ(0, ctx.input_tail);
        EXPECT_EQ(0, ctx.output_index);
        EXPECT_EQ(MC_STREAM_STATUS_OK, ctx.status);
        EXPECT_STREQ("", ctx.output_data);
    }

    TEST_F(FakeStreamTest, WriteSendsToOutputData)
    {
        bool ret = fake_stream_driver.write_char(&ctx, 'a');
        EXPECT_TRUE(ret);
        EXPECT_EQ(1, ctx.output_index);
        EXPECT_STREQ("a", ctx.output_data);
    }

    TEST_F(FakeStreamTest, WriteFailsIfBufferFull)
    {
        for (int i = 0; i < FAKE_TX_SIZE; i++)
        {
            fake_stream_driver.write_char(&ctx, 'a');
        }
        EXPECT_EQ(FAKE_TX_SIZE - 1, ctx.output_index);
        bool ret = fake_stream_driver.write_char(&ctx, 'b');
        EXPECT_FALSE(ret);
        // Index should remain max size - 1
        EXPECT_EQ(FAKE_TX_SIZE - 1, ctx.output_index);
    }

    TEST_F(FakeStreamTest, PushStringAppendsToInputData)
    {
        fake_stream_push_string(&ctx, "a");
        EXPECT_EQ(1, ctx.input_head);
        EXPECT_EQ(0, ctx.input_tail);
        EXPECT_EQ(0, std::memcmp("a", ctx.input_data, 1));

        fake_stream_push_string(&ctx, "b");
        EXPECT_EQ(2, ctx.input_head);
        EXPECT_EQ(0, ctx.input_tail);
        EXPECT_EQ(0, std::memcmp("ab", ctx.input_data, 2));

        fake_stream_push_char_array(&ctx, "\0\0", 2);
        EXPECT_EQ(4, ctx.input_head);
        EXPECT_EQ(0, ctx.input_tail);
        EXPECT_EQ(0, std::memcmp("ab\0\0", ctx.input_data, 4));
    }

    TEST_F(FakeStreamTest, ReadReturnsCharFromTail)
    {
        fake_stream_push_string(&ctx, "hello");
        char c;

        bool ret = fake_stream_driver.read_char(&ctx, &c);
        EXPECT_TRUE(ret);
        EXPECT_EQ('h', c);
        EXPECT_EQ(5, ctx.input_head);
        EXPECT_EQ(1, ctx.input_tail);

        ret = fake_stream_driver.read_char(&ctx, &c);
        EXPECT_TRUE(ret);
        EXPECT_EQ('e', c);
        EXPECT_EQ(5, ctx.input_head);
        EXPECT_EQ(2, ctx.input_tail);

        ret = fake_stream_driver.read_char(&ctx, &c);
        EXPECT_TRUE(ret);
        EXPECT_EQ('l', c);
        EXPECT_EQ(5, ctx.input_head);
        EXPECT_EQ(3, ctx.input_tail);

        ret = fake_stream_driver.read_char(&ctx, &c);
        EXPECT_TRUE(ret);
        EXPECT_EQ('l', c);
        EXPECT_EQ(5, ctx.input_head);
        EXPECT_EQ(4, ctx.input_tail);

        ret = fake_stream_driver.read_char(&ctx, &c);
        EXPECT_TRUE(ret);
        EXPECT_EQ('o', c);
        EXPECT_EQ(5, ctx.input_head);
        EXPECT_EQ(5, ctx.input_tail);

        // Next read should fail, since no more chars to read
        ret = fake_stream_driver.read_char(&ctx, &c);
        EXPECT_FALSE(ret);
        EXPECT_EQ(5, ctx.input_head);
        EXPECT_EQ(5, ctx.input_tail);
    }

    TEST_F(FakeStreamTest, GetStatusReturnsStatus)
    {
        ctx.status = MC_STREAM_STATUS_HW_BUSY;
        uint8_t status = fake_stream_driver.get_status(&ctx);
        EXPECT_EQ(MC_STREAM_STATUS_HW_BUSY, status);

        ctx.status |= MC_STREAM_STATUS_NO_RESPONSE;
        status = fake_stream_driver.get_status(&ctx);
        EXPECT_EQ(MC_STREAM_STATUS_HW_BUSY | MC_STREAM_STATUS_NO_RESPONSE, status);
    }

}