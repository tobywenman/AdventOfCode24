#include <stdio.h>
#include <string.h>
#include <stdbool.h>

#define PCRE2_CODE_UNIT_WIDTH 8

#include <pcre2.h>

int countStr(char* inp, bool part2, bool *enabled)
{
    char * myPattern = "(?:mul\\()(\\d*)[,](\\d*)[\\)]|(?:don't\\(\\))|(?:do\\(\\))";

    pcre2_code *re;
    PCRE2_SPTR pattern;     /* PCRE2_SPTR is a pointer to unsigned code units of */
    PCRE2_SPTR subject;     /* the appropriate width (8, 16, or 32 bits). */
    PCRE2_SPTR name_table;
    int rc;
    size_t subject_length;

    int errornumber;
    PCRE2_SIZE *ovector;
    PCRE2_SIZE erroroffset;

    pattern = (PCRE2_SPTR)myPattern;

    re = pcre2_compile(
                    pattern,               /* the pattern */
                    PCRE2_ZERO_TERMINATED, /* indicates pattern is zero-terminated */
                    0,                     /* default options */
                    &errornumber,          /* for error number */
                    &erroroffset,          /* for error offset */
                    NULL);                 /* use default compile context */

    /* Compilation failed: print the error message and exit. */
    if (re == NULL)
    {
    PCRE2_UCHAR buffer[256];
    pcre2_get_error_message(errornumber, buffer, sizeof(buffer));
    printf("PCRE2 compilation failed at offset %d: %s\n", (int)erroroffset,
    buffer);
    return 1;
    }

    pcre2_match_data *match_data;

    match_data = pcre2_match_data_create_from_pattern(re, NULL);

    subject = (PCRE2_SPTR)inp;
    subject_length = strlen(inp);

    PCRE2_SIZE start_offset = 0;
    uint32_t options = 0;

    int result = 0;
    while (true)
    {
        rc = pcre2_match(
        re,                   /* the compiled pattern */
        subject,              /* the subject string */
        subject_length,       /* the length of the subject */
        start_offset,         /* start at offset 0 in the subject */
        options,                    /* default options */
        match_data,           /* block for storing the result */
        NULL); 

        if (rc < 0)
        {
            int exitcode = 0;
            if (rc == PCRE2_ERROR_NOMATCH)
            {
                break;
            }
            else
            {
                fprintf(stderr, "Matching error %d\n", rc);
                exit(1);
            }
            switch(rc)
            {
                case PCRE2_ERROR_NOMATCH: printf("No match\n"); break;
                default:  break;
            }
        }

        ovector = pcre2_get_ovector_pointer(match_data);

        char match[100];

        PCRE2_SPTR substring_start = subject + ovector[0];
        size_t substring_length = ovector[1] - ovector[0];

        sprintf(match,"%.*s", (int)substring_length, (char *)substring_start);

        printf("match: %s\n", match);

        if (strcmp(match, "do()") == 0)
        {
            if(part2)
            {
                *enabled = true;
            }
        }
        else if (strcmp(match, "don't()") == 0)
        {
            if (part2)
            {
                *enabled = false;
            }
        }
        else if (*enabled)
        {
            char aStr[100];
            memcpy(aStr, subject + ovector[2*1], ovector[2*1+1] - ovector[2*1]);
            aStr[ovector[2*1+1] - ovector[2*1]] = 0;

            char bStr[100];
            memcpy(bStr, subject + ovector[2*2], ovector[2*2+1] - ovector[2*2]);
            bStr[ovector[2*2+1] - ovector[2*2]] = 0;

            int a = atoi(aStr);
            int b = atoi(bStr);
            
            result += a*b;
        }



        start_offset = ovector[1];

        if (ovector[0] == ovector[1])
        {
            if (ovector[0] == subject_length) break;
            options = PCRE2_NOTEMPTY_ATSTART | PCRE2_ANCHORED;
        }
    }
    pcre2_match_data_free(match_data);   /* Release memory used for the match */
    pcre2_code_free(re);

    return result;

}

int main()
{
    char * inp = "xmul(2,4)%%&mul[3,7]!@^do_not_mul(5,5)+mul(32,64]then(mul(11,8)mul(8,5))";
    
    int part1 = 0;
    int part2 = 0;

    FILE *fptr;

    fptr = fopen("../inp.txt","rb");

    char *line;
    size_t bufSize = 4000;

    line = malloc(bufSize);

    bool enabled = true;

    int strLen;
    while (true)
    {
        bool alwaysEnabled = true;
        strLen = getline(&line, &bufSize, fptr);
        if (strLen <= 0)
        {
            break;
        }

        part1 += countStr(line, false, &alwaysEnabled);
        part2 += countStr(line, true, &enabled);
    }

    free(line);

    printf("part1: %d\n", part1);
    printf("part2: %d\n", part2);

    return 0;
}