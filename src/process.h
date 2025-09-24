#ifndef PROCESS_H
#define PROCESS_H

#define MAX_PROCS 32

typedef struct {
    int id;
    char name[32];
    int burst;        // tiempo total requerido (unidades)
    int remaining;    // tiempo restante
    int alive;        // 1 = activo, 0 = terminado
    int mem_owner_id; // id de bloque de memoria asignado (o -1)
} Proc;

void proc_init();
int proc_create(const char *name, int burst);
void proc_list();
int proc_count();
void proc_scheduler_rr(int quantum);
int proc_kill(int id);

#endif
