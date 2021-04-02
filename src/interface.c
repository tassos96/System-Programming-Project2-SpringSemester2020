#define  _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/select.h>
#include  <errno.h> 
#include  <fcntl.h>
#include <signal.h>


#include "include/interface.h"
#include "include/parentOperations.h"
#include "include/fileOperations.h"

#define AGE_RANGES 4

#define LIST_COUNTRIES 1
#define DISEASE_FREQUENCY 2
#define TOP_K 3
#define SEARCH_PATIENT 4
#define PATIENT_ADMISSIONS 5
#define PATIENT_DISCHARGES 6
#define EXIT 7

#define ERROR -1

//Flag for signal handling
volatile sig_atomic_t stop = 0;

//Function for signal handling
void catchSignals(int signo)
{
    stop = 1;
}

//Function to read commands from interface
int readCommandFromInterface(StatisticsList statsList, int* parentWriteFDs, int* parentReadFDs, int numWorkers, int bufferSize, char** countries, int totalCountries, sigset_t signal_set, int* total, int* success, int* fail)
{
    //SIGNALS
    static struct sigaction act;
    act.sa_handler = catchSignals;
    sigfillset(&(act.sa_mask));
    sigaction(SIGINT, &act, NULL);
    sigaction(SIGQUIT, &act, NULL);
    ////////////////////////////////

    char* inputLine = NULL;
    size_t inputLength = 0;
    size_t lineSize = -1;

    //We can unblock signals here
    sigprocmask(SIG_UNBLOCK, &signal_set, NULL);
    //If signal have arrived then write parent log file and proceed to termination
    if(stop == 1)
    {
        writeParentLogFile(countries, totalCountries, (*total), (*success), (*fail));
        return EXIT;
    }
    lineSize = getline(&inputLine, &inputLength, stdin);
    if(lineSize == -1 && stop == 1)
    {
        writeParentLogFile(countries, totalCountries, (*total), (*success), (*fail));
        free(inputLine);
        return EXIT;
    }
    //Block again signals here
    sigprocmask(SIG_BLOCK, &signal_set, NULL);
    if(lineSize == -1)
    {
        (*fail)++;
        printf("Error occured: Check again your command.\n");
        free(inputLine);
        return -1;
    }
    else
    {
        inputLine[strlen(inputLine)-1] = '\0';

        //Run commands here
        int commandCode = runCommands(&inputLine, statsList, parentWriteFDs, parentReadFDs, numWorkers, bufferSize, countries, totalCountries, success, total);
        (*total)++;
        free(inputLine);
        return commandCode;
    }
}

//Function for running commands
int runCommands(char** inputLine, StatisticsList statsList, int* parentWriteFDs, int* parentReadFDs, int numWorkers, int bufferSize, char** countries, int totalCountries, int* success, int* fail)
{
    int functionReturnCode = -1;
    char* command = NULL;
    char* copyOfCommand = NULL;
    char** arguments = NULL;
    int argumentsNum = 0;

    copyOfCommand = malloc(strlen(*inputLine) * sizeof(char) + 1);
    strcpy(copyOfCommand, *inputLine);
    
    if(strcmp(copyOfCommand,"") == 0)
    {
        (*fail)++;
        printf("Please type a command\n");
        free(copyOfCommand);
        return ERROR;
    }
    command = strtok(*inputLine, " ");

    argumentsNum = countCommandArguments(&command);

    arguments = getCommandArguments(&arguments, &copyOfCommand, argumentsNum);

    free(copyOfCommand);

    if(strcmp(arguments[0], "/listCountries") == 0)
    {
        functionReturnCode = runListCountries(arguments, argumentsNum, statsList, parentWriteFDs, parentReadFDs, numWorkers, bufferSize, fail);
        (*success)++;
        printf("\n");
    }
    else if(strcmp(arguments[0], "/diseaseFrequency") == 0)
    {
        functionReturnCode = runDiseaseFrequency(arguments, argumentsNum, statsList, fail);
        (*success)++;
        printf("\n");
    }
    else if(strcmp(arguments[0], "/topK-AgeRanges") == 0)
    {
        functionReturnCode = runTopKAgeRanges(arguments, argumentsNum, statsList, fail);
        (*success)++;
        printf("\n");
    }
    else if(strcmp(arguments[0], "/searchPatientRecord") == 0)
    {
        functionReturnCode = runSearchPatientRecord(arguments, argumentsNum, parentWriteFDs, parentReadFDs, numWorkers, bufferSize, fail);
        (*success)++;
        printf("\n");
    }
    else if(strcmp(arguments[0], "/numPatientAdmissions") == 0)
    {
        functionReturnCode = runPatientAdmissions(arguments, argumentsNum, statsList, countries, totalCountries, fail);
        (*success)++;
        printf("\n");
    }
    else if(strcmp(arguments[0], "/numPatientDischarges") == 0)
    {
        functionReturnCode = runPatientDischarges(arguments, argumentsNum, parentWriteFDs, parentReadFDs, numWorkers, bufferSize, countries, totalCountries, fail);
        (*success)++;
        printf("\n");
    }
    else if(strcmp(arguments[0], "/exit") == 0)
    {
        for(int i = 0; i < numWorkers; i++)
        {
            writeToWorker(parentWriteFDs[i], "/exit");
        }
        functionReturnCode = EXIT;
        (*success)++;
    }

    freeArgumentsStringArray(&arguments, argumentsNum);


    if(functionReturnCode == -1)
    {
        printWrongCommandError();
    }
    return functionReturnCode;
}

