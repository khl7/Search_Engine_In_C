#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#include "Hashtable.h"
#include "LinkedList.h"

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

void DestroyMyThing(MyThing* thing) {
  free(thing);
  thing = NULL;
}

void DestroyThing(void* thing) {
  free(thing);
  thing = NULL;
}


typedef struct {
  int   num;    
} someNum, *SomeNumPtr;

void FreeSomeNumPtr(void *someNumPtr) {
  free(someNumPtr); 
}

void ex2(){
  Hashtable ht = CreateHashtable(5);
  MyThing *thing1 = CreateMyThing(5, first);

  // Make KeyValue Pair
  HTKeyValue kv;
  HTKeyValue old_kv;
  
  kv.key = FNVHashInt64(thing1->number);
  kv.value = thing1;
  PutInHashtable(ht, kv, &old_kv);

  HTKeyValue junk;
  RemoveFromHashtable(ht, kv.key, &junk);

  // Now that I've removed it from the HT, I need to destroy it
  DestroyMyThing(junk.value); 

  DestroyHashtable(ht, &DestroyThing);
}

void hhIterTesting() {
  const int num_items = 15; 
  Hashtable ht = CreateHashtable(5);

  HTKeyValuePtr kv; 

  printf("Inserting elements into the hashtable\n");
  
  for (int i = 0; i < num_items; i++) {
    // do the insert
    SomeNumPtr np = (SomeNumPtr)(malloc(sizeof(someNum)));
    HTKeyValue old, newkv;
   
    np->num = (i);
    newkv.key = i;
    newkv.value = (void *)(np);
    PutInHashtable(ht, newkv, &old);
  }

  printf("Creating an iterator for the HT\n"); 
  HTIter iter = CreateHashtableIterator(ht);

  printf("Number elements in Hashtable: %d\n", NumElemsInHashtable(ht));
  
  // Now, get each item, but make sure that
  // all items only get returned once.
  int payloadCounts[15] = { 0 };

  HTKeyValue np; 
  for (int i=0; i<num_items; i++){
    HTIteratorGet(iter, &np);
    printf("Got num: %d \n", ((SomeNumPtr)np.value)->num); 
    payloadCounts[((SomeNumPtr)np.value)->num]++;
    HTIteratorNext(iter);
  }
  
  printf("Done iterating. \n"); 
  DestroyHashtableIterator(iter);

  DestroyHashtable(ht, &FreeSomeNumPtr); 
}


int main(){
  Hashtable ht = CreateHashtable(5);
  DestroyHashtable(ht, NULL);

  hhIterTesting(); 
  ex2(); 
  return 0; 
}

