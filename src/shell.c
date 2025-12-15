#include "shell.h"

#include <stdlib.h>
#include <termios.h>
#include <unistd.h>

void enable_raw_mode()
{
    tcgetattr(STDIN_FILENO, &orig_termios);

    struct termios raw = orig_termios;
    raw.c_lflag &= ~(ECHO | ICANON);
    raw.c_cc[VMIN]  = 1;
    raw.c_cc[VTIME] = 0;

    tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw);
}

/*
 * Gets input from raw_mode and
 * manages Cursor & special chars:
 */
char *read_line_interactive(void)
{
    static char buffer[MAX_INPUT_SIZE];
    int         len    = 0;
    int         cursor = 0;

    while (1) {
        char c;
        read(STDIN_FILENO, &c, 1);

        if (c == '\n' || c == '\r') {
            buffer[len] = '\0';
            write(STDOUT_FILENO, "\n", 1);
            return buffer;
        }

        // backspace
        if (c == 127) {
            if (cursor == 0 || len == 0) {
                continue;
            }

            memmove(&buffer[cursor - 1], &buffer[cursor], len - cursor);
            len--;
            cursor--;
        }

        // arrow keys
        else if (c == 27)
        {
            char seq[2];
            read(STDIN_FILENO, &seq[0], 1);
            read(STDIN_FILENO, &seq[1], 1);

            if (seq[0] == '[') {
                if (seq[1] == 'D' && cursor > 0) {
                    cursor--;
                }
                if (seq[1] == 'C' && cursor < len) {
                    cursor++;
                }
            }
        }

        // normal char
        else if (len < (int)sizeof(buffer) - 1)
        {
            memmove(&buffer[cursor + 1], &buffer[cursor], len - cursor);
            buffer[cursor] = c;
            len++;
            cursor++;
        }

        // REDRAW LINE
        write(STDOUT_FILENO, "\r$ ", 3);
        write(STDOUT_FILENO, buffer, len);
        write(STDOUT_FILENO, "\033[K", 3);

        // Reposition cursor
        char seq[32];
        snprintf(seq, sizeof(seq), "\r\033[%dC", cursor + 2);
        write(STDOUT_FILENO, seq, strlen(seq));

        fflush(stdout);
    }
}

/*
 * Tokenize input line into argv.
 * Supports quoted strings:
 */
char **parse_input(char *line)
{
    char **argv = malloc(sizeof(char *) * MAX_TOKENS);
    int    argc = 0;

    int   is_env_var = 0;
    int   in_quotes  = 0;     // STATE: inside quotes?
    char *start      = NULL;  // STATE: currently building a token?

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

static int is_var_char(char c)
{
    return isalnum(c) || c == '_';
}

static char *expand_token(const char *src)
{
    size_t cap = (strlen(src) * 2) + 1;
    char  *out = malloc(cap);
    char  *dst = out;

    while (*src) {
        if (*src != '$') {
            *dst++ = *src++;
            continue;
        }

        src++;

        char var[128];
        int  i = 0;

        while (*src && is_var_char(*src)) {
            var[i++] = *src++;
        }

        var[i] = '\0';

        char *val = getenv(var);
        if (val) {
            size_t len = strlen(val);
            memcpy(dst, val, len);
            dst += len;
        }
    }

    *dst = '\0';
    return out;
}

void expand_env_vars(char **argv)
{
    for (int i = 0; argv[i]; i++) {
        if (strchr(argv[i], '$')) {
            argv[i] = expand_token(argv[i]);
        }
    }
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

    expand_env_vars(argv);

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
