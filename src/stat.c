#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "include/stat.h"

void initializeStatNode(StatNodePointer stat, char* country, DateStructure date, char* disease, int stat1, int stat2, int stat3, int stat4)
{
    stat->country = malloc(sizeof(char) * strlen(country) + 1);
    strcpy(stat->country, country);

    stat->date = date;

    stat->disease = malloc(sizeof(char) * strlen(disease) + 1);
    strcpy(stat->disease, disease);

    stat->stat1 = stat1;

    stat->stat2 = stat2;

    stat->stat3 = stat3;

    stat->stat4 = stat4;
}
