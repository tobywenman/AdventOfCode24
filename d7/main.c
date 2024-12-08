#include <stdlib.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <stdint.h>

typedef struct 
{
    uint64_t* data;
    size_t len;
    size_t allocSize;
}testNums_t;

testNums_t initNums()
{
    testNums_t out;
    out.allocSize = 0;
    out.len = 0;
    out.data = NULL;
    return out;
}

void pushBackNum(testNums_t* rule, uint64_t newVal)
{
    if (rule->allocSize == 0)
    {
        rule->data = malloc(sizeof(uint64_t));
        rule->allocSize = 1;
    }

    if (rule->len == rule->allocSize)
    {
        rule->allocSize *= 2;
        rule->data = realloc(rule->data, rule->allocSize*sizeof(uint64_t));
    }

    rule->data[rule->len] = newVal;
    ++rule->len;
}

typedef struct
{
    uint64_t testValue;
    testNums_t nums;
}test_t;

uint64_t testNumsAddMul(const test_t* test)
{
    uint64_t endVal = ((uint64_t)1) << (test->nums.len-1);

    uint64_t pluses;
    for (pluses=0; pluses<endVal; pluses++)
    {
        uint64_t output = test->nums.data[0];
        for (size_t i=0; i<test->nums.len-1; i++)
        {
            if ((pluses >> i) & 1)
            {
                output += test->nums.data[i+1];
            }
            else
            {
                output *= test->nums.data[i+1];
            }
        }
        if (output == test->testValue)
        {
            return output;
        }
    }
    return 0;
}

typedef enum
{
    ADD=0,
    MUL=1,
    CAT=2,
}oper_t;

uint64_t concat(uint64_t a, uint64_t b)
{
    uint64_t pow = 10;
    while(b >= pow)
    {
        pow *= 10;
    }

    return a * pow + b;
}


uint64_t testNumsAddMulCat(const test_t* test)
{
    oper_t* eq = calloc(test->nums.len-1, sizeof(oper_t));

    bool carry = false;
    while (!carry)
    {
        for (size_t i=0; i<test->nums.len-1; i++)
        {
            if (eq[i] == 2)
            {
                carry = true;
                eq[i] = 0;
            }
            else
            {
                carry = false;
                ++eq[i];
                break;
            }
        }
        uint64_t output = test->nums.data[0];
        for (size_t k=0; k<test->nums.len-1; k++)
        {
            switch (eq[k])
            {
            case ADD: output += test->nums.data[k+1]; break;
            case MUL: output *= test->nums.data[k+1]; break;
            case CAT: output = concat(output, test->nums.data[k+1]); break;
            default:
                fprintf(stderr, "invalid operType\n");
                break;
            }
        }
        if (output == test->testValue)
        {
            free(eq);
            return output;
        }
    }
    free(eq);
    return 0;
}

int main()
{

    size_t numTests = 0;
    test_t tests[1000];

    FILE* fptr = fopen("inp.txt", "r");

    char* line=NULL;
    size_t length = 0;
    int len;

    while ((len = getline(&line, &length, fptr)) > 0)
    {
        tests[numTests].nums = initNums();
        char *numStart = NULL;
        tests[numTests].testValue = strtoul(line, &numStart, 10);
        numStart += 2;

        while (*numStart != '\n')
        {
            pushBackNum(&tests[numTests].nums, strtoul(numStart, &numStart, 10));
        }
        ++numTests;
    }

    free(line);

    uint64_t part1 = 0;
    uint64_t part2 = 0;
    for (size_t i=0; i<numTests; i++)
    {
        part1 += testNumsAddMul(&tests[i]);
        part2 += testNumsAddMulCat(&tests[i]);
    }
    printf("part1: %ld\n", part1);
    printf("part2: %ld\n", part2);
}