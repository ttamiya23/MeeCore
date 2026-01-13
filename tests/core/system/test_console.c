#include "unity.h"
#include "mc/io.h"
#include "mc/system/console.h"
#include "mc/system/console.h"
#include "fakes/fake_io.h"
#include "fakes/system/fake_system.h"
#include "fakes/system/fake_composite.h"
#include "utils.h" // Needed for assert_helper.h
#include "assert_helper.h"

// Globals
fake_io_ctx_t io_ctx;
MC_DEFINE_IO(io, fake_io_driver, io_ctx, 1024, 1024);
fake_sys_ctx_t sys_ctx0;
MC_DEFINE_SYSTEM(sys0, fake_sys_driver, sys_ctx0);
fake_sys_ctx_t sys_ctx1;
MC_DEFINE_SYSTEM(sys1, fake_sys_driver, sys_ctx1);
fake_sys_ctx_t sys_ctx2;
MC_DEFINE_SYSTEM(sys2, fake_sys_driver, sys_ctx2);
fake_composite_ctx_t sys_ctx3;
MC_DEFINE_SYSTEM(sys3, fake_composite_sys_driver, sys_ctx3);
mc_system_console_t console;

const mc_system_entry_t systems[] = {
    MC_SYS_ENTRY(0, sys0, "fake0"),
    MC_SYS_ENTRY(1, sys1, "fake1"),
    MC_SYS_ENTRY(2, sys2, "fake2"),
    MC_SYS_ENTRY(3, sys3, NULL),
};

// Helper for sending a command.
static void send_command(const char *cmd)
{
    fake_io_push_string(&io_ctx, cmd);
    fake_io_push_string(&io_ctx, "\n");
    mc_io_update(&io);
}

void setUp()
{
    test_assert_init();
    mc_sys_init(&sys1);
    mc_sys_init(&sys2);
    mc_sys_init(&sys3);
    fake_io_init(&io_ctx);
    mc_io_init(&io);
    mc_sys_console_init(&console, &io, systems, 4);
}

void test_init_sets_is_initialized_field()
{
    // Artificially set is_initialized to a different value
    console.is_initialized = 3;

    mc_sys_console_init(&console, &io, systems, 3);

    // Should reset to MC_INITIALIZED
    TEST_ASSERT_EQUAL_INT32(MC_INITIALIZED, console.is_initialized);
}

void test_send_read_command_succeeds()
{
    sys_ctx1.x[0] = 3;
    send_command("s1.x0");
    TEST_ASSERT_EQUAL_STRING(
        "\x02\n"
        "s1.x0\n"
        "3\n"
        "\x03",
        io_ctx.output_data);
}

void test_send_write_command_succeeds()
{
    sys_ctx1.x[0] = 3;
    send_command("s1.x0 = 32");
    TEST_ASSERT_EQUAL_STRING(
        "\x02\n"
        "s1.x0 = 32\n"
        "0\n"
        "\x03",
        io_ctx.output_data);
    TEST_ASSERT_EQUAL_INT32(32, sys_ctx1.x[0]);
}

void test_send_invoke_command_succeeds()
{
    send_command("s1.f0");
    TEST_ASSERT_EQUAL_STRING(
        "\x02\n"
        "s1.f0\n"
        "0\n"
        "\x03",
        io_ctx.output_data);
    TEST_ASSERT_EQUAL_INT32(1, sys_ctx1.y[0]);
}

void test_other_systems_succeed()
{
    sys_ctx2.y[0] = 9;
    send_command("s2.y0");
    TEST_ASSERT_EQUAL_STRING(
        "\x02\n"
        "s2.y0\n"
        "9\n"
        "\x03",
        io_ctx.output_data);
}

void test_single_system_dump_succeeds()
{
    sys_ctx2.x[0] = 4;
    sys_ctx2.x[1] = 13;
    sys_ctx2.y[0] = 9;
    send_command("s2");

    /* Response should look like:
     * <STX>
     * s2
     * sys  0   1   2   3   4
     * s2.x 4  13  E2  E2  E2
     * s2.y 9  E2  E2  E2  E2
     * <ETX>
     */
    TEST_ASSERT_EQUAL_STRING(
        "\x02\n"
        "s2\n"
        "sys\t0\t1\t2\t3\t4\n"
        "s2.x\t4\t13\tE2\tE2\tE2\n"
        "s2.y\t9\tE2\tE2\tE2\tE2\n"
        "\x03",
        io_ctx.output_data);
}

