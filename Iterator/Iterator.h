#ifndef ITERATOR_H_
#define ITERATOR_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "util.h"

/* Definition of struct Iterator */
typedef struct Iterator Iterator;

/* Definition of struct Node */
typedef struct Node Node;

/* Request an iterator. If no more space available, iterator is set to NULL and
 * returns ERROR */
STATUS iter_CreateIterator(Iterator** iterator);

/* Add new node to iterator. If no more space available or iterator is NULL,
 * returns ERROR */
STATUS iter_AddNode(Iterator* iterator, void* data);

/* Get start node. If iterator is NULL, node is set to NULL and returns ERROR */
STATUS iter_GetStart(Iterator* iterator, Node** node);

/* Get next node. If startNode has no next or startNode is NULL, nextNode is set
 * to NULL and returns ERROR*/
STATUS iter_GetNext(Node* startNode, Node** nextNode);

/* Get count. If iterator or count is NULL, returns ERROR */
STATUS iter_GetCount(Iterator* iterator, uint16* count);

/* Get data from node. If node is NULL, data is set to NULL and returns ERROR */
STATUS iter_GetData(Node* node, void** data);

/* Find node from data. If iterator or data is NULL, node is set to NULL and
 * returns ERROR */
STATUS iter_FindNode(Iterator* iterator, void* data, Node** node);

/* Delete iterator and sets iterator to NULL. If iterator is NULL, returns
 * ERROR */
STATUS iter_DeleteIterator(Iterator** iterator);

/* Delete node and sets node to NULL. If node is NULL or does not belong to
 * iterator, returns ERROR */
STATUS iter_DeleteNode(Iterator* iterator, Node** node);

#ifdef __cplusplus
}
#endif

#endif /* ITERATOR_H_ */
