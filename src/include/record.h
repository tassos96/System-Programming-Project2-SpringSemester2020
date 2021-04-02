#ifndef RECORD_H
#define RECORD_H

#include "myDateStructure.h"


typedef struct Record
{
    //Record Info
    char* recordID;
    char* status; //ENTRY OR EXIT
    char* patientFirstName;
    char* patientLastName;
    char* diseaseID;
    int patientAge;
    char* country;  //To get from dir name
    DateStructure entryDate; //To get from file name
    DateStructure exitDate; //To get from file name

    //List's Next Record
    struct Record* next;

}Record;

typedef Record* RecordListNode;

void initializeRecord(RecordListNode record, char* recordID, char* status, char* patientFirstName, char* patientLastName, char* diseaseID, int patientAge, char* country, DateStructure entryDate, DateStructure exitDate);

#endif