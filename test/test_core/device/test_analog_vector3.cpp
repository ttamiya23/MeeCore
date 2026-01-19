#include <gtest/gtest.h>
#include "mc_test.h"

extern "C"
{
#include "mc/device/analog_vector3.h"
#include "mc/device/analog.h"
#include "mc/utils.h"
}

namespace
{

    // Globals
    MC_DEFINE_ANALOG_VECTOR3_DATA_OBJECT(dev);

    class AnalogVector3Test : public MeeCoreTest
    {
    protected:
        void SetUp() override
        {
            MeeCoreTest::SetUp();
            mc_analog_vector3_init(&dev, /*is_read_only=*/false);
        }
    };

    TEST_F(AnalogVector3Test, InitResetsContext)
    {
        // Artificially set context
        dev_ctx.x = 194;
        dev_ctx.y = 195;
        dev_ctx.z = 196;

        // Re-init
        mc_analog_vector3_init(&dev, /*is_read_only=*/false);

        // Should reset to defaults
        EXPECT_EQ(MC_INITIALIZED, dev.x.config->is_initialized);
        EXPECT_EQ(MC_INITIALIZED, dev.y.config->is_initialized);
        EXPECT_EQ(MC_INITIALIZED, dev.z.config->is_initialized);
        EXPECT_FALSE(dev.x.config->is_read_only);
        EXPECT_FALSE(dev.y.config->is_read_only);
        EXPECT_FALSE(dev.z.config->is_read_only);
        EXPECT_EQ(0, dev_ctx.x);
        EXPECT_EQ(0, dev_ctx.y);
        EXPECT_EQ(0, dev_ctx.z);
    }

    TEST_F(AnalogVector3Test, SetValueSucceeds)
    {
        mc_vector3_t value = {
            .x = 3,
            .y = 4,
            .z = 5};
        mc_status_t ret = mc_analog_vector3_set_value(&dev, value);
        EXPECT_EQ(MC_OK, ret);
        EXPECT_EQ(3, dev_ctx.x);
        EXPECT_EQ(4, dev_ctx.y);
        EXPECT_EQ(5, dev_ctx.z);
    }

    TEST_F(AnalogVector3Test, SetValueErrorGetsPropagated)
    {
        mc_vector3_t value = {
            .x = 3,
            .y = 4,
            .z = 5};
        dev_ctx.error = MC_ERROR_NO_RESPONSE;
        mc_status_t ret = mc_analog_vector3_set_value(&dev, value);
        EXPECT_EQ(MC_ERROR_NO_RESPONSE, ret);
    }

    TEST_F(AnalogVector3Test, GetValueSucceeds)
    {
        dev_ctx.x = 9;
        dev_ctx.y = 8;
        dev_ctx.z = 7;
        mc_vector3_t value;
        mc_status_t ret = mc_analog_vector3_get_value(&dev, &value);
        EXPECT_EQ(MC_OK, ret);
        EXPECT_EQ(9, value.x);
        EXPECT_EQ(8, value.y);
        EXPECT_EQ(7, value.z);
    }

    TEST_F(AnalogVector3Test, GetValueErrorGetsPropagated)
    {
        dev_ctx.x = 9;
        dev_ctx.y = 8;
        dev_ctx.z = 7;
        mc_vector3_t value;
        dev_ctx.error = MC_ERROR_BUSY;
        mc_status_t ret = mc_analog_vector3_get_value(&dev, &value);
        EXPECT_EQ(MC_ERROR_BUSY, ret);
    }

    TEST_F(AnalogVector3Test, GetValueAssertDeathIfNullPointer)
    {
        EXPECT_ANY_THROW(mc_analog_vector3_get_value(&dev, NULL));
    }

    TEST_F(AnalogVector3Test, SetReadOnlySucceeds)
    {
        mc_analog_vector3_set_read_only(&dev, true);
        EXPECT_TRUE(dev.x.config->is_read_only);
        EXPECT_TRUE(dev.y.config->is_read_only);
        EXPECT_TRUE(dev.z.config->is_read_only);
    }

}