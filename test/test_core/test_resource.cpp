#include <gtest/gtest.h>
#include "mc_test.h"
extern "C"
{
#include "mc/resource.h"
}

namespace
{
    // Globals
    int32_t x = 100;
    int32_t y = 200;
    int32_t z = 300;
    mc_resource_entry_t pool[] = {
        MC_RESOURCE_ENTRY(x),
        MC_RESOURCE_ENTRY(y),
        MC_RESOURCE_ENTRY(z)};
    MC_DEFINE_RESOURCE_MANAGER(manager, pool, 3);

    // The Test Fixture
    class ResourceTest : public MeeCoreTest
    {
    protected:
        void SetUp() override
        {
            MeeCoreTest::SetUp();

            // Release resources
            mc_resource_release(&manager, &x);
            mc_resource_release(&manager, &y);
            mc_resource_release(&manager, &z);
        }
    };

    TEST_F(ResourceTest, AcquireGetsFreeResource)
    {
        // Acquire Key 10
        int *res = (int *)mc_resource_acquire(&manager, 10);

        // Should get the first item
        EXPECT_TRUE(res != NULL);
        EXPECT_EQ(x, *res);
        EXPECT_TRUE(pool[0].is_busy);
        EXPECT_EQ(10, pool[0].key);

        // Acquire Key 11
        res = (int *)mc_resource_acquire(&manager, 11);

        // Should get the second item
        EXPECT_TRUE(res != NULL);
        EXPECT_EQ(y, *res);
        EXPECT_TRUE(pool[1].is_busy);
        EXPECT_EQ(11, pool[1].key);
    }

    TEST_F(ResourceTest, AcquireReturnsExistingIfKeyMatches)
    {
        // Acquire Key 50 twice
        int *res1 = (int *)mc_resource_acquire(&manager, 50);
        int *res2 = (int *)mc_resource_acquire(&manager, 50);

        // Should return the same resource
        EXPECT_EQ(*res1, *res2);

        // Should NOT have used a second slot
        EXPECT_FALSE(pool[1].is_busy);
    }

    TEST_F(ResourceTest, AcquireReturnsNullIfFull)
    {
        // Acquire thrice
        mc_resource_acquire(&manager, 1);
        mc_resource_acquire(&manager, 2);
        mc_resource_acquire(&manager, 3);

        // Try fourth acquire. Should be NULL.
        EXPECT_TRUE(mc_resource_acquire(&manager, 4) == NULL);
    }

    TEST_F(ResourceTest, GetReturnsExistingIfKeyMatches)
    {
        // Acquire then get
        mc_resource_acquire(&manager, 50);
        int *res = (int *)mc_resource_get(&manager, 50);
        EXPECT_EQ(x, *res);
    }

    TEST_F(ResourceTest, GetReturnsNullIfKeyMissing)
    {
        int *res = (int *)mc_resource_get(&manager, 50);
        EXPECT_TRUE(res == NULL);
    }

    TEST_F(ResourceTest, ReleaseByResourceFreesSlot)
    {
        mc_resource_acquire(&manager, 1);
        mc_resource_acquire(&manager, 2);

        // Release x
        mc_resource_release(&manager, &x);

        // Slot 1 is now free
        EXPECT_FALSE(pool[0].is_busy);
        EXPECT_TRUE(pool[1].is_busy);

        // Another aquire should take slot 1
        mc_resource_acquire(&manager, 3);
        EXPECT_TRUE(pool[0].is_busy);
    }

    TEST_F(ResourceTest, ReleaseByKeyFreesSlot)
    {
        mc_resource_acquire(&manager, 1);

        // Release key 1
        mc_resource_release_key(&manager, 1);

        // Slot 1 is now free
        EXPECT_FALSE(pool[0].is_busy);

        // Release non-existant key should do nothing
        mc_resource_release_key(&manager, 999);
    }

    TEST_F(ResourceTest, AssertDeathIfNull)
    {
        EXPECT_ANY_THROW(mc_resource_acquire(NULL, 1));
        EXPECT_ANY_THROW(mc_resource_get(NULL, 1));
        EXPECT_ANY_THROW(mc_resource_release(NULL, &x));
        EXPECT_ANY_THROW(mc_resource_release_key(NULL, 1));
    }
}