void test_commands_with_alias_succeed()
{
    send_command("fake2.incrementY");
    TEST_ASSERT_EQUAL_STRING(
        "\x02\n"
        "fake2.incrementY\n"
        "0\n"
        "\x03",
        io_ctx.output_data);
    TEST_ASSERT_EQUAL_INT32(1, sys_ctx2.y[0]);

    // Reset IO output buffer
    io_ctx.output_index = 0;

    sys_ctx3.sys2.x0_name = "testX0";
    send_command("s3.testX0 = 4");
    TEST_ASSERT_EQUAL_STRING(
        "\x02\n"
        "s3.testX0 = 4\n"
        "0\n"
        "\x03",
        io_ctx.output_data);
    TEST_ASSERT_EQUAL_INT32(4, sys_ctx3.sys2.x[0]);
}

void test_alias_with_preset_value_succeed()
{
    sys_ctx1.x[0] = 5;
    send_command("s1.reset");
    TEST_ASSERT_EQUAL_STRING(
        "\x02\n"
        "s1.reset\n"
        "0\n"
        "\x03",
        io_ctx.output_data);
    TEST_ASSERT_EQUAL_INT32(0, sys_ctx1.x[0]);
}

void test_error_result_gets_propagated()
{
    send_command("s1,f1,5");
    TEST_ASSERT_EQUAL_STRING(
        "\x02\n"
        "s1,f1,5\n"
        "E5\n"
        "\x03",
        io_ctx.output_data);
}

void test_extra_delimiters_get_ignored()
{
    // Simplifies to s1.x1=2
    send_command(",,,..===(s1)===,,,x1 (  .\t,2)\t,..    \t");
    TEST_ASSERT_EQUAL_STRING(
        "\x02\n"
        "s1)===,,,x1 (  .\t,2)\t,..\n"
        "0\n"
        "\x03",
        io_ctx.output_data);
    TEST_ASSERT_EQUAL_INT32(2, sys_ctx1.x[1]);
}

void test_delimiters_only_returns_error()
{
    send_command("\t ,, ==   = (   ,. .)  \t ");
    TEST_ASSERT_EQUAL_STRING(
        "\x02\n"
        "E2\n" // Invalid args
        "\x03",
        io_ctx.output_data);
}

void test_invalid_system_returns_error()
{
    send_command("s6.x0");
    TEST_ASSERT_EQUAL_STRING(
        "\x02\n"
        "s6.x0\n"
        "E2\n" // Invalid args
        "\x03",
        io_ctx.output_data);
}

void test_invalid_alias_returns_error()
{
    send_command("s1.myFunction");
    TEST_ASSERT_EQUAL_STRING(
        "\x02\n"
        "s1.myFunction\n"
        "E2\n" // Invalid args
        "\x03",
        io_ctx.output_data);
}

void test_invalid_system_id_returns_error()
{
    send_command("sx.x0 = 3");
    TEST_ASSERT_EQUAL_STRING(
        "\x02\n"
        "sx.x0 = 3\n"
        "E2\n" // Invalid args
        "\x03",
        io_ctx.output_data);
}

void test_invalid_arguments_returns_error()
{
    send_command("s1.x0 = hello");
    TEST_ASSERT_EQUAL_STRING(
        "\x02\n"
        "s1.x0 = hello\n"
        "E2\n" // Invalid args
        "\x03",
        io_ctx.output_data);
}

void test_clear_command_clears_terminal()
{
    send_command("clear");
    TEST_ASSERT_EQUAL_STRING("\x1B[2J\x1B[H", io_ctx.output_data);
}

void test_help_command_prints_help()
{
    send_command("?s1");
    TEST_ASSERT_EQUAL_STRING(
        "\x02\n"
        "?s1\n"
        "fake1 [s1]\n"
        "- incrementY [f0]\n"
        "- input [x0]\n"
        "- output [y0]\n"
        "- reset [x0 = 0]\n"
        "\x03",
        io_ctx.output_data);
}

