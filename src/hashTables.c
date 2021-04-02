/**
 * This file contains Hash Table methods implementation.
 * Creation, Insertion, Printing, Freeing and Hash Function.
**/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>

#include "include/hashTables.h"

//Function to initialize a Hash Table structure.
//hashTableSize: Number of hash table size
//bucketSize: Number of each bucket size given in Bytes
HashTable createHashTable(int hashTableSize, int bucketSize)
{
    HashTable hashTable = NULL;

    //CHECK SIZE
    if (hashTableSize < 1)
    {
        printf("Wrong HashTable size. Please try again...\n");
        return NULL;
    }
    hashTable = malloc(sizeof(HashTableStruct));
    hashTable->hashTableSize = hashTableSize;

    hashTable->bucketList = malloc(sizeof(Bucket*) * hashTableSize);

    for (int i = 0; i < hashTableSize; i++)
    {
        hashTable->bucketList[i] = initializeBucket(bucketSize);
        if (hashTable->bucketList[i] == NULL)
        {
            free(hashTable->bucketList);
            free(hashTable);
            return NULL;
        }
        
    }

    return hashTable;    
}

//Hash function for hashTables
int hashFunction(HashTable hashTable, char *key) 
{
	unsigned long int hashval = 0;
	int i = 0;

	//Convert our string to an integer
	while( hashval < ULONG_MAX && i < strlen( key ) ) {
		hashval = hashval << 8;
		hashval += key[ i ];
		i++;
	}

	return hashval % hashTable->hashTableSize;  //Return proper index
}

//Function to free memory allocated for the Hash Table structure (HashTable, Buckets, Entries, AvlTree)
void freeHashTable(HashTable hashTable)
{
    for (int i = 0; i < hashTable->hashTableSize; i++)
    {
        freeBucket(hashTable->bucketList[i]);
    }
    free(hashTable->bucketList);
}

//Function for record addition into Hash Table
void addToHashTable(HashTable hashTable, char* stringKey, RecordListNode recordNode)
{
    int hashValue = hashFunction(hashTable, stringKey);
    addBucketEntry(hashTable->bucketList[hashValue], stringKey, recordNode);
}

//Function to print in a pretty way a hash table structure (HashTable, Bucket, Entries, AvlTree)
void printHashTable(HashTable hashTable)
{
    Bucket temporary;
    for(int i=0; i<hashTable->hashTableSize; i++)
    {
        printf("---- Hash Table List_%d ----\n", i);
        int bucket_counter = 0;
        temporary = hashTable->bucketList[i];
        while (temporary != NULL)
        {
            bucket_counter++;
            printf("\t ---- Bucket_%d ----\n", bucket_counter);
            for(int j = 0; j<temporary->maxBucketEntries; j++)
            {
                printf("\t\t -> StringKey: %s\n", temporary->entries[j]->entryString);
                printf("\t\t\t Tree: \n");
                //printPreOrder(temporary->entries[j]->treeRoot);
                printInOrder(temporary->entries[j]->treeRoot);
                printf("\n");
            }
            temporary = temporary->nextBucket;
        }
    }
}