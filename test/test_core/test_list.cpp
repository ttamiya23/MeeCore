#include <gtest/gtest.h>
#include "mc/list.h"
#include "mc/utils.h"
#include "mc_test.h"

namespace
{
    // Simple data struct that just contains ID.
    typedef struct
    {
        int id;
        mc_node_t node;
    } my_list_data_t;

    // Globals
    MC_DEFINE_LIST(list);
    static my_list_data_t item1 = {0};
    static my_list_data_t item2 = {0};
    static my_list_data_t item3 = {0};

    class ListTest : public MeeCoreTest
    {
    protected:
        void SetUp() override
        {
            MeeCoreTest::SetUp();
            list = {0};
            item1 = {.id = 10, .node = {0}};
            item2 = {.id = 20, .node = {0}};
            item3 = {.id = 30, .node = {0}};
        }
    };

    TEST_F(ListTest, InitZerosOutList)
    {
        EXPECT_EQ(nullptr, list.head);
        EXPECT_EQ(nullptr, list.tail);
        EXPECT_EQ(0, mc_list_count(&list));
    }

    TEST_F(ListTest, AppendAddsToEnd)
    {
        mc_list_append(&list, &item1.node);

        // Assert: Head and Tail are Item 1
        EXPECT_EQ(&item1.node, mc_list_peek_head(&list));
        EXPECT_EQ(&item1.node, mc_list_peek_tail(&list));
        EXPECT_EQ(1, mc_list_count(&list));

        // Add Item 2
        mc_list_append(&list, &item2.node);

        // Assert: Head is still 1, Tail is now 2
        EXPECT_EQ(&item1.node, mc_list_peek_head(&list));
        EXPECT_EQ(&item2.node, mc_list_peek_tail(&list));
        EXPECT_EQ(2, mc_list_count(&list));

        // Verify Links
        EXPECT_EQ(&item2.node, item1.node.next); // 1 -> 2
        EXPECT_EQ(&item1.node, item2.node.prev); // 2 <- 1
    }

    TEST_F(ListTest, PrependAddsToFront)
    {
        mc_list_prepend(&list, &item1.node); // List: [1]

        // Assert: Head and Tail are Item 1
        EXPECT_EQ(&item1.node, mc_list_peek_head(&list));
        EXPECT_EQ(&item1.node, mc_list_peek_tail(&list));
        EXPECT_EQ(1, mc_list_count(&list));

        // Prepend Item 2
        mc_list_prepend(&list, &item2.node); // List: [2, 1]

        // Assert
        EXPECT_EQ(&item2.node, mc_list_peek_head(&list));
        EXPECT_EQ(&item1.node, mc_list_peek_tail(&list));

        // Verify Links
        EXPECT_EQ(&item1.node, item2.node.next); // 2 -> 1
        EXPECT_EQ(&item2.node, item1.node.prev); // 1 <- 2
    }

    TEST_F(ListTest, RemoveHandleSucceeds)
    {
        mc_list_append(&list, &item1.node);
        mc_list_append(&list, &item2.node);
        mc_list_append(&list, &item3.node);

        // Remove middle
        mc_list_remove(&list, &item2.node);

        // Assert: List is [1, 3]
        EXPECT_EQ(2, mc_list_count(&list));
        EXPECT_EQ(&item1.node, list.head);
        EXPECT_EQ(&item3.node, list.tail);

        // Verify Links (1 should point to 3)
        EXPECT_EQ(&item3.node, item1.node.next);
        EXPECT_EQ(&item1.node, item3.node.prev);

        // Remove head
        mc_list_remove(&list, &item1.node);

        // Assert: Head is now 3
        EXPECT_EQ(&item3.node, list.head);
        EXPECT_EQ(nullptr, item3.node.prev); // New head has no prev

        // Remove last item
        mc_list_remove(&list, &item3.node);

        // Assert: Head and tail are now null
        EXPECT_EQ(nullptr, list.head);
        EXPECT_EQ(nullptr, list.tail);
        EXPECT_EQ(0, mc_list_count(&list));
    }

    TEST_F(ListTest, RemoveHandlesDetachedNode)
    {
        mc_list_append(&list, &item1.node);
        mc_list_append(&list, &item2.node);
        // Don't add item 3

        // Remove item 3, which should do nothing
        mc_list_remove(&list, &item3.node);

        // Assert: List is [1, 2]
        EXPECT_EQ(2, mc_list_count(&list));
        EXPECT_EQ(&item1.node, list.head);
        EXPECT_EQ(&item2.node, list.tail);
    }

    TEST_F(ListTest, MacroEntryRecoversParentStruct)
    {
        mc_list_append(&list, &item1.node);

        // Get the generic node pointer from the list
        mc_node_t *node_ptr = mc_list_peek_head(&list);

        // Use the macro to recover the parent 'my_list_data_t'
        my_list_data_t *recovered_item = MC_LIST_ENTRY(node_ptr, my_list_data_t, node);

        // Assert it matches the original object
        EXPECT_EQ(&item1, recovered_item);
        EXPECT_EQ(10, recovered_item->id);
    }

    TEST_F(ListTest, MacroEntryIteratesList)
    {
        mc_list_append(&list, &item1.node);
        mc_list_append(&list, &item2.node);
        mc_list_append(&list, &item3.node);

        int i = 0;
        my_list_data_t *expected[] = {&item1, &item2, &item3};
        mc_node_t *curr;

        MC_LIST_FOR_EACH(curr, &list)
        {
            // Assert it matches the original object
            EXPECT_EQ(&expected[i]->node, curr);
            i++;
        }
    }

