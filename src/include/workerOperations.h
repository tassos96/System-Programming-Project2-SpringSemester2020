#ifndef WORKER_OPERATIONS_H
#define WORKER_OPERATIONS_H

#include "recordsList.h"
#include "hashTables.h"

//==============================================    WORKERS    ==============================================//

//Function for worker to open his pipes that already has been created
int workerOpenPipes(int* readFD, int* writeFD, int curWorker);

//Function for all the worker operations
int workerOperations(int readFD, int writeFD, int bufferSize, int input_dirLen);

//Function for message reading from parent
int readFromParent(int readFD, int msgTotalBytes, char *message, int bufferSize);

//Function for writting message to parent
int writeToParent(int writeFD, char* msgToSend);

//Function to free some memory allocated for records storage structures, paths and countries
void freeWorkerMemory(HashTable diseaseHashTable, HashTable countryHashTable, RecordsList recordsList, int totalDirPathsToOpen, char **countries, char **dirPathsArray);

//Function that executes requests from parent
int waitAndExecuteRequests(RecordsList recordsList, int readFD, int writeFD, int bufferSize, char** countries, int totalDirPathsToOpen, int* totalReceived, int* totalSuccess, int* totalFails);

//Function used for worker log file creation
void writeWorkerLogFile(char** countries, int totalCountries, int total, int success, int fail);

#endif