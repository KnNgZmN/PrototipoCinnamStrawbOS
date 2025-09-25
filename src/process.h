#ifndef PROCESS_H
#define PROCESS_H

// =====================================================
// 📌 Definiciones para la Gestión de Procesos
// =====================================================

// Número máximo de procesos que se pueden manejar simultáneamente
#define MAX_PROCS 32

// =====================================================
// 📌 Estructura que modela un Proceso
// =====================================================
typedef struct {
    int id;             // Identificador único del proceso (PID)
    char name[32];      // Nombre del proceso
    int burst;          // Tiempo total requerido de CPU (unidades)
    int remaining;      // Tiempo restante por ejecutar
    int alive;          // Estado del proceso: 1 = activo, 0 = terminado
    int mem_owner_id;   // ID del bloque de memoria asignado (o -1 si ninguno)
} Proc;

// =====================================================
// 📌 Funciones de gestión de procesos
// =====================================================

// Inicializa la tabla de procesos, marcando todos como vacíos
void proc_init();

// Crea un nuevo proceso con nombre y ráfaga de CPU especificada
// Devuelve el PID del proceso creado o -1 en caso de error
int proc_create(const char *name, int burst);

// Lista en consola todos los procesos con su información básica
void proc_list();

// Retorna la cantidad de procesos activos con tiempo restante > 0
int proc_count();

// Planificador Round-Robin: ejecuta procesos en intervalos de 'quantum'
void proc_scheduler_rr(int quantum);

// Termina el proceso con el ID dado (marca como no activo)
// Devuelve 0 si tuvo éxito o -1 si el PID no es válido
int proc_kill(int id);

#endif // PROCESS_H

