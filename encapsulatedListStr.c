/*
 * encapsulatedListStr.c
 * Author:Qiwei Li
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "encapsulatedListStr.h"

struct EncapsulatedList_Str {
	EncNode_Str	*head;
};

struct EncapsulatedList_Str_Node {
	char		*str;
	int		dup;
	EncNode_Str	*next;
	EncNode_Str	*prev;
};

/* Helpers for EncNode_Str */
EncNode_Str *encNode_Str__alloc(char *string, int dup)
{
	EncNode_Str *node;
	char *str = string;

	if (!string) {
		fprintf(stderr, "encNode_Str__alloc: The string is NULL.\n");
		return NULL;
	}

	/* Allocate a new node */
	node = (EncNode_Str *)malloc(sizeof(EncNode_Str));
	if (!node) {
		perror("malloc");
		return NULL;
	}

	/* Duplicate the string if necessary */
	if (dup) {
		str = (char *)malloc(sizeof(char) * (strlen(string) + 1));
		if (!str) {
			perror("malloc");
			/* Free the allocated node */
			free(node);
			return NULL;
		}
		strcpy(str, string);
	}

	/* Initialize the node */
	node->str = str;
	node->dup = dup;
	node->prev = NULL;
	node->next = NULL;

	return node;
}

void encNode_Str__free(EncNode_Str *node)
{
	if (!node) {
		fprintf(stderr, "encNode_Str__free: The node is NULL.\n");
		return;
	}

	/* Free the node without sanity check */
	if (node->dup)
		free(node->str);
	free(node);
}

void encNode_Str__addAfter(EncNode_Str *pos, EncNode_Str *node)
{
	if (!pos || !node) {
		fprintf(stderr, "encNode_Str__addAfter: The node(s) is NULL.\n");
		return;
	}

	/* Sanity check */
	if (node->prev || node->next) {
		fprintf(stderr, "encNode_Str__addAfter: The node is already on a list.\n");
		return;
	}

	node->prev = pos;
	node->next = pos->next;
	if (pos->next)
		pos->next->prev = node;
	pos->next = node;
}

/* Helpers for EncList_Str */
EncNode_Str *encList_Str__popHead(EncList_Str *obj)
{
	EncNode_Str *head;

	if (!obj) {
		fprintf(stderr, "encList_Str__popHead: The object is NULL.\n");
		return NULL;
	}

	head = obj->head;
	if (head) {
		if (head->next)
			head->next->prev = NULL;
		obj->head = head->next;

		head->prev = NULL;
		head->next = NULL;
	}

	return head;
}

// ------------- alloc() - Constructor ---------------
// Parameters: None
//
// Allocates a new EncList_Str object, and initializes it to hold an empty
// list (that is, no node objects should be allocated).
//
// ERRORS:
//   - malloc() fails.  Print error and return NULL

EncList_Str *encList_Str__alloc()
{
	EncList_Str *obj;

	/* Allocate a new object */
	obj = (EncList_Str *)malloc(sizeof(EncList_Str));
	if (!obj) {
	        perror("malloc");
	        return NULL;
	}

	/* Initialize the object */
	obj->head = NULL;

	return obj;
}

// -------------- free() - Destructor ----------------
// Parameters: 'this' pointer (for the wrapper object)
//
// Frees an existing EncList_Str object.  If there are any nodes inside this
// list, this also frees all of them.
//
// ERRORS:
//   - Pointer is NULL

void encList_Str__free(EncList_Str *obj)
{
	EncNode_Str *pos, *next;

	if (!obj) {
		fprintf(stderr, "encList_Str__free: The object is NULL.\n");
		return;
	}

	/* Free nodes */
	pos = obj->head;
	while (pos) {
		next = pos->next;
		encNode_Str__free(pos);
		pos = next;
	}

	/* Free the object itself */
	free(obj);
}

