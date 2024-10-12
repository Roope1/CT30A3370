#include <stdio.h>
#include <stdlib.h>

void readfile(char *filename);

int main(int argc, char *argv[])
{

    /* Not enough arguments */
    if (argc < 2)
    {
        fprintf(stderr, "my-unzip: file1 [file2...]\n");
        exit(1);
    }

    for (int i = 1; i < argc; i++)
    {
        readfile(argv[i]);
    }

    return 0;
}

void readfile(char *filename)
{
    FILE *fp;
    char current_char = '1'; // initialized with a number to know for first time check
    int count = 0;

    fp = fopen(filename, "rb");
    if (fp == NULL)
    {
        fprintf(stderr, "my-unzip: cannot open file\n");
        exit(1);
    }

    // get first 4 bytes for number of characters
    while (fread(&count, sizeof(int), 1, fp))
    {
        fread(&current_char, sizeof(char), 1, fp);
        // print current_char count amount of times
        for (int i = 0; i < count; i++)
        {
            fprintf(stdout, "%c", current_char);
        }
    }
    fclose(fp);
}