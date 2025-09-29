#ifndef SHELL_H
#define SHELL_H

// Inicializa subsistemas y muestra banner de bienvenida
void shell_init(void);

// Procesa UNA línea de comando; devuelve 1 si pidió salir, 0 en caso contrario
int shell_handle_line(char *line);

#endif
