# include <stdio.h>
# include <stdlib.h>
# include <string.h>
# include <unistd.h>
# include <sys/wait.h>
# include <stdbool.h>

#define DEBUG 1

char *remove_newline(char *str) {
    if (str[strlen(str) - 1] == '\n') {
        str[strlen(str) - 1] = '\0';
    }
    return str;
}

int get_array_len(char **array) {
    // loop over array until NULL is found
    int i = 0;
    while (array[i] != NULL) {
        i++;
    }
    return i;
}



// FIXME: adapt to the new input handling system
int execute_command(char **input_list, int index, char **path) {
    int return_val = 0;
    // get the command from input_list via index
    char *command = input_list[index];

    // get arguments for the command if exists -> rest of input_list until | or end
    // loop over arguments until | or end
    char **arg_list = (char **)malloc(sizeof(char*));

    for (int i = index; i < get_array_len(input_list); i++) {
        return_val++;

        if (DEBUG) {
            printf("input_list[%d]: %s\n", i, input_list[i]);
        }

        if (strcmp(input_list[i], "|") == 0) {
            printf("pipe found\n"); //TODO: implement piping
            break;
        }

        arg_list = (char **)realloc(arg_list, sizeof(char*) * ((i - index) + 1));
        if (arg_list == NULL) {
            fprintf(stderr, "Error allocating memory for args\n");
            exit(1);
        }

        arg_list[i] = (char *)malloc(strlen(input_list[i]));
        strcpy(arg_list[i], input_list[i]);

        if (DEBUG){
            printf("arg_list[%d]: %s\n", i, arg_list[i]);
        }
    }

    if (DEBUG) {
        printf("\ncommand: %s\n", command);
        printf("args: %s\n", arg_list[0]);
        fflush(stdout);
    }

    printf("starting to search for command\n");
    // loop over path and try to find the command
    for (int i = 0; i < sizeof(&path) / sizeof(char*); i++) {
        char *full_path = (char *)malloc(strlen(path[i]) + strlen(command) + 1);
        if (full_path == NULL) {
            fprintf(stderr, "Error allocating memory for full_path\n");
            exit(1);
        }

        strcpy(full_path, path[i]);
        strcat(full_path, "/");
        strcat(full_path, command);

        if (DEBUG) {
            printf("full path: %s\n", full_path);  
        }

        // check if file exists and is executable
        if (access(full_path, X_OK) != -1) {

            // fork and exec
            pid_t pid = fork();
            if (pid < 0) {
                fprintf(stderr, "Error forking\n");
            }
            else if (pid == 0) {
                execvp(arg_list[0], arg_list);
            }
            else {
                wait(NULL);
                free(full_path);
                return 1;
            }
        }
        free(full_path);

    }
    return return_val;

}


int main(int argc, char *argv[]) {

    char **path = NULL;
    path = (char **)malloc(sizeof(char*));
    if (path == NULL) {
        fprintf(stderr, "Error allocating memory for path\n");
    }

    // initialize path to /bin
    path[0] = "/bin";

    char *input = NULL;
    size_t len = 0;
    
    while (1)
    {
        // get input
        fprintf(stdout, "wish> ");
        getline(&input, &len, stdin);

        // get raw input
        char *raw_input = (char *)malloc(strlen(input) + 1);
        if (raw_input == NULL) {
            fprintf(stderr, "Error allocating memory for raw_input\n");
        }
        strcpy(raw_input, input);


        // tokenize input on spaces

        // tokenize input on spaces to a list of arguments
        
        // NEW!!
        /* Before passing the input to the if-else statement that determines what should be done
        format the input to a list of char* because later on there might be a need for tokenization.
        if this is not done the strtok pointers break as multiple tokens is not supported in the c language*/
        char *token = strtok(input, " ");
        // format tokens as a list
        char **input_list = (char **)malloc(sizeof(char*));
        if (input_list == NULL) {
            fprintf(stderr, "Unable to parse command");
            continue;
        }

        int i = 0;
        while (token != NULL) {
            token = remove_newline(token);
            printf("%s\n", token);
            input_list = (char **)realloc(input_list, sizeof(char*) * (i + 1));
            input_list[i] = (char *)malloc(sizeof(char) * strlen(token));
            strcpy(input_list[i], token);
            i++;
            token = strtok(NULL, " ");
        }

        printf("input_list[0]: %s\n", input_list[0]);
        
        // this index keeps track of what part of the command is being treated,
        // all functions after this index return the number to add to this index
        int index = 0;
        // reindex everything

        // check for built in commands
        if (strcmp(input_list[index], "exit") == 0) {

            // TODO: free everything needed
            free(path);

            exit(0);
        }
        else if (strcmp(input_list[index], "cd") == 0) {    
            chdir(input_list[index++]);
            index++;
        }
        else if (strcmp(input_list[index], "path") == 0) {
            // TODO: make sure everything is freed properly and move this code to a function
            // check instructions, this is probably done extremely wrong as its not very terminal-ish
            char *path_input = NULL;
            size_t len = 0;
            printf("Give the new path comma separated: ");
            getline(&path_input, &len, stdin);
            // tokenize path_input on commas
            char *path_token = strtok(path_input, ",");
            // free path
            free(path);
            // allocate memory for path
            path = (char **)malloc(sizeof(char*));
            if (path == NULL) {
                fprintf(stderr, "Error allocating memory for path\n");
                exit(1);
            }
            // add each path to path array
            int i = 0;
            while (path_token != NULL) {
                if (DEBUG) {
                    printf("%s\n", path_token);
                }
                // realloc path to fit all paths
                path = (char **)realloc(path, sizeof(char*) * (i + 1));
                if (path == NULL) {
                    fprintf(stderr, "Error reallocating memory for path\n");
                    exit(1);
                }
                // remove newline from path_token
                remove_newline(path_token);
                path[i] = (char *)malloc(strlen(path_token) );
                if (path[i] == NULL) {
                    fprintf(stderr, "Error allocating memory for path[%d]\n", i);
                    exit(1);
                }

                strcpy(path[i], path_token);
                i++;
                path_token = strtok(NULL, ",");
            }

            if (DEBUG) {
                // print all strings in path array
                //for (int i = 0; i < sizeof(&path) / sizeof(char *); i++) {
                //    fprintf(stdout, "%s\n", path[i]);
                //}
                //printf("%s", path[0]);   
                //printf("%s", path[1]);   
            }
        }
        else if (DEBUG && strcmp(input_list[index], "printpath") == 0)
        {
            // print all strings in path array
            for (int i = 0; i < sizeof(&path) / sizeof(char *); i++) {
                fprintf(stdout, "%s\n", path[i]);
            }

        }
        
        else {
            // check path for commands
            // print current token
            if (DEBUG) {
                // print full commnad
            }
            
            int return_val = execute_command(input_list, index, path);
            index = index + return_val;

        }
    }

    return 0;

}