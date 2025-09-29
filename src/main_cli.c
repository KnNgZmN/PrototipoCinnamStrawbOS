#include <stdio.h>
#include <string.h>
#include "shell.h"
#include "log.h"

#define PROMPT "CinnamStrawbOS> "

static void console_sink(const char *s) { fputs(s, stdout); }

int main(void) {
    char line[1024];

    set_output(console_sink);  // manda OUT(...) a stdout
    shell_init();              // banner + init subsistemas

    while (1) {
        fputs(PROMPT, stdout);
        if (!fgets(line, sizeof line, stdin)) break;
        if (shell_handle_line(line)) break;  // devuelve 1 si “Salir”
    }
    return 0;
}
