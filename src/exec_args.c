#include "../include/shell.h"

int execute_args(char **args) {
    // built in func list to simulate built in shell code
    char *builtin_func_list[] = {
        "cd",
        "help",
        "history",
        "env",
        "set",
        "exit"
    };

    // array of function pointers for built in functions
    int (*builtin_func[])(char **) = {
        &own_cd,
        &own_help,
        &own_history,
        &own_env,
        &own_set,
        &own_exit
    };

    if (args[0] != NULL) {
        write_to_history(args); // write cmd to history file
        // Check if the command matches a built-in function
        for (int i = 0; i < sizeof(builtin_func_list) / sizeof(char *); i++) {
            if (strcmp(args[0], builtin_func_list[i]) == 0) {
                return ((*builtin_func[i])(args));
            }
        }
        // if not execute new proc via fork()
        return new_process(args);
    }

    return 0;
}

