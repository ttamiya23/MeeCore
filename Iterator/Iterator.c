#include "Iterator.h"
#include "IteratorSettings.h"

/* Definition of Iterator struct */
#pragma pack(1)
typedef struct Iterator
{
    Node* start;
    Node* end;
    uint32 count;
} Iterator;

/* Definition of Node struct */
#pragma pack(1)
typedef struct Node
{
    void* data;
    Node* prev;
    Node* next;
} Node;

/* List of all iterators */
static Iterator iteratorList[ITERATOR_NUM];

/* Index of the first empty iterator. -1 if all iterators are used up */
static int16 emptyIteratorIndex = 0;

/* Variable to store which iterators are initialized */
static
#if ITERATOR_NUM <= 8
uint8
#elif ITERATOR_NUM <= 16
uint16
#elif ITERATOR_NUM <= 32
uint32
#else
uint64
#endif
initializedIterators = 0;

/* List of all nodes */
static Node nodeList[ITERATOR_NUM];

/* Index of the first empty node. -1 if all nodes are used up */
static int16 emptyNodeIndex = 0;

/* Variable to store which nodes are initialized */
static
#if NODE_NUM <= 8
uint8
#elif NODE_NUM <= 16
uint16
#elif NODE_NUM <= 32
uint32
#else
uint64
#endif
initializedNodes = 0;

/* Request an iterator. If no more space available, iterator is set to NULL and
 * returns ERROR */
STATUS iter_CreateIterator(Iterator** iterator)
{
    STATUS ret = ERROR;
    if (iterator == NULL)
        return ret;
    if (emptyIteratorIndex < 0)
    {
        *iterator = NULL;
        return ret;
    }

    // Initialize iterator
    (iteratorList + emptyIteratorIndex)->start = NULL;
    (iteratorList + emptyIteratorIndex)->end = NULL;
    (iteratorList + emptyIteratorIndex)->count = 0;
    *iterator = iteratorList + emptyIteratorIndex;

    SET_BIT(initializedIterators, emptyIteratorIndex);
    ret = SUCCESS;

    // Linearly search for empty iterator
    uint16 i, index;
    for (i = 1; i < ITERATOR_NUM; ++i)
    {
        index = (emptyIteratorIndex + i) % ITERATOR_NUM;
        if (!IS_BIT_SET(initializedIterators, index))
        {
            emptyIteratorIndex = index;
            return ret;
        }
    }

    // All iterators are used up!
    emptyIteratorIndex = -1;
    return ret;
}

/* Add new node to iterator. If no more space available or iterator is NULL,
 * returns ERROR */
STATUS iter_AddNode(Iterator* iterator, void* data)
{
    STATUS ret = ERROR;
    if (emptyNodeIndex < 0 || iterator == NULL)
        return ret;

    // Initialize node
    Node* newNode = nodeList + emptyNodeIndex;
    newNode->next = NULL;
    newNode->data = data;

    // If empty iterator, set new node to start and end
    if (!(iterator->count))
    {
        newNode->prev = NULL;
        iterator->start = newNode;
        iterator->end = newNode;
    }
    // Else if end is populated, set new node to its next
    else if (iterator->end != NULL)
    {
        newNode->prev = iterator->end;
        iterator->end->next = newNode;
        iterator->end = newNode;
    }
    // Weird case... Just return error. Should never happen
    else
    {
        return ret;
    }
    (iterator->count)++;

    SET_BIT(initializedNodes, emptyNodeIndex);
    ret = SUCCESS;

    // Linearly search for empty node
    uint16 i, index;
    for (i = 1; i < NODE_NUM; ++i)
    {
        index = (emptyNodeIndex + i) % NODE_NUM;
        if (!IS_BIT_SET(initializedNodes, index))
        {
            emptyNodeIndex = index;
            return ret;
        }
    }

    // All nodes are used up!
    emptyNodeIndex = -1;
    return ret;
}

/* Get start node. If iterator is NULL, node is set to NULL and returns ERROR */
STATUS iter_GetStart(Iterator* iterator, Node** node)
{
    STATUS ret = ERROR;
    if (node == NULL)
        return ret;
    if (iterator == NULL)
    {
        *node = NULL;
        return ret;
    }

    *node = iterator->start;
    ret = SUCCESS;
    return ret;
}

