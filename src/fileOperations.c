/**
 * This file contains all directories and files operations.
 * */ 
#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <sys/types.h>
#include <dirent.h>

#include "include/fileOperations.h"
#include "include/myDateStructure.h"
#include "include/recordsList.h"

#define SUCCESS 1
#define ERROR -1


static int stringCompare(const void* a, const void* b);

//Function to get all directories name into a string array
char** getDirectories(char* inputDirPath, int* totalDirectories)
{
    struct dirent *de;

    DIR *dr = opendir(inputDirPath);
    if (dr == NULL)
    {
        printf("Can't open current directory\n");
        return NULL;
    }

    int counter = 0;
    while ((de = readdir(dr)) != NULL )
    {
        if((strcmp(".", de->d_name) == 0) || (strcmp("..", de->d_name) == 0))
        {
            continue;
        }
        counter++;
    }

    char** directoriesNameArray;
    directoriesNameArray = malloc(sizeof(char*) * counter);

    rewinddir(dr);  //set dir pointer at start of stream

    int pos = 0;
    while ((de = readdir(dr)) != NULL )
    {
        if((strcmp(".", de->d_name) == 0) || (strcmp("..", de->d_name) == 0))
        {
            continue;
        }
        directoriesNameArray[pos] = malloc(sizeof(char) * ( strlen(inputDirPath) + strlen("/") + strlen(de->d_name) + 1));
        sprintf(directoriesNameArray[pos], "%s/%s", inputDirPath, de->d_name);
        pos++;
    }

    closedir(dr);

    qsort(directoriesNameArray, counter, sizeof(const char*), stringCompare);

    (*totalDirectories) = counter;
    return directoriesNameArray;
}

//String alphabetical order
static int stringCompare(const void* a, const void* b)
{
    return strcmp(*(const char**)a, *(const char**)b); 
}

//Compare dates(dd-mm-yyyy) chronologically
static int dateStringCompare(const void* a, const void* b)
{
    int strlenA = strlen(*(const char**)a);
    int strlenB = strlen(*(const char**)b);

    int datesLen = 10;
    char dateStrA[datesLen + 1]; // +1 for '\0'
    char dateStrB[datesLen + 1]; 

    int offsetA = strlenA - datesLen;
    int offsetB = strlenB - datesLen;
    
    strncpy(dateStrA, *(const char**)a + offsetA, datesLen);
    dateStrA[datesLen] = '\0';
    strncpy(dateStrB, *(const char**)b + offsetB, datesLen);
    dateStrB[datesLen] = '\0';

    char* dd_A;
    char* dd_B;
    char* mm_A;
    char* mm_B;
    char* yyyy_A;
    char* yyyy_B;

    dd_A = strtok(dateStrA, "-");
    mm_A = strtok(NULL, "-");
    yyyy_A = strtok(NULL, "-");

    dd_B = strtok(dateStrB, "-");
    mm_B = strtok(NULL, "-");
    yyyy_B = strtok(NULL, "-");

    int comp_year = strcmp(yyyy_A, yyyy_B);
    if( comp_year == 0 )    //yyyy_A = yyyy_B
    {
        int comp_month = strcmp(mm_A, mm_B);
        if(comp_month == 0) //mm_A = mm_B
        {
            int comp_day = strcmp(dd_A, dd_B);
            if (comp_day == 0) //dd_A = dd_B
            {
                return 0;
            }
            else if(comp_day < 0)   //dd_A < dd_B
            {
                return -1;
            }
            else //if(comp_day > 0)   //dd_A > dd_B
            {
                return 1;
            }
        }
        else if(comp_month < 0) //mm_A < mm_B
        {
            return -1;
        }
        else //if(comp_month > 0) //mm_A > mm_B
        {
            return 1;
        }
    }
    else if(comp_year < 0) //yyyy_A < yyyy_B
    {
        return -1;
    }
    else //if(comp_year > 0)  //yyyyA > yyyy_B
    {
        return 1;
    }
     
}

