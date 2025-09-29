#ifndef LOG_H
#define LOG_H
#include <stdarg.h>

typedef void (*out_sink_fn)(const char *text);

typedef enum { LOG_MODE_CLI = 0, LOG_MODE_GUI = 1 } log_mode_t;

void set_output(out_sink_fn fn);
void set_output_mode(log_mode_t mode);  // NUEVO
void outf(const char *fmt, ...);

#define OUT(...) outf(__VA_ARGS__)
#endif

