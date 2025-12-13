#include "shell.h"

#include <stdlib.h>

const shell_command_t BUILTIN_COMMANDS[] = {
    {"quit", do_quit, 0},
    {"cd", do_cd, 1},
    {"pwd", do_pwd, 0},
};

// Split input into token before checking builtins
char **parse_input(char *line)
{
    char **argv = malloc(sizeof(char *) * MAX_TOKENS);
    int    i    = 0;

    char *token = strtok(line, " \t\n");

    while (token && i < MAX_TOKENS - 1) {
        argv[i++] = token;
        token     = strtok(NULL, " \t\n");
    }

    argv[i] = NULL;
    return argv;
}

void execute_command(char *input)
{
    char **argv = parse_input(input);

    if (argv[0] == NULL) {
        free(argv);
        return;
    }

    for (size_t i = 0; i < NUM_BUILTINS; i++) {
        if (strcmp(argv[0], BUILTIN_COMMANDS[i].name) == 0) {
            if (!BUILTIN_COMMANDS[i].accepts_args && argv[1] != NULL) {
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

    // not a valid command
    fprintf(stderr, "%s: command not found\n", argv[0]);
    free(argv);
}

void execute_external(char **argv)
{
    pid_t pid = fork();

    if (pid == 0) {
        // child
        execvp(argv[0], argv);
        perror(argv[0]);
        exit(EXIT_FAILURE);
    } else if (pid > 0) {
        // parent
        int status;
        waitpid(pid, &status, 0);
    } else {
        perror("fork");
    }
}

void do_quit(char **argv)
{
    (void)argv;
    exit(EXIT_SUCCESS);
}

void do_pwd(char **argv)
{
    char cwd[MAX_CMD_INPUT];

    if (getcwd(cwd, sizeof(cwd)) != NULL) {
        printf("%s\n", cwd);
    } else {
        perror("pwd");
    }
}

void do_cd(char **argv)
{
    char  cwd[MAX_CMD_INPUT];
    char *target = NULL;

    if (!getcwd(cwd, sizeof(cwd))) {
        perror("getcwd");
        return;
    }

    if (argv[1] == NULL || strcmp(argv[1], "~") == 0) {
        target = getenv("HOME");

    } else if (strcmp(argv[1], "-") == 0) {
        if (PREV_DIR[0] == '\0') {
            fprintf(stderr, "cd: OLDPWD env not set\n");
            return;
        }

        target = PREV_DIR;
        printf("%s\n", target);

    } else {
        target = argv[1];
    }

    if (chdir(target) != 0) {
        perror("cd");
        return;
    }

    strcpy(PREV_DIR, cwd);
}

int main(int argc, char **argv)
{
    char input[MAX_CMD_INPUT];

    while (1) {
        printf("$ ");
        fflush(stdout);

        if (!fgets(input, sizeof(input), stdin)) {
            break;
        }

        execute_command(input);
    }
    return 0;
}