//Function to get file names, store them to sting array
char** getDateFileNames(char* inputCountryDirPath, int* totalFileNames)
{
    struct dirent *de;
    
    DIR *dr = opendir(inputCountryDirPath);
    if(dr == NULL)
    {
        printf("Can't open current directory\n");
        return NULL;
    }

    int counter = 0;
    while ((de = readdir(dr)) != NULL)
    {
        if((strcmp(".", de->d_name) == 0) || (strcmp("..", de->d_name) == 0))
        {
            continue;
        }
        counter++;
    }
    
    char** dateFileNames = malloc(sizeof(char*) * counter);

    rewinddir(dr);

    int pos = 0;
    while((de = readdir(dr)) != NULL)
    {
        if((strcmp(".", de->d_name) == 0) || (strcmp("..", de->d_name) == 0))
        {
            continue;
        }
        dateFileNames[pos] = malloc(sizeof(char) * (strlen(inputCountryDirPath) + strlen("/") + strlen(de->d_name) + 1));
        sprintf(dateFileNames[pos], "%s/%s", inputCountryDirPath, de->d_name);
        pos++;
    }

    closedir(dr);

    qsort(dateFileNames, counter, sizeof(const char*), dateStringCompare);

    (*totalFileNames) = counter;
    return dateFileNames;
}

//Function to read records from file line by line
int readRecordsFromFile(FILE** filePointer, HashTable diseaseHashTable, HashTable countryHashTable, RecordsList recordsList, char* fileNamePath, char* countryDirName, char* dateFileName, int* successCounter, int* failCounter)
{
    char* line = NULL;
    size_t length = 0;
    ssize_t read;

    *filePointer = fopen(fileNamePath, "r");   //Open file with read access rights
    if (*filePointer == NULL)   //Error: File with this name doen's exist
    {
        perror("Read date file: ");
        return ERROR;
    }

    while ((read = getline(&line, &length, *filePointer)) != -1)    //Get a line from file
    {
        line[strlen(line)-1] = '\0';
        int returnCode = getFileLineTokens(line, recordsList, diseaseHashTable, countryHashTable, countryDirName, dateFileName);
        if(returnCode == ERROR) //If return code is ERROR then either we have a duplicate ID or we have a wrong dates order
        {
            (*failCounter)++;
            printf("ERROR\n");
        }
        (*successCounter)++;
    }

    fclose(*filePointer);
    if(line)
    {
        free(line);
    }
    return SUCCESS;
}

