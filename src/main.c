#include "shell.h"

/*
 * Main shell loop
 */
int main(void)
{
    enable_raw_mode();

    while (1) {
        printf("$ ");
        fflush(stdout);

        char *line = read_line_interactive();
        execute_command(line);
    }
    return 0;
}
