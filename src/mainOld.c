#include <stdio.h>     // Entrada/Salida estándar (printf, fgets, etc.)
#include <string.h>    // Manejo de cadenas (strcmp, strtok, strlen, etc.)
#include <stdlib.h>    // Funciones estándar (atoi, malloc, free, etc.)
#include <locale.h>    // Configuración regional (para soporte de UTF-8 en consola)

#include "process.h"   // Módulo de gestión de procesos
#include "memory.h"    // Módulo de gestión de memoria
#include "fs.h"        // Módulo de sistema de archivos virtual

#ifdef _WIN32
#define strcasecmp _stricmp // Compatibilidad con Windows (strcasecmp no existe)
#endif

#define PROMPT "CinnamStrawbOS> " // Prefijo del shell interactivo
#define VFS_FILE "vfs.dat"        // Nombre del archivo persistente del VFS

// ======================================================
// 📌 Función: print_ayuda()
// Muestra el menú de ayuda con todos los comandos soportados
// ======================================================
void print_ayuda()
{
    printf("\n\033[1;37m============================================================\033[0m\n");
    printf("                   \033[1;36mManual de Comandos\033[0m\n");
    printf("\033[1;37m============================================================\033[0m\n");

    // 🧑‍💻 Procesos
    printf("\n\033[1;32m  Gestion de Procesos\033[0m\n");
    printf("------------------------------------------------------------\n");
    printf(" NuevoProceso <Nombre> <Rafaga>  | Crear proceso (rafaga en unidades)\n");
    printf(" ListarProcesos                  | Listar procesos activos\n");
    printf(" Ejecutar [Intervalo]            | Ejecutar planificador Round-Robin\n");
    printf(" TerminarProceso <Id_Proceso>    | Terminar un proceso especifico\n");

    // 💾 Memoria
    printf("\n\033[1;34m  Gestion de Memoria\033[0m\n");
    printf("------------------------------------------------------------\n");
    printf(" AsignarMemoria <Id_Proceso> <Tamano_Bytes> | Asignar memoria al proceso\n");
    printf(" LiberarMemoria <Id_Proceso>                | Liberar memoria del proceso\n");
    printf(" MostrarMapaMemoria                         | Mostrar mapa de memoria\n");

    // 📂 Archivos
    printf("\n\033[1;33m  Sistema de Archivos Virtual (VFS)\033[0m\n");
    printf("------------------------------------------------------------\n");
    printf(" CrearArchivo <Nombre>          | Crear archivo en VFS\n");
    printf(" ListarArchivos                 | Listar archivos en VFS\n");
    printf(" MostrarContenido <Nombre>      | Mostrar contenido de un archivo\n");
    printf(" EscribirArchivo <Nombre>       | Escribir contenido en un archivo\n");
    printf(" EliminarArchivo <Nombre>       | Eliminar archivo del VFS\n");
    printf(" GuardarFS                      | Guardar VFS en disco (vfs.dat)\n");
    printf(" CargarFS                       | Cargar VFS desde disco (vfs.dat)\n");

    // ⚙️ Sistema
    printf("\n\033[1;35m  Comandos del Sistema\033[0m\n");
    printf("------------------------------------------------------------\n");
    printf(" Ayuda                          | Mostrar este menu de ayuda\n");
    printf(" Salir                          | Salir del sistema\n");

    printf("\n\033[1;37m============================================================\033[0m\n\n");
}

