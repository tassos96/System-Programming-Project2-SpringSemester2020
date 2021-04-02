/**
 * This file contains HashTable's Bucket methods.
**/ 
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "include/hashTablesBucket.h"

//Function to initialize a new hash table's bucket
Bucket initializeBucket(int bucketSize)
{
    Bucket bucket;
    bucket = malloc(sizeof(HashTablesBucket));

    bucket->bucketCurrentEntries = 0;
    bucket->nextBucket = NULL;
    
    int freeBytes = bucketSize - sizeof(HashTablesBucket);
    int maxBucketEntries = freeBytes/sizeof(BucketEntry*);
    bucket->maxBucketEntries = maxBucketEntries;
    if (maxBucketEntries <= 0)
    {
        printf("Error: Very small bucketSize. Please try again with a bigger one\n");
        free(bucket);
        return NULL;
    }

    bucket->entries = malloc(sizeof(BucketEntry*) * maxBucketEntries);

    for (int i = 0; i < maxBucketEntries; i++)
    {
        bucket->entries[i] = initBucketEntry();
    }

    return bucket;
}

//Functio for freeing memory allocated for a bucket
void freeBucket(Bucket bucket)
{
    Bucket currentBucket = bucket;
    Bucket temporary;
    int counter = 0;
    while (currentBucket != NULL)
    {
        counter++;
        for (int i = 0; i < currentBucket->maxBucketEntries; i++)
        {
            freeBucketEntry(currentBucket->entries[i]);
        }
        free(currentBucket->entries);
        temporary = currentBucket;
        currentBucket = currentBucket->nextBucket;
        free(temporary);
    }
}

//Function to create a new bucket in list with previous one if its full
Bucket addBucketListNode(Bucket bucket)
{
    Bucket currentBucket = bucket;

    int maxEntries = currentBucket->maxBucketEntries;
    while (currentBucket->nextBucket != NULL)
    {
        currentBucket = currentBucket->nextBucket;
    }
    
    Bucket newBucket;
    newBucket = malloc(sizeof(HashTablesBucket));

    newBucket->bucketCurrentEntries = 0;
    newBucket->nextBucket = NULL;
    newBucket->maxBucketEntries = maxEntries;

    newBucket->entries = malloc(sizeof(BucketEntry*) * maxEntries);
    for (int i = 0; i < maxEntries; i++)
    {
        newBucket->entries[i] = initBucketEntry();
    }

    currentBucket->nextBucket = newBucket;
    return newBucket;
}

//Function Creating a new entry into bucket
void addBucketEntry(Bucket bucket, char* entryString, RecordListNode recordNode)
{
    Bucket currentBucket = bucket;
    BucketEntry entry = searchIntoEntriesForString(currentBucket, entryString);
    if (entry != NULL)
    {
        entry->treeRoot = insertNode(entry->treeRoot, recordNode->entryDate, recordNode);
        return;
    }

    while (currentBucket != NULL)
    {
        if (currentBucket->bucketCurrentEntries < currentBucket->maxBucketEntries)
        {
            addEntry(currentBucket->entries[currentBucket->bucketCurrentEntries], entryString, recordNode);
            currentBucket->bucketCurrentEntries++;
            return;
        }
        else if (currentBucket->bucketCurrentEntries >= currentBucket->maxBucketEntries && currentBucket->nextBucket != NULL)
        {
            currentBucket = currentBucket->nextBucket;
        }
        else if (currentBucket->bucketCurrentEntries >= currentBucket->maxBucketEntries && currentBucket->nextBucket == NULL)
        {
            currentBucket = addBucketListNode(currentBucket);
            addEntry(currentBucket->entries[currentBucket->bucketCurrentEntries], entryString, recordNode);
            currentBucket->bucketCurrentEntries++;
            return;           
        }
    }
}

//Function for searching entries and see if an entry already exists,
//if entry exists this function returns a pointer to this entry
//else it returns a NULL pointer
BucketEntry searchIntoEntriesForString(Bucket bucket, char* entryString)
{
    Bucket currentBucket = bucket;

    while (currentBucket != NULL)
    {
        for (int i = 0; i < currentBucket->bucketCurrentEntries; i++)
        {
            if (strcmp(currentBucket->entries[i]->entryString, entryString) == 0)
            {
                return currentBucket->entries[i];
            }
        }
        currentBucket = currentBucket->nextBucket;
    }
    return NULL;
}