#include <gtest/gtest.h>
#include <Arduino.h>
#include <SerialFake.h>

using namespace fakeit;

extern "C"
{
#include "mc/stream.h"
#include "ports/arduino/arduino_stream.h"
}

// Global boolean for mocking connection status
static bool is_connected;

class ArduinoStreamTest : public ::testing::Test
{
protected:
    void SetUp() override
    {
        ArduinoFakeReset();
        is_connected = true;
        _ctx = {
            .stream = &Serial,
            .is_connected = []
            {
                return is_connected;
            }};
    }

    // Arduino stream context
    mc_arduino_stream_ctx_t _ctx;
};

TEST_F(ArduinoStreamTest, WriteCharSendsDataToStram)
{
    When(OverloadedMethod(ArduinoFake(Serial), write, size_t(uint8_t))).AlwaysReturn(1);

    bool result = mc_arduino_stream_driver.write_char(&_ctx, 'A');

    EXPECT_TRUE(result);
    Verify(OverloadedMethod(ArduinoFake(Serial), write, size_t(uint8_t))('A')).Once();
}

TEST_F(ArduinoStreamTest, WriteCharReturnsFalseOnStreamFailure)
{
    When(OverloadedMethod(ArduinoFake(Serial), write, size_t(uint8_t))).AlwaysReturn(0);

    bool result = mc_arduino_stream_driver.write_char(&_ctx, 'B');

    EXPECT_FALSE(result);
}

TEST_F(ArduinoStreamTest, WriteCharReturnsFalseOnConnectionFailure)
{
    is_connected = false;

    When(OverloadedMethod(ArduinoFake(Serial), write, size_t(uint8_t))).AlwaysReturn(1);

    bool result = mc_arduino_stream_driver.write_char(&_ctx, 'A');

    EXPECT_FALSE(result);
}

TEST_F(ArduinoStreamTest, ReadCharGetsDataFromStream)
{
    When(Method(ArduinoFake(Serial), available)).AlwaysReturn(1);
    When(Method(ArduinoFake(Serial), read)).AlwaysReturn('A');

    char x;
    bool result = mc_arduino_stream_driver.read_char(&_ctx, &x);

    EXPECT_TRUE(result);
    Verify(Method(ArduinoFake(Serial), available)).Once();
    Verify(Method(ArduinoFake(Serial), read)).Once();
    EXPECT_EQ(x, 'A');
}

TEST_F(ArduinoStreamTest, ReadCharReturnsFalseOnNotAvailable)
{
    When(Method(ArduinoFake(Serial), available)).AlwaysReturn(0);

    char x;
    bool result = mc_arduino_stream_driver.read_char(&_ctx, &x);

    EXPECT_FALSE(result);
    Verify(Method(ArduinoFake(Serial), available)).Once();
    Verify(Method(ArduinoFake(Serial), read)).Never();
}

TEST_F(ArduinoStreamTest, ReadCharReturnsFalseOnConnectionFailure)
{
    is_connected = false;
    When(Method(ArduinoFake(Serial), available)).AlwaysReturn(1);

    char x;
    bool result = mc_arduino_stream_driver.read_char(&_ctx, &x);

    EXPECT_FALSE(result);
    Verify(Method(ArduinoFake(Serial), available)).Never();
    Verify(Method(ArduinoFake(Serial), read)).Never();
}

TEST_F(ArduinoStreamTest, GetStatusReturnsOkWhenConnected)
{
    uint8_t result = mc_arduino_stream_driver.get_status(&_ctx);
    EXPECT_EQ(result, MC_STREAM_STATUS_OK);
}

TEST_F(ArduinoStreamTest, GetStatusReturnsNoResponseWhenDisconnected)
{
    is_connected = false;
    uint8_t result = mc_arduino_stream_driver.get_status(&_ctx);
    EXPECT_EQ(result, MC_STREAM_STATUS_NO_RESPONSE);
}

int main(int argc, char **argv)
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}