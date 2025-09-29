#include "log.h"
#include <stdio.h>
#include <stdarg.h>
#include <string.h>

static out_sink_fn SINK = NULL;
static log_mode_t MODE = LOG_MODE_CLI;

// elimina secuencias ANSI tipo \x1B[ ... m
static void strip_ansi(const char *in, char *out, size_t outsz) {
    size_t oi = 0;
    for (size_t i = 0; in[i] && oi + 1 < outsz; ) {
        if (in[i] == '\x1B' && in[i+1] == '[') {
            // saltar hasta 'm' o fin
            i += 2;
            while (in[i] && in[i] != 'm') i++;
            if (in[i] == 'm') i++;
        } else {
            out[oi++] = in[i++];
        }
    }
    out[oi] = '\0';
}

void set_output(out_sink_fn fn) { SINK = fn; }
void set_output_mode(log_mode_t mode) { MODE = mode; }

void outf(const char *fmt, ...) {
    char buf[4096];
    va_list ap;
    va_start(ap, fmt);
    vsnprintf(buf, sizeof buf, fmt, ap);
    va_end(ap);

    if (MODE == LOG_MODE_GUI) {
        char clean[4096];
        strip_ansi(buf, clean, sizeof clean);
        if (SINK) SINK(clean); else fputs(clean, stdout);
    } else {
        if (SINK) SINK(buf); else fputs(buf, stdout);
    }
}

