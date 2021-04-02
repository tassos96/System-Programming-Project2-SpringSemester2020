#ifndef MY_DATE_STRUCTURE_H
#define MY_DATE_STRUCTURE_H

typedef struct MyDateStructure
{
    int day;
    int month;
    int year;
} Date;

typedef Date* DateStructure;
//Function to set date, returns pointer
DateStructure setDate(char* dateString);
//Function to free date structure memory allocation
void destroyDate(DateStructure date);
//Function to validate if day value in correct range    
int dayValidation(int day);
//Function to validate if month value in correct range             
int monthValidation(int month);         
//Function to validate if year value in correct range
int yearValidation(int year);           
//Function to validate if entryDate < exitDate
int datesOrderValidation (DateStructure entryDate, DateStructure exitDate);
//if date1 > date2 returns 1, if date1 == date2 return 0, if date1 < date2 return -1    
int compareDateKey(DateStructure date1, DateStructure date2); 

#endif