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

/* Request an iterator */
STATUS iter_CreateIterator(Iterator** iterator);

/* Add new node to iterator */
STATUS iter_AddNode(Iterator* iterator, void* data);

/* Get start node */
STATUS iter_GetStart(Iterator* iterator, Node** node);

/* Get next node. If startNode has no next, nextNode is set to NULL and returns
 * ERROR*/
STATUS iter_GetNext(Node* startNode, Node** nextNode);

/* Get count */
STATUS iter_GetCount(Iterator* iterator, uint16* count);

/* Get data from node */
STATUS iter_GetData(Node* node, void** data);

/* Find node from data. If not found, node is set to NULL and returns ERROR */
STATUS iter_FindNode(Iterator* iterator, void* data, Node** node);

/* Delete iterator and sets iterator to NULL */
STATUS iter_DeleteIterator(Iterator** iterator);

/* Delete node and sets node to NULL */
STATUS iter_DeleteNode(Iterator* iterator, Node** node);

#ifdef __cplusplus
}
#endif

#endif /* ITERATOR_H_ */