/**
 * Function to count command arguments 
 **/
int countCommandArguments(char** command)
{
    int argumentsCounter = 0;   //includes command name
    while( *command != NULL ) 
    {
        argumentsCounter++;
        *command = strtok(NULL, " ");
    }
    return argumentsCounter;
}

/**
 * Function to save command's arguments to a string array
 **/
char** getCommandArguments(char*** arguments, char** copyOfCommand, int argumentsNum) 
{
    char* command = NULL;

    arguments[0] = malloc(sizeof(char*) * (argumentsNum));
    int i = 0;
    command = strtok(*copyOfCommand, " ");
    while(command != NULL) 
    {
        arguments[0][i] = malloc(sizeof(char) * strlen(command) + 1);
        strcpy(arguments[0][i], command);
        command = strtok(NULL, " ");
        i++;
    }
    return arguments[0];
}

/**
 * Function to free our string array
 **/
void freeArgumentsStringArray(char*** arguments, int argumentsNum) 
{
    for(int i = 0; i < argumentsNum; i++)
    {
        free(arguments[0][i]);
    }
    free(arguments[0]);
}

//Function to print wrong command error
void printWrongCommandError()
{
    printf("Error occured: Something goes wrong with command.\n");
    printf("Please try again\n");
}

int runListCountries(char** arguments, int argumentsNum, StatisticsList statsList, int* parentWriteFDs, int* parentReadFDs, int numWorkers, int bufferSize, int* fail)
{
    if(argumentsNum == 1)
    {
        for(int i = 0; i < numWorkers; i++)
        {
            writeToWorker(parentWriteFDs[i], arguments[0]);
        }
        int responses = 0;
        while (1)
        {
            fd_set fds;
            int maximumFD = 0;
            FD_ZERO(&fds);

            for(int i = 0; i < numWorkers; i++)
            {
                FD_SET(parentReadFDs[i], &fds);
            }
            getMaximumFD(&maximumFD, parentWriteFDs, parentReadFDs, numWorkers);
            select(maximumFD + 1, &fds, NULL, NULL, NULL);

            //TEST READ MESSAGE (REPLY FROM ALL WORKERS)
            for(int i = 0; i < numWorkers; i++)
            {
                if (FD_ISSET(parentReadFDs[i], &fds))
                {
                    int msgTotalBytes = 0;
                    read(parentReadFDs[i], &msgTotalBytes, sizeof(int));

                    //char message[msgTotalBytes+1];
                    char* message = calloc(msgTotalBytes+1, sizeof(char*));
                    if(readFromWorker(parentReadFDs[i], msgTotalBytes, message, bufferSize) == 3)
                        responses++;

                    if(strcmp(message, "END") != 0)
                        printf("%s\n", message);
                    free(message);
                }
            }
            if(responses == numWorkers)
                break;
        }
        return LIST_COUNTRIES;
    }
    else
    {
        (*fail)++;
        return ERROR;
    } 
}

