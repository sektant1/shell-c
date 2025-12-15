#ifndef SHELL_H
#define SHELL_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <termios.h>
#include <unistd.h>

#define MAX_TOKENS     64
#define MAX_INPUT_SIZE 1024

typedef struct
{
    const char *name;                  // Command name ex: "quit"
    void        (*func)(char **argv);  // Pointer to the cmd function
    int         accepts_args;          // Flag for cmd args if they accept

} shell_command_t;

void   enable_raw_mode();
char  *read_line_interactive();
char **parse_input(char *line);
void   execute_command(char *input);
void   execute_external(char **argv);

// Builtins
void do_quit(char **argv);
void do_pwd(char **argv);
void do_cd(char **argv);

extern const shell_command_t BUILTIN_COMMANDS[];
extern const int             NUM_BUILTINS;
static struct termios        orig_termios;

#endif
