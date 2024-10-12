#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define BUFFER_SIZE 512

void readfile(char *filename);

int main(int argc, char *argv[])
{

    /* Not enough arguments */
    if (argc < 2)
    {
        fprintf(stderr, "my-zip: file1 [file2...]\n");
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
    char buffer[BUFFER_SIZE];
    char current_char = '1'; // initialized with a number to know for first time check
    int count = 0;

    fp = fopen(filename, "r");
    if (fp == NULL)
    {
        fprintf(stderr, "my-zip: cannot open file\n");
        exit(1);
    }

    while (fgets(buffer, BUFFER_SIZE, fp) != NULL)
    {
        for (int i = 0; i < BUFFER_SIZE - 1; i++)
        {
            if (buffer[i] == '\n') // end of line continue to next
            {
                fwrite(&count, sizeof(int), 1, stdout);
                fwrite(&current_char, sizeof(char), 1, stdout);
                current_char = buffer[i];
                count = 1;
                fwrite("\n", sizeof(char), 1, fp);
                continue;
            }

            if (buffer[i] == '\0') // end of doc
            {
                fwrite(&count, sizeof(int), 1, stdout);
                fwrite(&current_char, sizeof(char), 1, stdout);
                break;
            }

            // initialize current_char with the first character of the buffer
            // FOR 1st PASSTHROUGH ONLY
            if (current_char == '1')
            {
                current_char = buffer[0];
            }

            /* character has switched == write down results*/
            if (buffer[i] != current_char)
            {
                fwrite(&count, sizeof(int), 1, stdout);
                fwrite(&current_char, sizeof(char), 1, stdout);
                current_char = buffer[i];
                count = 1;
            }

            else
            {
                count++;
            }
        }
    }
    fclose(fp);
}