int runDiseaseFrequency(char** arguments, int argumentsNum, StatisticsList statsList, int* fail)
{
    char* virusName = NULL;
    char* date1_string = NULL;
    char* date2_string = NULL;
    char* country = NULL;

    if(argumentsNum == 4)   //diseaseFrequency virusName date1 date2
    {
        virusName = malloc(sizeof(char*) * strlen(arguments[1]) +1);
        strcpy(virusName, arguments[1]);

        date1_string = malloc(sizeof(char*) * strlen(arguments[2]) + 1);
        strcpy(date1_string, arguments[2]);

        date2_string = malloc(sizeof(char*) * strlen(arguments[3]) + 1);
        strcpy(date2_string, arguments[3]);

        DateStructure date1 = setDate(date1_string);
        if (date1->day == -1 || date1->month == -1 || date1->year == -1)
        {
            (*fail)++;
            destroyDate(date1);
            free(virusName);
            free(date1_string);
            free(date2_string);
            return ERROR;
        }

        DateStructure date2 = setDate(date2_string);
        if (date2->day == -1 || date2->month == -1 || date2->year == -1)
        {
            (*fail)++;
            destroyDate(date1);
            destroyDate(date2);
            free(virusName);
            free(date1_string);
            free(date2_string);
            return ERROR;
        }

        if(datesOrderValidation(date1, date2) == -1)
        {
            (*fail)++;
            destroyDate(date1);
            destroyDate(date2);
            free(virusName);
            free(date1_string);
            free(date2_string);
            return ERROR;
        }

        int totalCases = 0;
        countTotalCases(statsList, virusName, date1, date2, &totalCases);
        printf("%d\n", totalCases);
        
        destroyDate(date1);
        destroyDate(date2);
        free(virusName);
        free(date1_string);
        free(date2_string);

        return DISEASE_FREQUENCY;
    }
    else if(argumentsNum == 5) //diseaseFrequency virusName date1 date2 country
    {
        virusName = malloc(sizeof(char*) * strlen(arguments[1]) +1);
        strcpy(virusName, arguments[1]);

        date1_string = malloc(sizeof(char*) * strlen(arguments[2]) + 1);
        strcpy(date1_string, arguments[2]);

        date2_string = malloc(sizeof(char*) * strlen(arguments[3]) + 1);
        strcpy(date2_string, arguments[3]);

        country = malloc(sizeof(char*) * strlen(arguments[4]) + 1);
        strcpy(country, arguments[4]);

        DateStructure date1 = setDate(date1_string);
        if (date1->day == -1 || date1->month == -1 || date1->year == -1)
        {
            (*fail)++;
            destroyDate(date1);
            free(virusName);
            free(country);
            free(date1_string);
            free(date2_string);
            return ERROR;
        }

        DateStructure date2 = setDate(date2_string);
        if (date2->day == -1 || date2->month == -1 || date2->year == -1)
        {
            (*fail)++;
            destroyDate(date1);
            destroyDate(date2);
            free(virusName);
            free(country);
            free(date1_string);
            free(date2_string);
            return ERROR;
        }

        if(datesOrderValidation(date1, date2) == -1)
        {
            (*fail)++;
            destroyDate(date1);
            destroyDate(date2);
            free(virusName);
            free(country);
            free(date1_string);
            free(date2_string);
            return ERROR;
        }

        int totalCases = 0;
        countTotalCasesWithCountry(statsList, virusName, date1, date2, country, &totalCases);
        printf("%d\n", totalCases);

        destroyDate(date1);
        destroyDate(date2);
        free(virusName);
        free(date1_string);
        free(date2_string);
        free(country);
        return DISEASE_FREQUENCY;
    }
    else
    {
        (*fail)++;
        return ERROR;
    }
}

