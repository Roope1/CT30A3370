#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <stdbool.h>

# define MAX_LENGTH 1024

char *remove_newline(char *str) {
    if (str[strlen(str) - 1] == '\n') {
        str[strlen(str) - 1] = '\0';
    }
    return str;
}


pid_t execute_command(char *command[MAX_LENGTH], char *path_list[MAX_LENGTH])
{
    pid_t pid = 0; 
    // loop over path and try to find the command
    for (int i = 0; i < MAX_LENGTH; i++) {
        if (path_list[i] == NULL) {
            break;
        }
        char *full_path = (char *)malloc(strlen(path_list[i]) + strlen(command[0]) + 1);
        if (full_path == NULL) {
            fprintf(stderr, "Error allocating memory for full_path\n");
            exit(1);
        }

        strcpy(full_path, path_list[i]);
        strcat(full_path, "/");
        strcat(full_path, command[0]);

        remove_newline(full_path);

        // check if file exists and is executable
        if (access(full_path, X_OK) != -1) {
            // fork and exec
            pid = fork();
            if (pid < 0) {
                fprintf(stderr, "Error forking\n");
            }
            else if (pid == 0) {
                execv(full_path, command);
                exit(1);
                
            }            
        }
        free(full_path);
        return pid;
    }
    return -1;
}

bool execute_builtin(char *command[MAX_LENGTH], char *path_list[MAX_LENGTH])
{
    if (strcmp(command[0], "exit") == 0) {
       
        //int i = 0;
        //while(path_list[i] != NULL) {
        //    free(path_list[i]);
        //    i++;
        //}
        int j = 0;
        while(command[j] != NULL) {
            free(command[j]);
            j++;
        }
        exit(0);
       //return true;
    }
    else if (strcmp(command[0], "cd") == 0) {
        if (command[1] == NULL) {
            fprintf(stderr, "Error: no directory specified\n");
        }
        else {
            chdir(command[1]);
        }
        return true;
    }
    else if (strcmp(command[0], "path") == 0) {
        // set path to NULL
        for (int i = 0; i < MAX_LENGTH; i++) {
            path_list[i] = NULL;
        }

        // add each path to path array
        int i = 0;
        char *path_token = strtok(command[1], ",");
        while (path_token != NULL) {
            // remove newline from path_token
            remove_newline(path_token);
            path_list[i] = path_token;
            path_list[i + 1] = NULL;

            i++;
            path_token = strtok(NULL, ",");
        }
        return true;
    }
    return false;
}

void redirect(char *command[MAX_LENGTH])
{
    int arg_count = 0;
    // count number of arguments
    for (int i = 0; i < MAX_LENGTH; i++) {
        if (command[i] == NULL) {
            break;
        }
        arg_count++;
    }

    // check for redirection
    for (int i = 0; i < arg_count; i++)
    {
        if (strcmp(command[i], ">") == 0)
        {
            // check if there is a file to redirect to
            if (i != arg_count - 2)
            {
                fprintf(stderr, "Error: no file specified for redirection\n");
                return;
            }
            else
            {
                freopen(command[i + 1], "w", stdout);
                command[i] = NULL;
                break;
            }
        }
    }
    
}

void parse_input(char *input, char *pathList[MAX_LENGTH])
{
    char *token ;
    char *command[MAX_LENGTH];
    pid_t pids[MAX_LENGTH];
    int pid_count = 0;
    bool is_builtin = false;

    input = remove_newline(input);

    token = strtok(input, " ");

    int i = 0;
    while (token != NULL) {
        if (strcmp(token, "&") == 0) {
            // check for redirection
            redirect(command);

            is_builtin = execute_builtin(command, pathList);

            if (!is_builtin) {
                int pid = execute_command(command, pathList);
                if (pid != -1) {
                    pids[pid_count] = pid;
                    pid_count++;
                }
            } 
            command[0] = NULL;
            i = 0;
        }
        else
        {
            command[i++] = token;
        }
        token = strtok(NULL, " ");
    }


    // handle last command
    if (i != 0)
    {
        command[i] = NULL;
        // check for redirection
        redirect(command);

        is_builtin = execute_builtin(command, pathList);

        int pid = (!is_builtin) ? execute_command(command, pathList) : -1;
        if (pid != -1) {
            pids[pid_count] = pid;
            pid_count++;
        }
    }

    // wait for all child processes to finish
    for (int i = 0; i < pid_count; i++) {
        waitpid(pids[i], NULL, 0);
    }

    //free(input);
    // redirect output back to stdout
    freopen("/dev/tty", "w", stdout);

    
}

void interactive()
{
    char *pathList[MAX_LENGTH];
    pathList[0] = "/bin";
    pathList[1] = NULL; // end of path list is set to NULL to indicate end of list

    char* buffer = NULL;
    size_t buf_size = 0;

    int feof = 0;

    do {
        printf("wish> ");
        feof = getline(&buffer, &buf_size, stdin);
        parse_input(buffer, pathList);

    } while (feof != -1);

    free(buffer);
    return;
}

void batch(char *filename)
{
    char *pathList[MAX_LENGTH];
    FILE *file = fopen(filename, "r");

    if (file == NULL) {
        fprintf(stderr, "Error opening file %s\n", filename);
        exit(1);
    }

    pathList[0] = "/bin";
    pathList[1] = NULL; // end of path list is set to NULL to indicate end of list 

    char *line = NULL;
    size_t len = 0;

    while (getline(&line, &len, file) != -1) {
        parse_input(line, pathList);
    }
    free(line);
    fclose(file);
    return;
}

int main(int argc, char *argv[])
{
    if (argc == 1) {
        // interactive mode
        interactive();
        exit(0);
    }
    else if (argc == 2) {
        // batch mode
        batch(argv[1]);
        exit(0);
    }
    else {
        fprintf(stderr, "Usage: %s [batch_file]\n", argv[0]);
        exit(1);
    }
}