/**
 * This file contains Date Structure methods.
**/ 
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

#include "include/myDateStructure.h"

//Function to create a date struct
DateStructure setDate(char* dateString)
{   
    DateStructure datePointer = malloc(sizeof(Date));
    
    if(strcmp(dateString, "-") == 0)    //TODO: MIGHT NEED CHANGE TO --
    {
        datePointer->day = 0;
        datePointer->month = 0;
        datePointer ->year = 0;
        return datePointer;
    }
    char * copyOfString = malloc(sizeof(char*) * strlen(dateString) + 1);
    char* endptr;
    strcpy(copyOfString, dateString);

    int day;
    int month;
    int year;
    
    //printf("DATE STRUCT COPY OF STRING: %s\n", copyOfString);

    char* date = strtok(copyOfString, "-");
    day = strtol(date, &endptr, 10);
    if (*endptr) // If the result is 0, test for an error 
    {
        printf("\nConversion error occurred: Wrong day insertion !\n");
        day = -1;
        
    }
    if(dayValidation(day) == -1)    //Check if day range is between 1-31
    {
        printf("\nConversion error occured: Wrong day range (1-31) !\n");
        day = -1;
    }
    datePointer->day = day;

    date = strtok(NULL, "-");
    month = strtol(date, &endptr, 10);
    if (*endptr) // If the result is 0, test for an error 
    {
        printf("\nConversion error occurred: Wrong month insertion !\n");
        month = -1;
    }
    if(monthValidation(month) == -1)    //Check if month range is between 1-12
    {
        printf("\nConversion error occured: Wrong month range (1-12) !\n");
        month = -1;
    }
    datePointer->month = month;

    date = strtok(NULL, "-");
    year = strtol(date, &endptr, 10);
    if (*endptr) // If the result is 0, test for an error 
    {
        printf("\nConversion error occurred: Wrong year insertion !\n");
        year = -1;
    }
    if(yearValidation(year) == -1)  //Check if year range is between 1000-9999
    {
        printf("\nConversion error occured: Wrong year range (1000 - 9999)!\n");
        year = -1;
    }
    datePointer->year = year;

    free(copyOfString);

    return datePointer;
}

//Function to free memory allocated from a date structure
void destroyDate(DateStructure date)
{
    free(date);
}

//Function to check day's range(1-31)
int dayValidation(int day)
{
    if (day >= 1 && day <= 31)
    {
        return 1;
    }
    else
    {
        return -1;
    }
}

//Function to check month's range(1-12)
int monthValidation(int month)
{
    if (month >= 1 && month <= 12)
    {
        return 1;
    }
    else
    {
        return -1;
    }
}

//Function to check year's range(1000-9999)
int yearValidation(int year)
{
    if(year >= 1000 && year <= 9999)
    {
        return 1;
    }
    else
    {
        return -1;
    }
}

//Function to check if entry and exit dates is in correct order
int datesOrderValidation (DateStructure entryDate, DateStructure exitDate)
{
    if (entryDate->year > exitDate->year)
    {
        printf("Error Occured: Entry Date is bigger than Exit Date !\n");
        return -1;
    }
    else if (entryDate->year == exitDate->year)
    {
        if (entryDate->month > exitDate->month)
        {
            printf("Error Occured: Entry Date is bigger than Exit Date !\n");
            return -1;
        }
        else if (entryDate->month == exitDate->month)
        {
            if (entryDate->day > exitDate->day)
            {
                printf("Error Occured: Entry Date is bigger than Exit Date !\n");
                return -1;
            }
            else
            {
                return 1;
            }
        }
        else
        {
            return 1;
        }
    }
    else
    {
        return 1;
    }
}

//Function to calculate which date is bigger
int compareDateKey(DateStructure date1, DateStructure date2)
{
    if (date1->year > date2->year) // date1 > date2
    {
        return 1;
    }
    else if (date1->year < date2->year) // date1 < date2
    {
        return -1;
    }
    else //if (date1->year == date2->year)
    {
        if (date1->month > date2->month) // date1 > date2
        {
            return 1;
        }
        else if(date1->month < date2->month) // date1 < date2
        {
            return -1;
        }
        else //if(date1->month == date2->month)
        {
            if (date1->day > date2->day)    //date1 > date2
            {
                return 1;
            }
            else if(date1->day < date2->day) // date1 < date2
            {
                return -1;
            }
            else //if(date1->day == date2->day) //date1 = date2
            {
                return 0;
            }
        }
    }
}