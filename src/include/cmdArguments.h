#ifndef CMD_ARGUMENTS_H
#define CMD_ARGUMENTS_H

// Function for reading arguments from command line and initalize variables on main.c 
void readCommandLineArguments(int* numWorkers, int* bufferSize, char** inputDirPath, int argc, const char* argv[]);
//Function to make sure every argument needed passed from command line 
void checkForArgsError(int numWorkers, int bufferSize, char* inputDirPath);
//Function to print error messages for command line arguments
void printArgsErrorMsg();

#endif