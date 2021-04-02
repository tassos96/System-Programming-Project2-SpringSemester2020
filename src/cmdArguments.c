/**
 * This file contains methods for reading command line arguments.
**/
#define  _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


#include "include/cmdArguments.h"

#define ARGS_SIZE 7 //Correct number of command line total args

/**
 *  Function for reading arguments from command line and initalize variables on main.c 
 **/
void readCommandLineArguments(int* numWorkers, int* bufferSize, char** inputDirPath, int argc, const char* argv[])
{
    //Check if all needed arguments passed
    if(argc < ARGS_SIZE || argc > ARGS_SIZE)
    {
        printArgsErrorMsg();
        exit(1);    //Terminate program
    }
    else
    {
        for(int i = 1; i < argc - 1; i+=2)
        {
            if(strcmp(argv[i], "-w") == 0)  //num of workers
            {
                char* endptr;
                *numWorkers = strtol(argv[i+1], &endptr, 10);
                if (*endptr) //if the result is 0, error
                {
                    printArgsErrorMsg();
                    exit(1);
                }
            }
            else if(strcmp(argv[i], "-b") == 0) //Size of buffer
            {
                char* endptr;
                *bufferSize = strtol(argv[i+1], &endptr, 10);
                if(*endptr)//if the result is 0, error
                {
                    printArgsErrorMsg();
                    exit(1);
                }
            }
            else if(strcmp(argv[i], "-i") == 0) //Input directory path
            {
                *inputDirPath = malloc(strlen(argv[i+1]) * sizeof(char) + 1);
                strcpy(*inputDirPath, argv[i+1]);
            }
            else
            {
                printArgsErrorMsg();
                exit(1);
            }
        }
    }
}

/**
 * Function to make sure every argument needed passed from command line 
 **/
void checkForArgsError(int numWorkers, int bufferSize, char* inputDirPath)
{
    if ((inputDirPath == NULL) || (numWorkers == 0) || (bufferSize == 0) )
    {
        printArgsErrorMsg();    //Print error
        exit(2);                //Terminate program
    }  
}

/**
 * Function to print error messages for command line arguments
 **/
void printArgsErrorMsg()
{
    printf("Error occured: Something is wrong with arguments. \n");
    printf("Please try again.\n");
}