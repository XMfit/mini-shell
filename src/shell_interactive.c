#include "../include/shell.h"

char **my_completion(const char *text, int start, int end);
char *my_generator(const char *text, int state);
void set_file_paths();
void trim_bin(char *path);

char historyPath[BUFFER * 2];   // full path to history file
char envPath[BUFFER * 2];       // full path to environment file

void shell_interactive (void) {
    // set history and envi file paths
    set_file_paths();

    // Creating vars to store user prompted text
    char *args[COMMAND_ARGS];   // tokenized prompt
    char prompt[BUFFER * 2];    // users prompt

    // auto-completion function
    rl_attempted_completion_function = my_completion;
    int status = 0;

    print_home_msg();

    while (!status) {
        // retrieve and store cwd
        char cwd[BUFFER];
        getcwd(cwd, sizeof(cwd));

        // readline() is interactive and gets weird with setColor funcs
        // so im just manually adding the colors to the string
        snprintf(prompt, sizeof(prompt), "\033[1;35m%s:\033[0m\033[1;34m%s\033[0m# ", PROMPT, cwd);

        // Read input
        char *input = readline(prompt);
        if (input == NULL) {
            fprintf(stderr, "Error reading input");
            break;
        }

        // Parse input and store into args[]
        char *token = strtok(input, " ");
        int i = 0;
        while (token != NULL && i < COMMAND_ARGS) {
            args[i++] = token;
            token = strtok(NULL, " ");
        }
        args[i] = NULL;

        // Add input to history to be used 
        add_history(input);

        // Execute commands
        remove_quotes(args);
        status = execute_args(args);
        free(input);
    }
}

// Set file paths for history and env file
void set_file_paths() {
    // Setting up file paths for shell files
    char shellDir[BUFFER];       // str storing path of exe

    ssize_t len = readlink("/proc/self/exe", shellDir, sizeof(shellDir) - 1);
    shellDir[len] = '\0';
    trim_bin(shellDir);
    
    snprintf(historyPath, sizeof(historyPath), "%s/.mini_history", shellDir);
    snprintf(envPath, sizeof(envPath), "%s/.mini_env", shellDir);

    // Ensure history file exists
    FILE *history_file = fopen(historyPath, "a");
    if (history_file) {
        fclose(history_file);
    }

    // Ensure env file exists
    FILE *env_file = fopen(envPath, "a");
    if (env_file) {
        fclose(env_file);
    }
}
// Remove bin from filepath string
void trim_bin(char *path) {
    char *bin_pos = strstr(path, "/bin");
    if (bin_pos)
        *bin_pos = '\0';
}

// Remove quations from arguments
void remove_quotes(char **args) {
    for (int i = 0; args[i] != NULL; i++) {
        char *str = args[i];
        char *src = str;
        char *dst = str;

        while (*src) {
            if (*src != '"' && *src != '\'') {
                *dst++ = *src;
            }
            src++;
        }
        *dst = '\0'; // Null-terminate the modified string
    }
}

// Greeting :D
void print_home_msg() {
    setColor(GREEN);
    printf("\n\n");
    printf("               /$$           /$$                 /$$                 /$$ /$$\n");
    printf("              |__/          |__/                | $$                | $$| $$\n");
    printf(" /$$$$$$/$$$$  /$$ /$$$$$$$  /$$        /$$$$$$$| $$$$$$$   /$$$$$$ | $$| $$\n");
    printf("| $$_  $$_  $$| $$| $$__  $$| $$       /$$_____/| $$__  $$ /$$__  $$| $$| $$\n");
    printf("| $$ \\ $$ \\ $$| $$| $$  \\ $$| $$      |  $$$$$$ | $$  \\ $$| $$$$$$$$| $$| $$\n");
    printf("| $$ | $$ | $$| $$| $$  | $$| $$       \\____  $$| $$  | $$| $$_____/| $$| $$\n");
    printf("| $$ | $$ | $$| $$| $$  | $$| $$       /$$$$$$$/| $$  | $$|  $$$$$$$| $$| $$\n");
    printf("|__/ |__/ |__/|__/|__/  |__/|__/      |_______/ |__/  |__/ \\_______/|__/|__/\n");
    resetColor();
    printf("\n\n");
}

// Auto completion function used by readline
char **my_completion(const char *text, int start, int end) {
    rl_completion_append_character = ' ';
    return rl_completion_matches(text, my_generator);
}

// The generator function called by readline to generate possible matches
char *my_generator(const char *text, int state) {
    // Current cmd list (need to figure out how to add more automatically during runtime)
    static const char *commands[] = {"cd", "env", "help", "exit", "ls", "clear", "gcc", "git", "vim", NULL};
    static int list_index, len;
    
    // Initialize the search
    if (!state) {
        list_index = 0;
        len = strlen(text);
    }

    // Return each matching command
    while (commands[list_index]) {
        const char *command = commands[list_index++];
        if (strncmp(command, text, len) == 0) {
            return strdup(command);  // Return matching command
        }
    }

    // No more matches
    return NULL;
}
