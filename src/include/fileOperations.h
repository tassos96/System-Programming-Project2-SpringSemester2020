#ifndef FILE_OPERATIONS_H
#define FILE_OPERATIONS_H

#include "recordsList.h"
#include "hashTables.h"

//Function to get all directories name into a string array
char** getDirectories(char* inputDirPath, int* totalDirectories);

//Function to get file names, store them to sting array
char** getDateFileNames(char* inputCountryDirPath, int* totalFileNames);

//Function to read records from file line by line
int readRecordsFromFile(FILE** filePointer, HashTable diseaseHashTable, HashTable countryHashTable, RecordsList recordsList, char* fileNamePath, char* countryDirName, char* dateFileName, int* successCounter, int* failCounter);

//Function to tokenize each line and then add this record at each structure(RecordsList and the hashTables)
int getFileLineTokens(char* line, RecordsList recordsList, HashTable diseaseHashTable, HashTable countryHashTable, char* countryDirName, char* dateFileName);

//Function to free some allocated memory
void freeFileReaderVariables(char* recordId, char* status, char* patientFirstName, char* patientLastName, char* diseaseId, char* country, char* entryDateString, char* exitDateString, char* copyOfLine);

//Function to get digits of an int
int howManyDigits(int integer);

#endif