#include "unity.h"
#include "mc/event.h"
#include "mc/list.h"
#include "mock_utils.h" // Needed for assert_helper.h
#include "assert_helper.h"

// Test struct for this test.
typedef struct
{
    int num;
    mc_event_t *event;
    mc_callback_t *callback;
} my_data_t;

// Helper function to initialize my_data.
void my_data_init(my_data_t *data, mc_event_t *event, mc_callback_t *callback)
{
    data->num = 0;
    data->event = event;
    data->callback = callback;
}

// Test callback. Increment num by a certain amount.
void add_to_num(void *context, void *arg)
{
    my_data_t *data = (my_data_t *)context;
    int *increment = (int *)arg;
    data->num += *increment;
}

// Test callback. Same as above, but then unregister itself.
void add_to_num_and_unregister(void *context, void *arg)
{
    add_to_num(context, arg);
    my_data_t *data = (my_data_t *)context;
    mc_event_unregister(data->event, data->callback);
}

void test_event_trigger_should_invoke_callbacks(void)
{
    mc_event_t event;
    mc_callback_t callback;
    my_data_t data;
    my_data_init(&data, &event, &callback);
    mc_event_init(&event);
    mc_callback_init(&callback, add_to_num, &data);

    // Register and trigger event.
    int increment = 4;
    mc_event_register(&event, &callback);
    mc_event_trigger(&event, &increment);
    TEST_ASSERT_EQUAL_INT(4, data.num);

    // Trigger event again.
    mc_event_trigger(&event, &increment);
    TEST_ASSERT_EQUAL_INT(8, data.num);
}

void test_unregister_during_event_trigger_should_be_safe(void)
{
    mc_event_t event;
    mc_callback_t callback_1;
    mc_callback_t callback_2;
    mc_callback_t callback_3;
    my_data_t data_1;
    my_data_t data_2;
    my_data_t data_3;
    my_data_init(&data_1, &event, &callback_1);
    my_data_init(&data_2, &event, &callback_2);
    my_data_init(&data_3, &event, &callback_3);
    mc_event_init(&event);
    mc_callback_init(&callback_1, add_to_num, &data_1);
    mc_callback_init(&callback_3, add_to_num, &data_3);
    // Callback 2 should unregister itself after being invoked.
    mc_callback_init(&callback_2, add_to_num_and_unregister, &data_2);

    // Register and trigger event.
    int increment = 4;
    mc_event_register(&event, &callback_1);
    mc_event_register(&event, &callback_2);
    mc_event_register(&event, &callback_3);
    mc_event_trigger(&event, &increment);
    TEST_ASSERT_EQUAL_INT(4, data_1.num);
    TEST_ASSERT_EQUAL_INT(4, data_2.num);
    TEST_ASSERT_EQUAL_INT(4, data_3.num);

    // Trigger event again.
    mc_event_trigger(&event, &increment);
    TEST_ASSERT_EQUAL_INT(8, data_1.num);
    TEST_ASSERT_EQUAL_INT(8, data_3.num);
    // Callback 2 should be unregistered, so data_2.num
    // should remain 4
    TEST_ASSERT_EQUAL_INT(4, data_2.num);
}

void test_should_assert_if_event_is_null(void)
{
    mc_callback_t callback;
    my_data_t data;
    int increment = 4;
    my_data_init(&data, NULL, &callback);
    mc_callback_init(&callback, add_to_num, &data);

    // Pass NULL to assert failure
    TEST_ASSERT_DEATH(mc_event_init(NULL));
    TEST_ASSERT_DEATH(mc_event_register(NULL, &callback));
    TEST_ASSERT_DEATH(mc_event_unregister(NULL, &callback));
    TEST_ASSERT_DEATH(mc_event_trigger(NULL, &increment));
}

void test_should_assert_if_callback_is_null(void)
{
    mc_event_t event;
    my_data_t data;
    my_data_init(&data, &event, NULL);
    mc_event_init(&event);

    // Pass NULL to assert failure
    TEST_ASSERT_DEATH(mc_callback_init(NULL, add_to_num, &data));
    TEST_ASSERT_DEATH(mc_event_register(&event, NULL));
    TEST_ASSERT_DEATH(mc_event_unregister(&event, NULL));
}

void test_should_assert_if_event_not_initialized(void)
{
    mc_event_t event;
    mc_callback_t callback;
    my_data_t data;
    int increment = 4;
    my_data_init(&data, &event, &callback);
    mc_callback_init(&callback, add_to_num, &data);

    // Pass NULL to assert failure
    TEST_ASSERT_DEATH(mc_event_register(&event, &callback));
    TEST_ASSERT_DEATH(mc_event_unregister(&event, &callback));
    TEST_ASSERT_DEATH(mc_event_trigger(&event, &increment));
}

void test_should_assert_if_callback_not_initialized(void)
{
    mc_event_t event;
    mc_callback_t callback;
    my_data_t data;
    int increment = 4;
    my_data_init(&data, &event, &callback);
    mc_event_init(&event);

    // Pass NULL to assert failure
    TEST_ASSERT_DEATH(mc_event_register(&event, &callback));
    TEST_ASSERT_DEATH(mc_event_unregister(&event, &callback));
}
