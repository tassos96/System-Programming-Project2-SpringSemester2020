#ifndef STAT_H
#define STAT_H

#include "myDateStructure.h"

typedef struct Stat
{
    char* country;
    DateStructure date;
    char* disease;
    int stat1;
    int stat2;
    int stat3;
    int stat4;


    struct Stat* next;

}Stat;

typedef Stat* StatNodePointer;

void initializeStatNode(StatNodePointer stat, char* country, DateStructure date, char* disease, int stat1, int stat2, int stat3, int stat4);



#endif