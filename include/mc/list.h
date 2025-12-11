#ifndef MC_LIST_H_
#define MC_LIST_H_

#ifdef __cplusplus
extern "C"
{
#endif

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h> // For offsetof

#define MC_LIST_INIT_DEFAULT {.head = NULL, .tail = NULL, .count = 0, .is_initialized = 1}

/** Macro to get the parent struct from the node pointer
 *  Example:
 * * @code
 * // 1. Define a struct that contains a node
 * typedef struct {
 * int id;
 * char name[10];
 * mc_node_t node; // Embedded link
 * } my_task_t;
 *
 * * // 2. Iterate through the list
 * mc_node_t *curr;
 * MC_LIST_FOR_EACH(curr, &scheduler_list) {
 * * // 3. Recover the parent task from the generic node pointer
 * my_task_t *task = MC_LIST_ENTRY(curr, my_task_t, node);
 * * printf("Processing task: %s\n", task->name);
 * }
 * @endcode
 */
#define MC_LIST_ENTRY(ptr, type, member) \
    ((type *)((char *)(ptr) - offsetof(type, member)))

/* For each loop. */
#define MC_LIST_FOR_EACH(pos, list) \
    for (pos = mc_list_peek_head(list); pos != NULL; pos = mc_list_next(pos))

    typedef struct mc_node_t
    {
        struct mc_node_t *next;
        struct mc_node_t *prev;
    } mc_node_t;

    typedef struct mc_list_t
    {
        mc_node_t *head;
        mc_node_t *tail;
        uint32_t count;
        uint8_t is_initialized;
    } mc_list_t;

    /* Initialize empty list. Same as doing `mc_list_t list = MC_LIST_INIT_DEFAULT;` */
    void mc_list_init(mc_list_t *list);

    /* Add a node to the end of the list. */
    void mc_list_append(mc_list_t *list, mc_node_t *node);

    /* Add a node to the front of the list. */
    void mc_list_prepend(mc_list_t *list, mc_node_t *node);

    /* Remove a node from the list. Note: to ensure O(1), we do NOT check if the
     * node actually exists in the list (only checked for easy cases, like head and
     * tail). */
    void mc_list_remove(mc_list_t *list, mc_node_t *node);

    /* Peek at head. */
    mc_node_t *mc_list_peek_head(mc_list_t *list);

    /* Peek at tail. */
    mc_node_t *mc_list_peek_tail(mc_list_t *list);

    /* Pop head. */
    mc_node_t *mc_list_pop_head(mc_list_t *list);

    /* Pop tail. */
    mc_node_t *mc_list_pop_tail(mc_list_t *list);

    /* Get the next node in the list. */
    mc_node_t *mc_list_next(mc_node_t *node);

    /* Get the previous node in the list. */
    mc_node_t *mc_list_prev(mc_node_t *node);

    /* Get count. */
    uint32_t mc_list_count(mc_list_t *list);

#ifdef __cplusplus
}
#endif

#endif /* MC_LIST_H_ */
