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

typedef enum 
{
    N,
    E,
    S,
    W,
}dir_t;

typedef struct 
{
    int x,y;
}vec2_t;

vec2_t vec2Add(vec2_t a, vec2_t b)
{
    vec2_t out;
    out.x = a.x + b.x;
    out.y = a.y + b.y;
    return out;
}

typedef struct 
{
    vec2_t* data;
    size_t len;
    size_t allocSize;
}dynVec2Arr_t;

dynVec2Arr_t initVecArr()
{
    dynVec2Arr_t out;
    out.allocSize = 0;
    out.len = 0;
    out.data = NULL;
    return out;
}

void pushBackVec(dynVec2Arr_t* rule, vec2_t newVal)
{
    if (rule->allocSize == 0)
    {
        rule->data = malloc(sizeof(vec2_t));
        rule->allocSize = 1;
    }

    if (rule->len == rule->allocSize)
    {
        rule->allocSize *= 2;
        rule->data = realloc(rule->data, rule->allocSize*sizeof(vec2_t));
    }

    rule->data[rule->len] = newVal;
    ++rule->len;
}

typedef struct main
{
    vec2_t pos;
    dir_t dir;
}dude_t;

vec2_t getOffset(dir_t dir)
{
    vec2_t out;
    switch (dir)
    {
        case N: out.x = 0; out.y = -1; break;
        case E: out.x = 1; out.y = 0; break;
        case S: out.x = 0; out.y = 1; break;
        case W: out.x = -1; out.y = 0; break;
        default:
            fprintf(stderr, "invalid dir\n");
            exit(1);
            break;
    }

    return out;
}

dir_t rotateRight(dir_t dir)
{
    switch (dir)
    {
        case N: return E;
        case E: return S;
        case S: return W;
        case W: return N;
        default:
            fprintf(stderr, "invalid dir\n");
            exit(1);
            break;
    }
}

dude_t dudeFromGrid(grid_t* grid)
{
    dude_t dude;
    for (size_t y=0; y<grid->y; y++)
    {
        for (size_t x=0; x<grid->x; x++)
        {
            char* idxRes = idxGridRef(grid, x, y);

            bool found = true;
            switch (*idxRes)
            {
                case '^': dude.dir = N; break;
                case '>': dude.dir = E; break;
                case '<': dude.dir = W; break;
                case 'v': dude.dir = S; break;
                default:
                    found = false;
                    break;
            }

            if (found)
            {
                dude.pos.x = x;
                dude.pos.y = y;
                *idxRes = '.';
                return dude;
            }
        }
    }
}

bool updateDudePos(grid_t *data, dude_t *dude)
{
    while (true)
    {
        vec2_t newPos = vec2Add(dude->pos, getOffset(dude->dir));

        if (newPos.x < 0 || newPos.x >= data->x)
        {
            char *idxRes = idxGridRef(data, dude->pos.x, dude->pos.y);
            *idxRes = 'X';
            return false;
        }
        if (newPos.y < 0 || newPos.y >= data->y)
        {
            char *idxRes = idxGridRef(data, dude->pos.x, dude->pos.y);
            *idxRes = 'X';
            return false;
        }

        if (idxGrid(data, newPos.x, newPos.y) == '#')
        {
            dude->dir = rotateRight(dude->dir);
        }
        else
        {
            char *idxRes = idxGridRef(data, dude->pos.x, dude->pos.y);
            *idxRes = 'X';
            dude->pos = newPos;
            break;
        }
    }
    
    return true;
}

unsigned countVisits(const grid_t* grid, dynVec2Arr_t* vecArr)
{
    unsigned visits = 0;
    for (size_t x=0; x<grid->x; x++)
    {
        for (size_t y=0; y<grid->y; y++)
        {
            if (idxGrid(grid, x, y) == 'X')
            {
                vec2_t vec;
                vec.x = x;
                vec.y = y;
                pushBackVec(vecArr, vec);
                ++visits;
            }
        }
    }
    return visits;
}

void resetGrid(grid_t* grid)
{
    for (size_t i=0; i<grid->x*grid->y; i++)
    {
        if (grid->data[i] == 'X')
        {
            grid->data[i] = '.';
        }
    }
}

bool testStuck(grid_t* grid, dude_t dude)
{
    u_int64_t *lastVisit = calloc(grid->x*grid->y, sizeof(u_int64_t));

    u_int64_t visitCount=0;
    u_int64_t lastClearSeen=0;
    do
    {
        if (lastVisit[grid->x*dude.pos.y + dude.pos.x] > lastClearSeen)
        {
            free(lastVisit);
            return true;
        }
        lastVisit[grid->x*dude.pos.y + dude.pos.x] = visitCount;
        if (idxGrid(grid, dude.pos.x, dude.pos.y) == '.')
        {
            lastClearSeen = visitCount;
        }
        ++visitCount;
    }while (updateDudePos(grid, &dude));
    free(lastVisit);
    return false;
}

int main()
{
    grid_t data = initGridFromFile("inp.txt");

    dude_t initDude = dudeFromGrid(&data);
    dude_t dude = initDude;

    dynVec2Arr_t positions = initVecArr();

    while (updateDudePos(&data, &dude));

    printf("part1: %d\n", countVisits(&data, &positions));

    unsigned part2 = 0;

    for (size_t i=0; i<positions.len; i++)
    {
        char *Curpos = idxGridRef(&data, positions.data[i].x, positions.data[i].y);

        resetGrid(&data);
        *Curpos = '#';

        if (testStuck(&data, initDude))
        {
            ++part2;
        }
        *Curpos = '.';
    }
    printf("part2: %d\n", part2);
}