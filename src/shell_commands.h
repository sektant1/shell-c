#include <stdio.h>
#include <stdlib.h>

typedef void (*command_func_t)(char **argv);

typedef struct
{
    const char    *name;  // Command name ex: "quit"
    command_func_t func;  // Pointer to the cmd function
} shell_command_t;

extern const shell_command_t BUILTIN_COMMANDS[];
extern const int             NUM_BUILTINS;

void do_quit();
void do_pwd();
void do_cd(char *path);
