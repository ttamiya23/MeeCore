#include "mc_test.h"

extern "C"
{
#include "mc/io.h"
#include "mc/system/console.h"
#include "fakes/fake_io.h"
#include "fakes/system/fake_system.h"
#include "fakes/system/fake_composite.h"
}

namespace
{

    // Globals
    fake_io_ctx_t io_ctx;
    MC_DEFINE_IO(io, fake_io_driver, io_ctx, 256, 256, MC_IO_MODE_TEXT_LINE);
    fake_sys_ctx_t sys_ctx1;
    MC_DEFINE_SYSTEM(sys1, fake_sys_driver, sys_ctx1);
    fake_sys_ctx_t sys_ctx2;
    MC_DEFINE_SYSTEM(sys2, fake_sys_driver, sys_ctx2);
    fake_composite_ctx_t sys_ctx3;
    MC_DEFINE_SYSTEM(sys3, fake_composite_sys_driver, sys_ctx3);

    const mc_system_entry_t systems[] = {
        MC_SYS_ENTRY(1, sys1, "fake1"),
        MC_SYS_ENTRY(2, sys2, "fake2"),
        MC_SYS_ENTRY(3, sys3, NULL),
    };

    MC_DEFINE_SYSTEM_CONSOLE(console, io, systems, 3, 8, 5);

    class ConsoleTest : public MeeCoreTest
    {
    protected:
        void SetUp() override
        {
            MeeCoreTest::SetUp();

            mc_sys_init(&sys1);
            mc_sys_init(&sys2);
            mc_sys_init(&sys3);
            mc_io_init(&io);
            // Hack: unregister and register callback
            mc_event_unregister(&io.state->rx_event, &console.state->rx_callback);
            mc_sys_console_init(&console);
        }

        // Helper for sending a command.
        void send_command(const char *cmd)
        {
            fake_io_push_string(&io_ctx, cmd);
            fake_io_push_string(&io_ctx, "\n");
            mc_io_update(&io);
        }
    };

    TEST_F(ConsoleTest, InitSetsIsInitializedField)
    {
        EXPECT_EQ(MC_INITIALIZED, console.state->is_initialized);
    }

    TEST_F(ConsoleTest, SendReadCommandSucceeds)
    {
        sys_ctx1.x[0] = 3;
        send_command("s1.x0");
        EXPECT_STREQ(
            "\x02\n"
            "s1.x0\n"
            "3\n"
            "\x03",
            io_ctx.output_data);
    }

    TEST_F(ConsoleTest, SendWriteCommandSucceeds)
    {
        sys_ctx1.x[0] = 3;
        send_command("s1.x0 = 32");
        EXPECT_STREQ(
            "\x02\n"
            "s1.x0 = 32\n"
            "0\n"
            "\x03",
            io_ctx.output_data);
        EXPECT_EQ(32, sys_ctx1.x[0]);
    }

    TEST_F(ConsoleTest, SendInvokeCommandSucceeds)
    {
        send_command("s1.f0");
        EXPECT_STREQ(
            "\x02\n"
            "s1.f0\n"
            "0\n"
            "\x03",
            io_ctx.output_data);
        EXPECT_EQ(1, sys_ctx1.y[0]);
    }

    TEST_F(ConsoleTest, OtherSystemsSucceed)
    {
        sys_ctx2.y[0] = 9;
        send_command("s2.y0");
        EXPECT_STREQ(
            "\x02\n"
            "s2.y0\n"
            "9\n"
            "\x03",
            io_ctx.output_data);
    }

