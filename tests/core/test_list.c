#include "unity.h"
#include "mc/list.h"
#include "utils.h" // Needed for assert_helper.h
#include "assert_helper.h"

typedef struct
{
    int id;
    mc_node_t node; // The intrusive link
} my_data_t;

void setUp()
{
    test_assert_init();
}

void test_init_zeros_out_list(void)
{
    mc_list_t list;
    mc_list_init(&list);
    TEST_ASSERT_NULL(list.head);
    TEST_ASSERT_NULL(list.tail);
    TEST_ASSERT_EQUAL_UINT32(0, mc_list_count(&list));
}

void test_append_adds_to_end(void)
{
    mc_list_t list;
    my_data_t item1 = {.id = 10};
    mc_list_init(&list);
    mc_list_append(&list, &item1.node);

    // Assert: Head and Tail are Item 1
    TEST_ASSERT_EQUAL_PTR(&item1.node, mc_list_peek_head(&list));
    TEST_ASSERT_EQUAL_PTR(&item1.node, mc_list_peek_tail(&list));
    TEST_ASSERT_EQUAL_UINT32(1, mc_list_count(&list));

    // Add Item 2
    my_data_t item2 = {.id = 20};
    mc_list_append(&list, &item2.node);

    // Assert: Head is still 1, Tail is now 2
    TEST_ASSERT_EQUAL_PTR(&item1.node, mc_list_peek_head(&list));
    TEST_ASSERT_EQUAL_PTR(&item2.node, mc_list_peek_tail(&list));
    TEST_ASSERT_EQUAL_UINT32(2, mc_list_count(&list));

    // Verify Links
    TEST_ASSERT_EQUAL_PTR(&item2.node, item1.node.next); // 1 -> 2
    TEST_ASSERT_EQUAL_PTR(&item1.node, item2.node.prev); // 2 <- 1
}

void test_prepend_adds_to_front(void)
{
    mc_list_t list;
    mc_list_init(&list);
    my_data_t item1 = {.id = 10};
    mc_list_prepend(&list, &item1.node); // List: [1]

    // Assert: Head and Tail are Item 1
    TEST_ASSERT_EQUAL_PTR(&item1.node, mc_list_peek_head(&list));
    TEST_ASSERT_EQUAL_PTR(&item1.node, mc_list_peek_tail(&list));
    TEST_ASSERT_EQUAL_UINT32(1, mc_list_count(&list));

    // Prepend Item 2
    my_data_t item2 = {.id = 20};
    mc_list_prepend(&list, &item2.node); // List: [2, 1]

    // Assert
    TEST_ASSERT_EQUAL_PTR(&item2.node, mc_list_peek_head(&list));
    TEST_ASSERT_EQUAL_PTR(&item1.node, mc_list_peek_tail(&list));

    // Verify Links
    TEST_ASSERT_EQUAL_PTR(&item1.node, item2.node.next); // 2 -> 1
    TEST_ASSERT_EQUAL_PTR(&item2.node, item1.node.prev); // 1 <- 2
}

void test_remove_handle_succeeds(void)
{
    mc_list_t list;
    my_data_t item1 = {.id = 10};
    my_data_t item2 = {.id = 20};
    my_data_t item3 = {.id = 30};
    mc_list_init(&list);
    mc_list_append(&list, &item1.node);
    mc_list_append(&list, &item2.node);
    mc_list_append(&list, &item3.node);

    // Remove middle
    mc_list_remove(&list, &item2.node);

    // Assert: List is [1, 3]
    TEST_ASSERT_EQUAL_UINT32(2, mc_list_count(&list));
    TEST_ASSERT_EQUAL_PTR(&item1.node, list.head);
    TEST_ASSERT_EQUAL_PTR(&item3.node, list.tail);

    // Verify Links (1 should point to 3)
    TEST_ASSERT_EQUAL_PTR(&item3.node, item1.node.next);
    TEST_ASSERT_EQUAL_PTR(&item1.node, item3.node.prev);

    // Remove head
    mc_list_remove(&list, &item1.node);

    // Assert: Head is now 3
    TEST_ASSERT_EQUAL_PTR(&item3.node, list.head);
    TEST_ASSERT_NULL(item3.node.prev); // New head has no prev

    // Remove last item
    mc_list_remove(&list, &item3.node);

    // Assert: Head and tail are now null
    TEST_ASSERT_NULL(list.head);
    TEST_ASSERT_NULL(list.tail);
    TEST_ASSERT_EQUAL_UINT32(0, mc_list_count(&list));
}

