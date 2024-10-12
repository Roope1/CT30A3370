#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* For stack data structure*/
typedef struct stackNode
{
    char *str;
    struct stackNode *next;
} NODE;

NODE *push(NODE *head, char *str);
NODE *pop_and_print(NODE *head, FILE *stream);
void print_stack(NODE *head);

char *line = NULL;
size_t len = 0;

int main(int argc, char *argv[])
{
    /* Declaring variables */
    FILE *input_stream;
    FILE *output_stream;

    NODE *stack = NULL;

    /* Checking amount of arguments and creating stream handles */
    if (argc == 1)
    {
        input_stream = stdin;
        output_stream = stdout;
    }

    else if (argc == 2)
    {
        if ((input_stream = fopen(argv[1], "r")) == NULL)
        {
            fprintf(stderr, "error: cannot open file '%s'", argv[1]);
            exit(1);
        }
        output_stream = stdout;
    }

    else if (argc == 3)
    {
        /* Checking if input and output files are the same */
        if (strcmp(argv[1], argv[2]) == 0)
        {
            fprintf(stderr, "Input and output file must differ\n");
            exit(1);
        }

        if ((input_stream = fopen(argv[1], "r")) == NULL)
        {
            fprintf(stderr, "error: cannot open file '%s'", argv[1]);
            exit(1);
        }

        if ((output_stream = fopen(argv[2], "w")) == NULL)
        {
            fprintf(stderr, "error: cannot open file '%s'", argv[2]);
            exit(1);
        }
    }

    else /* Unknown amount of arguments*/
    {
        fprintf(stderr, "usage: reverse <input> <output>\n");
        exit(1);
    }

    /* Reading from input line by line */

    while ((getline(&line, &len, input_stream)) != -1)
    {
        if (strlen(line) == 1)
        {
            break;
        }
        stack = push(stack, line);
    }

    /* pop stack and print*/
    while (stack != NULL)
    {
        stack = pop_and_print(stack, output_stream);
    }

    /* Ending procedures */
    fclose(input_stream);
    fclose(output_stream);
    free(line);
    exit(0);
}

/* Adds a NODE to stack and returns the new first element in LL*/
NODE *push(NODE *head, char *str)
{
    char *tempstr;
    NODE *newnode;

    if ((newnode = (NODE *)malloc(sizeof(NODE))) == NULL)
    {
        fprintf(stderr, "malloc failed");
        exit(1);
    }

    if ((tempstr = (char *)malloc((strlen(str) + 1))) == NULL)
    {
        fprintf(stderr, "malloc failed");
        exit(1);
    }

    strcpy(tempstr, str);
    newnode->str = tempstr;
    newnode->next = head;
    head = newnode;
    return head;
}

/* pops the top element and prints it, returns the new top element*/
NODE *pop_and_print(NODE *head, FILE *stream)
{
    NODE *newhead = head->next;

    fprintf(stream, "%s", head->str);
    free(head->str);
    free(head);

    return newhead;
}
