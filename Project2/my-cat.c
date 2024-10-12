#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define BUFFER_SIZE 1024

void read_file(char *filename);

int main(int argc, char *argv[])
{
    /* if no arguments are given exit immediately*/
    if (argc < 2)
    {
        exit(0);
    }

    /* loop over the given filenames*/
    for (int i = 1; i < argc; i++)
    {
        read_file(argv[i]);
    }

    return 0;
}

/* reads file and prints it to stdout*/
void read_file(char *filename)
{
    char buffer[BUFFER_SIZE];
    /* open file and check it actually opened*/
    FILE *fp;
    fp = fopen(filename, "r");
    if (fp == NULL)
    {
        fprintf(stderr, "my-cat: cannot open file\n");
        exit(1);
    }

    /* print file contents to stdout */
    while (fgets(buffer, BUFFER_SIZE, fp) != NULL)
    {
        printf("%s", buffer);
    }

    fclose(fp);
}