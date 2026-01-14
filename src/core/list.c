#include "mc/list.h"
#include "mc/utils.h"

void mc_list_append(mc_list_t *list, mc_node_t *node)
{
    MC_ASSERT(list != NULL);
    MC_ASSERT(node != NULL);
    node->next = NULL;
    node->prev = list->tail;

    if (list->count == 0)
    {
        // List was empty, this is now head AND tail
        list->head = node;
        list->tail = node;
    }
    else
    {
        // List had items, update the old tail
        list->tail->next = node;
        list->tail = node;
    }

    list->count++;
}

void mc_list_prepend(mc_list_t *list, mc_node_t *node)
{
    MC_ASSERT(list != NULL);
    MC_ASSERT(node != NULL);
    node->prev = NULL;
    node->next = list->head;

    if (list->count == 0)
    {
        // List was empty, this is now head AND tail
        list->head = node;
        list->tail = node;
    }
    else
    {
        // List had items, update the old head
        list->head->prev = node;
        list->head = node;
    }

    list->count++;
}

void mc_list_remove(mc_list_t *list, mc_node_t *node)
{
    MC_ASSERT(list != NULL);
    MC_ASSERT(node != NULL);
    // Safety check: Don't remove if list is empty
    if (list->count == 0)
    {
        return;
    }

    // 1. Handle "Previous" links
    if (node->prev == NULL)
    {
        if (list->head != node)
        {
            // Node is detached or belongs to a different list. Abort.
            return;
        }
        // This was the head
        list->head = node->next;
    }
    else
    {
        // Not the head, bridge the gap
        node->prev->next = node->next;
    }

    // 2. Handle "Next" links
    if (node->next == NULL)
    {
        if (list->tail != node)
        {
            // Node is detached or belongs to a different list. Abort.
            return;
        }
        // This was the tail
        list->tail = node->prev;
    }
    else
    {
        // Not the tail, bridge the gap
        node->next->prev = node->prev;
    }

    // 3. Cleanup the removed node
    node->next = NULL;
    node->prev = NULL;
    if (list->count > 0)
    {
        list->count--;
    }
}

mc_node_t *mc_list_pop_head(mc_list_t *list)
{
    MC_ASSERT(list != NULL);
    mc_node_t *node = list->head;
    if (node != NULL)
    {
        mc_list_remove(list, node);
    }
    return node;
}

mc_node_t *mc_list_pop_tail(mc_list_t *list)
{
    MC_ASSERT(list != NULL);
    mc_node_t *node = list->tail;
    if (node != NULL)
    {
        mc_list_remove(list, node);
    }
    return node;
}

mc_node_t *mc_list_peek_head(mc_list_t *list)
{
    MC_ASSERT(list != NULL);
    return list->head;
}

mc_node_t *mc_list_peek_tail(mc_list_t *list)
{
    MC_ASSERT(list != NULL);
    return list->tail;
}

mc_node_t *mc_list_next(mc_node_t *node)
{
    if (node == NULL)
    {
        return NULL;
    }
    return node->next;
}

mc_node_t *mc_list_prev(mc_node_t *node)
{
    if (node == NULL)
    {
        return NULL;
    }
    return node->prev;
}

uint32_t mc_list_count(mc_list_t *list)
{
    MC_ASSERT(list != NULL);
    return list->count;
}