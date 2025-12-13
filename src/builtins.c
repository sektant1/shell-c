#include "shell.h"

static char PREV_DIR[MAX_INPUT_SIZE] = "";

void do_cd(char **argv)
{
    char  cwd[MAX_INPUT_SIZE];
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

void do_pwd(char **argv)
{
    char cwd[MAX_INPUT_SIZE];
    if (getcwd(cwd, sizeof(cwd))) {
        printf("%s\n", cwd);
    }
}

void do_quit(char **argv)
{
    (void)argv;
    exit(EXIT_SUCCESS);
}

const shell_command_t BUILTIN_COMMANDS[] = {
    {"cd", do_cd, 1},
    {"pwd", do_pwd, 0},
    {"quit", do_quit, 0},
};

const int NUM_BUILTINS = sizeof(BUILTIN_COMMANDS) / sizeof(BUILTIN_COMMANDS[0]);
