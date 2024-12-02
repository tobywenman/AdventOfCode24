#include "stdlib.h"
#include "stdbool.h"
#include "stdio.h"
#include "string.h"

typedef struct
{
    int* values;
    size_t len;
    size_t buffSize;
}report_t;

void reportPushBack(report_t* in, int newVal)
{
    if (in->len == in->buffSize)
    {
        size_t newSize;
        if (in->buffSize == 0)
        {
            newSize = 1;
        }
        else
        {
            newSize = 2*in->buffSize;
        }
        in->values = realloc(in->values, newSize*sizeof(int));
        in->buffSize = newSize;
    }

    in->values[in->len] = newVal;
    ++in->len;
}

void initReport(report_t* in)
{
    in->len = 0;
    in->buffSize = 0;
    in->values = NULL;
}

#define LISTSIZE 1000

bool isSafe(report_t* in)
{
    bool seenIncrease = false;
    bool seenDecrease = false;
    for (unsigned j=0; j<in->len-1; j++)
    {
        int diff = in->values[j] - in->values[j+1];
        if (diff >= 1 && diff <=3)
        {
            seenIncrease = true;
            if (seenDecrease)
            {
                return false;
            }
        }
        else if (diff <= -1 && diff >=-3)
        {
            seenDecrease = true;
            if (seenIncrease)
            {
                return false;
            }
        }
        else
        {
            return false;
        }
    }

    return true;
}

report_t cpyAndRemoveIdx(const report_t* in, size_t idx)
{
    report_t out = *in;

    out.values = malloc(sizeof(int)*in->buffSize);
    out.len = in->len - 1;
    memcpy(out.values,in->values,sizeof(int)*idx);
    memcpy(out.values+idx, in->values+idx+1, sizeof(int)*(in->len-idx-1));
    return out;
}

int main()
{
    report_t reports[LISTSIZE];
    FILE *fptr;

    fptr = fopen("inp.txt","rb");

    char *line;
    size_t bufSize = 100;

    line = malloc(bufSize);

    int strLen;
    size_t idx=0;
    while (true)
    {
        char num1[6];
        char num2[6];

        strLen = getline(&line, &bufSize, fptr);
        if (strLen <= 0)
        {
            break;
        }

        initReport(&reports[idx]);
        char *numStart = line;
        do
        {
            reportPushBack(&reports[idx], atoi(numStart));
            numStart = strchr(numStart, ' ');
        } while (numStart++ != NULL);
        ++idx;
    }

    int safe = 0;

    for (unsigned i=0; i<idx; i++)
    {
        if (isSafe(&reports[i]))
        {
            ++safe;
        }
    }
    printf("part1: %d\n", safe);

    safe = 0;
    for (unsigned i=0; i<idx; i++)
    {
        if (isSafe(&reports[i]))
        {
            ++safe;
        }
        else
        {
            bool foundSafe = false;
            report_t newReport;
            for (unsigned j=0; j<reports[i].len; j++)
            {
                newReport = cpyAndRemoveIdx(&reports[i], j);
                if (isSafe(&newReport))
                {
                    foundSafe = true;
                    break;
                }
            }
            free(newReport.values);
            if (foundSafe)
            {
                ++safe;
            }
        }
    }

    printf("part2: %d\n", safe);
}
