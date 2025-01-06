#include "../include/shell.h"

// function prototypes
void execute_pipes(char ***commands, int num_commands);
void execute_out_redir(char ***commands);
void execute_in_redir(char ***commands);
void parse_args_into_commands(char *args[], char ***commands, int *num_commands, char *delim);
int has_flag(char **args, int args_length, char *delim);

int new_process(char **args) {
    // get length of **args
    int args_length = 0;
    while (args[args_length] != NULL)
        args_length++;


    // find if pipe is in cmd
    int has_pipe = has_flag(args, args_length, "|");
    int has_out_redir = has_flag(args, args_length, ">");
    int has_inp_redir = has_flag(args, args_length, "<");

    if (has_pipe) {
        // 2d char *pointer array
        char **commands[MAX_COMMANDS];
        int num_commands = 0;

        // fill in commands, and execute
        parse_args_into_commands(args, commands, &num_commands, "|");
        execute_pipes(commands, num_commands);

        // free up commands
        for (int i = 0; i < num_commands; i++) {
            free(commands[i]);
        }

    } else if (has_out_redir) {
        char **commands[MAX_COMMANDS];
        int num_commands = 0;

        // fill in commands
        parse_args_into_commands(args, commands, &num_commands, ">");
        execute_out_redir(commands);
    } else if (has_inp_redir) {
        char **commands[MAX_COMMANDS];
        int num_commands = 0;

        // fill in commands
        parse_args_into_commands(args, commands, &num_commands, "<");
        execute_in_redir(commands);
    } else {
        // Run single command
        pid_t pid;

        if ((pid = fork()) == -1) {
            perror("fork failed");
        }
        
        if (pid == 0) {
            execvp(args[0], args);
            perror("execvp failed");
        } else if (pid > 0) {
            wait(NULL);
        } else {
            perror("fork failed");
        }
    }

    return 0;
}

int has_flag(char **args, int args_length, char *delim) {
    int flag = 0;
    for (int i = 0; i < args_length; i++) {
        if (strcmp(args[i], delim) == 0) {
            flag++;
            break;
        }
    }
    return flag;
}

/* Piping Funcs */
void execute_pipes(char ***commands, int num_commands) {
    int pipefd[2];
    pid_t pid;
    int fd_in = 0;  // input for the next command

    for (int i = 0; i < num_commands; i++) {
        if (pipe(pipefd) == -1) {
            perror("pipe failed");
        }

        if ((pid = fork()) == -1) {
            perror("fork failed");
        }

        if (pid == 0) {
            // in child process
            dup2(fd_in, STDIN_FILENO);  // get input from the previous command
            if (i < num_commands - 1) {
                dup2(pipefd[1], STDOUT_FILENO); // redirect output to the pipe for the next command
            }
            close(pipefd[0]);  // close unused read end
            close(pipefd[1]);  // close write end after redirect

            // execute the current command
            execvp(commands[i][0], commands[i]);
            perror("execvp failed");
        } else {
            // in parent process
            wait(NULL);          // wait for the child to finish
            close(pipefd[1]);    // close write end in the parent
            fd_in = pipefd[0];   // use read end as input for the next command
        }
    }

    // idk if i need this code
    close(pipefd[0]);
    close(pipefd[1]);
}

/* Output Redirection Funcs */
void execute_out_redir(char ***commands) {
    int saved_stdout = dup(STDOUT_FILENO);
    int fd = open(*commands[1], O_WRONLY | O_CREAT | O_TRUNC, 0644);

    if (fd == -1) {
        perror("opening file failed");
        close(saved_stdout);
    }

    dup2(fd, STDOUT_FILENO);
    close(fd);

    pid_t pid;
    if ((pid = fork()) == -1) {
        perror("fork failed");
    }
    if (pid == 0) {
        execvp(commands[0][0], commands[0]);
        perror("execvp failed");
    } else if (pid > 0) {
        wait(NULL);
    } else {
        perror("fork failed");
    }

    fflush(stdout);  // force clear buffer
    dup2(saved_stdout, STDOUT_FILENO);
    close(saved_stdout);
}

/* Input Redirection Funcs */
void execute_in_redir(char ***commands) {
    pid_t pid;
    int fd, saved_stdin;
    if ((pid = fork()) == -1) {
        perror("fork failed");
    } else if (pid == 0) {
        saved_stdin = dup(STDIN_FILENO);
        fd = open(*commands[1], O_RDONLY);
        dup2(fd, STDIN_FILENO);
        close(fd);

        execvp(commands[0][0], commands[0]);

        // restore if failure
        dup2(saved_stdin, STDIN_FILENO);
        close(saved_stdin);
        perror("execvp failed");
    } else if (pid > 0) {
        wait(NULL);
    } else {
        perror("fork failed");
    }
}

// Parse into commands
void parse_args_into_commands(char *args[], char ***commands, int *num_commands, char *delim) {
    *num_commands = 0;
    int command_start = 0;
    int arg_index = 0;
    
    while (args[arg_index] != NULL) {
        if (strcmp(args[arg_index], delim) == 0) {
            // Create a command between command_start and arg_index
            int command_length = arg_index - command_start;
            commands[*num_commands] = malloc((command_length + 1) * sizeof(char *));
            
            for (int i = 0; i < command_length; i++) {
                commands[*num_commands][i] = args[command_start + i];
            }
            commands[*num_commands][command_length] = NULL;
            
            (*num_commands)++;
            command_start = arg_index + 1; // start the next command after the delim
        }
        arg_index++;
    }
    
    // Add the final command post last delim
    int command_length = arg_index - command_start;
    commands[*num_commands] = malloc((command_length + 1) * sizeof(char *));
    for (int i = 0; i < command_length; i++) {
        commands[*num_commands][i] = args[command_start + i];
    }
    commands[*num_commands][command_length] = NULL;
    (*num_commands)++;
}