void test_remove_handles_detatched_node(void)
{
    mc_list_t list;
    my_data_t item1 = {.id = 10};
    my_data_t item2 = {.id = 20};
    my_data_t item3 = {.id = 30};
    mc_list_init(&list);
    mc_list_append(&list, &item1.node);
    mc_list_append(&list, &item2.node);
    // Don't add item 3

    // Remove item 3, which should do nothing
    mc_list_remove(&list, &item3.node);

    // Assert: List is [1, 2]
    TEST_ASSERT_EQUAL_UINT32(2, mc_list_count(&list));
    TEST_ASSERT_EQUAL_PTR(&item1.node, list.head);
    TEST_ASSERT_EQUAL_PTR(&item2.node, list.tail);
}

void test_macro_entry_recovers_parent_struct(void)
{
    mc_list_t list;
    my_data_t item1 = {.id = 10};
    mc_list_init(&list);
    mc_list_append(&list, &item1.node);

    // Get the generic node pointer from the list
    mc_node_t *node_ptr = mc_list_peek_head(&list);

    // Use the macro to recover the parent 'my_data_t'
    my_data_t *recovered_item = MC_LIST_ENTRY(node_ptr, my_data_t, node);

    // Assert it matches the original object
    TEST_ASSERT_EQUAL_PTR(&item1, recovered_item);
    TEST_ASSERT_EQUAL_INT(10, recovered_item->id);
}

void test_macro_entry_iterates_list(void)
{
    mc_list_t list;
    my_data_t item1 = {.id = 10};
    my_data_t item2 = {.id = 20};
    my_data_t item3 = {.id = 30};
    mc_list_init(&list);
    mc_list_append(&list, &item1.node);
    mc_list_append(&list, &item2.node);
    mc_list_append(&list, &item3.node);
    int i = 0;
    my_data_t *expected[] = {&item1, &item2, &item3};
    mc_node_t *curr;
    MC_LIST_FOR_EACH(curr, &list)
    {
        // Assert it matches the original object
        TEST_ASSERT_EQUAL_PTR(&expected[i]->node, curr);
        i++;
    }
}

void test_macro_entry_iterates_list_safely(void)
{
    mc_list_t list;
    my_data_t item1 = {.id = 10};
    my_data_t item2 = {.id = 20};
    my_data_t item3 = {.id = 30};
    mc_list_init(&list);
    mc_list_append(&list, &item1.node);
    mc_list_append(&list, &item2.node);
    mc_list_append(&list, &item3.node);
    int i = 0;
    my_data_t *expected[] = {&item1, &item2, &item3};
    mc_node_t *curr, *temp;

    MC_LIST_FOR_EACH_SAFE(curr, temp, &list)
    {
        // Assert it matches the original object
        TEST_ASSERT_EQUAL_PTR(&expected[i]->node, curr);
        // Remove middle element
        if (i == 1)
        {
            mc_list_remove(&list, curr);
        }
        i++;
    }
    // Expect list to be [1, 3]
    TEST_ASSERT_EQUAL_UINT32(2, mc_list_count(&list));
    TEST_ASSERT_EQUAL_PTR(&item1.node, mc_list_peek_head(&list));
    TEST_ASSERT_EQUAL_PTR(&item3.node, mc_list_peek_tail(&list));
}

void test_peek_returns_null_if_empty(void)
{
    mc_list_t list;
    mc_list_init(&list);

    TEST_ASSERT_NULL(mc_list_peek_head(&list));
    TEST_ASSERT_NULL(mc_list_peek_tail(&list));
}

void test_peek_returns_nodes_without_removing(void)
{
    mc_list_t list;
    my_data_t item1 = {.id = 10};
    my_data_t item2 = {.id = 20};
    mc_list_init(&list);
    mc_list_append(&list, &item1.node);
    mc_list_append(&list, &item2.node);

    mc_node_t *head = mc_list_peek_head(&list);
    TEST_ASSERT_EQUAL_PTR(&item1.node, head);

    mc_node_t *tail = mc_list_peek_tail(&list);
    TEST_ASSERT_EQUAL_PTR(&item2.node, tail);

    // Assert: List size must NOT change
    TEST_ASSERT_EQUAL_UINT32(2, mc_list_count(&list));
}

void test_pop_head_returns_null_if_empty(void)
{
    mc_list_t list;
    mc_list_init(&list);
    mc_node_t *popped = mc_list_pop_head(&list);

    // Assert
    TEST_ASSERT_NULL(popped);
    TEST_ASSERT_EQUAL_UINT32(0, mc_list_count(&list));
}

