#ifndef HASH_TABLES_BUCKET_H
#define HASH_TABLES_BUCKET_H

#include "hashTablesBucketEntry.h"


typedef struct HashTablesBucket
{
    int bucketCurrentEntries;
    int maxBucketEntries;
    struct HashTablesBucket* nextBucket;
    
    //Array of Pointers to entries (Each array position, one entry)
    BucketEntry* entries;

}HashTablesBucket;

typedef HashTablesBucket* Bucket;

//Function to initialize a new hash table's bucket
Bucket initializeBucket(int bucketSize);
//Function for freeing memory allocated for a bucket
void freeBucket(Bucket bucket);

//Function to create a new bucket in list with previous one if its full
Bucket addBucketListNode(Bucket bucket);
//Function Creating a new entry into bucket
void addBucketEntry(Bucket bucket, char* entryString, RecordListNode recordNode);
//Function for searching entries and see if an entry already exists,
BucketEntry searchIntoEntriesForString(Bucket bucket, char* entryString);


#endif