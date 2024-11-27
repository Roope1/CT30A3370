# include <stdio.h>
# include <stdlib.h>
# include <string.h>
# include <unistd.h>
# include <sys/wait.h>


char *remove_newline(char *str) {
    if (str[strlen(str) - 1] == '\n') {
        str[strlen(str) - 1] = '\0';
    }
    return str;
}

void execute_command(char **path, char *command, char *args) {
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

            // create args array

            char *args[] = {full_path, NULL}; 
            
            // fork and exec
            pid_t pid = fork();
            if (pid < 0) {
                fprintf(stderr, "Error forking\n");
                exit(1);
            }
            else if (pid == 0) {
                execvp(args[0], args);
            }
            else {
                wait(NULL);
            }
        }
        free(full_path);
    }
}

char* get_args(char *input) {
    char *args = (char *)malloc(strlen(input) + 1);
    if (args == NULL) {
        fprintf(stderr, "Error allocating memory for args\n");
        exit(1);
    }
    strcpy(args, input);
    printf("args: %s\n", args);
    return args;
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

        // get raw input
        char *raw_input = (char *)malloc(strlen(input) + 1);
        if (raw_input == NULL) {
            fprintf(stderr, "Error allocating memory for raw_input\n");
            exit(1);
        }
        strcpy(raw_input, input);


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
            // print current token
            printf("current token: %s\n", token);
            // print full commnad
            printf("full command: %s\n", raw_input);
            

            execute_command(path, token, get_args(raw_input));

        }
    }

    return 0;

}