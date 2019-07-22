// CS 5007, Northeastern University, Seattle
// Spring 2019
// Adrienne Slaughter
// Edited by: Khai Ly on 3/12/2019
//
// Inspired by UW CSE 333; used with permission.
//
// This is free software: you can redistribute it and/or modify
//  it under the terms of the GNU General Public License as published
//  by the Free Software Foundation, either version 3 of the License,
//  or (at your option) any later version.
// It is distributed in the hope that it will be useful, but
//  WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
//  General Public License for more details.

#include "LinkedList.h"
#include "LinkedList_priv.h"
#include "Assert007.h"

#include <stdio.h>
#include <stdlib.h>

LinkedList CreateLinkedList() {
  LinkedList list = (LinkedList)malloc(sizeof(LinkedListHead));
  if (list == NULL) {
    // out of memory
    return (LinkedList) NULL;
  }
  // Step 1.
  // initialize the newly allocated record structure
  list -> num_elements = 0U;
  list -> head = NULL;
  list -> tail = NULL;
  return list;
}

int DestroyLinkedList(LinkedList list,
                      LLPayloadFreeFnPtr payload_free_function) {
  Assert007(list != NULL);
  Assert007(payload_free_function != NULL);

  // Step 2.
  // Free the payloads, as well as the nodes
  // LinkedListNodePtr temp = list -> head;
  while (list -> head != NULL) {
    LinkedListNodePtr temp = list -> head;
    payload_free_function(temp -> payload);
    list -> head = temp -> next;
    DestroyLinkedListNode(temp);
  }
  free(list);

  return 0;
}

unsigned int NumElementsInLinkedList(LinkedList list) {
  Assert007(list != NULL);
  return list->num_elements;
}

LinkedListNodePtr CreateLinkedListNode(void *data) {
  LinkedListNodePtr node = (LinkedListNodePtr)malloc(sizeof(LinkedListNode));
  if (node == NULL) {
        // Out of memory
    return NULL;
  }
  node->payload = data;
  node->next = NULL;
  node->prev = NULL;

  return node;
  }

int DestroyLinkedListNode(LinkedListNode *node) {
  Assert007(node != NULL);
  node->payload = NULL;
  node->next = NULL;
  node->prev = NULL;
  free(node);
  return 0;
  }

int InsertLinkedList(LinkedList list, void *data) {
  Assert007(list != NULL);
  Assert007(data != NULL);
  LinkedListNodePtr new_node = CreateLinkedListNode(data);

  if (new_node == NULL) {
    return 1;
  }
  // when list has 0 elemen.
  if (list->num_elements == 0) {
    Assert007(list->head == NULL);  // debugging aid
    Assert007(list->tail == NULL);  // debugging aid
    list->head = new_node;
    list->tail = new_node;
    new_node->next = new_node->prev = NULL;
    list->num_elements = 1U;
    return 0;

  // when list has >0 elements/
  } else {
    new_node -> next = list -> head;
    list -> head -> prev = new_node;
    list -> head = new_node;
    list -> num_elements += 1U;
}

  // Step 3.
  // typical case; list has >=1 elements (implementation above).

  return 0;
}

int AppendLinkedList(LinkedList list, void *data) {
  Assert007(list != NULL);
  Assert007(data != NULL);

  // Step 5: implement AppendLinkedList.  It's kind of like
  // InsertLinkedList, but add to the end instead of the beginning.

  LinkedListNodePtr new_node = CreateLinkedListNode(data);
  if (new_node == NULL) {  // Check to see if malloc sucessfully.
    return 1;
}

  if (list -> num_elements == 0U) {
    Assert007(list -> head == NULL);  // debugging aids.
    Assert007(list -> tail == NULL);
    list -> tail = new_node;
    list -> head = new_node;
    list -> num_elements = 0U;
  } else {  // if list has 1+ elements.
    Assert007(list -> head != NULL);
    Assert007(list -> tail != NULL);  // Debugging aids.
    list -> tail -> next = new_node;
    new_node -> prev = list -> tail;
    list -> tail = new_node;
    list -> num_elements += 1U;
  }

  return 0;
}


int PopLinkedList(LinkedList list, void **data) {
  Assert007(list != NULL);
  Assert007(data != NULL);

  // Step 4: implement PopLinkedList.  Make sure you test for
  // and empty list and fail.  If the list is non-empty, there
  // are two cases to consider: (a) a list with a single element in it
  // and (b) the general case of a list with >=2 elements in it.
  // Be sure to call free() to deallocate the memory that was
  // previously allocated by InsertLinkedList().

  if (list -> num_elements == 0) {
    return 1;
  }
  *data = list -> head -> payload;
  LinkedListNodePtr temp = list -> head;
  if (list -> num_elements == 1) {  // if list has 1 element.
    list -> head = NULL;
    list -> tail = NULL;
  } else {
    list -> head = list -> head -> next;  // if list has 1+ elements.
    list -> head -> prev = NULL;
  }
  list -> num_elements -= 1U;
  DestroyLinkedListNode(temp);
  return 0;
}


int SliceLinkedList(LinkedList list, void **data) {
  Assert007(list != NULL);
  Assert007(data != NULL);

  // Step 6: implement SliceLinkedList.

  LinkedListNodePtr temp = list -> tail;
  if (temp == NULL) {  // Tail is NULL == fail.
    return 1;
  }
  if (list -> num_elements == 0) {  // Extra checking step.
    return 1;
  }

  *data = list -> tail -> payload;

  // When list has 1 element.
  if (list -> num_elements == 1) {
    list -> head = NULL;
    list -> tail = NULL;

  // When list has > 1 elements.
  } else {
    list -> tail = list -> tail -> prev;
    list -> tail -> next = NULL;  // Need to update pointer to NULL.
  }
  DestroyLinkedListNode(temp);
  list -> num_elements -= 1U;
  return 0;
}