    TEST_F(ConsoleTest, SingleSystemDumpSucceeds)
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
        EXPECT_STREQ(
            "\x02\n"
            "s2\n"
            "sys\t0\t1\t2\t3\t4\n"
            "s2.x\t4\t13\tE2\tE2\tE2\n"
            "s2.y\t9\tE2\tE2\tE2\tE2\n"
            "\x03",
            io_ctx.output_data);
    }

    TEST_F(ConsoleTest, CommandsWithAliasSucceed)
    {
        send_command("fake2.incrementY");
        EXPECT_STREQ(
            "\x02\n"
            "fake2.incrementY\n"
            "0\n"
            "\x03",
            io_ctx.output_data);
        EXPECT_EQ(1, sys_ctx2.y[0]);

        // Reset IO output buffer
        io_ctx.output_index = 0;
        // Ensure the buffer string is also terminated/cleared if fake_io implementation relies on null terminator
        io_ctx.output_data[0] = '\0';

        sys_ctx3.sys2.x0_name = "testX0";
        send_command("s3.testX0 = 4");
        EXPECT_STREQ(
            "\x02\n"
            "s3.testX0 = 4\n"
            "0\n"
            "\x03",
            io_ctx.output_data);
        EXPECT_EQ(4, sys_ctx3.sys2.x[0]);
    }

    TEST_F(ConsoleTest, AliasWithPresetValueSucceed)
    {
        sys_ctx1.x[0] = 5;
        send_command("s1.reset");
        EXPECT_STREQ(
            "\x02\n"
            "s1.reset\n"
            "0\n"
            "\x03",
            io_ctx.output_data);
        EXPECT_EQ(0, sys_ctx1.x[0]);
    }

    TEST_F(ConsoleTest, ErrorResultGetsPropagated)
    {
        send_command("s1,f1,5");
        EXPECT_STREQ(
            "\x02\n"
            "s1,f1,5\n"
            "E5\n"
            "\x03",
            io_ctx.output_data);
    }

    TEST_F(ConsoleTest, ExtraDelimitersGetIgnored)
    {
        // Simplifies to s1.x1=2
        send_command(",,,..===(s1)===,,,x1 (  .\t,2)\t,..    \t");
        EXPECT_STREQ(
            "\x02\n"
            "s1)===,,,x1 (  .\t,2)\t,..\n"
            "0\n"
            "\x03",
            io_ctx.output_data);
        EXPECT_EQ(2, sys_ctx1.x[1]);
    }

    TEST_F(ConsoleTest, DelimitersOnlyReturnsError)
    {
        send_command("\t ,, ==   = (   ,. .)  \t ");
        EXPECT_STREQ(
            "\x02\n"
            "E2\n" // Invalid args
            "\x03",
            io_ctx.output_data);
    }

    TEST_F(ConsoleTest, InvalidSystemReturnsError)
    {
        send_command("s6.x0");
        EXPECT_STREQ(
            "\x02\n"
            "s6.x0\n"
            "E2\n" // Invalid args
            "\x03",
            io_ctx.output_data);
    }

    TEST_F(ConsoleTest, InvalidAliasReturnsError)
    {
        send_command("s1.myFunction");
        EXPECT_STREQ(
            "\x02\n"
            "s1.myFunction\n"
            "E2\n" // Invalid args
            "\x03",
            io_ctx.output_data);
    }

    TEST_F(ConsoleTest, ClearCommandClearsTerminal)
    {
        send_command("clear");
        EXPECT_STREQ("\x1B[2J\x1B[H", io_ctx.output_data);
    }

    TEST_F(ConsoleTest, HelpCommandPrintsHelp)
    {
        send_command("?s1");
        EXPECT_STREQ(
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

    TEST_F(ConsoleTest, DumpSucceeds)
    {
        sys_ctx1.x[0] = 0;
        sys_ctx1.x[1] = 1;
        sys_ctx1.y[0] = 2;
        sys_ctx2.x[0] = 3;
        sys_ctx2.x[1] = 4;
        sys_ctx2.y[0] = 5;
        sys_ctx3.sys1.x[0] = 6;
        sys_ctx3.sys1.x[1] = 7;
        sys_ctx3.sys2.x[0] = 8;
        sys_ctx3.sys2.x[1] = 9;
        sys_ctx3.sys1.y[0] = 10;
        sys_ctx3.sys2.y[0] = 11;

        mc_status_t ret = mc_sys_console_dump(&console, systems, 3);
        EXPECT_EQ(MC_OK, ret);
        EXPECT_STREQ(
            "\x02\n"
            "sys\t0\t1\t2\t3\t4\n"
            "s1.x\t0\t1\tE2\tE2\tE2\n"
            "s1.y\t2\tE2\tE2\tE2\tE2\n"
            "s2.x\t3\t4\tE2\tE2\tE2\n"
            "s2.y\t5\tE2\tE2\tE2\tE2\n"
            "s3.x\t6\t7\t8\t9\tE2\n"
            "s3.y\t10\t11\tE2\tE2\tE2\n"
            "\x03",
            io_ctx.output_data);
    }

}