//Function to tokenize each line and then add this record at each structure(RecordsList and the hashTables)
int getFileLineTokens(char* line, RecordsList recordsList, HashTable diseaseHashTable, HashTable countryHashTable, char* countryDirName, char* dateFileName)
{    
    //Copy of each line to get tokens from
    char* copyOfLine = malloc(sizeof(char*) * strlen(line)+1);
    strcpy(copyOfLine, line);

    char* record;   //contains a token from strtok
    char* recordId = NULL;
    char* status = NULL;
    char* patientFirstName = NULL;
    char* patientLastName = NULL;
    char* diseaseId = NULL;
    int patientAge = -1;
    
    //HAVE TO GET THIS FROM DIR NAME AND FILENAME
    char* country = NULL;
    char* entryDateString = NULL;
    char* exitDateString = NULL;

    
    record = strtok(copyOfLine, " "); //get first token
    recordId = malloc(sizeof(char*) * strlen(record) + 1);
    strcpy(recordId, record);   //copy it to recordId

    record = strtok(NULL, " "); //get first token
    status = malloc(sizeof(char*) * strlen(record) + 1);
    strcpy(status, record);   //copy it to status

    record = strtok(NULL, " ");
    patientFirstName = malloc(sizeof(char*) * strlen(record) + 1);
    strcpy(patientFirstName, record);

    record = strtok(NULL, " ");
    patientLastName = malloc(sizeof(char*) * strlen(record) + 1);
    strcpy(patientLastName, record);

    record = strtok(NULL, " ");
    diseaseId = malloc(sizeof(char*) * strlen(record) + 1);
    strcpy(diseaseId, record);    

    record = strtok(NULL, " ");
    patientAge = atoi(record);

    country = malloc(sizeof(char*) * strlen(countryDirName) + 1);
    strcpy(country, countryDirName);
    
    DateStructure  entryDate;//Convert date string to date structure
    DateStructure exitDate;   //Convert date string to date structure

    if(strcmp(status,"ENTER") == 0)
    {
        entryDateString = malloc(sizeof(char*) * strlen(dateFileName) + 1);
        strcpy(entryDateString, dateFileName);
        entryDate = setDate(entryDateString);
        exitDate = setDate("-");
        //Add to records list
        RecordListNode recordNode = checkId(recordsList, recordId);
        if(recordNode == NULL)  //This id doesn't exist so add it
        {
            addRecordToList(recordsList, recordId, status, patientFirstName, patientLastName, diseaseId, patientAge, country, entryDate, exitDate);

            //Insert to Hash Table
            addToHashTable(diseaseHashTable, diseaseId, recordsList->tailNode);
            //Insert to countryHashTable
            addToHashTable(countryHashTable, country, recordsList->tailNode);
        }
        else    //This id already exists
        {
            freeFileReaderVariables(recordId, status, patientFirstName, patientLastName, diseaseId, country, entryDateString, exitDateString, copyOfLine);
            return ERROR;
        }
    }
    else if (strcmp(status, "EXIT") == 0)
    {
        RecordListNode recordNode = checkId(recordsList, recordId);
        if( recordNode == NULL)
        {
            freeFileReaderVariables(recordId, status, patientFirstName, patientLastName, diseaseId, country, entryDateString, exitDateString, copyOfLine);
            return ERROR;
        }
        else if( recordNode != NULL )
        {
            //check if all attributes are the same
            if((strcmp(recordNode->status, "ENTER") == 0) && (strcmp(recordNode->patientFirstName, patientFirstName)==0) && (strcmp(recordNode->patientLastName, patientLastName) == 0) && (strcmp(recordNode->diseaseID, diseaseId) == 0) && (strcmp(recordNode->country, country) == 0) )
            {
                exitDateString =  malloc(sizeof(char*) * strlen(dateFileName) + 1);
                strcpy(exitDateString, dateFileName);

                //entryDate = setDate("-");
                exitDate = setDate(exitDateString);
                if(datesOrderValidation(recordNode->entryDate, exitDate) == ERROR)  //NOT CORRECT DATE ORDER
                {
                    freeFileReaderVariables(recordId, status, patientFirstName, patientLastName, diseaseId, country, entryDateString, exitDateString, copyOfLine);
                    return ERROR;
                }
                else
                {
                    recordNode->exitDate = exitDate;
                }
            }
            else //NOT SAME RECORD INFO
            {
                freeFileReaderVariables(recordId, status, patientFirstName, patientLastName, diseaseId, country, entryDateString, exitDateString, copyOfLine);
                return ERROR;
            }
            
        }
    }    
    
    freeFileReaderVariables(recordId, status, patientFirstName, patientLastName, diseaseId, country, entryDateString, exitDateString, copyOfLine);

    return SUCCESS;
}

//Function to free some allocated memory
void freeFileReaderVariables(char* recordId, char* status, char* patientFirstName, char* patientLastName, char* diseaseId, char* country, char* entryDateString, char* exitDateString, char* copyOfLine)
{
    free(recordId);
    free(status);
    free(patientFirstName);
    free(patientLastName);
    free(diseaseId);
    free(country);
    if(entryDateString != NULL)
        free(entryDateString);
    if(exitDateString != NULL)
        free(exitDateString);
    free(copyOfLine);
}

//Function to get digits of an int
int howManyDigits(int integer)
{
    if(integer != 0)
        return floor(log10(abs(integer))) + 1;
    else
        return 1;
}