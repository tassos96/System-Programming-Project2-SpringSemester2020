#ifndef NAMED_PIPES_H
#define NAMED_PIPES_H

//Function to create all named pipes that program needs
void createNamedPipes(int numWorkers);

//Function that creates named pipes name
void getNamedPipesName(int curWorker, char** parentToWorker, char** workerToParent);

//Function to remove all named pipes created
void removeNamedPipes(int numWorkers);

#endif