#include <gtest/gtest.h>
#include "mc_test.h"

extern "C"
{
#include "mc/device/digital.h"
#include "mc/utils.h"
}

namespace
{

    // Globals
    MC_DEFINE_DIGITAL_DATA_OBJECT(dev);

    class DigitalTest : public MeeCoreTest
    {
    protected:
        void SetUp() override
        {
            MeeCoreTest::SetUp();
            mc_digital_init(&dev, /*is_read_only=*/false);
        }
    };

    TEST_F(DigitalTest, InitResetsContext)
    {
        // Artificially set context
        dev_ctx.state = true;
        mc_digital_init(&dev, /*is_read_only=*/false);

        // Should reset to defaults
        EXPECT_EQ(MC_INITIALIZED, dev.config->is_initialized);
        EXPECT_FALSE(dev.config->is_read_only);
        EXPECT_FALSE(dev_ctx.state);
    }

    TEST_F(DigitalTest, SetStateSucceeds)
    {
        mc_status_t ret = mc_digital_set_state(&dev, true);
        EXPECT_EQ(MC_OK, ret);
        EXPECT_TRUE(dev_ctx.state);

        ret = mc_digital_set_state(&dev, false);
        EXPECT_EQ(MC_OK, ret);
        EXPECT_FALSE(dev_ctx.state);
    }

    TEST_F(DigitalTest, GetStateSucceeds)
    {
        dev_ctx.state = true;
        mc_result_t res = mc_digital_get_state(&dev);
        EXPECT_TRUE(res.ok);
        EXPECT_TRUE(res.value);

        dev_ctx.state = false;
        res = mc_digital_get_state(&dev);
        EXPECT_TRUE(res.ok);
        EXPECT_FALSE(res.value);
    }

    TEST_F(DigitalTest, ToggleSucceeds)
    {
        mc_status_t ret = mc_digital_toggle(&dev);
        EXPECT_EQ(MC_OK, ret);
        EXPECT_TRUE(dev_ctx.state);

        ret = mc_digital_toggle(&dev);
        EXPECT_EQ(MC_OK, ret);
        EXPECT_FALSE(dev_ctx.state);
    }

    TEST_F(DigitalTest, ReadOnlyGetStateSucceeds)
    {
        mc_digital_set_read_only(&dev, true);
        dev_ctx.state = true;
        mc_result_t res = mc_digital_get_state(&dev);
        EXPECT_TRUE(res.ok);
        EXPECT_TRUE(res.value);

        dev_ctx.state = false;
        res = mc_digital_get_state(&dev);
        EXPECT_TRUE(res.ok);
        EXPECT_FALSE(res.value);
    }

    TEST_F(DigitalTest, ReadOnlySetStateFails)
    {
        mc_digital_set_read_only(&dev, true);
        mc_status_t ret = mc_digital_set_state(&dev, true);
        EXPECT_EQ(MC_ERROR_WRITE_PROTECTED, ret);
    }

    TEST_F(DigitalTest, ReadOnlyToggleFails)
    {
        mc_digital_set_read_only(&dev, true);
        mc_status_t ret = mc_digital_toggle(&dev);
        EXPECT_EQ(MC_ERROR_WRITE_PROTECTED, ret);
    }

    TEST_F(DigitalTest, ErrorStatusGetsPropagated)
    {
        mc_status_t error = MC_ERROR_BUSY;
        dev_ctx.error = error;

        mc_status_t ret = mc_digital_set_state(&dev, true);
        EXPECT_EQ(error, ret);

        mc_result_t res;
        res = mc_digital_get_state(&dev);
        EXPECT_FALSE(res.ok);
        EXPECT_EQ(error, res.error);

        ret = mc_digital_toggle(&dev);
        EXPECT_EQ(error, ret);
    }

    TEST_F(DigitalTest, UnimplementedMethodsReturnNotSupported)
    {
        mc_digital_driver_t null_digital_driver = {
            .init = NULL,
            .set_state = NULL,
            .get_state = NULL};
        mc_digital_config_t config;
        mc_digital_t null_dev = {
            .driver = &null_digital_driver,
            .ctx = NULL,
            .config = &config};

        mc_digital_init(&null_dev, /*is_read_only=*/false);

        mc_status_t ret = mc_digital_set_state(&null_dev, true);
        EXPECT_EQ(MC_ERROR_NOT_SUPPORTED, ret);

        mc_result_t res = mc_digital_get_state(&null_dev);
        EXPECT_FALSE(res.ok);
        EXPECT_EQ(MC_ERROR_NOT_SUPPORTED, res.error);

        ret = mc_digital_toggle(&null_dev);
        EXPECT_EQ(MC_ERROR_NOT_SUPPORTED, ret);
    }

    TEST_F(DigitalTest, AssertDeathIfNullPointer)
    {
        mc_digital_t null_dev = {
            .driver = NULL,
            .ctx = NULL,
            .config = NULL};

        EXPECT_ANY_THROW(mc_digital_set_state(NULL, true));
        EXPECT_ANY_THROW(mc_digital_set_state(&null_dev, true));

        EXPECT_ANY_THROW(mc_digital_get_state(NULL));
        EXPECT_ANY_THROW(mc_digital_get_state(&null_dev));

        EXPECT_ANY_THROW(mc_digital_toggle(NULL));
        EXPECT_ANY_THROW(mc_digital_toggle(&null_dev));

        EXPECT_ANY_THROW(mc_digital_set_read_only(NULL, true));
        EXPECT_ANY_THROW(mc_digital_set_read_only(&null_dev, true));
    }

    TEST_F(DigitalTest, AssertDeathIfUninitialized)
    {
        mc_digital_config_t config;
        mc_digital_t uninitialized_dev = {
            .driver = &mc_digital_data_object_driver,
            .ctx = &dev_ctx,
            .config = &config};

        EXPECT_ANY_THROW(mc_digital_set_state(&uninitialized_dev, true));
        EXPECT_ANY_THROW(mc_digital_get_state(&uninitialized_dev));
        EXPECT_ANY_THROW(mc_digital_toggle(&uninitialized_dev));
        EXPECT_ANY_THROW(mc_digital_set_read_only(&uninitialized_dev, true));
    }

}