    TEST_F(ListTest, MacroEntryIteratesListSafely)
    {
        mc_list_append(&list, &item1.node);
        mc_list_append(&list, &item2.node);
        mc_list_append(&list, &item3.node);

        int i = 0;
        my_list_data_t *expected[] = {&item1, &item2, &item3};
        mc_node_t *curr, *temp;

        MC_LIST_FOR_EACH_SAFE(curr, temp, &list)
        {
            // Assert it matches the original object
            EXPECT_EQ(&expected[i]->node, curr);
            // Remove middle element
            if (i == 1)
            {
                mc_list_remove(&list, curr);
            }
            i++;
        }
        // Expect list to be [1, 3]
        EXPECT_EQ(2, mc_list_count(&list));
        EXPECT_EQ(&item1.node, mc_list_peek_head(&list));
        EXPECT_EQ(&item3.node, mc_list_peek_tail(&list));
    }

    TEST_F(ListTest, PeekReturnsNullIfEmpty)
    {
        EXPECT_EQ(nullptr, mc_list_peek_head(&list));
        EXPECT_EQ(nullptr, mc_list_peek_tail(&list));
    }

    TEST_F(ListTest, PeekReturnsNodesWithoutRemoving)
    {
        mc_list_append(&list, &item1.node);
        mc_list_append(&list, &item2.node);

        mc_node_t *head = mc_list_peek_head(&list);
        EXPECT_EQ(&item1.node, head);

        mc_node_t *tail = mc_list_peek_tail(&list);
        EXPECT_EQ(&item2.node, tail);

        // Assert: List size must NOT change
        EXPECT_EQ(2, mc_list_count(&list));
    }

    TEST_F(ListTest, PopHeadReturnsNullIfEmpty)
    {
        mc_node_t *popped = mc_list_pop_head(&list);

        // Assert
        EXPECT_EQ(nullptr, popped);
        EXPECT_EQ(0, mc_list_count(&list));
    }

    TEST_F(ListTest, PopHeadRemovesFirstItem)
    {
        mc_list_append(&list, &item1.node);
        mc_list_append(&list, &item2.node);
        mc_list_append(&list, &item3.node);

        // Pop Item 1
        mc_node_t *popped1 = mc_list_pop_head(&list);
        EXPECT_EQ(&item1.node, popped1);
        EXPECT_EQ(&item2.node, list.head);
        EXPECT_EQ(2, mc_list_count(&list));

        // Pop Item 2
        mc_node_t *popped2 = mc_list_pop_head(&list);
        EXPECT_EQ(&item2.node, popped2);
        EXPECT_EQ(&item3.node, list.head);
        EXPECT_EQ(1, mc_list_count(&list));

        // Pop Item 3
        mc_node_t *popped3 = mc_list_pop_head(&list);
        EXPECT_EQ(&item3.node, popped3);
        EXPECT_EQ(nullptr, list.head);
        EXPECT_EQ(0, mc_list_count(&list));
    }

    TEST_F(ListTest, PopTailRemovesLastItem)
    {
        mc_list_append(&list, &item1.node);
        mc_list_append(&list, &item2.node);
        mc_list_append(&list, &item3.node);

        // Pop Item 3
        mc_node_t *popped3 = mc_list_pop_tail(&list);
        EXPECT_EQ(&item3.node, popped3);
        EXPECT_EQ(&item2.node, list.tail);
        EXPECT_EQ(2, mc_list_count(&list));

        // Pop Item 2
        mc_node_t *popped2 = mc_list_pop_tail(&list);
        EXPECT_EQ(&item2.node, popped2);
        EXPECT_EQ(&item1.node, list.tail);
        EXPECT_EQ(1, mc_list_count(&list));

        // Pop Item 1
        mc_node_t *popped1 = mc_list_pop_head(&list);
        EXPECT_EQ(&item1.node, popped1);
        EXPECT_EQ(nullptr, list.tail);
        EXPECT_EQ(0, mc_list_count(&list));
    }

    TEST_F(ListTest, IterationWalksList)
    {
        mc_list_append(&list, &item1.node);
        mc_list_append(&list, &item2.node);
        mc_list_append(&list, &item3.node);

        mc_node_t *curr = mc_list_peek_head(&list);

        // First node
        EXPECT_EQ(&item1.node, curr);

        // Next
        curr = mc_list_next(curr);
        EXPECT_EQ(&item2.node, curr);

        // Next
        curr = mc_list_next(curr);
        EXPECT_EQ(&item3.node, curr);

        // Next (Should be NULL)
        EXPECT_EQ(nullptr, mc_list_next(curr));

        // Prev
        curr = mc_list_prev(curr);
        EXPECT_EQ(&item2.node, curr);

        // Prev
        curr = mc_list_prev(curr);
        EXPECT_EQ(&item1.node, curr);

        // Prev (Should be NULL)
        EXPECT_EQ(nullptr, mc_list_prev(curr));
    }

    TEST_F(ListTest, AppendAssertDeathIfListIsNull)
    {
        EXPECT_ANY_THROW(mc_list_append(NULL, &item1.node));
    }
}
