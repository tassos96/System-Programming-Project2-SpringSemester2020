/**
 * This files contains method for a records list node initialization. 
**/ 
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "include/record.h"

//Function to initialize a records list node
void initializeRecord(RecordListNode record, char* recordID, char* status, char* patientFirstName, char* patientLastName, char* diseaseID, int patientAge, char* country, DateStructure entryDate, DateStructure exitDate)
{
    record->recordID = malloc(sizeof(char*) * strlen(recordID) + 1);
    strcpy(record->recordID, recordID);

    record->status = malloc(sizeof(char*) * strlen(status) + 1);
    strcpy(record->status, status);
    
    record->patientFirstName = malloc(sizeof(char*) * strlen(patientFirstName) +1);
    strcpy(record->patientFirstName, patientFirstName);

    record->patientLastName = malloc(sizeof(char*) * strlen(patientLastName) + 1);
    strcpy(record->patientLastName, patientLastName);

    record->diseaseID = malloc(sizeof(char*) * strlen(diseaseID) + 1);
    strcpy(record->diseaseID, diseaseID);

    record->patientAge = patientAge;

    record->country = malloc(sizeof(char*) * strlen(country) + 1);
    strcpy(record->country, country);

    record->entryDate = entryDate;

    record->exitDate = exitDate;
}
