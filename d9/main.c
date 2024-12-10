#include <stdlib.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <stdint.h>

typedef struct 
{
    int* data;
    size_t len;
    size_t allocSize;
}expandData_t;

expandData_t initData()
{
    expandData_t out;
    out.allocSize = 0;
    out.len = 0;
    out.data = NULL;
    return out;
}

void pushBackData(expandData_t* expandData, int newVal, size_t repeat)
{
    size_t newLen = expandData->len + repeat;

    if (newLen >= expandData->allocSize)
    {
        expandData->allocSize = newLen*2;
        expandData->data = realloc(expandData->data, expandData->allocSize*sizeof(int));
    }

    for (size_t i=expandData->len; i<newLen; i++)
    {
        expandData->data[i] = newVal;
    }

    expandData->len = newLen;
}

expandData_t dataFromStr(const char* line)
{
    expandData_t out = initData();

    bool isData = true;
    int dataIdx = 0;
    size_t i=0;
    while (line[i] != 0)
    {
        if (isData)
        {
            pushBackData(&out, dataIdx, line[i]-48);
            ++dataIdx;
        }
        else
        {
            pushBackData(&out, -1, line[i]-48);
        }
        isData = !isData;
        ++i;
    }
    return out;
}

void compressData(expandData_t* expandData)
{
    size_t lastSeenEmpty = 0;
    for(size_t i=expandData->len; i-->lastSeenEmpty;)
    {
        int curVal = expandData->data[i];
        if (curVal == -1)
        {
            continue;
        }
        expandData->data[i] = -1;

        while (lastSeenEmpty != expandData->len && expandData->data[lastSeenEmpty] != -1)
        {
            ++lastSeenEmpty;
        }
        
        expandData->data[lastSeenEmpty] = curVal;
    }
}

void printData(const expandData_t* expandData)
{
    for (size_t i=0; i<expandData->len; i++)
    {
        if (expandData->data[i] == -1)
        {
            printf(".");
        }
        else
        {
            printf("%d", expandData->data[i]);
        }
    }
    printf("\n");
}


void compressData2(expandData_t* expandData)
{
    for(size_t i=expandData->len; i-->0;)
    {
        int curVal = expandData->data[i];
        if (curVal == -1)
        {
            continue;
        }

        size_t dataLen=1;
        while (i > 0 && expandData->data[i-1] == curVal)
        {
            ++dataLen;
            --i;
        }
        
        size_t gapSize=0;
        for (size_t j=0; j<i; j++)
        {
            if (expandData->data[j] == -1)
            {
                ++gapSize;
            }
            else
            {
                gapSize = 0;
            }
            if (gapSize == dataLen)
            {
                for (size_t k=j-gapSize+1; k<j+1; k++)
                {
                    expandData->data[k] = curVal;
                }
                for (size_t k=i; k<i+dataLen; k++)
                {
                    expandData->data[k] = -1;
                }
                break;
            }
        }
    }
}

uint64_t checksum(expandData_t* expandData)
{
    uint64_t out = 0;
    for (size_t i=1; i<expandData->len; i++)
    {
        if (expandData->data[i] == -1)
        {
            continue;
        }
        out += i*expandData->data[i];
    }
    return out;
}

int main()
{
    FILE* fptr = fopen("inp.txt", "r");

    char* line=NULL;
    size_t length = 0;
    int len;

    len = getline(&line, &length, fptr);

    if (len<0)
    {
        fprintf(stderr, "error reading file\n");
        exit(1);
    }

    expandData_t expandData = dataFromStr(line);


    compressData(&expandData);

    uint64_t part1 = checksum(&expandData);

    printf("part1: %ld\n", part1);

    expandData_t expandData2 = dataFromStr(line);

    compressData2(&expandData2);

    uint64_t part2 = checksum(&expandData2);
    printf("part2: %ld\n", part2);
}