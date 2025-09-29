#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <locale.h>

#include "shell.h"
#include "log.h"
#include "process.h"
#include "memory.h"
#include "fs.h"

#ifdef _WIN32
#define strcasecmp _stricmp
#endif

#define VFS_FILE "vfs.dat"

// ===== Ayuda con colores (tu misma salida) =====
static void print_ayuda(void)
{
    OUT("\n\033[1;37m============================================================\033[0m\n");
    OUT("                   \033[1;36mManual de Comandos\033[0m\n");
    OUT("\033[1;37m============================================================\033[0m\n");

    OUT("\n\033[1;32m  Gestion de Procesos\033[0m\n");
    OUT("------------------------------------------------------------\n");
    OUT(" NuevoProceso <Nombre> <Rafaga>  | Crear proceso (rafaga en unidades)\n");
    OUT(" ListarProcesos                  | Listar procesos activos\n");
    OUT(" Ejecutar [Intervalo]            | Ejecutar planificador Round-Robin\n");
    OUT(" TerminarProceso <Id_Proceso>    | Terminar un proceso especifico\n");

    OUT("\n\033[1;34m  Gestion de Memoria\033[0m\n");
    OUT("------------------------------------------------------------\n");
    OUT(" AsignarMemoria <Id_Proceso> <Tamano_Bytes> | Asignar memoria al proceso\n");
    OUT(" LiberarMemoria <Id_Proceso>                | Liberar memoria del proceso\n");
    OUT(" MostrarMapaMemoria                         | Mostrar mapa de memoria\n");

    OUT("\n\033[1;33m  Sistema de Archivos Virtual (VFS)\033[0m\n");
    OUT("------------------------------------------------------------\n");
    OUT(" CrearArchivo <Nombre>          | Crear archivo en VFS\n");
    OUT(" ListarArchivos                 | Listar archivos en VFS\n");
    OUT(" MostrarContenido <Nombre>      | Mostrar contenido de un archivo\n");
    OUT(" EscribirArchivo <Nombre> [contenido] | Escribir contenido (en la misma linea)\n");
    OUT(" EliminarArchivo <Nombre>       | Eliminar archivo del VFS\n");
    OUT(" GuardarFS                      | Guardar VFS en disco (vfs.dat)\n");
    OUT(" CargarFS                       | Cargar VFS desde disco (vfs.dat)\n");

    OUT("\n\033[1;35m  Comandos del Sistema\033[0m\n");
    OUT("------------------------------------------------------------\n");
    OUT(" Ayuda                          | Mostrar este menu de ayuda\n");
    OUT(" Salir                          | Salir del sistema\n");

    OUT("\n\033[1;37m============================================================\033[0m\n\n");
}

void shell_init(void)
{
    setlocale(LC_ALL, "");

    proc_init();
    mem_init();
    fs_init();
    fs_load(VFS_FILE);

    OUT("============================================================\n");
    OUT("       Prototipo Sistema Operativo \"CinnamStrawbOS\"  \n");
    OUT("============================================================\n");
    OUT("                 Version 0.1 - 2025\n");
    OUT("------------------------------------------------------------\n");
    OUT("  Desarrollado por: Maria Alejandra Toro Ortiz\n");
    OUT("                    Kevin Steven Guzman Acevedo\n");
    OUT("------------------------------------------------------------\n");
    OUT(" Escribe 'Ayuda' para ver la lista de comandos disponibles.\n");
    OUT("============================================================\n\n");
}