// ======================================================
// 📌 Función principal: main()
// Punto de entrada del sistema operativo prototipo
// Inicializa subsistemas y gestiona el loop interactivo
// ======================================================
int main()
{
    char line[1024];        // Buffer para almacenar la línea de entrada del usuario

    // Inicialización de subsistemas
    proc_init();            // Inicializa gestor de procesos
    mem_init();             // Inicializa gestor de memoria
    fs_init();              // Inicializa sistema de archivos
    fs_load(VFS_FILE);      // Intenta cargar sistema de archivos desde disco

    setlocale(LC_ALL, "");  // Habilita soporte UTF-8 (dependiendo del SO)

    // Mensaje de bienvenida
    printf("============================================================\n");
    printf("       Prototipo Sistema Operativo \"CinnamStrawbOS\"  \n");
    printf("============================================================\n");
    printf("                 Version 0.1 - 2025\n");
    printf("------------------------------------------------------------\n");
    printf("  Desarrollado por: Maria Alejandra Toro Ortiz\n");
    printf("                    Kevin Steven Guzman Acevedo\n");
    printf("------------------------------------------------------------\n");
    printf(" Escribe 'Ayuda' para ver la lista de comandos disponibles.\n");
    printf("============================================================\n\n");

    // Bucle principal del shell
    while (1)
    {
        printf("%s", PROMPT); // Muestra prompt en pantalla

        if (!fgets(line, sizeof(line), stdin)) // Lee comando del usuario
            break;
        line[strcspn(line, "\n")] = '\0';      // Elimina salto de línea final
        if (strlen(line) == 0)                 // Ignora entradas vacías
            continue;

        char *cmd = strtok(line, " ");         // Extrae primer token (el comando)
        if (!cmd)
            continue;

        // =============================
        //  Bloque de Procesos
        // =============================
        if (strcasecmp(cmd, "Ayuda") == 0)
        {
            print_ayuda();
        }
        else if (strcasecmp(cmd, "NuevoProceso") == 0)
        {
            char *name = strtok(NULL, " ");
            char *burst_s = strtok(NULL, " ");
            if (!name || !burst_s) {
                printf("Uso: NuevoProceso <name> <burst>\n");
                continue;
            }
            int burst = atoi(burst_s); // Convierte argumento a número
            proc_create(name, burst);  // Crea nuevo proceso
        }
        else if (strcasecmp(cmd, "ListarProcesos") == 0)
        {
            proc_list();
        }
        else if (strcasecmp(cmd, "Ejecutar") == 0)
        {
            char *q = strtok(NULL, " ");
            int quantum = q ? atoi(q) : 1;   // Quantum por defecto = 1
            proc_scheduler_rr(quantum);      // Ejecuta planificador RR
        }
        else if (strcasecmp(cmd, "TerminarProceso") == 0)
        {
            char *pid_s = strtok(NULL, " ");
            if (!pid_s) {
                printf("Uso: TerminarProceso <pid>\n");
                continue;
            }
            int pid = atoi(pid_s);
            proc_kill(pid); // Elimina proceso específico
        }

        // =============================
        //  Bloque de Memoria
        // =============================
        else if (strcasecmp(cmd, "AsignarMemoria") == 0)
        {
            char *pid_s = strtok(NULL, " ");
            char *size_s = strtok(NULL, " ");
            if (!pid_s || !size_s) {
                printf("Uso: AsignarMemoria <pid> <size>\n");
                continue;
            }
            int pid = atoi(pid_s);
            int size = atoi(size_s);
            int blk = mem_alloc(pid, size); // Intenta asignar memoria
            if (blk == -1)
                printf("[ERROR] Fallo la asignacion de memoria (no hay fit o limite)\n");
            else
                printf("[OK] Memoria asignada (block idx=%d) para PID=%d\n", blk, pid);
        }
        else if (strcasecmp(cmd, "LiberarMemoria") == 0)
        {
            char *pid_s = strtok(NULL, " ");
            if (!pid_s) {
                printf("Uso: LiberarMemoria <pid>\n");
                continue;
            }
            int pid = atoi(pid_s);
            int freed = mem_free_by_owner(pid); // Libera memoria del proceso
            if (freed == 0)
                printf("[WARNING] No se encontraron bloques para PID=%d\n", pid);
            else
                printf("[INFO] Liberados %d bloque(s) para PID=%d\n", freed, pid);
        }
        else if (strcasecmp(cmd, "MostrarMapaMemoria") == 0)
        {
            mem_map(); // Muestra estado de la memoria
        }

        // =============================
        //  Bloque de Archivos (VFS)
        // =============================
        else if (strcasecmp(cmd, "CrearArchivo") == 0)
        {
            char *name = strtok(NULL, " ");
            if (!name) {
                printf("Uso: CrearArchivo <name>\n");
                continue;
            }
            if (fs_mkfile(name) == -1)
                printf("[WARNING] No se pudo crear (ya existe o espacio lleno)\n");
            else
                printf("[OK] Archivo creado: %s\n", name);
        }
        else if (strcasecmp(cmd, "ListarArchivos") == 0)
        {
            fs_ls();
        }
        else if (strcasecmp(cmd, "MostrarContenido") == 0)
        {
            char *name = strtok(NULL, " ");
            if (!name) {
                printf("Uso: MostrarContenido <name>\n");
                continue;
            }
            char buf[MAX_CONTENT];
            if (fs_read(name, buf, sizeof(buf)) == -1)
                printf("[WARNING] Archivo no encontrado\n");
            else
                printf("Contenido de %s:\n%s\n", name, buf);
        }
        else if (strcasecmp(cmd, "EscribirArchivo") == 0)
        {
            char *name = strtok(NULL, " ");
            if (!name) {
                printf("Uso: EscribirArchivo <name>\n");
                continue;
            }
            if (fs_find(name) == -1) { // Si no existe, crearlo
                if (fs_mkfile(name) == -1) {
                    printf("[WARNING] No se pudo crear archivo\n");
                    continue;
                }
            }
            printf("Ingrese contenido (una linea). Finalizar con ENTER:\n");
            char content[MAX_CONTENT];
            if (!fgets(content, sizeof(content), stdin))
                content[0] = '\0';
            content[strcspn(content, "\n")] = '\0'; // Quita salto de línea
            fs_write(name, content);
            printf("[OK] Contenido escrito en %s\n", name);
        }
        else if (strcasecmp(cmd, "EliminarArchivo") == 0)
        {
            char *name = strtok(NULL, " ");
            if (!name) {
                printf("Uso: EliminarArchivo <name>\n");
                continue;
            }
            if (fs_rmfile(name) == -1)
                printf("[WARNING] Archivo no encontrado\n");
            else
                printf("[OK] Archivo eliminado: %s\n", name);
        }
        else if (strcasecmp(cmd, "GuardarFS") == 0)
        {
            if (fs_save(VFS_FILE) == 0)
                printf("[OK] VFS guardado en %s\n", VFS_FILE);
            else
                printf("[ERROR] Error guardando VFS\n");
        }
        else if (strcasecmp(cmd, "CargarFS") == 0)
        {
            if (fs_load(VFS_FILE) == 0)
                printf("[OK] VFS cargado desde %s\n", VFS_FILE);
            else
                printf("[WARNING] Error cargando VFS (existe %s?)\n", VFS_FILE);
        }

        // =============================
        //  Bloque del Sistema
        // =============================
        else if (strcasecmp(cmd, "Salir") == 0)
        {
            printf("[INFO] Saliendo...\n");
            break; // Sale del bucle principal → finaliza el programa
        }
        else
        {
            printf("[WARNING] Comando desconocido. Escribe 'Ayuda'\n");
        }
    }

    return 0; // Fin del programa
}
