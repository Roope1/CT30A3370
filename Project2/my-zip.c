#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define BUFFER_SIZE 512

void readfile(char *filename);
void readfile2(char *filename);

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
    char current_char; 

    fp = fopen(filename, "r");
    if (fp == NULL)
    {
        fprintf(stderr, "my-zip: cannot open file\n");
        exit(1);
    }

    while (fgets(buffer, BUFFER_SIZE, fp) != NULL)
    {
        int count = 1;
        for (int i = 0; buffer[i] != '\0'; i++)
        {
            current_char = buffer[i];
            if (current_char == buffer[i + 1])
            {
                count++;
            }
            else
            {
                fwrite(&count, sizeof(int), 1, stdout);
                count = 1;
                fwrite(&current_char, sizeof(char), 1, stdout);
            }
        
        }
    } 
    fclose(fp);
}

