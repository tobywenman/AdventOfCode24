#include <stdlib.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <stdint.h>

typedef struct 
{
    char* data;
    size_t x,y;
}grid_t;

grid_t initGridFromFile(char* fileName)
{
    FILE* fptr = fopen(fileName, "r");

    char* line=NULL;
    size_t length = 0;
    int len;

    grid_t out;
    out.data = NULL;
    out.x = 0;
    out.y = 0;

    while ((len = getline(&line, &length, fptr)) > 0)
    {
        --len;
        
        if (out.x == 0)
        {
            out.x = len;
        }
        else
        {
            if (len != out.x)
            {
                fprintf(stderr, "line %d is a different length to the previous: %ld\n", len, out.x);
                exit(1);
            }
        }
        out.data = realloc(out.data, out.x*(out.y+1)*sizeof(char)); // Is char always 1-byte???? 

        memcpy(&out.data[out.x*out.y],line,len*sizeof(char));

        ++out.y;
    }

    fclose(fptr);

    free(line);

    return out;
}

grid_t initBlankGrid(size_t x, size_t y)
{
    grid_t out;
    out.data = malloc(x*y*sizeof(char));

    memset(out.data, '.', x*y*sizeof(char));

    out.x = x;
    out.y = y;

    return out;
}

void freeGrid(grid_t* grid)
{
    free(grid->data);
    grid->data = NULL;
}

#define idxGrid(GRID,X,Y) ((GRID)->data[(GRID)->x*(Y) + (X)])

void printGrid(const grid_t* grid)
{
    for (size_t y=0; y<grid->y; y++)
    {
        for (size_t x=0; x<grid->x; x++)
        {
            printf("%c", idxGrid(grid, x, y));
            fflush(stdout);
        }
        printf("\n");
    }
}

unsigned countRoutesRecur(const grid_t* map, size_t x, size_t y, grid_t* output)
{
    char curValue = idxGrid(map, x, y);

    if (curValue == '9')
    {
        idxGrid(output, x, y) = 'x';
        return 1;
    }

    unsigned returnValue = 0;
    
    if (x+1 < map->x && idxGrid(map, x+1, y) == curValue+1)
    {
        returnValue += countRoutesRecur(map, x+1, y, output);
    }
    if (x > 0 && idxGrid(map, x-1, y) == curValue+1)
    {
        returnValue += countRoutesRecur(map, x-1, y, output);
    }
    if (y+1 < map->y && idxGrid(map, x, y+1) == curValue+1)
    {
        returnValue += countRoutesRecur(map, x, y+1, output);
    }
    if (y > 0 && idxGrid(map, x, y-1) == curValue+1)
    {
        returnValue += countRoutesRecur(map, x, y-1, output);
    }
    return returnValue;
}

void countRoutes(const grid_t* map, size_t x, size_t y, unsigned *part1, unsigned *part2)
{
    grid_t output = initBlankGrid(map->x, map->y);

    *part2 += countRoutesRecur(map, x, y, &output);

    for (size_t y=0; y<map->y; y++)
    {
        for (size_t x=0; x<map->x; x++)
        {
            if (idxGrid(&output,x,y) == 'x')
            {
                ++*part1;
            }
        }
    }

    freeGrid(&output);
}

int main()
{
    grid_t map = initGridFromFile("inp.txt");

    printGrid(&map);

    unsigned part1 = 0;
    unsigned part2 = 0;
    for (size_t y=0; y<map.y; y++)
    {
        for (size_t x=0; x<map.x; x++)
        {
            if (idxGrid(&map, x, y) == '0')
            {
                countRoutes(&map, x, y, &part1, &part2);
            }
        }
    }

    freeGrid(&map);

    printf("part1: %d\n", part1);
    printf("part2: %d\n", part2);
}