void test_pop_head_removes_first_item(void)
{
    mc_list_t list;
    my_data_t item1 = {.id = 10};
    my_data_t item2 = {.id = 20};
    my_data_t item3 = {.id = 30};
    mc_list_init(&list);
    mc_list_append(&list, &item1.node);
    mc_list_append(&list, &item2.node);
    mc_list_append(&list, &item3.node);

    // Pop Item 1
    mc_node_t *popped1 = mc_list_pop_head(&list);
    TEST_ASSERT_EQUAL_PTR(&item1.node, popped1);
    TEST_ASSERT_EQUAL_PTR(&item2.node, list.head);
    TEST_ASSERT_EQUAL_UINT32(2, mc_list_count(&list));

    // Pop Item 2
    mc_node_t *popped2 = mc_list_pop_head(&list);
    TEST_ASSERT_EQUAL_PTR(&item2.node, popped2);
    TEST_ASSERT_EQUAL_PTR(&item3.node, list.head);
    TEST_ASSERT_EQUAL_UINT32(1, mc_list_count(&list));

    // Pop Item 3
    mc_node_t *popped3 = mc_list_pop_head(&list);
    TEST_ASSERT_EQUAL_PTR(&item3.node, popped3);
    TEST_ASSERT_NULL(list.head);
    TEST_ASSERT_EQUAL_UINT32(0, mc_list_count(&list));
}

void test_pop_tail_removes_first_item(void)
{
    mc_list_t list;
    my_data_t item1 = {.id = 10};
    my_data_t item2 = {.id = 20};
    my_data_t item3 = {.id = 30};
    mc_list_init(&list);
    mc_list_append(&list, &item1.node);
    mc_list_append(&list, &item2.node);
    mc_list_append(&list, &item3.node);

    // Pop Item 3
    mc_node_t *popped3 = mc_list_pop_tail(&list);
    TEST_ASSERT_EQUAL_PTR(&item3.node, popped3);
    TEST_ASSERT_EQUAL_PTR(&item2.node, list.tail);
    TEST_ASSERT_EQUAL_UINT32(2, mc_list_count(&list));

    // Pop Item 2
    mc_node_t *popped2 = mc_list_pop_tail(&list);
    TEST_ASSERT_EQUAL_PTR(&item2.node, popped2);
    TEST_ASSERT_EQUAL_PTR(&item1.node, list.tail);
    TEST_ASSERT_EQUAL_UINT32(1, mc_list_count(&list));

    // Pop Item 1
    mc_node_t *popped1 = mc_list_pop_head(&list);
    TEST_ASSERT_EQUAL_PTR(&item1.node, popped1);
    TEST_ASSERT_NULL(list.tail);
    TEST_ASSERT_EQUAL_UINT32(0, mc_list_count(&list));
}

void test_iteration_walks_list(void)
{
    mc_list_t list;
    my_data_t item1 = {.id = 10};
    my_data_t item2 = {.id = 20};
    my_data_t item3 = {.id = 30};
    mc_list_init(&list);
    mc_list_append(&list, &item1.node);
    mc_list_append(&list, &item2.node);
    mc_list_append(&list, &item3.node);

    mc_node_t *curr = mc_list_peek_head(&list);

    // First node
    TEST_ASSERT_EQUAL_PTR(&item1.node, curr);

    // Next
    curr = mc_list_next(curr);
    TEST_ASSERT_EQUAL_PTR(&item2.node, curr);

    // Next
    curr = mc_list_next(curr);
    TEST_ASSERT_EQUAL_PTR(&item3.node, curr);

    // Next (Should be NULL)
    TEST_ASSERT_NULL(mc_list_next(curr));

    // Prev
    curr = mc_list_prev(curr);
    TEST_ASSERT_EQUAL_PTR(&item2.node, curr);

    // Prev
    curr = mc_list_prev(curr);
    TEST_ASSERT_EQUAL_PTR(&item1.node, curr);

    // Prev (Should be NULL)
    TEST_ASSERT_NULL(mc_list_prev(curr));
}

void test_append_assert_death_if_list_is_null(void)
{
    my_data_t item1 = {.id = 10};

    // Pass NULL to assert failure
    TEST_ASSERT_DEATH(mc_list_append(NULL, &item1.node));
}

void test_append_assert_death_if_list_not_initialized(void)
{
    mc_list_t list;
    my_data_t item1 = {.id = 10};

    // Pass uninitialized list to assert failure
    TEST_ASSERT_DEATH(mc_list_append(&list, &item1.node));
}