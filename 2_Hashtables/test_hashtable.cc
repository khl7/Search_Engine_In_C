// CS 5007, Northeastern University, Seattle
// Spring 2019
// Adrienne Slaughter
//
// Assignment 7
#include "gtest/gtest.h"
extern "C" {
    #include "Hashtable.h"
    #include "Hashtable_priv.h"
    #include "LinkedList.h"
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

typedef struct {
  int num;
} someNum, *SomeNumPtr;

void FreeSomeNumPtr(void *someNumPtr) {
  free(someNumPtr);
}

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

TEST(Hashtable, Create) {
  Hashtable ht = CreateHashtable(5);
  EXPECT_FALSE(ht == NULL);
  EXPECT_FALSE(ht->buckets == NULL);
  EXPECT_EQ(NumElemsInHashtable(ht), 0);
  DestroyHashtable(ht, NULL);
  
}

TEST(Hashtable, AddOneRemoveOne) {
    Hashtable ht = CreateHashtable(5);
    MyThing *thing1 = CreateMyThing(5, first);

    EXPECT_EQ(NumElemsInHashtable(ht), 0);

    // Make KeyValue Pair
    HTKeyValue kv, old_kv;

    kv.key = FNVHashInt64(thing1->number);
    kv.value = thing1;
    PutInHashtable(ht, kv, &old_kv);
    EXPECT_EQ(NumElemsInHashtable(ht), 1);

    HTKeyValue junk;
    RemoveFromHashtable(ht, kv.key, &junk); 

    DestroyThing(junk.value);
    
    EXPECT_EQ(NumElemsInHashtable(ht), 0);

    DestroyHashtable(ht, &DestroyThing);
}

TEST(Hashtable, AddOneElemDestroy) {
    Hashtable ht = CreateHashtable(5);
    MyThing *thing1 = CreateMyThing(5, first);

    EXPECT_EQ(NumElemsInHashtable(ht), 0);

    // Make KeyValue Pair
    HTKeyValue kv, old_kv;

    kv.key = FNVHashInt64(thing1->number);
    kv.value = thing1;
    PutInHashtable(ht, kv, &old_kv);

    EXPECT_EQ(NumElemsInHashtable(ht), 1);

    DestroyHashtable(ht, &DestroyThing);
}

TEST(Hashtable, AddOneElemTwoTimes) {
    Hashtable ht = CreateHashtable(5);
    MyThing *thing1 = CreateMyThing(5, first);
    MyThing *thing2 = CreateMyThing(5, second);

    EXPECT_EQ(NumElemsInHashtable(ht), 0);

    // Make KeyValue Pair
    HTKeyValue kv;
    HTKeyValue old_kv;
    old_kv.value = NULL;
    
    kv.key = FNVHashInt64(500);
    kv.value = thing1;

    int result = PutInHashtable(ht, kv, &old_kv);

    EXPECT_EQ(result, 0);
    EXPECT_EQ(NumElemsInHashtable(ht), 1);

    // Trying to put it in again should result in an error
    result = PutInHashtable(ht, kv, &old_kv);
    EXPECT_EQ(result, 2);
    EXPECT_EQ(NumElemsInHashtable(ht), 1);

    // Also want to try a different element with the same key (diff val)
    HTKeyValue kv2;
    kv2.key = FNVHashInt64(500);
    kv2.value = thing2;

    result = PutInHashtable(ht, kv2, &old_kv);
    EXPECT_EQ(result, 2);
    EXPECT_EQ(NumElemsInHashtable(ht), 1);

    // Because this was replaced, gotta free the value
    EXPECT_EQ(old_kv.value, thing1); 
    DestroyThing(old_kv.value);
    
    DestroyHashtable(ht, &DestroyThing);
    // thing2 is destroyed as part of the HT
}

TEST(Hashtable, AddOneRemoveTwice) {
    Hashtable ht = CreateHashtable(5);
    MyThing *thing1 = CreateMyThing(5, first);

    EXPECT_EQ(NumElemsInHashtable(ht), 0);

    // Make KeyValue Pair
    HTKeyValue kv, old_kv;
    kv.key = FNVHashInt64(500);
    kv.value = thing1;

    int result = PutInHashtable(ht, kv, &old_kv);

    EXPECT_EQ(result, 0);
    EXPECT_EQ(NumElemsInHashtable(ht), 1);

    HTKeyValue junk;
    result = RemoveFromHashtable(ht, kv.key, &junk); 

    EXPECT_EQ(result, 0);
    EXPECT_EQ(NumElemsInHashtable(ht), 0);
    EXPECT_EQ(junk.value, thing1); 
    
    // Now that I've removed it from the table, I need to free it
    DestroyMyThing((MyThing*)junk.value); 

    result = RemoveFromHashtable(ht, kv.key, &junk); 
    EXPECT_EQ(result, -1);
    EXPECT_EQ(NumElemsInHashtable(ht), 0);

    DestroyHashtable(ht, &DestroyThing);
}

TEST(Hashtable, AddMultipleItems) {
    Hashtable ht = CreateHashtable(100);
    MyThing *thing1 = CreateMyThing(5, first);
    MyThing *thing2 = CreateMyThing(395, second);
    MyThing *thing3 = CreateMyThing(18439286, third);

    EXPECT_EQ(NumElemsInHashtable(ht), 0);

    // Make KeyValue Pair
    HTKeyValue kv;

    HTKeyValue old_kv; 
    
    kv.key = FNVHashInt64(thing1->number);
    kv.value = thing1;
    PutInHashtable(ht, kv, &old_kv);

    ASSERT_EQ(NumElemsInHashtable(ht), 1);

    kv.key = FNVHashInt64(thing2->number);
    kv.value = thing2;
    PutInHashtable(ht, kv, &old_kv);

    ASSERT_EQ(NumElemsInHashtable(ht), 2);

    kv.key = FNVHashInt64(thing3->number);
    kv.value = thing3;
    PutInHashtable(ht, kv, &old_kv);

    ASSERT_EQ(NumElemsInHashtable(ht), 3);

    DestroyHashtable(ht, &DestroyThing);
}

TEST(Hashtable, LookupInHashtable) {
    Hashtable ht = CreateHashtable(100);
    MyThing *thing1 = CreateMyThing(5, first);
    MyThing *thing2 = CreateMyThing(381937362, second);
    MyThing *thing3 = CreateMyThing(9284, third);

    EXPECT_EQ(NumElemsInHashtable(ht), 0);

    // Make KeyValue Pair
    HTKeyValue kv1;
    HTKeyValue old_kv;
    
    kv1.key = FNVHashInt64(thing1->number);
    kv1.value = thing1;
    PutInHashtable(ht, kv1, &old_kv);

    ASSERT_EQ(NumElemsInHashtable(ht), 1);

    HTKeyValue kv2;
    kv2.key = FNVHashInt64(thing2->number);
    kv2.value = thing2;
    PutInHashtable(ht, kv2, &old_kv);
    
    ASSERT_EQ(NumElemsInHashtable(ht), 2);

    HTKeyValue kv3;
    kv3.key = FNVHashInt64(thing3->number);
    kv3.value = thing3;
    PutInHashtable(ht, kv3, &old_kv);

    ASSERT_EQ(NumElemsInHashtable(ht), 3);

    HTKeyValue theThing;
    theThing.key = 0u;
    theThing.value = NULL; 

    // Now, lookup:
    int result = LookupInHashtable(ht,
        FNVHashInt64(thing2->number),
        &theThing);
    EXPECT_EQ(result, 0);
    EXPECT_EQ(theThing.key, FNVHashInt64(thing2->number));
    EXPECT_EQ(theThing.value, thing2);

    EXPECT_EQ(NumElemsInHashtable(ht), 3);

    // TODO: Test looking up something not in the table

    DestroyHashtable(ht, &DestroyThing);
}

TEST(Hashtable, TwoElemsOneBucket) {
    Hashtable ht = CreateHashtable(15);
    MyThing *thing1 = CreateMyThing(5, first);
    MyThing *thing2 = CreateMyThing(381937362, second);

    EXPECT_EQ(NumElemsInHashtable(ht), 0);

    // Make KeyValue Pair
    HTKeyValue kv, kv2;
    HTKeyValue old_kv;
    
    kv.key = 5;
    kv.value = thing1;
    PutInHashtable(ht, kv, &old_kv);

    ASSERT_EQ(NumElemsInHashtable(ht), 1);

    kv2.key = 20;
    kv2.value = thing2;
    PutInHashtable(ht, kv2, &old_kv);

    ASSERT_EQ(NumElemsInHashtable(ht), 2);

    HTKeyValue theThing;
    int result = LookupInHashtable(ht,
        5,
        &theThing);
    ASSERT_EQ(result, 0);
    ASSERT_EQ(theThing.key, 5u);
    ASSERT_EQ(theThing.value, thing1);

    result = LookupInHashtable(ht,
                               20,
                               &theThing);
    ASSERT_EQ(result, 0);
    ASSERT_EQ(theThing.key, 20u);
    ASSERT_EQ(theThing.value, thing2);

    DestroyHashtable(ht, &DestroyThing);
}

TEST(Hashtable, Resize) {
  Hashtable ht = CreateHashtable(15);
  
  for (unsigned int i = 0; i < 60; i++) {
    // do the insert
    SomeNumPtr np = static_cast<SomeNumPtr>(malloc(sizeof(someNum)));
    HTKeyValue old, newkv, old_kv;
    assert(np != NULL);
    np->num = static_cast<int>(i);
    newkv.key = i;
    newkv.value = static_cast<void *>(np);
    ASSERT_EQ(0, PutInHashtable(ht, newkv, &old_kv));

    //printf("i = %d\n", i); 

    // test double insert
    ASSERT_EQ(2, PutInHashtable(ht, newkv, &old_kv));

    // test lookup
    old.key = 1;
    old.value = NULL;
    ASSERT_EQ(0, LookupInHashtable(ht, i, &old));
    ASSERT_EQ(i, old.key);
    ASSERT_EQ(static_cast<void *>(np), old.value);

    // test bad lookup
    ASSERT_EQ(-1, LookupInHashtable(ht, i+1, &old));

    // test bad remove
    ASSERT_EQ(-1, RemoveFromHashtable(ht, i+1, &old)); 

    // test good remove and reinsert
    old.key = -100;
    old.value = NULL;
    ASSERT_EQ(0, RemoveFromHashtable(ht, i, &old));
    ASSERT_EQ(i, old.key);
    ASSERT_EQ(static_cast<void *>(np), old.value);
    ASSERT_EQ(i, (unsigned)NumElemsInHashtable(ht));
    ASSERT_EQ(0, PutInHashtable(ht, newkv, &old_kv));
    ASSERT_EQ(2, PutInHashtable(ht, newkv, &old_kv));
    ASSERT_EQ(i+1, (unsigned)NumElemsInHashtable(ht));
  }
    DestroyHashtable(ht, &DestroyThing);
}

TEST(HashtableIter, CreateDestroy) {
  // First create a hashtable that's empty
  Hashtable ht = CreateHashtable(5);

  // Create an iter, Destroy it
  HTIter iter = CreateHashtableIterator(ht);

  ASSERT_EQ(NULL, iter); 

  DestroyHashtable(ht, &DestroyThing); 
}

TEST(HashtableIter, AddAndIterate) {
  const int num_items = 10; //45;
  Hashtable ht = CreateHashtable(5);
  
  for (int i = 0; i < num_items; i++) {
    // do the insert
    SomeNumPtr np = static_cast<SomeNumPtr>(malloc(sizeof(someNum)));
    HTKeyValue newkv, old_kv;
    assert(np != NULL);
    np->num = static_cast<int>(i);
    newkv.key = i;
    newkv.value = static_cast<void *>(np);
    ASSERT_EQ(0, PutInHashtable(ht, newkv, &old_kv));
  }
  
  ASSERT_EQ(num_items, NumElemsInHashtable(ht)); 
  
  HTIter iter = CreateHashtableIterator(ht);
  
  // Now, get each item, but make sure that
  // all items only get returned once.
  int payload_counts[num_items] = { 0 };
  ASSERT_NE(0, HTIteratorHasMore(iter));
  
  HTKeyValue item;
  //  int result = HTIteratorGet(it, item);

  for (int i = 0; i < num_items; i++) {
    int result = HTIteratorGet(iter, &item);
    ASSERT_EQ(0, result);
    
    payload_counts[((SomeNumPtr)(item.value))->num]++;
    if (i < (num_items -1)) {
      ASSERT_NE(0, HTIteratorHasMore(iter));
    }
    else {
      ASSERT_EQ(0, HTIteratorHasMore(iter));
    }
    HTIteratorNext(iter); 
  }
  for (int i = 0; i < num_items; i++) {
    ASSERT_EQ(1, payload_counts[i]);
  }

  ASSERT_EQ(0, HTIteratorHasMore(iter));
  DestroyHashtableIterator(iter);
  DestroyHashtable(ht, &FreeSomeNumPtr);
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
