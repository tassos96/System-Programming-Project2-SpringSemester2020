#ifndef PARENT_OPERATIONS_H
#define PARENT_OPERATIONS_H

#include "statsList.h"

//==============================================    PARENT    ==============================================//

//Function for parent to open his pipes that already has been created
int parentOpenPipes(int numWorkers, int** parentWriteFDs, int** parentReadFDs);

//Function for all the parent operations
int parentOperations(int numWorkers, int* parentWriteFDs, int* parentReadFds, char** directoriesNames, int totalDirectories, int bufferSize, int input_dirLen);

//Function for writting message to worker
int writeToWorker(int writeFd, char* msgToSend);

//Function for message reading from worker
int readFromWorker(int readFd,  int msgTotalBytes, char* message, int bufferSize);

//Function to get maximum fd for select
void getMaximumFD(int* maximumFD, int* parentWriteFDs, int* parentReadFDs, int numWorkers);

//Function that take tokens of stats message and store them into stats list
void storeStats(StatisticsList statsList, char* message);

//Take tokens and store to list
void tokStats(StatisticsList statsList, char* country, char* date, char** copyOfDiseaseStats, int diseasesStatsCounter);

#endif