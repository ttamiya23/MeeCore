#include <gtest/gtest.h>
#include "mc_test.h"

extern "C"
{
#include "mc/device/analog.h"
#include "mc/utils.h"
}

namespace
{

    // Globals
    MC_DEFINE_ANALOG_DATA_OBJECT(dev);

    class AnalogTest : public MeeCoreTest
    {
    protected:
        void SetUp() override
        {
            MeeCoreTest::SetUp();
            mc_analog_init(&dev, /*is_read_only=*/false);
        }
    };

    TEST_F(AnalogTest, InitResetsContext)
    {
        // Artificially set context
        dev_ctx.value = 194;
        mc_analog_init(&dev, /*is_read_only=*/false);

        // Should reset to defaults
        EXPECT_EQ(MC_INITIALIZED, dev.config->is_initialized);
        EXPECT_FALSE(dev.config->is_read_only);
        EXPECT_EQ(0, dev_ctx.value);
    }

    TEST_F(AnalogTest, SetValueSucceeds)
    {
        mc_status_t ret = mc_analog_set_value(&dev, 1553);
        EXPECT_EQ(MC_OK, ret);
        EXPECT_EQ(1553, dev_ctx.value);
    }

    TEST_F(AnalogTest, GetValueSucceeds)
    {
        dev_ctx.value = -24445;
        mc_result_t res = mc_analog_get_value(&dev);
        EXPECT_TRUE(res.ok);
        EXPECT_EQ(-24445, res.value);
    }

    TEST_F(AnalogTest, ReadOnlyGetValueSucceeds)
    {
        mc_analog_set_read_only(&dev, true);
        dev_ctx.value = 849922;
        mc_result_t res = mc_analog_get_value(&dev);
        EXPECT_TRUE(res.ok);
        EXPECT_EQ(849922, res.value);
    }

    TEST_F(AnalogTest, ReadOnlySetValueFails)
    {
        mc_analog_set_read_only(&dev, true);
        mc_status_t ret = mc_analog_set_value(&dev, 322);
        EXPECT_EQ(MC_ERROR_WRITE_PROTECTED, ret);
    }

    TEST_F(AnalogTest, ErrorStatusGetsPropagated)
    {
        mc_status_t error = MC_ERROR_BUSY;
        dev_ctx.error = error;

        mc_status_t ret = mc_analog_set_value(&dev, 0);
        EXPECT_EQ(error, ret);

        mc_result_t res;
        res = mc_analog_get_value(&dev);
        EXPECT_FALSE(res.ok);
        EXPECT_EQ(error, res.error);
    }

    TEST_F(AnalogTest, UnimplementedMethodsReturnNotSupported)
    {
        mc_analog_driver_t null_analog_driver = {
            .init = NULL,
            .set_value = NULL,
            .get_value = NULL};
        mc_analog_config_t config;
        mc_analog_t null_dev = {
            .driver = &null_analog_driver,
            .ctx = NULL,
            .config = &config};

        mc_analog_init(&null_dev, /*is_read_only=*/false);

        mc_status_t ret = mc_analog_set_value(&null_dev, 22);
        EXPECT_EQ(MC_ERROR_NOT_SUPPORTED, ret);

        mc_result_t res = mc_analog_get_value(&null_dev);
        EXPECT_FALSE(res.ok);
        EXPECT_EQ(MC_ERROR_NOT_SUPPORTED, res.error);
    }

    TEST_F(AnalogTest, AssertDeathIfNullPointer)
    {
        mc_analog_t null_dev = {
            .driver = NULL,
            .ctx = NULL,
            .config = NULL};

        EXPECT_ANY_THROW(mc_analog_set_value(NULL, 13242));
        EXPECT_ANY_THROW(mc_analog_set_value(&null_dev, -42485));

        EXPECT_ANY_THROW(mc_analog_get_value(NULL));
        EXPECT_ANY_THROW(mc_analog_get_value(&null_dev));

        EXPECT_ANY_THROW(mc_analog_set_read_only(NULL, true));
        EXPECT_ANY_THROW(mc_analog_set_read_only(&null_dev, true));
    }

    TEST_F(AnalogTest, AssertDeathIfUninitialized)
    {
        mc_analog_config_t config;
        mc_analog_t uninitialized_dev = {
            .driver = &mc_analog_data_object_driver,
            .ctx = &dev_ctx,
            .config = &config};

        EXPECT_ANY_THROW(mc_analog_set_value(&uninitialized_dev, -2));
        EXPECT_ANY_THROW(mc_analog_get_value(&uninitialized_dev));
        EXPECT_ANY_THROW(mc_analog_set_read_only(&uninitialized_dev, true));
    }

}