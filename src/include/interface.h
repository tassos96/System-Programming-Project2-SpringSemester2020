#ifndef INTERFACE_H
#define INTERFACE_H

#include "statsList.h"

//Struct for topK command
typedef struct Stats
{
    char ageRange[10];
    int percentage;
}Stats;
///////////

//Function to read commands from interface
int readCommandFromInterface(StatisticsList statsList, int* parentWriteFDs, int* parentReadFDs, int numWorkers, int bufferSize, char** countries, int totalCountries, sigset_t signal_set, int* total, int* success, int* fail);

//Function for running commands
int runCommands(char** inputLine, StatisticsList statsList, int* parentWriteFDs, int* parentReadFDs, int numWorkers, int bufferSize, char** countries, int totalCountries, int* success, int* fail);

//Function to count command arguments 
int countCommandArguments(char** command);

//Function to save command's arguments to a string array
char** getCommandArguments(char*** arguments, char** copyOfCommand, int argumentsNum);

//Function to free our string array
void freeArgumentsStringArray(char*** arguments, int argumentsNum);

//Function to print wrong command error
void printWrongCommandError();

//----------- COMMANDS IMPLEMENTATION ---------------//

int runListCountries(char** arguments, int argumentsNum, StatisticsList statsList, int* parentWriteFDs, int* parentReadFDs, int numWorkers, int bufferSize, int* fail);
int runDiseaseFrequency(char** arguments, int argumentsNum, StatisticsList statsList, int* fail);
int runTopKAgeRanges(char** arguments, int argumentsNum, StatisticsList statsList, int* fail);
int runSearchPatientRecord(char** arguments, int argumentsNum, int* parentWriteFDs, int* parentReadFDs, int numWorkers, int bufferSize, int* fail);
int runPatientAdmissions(char** arguments, int argumentsNum, StatisticsList statsList, char** countries, int totalCountries, int* fail);
int runPatientDischarges(char** arguments, int argumentsNum, int* parentWriteFDs, int* parentReadFDs, int numWorkers, int bufferSize, char** countries, int totalCountries, int* fail);

//---------------------------------------------------//

//Function to sort ASC order stats percentages
void sortPercentages(Stats stats[4]);

//Function for parent's log file creation
void writeParentLogFile(char** countries, int totalCountries, int total, int success, int fail);



#endif