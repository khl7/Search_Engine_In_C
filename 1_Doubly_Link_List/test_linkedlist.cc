// CS 5007, Northeastern University, Seattle
// Spring 2019
// Adrienne Slaughter
//
// Used by Khai Ly 3/12/2019.
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

#include "gtest/gtest.h"

extern "C" {
    #include "LinkedList.h"
    #include "LinkedList_priv.h"
}

const char* first = "first";
const char* second="second";
const char* third = "third";
const char* fourth = "fourth";

struct myThing {
    int number;
    const char *name;
};

typedef struct myThing MyThing;

MyThing* CreateMyThing(int num, const char *word) {
  MyThing* thing = (MyThing*)malloc(sizeof(MyThing));
  thing->number = num;
  thing->name = word;
  return thing;
}

void DestroyMyThing(void* thing) {
  free(thing);
  thing = NULL;
}

int CompareMyThing(void* thing1, void* thing2) {
    if (((MyThing*)thing1)->number == ((MyThing*)thing2)->number) {
        return 0;
    }
    if (((MyThing*)thing1)->number < ((MyThing*)thing2)->number) {
        return -1;  
    } else {
        return 1; 
    }
}

// Size of LL is 0
TEST(LinkedList, emptyList) {
  LinkedList list = CreateLinkedList();
  EXPECT_FALSE(list == NULL);
  EXPECT_EQ(list->num_elements, 0u);
  DestroyLinkedList(list, &DestroyMyThing);
}

TEST(LinkedList, addElem) {
    LinkedList list = CreateLinkedList();

    // Add elements to the LL
    MyThing *thing1 = CreateMyThing(5, first);

    InsertLinkedList(list, thing1);

    // The 1u refers to unsigned 1 
    EXPECT_EQ(list->num_elements, 1u);
    EXPECT_EQ(list->head->payload, thing1);
    EXPECT_EQ(list->tail->payload, thing1);
    EXPECT_TRUE(list->head->next == NULL);
    EXPECT_TRUE(list->head->prev == NULL);
    EXPECT_TRUE(list->tail->next == NULL);
    EXPECT_TRUE(list->tail->prev == NULL);

    MyThing *payload; 
    int res = PopLinkedList(list, (void**)&payload); 
    EXPECT_EQ(res, 0); 
    EXPECT_EQ(payload, thing1);
    EXPECT_EQ(payload->number, thing1->number);
    EXPECT_EQ(payload->name, thing1->name); 
    
    DestroyMyThing(payload); 
    
    DestroyLinkedList(list, &DestroyMyThing);
}

TEST(LinkedList, addTwoElems) {
    LinkedList list = CreateLinkedList();

    // Add elements to the LL
    MyThing *thing1 = CreateMyThing(5, first);
    MyThing *thing2 = CreateMyThing(15, second);

    InsertLinkedList(list, thing1);
    InsertLinkedList(list, thing2);

    EXPECT_TRUE(list->head->prev == NULL);

    EXPECT_EQ(list->num_elements, 2u);
    EXPECT_EQ(list->head->payload, thing2);
    EXPECT_EQ(list->tail->payload, thing1);
    EXPECT_FALSE(list->head->next == NULL);
    EXPECT_TRUE(list->head->prev == NULL);
    EXPECT_TRUE(list->tail->next == NULL);
    EXPECT_FALSE(list->tail->prev == NULL);

    DestroyLinkedList(list, &DestroyMyThing);
}

TEST(LinkedList, removeOneElem) {
    LinkedList list = CreateLinkedList();

    // Add elements to the LL
    MyThing *thing1 = CreateMyThing(5, first);
    MyThing *thing2 = CreateMyThing(15, second);

    InsertLinkedList(list, thing1);
    InsertLinkedList(list, thing2);

    EXPECT_EQ(list->num_elements, 2u);

    MyThing *thing3; 

    int code = PopLinkedList(list, (void**)&thing3);
    EXPECT_EQ(code, 0);
    EXPECT_EQ(list->num_elements, 1u);
    EXPECT_EQ(thing3, thing2);

    DestroyMyThing(thing3); 
    
    DestroyLinkedList(list, &DestroyMyThing);
}