// ---------------- addHead ---------------------------------
// Parameters: 'this' pointer (for the wrapper object)
//             *string*
//             dup (boolean flag)
//
// Adds the given string to the front of the list (duplicates are allowed).  If
// dup=1, then this method will malloc() a new buffer, and copy the string
// into that buffer.  If dup=0, it will simply save the pointer into the node.
//
// When the node is destroyed later, free() will free the string if it was
// duplicated in this function - but if it was not duplicated, then it will
// *NOT* free it, and the caller is responsible for doing so.
//
// ERRORS:
//   - Pointer is NULL.  Print error.

void encList_Str__addHead(EncList_Str *obj, char *string, int dup)
{
	EncNode_Str *node;

	if (!obj) {
		fprintf(stderr, "encList_Str__addHead: The object is NULL.\n");
		return;
	}

	/* Allocate a new node */
	node = encNode_Str__alloc(string, dup);
	/* Errors should be handled in encNode_Str__alloc */
	if (!node)
		return;

	/* Add to the front of the list */
	node->prev = NULL;
	node->next = obj->head;
	if (obj->head)
		obj->head->prev = node;
	obj->head = node;
}

// ---------------- addTail ---------------------------------
// Parameters: 'this' pointer (for the wrapper object)
//             *string*
//             dup (boolean flag)
//
// Adds the given string to the end of the list (duplicates are allowed).  If
// dup=1, then this method will malloc() a new buffer, and copy the string
// into that buffer.  If dup=0, it will simply save the pointer into the node.
//
// When the node is destroyed later, free() will free the string if it was
// duplicated in this function - but if it was not duplicated, then it will
// *NOT* free it, and the caller is responsible for doing so.
//
// ERRORS:
//   - Pointer is NULL.  Print error.

void encList_Str__addTail(EncList_Str *obj, char *string, int dup)
{
	EncNode_Str *node, *tail;

	if (!obj) {
		fprintf(stderr, "encList_Str__addTail: The object is NULL.\n");
		return;
	}

	/* Allocate a new node */
	node = encNode_Str__alloc(string, dup);
	/* Errors should be handled in encNode_Str__alloc */
	if (!node)
		return;

	/* Add to the end of the list */
	tail = encList_Str__getTail(obj);
	if (!tail)
		obj->head = node;
	else {
		node->prev = tail;
		tail->next = node;
	}
}

// ---------------- count ----------------------------
// Parameters: 'this' pointer (of the wrapper class)
//
// Returns the number of nodes in the list.
//
// ERRORS:
//   'this' is NULL.  Print error and return -1.

int encList_Str__count(EncList_Str *obj)
{
	EncNode_Str *pos;
	int cnt = 0;

	if (!obj) {
		fprintf(stderr, "encList_Str__count: The object is NULL.\n");
		return 0;
	}

	pos = obj->head;
	while (pos) {
		cnt++;
		pos = pos->next;
	}
	return cnt;
}

// ---------------- getMin/getMax ----------------------------
// Parameters: 'this' pointer (of the wrapper class)
//
// Searches the list for the minimum or maximum string.  This does *NOT*
// assume that the list is sorted, and so it probably will perform a
// brute-force scan of the entire list.
//
// Returns NULL if the list is empty.
//
// ERRORS:
//   'this' is NULL.  Print error and return NULL.

char *encList_Str__getMin(EncList_Str *obj)
{
	EncNode_Str *node;
	char* str;

	if (!obj) {
		fprintf(stderr, "encList_Str__getMin: The object is NULL.\n");
		return 0;
	}

	if (!obj->head)
		return NULL;

	node = obj->head;
	str = node->str;
	while (node->next) {
		node = node->next;
		if (strcmp(str, node->str) > 0)
			str = node->str;
	}

	return str;
}
char *encList_Str__getMax(EncList_Str *obj)
{
	EncNode_Str *node;
	char* str;

	if (!obj) {
		fprintf(stderr, "encList_Str__getMax: The object is NULL.\n");
		return 0;
	}

	if (!obj->head)
		return NULL;

	node = obj->head;
	str = node->str;
	while (node->next) {
		node = node->next;
		if (strcmp(str, node->str) < 0)
			str = node->str;
	}

	return str;
}

