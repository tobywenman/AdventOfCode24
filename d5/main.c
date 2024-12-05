#include <stdlib.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <stdint.h>


typedef struct 
{
    int8_t* data;
    size_t len;
    size_t allocSize;
}dynPageArr_t;


typedef struct 
{
    dynPageArr_t rules[100]; 

    dynPageArr_t* pages;
    size_t numPages;
}puzzleState_t;

dynPageArr_t initPageArr()
{
    dynPageArr_t out;
    out.allocSize = 0;
    out.len = 0;
    out.data = NULL;
    return out;
}

void pushBackPage(dynPageArr_t* rule, int8_t newVal)
{
    if (rule->allocSize == 0)
    {
        rule->data = malloc(1);
        rule->allocSize = 1;
    }

    if (rule->len == rule->allocSize)
    {
        rule->allocSize *= 2;
        rule->data = realloc(rule->data, rule->allocSize);
    }

    rule->data[rule->len] = newVal;
    ++rule->len;
}

puzzleState_t readFromFile(char* fileName)
{
    FILE* fptr = fopen(fileName, "r");

    char* line=NULL;
    size_t length = 0;
    int len;

    puzzleState_t out;
    out.numPages = 0;
    out.pages = NULL;

    for (unsigned i=0; i<100; i++)
    {
        out.rules[i] = initPageArr();
    }

    bool parsingRules = true;
    while ((len = getline(&line, &length, fptr)) >= 0)
    {
        if (len == 1)
        {
            parsingRules = false;
            continue;
        }

        if (parsingRules)
        {
            int idx = atoi(line);
            pushBackPage(&out.rules[idx], atoi(line+3));

            printf("%d|%d\n",idx,atoi(line+3));
        }
        else
        {
            ++out.numPages;
            out.pages = realloc(out.pages, sizeof(dynPageArr_t)*out.numPages); // I cannot be bothered to write another dynamic array allocator...
            out.pages[out.numPages-1] = initPageArr();
            for (int i=0; i<len; i+=3)
            {
                pushBackPage(&out.pages[out.numPages-1], atoi(line+i));
            }
        }
    }
    printf("\n");
    return out;
}

void printState(const puzzleState_t* state)
{
    for (unsigned i=0; i<100; i++)
    {
        for (size_t j=0; j<state->rules[i].len; j++)
        {
            printf("%d|%d\n",i,state->rules[i].data[j]);
        }
    }
    printf("\n");
    for (size_t i=0; i<state->numPages; i++)
    {
        for (size_t j=0; j<state->pages[i].len; j++)
        {
            if (j+1 == state->pages[i].len)
            {
                printf("%d\n", state->pages[i].data[j]);
            }
            else
            {
                printf("%d,", state->pages[i].data[j]);
            }
        }
    }
}

static puzzleState_t state;

int compare(const void* a, const void* b)
{
    int8_t page1 = *(int8_t*)a;
    int8_t page2 = *(int8_t*)b;

    for (size_t i=0; i<state.rules[page1].len; i++)
    {
        if (state.rules[page1].data[i] == page2)
        {
            return -1;
        }
    }
    for (size_t i=0; i<state.rules[page2].len; i++)
    {
        if (state.rules[page2].data[i] == page1)
        {
            return 1;
        }
    }
    return 0;
}

int main()
{
    

    state = readFromFile("inp.txt");

    printState(&state);

    int part1 = 0;
    int part2 = 0;

    for (size_t i=0; i<state.numPages; i++)
    {
        bool failed = false;
        bool banned[100]; // Numbers which cannot be seen without failing a check
        memset(banned,0,sizeof(bool)*100);

        for (size_t j=state.pages[i].len; j-->0;)
        {
            int8_t page = state.pages[i].data[j];
            if (banned[page])
            {
                failed = true;
                break;
            }

            for (size_t k=0; k<state.rules[page].len; k++)
            {
                banned[state.rules[page].data[k]] = true;
            }
        }
        if (!failed)
        {
            part1 += state.pages[i].data[(state.pages[i].len/2)];
        }
        else
        {
            qsort(state.pages[i].data, state.pages[i].len, 1, compare);
            part2 += state.pages[i].data[(state.pages[i].len/2)];
        }
        printf("\n");
    }

    printf("part1: %d\n", part1);
    printf("part2: %d\n", part2);
}