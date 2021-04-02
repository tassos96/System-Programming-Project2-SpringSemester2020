#ifndef HASH_TABLES_H
#define HASH_TABLES_H

#include "hashTablesBucket.h"
#include "recordsList.h"

typedef struct HashTableStruct
{
    int hashTableSize;
    Bucket* bucketList;

}HashTableStruct;

typedef HashTableStruct* HashTable;

//Function to initialize a Hash Table structure.
HashTable createHashTable(int hashTableSize, int bucketSize);
//Hash function for hashTables
int hashFunction(HashTable hashTable, char *key);
//Function to free memory allocated for the Hash Table structure (HashTable, Buckets, Entries, AvlTree)
void freeHashTable(HashTable hashTable);

//Function for record addition into Hash Table
void addToHashTable(HashTable hashTable, char* stringKey, RecordListNode recordNode);
//Function to print in a pretty way a hash table structure (HashTable, Bucket, Entries, AvlTree)
void printHashTable(HashTable hashTable);

#endif