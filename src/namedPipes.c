/**
 * This file contains methods for named pipes creation and deletion.
**/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

#include "include/namedPipes.h"


#define PERMISIONS 0666 //Define read and write permissions for pipes creation

//Function to create all named pipes that program needs
void createNamedPipes(int numWorkers)
{
    //For all the workers we have to create
    for(int curWorker = 0; curWorker < numWorkers; curWorker++)
    {
        //named pipes names
        char* namedPipeParentToWorker = NULL;
        char* namedPipeWorkerToParent = NULL;
        
        //create pipes name
        getNamedPipesName(curWorker, &namedPipeParentToWorker, &namedPipeWorkerToParent);

        //Make the named pipes
        if ((mkfifo(namedPipeParentToWorker, PERMISIONS) < 0) && (errno != EEXIST))
        {
            perror("Error with named pipe creation");
        }
        if((mkfifo(namedPipeWorkerToParent, PERMISIONS) < 0) && (errno != EEXIST))
        {
            unlink(namedPipeParentToWorker);
            perror("Error with named pipe creation");
        }

        //free alocated memory
        free(namedPipeParentToWorker);
        free(namedPipeWorkerToParent);
    }
}

//Function that creates named pipes name
void getNamedPipesName(int curWorker, char** parentToWorker, char** workerToParent)
{
    //Calculate total digits of worker number to allocate correct memory
    int numWorkersTotalDigits;
    if(curWorker == 0)
    {
        numWorkersTotalDigits = 1;
    }
    else
    {
        numWorkersTotalDigits = floor(log10(abs(curWorker))) + 1;
    }

    //Allocate corrent memory for path names
    *parentToWorker = malloc((strlen("./namedPipes/parent_to_worker") + numWorkersTotalDigits + 1) * sizeof(char));
    *workerToParent = malloc((strlen("./namedPipes/worker_to_parent") + numWorkersTotalDigits + 1) * sizeof(char));

    //Concatenate named pipe name with the correct number of worker
    sprintf(*parentToWorker, "./namedPipes/parent_to_worker%d", curWorker);
    sprintf(*workerToParent, "./namedPipes/worker%d_to_parent", curWorker);
}

void removeNamedPipes(int numWorkers)
{
    for(int curWorker = 0; curWorker < numWorkers; curWorker++)
    {
        //named pipes names
        char* namedPipeParentToWorker = NULL;
        char* namedPipeWorkerToParent = NULL;

        //get pipe names
        getNamedPipesName(curWorker, &namedPipeParentToWorker, &namedPipeWorkerToParent);

        //remove pipes
        unlink(namedPipeParentToWorker);
        unlink(namedPipeWorkerToParent);

        free(namedPipeParentToWorker);
        free(namedPipeWorkerToParent);
    }
}