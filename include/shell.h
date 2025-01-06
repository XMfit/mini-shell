#ifndef SHELL_H
#define SHELL_H

/* Libraries */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>     // Posix sys calls
#include <fcntl.h>      // To manipulate file descriptors
#include <sys/wait.h>   // Manage child procs
#include <libgen.h>     // File paths
#include <limits.h>     // Sys limits

/* GNU Libraries */
#include <readline/readline.h>  // Interactive read line
#include <readline/history.h>   // Track input history

/* Macros */
#define BUFFER 1024             // Standard size of string
#define COMMAND_ARGS 64         // Max number of arguments in a command
#define MAX_COMMANDS 24         // Max number of commands in a pipe chain
#define PROMPT "mini-shell"     // Current prompt

/* Vars shared between source files */
extern char historyPath[BUFFER * 2];    // Max file path size
extern char envPath[BUFFER * 2];        // Max file path size

/* functions */

// An interactive Unix RPEL shell
void shell_interactive(void);       // Main RPEL loop
void print_home_msg(void);
void remove_quotes(char **args);

// Executing Arguments
int execute_args(char **args);

// New processes
int new_process(char **args);

// built in functions
int own_cd(char **args);
int own_history(char **args);
int write_to_history(char **args);
int own_env(char **args);
int own_set(char **args);
int own_help(char **args);
int own_exit(char **args);

// colors :D

#define RED "\033[1;31m"
#define GREEN "\033[1;32m"
#define MAGENTA "\033[1;35m"
#define YELLOW "\033[1;33m"
#define CYAN "\033[1;37m"

void setColor(const char *);
void resetColor();

#endif
