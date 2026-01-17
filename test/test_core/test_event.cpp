#include <gtest/gtest.h>
#include "mc_test.h"
#include "mc/event.h"

namespace
{
    // Custom data struct. Contains pointer to even and callback so that it can
    // unregister itself.
    typedef struct
    {
        int num;
        mc_event_t *event;
        mc_callback_t *callback;
    } my_event_data_t;

    // Test callback. Increment num by a certain amount.
    void add_to_num(void *context, void *arg)
    {
        my_event_data_t *data = (my_event_data_t *)context;
        int *increment = (int *)arg;
        data->num += *increment;
    }

    // Test callback. Same as above, but then unregister itself.
    void add_to_num_and_unregister(void *context, void *arg)
    {
        add_to_num(context, arg);
        my_event_data_t *data = (my_event_data_t *)context;
        mc_event_unregister(data->event, data->callback);
    }

    // Globals
    static my_event_data_t data1;
    static my_event_data_t data2;
    static my_event_data_t data3;
    MC_DEFINE_EVENT(event);
    MC_DEFINE_CALLBACK(cb1, add_to_num, data1);
    // Only cb2 unregisters itself
    MC_DEFINE_CALLBACK(cb2, add_to_num_and_unregister, data2);
    MC_DEFINE_CALLBACK(cb3, add_to_num, data3);

    class EventTest : public MeeCoreTest
    {
    protected:
        void SetUp() override
        {
            MeeCoreTest::SetUp();
            mc_event_init(&event);
            InitializeData(data1, cb1);
            InitializeData(data2, cb2);
            InitializeData(data3, cb3);
        }

    private:
        void InitializeData(my_event_data_t &data, mc_callback_t &cb)
        {
            data.num = 0;
            data.event = &event;
            data.callback = &cb;
        }
    };

    TEST_F(EventTest, InitResetsEvent)
    {
        mc_event_register(&event, &cb1);
        EXPECT_NE(0, event.listeners.count);

        // Init should reset state.
        mc_event_init(&event);
        EXPECT_EQ(0, event.listeners.count);
    }

    TEST_F(EventTest, InitCallbackSucceeds)
    {
        // Local cb
        mc_callback_t cb;
        mc_callback_init(&cb, add_to_num, &data1);
        EXPECT_EQ(cb.ctx, &data1);
        EXPECT_EQ(cb.func, add_to_num);
    }

    TEST_F(EventTest, TriggerInvokesCallbacks)
    {
        // Register and trigger event.
        int increment = 4;
        mc_event_register(&event, &cb1);
        mc_event_trigger(&event, &increment);
        EXPECT_EQ(4, data1.num);

        // Trigger event again.
        mc_event_trigger(&event, &increment);
        EXPECT_EQ(8, data1.num);
    }

    TEST_F(EventTest, UnregisterDuringEventTriggerSucceeds)
    {
        // Register and trigger event. cb2 should unregister itself.
        int increment = 4;
        mc_event_register(&event, &cb1);
        mc_event_register(&event, &cb2);
        mc_event_register(&event, &cb3);

        mc_event_trigger(&event, &increment);
        EXPECT_EQ(4, data1.num);
        EXPECT_EQ(4, data2.num);
        EXPECT_EQ(4, data3.num);

        // Trigger event again. This time, data2 should be unchanged.
        mc_event_trigger(&event, &increment);
        EXPECT_EQ(8, data1.num);
        EXPECT_EQ(8, data3.num);

        // cb2 should be unregistered, so data2.num should remain 4
        EXPECT_EQ(4, data2.num);
    }

    TEST_F(EventTest, AssertDeathIfNull)
    {
        int increment = 4;
        EXPECT_ANY_THROW(mc_event_init(NULL));
        EXPECT_ANY_THROW(mc_callback_init(NULL, add_to_num, &data1));
        EXPECT_ANY_THROW(mc_event_register(NULL, &cb1));
        EXPECT_ANY_THROW(mc_event_register(&event, NULL));
        EXPECT_ANY_THROW(mc_event_unregister(NULL, &cb1));
        EXPECT_ANY_THROW(mc_event_unregister(&event, NULL));
        EXPECT_ANY_THROW(mc_event_trigger(NULL, &increment));
    }
}
