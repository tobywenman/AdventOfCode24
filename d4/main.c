#include <stdlib.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>

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
    ssize_t len;

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
                fprintf(stderr, "line %ld is a different length to the previous: %ld\n", len, out.x);
                exit(1);
            }
        }
        out.data = realloc(out.data, out.x*(out.y+1)*sizeof(char)); // Is char always 1-byte???? 

        memcpy(&out.data[out.x*out.y],line,len*sizeof(char));

        ++out.y;
    }
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

char idxGrid(const grid_t* grid, size_t x, size_t y)
{
    return grid->data[grid->x*y + x];
}

char* idxGridRef(grid_t* grid, size_t x, size_t y)
{
    return &grid->data[grid->x*y + x];
}

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

typedef enum 
{
    N = 0,
    NE = 1,
    E = 2,
    SE = 3,
    S = 4,
    SW = 5,
    W = 6,
    NW = 7,
    DIRMAX = 8,
}dirs;

bool testXMAS(const grid_t* grid, dirs dir, int x, int y, grid_t* example)
{
    int xOff,yOff;

    switch (dir)
    {
        case N:  xOff = 0;  yOff = -1; break;
        case NE: xOff = 1;  yOff = -1; break;
        case E:  xOff = 1;  yOff = 0;  break;
        case SE: xOff = 1;  yOff = 1;  break;
        case S:  xOff = 0;  yOff = 1;  break;
        case SW: xOff = -1; yOff = 1;  break;
        case W:  xOff = -1; yOff = 0;  break;
        case NW: xOff = -1; yOff = -1; break;
        default: 
            fprintf(stderr, "invalid dir enum: %d\n", (int)dir); 
            exit(1);
    }

    char* testStr = "XMAS";
    size_t len = strlen(testStr);

    int newX = x;
    int newY = y;

    for (size_t i=0; i<len; i++)
    {
        if (newX < 0 || newX >= grid->x)
        {
            return false;
        }
        if (newY < 0 || newY >= grid->y)
        {
            return false;
        }
        if (idxGrid(grid, newX, newY) != testStr[i])
        {
            return false;
        }
        newX += xOff;
        newY += yOff;
    }

    newX = x;
    newY = y;

    for (size_t i=0; i<len; i++)
    {
        char* touched = idxGridRef(example, newX, newY);

        *touched = idxGrid(grid, newX, newY);

        newX += xOff;
        newY += yOff;
    }
    return true;
}

bool testP2(const grid_t* grid, int x, int y)
{
    if (x < 1 || x >= grid->x-1)
    {
        return false;
    }
    if (y < 0 || y >= grid->y-1)
    {
        return false;
    }
    if (idxGrid(grid,x,y) != 'A')
    {
        return false;
    }

    if (idxGrid(grid, x-1, y-1) == 'M')
    {
        if (idxGrid(grid, x+1, y+1) != 'S')
        {
            return false;
        }
    }
    else if (idxGrid(grid, x-1, y-1) == 'S')
    {
        if (idxGrid(grid, x+1, y+1) != 'M')
        {
            return false;
        }
    }
    else
    {
        return false;
    }

    if (idxGrid(grid, x+1, y-1) == 'M')
    {
        if (idxGrid(grid, x-1, y+1) != 'S')
        {
            return false;
        }
    }
    else if (idxGrid(grid, x+1, y-1) == 'S')
    {
        if (idxGrid(grid, x-1, y+1) != 'M')
        {
            return false;
        }
    }
    else
    {
        return false;
    }

    return true;
}


int main()
{
    grid_t data = initGridFromFile("inp.txt");
    grid_t example = initBlankGrid(data.x,data.y);

    unsigned part1 = 0;

    for (size_t y=0; y<data.y; y++)
    {
        for (size_t x=0; x<data.x; x++)
        {
            for (unsigned dir=0; dir<DIRMAX; dir++)
            {
                if (testXMAS(&data, (dirs)dir, x, y, &example))
                {
                    ++part1;
                }
            }
        }
    }

    unsigned part2 = 0;

    for (size_t y=1; y<data.y-1; y++)
    {
        for (size_t x=1; x<data.x-1; x++)
        {
            if (testP2(&data, x, y))
            {
                ++part2;
            }
        }
    }

    printf("part1: %u\n", part1);
    printf("part2: %u\n", part2);
}
