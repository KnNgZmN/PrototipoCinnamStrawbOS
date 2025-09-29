#include "log.h"
#include <stdio.h>
#include <stdarg.h>
#include <string.h>

static out_sink_fn SINK = NULL;
static log_mode_t MODE = LOG_MODE_CLI;

// Convierte texto plano en formato mejorado para GUI
static void format_for_gui(const char *in, char *out, size_t outsz) {
    size_t oi = 0;
    int line_start = 1;

    for (size_t i = 0; in[i] && oi + 1 < outsz; i++) {
        char c = in[i];

        // Detectar líneas de separación (===== o -----)
        if (line_start && (c == '=' || c == '-')) {
            // Contar cuántos caracteres iguales siguen
            int count = 0;
            size_t j = i;
            while (in[j] == c && count < 20) { count++; j++; }

            if (count >= 10) { // Es una línea de separación
                // Reemplazar con línea decorativa
                const char *decoration = (c == '=') ?
                    "\n╔══════════════════════════════════════════════════════════════╗\n" :
                    "\n├──────────────────────────────────────────────────────────────┤\n";

                size_t dec_len = strlen(decoration);
                if (oi + dec_len < outsz) {
                    strcpy(out + oi, decoration);
                    oi += dec_len;
                }

                // Saltar la línea original
                while (in[i] && in[i] != '\n') i++;
                if (in[i] == '\n') i++;
                line_start = 1;
                continue;
            }
        }

        // Agregar caracteres normales
        if (oi + 1 < outsz) {
            // Agregar prefijos visuales a líneas especiales
            if (line_start) {
                if (strncmp(&in[i], "[OK]", 4) == 0) {
                    const char *prefix = "✅ ";
                    strcpy(out + oi, prefix);
                    oi += strlen(prefix);
                } else if (strncmp(&in[i], "[ERROR]", 7) == 0) {
                    const char *prefix = "❌ ";
                    strcpy(out + oi, prefix);
                    oi += strlen(prefix);
                } else if (strncmp(&in[i], "[WARNING]", 9) == 0) {
                    const char *prefix = "⚠️ ";
                    strcpy(out + oi, prefix);
                    oi += strlen(prefix);
                } else if (strncmp(&in[i], "[INFO]", 6) == 0) {
                    const char *prefix = "ℹ️ ";
                    strcpy(out + oi, prefix);
                    oi += strlen(prefix);
                } else if (c == ' ' && strncmp(&in[i+1], "Gestion", 7) == 0) {
                    const char *prefix = "\n📋 ";
                    strcpy(out + oi, prefix);
                    oi += strlen(prefix);
                    i++; // Saltar el espacio
                } else if (c == ' ' && in[i+1] != ' ' && in[i+1] != '\n') {
                    // Líneas de comandos (con espacio al inicio)
                    const char *prefix = "  🔸 ";
                    strcpy(out + oi, prefix);
                    oi += strlen(prefix);
                    i++; // Saltar el espacio
                }
                line_start = 0;
            }

            out[oi++] = c;
            if (c == '\n') line_start = 1;
        }
    }

    // Agregar borde final si es un bloque de ayuda
    if (strstr(out, "Manual de Comandos") || strstr(out, "Gestion")) {
        const char *footer = "\n╚══════════════════════════════════════════════════════════════╝\n";
        size_t footer_len = strlen(footer);
        if (oi + footer_len < outsz) {
            strcpy(out + oi, footer);
            oi += footer_len;
        }
    }

    out[oi] = '\0';
}

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

// Función principal de salida con formateo inteligente
void outf(const char *fmt, ...) {
    char buf[4096];
    va_list ap;
    va_start(ap, fmt);
    vsnprintf(buf, sizeof buf, fmt, ap);
    va_end(ap);

    if (MODE == LOG_MODE_GUI) {
        char clean[4096];
        char formatted[8192];  // Buffer más grande para el formato

        // Primero quitar secuencias ANSI
        strip_ansi(buf, clean, sizeof clean);

        // Luego aplicar formateo para GUI
        format_for_gui(clean, formatted, sizeof formatted);

        if (SINK) SINK(formatted); else fputs(formatted, stdout);
    } else {
        if (SINK) SINK(buf); else fputs(buf, stdout);
    }
}

