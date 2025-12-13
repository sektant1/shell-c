#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

#include "shell.h"

/*
 * Main shell loop
 */
int main(void)
{
    char input[1024];

    while (1) {
        printf("$ ");
        fflush(stdout);

        if (!fgets(input, sizeof(input), stdin)) {
            break;
        }

        input[strcspn(input, "\n")] = '\0';

        execute_command(input);
    }

    return 0;
}