int shell_handle_line(char *line)
{
    if (!line) return 0;
    line[strcspn(line, "\n")] = '\0';
    if (!*line) return 0;

    char *cmd = strtok(line, " ");
    if (!cmd) return 0;

    // ===== Procesos =====
    if (strcasecmp(cmd, "Ayuda") == 0) {
        print_ayuda();
    }
    else if (strcasecmp(cmd, "NuevoProceso") == 0) {
        char *name = strtok(NULL, " ");
        char *burst_s = strtok(NULL, " ");
        if (!name || !burst_s) { OUT("Uso: NuevoProceso <name> <burst>\n"); return 0; }
        int burst = atoi(burst_s);
        proc_create(name, burst);
    }
    else if (strcasecmp(cmd, "ListarProcesos") == 0) {
        proc_list();
    }
    else if (strcasecmp(cmd, "Ejecutar") == 0) {
        char *q = strtok(NULL, " ");
        int quantum = q ? atoi(q) : 1;
        proc_scheduler_rr(quantum);
    }
    else if (strcasecmp(cmd, "TerminarProceso") == 0) {
        char *pid_s = strtok(NULL, " ");
        if (!pid_s) { OUT("Uso: TerminarProceso <pid>\n"); return 0; }
        int pid = atoi(pid_s);
        proc_kill(pid);
    }

    // ===== Memoria =====
    else if (strcasecmp(cmd, "AsignarMemoria") == 0) {
        char *pid_s = strtok(NULL, " ");
        char *size_s = strtok(NULL, " ");
        if (!pid_s || !size_s) { OUT("Uso: AsignarMemoria <pid> <size>\n"); return 0; }
        int pid = atoi(pid_s), size = atoi(size_s);
        int blk = mem_alloc(pid, size);
        if (blk == -1) OUT("[ERROR] Fallo la asignacion de memoria (no hay fit o limite)\n");
        else OUT("[OK] Memoria asignada (block idx=%d) para PID=%d\n", blk, pid);
    }
    else if (strcasecmp(cmd, "LiberarMemoria") == 0) {
        char *pid_s = strtok(NULL, " ");
        if (!pid_s) { OUT("Uso: LiberarMemoria <pid>\n"); return 0; }
        int pid = atoi(pid_s);
        int freed = mem_free_by_owner(pid);
        if (freed == 0) OUT("[WARNING] No se encontraron bloques para PID=%d\n", pid);
        else OUT("[INFO] Liberados %d bloque(s) para PID=%d\n", freed, pid);
    }
    else if (strcasecmp(cmd, "MostrarMapaMemoria") == 0) {
        mem_map();
    }

    // ===== VFS =====
    else if (strcasecmp(cmd, "CrearArchivo") == 0) {
        char *name = strtok(NULL, " ");
        if (!name) { OUT("Uso: CrearArchivo <name>\n"); return 0; }
        if (fs_mkfile(name) == -1) OUT("[WARNING] No se pudo crear (ya existe o espacio lleno)\n");
        else OUT("[OK] Archivo creado: %s\n", name);
    }
    else if (strcasecmp(cmd, "ListarArchivos") == 0) {
        fs_ls();
    }
    else if (strcasecmp(cmd, "MostrarContenido") == 0) {
        char *name = strtok(NULL, " ");
        if (!name) { OUT("Uso: MostrarContenido <name>\n"); return 0; }
        char buf[MAX_CONTENT];
        if (fs_read(name, buf, sizeof(buf)) == -1) OUT("[WARNING] Archivo no encontrado\n");
        else OUT("Contenido de %s:\n%s\n", name, buf);
    }
    else if (strcasecmp(cmd, "EscribirArchivo") == 0) {
        char *name = strtok(NULL, " ");
        char *rest = strtok(NULL, "");   // toma TODO lo que sigue como contenido
        if (!name) { OUT("Uso: EscribirArchivo <name> [contenido]\n"); return 0; }

        if (fs_find(name) == -1) {
            if (fs_mkfile(name) == -1) { OUT("[WARNING] No se pudo crear archivo\n"); return 0; }
        }
        if (!rest) { OUT("[INFO] Provee contenido en la misma linea. Ej:\n");
                     OUT("       EscribirArchivo notas Hola mundo\n"); return 0; }

        fs_write(name, rest);
        OUT("[OK] Contenido escrito en %s\n", name);
    }
    else if (strcasecmp(cmd, "EliminarArchivo") == 0) {
        char *name = strtok(NULL, " ");
        if (!name) { OUT("Uso: EliminarArchivo <name>\n"); return 0; }
        if (fs_rmfile(name) == -1) OUT("[WARNING] Archivo no encontrado\n");
        else OUT("[OK] Archivo eliminado: %s\n", name);
    }
    else if (strcasecmp(cmd, "GuardarFS") == 0) {
        if (fs_save(VFS_FILE) == 0) OUT("[OK] VFS guardado en %s\n", VFS_FILE);
        else OUT("[ERROR] Error guardando VFS\n");
    }
    else if (strcasecmp(cmd, "CargarFS") == 0) {
        if (fs_load(VFS_FILE) == 0) OUT("[OK] VFS cargado desde %s\n", VFS_FILE);
        else OUT("[WARNING] Error cargando VFS (existe %s?)\n", VFS_FILE);
    }

    // ===== Sistema =====
    else if (strcasecmp(cmd, "Salir") == 0) {
        OUT("[INFO] Saliendo...\n");
        return 1;
    }
    else {
        OUT("[WARNING] Comando desconocido. Escribe 'Ayuda'\n");
    }
    return 0;
}