// ---------------- sort ----------------------------
// Parameters: 'this' pointer (of the wrapper class)
//             another list
//
// This function assumes (but does not verify) that the two lists are both
// already sorted.  It iterates through the two lists in parallel, always
// taking the smaller value from each list and placing into the resulting,
// merged list.
//
// Thus, the resulting list will be sorted when the function completes.
//
// The merged list will be stored in the first parameter; all of the nodes
// will be removed from the second parameter (and placed in their proper
// locations within the first).
//
// EXAMPLE:
//   Before merge():
//       left  list: foo jkl qwerty
//       right list: asdf bar good zzzz
//   After merge():
//       left  list: asdf bar foo good jkl qwerty zzzz
//       right list: <empty>
//
// NOTE: Strings must *NOT* be copied back and forth between nodes!
//       This function *ONLY* changes the next/prev arrows that link nodes!
//
// ERRORS:
//   Either pointer is NULL.  Print error.

void encList_Str__merge(EncList_Str *lhs, EncList_Str *rhs)
{
	EncList_Str obj = { NULL };
	EncNode_Str *head = NULL, *pos = NULL;

	if (!lhs || !rhs) {
		fprintf(stderr, "encList_Str__merge: The object is NULL.\n");
		return;
	}

	/* Start from heads of two lists */
	while (lhs->head && rhs->head) {
		EncNode_Str *node;

		/* Get the minimum node and remove from list */
		if (strcmp(lhs->head->str, rhs->head->str) <= 0)
			node = encList_Str__popHead(lhs);
		else
			node = encList_Str__popHead(rhs);

		/* Append to the temporary list */
		if (!head) {
			head = node;
			pos = node;
		} else {
			encNode_Str__addAfter(pos, node);
			pos = pos->next;
		}
	}

	/* Append the list that has not yet been fully merged */
	obj.head = head;
	if (lhs->head)
		encList_Str__append(&obj, lhs);
	else if (rhs->head)
		encList_Str__append(&obj, rhs);

	/* Assertion */
	if (lhs->head || rhs->head)
		fprintf(stderr, "encList_Str__merge: Internal error.\n");

	/* Assign the new head to lhs anyway */
	lhs->head = obj.head;
}

// ---------------- append ----------------------------
// Parameters: 'this' pointer (of the wrapper class)
//             pointer to another list
//
// Removes all of the nodes from the 'other' list, and adds them to the tail
// of the 'this' list.  After this function, the 'other' list will still be
// valid, but will be empty.
//
// ERRORS:
//   - Either pointer is NULL.  Print error.

void encList_Str__append(EncList_Str *lhs, EncList_Str *rhs)
{
	EncNode_Str *tail, *head;

	if (!lhs || !rhs) {
		fprintf(stderr, "encList_Str__append: The object is NULL.\n");
		return;
	}

	/* Get tail of first list, and head of second list */
	tail = encList_Str__getTail(lhs);
	head = encList_Str__getHead(rhs);

	/* Append to the first list */
	if (!tail) {
		lhs->head = head;
	} else {
		tail->next = head;
		if (head)
			head->prev = tail;
	}

	/* Empty the other list */
	rhs->head = NULL;
}

// ---------------- index ---------------------------------
// Parameters: 'this' pointer (for the wrapper object)
//             index
//
// Returns the node at a given "index."  Index 0 is the head; index 1 is the
// node immediately after the head.
//
// Just like an array, the valid indices are 0 through count()-1, inclusive.
//
// ERRORS:
//   - Pointer is NULL.  Print error and return NULL.
//   - Invalid index (negative, or too large).  Print error and return NULL.