TEST(LinkedList, removeTwoElem) {
    LinkedList list = CreateLinkedList();

    // Add elements to the LL
    MyThing *thing1 = CreateMyThing(5, first);
    MyThing *thing2 = CreateMyThing(15, second);

    InsertLinkedList(list, thing1);
    InsertLinkedList(list, thing2);

    MyThing *popped; 

    EXPECT_EQ(list->num_elements, 2u);

    int code = PopLinkedList(list, (void**)&popped);
    EXPECT_EQ(code, 0);
    EXPECT_EQ(list->num_elements, 1u);
    EXPECT_EQ(popped, thing2);
    DestroyMyThing(popped);
    
    code = PopLinkedList(list, (void**)&popped);

    EXPECT_EQ(code, 0);
    EXPECT_EQ(list->num_elements, 0u);
    EXPECT_EQ(popped, thing1);
    DestroyMyThing(popped);
    
    DestroyLinkedList(list, &DestroyMyThing);
}

TEST(LinkedList, Append) {
    LinkedList list = CreateLinkedList();

    // Add elements to the LL
    MyThing *thing1 = CreateMyThing(5, first);
    MyThing *thing2 = CreateMyThing(15, second);

    InsertLinkedList(list, thing1);
    AppendLinkedList(list, thing2);

    EXPECT_TRUE(list->head->prev == NULL);

    EXPECT_EQ(list->num_elements, 2u);
    EXPECT_EQ(list->head->payload, thing1);
    EXPECT_EQ(list->tail->payload, thing2);
    EXPECT_FALSE(list->head->next == NULL);
    EXPECT_TRUE(list->head->prev == NULL);
    EXPECT_TRUE(list->tail->next == NULL);
    EXPECT_FALSE(list->tail->prev == NULL);

    DestroyLinkedList(list, &DestroyMyThing);
}

TEST(LinkedList, Slice) {
    LinkedList list = CreateLinkedList();

    // Add elements to the LL
    MyThing *thing1 = CreateMyThing(5, first);
    MyThing *thing2 = CreateMyThing(15, second);

    InsertLinkedList(list, thing1);
    InsertLinkedList(list, thing2);

    MyThing *popped; 

    EXPECT_EQ(list->num_elements, 2u);

    int code = SliceLinkedList(list, (void**)&popped);
    EXPECT_EQ(code, 0);
    EXPECT_EQ(list->num_elements, 1u);
    EXPECT_EQ(popped, thing1);
    DestroyMyThing(popped);
    
    code = SliceLinkedList(list, (void**)&popped);

    EXPECT_EQ(code, 0);
    EXPECT_EQ(list->num_elements, 0u);
    EXPECT_EQ(popped, thing2);
    DestroyMyThing(popped);
    
    DestroyLinkedList(list, &DestroyMyThing);
}

TEST(LLIterator, GetPayload) {
    LinkedList list = CreateLinkedList();

    if (list == NULL) return;
    
    // Add elements to the LL
    unsigned int num_items = 10; 
    
    MyThing* things[num_items]; 
    for (unsigned int i=0; i<num_items; i++) {
      things[i] = CreateMyThing(i, "aThing");
      InsertLinkedList(list, things[i]); 
    }
    
    EXPECT_EQ(list->num_elements, num_items);    

    void* payload;
        
    LLIter iter = CreateLLIter(list);

    for (int i=num_items-1; i>=0; i--) {
      LLIterGetPayload(iter, &payload); 
      EXPECT_EQ(payload, things[i]);
      LLIterNext(iter);
    }
        
    DestroyLLIter(iter); 
    DestroyLinkedList(list, &DestroyMyThing);
}


