/**
 * This file contains each HashTable's Bucket Entry methods.
**/ 
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "include/hashTablesBucketEntry.h"

//Function to initialize a bucket entry
BucketEntry initBucketEntry()
{
    BucketEntry bucketEntry;
    bucketEntry = malloc(sizeof(HashTablesBucketEntry));
    bucketEntry->entryString = NULL;
    bucketEntry->treeRoot = NULL;
    return bucketEntry;
}

//Function to add an entry
void addEntry(BucketEntry bucketEntry, char* entryString, RecordListNode recordNode)
{
    bucketEntry->entryString = malloc(sizeof(char*) * strlen(entryString) + 1);
    strcpy(bucketEntry->entryString, entryString);

    bucketEntry->treeRoot = insertNode(bucketEntry->treeRoot, recordNode->entryDate, recordNode);
}

//Function to free entry's memory
void freeBucketEntry(BucketEntry bucketEntry)
{
    free(bucketEntry->entryString);
    freeAvlTree(bucketEntry->treeRoot);
    free(bucketEntry);
}
