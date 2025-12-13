#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

int MAX_CMD_SIZE = 1024;

void do_quit()
{
    printf("Exiting shell...\n");
    exit(EXIT_FAILURE);
}

void do_pwd()
{
    char current_path[MAX_CMD_SIZE];
    printf("%s\n", getcwd(current_path, 1024));
}

void do_cd(char *path)
{
    char current_path[MAX_CMD_SIZE];
    printf("%s\n", getcwd(current_path, 1024));

    if (chdir(path) != 0) {
        perror("chdir() failed");
    }

    printf("%s\n", getcwd(current_path, 1024));
}

int main(int argc, char *argv[])
{
    char input[MAX_CMD_SIZE];
    printf("$ ");

    while (fgets(input, MAX_CMD_SIZE, stdin) != NULL) {
        // Flush after every printf
        setbuf(stdout, NULL);

        input[strcspn(input, "\n")] = '\0';

        if (!strcmp(input, "cd")) {
            do_cd("/home/sektant/dev");

        } else if (!strcmp(input, "pwd")) {
            do_pwd();

        } else if (!strcmp(input, "quit")) {
            do_quit();

        } else {
            printf("%s: command not found\n", input);
        }

        printf("$ ");
    }

    return 0;
}
