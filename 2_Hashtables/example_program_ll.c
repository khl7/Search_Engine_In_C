// CS 5007, Northeastern University, Seattle
// Spring 2019
// Adrienne Slaughter
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
#include "Assert007.h"

#include <stdio.h>
#include <stdlib.h>

struct myThing {
    int number;
    char *name;
};

typedef struct myThing MyThing;

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


void PrintMyThing(MyThing* aThing) {
    printf("[%s, %d]", aThing->name, aThing->number);
}

int main(int argc, char* argv[]) {
    printf("Creating a new LinkedList\n");
    // Allocate LL
    LinkedList list = CreateLinkedList();
    
    unsigned int num_items = 15; 
    for (unsigned int i=0; i<num_items; i++) {
      MyThing *thing = (MyThing*)malloc(sizeof(MyThing));
      // TODO: Check that thing isn't NULL (out of memory)
      thing->number = i;
      thing->name = "Foobar";
      int result = InsertLinkedList(list, thing);
      Assert007(result == 0); // success!
      Assert007(NumElementsInLinkedList(list) == i+1); 
    }
    
    // Create an iterator
    LLIter iter = CreateLLIter(list);
    
    // Iterate through the list and print out the items. 
    while (LLIterHasNext(iter)) {
        MyThing *item; 
        LLIterGetPayload(iter, (void*)&item);
        PrintMyThing(item);
        printf("\n");
        LLIterNext(iter);
    }

    DestroyLLIter(iter); 
    
    // Since every element is inserted at the head,
    // the list is by default reverse sorted
    SortLinkedList(list, 1, &CompareMyThing);
    
    MyThing* payload;
    Assert007(PopLinkedList(list, (void**)&payload) == 0);
    Assert007(payload->number == 0);

    // Now that I've popped it, I have to remember to free it
    DestroyMyThing(payload); 
    
    // Create a new iter and print out again
    iter = CreateLLIter(list); 

    // Iterate through the list
    while (LLIterHasNext(iter)) {
        MyThing *item; 
        LLIterGetPayload(iter, (void*)&item);
        PrintMyThing(item);
        printf("\n");
        LLIterNext(iter);
    }

    // Clean up
    DestroyLLIter(iter);
    
        DestroyLinkedList(list, &DestroyMyThing);
    printf("done. \n"); 
}
