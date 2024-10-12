#include <stdio.h>
#include <string.h>
#include <stdlib.h>

void read_file(char *filename, char *pattern);
int matches(char *string, char *pattern);

int main(int argc, char *argv[])
{
    /* check amount of arguments */
    if (argc < 2)
    {
        fprintf(stderr, "my-grep: searchterm [file ...]\n");
        exit(1);
    }

    for (int i = 2; i < argc; i++)
    {
        read_file(argv[i], argv[1]);
    }

    if (argc == 2)
    {
        read_file(NULL, argv[1]);
    }

    return 0;
}

void read_file(char *filename, char *pattern)
{
    FILE *fp;
    char *line = NULL;
    size_t len = 0;

    if (filename != NULL)
    {

        fp = fopen(filename, "r");
        if (fp == NULL)
        {
            fprintf(stderr, "my-grep: cannot open file\n");
            exit(1);
        }
    }
    else
    {
        fp = stdin;
    }

    while ((getline(&line, &len, fp)) != -1)
    {
        if (matches(line, pattern) > 0)
        {
            printf("%s", line);
        }
    }
    fclose(fp);
    free(line);
}

/* Returns 1 if a match is found, 0 if not */
int matches(char *string, char *pattern)
{
    int lenght = strlen(string);
    int pat_len = strlen(pattern);
    int j, i;

    for (i = 0; i < lenght; i++)
    {
        if (string[i] == pattern[0])
        {
            for (j = 1; j < pat_len; j++)
            {
                if (string[i + j] != pattern[j])
                {
                    break;
                }
            }
            if (j == pat_len)
            {
                return 1;
            }
        }
    }
    return 0;
}
