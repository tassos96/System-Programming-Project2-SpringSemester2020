#define  _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <errno.h> 
#include <fcntl.h>  

#include "include/cmdArguments.h"
#include "include/namedPipes.h"
#include "include/parentOperations.h"
#include "include/workerOperations.h"
#include "include/fileOperations.h"


int main(int argc, char const *argv[])
{
    int numWorkers = 0; //Number of total workers
    int bufferSize = 0; //Size of buffer
    char* inputDirPath = NULL; //Path of input directory

    //Read all arguments from command line and initialize the variables
    readCommandLineArguments(&numWorkers, &bufferSize, &inputDirPath, argc, argv);
    //Check for error (If some of args have not initialized, return error)
    checkForArgsError(numWorkers, bufferSize, inputDirPath);

    //Print the args value
    printf("Num of workers: %d\n", numWorkers);
    printf("Buffer Size: %d\n", bufferSize);
    printf("Input Dir Path: %s\n", inputDirPath);


    pid_t pids[numWorkers]; //array of all pids
    pid_t parent = getpid(); //parent process id


    //Creation of all named pipes that the program needs
    createNamedPipes(numWorkers);
    sleep(1); //sleep for more realism

    //Create all workers user wanted
    for(int curWorker = 0; curWorker < numWorkers; curWorker++)
    {  
        if ((pids[curWorker] = fork()) < 0)
        {
            fprintf(stderr, "Fork error\n");
            free(inputDirPath);
            exit(-1);
        }
        else if( pids[curWorker] == 0)  //WORKERS
        {
            printf("Worker %d -->pid = %d and parentId: %d\n", curWorker, getpid(), getppid());
            sleep(1); //sleep for more realism
            
            int readFD = -1;    //Read file descriptor
            int writeFD = -1;   //Write file descriptor

            //Function for worker to open his pipes that already has been created
            if((workerOpenPipes(&readFD, &writeFD, curWorker)) == -1)
            {
                free(inputDirPath);
                exit(-1);
            }

            int input_dirLen = strlen(inputDirPath);
            //Function that do all the worker operations
            workerOperations(readFD, writeFD, bufferSize, input_dirLen);

            free(inputDirPath);
            //Wait for SIGKILL signal from parent to terminate
            while (1)
            {

            }
            //return 0; //Worker terminates here
        }
    }
    //if parent
	if(parent == getpid())
    {
        printf("Parent --> pid: %d \n", getpid());
        
        //Arrays to store parent fds
        int* parentWriteFDs = malloc(numWorkers * sizeof(int));
        int* parentReadFDs = malloc(numWorkers * sizeof(int));
        
        //Open all read and write pipes and store their fds into proper arrays
        if (parentOpenPipes(numWorkers, &parentWriteFDs, &parentReadFDs) == -1)
        {
            free(parentWriteFDs);
            free(parentReadFDs);
            free(inputDirPath);
            exit(-1);
        }

        //Get all directories into inputDirPath
        int totalDirectories = 0;
        char** directoriesNames = NULL;
        directoriesNames = getDirectories(inputDirPath, &totalDirectories);
        
        //Function that do all the father operations
        int input_dirLen = strlen(inputDirPath);
        parentOperations(numWorkers, parentWriteFDs, parentReadFDs, directoriesNames, totalDirectories, bufferSize, input_dirLen);

        //FREEEE SOME PARENT VARS
        free(parentWriteFDs);
        free(parentReadFDs);
        for(int i = 0; i < totalDirectories; i++)
        {
            free(directoriesNames[i]);
        }
        free(directoriesNames);
    }
    free(inputDirPath);
    
    sleep(1); //make sure that worker exit code and memory freeing have run first
    for(int i = 0; i < numWorkers; i++)
    {
        kill(pids[i], SIGKILL); //Send SIGKILL to all workers created
    }
    
    //Wait all workers to terminate
    for(int i = 0; i < numWorkers; i++)
    {
        int returnCode;    
        waitpid(pids[i], &returnCode, 0);

        if (returnCode == 9)  //Check worker status code
        {
            printf("Worker process terminated successfully\n");    
        }
    }
    //Parent terminates
    printf("Parent terminated successfully\n");
    return 0;
}