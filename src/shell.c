#include "shell.h"

/*
 * Tokenize input line into argv.
 * Supports quoted strings:
 */
char **parse_input(char *line)
{
    char **argv = malloc(sizeof(char *) * MAX_TOKENS);
    int    argc = 0;

    int   in_quotes = 0;     // STATE: inside quotes?
    char *start     = NULL;  // STATE: currently building a token?

    // State transition based on chars
    for (char *p = line; *p; p++) {
        if (*p == '"') {
            in_quotes = !in_quotes;  // toggle state
            if (in_quotes) {
                start = p + 1;
            } else {
                *p           = '\0';
                argv[argc++] = start;
                start        = NULL;
            }
        } else if (!in_quotes && (*p == ' ' || *p == '\t')) {
            *p = '\0';
            if (start) {
                argv[argc++] = start;
                start        = NULL;
            }
        } else if (!start) {
            start = p;
        }
    }

    if (start) {
        argv[argc++] = start;
    }

    argv[argc] = NULL;
    return argv;
}

/*
 * Execute external (non-builtin) command
 */
void execute_external(char **argv)
{
    pid_t pid = fork();  // Create child process alongside parent

    if (pid == 0) {
        execvp(argv[0], argv);  // Never returns if sucessful
        perror(argv[0]);        // Only reached if above fails

        exit(EXIT_FAILURE);

    } else if (pid > 0) {
        int status;
        waitpid(pid, &status, 0);  // Hold the program until child exits

    } else {
        perror("fork");
    }
}

/*
 * Dispatch command: builtin or external
 */
void execute_command(char *input)
{
    char **argv = parse_input(input);

    if (!argv || !argv[0]) {
        free(argv);
        return;
    }

    for (int i = 0; i < NUM_BUILTINS; i++) {
        if (strcmp(argv[0], BUILTIN_COMMANDS[i].name) == 0) {
            if (!BUILTIN_COMMANDS[i].accepts_args && argv[1]) {
                fprintf(stderr, "%s: does not accept arguments\n", argv[0]);
                free(argv);
                return;
            }

            BUILTIN_COMMANDS[i].func(argv);
            free(argv);
            return;
        }
    }

    execute_external(argv);
    free(argv);
}