EncNode_Str *encList_Str__index(EncList_Str *obj, int index)
{
	EncNode_Str *pos;
	int idx;

	if (!obj) {
		fprintf(stderr, "encList_Str__index: The object is NULL.\n");
		return NULL;
	}
	if (index < 0) {
		fprintf(stderr, "encList_Str__index: The index is negative.\n");
		return NULL;
	}

	pos = obj->head;
	idx = 0;
	while (pos && (idx < index)) {
		pos = pos->next;
		idx++;
	}

	if (!pos) {
		fprintf(stderr, "encList_Str__index: The index is too large.\n");
		return NULL;
	}

	return pos;
}

// ---------------- splitAt ----------------------------
// Parameters: 'this' pointer (of the wrapper class)
//             index into the list
//
// Splits the current list into two.  This allocates a new list object.  It
// then moves some of the "back" end of this list into the new object (these
// elements are removed from the current list).
//
// The position for the split is given by the 'index;' the index is the first
// node that will be *MOVED* to the other list.  So if index=0, then the
// *ENTIRE* list will be moved into the new object; if index=4, then 3 will
// remain in the current list, and all of the nodes after it will be moved.
//
// Basically, the index parameter works like the index() method above, with
// one exception: the value count() is *LEGAL* in this function.  (This simply
// means, move *NOTHING* into the new list.)
//
// ERRORS:
//   - Either pointer is NULL.  Print error and return NULL
//   - Index is invalid.  Print error and return NULL

EncList_Str *encList_Str__splitAt(EncList_Str *obj, int index)
{
	EncList_Str *newObj;
	int count;
	EncNode_Str *node;

	if (!obj) {
		fprintf(stderr, "encList_Str__splitAt: The object is NULL.\n");
		return NULL;
	}

	count = encList_Str__count(obj);
	if (index < 0 || index > count) {
		fprintf(stderr, "encList_Str__splitAt: The index is invalid.\n");
		return NULL;
	}

	newObj = encList_Str__alloc();
	/* Errors should be handled in encList_Str__alloc */
	if (!newObj)
		return NULL;

	/* Move nothing into the new list */
	if (index == count)
		return newObj;

	node = encList_Str__index(obj, index);

	/* Remove node from the original list, but not reset node->next */
	if (node->prev)
		node->prev->next = NULL;
	else	/* node is head */
		obj->head = NULL;

	/* Append node to the new list as head */
	node->prev = NULL;
	newObj->head = node;

	return newObj;
}

// ---------------- getHead ---------------------------------
// Parameters: 'this' pointer (for the wrapper object)
//
// Returns a pointer to the head of the list (if the list is empty, this
// returns NULL).  See below for the available "methods" you can call on a
// node.
//
// getTail(): Equivalent, but finds the last element
//
// ERRORS:
//   - Pointer is NULL.  Print error and return NULL.

EncNode_Str *encList_Str__getHead(EncList_Str *obj)
{
	if (!obj) {
		fprintf(stderr, "encList_Str__getHead: The object is NULL.\n");
		return NULL;
	}

	return obj->head;
}
EncNode_Str *encList_Str__getTail(EncList_Str *obj)
{
	EncNode_Str *node;

	if (!obj) {
		fprintf(stderr, "encList_Str__getTail: The object is NULL.\n");
		return NULL;
	}

	/* Search toward the tail of the list */
	node = obj->head;
	while (node && node->next)
		node = node->next;

	return node;
}


// ---------------- gettors (various) -----------------------
// Parameters: 'this' pointer (for the NODE!)
//
// Returns various properties of the list node.  next/prev will return NULL
// when we hit the extreme of the list.
//
// ERRORS:
//   - None
//     (the pointer might be NULL; just let it segfault)

char *encNode_Str__getStr(EncNode_Str *node)
{
	return node->str;
}
EncNode_Str *encNode_Str__getNext(EncNode_Str *node)
{
	return node->next;
}
EncNode_Str *encNode_Str__getPrev(EncNode_Str *node)
{
	return node->prev;
}