TEST(LinkedList, Sort) {
    LinkedList list = CreateLinkedList();

    if (list == NULL) return;
    
    // Add elements to the LL
    InsertLinkedList(list, CreateMyThing(5, "foo"));
    InsertLinkedList(list, CreateMyThing(10, "foo"));
    InsertLinkedList(list, CreateMyThing(7, "foo"));
    InsertLinkedList(list, CreateMyThing(1, "foo"));
    InsertLinkedList(list, CreateMyThing(8, "foo"));
    InsertLinkedList(list, CreateMyThing(2, "foo"));
    InsertLinkedList(list, CreateMyThing(1, "foo"));

    EXPECT_EQ(list->num_elements, 7u);

    SortLinkedList(list, 1, CompareMyThing); 

    LLIter iter = CreateLLIter(list);

    MyThing* payload; 
    int last_val = 0; 

    while(LLIterHasNext(iter)) {
        LLIterGetPayload(iter, (void**)&payload); 
        EXPECT_TRUE((payload)->number >= last_val); 
        last_val = payload->number;
        LLIterNext(iter);
    }
    
    DestroyLLIter(iter); 
    DestroyLinkedList(list, &DestroyMyThing);
}



TEST(LLIterator, IterForward) {
    LinkedList list = CreateLinkedList();

    unsigned int num_items = 10; 
    
    MyThing* things[num_items]; 
    for (unsigned int i=0; i<num_items; i++) {
      things[i] = CreateMyThing(i, "aThing");
      InsertLinkedList(list, things[i]); 
    }
    
    EXPECT_EQ(list->num_elements, num_items);    

    LLIter iter = CreateLLIter(list);
    EXPECT_TRUE(iter->cur_node != NULL);
    
    // If it's pointing at the head of the list, the prev node should be null
    EXPECT_EQ(iter->cur_node, list->head);
    EXPECT_TRUE(iter->cur_node->prev == NULL);
    EXPECT_TRUE(LLIterHasNext(iter));
    EXPECT_FALSE(LLIterHasPrev(iter));
    // Get the first one (no prev node)
    EXPECT_EQ(0, LLIterNext(iter));
    void *payload; 
    
    for (int i=num_items-2; i>=2; i--) {
      LLIterGetPayload(iter, &payload);
      EXPECT_EQ(things[i],payload);
      EXPECT_TRUE(LLIterHasNext(iter));
      EXPECT_TRUE(LLIterHasPrev(iter));
      EXPECT_EQ(0, LLIterNext(iter));
    }
    LLIterGetPayload(iter, &payload);
    EXPECT_EQ(things[1],payload);
    // Now, get the last one
    // Moves to element 0
    EXPECT_EQ(0, LLIterNext(iter));
    LLIterGetPayload(iter, &payload);
    EXPECT_EQ(things[0],payload);
    
    EXPECT_FALSE(LLIterHasNext(iter));
    EXPECT_TRUE(LLIterHasPrev(iter));

    EXPECT_EQ(1, LLIterNext(iter)); 

    DestroyLLIter(iter);
    DestroyLinkedList(list, &DestroyMyThing);
}


TEST(LLIterator, IterBackward) {
    LinkedList list = CreateLinkedList();

    // Add elements to the LL
    MyThing *thing1 = CreateMyThing(5, first);
    MyThing *thing2 = CreateMyThing(15, second);
    MyThing *thing3 = CreateMyThing(20, third);
    InsertLinkedList(list, thing1);
    InsertLinkedList(list, thing2);
    InsertLinkedList(list, thing3);

    LLIter iter = CreateLLIter(list);
    EXPECT_TRUE(iter->cur_node != NULL);

    EXPECT_EQ(0, LLIterNext(iter));
    EXPECT_EQ(0, LLIterNext(iter));
    EXPECT_EQ(0, LLIterPrev(iter));
    EXPECT_EQ(0, LLIterPrev(iter));

    DestroyLLIter(iter);
    DestroyLinkedList(list, &DestroyMyThing);
}

// TODO: Utilize this more
void CreateTestList(LinkedList* list, MyThing* elements[], unsigned int num_elements) {
// Create a list
   *list = CreateLinkedList();

   for (unsigned int i = 0; i<num_elements; i++) {
     elements[i] = CreateMyThing(i, "bar"); 
     InsertLinkedList(*list, elements[i]);
     EXPECT_EQ(i+1u, (*list)->num_elements);
   }
}