int runTopKAgeRanges(char** arguments, int argumentsNum, StatisticsList statsList, int* fail)
{
    if(argumentsNum == 6)
    {
        int k = 0;
        char* country = NULL;
        char* virusName = NULL;
        char* date1_string = NULL;
        char* date2_string = NULL;

        k = atoi(arguments[1]);

        country = malloc(sizeof(char*) * strlen(arguments[2]) + 1);
        strcpy(country, arguments[2]);

        virusName = malloc(sizeof(char*) * strlen(arguments[3]) + 1);
        strcpy(virusName, arguments[3]);

        date1_string = malloc(sizeof(char*) * strlen(arguments[4]) + 1);
        strcpy(date1_string, arguments[4]);

        date2_string = malloc(sizeof(char*) * strlen(arguments[5]) + 1);
        strcpy(date2_string, arguments[5]);

        DateStructure date1 = setDate(date1_string);
        if (date1->day == -1 || date1->month == -1 || date1->year == -1)
        {
            (*fail)++;
            destroyDate(date1);
            free(country);
            free(virusName);
            free(date1_string);
            free(date2_string);
            return ERROR;
        }

        DateStructure date2 = setDate(date2_string);
        if (date2->day == -1 || date2->month == -1 || date2->year == -1)
        {
            (*fail)++;
            destroyDate(date1);
            destroyDate(date2);
            free(country);
            free(virusName);
            free(date1_string);
            free(date2_string);
            return ERROR;
        }

        if(datesOrderValidation(date1, date2) == -1)
        {
            (*fail)++;
            destroyDate(date1);
            destroyDate(date2);
            free(country);
            free(virusName);
            free(date1_string);
            free(date2_string);
            return ERROR;
        }

        int totalCases = 0;
        int total_0_20 = 0;
        int total_21_40 = 0;
        int total_41_60 = 0;
        int total_61_max = 0;

        countTopKAgeRangesCases(statsList, virusName, date1, date2, country, &totalCases, &total_0_20, &total_21_40, &total_41_60, &total_61_max);

        int percetange1 = 0;
        int percetange2 = 0;
        int percetange3 = 0;
        int percetange4 = 0;
        if(totalCases != 0)
        {
            percetange1 = (float)total_0_20/totalCases * 100.0;
            percetange2 = (float)total_21_40/totalCases * 100.0;
            percetange3 = (float)total_41_60/totalCases * 100.0;
            percetange4 = (float)total_61_max/totalCases * 100.0;
        }

        //printf("%d %d %d %d %d\n", totalCases, total_0_20, total_21_40, total_41_60, total_61_max);

        Stats stats[AGE_RANGES];
        strcpy(stats[0].ageRange,"0-20");
        stats[0].percentage = percetange1;
        strcpy(stats[1].ageRange,"21-40");
        stats[1].percentage = percetange2;
        strcpy(stats[2].ageRange,"41-60");
        stats[2].percentage = percetange3;
        strcpy(stats[3].ageRange,"60+");
        stats[3].percentage = percetange4;

        sortPercentages(stats);
        
        if(k > AGE_RANGES)
            k = AGE_RANGES;

        for(int i = 0; i < k; i++)
        {
            printf("%s: %d%%\n", stats[i].ageRange, stats[i].percentage);
        }

        destroyDate(date1);
        destroyDate(date2);
        free(country);
        free(virusName);
        free(date1_string);
        free(date2_string);

        return TOP_K;    
    }
    else
    {
        (*fail)++;
        return ERROR;
    }
    
}

