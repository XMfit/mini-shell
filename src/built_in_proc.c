#include "../include/shell.h"

int own_cd(char **args) {
    // No arguments
    if (args[1] == NULL) {
        char *home = getenv("HOME");
        if (chdir(home) != 0)
            perror("cd");
    } else if (args[2] == NULL) {
        if (chdir(args[1]) != 0) {
            perror("cd");
        }
    } else {
        fprintf(stderr, "cd: too many arguments supplied\n");
    }

    return 0;
}

int own_history(char **args) {
    FILE *file = fopen(historyPath, "r");
    if (file == NULL)
        perror("Error opening file");

    fseek(file, 0, SEEK_END);

    long file_size = ftell(file);
    int newline_count = 0;
    char buffer[BUFFER];
    long position;

    // Loop to count newlines and set pos to the start of the last N lines
    for (position = file_size - 1; position >= 0; position--) {
        fseek(file, position, SEEK_SET);
        char c = fgetc(file);
        if (c == '\n') {
            newline_count++;
            if (newline_count > 50)
                break;
        }
    }

    fseek(file, position + 1, SEEK_SET);    // Move up one from newline char

    // While non null lines print lines
    while (fgets(buffer, BUFFER, file) != NULL) {
        printf("%s", buffer);
    }

    fclose(file);
    
    return 0;
}

int write_to_history(char **args) {
    FILE *file = fopen(historyPath, "a");
    if (file == NULL)
        perror("Error opening file");

    int i = 0;
    while (args[i] != NULL) {
        fprintf(file, "%s", args[i++]);
        if (args[i] != NULL) {
            fprintf(file, " ");
        }
    }

    fprintf(file, "\n");
    fclose(file);

    return 0;
}

int own_env(char **args) {
    FILE *file = fopen(envPath, "r");
    char line[BUFFER];
    if (file == NULL)
        perror("Error opening file");

    while (fgets(line, sizeof(line), file)) {
        printf("%s", line);
    }

    fclose(file);
    return 0;
}

int own_set(char **args) {
    int length = 0;
    while (args[length] != NULL)
        length++;

    if (length < 3) {
        fprintf(stderr, "Invalid amount of arguments: set <var_name> <value>\n");
        return 0;
    }

    // args[0] -> cmd
    // args[1] -> var
    // args[2] -> new_val

    FILE *file = fopen(envPath, "r");
    FILE *temp_file = fopen("temp.env", "w");

    char line [BUFFER];
    int replaced = 0;

    if (!file || !temp_file) {
        perror("Error opening file(s)\n");
        return 0;
    }

    while (fgets(line, BUFFER, file)) {
        // Check if line starts with target variable
        if (strncmp(line, args[1], strlen(args[1])) == 0 && !replaced) {
            // Replace line with new value
            fprintf(temp_file, "%s = \"%s\"\n", args[1], args[2]);
            replaced = 1;
        } else {
            // Write line to temp file
            fputs(line, temp_file);
        }
    }
    
    fclose(file);
    fclose(temp_file);

    if (replaced) {
        remove(envPath);
        rename("temp.env", envPath);
    } else {
        fprintf(stderr, "Error variable not found\n");
        remove("temp.env");
    }

    return 0;
}

int own_help(char **args) {
    printf("My simple shell\n");
    printf("Following built in commands:\n");
    printf("cd\n");
    printf("help\n");
    printf("history\n");
    printf("env\n");
    printf("set <var> <value>\n");
    printf("exit\n");
    printf("Use man command for more info on commands\n");
    return 0;
}

int own_exit(char **args) {
    setColor(YELLOW);
    printf("Exiting shell\n");
    resetColor();
    return 1;
}
