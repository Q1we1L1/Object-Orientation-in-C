/*
 * dblListInt.c
 * Author:Qiwei Li
 */

#include <stdio.h>
#include <stdlib.h>

#include "dblListInt.h"

// ------------- alloc() - Constructor ---------------
// Parameters: int (value for the new node)
//             NOTE: no 'this' pointer
//
// Allocates a new DblList_Int object.  Initializes the val to the value
// provided.  Initializes the next,prev pointers to NULL (that is, the
// new node is not on any list).  Returns the newly allocated object.
//
// ERRORS:
//   - malloc() fails.  Print error and return NULL

DblList_Int *dblList_Int__alloc(int val)
{
	DblList_Int *node;

	/* Allocate a new node */
	node = (DblList_Int *)malloc(sizeof(DblList_Int));
	if (!node) {
		perror("malloc");
		return NULL;
	}

	/* Initialize the node */
	node->val = val;
	node->prev = NULL;
	node->next = NULL;

	return node;
}

// -------------- free() - Destructor ----------------
// Parameters: 'this' pointer
//
// Frees an existing DblList_Int object.  The object must not be part of any
// list (that is, its 'next' and 'prev' must both be NULL).
//
// ERRORS:
//   - Pointer is NULL
//   - The node has non-NULL next or prev pointers.  Print error, but still
//     free the object before returning.

void dblList_Int__free(DblList_Int *node)
{
	if (!node) {
		fprintf(stderr, "dblList_Int__free: The node is NULL.\n");
		return;
	}

	/* Sanity check */
	if (node->prev || node->next)
		fprintf(stderr, "dblList_Int__free: The existing node has non-NULL next or prev pointers.\n");

	/* Free the node anyway */
	free(node);
}


// ---------------- gettors (various) -----------------------
// Parameters: 'this' pointer
//
// Returns various properties of the list node.
//
// ERRORS: None
//         (the pointer might be NULL; just let it segfault)

int dblList_Int__getVal(DblList_Int *node)
{
	return node->val;
}
DblList_Int *dblList_Int__getNext(DblList_Int *node)
{
	return node->next;
}
DblList_Int *dblList_Int__getPrev(DblList_Int *node)
{
	return node->prev;
}


// ---------------- getHead ---------------------------------
// Parameters: 'this' pointer
//
// Searches toward the front of the list, from the 'this' pointer; returns the
// node at the head of the list.  (This might be the 'this' pointer.)
//
// getTail(): Equivalent, but finds the last element
//
// ERRORS:
//   - Pointer is NULL.  Print error and return NULL.

DblList_Int *dblList_Int__getHead(DblList_Int *node)
{
	if (!node) {
		fprintf(stderr, "dblList_Int__getHead: The node is NULL.\n");
		return NULL;
	}

	/* Search toward the front of the list */
	while (node->prev)
		node = node->prev;

	return node;
}
DblList_Int *dblList_Int__getTail(DblList_Int *node)
{
	if (!node) {
		fprintf(stderr, "dblList_Int__addTail: The node is NULL.\n");
		return NULL;
	}

	/* Search toward the tail of the list */
	while (node->next)
		node = node->next;

	return node;
}


// ---------------- addAfter --------------------------------
// Parameters: 'this' pointer
//             Pointer to another object
//
// Chains the 'other' node, immediately after the 'this' node.  We assume that
// the 'this' object is part of a list (though it might be a list of length 1),
// so we consider that there might already be a 'next' node; if that is, true,
// then the 'other' is inserted *BETWEEN* 'this' and its (old) next neighbor.
//
// For example imagine that node A is the 'this' pointer, node B is the one
// that comes after it, and we call addAfter(), passing X as the 'other' node.
//    This is what the list looked like before the method:
//                A - B
//    This is what it will look like after the method:
//                A - X - B
//
// ERRORS:
//   - Either Pointer is NULL.  Print error.
//   - The 'other' object is already on a list.  Print error and return; do
//     *NOT* change either list.

void dblList_Int__addAfter(DblList_Int *pos, DblList_Int *node)
{
	if (!pos || !node) {
		fprintf(stderr, "dblList_Int__addAfter: The node(s) is NULL.\n");
		return;
	}

	/* Sanity check */
	if (node->prev || node->next) {
		fprintf(stderr, "dblList_Int__addAfter: The node is already on a list.\n");
		return;
	}

	node->prev = pos;
	node->next = pos->next;
	if (pos->next)
		pos->next->prev = node;
	pos->next = node;
}

// ---------------- addTail ---------------------------------
// Parameters: 'this' pointer
//             *value*
//
// Searches for the tail end of the list; allocates a new node (using the
// value given), and appends it to the tail of the list.  Returns nothing.
//
// ERRORS:
//   - Pointer is NULL.  Print error.

void dblList_Int__addTail(DblList_Int *list, int value)
{
	DblList_Int *tail, *node;

	/* Get tail of the list */
	tail = dblList_Int__getTail(list);
	/* Errors should be handled in dblList_Int__getTail */
	if (!tail)
		return;

	/* Alloc a new node */
	node = dblList_Int__alloc(value);
	/* Errors should be handled in dblList_Int__alloc */
	if (!node)
		return;

	/* Insert the node after tail of the list */
	dblList_Int__addAfter(tail, node);
}

// ---------------- remove ---------------------------------
// Parameters: 'this' pointer
//
// Removes the current node from the list it is part of.
//
// ERRORS:
//   - Pointer is NULL.  Print error.
//   - Object is not part of any list (that is, next==prev==NULL).  Print
//     error.

void dblList_Int__remove(DblList_Int *node)
{
	if (!node) {
		fprintf(stderr, "dblList_Int__remove: The node is NULL.\n");
		return;
	}

	/* Sanity check */
	if (!node->prev && !node->next) {
		fprintf(stderr, "dblList_Int__remove: The node is not part of any list.\n");
		return;
	}

	/* Remove the node from the list */
	if (node->prev)
		node->prev->next = node->next;
	if (node->next)
		node->next->prev = node->prev;

	/* Reset the node */
	node->prev = NULL;
	node->next = NULL;
}

// ---------------- swapWithNext ----------------------------
// Parameters: 'this' pointer
//
// Swaps the position of the current node and the next on the list.  This
// function *MUST* actually change the 'next' and 'prev' pointers; it is *NOT*
// OK to copy the 'val' fields back and forth.
//
// For example, if we have the list
//    A - B - C - D
// and we call swapWithNext() on B, then the list will change to:
//    A - C - B - D
//
// Note that we assume that thee object (and its next object) are both
// non_NULL.  However, the function will work properly even if the object is
// the head (its successor becomes the new head) or the 'next' field is the
// tail (the 'this' object becomes the new tail).
//
// ERRORS:
//   'this', or the next object, are NULL.  Print error.

void dblList_Int__swapWithNext(DblList_Int *node)
{
	DblList_Int *next;

	if (!node || !node->next) {
		fprintf(stderr, "dblList_Int__swapWithNext: The node, or the next object, are NULL.\n");
		return;
	}

	/* Keep the next node */
	next = node->next;

	/* Remove this node */
	dblList_Int__remove(node);

	/* Add this node after the original next node */
	dblList_Int__addAfter(next, node);
}