int runSearchPatientRecord(char** arguments, int argumentsNum, int* parentWriteFDs, int* parentReadFDs, int numWorkers, int bufferSize, int* fail)
{
    if(argumentsNum == 2)
    {
        char* recordID = NULL;
        recordID = malloc(sizeof(char*) * strlen(arguments[1])+1);
        strcpy(recordID, arguments[1]);

        char* msg = malloc(sizeof(char*) * ( strlen(arguments[0]) + strlen(recordID) ) + 1);
        sprintf(msg, "%s %s", arguments[0], recordID);

        for(int i = 0; i < numWorkers; i++)
        {
            writeToWorker(parentWriteFDs[i], msg);
        }
        int responses = 0;
        while (1)
        {
            fd_set fds;
            int maximumFD = 0;
            FD_ZERO(&fds);

            for(int i = 0; i < numWorkers; i++)
            {
                FD_SET(parentReadFDs[i], &fds);
            }
            getMaximumFD(&maximumFD, parentWriteFDs, parentReadFDs, numWorkers);
            select(maximumFD + 1, &fds, NULL, NULL, NULL);

            //TEST READ MESSAGE (REPLY FROM ALL WORKERS)
            for(int i = 0; i < numWorkers; i++)
            {
                if (FD_ISSET(parentReadFDs[i], &fds))
                {
                    int msgTotalBytes = 0;
                    read(parentReadFDs[i], &msgTotalBytes, sizeof(int));

                    char* message = calloc(msgTotalBytes+1, sizeof(char*));
                    if(readFromWorker(parentReadFDs[i], msgTotalBytes, message, bufferSize) == 3)
                        responses++;

                    if(strcmp(message, "END") != 0)
                        printf("%s\n", message);
                    free(message);
                }
            }
            if(responses == numWorkers)
                break;
        }

        free(recordID);
        free(msg);

        return SEARCH_PATIENT;
    }
    else
    {
        (*fail)++;
        return ERROR;
    }
}    

int runPatientAdmissions(char** arguments, int argumentsNum, StatisticsList statsList, char** countries, int totalCountries, int* fail)
{
    char* virusName = NULL;
    char* date1_string = NULL;
    char* date2_string = NULL;
    char* country = NULL;

    if (argumentsNum == 4) // ALL COUNTRIES
    {
        virusName = malloc(sizeof(char*) * strlen(arguments[1]) +1);
        strcpy(virusName, arguments[1]);

        date1_string = malloc(sizeof(char*) * strlen(arguments[2]) + 1);
        strcpy(date1_string, arguments[2]);

        date2_string = malloc(sizeof(char*) * strlen(arguments[3]) + 1);
        strcpy(date2_string, arguments[3]);

        DateStructure date1 = setDate(date1_string);
        if (date1->day == -1 || date1->month == -1 || date1->year == -1)
        {
            (*fail)++;
            destroyDate(date1);
            free(virusName);
            free(date1_string);
            free(date2_string);
            return ERROR;
        }

        DateStructure date2 = setDate(date2_string);
        if (date2->day == -1 || date2->month == -1 || date2->year == -1)
        {
            (*fail)++;
            destroyDate(date1);
            destroyDate(date2);
            free(virusName);
            free(date1_string);
            free(date2_string);
            return ERROR;
        }

        if(datesOrderValidation(date1, date2) == -1)
        {
            (*fail)++;
            destroyDate(date1);
            destroyDate(date2);
            free(virusName);
            free(date1_string);
            free(date2_string);
            return ERROR;
        }

        for(int i = 0; i < totalCountries; i++)
        {
            int totalCases = 0;
            countTotalCasesWithCountry(statsList, virusName, date1, date2, countries[i], &totalCases);
            printf("%s %d\n", countries[i], totalCases);
        }

        destroyDate(date1);
        destroyDate(date2);
        free(virusName);
        free(date1_string);
        free(date2_string);
        return PATIENT_ADMISSIONS;
    }
    else if(argumentsNum == 5)
    {
        virusName = malloc(sizeof(char*) * strlen(arguments[1]) +1);
        strcpy(virusName, arguments[1]);

        date1_string = malloc(sizeof(char*) * strlen(arguments[2]) + 1);
        strcpy(date1_string, arguments[2]);

        date2_string = malloc(sizeof(char*) * strlen(arguments[3]) + 1);
        strcpy(date2_string, arguments[3]);

        country = malloc(sizeof(char*) * strlen(arguments[4]) + 1);
        strcpy(country, arguments[4]);

        DateStructure date1 = setDate(date1_string);
        if (date1->day == -1 || date1->month == -1 || date1->year == -1)
        {
            (*fail)++;
            destroyDate(date1);
            free(virusName);
            free(country);
            free(date1_string);
            free(date2_string);
            return ERROR;
        }

        DateStructure date2 = setDate(date2_string);
        if (date2->day == -1 || date2->month == -1 || date2->year == -1)
        {
            (*fail)++;
            destroyDate(date1);
            destroyDate(date2);
            free(virusName);
            free(country);
            free(date1_string);
            free(date2_string);
            return ERROR;
        }

        if(datesOrderValidation(date1, date2) == -1)
        {
            (*fail)++;
            destroyDate(date1);
            destroyDate(date2);
            free(virusName);
            free(country);
            free(date1_string);
            free(date2_string);
            return ERROR;
        }

        int totalCases = 0;
        countTotalCasesWithCountry(statsList, virusName, date1, date2, country, &totalCases);
        printf("%s %d\n", country, totalCases);
        

        destroyDate(date1);
        destroyDate(date2);
        free(virusName);
        free(country);
        free(date1_string);
        free(date2_string);
        return PATIENT_ADMISSIONS;
    }
    else
    {
        (*fail)++;
        return ERROR;
    }
}

