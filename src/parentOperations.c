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
#include <dirent.h>

#include "include/namedPipes.h"
#include "include/parentOperations.h"
#include "include/statsList.h"
#include "include/interface.h"


#define EXIT 7


//==============================================    PARENT    ==============================================//

//Function for parent to open his pipes that already has been created
int parentOpenPipes(int numWorkers, int** parentWriteFDs, int** parentReadFDs)
{
    //For every worker
    for(int i = 0; i < numWorkers; i++)
    {
        char* parentToWorker = NULL;    
        char* workerToParent = NULL;    
        //Get named pipes name to open them to workers and parent processes
        getNamedPipesName(i, &parentToWorker, &workerToParent);

        //Open fifo that parent writes to worker
        if( (parentWriteFDs[0][i] = open(parentToWorker, O_WRONLY )) < 0)
        {
            perror("Named pipe open error");
            return -1;
        }
        fcntl(parentWriteFDs[0][i], F_SETFL, O_NONBLOCK); //Make it non-block
        
        //Open fifo that parent reads from worker
        if( (parentReadFDs[0][i] = open(workerToParent, O_RDWR )) < 0)
        {
            perror("Named pipe open error");
            return -1;
        }
        free(parentToWorker);
        free(workerToParent);
    }
    
    return 1;
}

//Function for all the parent operations
int parentOperations(int numWorkers, int* parentWriteFDs, int* parentReadFDs, char** directoriesNames, int totalDirectories, int bufferSize, int input_dirLen)
{   
    
    //SIGNALS
    sigset_t signal_set;
    sigemptyset(&signal_set);

    sigaddset(&signal_set, SIGINT);
    sigaddset(&signal_set, SIGQUIT);

    //Block signals of signal set above
    sigprocmask(SIG_BLOCK, &signal_set, NULL);

    ///////////////////
    //SEND PATHS TO WORKERS WITH ROUND ROBIN FORMAT
    //also store to string array countries all the countries from input_dir
    char ** countries = malloc(totalDirectories * sizeof(*countries));
    int totalCountries = totalDirectories;
    int offset = input_dirLen+1;

    int curWorker = 0;
    for(int curDir = 0; curDir < totalDirectories; curDir++)
    {
        int bytesToCopy = strlen(directoriesNames[curDir]) - input_dirLen;
        countries[curDir] = malloc(sizeof(char) * (bytesToCopy + 1));
        strncpy(countries[curDir], directoriesNames[curDir]+offset, bytesToCopy);
        countries[curDir][bytesToCopy] = '\0';

        if(curWorker == numWorkers)
        {
            curWorker = 0;
        }
        writeToWorker(parentWriteFDs[curWorker], directoriesNames[curDir]);
        curWorker++;
        //sleep(1);
    }
    ///////////////////
    //Print all countries
    // for(int curDir = 0; curDir < totalDirectories; curDir++)
    // {
    //     printf("C: %s\n", countries[curDir]);
    // }

    for(int i = 0; i < numWorkers; i++)
    {
        writeToWorker(parentWriteFDs[i], "PATHS_END");
    }

    int paths_end_responses = 0; //break flag
    while (1)
    {
        //VARS FOR SELECT()
        fd_set fds;
        int maximumFD = 0;
        FD_ZERO(&fds);

        for(int i = 0; i < numWorkers; i++)
        {
            FD_SET(parentReadFDs[i], &fds);
        }
        getMaximumFD(&maximumFD, parentWriteFDs, parentReadFDs, numWorkers);
        select(maximumFD + 1, &fds, NULL, NULL, NULL);

        //READ MESSAGE (REPLY FROM ALL WORKERS THAT THEY RECEIVED ALL PATHS)
        for(int i = 0; i < numWorkers; i++)
        {
            if (FD_ISSET(parentReadFDs[i], &fds))
            {
                //printf("Read from channel %d\n", i);
                //Read how many bytes worker will send
                int msgTotalBytes=0;
                read(parentReadFDs[i], &msgTotalBytes, sizeof(int));
                //printf("Bytes to read from worker: %d\n", msgTotalBytes);

                //Read message from worker and store it
                //char message[msgTotalBytes+1];
                char* message = calloc(msgTotalBytes+1, sizeof(char*));
                if(readFromWorker(parentReadFDs[i], msgTotalBytes, message, bufferSize) == 2)
                    paths_end_responses++;
                free(message);
            }
        }
        if(paths_end_responses == numWorkers)
            break;
    }
    
    //here we have to wait for all the statitics
    int responses = 0;
    //Initialize list to store the statistics taken from workers
    StatisticsList statsList = calloc(1, sizeof(StatsList));
    statsList->headNode = initializeStatsList();
    statsList->tailNode = statsList->headNode;

    while (1)
    {
        //TEST WRITE MESSAGE
        //VARS FOR SELECT()
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
                //Read how many bytes worker will send
                int msgTotalBytes = 0;
                read(parentReadFDs[i], &msgTotalBytes, sizeof(int));

                //Read message from worker and store it
                char* message = calloc(msgTotalBytes+1, sizeof(char*));
                if(readFromWorker(parentReadFDs[i], msgTotalBytes, message, bufferSize) == 3)
                    responses++;
                //printf("Parent: %s\n", message);
                
                //Here take tokens
                if(strcmp(message, "END") != 0)
                    storeStats(statsList, message); //Store stats to list
                free(message);
            }
        }
        if(responses == numWorkers)
            break;
    }

    //#####################################
    //READ COMMANDS FROM USER
    int total = 0;
    int success = 0;
    int fail = 0;
    printf("Ready for queries\n\n");
    while (1)
    {   
        sigprocmask(SIG_UNBLOCK, &signal_set, NULL);
        int commandCode = readCommandFromInterface(statsList, parentWriteFDs, parentReadFDs, numWorkers, bufferSize, countries, totalCountries, signal_set, &total, &success, &fail);
        if(commandCode == -1)
        {
            fail++;
        }
        if(commandCode == EXIT )
        {
            break;
        }
    }
    
    //Print logfile info counters
    //printf("Total %d\nSuccess %d\nFail %d\n", total, success, fail);
    
    //Freeee some memory
    for(int i = 0; i < totalDirectories; i++)
    {
        free(countries[i]);
    }
    free(countries);
    removeNamedPipes(numWorkers);
    freeStatsList(statsList);
    free(statsList);
    return 1;
}

