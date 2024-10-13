# include <stdio.h>
# include <stdlib.h>
# include <string.h>
# include <unistd.h>


char *remove_newline(char *str) {
    if (str[strlen(str) - 1] == '\n') {
        str[strlen(str) - 1] = '\0';
    }
    return str;
}

void execute_command(char **path, char *command) {
    // try find command in each path in array
    for (int i = 0; i < sizeof(&path) / sizeof(char*); i++) {
        char *full_path = (char *)malloc(strlen(path[i]) + strlen(command) + 1);
        if (full_path == NULL) {
            fprintf(stderr, "Error allocating memory for full_path\n");
            exit(1);
        }
        strcpy(full_path, path[i]);
        strcat(full_path, "/");
        strcat(full_path, command);
        printf("full path: %s\n", full_path);  

        // check if file exists
        if (access(full_path, F_OK) != -1) {
            // file exists
            char *args[] = {full_path, NULL};
            execvp(args[0], args); //TODO: add wait for child process to finish
        }
        free(full_path);
    }
}



int main(int argc, char *argv[]) {

    char **path = NULL;
    path = (char **)malloc(sizeof(char*));
    if (path == NULL) {
        fprintf(stderr, "Error allocating memory for path\n");
        exit(1);
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

        // tokenize input on spaces
        char *token = strtok(input, " ");
        
        // DEBUG ONLY
        if (token == 0) {
            printf("token was null");
        }

        // if last character is newline set it to null
        remove_newline(token);

        printf("%s\n", token);

        // check for built in commands
        if (strcmp(token, "exit") == 0) {

            // TODO: free everything needed
            free(path);

            exit(0);
        }
        else if (strcmp(token, "cd") == 0) {    // FIXME: If no path is given -> segfault
            // get next token
            token = strtok(NULL, " ");

            // remove newline
            remove_newline(token);
            chdir(token);
        }
        else if (strcmp(token, "path") == 0) {
           return 0; 
        }
        else if (strcmp(token, "printpath") == 0)
        {
            // print all strings in path array
            for (int i = 0; i < sizeof(&path) / sizeof(char *); i++) {
                fprintf(stdout, "%s\n", path[i]);
            }

        }
        
        else {
            // check path for commands
            execute_command(path, token);
        }
    }


    // fork ls and quit
    if (argc == 1) {
        char *args[] = {"/bin/ls", NULL};
        execvp(args[0], args);
    }
    
    return 0;

}