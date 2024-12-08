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


typedef struct 
{
    char freq;
    int x,y;
}antenna_t;


typedef struct 
{
    antenna_t* data;
    size_t len;
    size_t allocSize;
}antennas_t;

antennas_t initAnts()
{
    antennas_t out;
    out.allocSize = 0;
    out.len = 0;
    out.data = NULL;
    return out;
}

void pushBackAnt(antennas_t* antennas, antenna_t newVal)
{
    if (antennas->allocSize == 0)
    {
        antennas->data = malloc(sizeof(antennas_t));
        antennas->allocSize = 1;
    }

    if (antennas->len == antennas->allocSize)
    {
        antennas->allocSize *= 2;
        antennas->data = realloc(antennas->data, antennas->allocSize*sizeof(antennas_t));
    }

    antennas->data[antennas->len] = newVal;
    ++antennas->len;
}

bool inBounds(const grid_t* grid, int x, int y)
{
    if (x >= 0 && x < grid->x)
    {
        if (y >= 0 && y < grid->y)
        {
            return true;
        }
    }
    return false;
}

int main()
{
    grid_t grid = initGridFromFile("inp.txt");

    antennas_t antennas = initAnts();

    for (size_t y=0; y<grid.y; y++)
    {
        for (size_t x=0; x<grid.x; x++)
        {
            char gridVal = idxGrid(&grid, x,y);
            if (gridVal != '.')
            {
                antenna_t newAnt;
                newAnt.freq = gridVal;
                newAnt.x = x;
                newAnt.y = y;
                
                pushBackAnt(&antennas, newAnt);
            }
        }
    }

    for (size_t i=0; i<antennas.len; i++)
    {
        printf("antenna: %d,%d,%c\n", antennas.data[i].x, antennas.data[i].y, antennas.data[i].freq);
    }

    bool *antiNodesP1 = calloc(grid.x*grid.y, sizeof(bool)); 
    bool *antiNodesP2 = calloc(grid.x*grid.y, sizeof(bool)); 

    for (size_t i=0; i<antennas.len; i++)
    {
        for (size_t j=i+1; j<antennas.len; j++)
        {
            char freq1,freq2;
            freq1 = antennas.data[i].freq;
            freq2 = antennas.data[j].freq;
            
            if (freq1 != freq2)
            {
                continue;
            }

            int X1 = antennas.data[i].x;
            int X2 = antennas.data[j].x;

            int Y1 = antennas.data[i].y;
            int Y2 = antennas.data[j].y;

            // Part 1 solution
            // Simply add/Subtract one delta between the two to each antenna and check their in bounds
            int Xd = X1 - X2;
            int Yd = Y1 - Y2;

            int Xr = X1 + Xd;
            int Yr = Y1 + Yd;

            if (inBounds(&grid, Xr, Yr))
            {
                antiNodesP1[Yr*grid.x + Xr] = true;
            }

            Xr = X2 - Xd;
            Yr = Y2 - Yd;

            if (inBounds(&grid, Xr, Yr))
            {
                antiNodesP1[Yr*grid.x + Xr] = true;
            }


            // Part 2 solution
            // Did some maths here, basically (xr,yr) = t*(x1-x2,y1-y2) + (x1,y1)
            // If you do some suffling you can get a function for t in terms of x, (Xr-X1) / (X1-X2), if this is an integer then its a valid node

            // Xr is the x position we are checking
            // Find the first one which is an integer
            Xr = X1;
            // Now increment by X1-X2, as they are all guaranteed to be integers too
            // This avoids the expensive mod operator (probably 10 ish cycles non-pipelined)
            for (Xr; Xr<grid.x; Xr+=abs(X1-X2))
            {
                Yr = ((Xr - X1)/(X1-X2)) * (Y1-Y2) + Y1;
                if (Yr >= 0 && Yr < grid.y)
                {
                    antiNodesP2[Yr*grid.x + Xr] = true;
                }
                else
                {
                    break;
                }
            }
            Xr = X1 - abs(X1-X2);
            for (Xr; Xr>=0; Xr-=abs(X1-X2))
            {
                Yr = ((Xr - X1)/(X1-X2)) * (Y1-Y2) + Y1;
                if (Yr >= 0 && Yr < grid.y)
                {
                    antiNodesP2[Yr*grid.x + Xr] = true;
                }
                else
                {
                    break;
                }
            }
        }
    }

    int part1 = 0;
    int part2 = 0;
    for (size_t y=0; y<grid.y; y++)
    {
        for (size_t x=0; x<grid.x; x++)
        {
            if (antiNodesP1[y*grid.x + x])
            {
                ++part1;
            }
            if (antiNodesP2[y*grid.x + x])
            {
                printf("#");
                ++part2;
            }
            else
            {
                printf(".");
            }
        }
        printf("\n");
    }
    printf("part1: %d\n", part1);
    printf("part2: %d\n", part2);
}