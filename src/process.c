#include <stdio.h>      // Para printf (mensajes al usuario)
#include <string.h>     // Para strncpy (copiar nombre del proceso)
#include <unistd.h>     // Para sleep() (simulación de tiempo en scheduler)
#include "process.h"    // Cabecera con definición de Proc, MAX_PROCS, etc.

// ======================================================
// 📌 Variables globales
// ======================================================
static Proc procs[MAX_PROCS];  // Tabla de procesos (simulación de PCB)
static int next_id = 0;        // Próximo ID de proceso a asignar

// ======================================================
// 📌 proc_init()
// Inicializa la tabla de procesos: marca todo como vacío
// ======================================================
void proc_init() {
    for (int i = 0; i < MAX_PROCS; ++i) {
        procs[i].id = -1;          // Sin ID asignado
        procs[i].alive = 0;        // Proceso no está activo
        procs[i].mem_owner_id = -1;// Ningún bloque de memoria asignado
    }
    next_id = 0;                   // Reiniciar contador de procesos
}

// ======================================================
// 📌 proc_create(name, burst)
// Crea un nuevo proceso con nombre y burst time dado.
// Devuelve el ID del proceso o -1 si no hay espacio.
// ======================================================
int proc_create(const char *name, int burst) {
    if (next_id >= MAX_PROCS) {
        printf("[WARNING] Limite de procesos alcanzado (%d)\n", MAX_PROCS);
        return -1; // No hay espacio
    }

    int idx = next_id++;   // Asignar nuevo índice
    procs[idx].id = idx;   // ID del proceso
    strncpy(procs[idx].name, name, sizeof(procs[idx].name)-1); // Guardar nombre
    procs[idx].burst = burst;       // Tiempo total requerido
    procs[idx].remaining = burst;   // Tiempo restante = burst inicial
    procs[idx].alive = 1;           // Activo
    procs[idx].mem_owner_id = -1;   // Aún sin memoria asignada

    printf("[OK] Proceso creado: ID=%d, name=%s, burst=%d\n",
           idx, procs[idx].name, burst);

    return idx;
}

// ======================================================
// 📌 proc_list()
// Lista todos los procesos con sus atributos principales
// ======================================================
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

// ======================================================
// 📌 proc_count()
// Cuenta procesos vivos con tiempo restante > 0
// ======================================================
int proc_count() {
    int c = 0;
    for (int i = 0; i < next_id; ++i) {
        if (procs[i].alive && procs[i].remaining > 0) c++;
    }
    return c;
}

// ======================================================
// 📌 proc_kill(id)
// Termina un proceso específico por su ID.
// Devuelve 0 si se eliminó, -1 si no existe.
// ======================================================
int proc_kill(int id) {
    if (id < 0 || id >= next_id || procs[id].id == -1) return -1;

    procs[id].alive = 0;      // Marcamos como muerto
    procs[id].remaining = 0;  // Ya no tiene CPU por ejecutar
    printf("[INFO] Proceso ID=%d terminado por peticion\n", id);

    return 0;
}

// ======================================================
// 📌 proc_scheduler_rr(quantum)
// Implementa un scheduler Round-Robin simplificado.
// Cada unidad de tiempo = 1 segundo (sleep).
// ======================================================
void proc_scheduler_rr(int quantum) {
    if (quantum <= 0) quantum = 1; // Quantum mínimo = 1

    printf("\n[INFO] Iniciando scheduler Round-Robin (quantum=%d unidades)\n", quantum);

    int remaining_procs = proc_count();

    // Mientras existan procesos listos
    while (remaining_procs > 0) {
        for (int i = 0; i < next_id; ++i) {
            // Saltar si no está activo
            if (!procs[i].alive || procs[i].remaining <= 0) continue;

            // Determinar cuánto ejecuta este proceso
            int exec = (procs[i].remaining > quantum) ? quantum : procs[i].remaining;

            printf("[OK] Ejecutando PID=%d (%s) por %d unidad(es). Restante: %d\n",
                   procs[i].id, procs[i].name, exec, procs[i].remaining);

            // Simular ejecución en intervalos de 1 segundo
            for (int t = 0; t < exec; ++t) {
                sleep(1); // Simula uso de CPU
                procs[i].remaining -= 1; // Reducir tiempo restante
                printf("   [OK] PID=%d: ejecutado 1 unidad, resta %d\n",
                       procs[i].id, procs[i].remaining);

                if (procs[i].remaining <= 0) break; // Terminó el proceso
            }

            // Si terminó durante este quantum → marcar como finalizado
            if (procs[i].remaining <= 0) {
                procs[i].alive = 0;
                printf("[INFO] PID=%d (%s) finalizado\n", procs[i].id, procs[i].name);
            }
        }

        // Recalcular cuántos procesos quedan
        remaining_procs = proc_count();
    }

    printf("[INFO] Scheduler finalizado. No quedan procesos listos.\n\n");
}
