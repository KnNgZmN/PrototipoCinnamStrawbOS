#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include "process.h"

static Proc procs[MAX_PROCS];
static int next_id = 0;

void proc_init() {
    for (int i = 0; i < MAX_PROCS; ++i) {
        procs[i].id = -1;
        procs[i].alive = 0;
        procs[i].mem_owner_id = -1;
    }
    next_id = 0;
}

int proc_create(const char *name, int burst) {
    if (next_id >= MAX_PROCS) {
        printf("[WARNING] Limite de procesos alcanzado (%d)\n", MAX_PROCS);
        return -1;
    }
    int idx = next_id++;
    procs[idx].id = idx;
    strncpy(procs[idx].name, name, sizeof(procs[idx].name)-1);
    procs[idx].burst = burst;
    procs[idx].remaining = burst;
    procs[idx].alive = 1;
    procs[idx].mem_owner_id = -1;
    printf("[OK] Proceso creado: ID=%d, name=%s, burst=%d\n", idx, procs[idx].name, burst);
    return idx;
}

void proc_list() {
    printf("ID\tName\tBurst\tRemaining\tAlive\tMemOwner\n");
    for (int i = 0; i < next_id; ++i) {
        if (procs[i].id != -1) {
            printf("%d\t%s\t%d\t%d\t\t%d\t%d\n",
                procs[i].id,
                procs[i].name,
                procs[i].burst,
                procs[i].remaining,
                procs[i].alive,
                procs[i].mem_owner_id);
        }
    }
}

int proc_count() {
    int c = 0;
    for (int i = 0; i < next_id; ++i) if (procs[i].alive && procs[i].remaining > 0) c++;
    return c;
}

int proc_kill(int id) {
    if (id < 0 || id >= next_id || procs[id].id == -1) return -1;
    procs[id].alive = 0;
    procs[id].remaining = 0;
    printf("[INFO] Proceso ID=%d terminado por peticion\n", id);
    return 0;
}

// Round-Robin scheduler (simulado): cada 'unidad' = 1 segundo
void proc_scheduler_rr(int quantum) {
    if (quantum <= 0) quantum = 1;
    printf("\n[INFO] Iniciando scheduler Round-Robin (quantum=%d unidades)\n", quantum);
    int remaining_procs = proc_count();
    while (remaining_procs > 0) {
        for (int i = 0; i < next_id; ++i) {
            if (!procs[i].alive || procs[i].remaining <= 0) continue;
            int exec = (procs[i].remaining > quantum) ? quantum : procs[i].remaining;
            printf("[OK] Ejecutando PID=%d (%s) por %d unidad(es). Restante: %d\n",
                   procs[i].id, procs[i].name, exec, procs[i].remaining);
            for (int t = 0; t < exec; ++t) {
                sleep(1); // simula trabajo (1 segundo/unidad)
                procs[i].remaining -= 1;
                printf("   [OK] PID=%d: ejecutado 1 unidad, resta %d\n", procs[i].id, procs[i].remaining);
                if (procs[i].remaining <= 0) break;
            }
            if (procs[i].remaining <= 0) {
                procs[i].alive = 0;
                printf("[INFO] PID=%d (%s) finalizado\n", procs[i].id, procs[i].name);
            }
        }
        remaining_procs = proc_count();
    }
    printf("[INFO] Scheduler finalizado. No quedan procesos listos.\n\n");
}