void test_dump_succeeds()
{
    sys_ctx0.x[0] = 0;
    sys_ctx0.x[1] = 1;
    sys_ctx0.y[0] = 2;
    sys_ctx1.x[0] = 3;
    sys_ctx1.x[1] = 4;
    sys_ctx1.y[0] = 5;
    sys_ctx2.x[0] = 6;
    sys_ctx2.x[1] = 7;
    sys_ctx2.y[0] = 8;
    sys_ctx3.sys1.x[0] = 9;
    sys_ctx3.sys1.x[1] = 10;
    sys_ctx3.sys2.x[0] = 11;
    sys_ctx3.sys2.x[1] = 12;
    sys_ctx3.sys1.y[0] = 13;
    sys_ctx3.sys2.y[0] = 14;

    /* Response should look like:
     * <STX>
     * sys  0   1   2   3   4
     * s0.x 0   1  E2  E2  E2
     * s0.y 2  E2  E2  E2  E2
     * s1.x 3   4  E2  E2  E2
     * s1.y 5  E2  E2  E2  E2
     * s2.x 6   7  E2  E2  E2
     * s2.y 8  E2  E2  E2  E2
     * s3.x 9  10  11  12  E2
     * s3.y 13 14  E2  E2  E2
     * <ETX>
     */
    mc_status_t ret = mc_sys_console_dump(&console, systems, 4);
    TEST_ASSERT_EQUAL_INT(MC_OK, ret);
    TEST_ASSERT_EQUAL_STRING(
        "\x02\n"
        "sys\t0\t1\t2\t3\t4\n"
        "s0.x\t0\t1\tE2\tE2\tE2\n"
        "s0.y\t2\tE2\tE2\tE2\tE2\n"
        "s1.x\t3\t4\tE2\tE2\tE2\n"
        "s1.y\t5\tE2\tE2\tE2\tE2\n"
        "s2.x\t6\t7\tE2\tE2\tE2\n"
        "s2.y\t8\tE2\tE2\tE2\tE2\n"
        "s3.x\t9\t10\t11\t12\tE2\n"
        "s3.y\t13\t14\tE2\tE2\tE2\n"
        "\x03",
        io_ctx.output_data);
}

void test_set_header_count_suceeds()
{
    sys_ctx0.x[0] = 0;
    sys_ctx0.x[1] = 1;
    sys_ctx0.y[0] = 2;
    sys_ctx1.x[0] = 3;
    sys_ctx1.x[1] = 4;
    sys_ctx1.y[0] = 5;
    sys_ctx2.x[0] = 6;
    sys_ctx2.x[1] = 7;
    sys_ctx2.y[0] = 8;
    sys_ctx3.sys1.x[0] = 9;
    sys_ctx3.sys1.x[1] = 10;
    sys_ctx3.sys2.x[0] = 11;
    sys_ctx3.sys2.x[1] = 12;
    sys_ctx3.sys1.y[0] = 13;
    sys_ctx3.sys2.y[0] = 14;

    mc_sys_console_set_header_count(&console, 2);
    /* Response should look like:
     * <STX>
     * sys  0   1
     * s0.x 0   1
     * s0.y 2  E2
     * s1.x 3   4
     * s1.y 5  E2
     * s2.x 6   7
     * s2.y 8  E2
     * s3.x 9  10
     * s3.y 13 14
     * <ETX>
     */
    mc_status_t ret = mc_sys_console_dump(&console, systems, 4);
    TEST_ASSERT_EQUAL_INT(MC_OK, ret);
    TEST_ASSERT_EQUAL_STRING(
        "\x02\n"
        "sys\t0\t1\n"
        "s0.x\t0\t1\n"
        "s0.y\t2\tE2\n"
        "s1.x\t3\t4\n"
        "s1.y\t5\tE2\n"
        "s2.x\t6\t7\n"
        "s2.y\t8\tE2\n"
        "s3.x\t9\t10\n"
        "s3.y\t13\t14\n"
        "\x03",
        io_ctx.output_data);
}

void test_set_args_buffer_suceeds()
{
    int new_args_buffer[4];
    mc_sys_console_set_args_buffer(&console, new_args_buffer, 4);
    TEST_ASSERT_EQUAL_INT(4, console.args_count);
    TEST_ASSERT_EQUAL_PTR(new_args_buffer, console.args_buffer);
}