int runPatientDischarges(char** arguments, int argumentsNum, int* parentWriteFDs, int* parentReadFDs, int numWorkers, int bufferSize, char** countries, int totalCountries, int* fail)
{
    char* virusName = NULL;
    char* date1_string = NULL;
    char* date2_string = NULL;
    char* country = NULL;

    if(argumentsNum == 4)
    {
        virusName = malloc(sizeof(char*) * strlen(arguments[1]) +1);
        strcpy(virusName, arguments[1]);

        date1_string = malloc(sizeof(char*) * strlen(arguments[2]) + 1);
        strcpy(date1_string, arguments[2]);

        date2_string = malloc(sizeof(char*) * strlen(arguments[3]) + 1);
        strcpy(date2_string, arguments[3]);

        char* msg = malloc(sizeof(char*) * ( strlen(arguments[0]) + 1 + strlen(virusName) + 1 + strlen(date1_string) + 1 + strlen(date2_string) ) + 1);
        sprintf(msg, "%s %s %s %s", arguments[0], virusName, date1_string, date2_string);

        for(int i = 0; i < numWorkers; i++)
        {
            writeToWorker(parentWriteFDs[i], msg);
        }
        
        int responses = 0;
        while (1)
        {
            fd_set fds;
            int maximumFD = 0;
            FD_ZERO(&fds);

            for(int i = 0; i < numWorkers; i++)
            {
                FD_SET(parentReadFDs[i], &fds);
            }
            getMaximumFD(&maximumFD, parentWriteFDs, parentReadFDs, numWorkers);
            select(maximumFD + 1, &fds, NULL, NULL, NULL);

            //TEST READ MESSAGE (REPLY FROM ALL WORKERS)
            for(int i = 0; i < numWorkers; i++)
            {
                if (FD_ISSET(parentReadFDs[i], &fds))
                {
                    int msgTotalBytes = 0;
                    read(parentReadFDs[i], &msgTotalBytes, sizeof(int));

                    char* message = calloc(msgTotalBytes+1, sizeof(char*));
                    if(readFromWorker(parentReadFDs[i], msgTotalBytes, message, bufferSize) == 3)
                        responses++;

                    if(strcmp(message, "END") != 0)
                        printf("%s\n", message);
                    free(message);
                }
            }
            if(responses == numWorkers)
                break;
        }

        free(virusName);
        free(date1_string);
        free(date2_string);
        free(msg);
        return PATIENT_DISCHARGES;
    }
    else if(argumentsNum == 5)
    {
        virusName = malloc(sizeof(char*) * strlen(arguments[1]) +1);
        strcpy(virusName, arguments[1]);

        date1_string = malloc(sizeof(char*) * strlen(arguments[2]) + 1);
        strcpy(date1_string, arguments[2]);

        date2_string = malloc(sizeof(char*) * strlen(arguments[3]) + 1);
        strcpy(date2_string, arguments[3]);

        country = malloc(sizeof(char*) * strlen(arguments[4]) + 1);
        strcpy(country, arguments[4]);

        char* msg = malloc(sizeof(char*) * ( strlen(arguments[0]) + 1 + strlen(virusName) + 1 + strlen(date1_string) + 1 + strlen(date2_string) + 1 + strlen(country)) + 1);
        sprintf(msg, "%s %s %s %s %s", arguments[0], virusName, date1_string, date2_string, country);

        for(int i = 0; i < numWorkers; i++)
        {
            writeToWorker(parentWriteFDs[i], msg);
        }
        
        int responses = 0;
        while (1)
        {
            fd_set fds;
            int maximumFD = 0;
            FD_ZERO(&fds);

            for(int i = 0; i < numWorkers; i++)
            {
                FD_SET(parentReadFDs[i], &fds);
            }
            getMaximumFD(&maximumFD, parentWriteFDs, parentReadFDs, numWorkers);
            select(maximumFD + 1, &fds, NULL, NULL, NULL);

            //TEST READ MESSAGE (REPLY FROM ALL WORKERS)
            for(int i = 0; i < numWorkers; i++)
            {
                if (FD_ISSET(parentReadFDs[i], &fds))
                {
                    int msgTotalBytes = 0;
                    read(parentReadFDs[i], &msgTotalBytes, sizeof(int));

                    char* message = calloc(msgTotalBytes+1, sizeof(char*));
                    if(readFromWorker(parentReadFDs[i], msgTotalBytes, message, bufferSize) == 3)
                        responses++;

                    if(strcmp(message, "END") != 0)
                        printf("%s\n", message);
                    free(message);
                }
            }
            if(responses == numWorkers)
                break;
        }

        free(virusName);
        free(country);
        free(date1_string);
        free(date2_string);
        free(msg);
        return PATIENT_DISCHARGES;
    }
    else
    {
        (*fail)++;
        return ERROR;
    }
}