/* Get next node. If startNode has no next or startNode is NULL, nextNode is set
 * to NULL and returns ERROR*/
STATUS iter_GetNext(Node* startNode, Node** nextNode)
{
    STATUS ret = ERROR;
    if (nextNode == NULL)
        return ret;
    if (startNode == NULL || startNode->next == NULL)
    {
        *nextNode = NULL;
        return ret;
    }

    *nextNode = startNode->next;
    ret = SUCCESS;
    return ret;
}

/* Get count. If iterator or count is NULL, returns ERROR */
STATUS iter_GetCount(Iterator* iterator, uint16* count)
{
    STATUS ret = ERROR;
    if (iterator == NULL || count == NULL)
        return ret;

    *count = iterator->count;

    ret = SUCCESS;
    return ret;
}

/* Get data from node. If node is NULL, data is set to NULL and returns ERROR */
STATUS iter_GetData(Node* node, void** data)
{
    STATUS ret = ERROR;
    if (data == NULL)
        return ret;
    if (node == NULL)
    {
        *data = NULL;
        return ret;
    }

    *data = node->data;
    ret = SUCCESS;
    return ret;
}

/* Find node from data. If iterator or data is NULL, node is set to NULL and
 * returns ERROR */
STATUS iter_FindNode(Iterator* iterator, void* data, Node** node)
{
    STATUS ret = ERROR;
    if (node == NULL)
        return ret;
    if (iterator == NULL || data == NULL)
    {
        *node = NULL;
        return ret;
    }

    Node* currNode = iterator->start;
    while(currNode)
    {
        if (currNode->data == data)
        {
            *node = currNode;
            ret = SUCCESS;
            break;
        }
        currNode = currNode->next;
    }

    return ret;
}

/* Delete iterator and sets iterator to NULL. If iterator is NULL, returns
 * ERROR */
STATUS iter_DeleteIterator(Iterator** iterator)
{
    STATUS ret = ERROR;
    if (iterator == NULL || *iterator == NULL)
        return ret;

    // Delete all nodes in iterator
    Iterator* iteratorToDelete = *iterator;
    Node* nextNode;
    Node* currNode = iteratorToDelete->start;
    while(currNode)
    {
        nextNode = currNode->next;
        iter_DeleteNode(iteratorToDelete, &currNode);
        currNode = nextNode;
    }

    // Update emptyIteratorIndex and remove from initializedIterators
    emptyIteratorIndex = iteratorToDelete - iteratorList;
    UNSET_BIT(initializedIterators, emptyIteratorIndex);
    *iterator = NULL;
    ret = SUCCESS;
    return ret;
}

/* Delete node and sets node to NULL. If node is NULL or does not belong to
 * iterator, returns ERROR */
STATUS iter_DeleteNode(Iterator* iterator, Node** node)
{
    STATUS ret = ERROR;
    if (node == NULL || *node == NULL)
        return ret;

    // Check if node belongs to iterator. Can do so by comparing last nodes
    Node* nodeToDelete = *node;
    Node* currNode = nodeToDelete;
    Node* nextNode = currNode->next;
    while(nextNode)
    {
        currNode = nextNode;
        nextNode = currNode->next;
    }
    if (currNode != iterator->end)
        return ret;

    // If previous node exists, tie previous and next nodes together
    if (nodeToDelete->prev)
        nodeToDelete->prev->next = nodeToDelete->next;
    // Else, must be first node. Update iterator->start
    else
        iterator->start = nodeToDelete->next;

    // If next node exists, tie next and previous nodes together
    if (nodeToDelete->next)
        nodeToDelete->next->prev = nodeToDelete->prev;
    // Else, must be last node. Update iterator->end
    else
        iterator->end = nodeToDelete->prev;

    (iterator->count)--;

    // Update emptyNodeIndex and remove from initializedNodes
    emptyNodeIndex = nodeToDelete - nodeList;
    UNSET_BIT(initializedNodes, emptyNodeIndex);
    *node = NULL;
    ret = SUCCESS;
    return ret;
}
