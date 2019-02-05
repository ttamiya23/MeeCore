#include "Iterator.h"
#include "IteratorSettings.h"
#include "Debug/Debug.h"
#include "assert.h"

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
static Node nodeList[NODE_NUM];

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

/* Request an iterator */
STATUS iter_CreateIterator(Iterator** iterator)
{
    STATUS ret = ERROR;

    dbg_LogInformation("Initializing iterator[%i]", emptyIteratorIndex);

    assert(iterator != NULL);
    assert(emptyIteratorIndex >= 0);

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
            dbg_LogDebug("Next iterator: %i", emptyIteratorIndex);
            return ret;
        }
    }

    // All iterators are used up!
    dbg_LogDebug("All iterators used up");
    emptyIteratorIndex = -1;

    return ret;
}

/* Add new node to iterator */
STATUS iter_AddNode(Iterator* iterator, void* data)
{
    STATUS ret = ERROR;
    int16 id = iterator - iteratorList;

    dbg_LogInformation("Adding node[%i] to iterator[%i]", emptyNodeIndex, emptyIteratorIndex);

    assert(emptyNodeIndex >= 0);
    assert(iterator != NULL);
    assert(id >= 0);
    assert(id < ITERATOR_NUM);

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
    // Weird case... Cause error. Should never happen
    else
    {
        assert(TRUE);
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
            dbg_LogDebug("Next node: %i", emptyNodeIndex);
            emptyNodeIndex = index;
            return ret;
        }
    }

    // All nodes are used up!
    dbg_LogDebug("All nodes are used up");
    emptyNodeIndex = -1;
    return ret;
}

/* Get start node */
STATUS iter_GetStart(Iterator* iterator, Node** node)
{
    int16 id = iterator - iteratorList;

    assert(iterator != NULL);
    assert(node != NULL);
    assert(id >= 0);
    assert(id < ITERATOR_NUM);

    *node = iterator->start;

    return SUCCESS;
}

/* Get next node. If startNode has no next, nextNode is set to NULL and returns
 * ERROR*/
STATUS iter_GetNext(Node* startNode, Node** nextNode)
{
    int16 id = startNode - nodeList;

    assert(startNode != NULL);
    assert(nextNode != NULL);
    assert(id >= 0);
    assert(id < NODE_NUM);

    *nextNode = startNode->next;

    if (*nextNode != NULL)
        return SUCCESS;
    return ERROR;
}

/* Get count */
STATUS iter_GetCount(Iterator* iterator, uint16* count)
{
    int16 id = iterator - iteratorList;

    assert(iterator != NULL);
    assert(count != NULL);
    assert(id >= 0);
    assert(id < ITERATOR_NUM);

    *count = iterator->count;

    return SUCCESS;
}

/* Get data from node */
STATUS iter_GetData(Node* node, void** data)
{
    int16 id = node - nodeList;

    assert(data != NULL);
    assert(node != NULL);
    assert(id >= 0);
    assert(id < NODE_NUM);

    *data = node->data;

    return SUCCESS;
}

/* Find node from data. If not found, node is set to NULL and returns ERROR */
STATUS iter_FindNode(Iterator* iterator, void* data, Node** node)
{
    int16 id = iterator - iteratorList;

    assert(iterator != NULL);
    assert(data != NULL);
    assert(node != NULL);
    assert(id >= 0);
    assert(id < ITERATOR_NUM);

    Node* currNode = iterator->start;
    while(currNode)
    {
        if (currNode->data == data)
        {
            *node = currNode;
            return SUCCESS;
        }
        currNode = currNode->next;
    }

    return ERROR;
}

/* Delete iterator and sets iterator to NULL */
STATUS iter_DeleteIterator(Iterator** iterator)
{
    assert(iterator != NULL);
    assert(*iterator != NULL);

    int16 id = *iterator - iteratorList;

    dbg_LogInformation("Deleting iterator[%i]", id);

    assert(id >= 0);
    assert(id < ITERATOR_NUM);

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

    return SUCCESS;
}

/* Delete node and sets node to NULL */
STATUS iter_DeleteNode(Iterator* iterator, Node** node)
{
    assert(iterator != NULL);
    assert(node != NULL);
    assert(*node != NULL);

    int16 iteratorId = iterator - iteratorList;
    int16 nodeId = *node - nodeList;

    dbg_LogInformation("Deleting node[%i] in iterator[%i]", nodeId, iteratorId);

    assert(iteratorId >= 0);
    assert(iteratorId < ITERATOR_NUM);
    assert(nodeId >= 0);
    assert(nodeId < NODE_NUM);

    // Check if node belongs to iterator. Can do so by comparing last nodes
    Node* nodeToDelete = *node;
    Node* currNode = nodeToDelete;
    Node* nextNode = currNode->next;
    while(nextNode)
    {
        currNode = nextNode;
        nextNode = currNode->next;
    }
    assert(currNode == iterator->end);

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
    return SUCCESS;
}
