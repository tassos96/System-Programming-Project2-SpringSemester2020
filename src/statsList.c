#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "include/statsList.h"
#include "include/stat.h"

//Function to initialize stats list
StatNodePointer initializeStatsList()
{
    StatNodePointer head;
    head = malloc(sizeof(Stat));

    head->country = NULL;
    head->date = NULL;
    head->disease = NULL;
    head->stat1 = -1;
    head->stat2 = -1;
    head->stat3 = -1;
    head->stat4 = -1;

    head->next = NULL;

    return head;
}

//Function to add a node to stats list
void addStatToList(StatisticsList statsList, char* country, DateStructure date, char* disease, int stat1, int stat2, int stat3, int stat4)
{
    StatNodePointer currentNode = statsList->tailNode;

    if(currentNode->country == NULL)
    {
        initializeStatNode(currentNode, country, date, disease, stat1, stat2, stat3, stat4);
        currentNode->next = NULL;
        return;
    }

    StatNodePointer newStatNode = NULL;

    //newStatNode = malloc(sizeof(Stat));
    newStatNode = calloc(1, sizeof(Stat));
    initializeStatNode(newStatNode, country, date, disease, stat1, stat2, stat3, stat4);
    currentNode->next = newStatNode;
    statsList->tailNode = newStatNode;

    return;
}

//Function to print stats list
void printStatsList(StatisticsList statsList)
{
    StatNodePointer current = statsList->headNode;
    while(current != NULL)
    {
        printf("%s %d-%d-%d %s %d %d %d %d\n", current->country, current->date->day, current->date->month, current->date->year, current->disease, current->stat1, current->stat2, current->stat3, current->stat4);
        current = current->next;
    }
}

//Function to free memory allocated for stats list
void freeStatsList(StatisticsList statsList)
{
    StatNodePointer current = statsList->headNode;
    StatNodePointer temporary_next;

    while(current != NULL)
    {
        free(current->country);
        destroyDate(current->date);
        free(current->disease);
        
        temporary_next = current;
        current = current->next;
        free(temporary_next);
    }
}

//Function to count total cases from stats list for user commands
void countTotalCases(StatisticsList statsList, char* virusName, DateStructure date1, DateStructure date2, int* totalCases)
{
    StatNodePointer current = statsList->headNode;
    while(current != NULL)
    {
        if( strcmp(current->disease, virusName) == 0 )
        {
            if(compareDateKey(current->date, date1) == 1 || compareDateKey(current->date, date1) == 0)
            {
                if(compareDateKey(current->date, date2) == -1 || compareDateKey(current->date, date2) == 0)
                {
                    (*totalCases) += current->stat1 + current->stat2 + current->stat3 + current->stat4;
                }
            }
        }
        current = current->next;
    }
}

//Function to count total cases from stats list for user commands with additional country filter
void countTotalCasesWithCountry(StatisticsList statsList, char* virusName, DateStructure date1, DateStructure date2, char* country, int* totalCases)
{
    StatNodePointer current = statsList->headNode;
    while(current != NULL)
    {
        if( (strcmp(current->disease, virusName) == 0) && (strcmp(current->country, country) == 0) )
        {
            if(compareDateKey(current->date, date1) == 1 || compareDateKey(current->date, date1) == 0)
            {
                if(compareDateKey(current->date, date2) == -1 || compareDateKey(current->date, date2) == 0)
                {
                    (*totalCases) += current->stat1 + current->stat2 + current->stat3 + current->stat4;
                }
            }
        }
        current = current->next;
    }
}

//Function to count topK cases
void countTopKAgeRangesCases(StatisticsList statsList, char* virusName, DateStructure date1, DateStructure date2, char* country, int* totalCases, int* total_0_20, int* total_21_40, int* total_41_60, int* total_61_max)
{
    StatNodePointer current = statsList->headNode;
    while(current != NULL)
    {
        if( (strcmp(current->disease, virusName) == 0) && (strcmp(current->country, country) == 0) )
        {
            if(compareDateKey(current->date, date1) == 1 || compareDateKey(current->date, date1) == 0)
            {
                if(compareDateKey(current->date, date2) == -1 || compareDateKey(current->date, date2) == 0)
                {
                    (*totalCases) += current->stat1 + current->stat2 + current->stat3 + current->stat4;
                    (*total_0_20) += current->stat1;
                    (*total_21_40) += current->stat2;
                    (*total_41_60) += current->stat3;
                    (*total_61_max) += current->stat4;
                }
            }
        }
        current = current->next;
    }
}
