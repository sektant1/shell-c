#include "shell.h"

/*
 * Main shell loop
 */
int main(void)
{
    char input[MAX_INPUT_SIZE];

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
