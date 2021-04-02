#ifndef RECORDS_LIST_H
#define RECORDS_LIST_H

#include "record.h"
#include "myDateStructure.h"

typedef struct List
{
    RecordListNode headNode;
    RecordListNode tailNode;

}List;

typedef List* RecordsList;

//Function to initialize records list
RecordListNode initializeRecordsList();
//Function to insert a record into list
void addRecordToList(RecordsList recordsList, char* recordID, char* status, char* patientFirstName, char* patientLastName, char* diseaseID, int patientAge, char* country, DateStructure entryDate, DateStructure exitDate);
//Function to check id duplication error
RecordListNode checkId(RecordsList recordsList, char* recordID);
//Function to print records list
void printRecordsListIds(RecordsList recordsList);
//Function to free records list memory allocated
void freeRecordsList(RecordsList recordsList);

void countDischarges(RecordsList recordsList, char* virusName, DateStructure date1, DateStructure date2, char* country, int* totalDischarges);


#endif