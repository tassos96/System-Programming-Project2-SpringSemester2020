#ifndef HASH_TABLES_ENTRY_H
#define HASH_TABLES_ENTRY_H

#include "recordsList.h"
#include "avlTree.h"

typedef struct HashTablesBucketEntry
{
    char* entryString;      //String Key
    AvlTreeNode treeRoot;   //Pointer to a tree root

}
HashTablesBucketEntry;

typedef HashTablesBucketEntry* BucketEntry;

//Function to initialize a bucket entry
BucketEntry initBucketEntry();
//Function to add an entry
void addEntry(BucketEntry bucketEntry, char* entryString, RecordListNode recordNode);
//Function to free entry's memory
void freeBucketEntry(BucketEntry bucketEntry);

#endif