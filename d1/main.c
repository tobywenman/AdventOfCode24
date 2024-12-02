#include "stdlib.h"
#include "stdbool.h"
#include "stdio.h"
#include "string.h"

int compare(const void* a, const void* b) {
   return (*(int*)a - *(int*)b);
}

#define LISTSIZE 1000

int main()
{
    int arr1[LISTSIZE];
    int arr2[LISTSIZE];
    
    FILE *fptr;

    fptr = fopen("inp.txt","rb");

    char *line;
    size_t bufSize = 100;

    line = malloc(bufSize);

    int strLen;
    for (unsigned i=0; i<LISTSIZE; i++)
    {
        char num1[6];
        char num2[6];

        strLen = getline(&line, &bufSize, fptr);
        if (strLen <= 0)
        {
            break;
        }
        
        memcpy(num1, line, 5);
        memcpy(num2, line+8, 5);
        num1[5] = 0;
        num2[5] = 0;

        arr1[i] = strtoul(num1, NULL, 10);
        arr2[i] = strtoul(num2, NULL,10);
    } 
    free(line);

    qsort(arr1, LISTSIZE, sizeof(int), compare);
    qsort(arr2, LISTSIZE, sizeof(int), compare);

    int totalDiff = 0;

    for (unsigned i=0; i<LISTSIZE; i++)
    {
        int diff = abs(arr1[i]-arr2[i]);
        totalDiff += diff;
    }

    printf("part1: %d\n", totalDiff);

    int result = 0;
    for (unsigned i=0; i<LISTSIZE; i++)
    {
        int seen = 0;
        for (unsigned j=0; j<LISTSIZE; j++)
        {
            if (arr1[i] == arr2[j])
            {
                ++seen;
            }
        }
        result += seen * arr1[i];
    }
    printf("part2: %d\n", result);
}