TEST(LLIterator, HasNext) {
  LinkedList list = NULL;
  unsigned int num_items = 10; 
  MyThing* items[num_items];
  CreateTestList(&list, items, num_items);

  LLIter iter = CreateLLIter(list); 
  
  for (unsigned int i=0; i<num_items-1; i++) {
    EXPECT_EQ(1, LLIterHasNext(iter));
    EXPECT_EQ(0, LLIterNext(iter)); 
  }
  
  EXPECT_EQ(0, LLIterHasNext(iter));

  DestroyLLIter(iter);
  DestroyLinkedList(list, &DestroyMyThing); 
  
}

TEST(LLIterator, Delete) {
  // Create a list
  LinkedList list = CreateLinkedList();

  unsigned int num_elems = 10;
  MyThing* elements[num_elems];
  for (unsigned int i = 0; i<num_elems; i++) {
    elements[i] = CreateMyThing(i, "bar"); 
    InsertLinkedList(list, elements[i]);
    EXPECT_EQ(i+1u, list->num_elements);
  }
    
  // Create an iter
  LLIter iter = CreateLLIter(list);

  LLIterNext(iter);
  LLIterNext(iter);
  
  // Should be at element 7 now
  MyThing* payload;
  LLIterGetPayload(iter, (void**)&payload);
  EXPECT_EQ(7, payload->number);

  // Delete the element
  EXPECT_EQ(0, LLIterDelete(iter, &DestroyMyThing));

  // Go to the end and delete the end
  while (LLIterHasNext(iter)) {
    EXPECT_EQ(0, LLIterNext(iter)); 
  }
  EXPECT_EQ(0, LLIterDelete(iter, &DestroyMyThing));

  // Go to the begining and delete the head
  while (LLIterHasPrev(iter)) {
    EXPECT_EQ(0, LLIterPrev(iter)); 
  }
  EXPECT_EQ(0, LLIterDelete(iter, &DestroyMyThing));

  EXPECT_EQ(7u, list->num_elements); 
  
  DestroyLLIter(iter);
  DestroyLinkedList(list, &DestroyMyThing);
}

TEST(LLIterator, Insert) {
  // Create a list
  LinkedList list = CreateLinkedList();

  unsigned int num_elems = 10;
  MyThing* elements[num_elems];
  for (unsigned int i = 0; i<num_elems; i++) {
    elements[i] = CreateMyThing(i, "bar"); 
    InsertLinkedList(list, elements[i]);
    EXPECT_EQ(i+1u, list->num_elements);
  }
    
  // Create an iter
  LLIter iter = CreateLLIter(list);
  
  // Insert a new item
  LLIterNext(iter);
  LLIterNext(iter);

  // Should be at element 7 now
  MyThing* payload;
  LLIterGetPayload(iter, (void**)&payload);
  EXPECT_EQ(7, payload->number);

  MyThing *thing11 = CreateMyThing(11, "foo"); 
  EXPECT_EQ(0, LLIterInsertBefore(iter, thing11));
  
  EXPECT_EQ(0, LLIterGetPayload(iter, (void**)&payload));
  EXPECT_EQ(7, payload->number);

  EXPECT_EQ(0, LLIterPrev(iter));
  EXPECT_EQ(0, LLIterGetPayload(iter, (void**)&payload));
  EXPECT_EQ(11, payload->number);
  
  // Check the items
  DestroyLLIter(iter);
  DestroyLinkedList(list, &DestroyMyThing);
  
}


TEST(LLIterator, IterOne) {
    LinkedList list = CreateLinkedList();

    // Add one element to the LL
    MyThing *thing1 = CreateMyThing(5, first);

    InsertLinkedList(list, thing1);

    LLIter iter = CreateLLIter(list);
    EXPECT_TRUE(iter->cur_node != NULL);

    EXPECT_EQ(1, LLIterNext(iter));
    EXPECT_TRUE(LLIterNext(iter) != 0);

    DestroyLLIter(iter);
    DestroyLinkedList(list, &DestroyMyThing);
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
