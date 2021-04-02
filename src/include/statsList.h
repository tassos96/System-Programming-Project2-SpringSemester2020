#ifndef STATS_LIST_H
#define STATS_LIST_H

#include "stat.h"
#include "myDateStructure.h"

typedef struct StatsList
{
    StatNodePointer headNode;
    StatNodePointer tailNode;

}StatsList;

typedef StatsList* StatisticsList;

//Function to initialize stats list
StatNodePointer initializeStatsList();

//Function to add a node to stats list
void addStatToList(StatisticsList statsList, char* country, DateStructure date, char* disease, int stat1, int stat2, int stat3, int stat4);

//Function to print stats list
void printStatsList(StatisticsList statsList);

//Function to free memory allocated for stats list
void freeStatsList(StatisticsList statsList);

//Function to count total cases from stats list for user commands
void countTotalCases(StatisticsList statsList, char* virusName, DateStructure date1, DateStructure date2, int* totalCases);

//Function to count total cases from stats list for user commands with additional country filter
void countTotalCasesWithCountry(StatisticsList statsList, char* virusName, DateStructure date1, DateStructure date2, char* country, int* totalCases);

//Function to count topK cases
void countTopKAgeRangesCases(StatisticsList statsList, char* virusName, DateStructure date1, DateStructure date2, char* country, int* totalCases, int* total_0_20, int* total_21_40, int* total_41_60, int* total_61_max);

#endif