//Function for writting message to worker
int writeToWorker(int writeFd, char* msgToSend)
{
    int nwrite;
    size_t bytesToSend = strlen(msgToSend)+1;
    write(writeFd, &bytesToSend, sizeof(int));
    
    if((nwrite = write(writeFd, msgToSend, strlen(msgToSend)+1)) == -1)
    {
        perror("Error in writing");
        return -1;
    }

    return 1;
}

//Function for message reading from worker
int readFromWorker(int readFd, int msgTotalBytes, char* message, int bufferSize)
{
    
    int nread;
    int totalReaden = 0;
    char buf[bufferSize];

    for(;;)
    {   
        //To not exceed msg if there is and other messages into the pipe
        //Read only whats left
        if((totalReaden+bufferSize) > msgTotalBytes)
        {
            int toRead = msgTotalBytes - totalReaden;
            nread = read(readFd, buf, toRead);  
        }
        else
        {
            nread = read(readFd, buf, bufferSize);
        }

        if(nread > 0)
        {
            for(int i = 0; i < nread; i++)
            {
                (message)[totalReaden+i] = buf[i];
            }
            totalReaden += nread;

            if (totalReaden == msgTotalBytes )
            {
                break;
            }
        }
    }
    (message)[msgTotalBytes] = '\0';
    if(strcmp((message), "FATHER ALL PATHS RECEIVED") == 0)
    {
        return 2;
    }
    if(strcmp((message), "END") == 0)
        return 3;
    return 1;
}

//Function to get maximum fd for select
void getMaximumFD(int* maximumFD, int* parentWriteFDs, int* parentReadFDs, int numWorkers)
{
    for(int i = 0; i < numWorkers; i++)
    {
        if(parentWriteFDs[i] > *maximumFD)
        {
            *maximumFD = parentWriteFDs[i];
        }
    }
    for(int i = 0; i < numWorkers; i++)
    {
        if(parentReadFDs[i] > *maximumFD)
        {
            *maximumFD = parentReadFDs[i];
        }
    }
}

//Function that take tokens of stats message and store them into stats list
void storeStats(StatisticsList statsList, char* message)
{   
    //First take country and date tokens
    char* copyOfMessage = malloc(sizeof(char) * strlen(message)+1);
    strcpy(copyOfMessage, message);

    char* token = NULL;
    char* country = NULL;
    char* date = NULL;
    char** copyOfDiseaseStats = NULL;
    int diseasesStatsCounter = 0;

    token = strtok(copyOfMessage, "|");
    country = malloc(sizeof(char)*strlen(token)+1);
    strcpy(country, token);

    token = strtok(NULL, "|");
    date = malloc(sizeof(char) * strlen(token) + 1);
    strcpy(date, token);
    ////////////////////////////////////////////////////////////
    //Must save into string array all other tokens
    token = strtok(NULL, "|");
    while( token != NULL ) 
    {
        diseasesStatsCounter++;
        copyOfDiseaseStats = realloc(copyOfDiseaseStats, diseasesStatsCounter * sizeof(*copyOfDiseaseStats));
        copyOfDiseaseStats[diseasesStatsCounter - 1] = malloc(sizeof(char)*(strlen(token)+1) );
        strcpy(copyOfDiseaseStats[diseasesStatsCounter-1], token);
        //printf( " %s\n", token );
        token = strtok(NULL, "|");
    }

    //Store tokens to list
    tokStats(statsList, country, date, copyOfDiseaseStats, diseasesStatsCounter);

    free(copyOfMessage);
    free(country);
    free(date);
    for(int i = 0; i < diseasesStatsCounter; i++)
    {
        free(copyOfDiseaseStats[i]);
    }
    free(copyOfDiseaseStats);
}

//Take tokens and store to list
void tokStats(StatisticsList statsList, char* country, char* dateString, char** copyOfDiseaseStats, int diseasesStatsCounter)
{
    char* stat = NULL;
    char* disease = NULL;
    int stat1 = -1;
    int stat2 = -1;
    int stat3 = -1;
    int stat4 = -1;
    
    for(int i = 0; i < diseasesStatsCounter; i++)
    {
        stat = strtok(copyOfDiseaseStats[i], ".");
        disease = malloc(sizeof(char) * strlen(stat) + 1);
        strcpy(disease, stat);

        stat = strtok(NULL, ".");
        stat1 = atoi(stat);

        stat = strtok(NULL, ".");
        stat2 = atoi(stat);

        stat = strtok(NULL, ".");
        stat3 = atoi(stat);

        stat = strtok(NULL, ".");
        stat4 = atoi(stat);

        //printf("%s\n %d\n %d\n %d\n %d\n", disease, stat1, stat2, stat3, stat4);
        DateStructure date = setDate(dateString);
        addStatToList(statsList, country, date, disease, stat1, stat2, stat3, stat4);
        free(disease);
    }
}