//Function to sort ASC order stats percentages
void sortPercentages(Stats stats[AGE_RANGES])
{
    int i, j;
    Stats temp;
    
    for (i = 0; i < AGE_RANGES - 1; i++)
    {
        for (j = 0; j < (AGE_RANGES - 1-i); j++)
        {
            if (stats[j].percentage < stats[j + 1].percentage)
            {
                temp = stats[j];
                stats[j] = stats[j + 1];
                stats[j + 1] = temp;
            } 
        }
    }
}

//Function for parent's log file creation
void writeParentLogFile(char** countries, int totalCountries, int total, int success, int fail)
{
    pid_t parentID = getpid();
    int idDigits = howManyDigits(parentID);

    char* logFileName = malloc(sizeof(char*) * (strlen("./log_files/log_file.") + idDigits) + 1);
    sprintf(logFileName, "./log_files/log_file.%d", parentID);

    char* msgToLog = NULL;
    int totalBytes = 0;
    for(int i = 0; i < totalCountries; i++)
    {
        totalBytes += strlen(countries[i]) + strlen("\n");
    }
    int totalDigits = howManyDigits(total);
    int successDigits = howManyDigits(success);
    int failDigits = howManyDigits(fail);

    totalBytes += strlen("TOTAL") + totalDigits + strlen("\n");
    totalBytes += strlen("SUCCESS") + successDigits + strlen("\n");
    totalBytes += strlen("FAIL") + failDigits + strlen("\n");
    
    char* totalMsg = malloc(sizeof(char*) * (strlen("TOTAL ") + totalDigits + strlen("\n")) + 1);
    sprintf(totalMsg, "TOTAL %d\n", total);

    char* successMsg = malloc(sizeof(char*) * (strlen("SUCCESS ") + successDigits + strlen("\n")) + 1);
    sprintf(successMsg, "SUCCESS %d\n", success);

    char* failMsg = malloc(sizeof(char*) * (strlen("FAIL ") + failDigits + strlen("\n")) + 1);
    sprintf(failMsg, "FAIL %d\n", fail);

    //msgToLog = calloc(sizeof(char*) * totalBytes + 1);
    msgToLog = calloc(totalBytes + 1, sizeof(char*));
    for(int i = 0; i < totalCountries; i++)
    {
        strcat(msgToLog, countries[i]);
        strcat(msgToLog, "\n");
    }
    strcat(msgToLog, totalMsg);
    strcat(msgToLog, successMsg);
    strcat(msgToLog, failMsg);

    FILE* file;
    file = fopen(logFileName, "w");
    if(file == NULL)
    {
        perror("Parent LogFile");
        return;
    }

    fputs(msgToLog, file);

    free(logFileName);
    free(msgToLog);
    free(totalMsg);
    free(successMsg);
    free(failMsg);
    fclose(file);
    return;
}