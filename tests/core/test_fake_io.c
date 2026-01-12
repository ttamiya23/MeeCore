#include "unity.h"
#include <stdio.h>
#include <string.h>

#include "mc/io.h"
#include "mc/list.h"
#include "fakes/fake_io.h"
#include "assert_helper.h"

// Globals for tests
fake_io_ctx_t ctx;
mc_io_t io;

void setUp()
{
    fake_io_init(&io, &ctx);
}

void test_init_resets_data()
{
    TEST_ASSERT_EQUAL_INT(0, ctx.input_head);
    TEST_ASSERT_EQUAL_INT(0, ctx.input_tail);
    TEST_ASSERT_EQUAL_INT(0, ctx.output_index);
    TEST_ASSERT_EQUAL(MC_IO_STATUS_OK, ctx.status);
    TEST_ASSERT_EQUAL_STRING("", ctx.output_data);
}

void test_write_sends_to_output_data()
{
    bool ret = fake_io_driver.write_char(&ctx, 'a');
    TEST_ASSERT_TRUE(ret);
    TEST_ASSERT_EQUAL_INT(1, ctx.output_index);
    TEST_ASSERT_EQUAL_STRING("a", ctx.output_data);
}

void test_write_fails_if_buffer_full()
{
    for (int i = 0; i < FAKE_TX_SIZE; i++)
    {
        fake_io_driver.write_char(&ctx, 'a');
    }
    TEST_ASSERT_EQUAL_INT(FAKE_TX_SIZE - 1, ctx.output_index);
    bool ret = fake_io_driver.write_char(&ctx, 'b');
    TEST_ASSERT_FALSE(ret);
    // Index should remain max size - 1
    TEST_ASSERT_EQUAL_INT(FAKE_TX_SIZE - 1, ctx.output_index);
}

void test_push_string_appends_to_input_data()
{
    fake_io_push_string(&ctx, "a");
    TEST_ASSERT_EQUAL_INT(1, ctx.input_head);
    TEST_ASSERT_EQUAL_INT(0, ctx.input_tail);
    TEST_ASSERT_EQUAL_CHAR_ARRAY("a", ctx.input_data, 1);
    fake_io_push_string(&ctx, "b");
    TEST_ASSERT_EQUAL_INT(2, ctx.input_head);
    TEST_ASSERT_EQUAL_INT(0, ctx.input_tail);
    TEST_ASSERT_EQUAL_CHAR_ARRAY("ab", ctx.input_data, 2);
}

void test_read_returns_char_from_tail()
{
    fake_io_push_string(&ctx, "hello");
    char c;

    bool ret = fake_io_driver.read_char(&ctx, &c);
    TEST_ASSERT_TRUE(ret);
    TEST_ASSERT_EQUAL_CHAR('h', c);
    TEST_ASSERT_EQUAL_INT(5, ctx.input_head);
    TEST_ASSERT_EQUAL_INT(1, ctx.input_tail);

    ret = fake_io_driver.read_char(&ctx, &c);
    TEST_ASSERT_TRUE(ret);
    TEST_ASSERT_EQUAL_CHAR('e', c);
    TEST_ASSERT_EQUAL_INT(5, ctx.input_head);
    TEST_ASSERT_EQUAL_INT(2, ctx.input_tail);

    ret = fake_io_driver.read_char(&ctx, &c);
    TEST_ASSERT_TRUE(ret);
    TEST_ASSERT_EQUAL_CHAR('l', c);
    TEST_ASSERT_EQUAL_INT(5, ctx.input_head);
    TEST_ASSERT_EQUAL_INT(3, ctx.input_tail);

    ret = fake_io_driver.read_char(&ctx, &c);
    TEST_ASSERT_TRUE(ret);
    TEST_ASSERT_EQUAL_CHAR('l', c);
    TEST_ASSERT_EQUAL_INT(5, ctx.input_head);
    TEST_ASSERT_EQUAL_INT(4, ctx.input_tail);

    ret = fake_io_driver.read_char(&ctx, &c);
    TEST_ASSERT_TRUE(ret);
    TEST_ASSERT_EQUAL_CHAR('o', c);
    TEST_ASSERT_EQUAL_INT(5, ctx.input_head);
    TEST_ASSERT_EQUAL_INT(5, ctx.input_tail);

    // Next read should fail, since no more chars to read
    ret = fake_io_driver.read_char(&ctx, &c);
    TEST_ASSERT_FALSE(ret);
    TEST_ASSERT_EQUAL_INT(5, ctx.input_head);
    TEST_ASSERT_EQUAL_INT(5, ctx.input_tail);
}

void test_get_status_returns_status()
{
    ctx.status = MC_IO_STATUS_HW_BUSY;
    mc_io_status_t status = fake_io_driver.get_status(&ctx);
    TEST_ASSERT_EQUAL(MC_IO_STATUS_HW_BUSY, status);

    ctx.status |= MC_IO_STATUS_NO_RESPONSE;
    status = fake_io_driver.get_status(&ctx);
    TEST_ASSERT_EQUAL(MC_IO_STATUS_HW_BUSY | MC_IO_STATUS_NO_RESPONSE, status);
}