#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <fcntl.h>

#define MAX_LINE 80

void parse_input(char *input, char **args) {
    int i = 0;
    char *token = strtok(input, " \n");
    while (token != NULL) {
        args[i++] = token;
        token = strtok(NULL, " \n");
    }
    args[i] = NULL;
}

int main(void) {
    char *args[MAX_LINE/2 + 1];
    char input[MAX_LINE];
    char history[MAX_LINE];
    int has_history = 0;
    int should_run = 1;

    while (should_run) {
        printf("osh>");
        fflush(stdout);
        
        // get inputs
        if (fgets(input, MAX_LINE, stdin) == NULL) break;
        // address empty enter
        if (input[0] == '\n') continue;

        // check whether use history function
        if (strncmp(input, "!!", 2) == 0) {
            if (!has_history) {
                printf("No commands in history.\n");
                continue;
            }
            printf("%s", history); // print the history command
            strcpy(input, history);
        } else {
            strcpy(history, input); // save the current command to history
            has_history = 1;
        }
        
        // parse
        parse_input(input, args);
        if (args[0] == NULL) continue;
        if (strcmp(args[0], "exit") == 0) {
            should_run = 0;
            continue;
        }

        // check "&"
        int background = 0;
        int i = 0;
        while (args[i] != NULL) i++;
        if (i > 0 && strcmp(args[i-1], "&") == 0) {
            background = 1;
            args[i-1] = NULL;
        }
        
        // fork a child process and let it execute the command
        pid_t pid = fork();
        if (pid == 0) { // child process
            // check whether a pipe is used
            int pipe_idx = -1;
            for (int j = 0; args[j] != NULL; j++) {
                if (strcmp(args[j], "|") == 0) {
                    pipe_idx = j;
                    break;
                }
            }

            // pipe is used
            if (pipe_idx != -1) {
                int fd[2];
                pipe(fd);
                args[pipe_idx] = NULL;
                if (fork() == 0) { // fork a child process
                    dup2(fd[1], STDOUT_FILENO);
                    close(fd[0]);
                    close(fd[1]);
                    execvp(args[0], args);
                    exit(1); // if failed to execute the command
                }
                dup2(fd[0], STDIN_FILENO);
                close(fd[1]);
                close(fd[0]);
                execvp(args[pipe_idx + 1], &args[pipe_idx + 1]);
                exit(1);
            }

            // redirection detection
            for (int j = 0; args[j] != NULL; j++) {
                if (strcmp(args[j], ">") == 0) {
                    int fd = open(args[j+1], O_WRONLY | O_CREAT | O_TRUNC, 0644);
                    dup2(fd, STDOUT_FILENO); // use fd to cover STDOUT_FILENO
                    args[j] = NULL;
                } else if (strcmp(args[j], "<") == 0) {
                    int fd = open(args[j+1], O_RDONLY);
                    dup2(fd, STDIN_FILENO);
                    args[j] = NULL;
                }
            }

            // execute the command
            execvp(args[0], args);
            perror("Execution failed");
            exit(1);
        } else { // parent process
            if (!background) waitpid(pid, NULL, 0);
        }
    }
    return 0;
}