void SortLinkedList(LinkedList list, unsigned int ascending,
  LLPayloadComparatorFnPtr compare) {
  Assert007(list != NULL);
  if (list->num_elements <2) {
    return;
  }
  int swapped;
  do {  // Use do-while loop to have the last node sorted.
    LinkedListNodePtr curnode = list->head;
    swapped = 0;  // If the first value does not need to be swapped, stop.
    while (curnode->next != NULL) {
        // Compare this node with the next; swap if needed
      int compare_result = compare(curnode->payload, curnode->next->payload);

         // Only 2 cases need to be swapped. Combine this with descending
        // and negative compare_result.
      if (ascending) {
        compare_result *= -1;
        }
      if (compare_result < 0) {
         // swap
        void* tmp;
        tmp = curnode->payload;
        curnode->payload = curnode->next->payload;
        curnode->next->payload = tmp;
        swapped = 1;
      }
      curnode = curnode->next;
      }
    } while (swapped);
}

void PrintLinkedList(LinkedList list) {
  printf("List has %lu elements. \n", list->num_elements);
}

LLIter CreateLLIter(LinkedList list) {
  Assert007(list != NULL);
  Assert007(list->num_elements > 0);

  LLIter iter = (LLIter)malloc(sizeof(struct ll_iter));
  Assert007(iter != NULL);

  iter->list = list;
  iter->cur_node = list->head;
  return iter;
}

int LLIterHasNext(LLIter iter) {
  Assert007(iter != NULL);
  return (iter->cur_node->next != NULL);
}

int LLIterNext(LLIter iter) {
  Assert007(iter != NULL);

  // Step 7: if there is another node beyond the iterator, advance to it,
  // and return 0. If there isn't another node, return 1.

  if (LLIterHasNext(iter)) {
    iter -> cur_node = iter -> cur_node -> next;
    return 0;
  }
  return 1;
}

int LLIterGetPayload(LLIter iter, void** data) {
  Assert007(iter != NULL);
  *data = iter->cur_node->payload;
  return 0;
}

int LLIterHasPrev(LLIter iter) {
  Assert007(iter != NULL);
  return (iter->cur_node->prev != NULL);
}

int LLIterPrev(LLIter iter) {
  Assert007(iter != NULL);
  // Step 8:  if there is another node beyond the iterator, go to it,
  // and return 0. If not return 1.
  if (LLIterHasPrev(iter)) {
    iter -> cur_node = iter -> cur_node -> prev;
    return 0;
  }
  return 1;
}

int DestroyLLIter(LLIter iter) {
  Assert007(iter != NULL);
  iter->cur_node = NULL;
  iter->list = NULL;
  free(iter);
  return 0;
}

int LLIterInsertBefore(LLIter iter, void* payload) {
  Assert007(iter != NULL);
  if ((iter->list->num_elements <= 1) ||
      (iter->cur_node == iter->list->head)) {
    // insert item
    return InsertLinkedList(iter->list, payload);
  }

  LinkedListNodePtr new_node = CreateLinkedListNode(payload);
  if (new_node == NULL) return 1;

  new_node->next = iter->cur_node;
  new_node->prev = iter->cur_node->prev;
  iter->cur_node->prev->next = new_node;
  iter->cur_node->prev = new_node;

  iter->list->num_elements++;

  return 0;
}

int LLIterDelete(LLIter iter, LLPayloadFreeFnPtr payload_free_function) {
  Assert007(iter != NULL);
  Assert007(iter -> list != NULL);
  Assert007(iter -> cur_node != NULL);
  // Step 9: implement LLIterDelete.  This is the most
  // complex function you'll build.  There are several cases
  // to consider:
  //
  // - degenerate case: the list becomes empty after deleting.
  // - degenerate case: iter points at head
  // - degenerate case: iter points at tail
  // - fully general case: iter points in the middle of a list,
  //                       and you have to "splice".
  //
  // Be sure to call the payload_free_function to free the payload
  // the iterator is pointing to, and also free any LinkedList
  // data structure element as appropriate.


  // When the list has exactly 1 element.
  if (iter -> list -> num_elements <= 1) {
    void* payload;
    PopLinkedList(iter -> list, (void**) &payload);
    payload_free_function(payload);
    iter -> list = NULL;
    iter -> cur_node = NULL;
    free(iter -> list);
    DestroyLLIter(iter);

  // When the list has 1 + element and pointer is pointing at tail.
  } else if (iter -> list -> tail == iter -> cur_node) {
    void* payload;
    LinkedListNodePtr newPtr = iter -> list -> tail -> prev;
    SliceLinkedList(iter -> list, (void**) &payload);
    payload_free_function(payload);
    iter -> cur_node = newPtr;

  // When the list has 1+ element and pointer is pointing at head.
  } else if (iter -> list -> head == iter -> cur_node) {
    void* payload;
    LinkedListNodePtr temp = iter -> list -> head -> next;
    PopLinkedList(iter -> list, (void**) &payload);
    payload_free_function(payload);
    iter -> cur_node = temp;

  // When the list has 1+ element and pointer is not head nor tail.
  } else {
    LinkedListNodePtr temp = iter -> cur_node;
    iter -> cur_node -> prev -> next = iter -> cur_node -> next;
    iter-> cur_node -> next -> prev = iter -> cur_node -> prev;
    LLIterNext(iter);
    payload_free_function(temp -> payload);
    DestroyLinkedListNode(temp);
    iter -> list -> num_elements -= 1U;
  }
  return 0;
}

