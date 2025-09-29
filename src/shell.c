#include <stdio.h>     // Entrada/Salida estándar (printf, fgets, etc.)
#include <string.h>    // Manejo de cadenas (strcmp, strtok, strlen, etc.)
#include <stdlib.h>    // Funciones estándar (atoi, malloc, free, etc.)
#include <locale.h>    // Configuración regional (para soporte de UTF-8 en consola)

#include "shell.h"
#include "log.h"
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

void print_ayuda(void)
{
    Mostrar("\n");
    Mostrar("╔══════════════════════════════════════════════════════════════╗\n");
    Mostrar("  📚  CinnamStrawbOS - Manual de Comandos\n");
    Mostrar("╚══════════════════════════════════════════════════════════════╝\n\n");

    // 🧑‍💻 Procesos
    Mostrar("📌  Gestion de Procesos\n");
    Mostrar("──────────────────────────────────────────────────────────────\n");
    Mostrar("  🔹 NuevoProceso <Nombre> <Rafaga>   → Crear proceso (rafaga en unidades)\n");
    Mostrar("  🔹 ListarProcesos                   → Listar procesos activos\n");
    Mostrar("  🔹 Ejecutar [Intervalo]             → Ejecutar planificador Round-Robin\n");
    Mostrar("  🔹 TerminarProceso <Id_Proceso>     → Terminar un proceso especifico\n\n");

    // 💾 Memoria
    Mostrar("📌  Gestion de Memoria\n");
    Mostrar("──────────────────────────────────────────────────────────────\n");
    Mostrar("  🔹 AsignarMemoria <Id_Proceso> <Tamano_Bytes> → Asignar memoria al proceso\n");
    Mostrar("  🔹 LiberarMemoria <Id_Proceso>                → Liberar memoria del proceso\n");
    Mostrar("  🔹 MostrarMapaMemoria                         → Mostrar mapa de memoria\n\n");

    // 📂 Archivos
    Mostrar("📌  Sistema de Archivos Virtual (VFS)\n");
    Mostrar("──────────────────────────────────────────────────────────────\n");
    Mostrar("  🔹 CrearArchivo <Nombre>        → Crear archivo en VFS\n");
    Mostrar("  🔹 ListarArchivos               → Listar archivos en VFS\n");
    Mostrar("  🔹 MostrarContenido <Nombre>    → Mostrar contenido de un archivo\n");
    Mostrar("  🔹 EscribirArchivo <Nombre>     → Escribir contenido en un archivo\n");
    Mostrar("  🔹 EliminarArchivo <Nombre>     → Eliminar archivo del VFS\n");
    Mostrar("  🔹 GuardarFS                    → Guardar VFS en disco (vfs.dat)\n");
    Mostrar("  🔹 CargarFS                     → Cargar VFS desde disco (vfs.dat)\n\n");

    // ⚙️ Sistema
    Mostrar("📌  Comandos del Sistema\n");
    Mostrar("──────────────────────────────────────────────────────────────\n");
    Mostrar("  🔹 Ayuda       → Mostrar este menú de ayuda\n");
    Mostrar("  🔹 Salir       → Salir del sistema\n\n");

    Mostrar("💡 Tip: Usa los botones superiores para acceso rápido\n");
    Mostrar("╔══════════════════════════════════════════════════════════════╗\n");
    Mostrar("  ✅ Listo! Ahora prueba algún comando para comenzar\n");
    Mostrar("╚══════════════════════════════════════════════════════════════╝\n\n");
}

// Inicializa todos los subsistemas del SO y muestra banner de bienvenida
void shell_init(void)
{   

    // Inicialización de subsistemas
    proc_init();            // Inicializa gestor de procesos
    mem_init();             // Inicializa gestor de memoria
    fs_init();              // Inicializa sistema de archivos
    fs_load(VFS_FILE);      // Intenta cargar sistema de archivos desde disco

    setlocale(LC_ALL, "");  // Habilita soporte UTF-8 (dependiendo del SO)

    Mostrar("\n");
    Mostrar("╔══════════════════════════════════════════════════════════════╗\n");
    Mostrar("║                                                              ║\n");
    Mostrar("║   🚀  Prototipo Sistema Operativo \"CinnamStrawbOS\"  🚀     ║\n");
    Mostrar("║                                                              ║\n");
    Mostrar("╠══════════════════════════════════════════════════════════════╣\n");
    Mostrar("║                       Versión 0.1 - 2025                     ║\n");
    Mostrar("╟──────────────────────────────────────────────────────────────╢\n");
    Mostrar("║   Desarrollado por:                                          ║\n");
    Mostrar("║               👩‍💻  Maria Alejandra Toro Ortiz                         ║\n");
    Mostrar("║               👨‍💻  Kevin Steven Guzman Acevedo                        ║\n");
    Mostrar("╟──────────────────────────────────────────────────────────────╢\n");
    Mostrar("║   💡 Escribe 'Ayuda' para ver la lista de comandos          ║\n");
    Mostrar("║      disponibles.                                            ║\n");
    Mostrar("╚══════════════════════════════════════════════════════════════╝\n\n");
}

// Procesa una línea de comando completa y ejecuta la acción correspondiente
int shell_handle_line(char *line)
{
    if (!line) return 0;
    line[strcspn(line, "\n")] = '\0';  // Eliminar salto de línea
    if (!*line) return 0;  // Ignorar líneas vacías

    char *cmd = strtok(line, " ");  // Extraer comando principal
    if (!cmd) return 0;

    // =============================
    //  Bloque de Procesos
    // =============================

    if (strcasecmp(cmd, "Ayuda") == 0) {
        print_ayuda();
    }
    else if (strcasecmp(cmd, "NuevoProceso") == 0) {
        char *name = strtok(NULL, " ");
        char *burst_s = strtok(NULL, " ");
        if (!name || !burst_s) { Mostrar("Uso: NuevoProceso <name> <burst>\n"); return 0; }
        int burst = atoi(burst_s); // Convierte argumento a número
            proc_create(name, burst);  // Crea nuevo proceso
    }
    else if (strcasecmp(cmd, "ListarProcesos") == 0) {
        proc_list();
    }
    else if (strcasecmp(cmd, "Ejecutar") == 0) {
        char *q = strtok(NULL, " ");
        int quantum = q ? atoi(q) : 1;   // Quantum por defecto = 1
        proc_scheduler_rr(quantum);      // Ejecuta planificador RR
    }
    else if (strcasecmp(cmd, "TerminarProceso") == 0) {
        char *pid_s = strtok(NULL, " ");
        if (!pid_s) { Mostrar("Uso: TerminarProceso <pid>\n"); return 0; }
        int pid = atoi(pid_s);
        proc_kill(pid); // Elimina proceso específico
    }

    // =============================
    //  Bloque de Memoria
    // =============================

    else if (strcasecmp(cmd, "AsignarMemoria") == 0) {
        char *pid_s = strtok(NULL, " ");
        char *size_s = strtok(NULL, " ");
        if (!pid_s || !size_s) { Mostrar("Uso: AsignarMemoria <pid> <size>\n"); return 0; }
        int pid = atoi(pid_s), size = atoi(size_s);
        int blk = mem_alloc(pid, size);
        if (blk == -1) Mostrar("[ERROR] Fallo la asignacion de memoria (no hay fit o limite)\n");
        else Mostrar("[OK] Memoria asignada (block idx=%d) para PID=%d\n", blk, pid);
    }
    else if (strcasecmp(cmd, "LiberarMemoria") == 0) {
        char *pid_s = strtok(NULL, " ");
        if (!pid_s) { Mostrar("Uso: LiberarMemoria <pid>\n"); return 0; }
        int pid = atoi(pid_s);
        int freed = mem_free_by_owner(pid); // Libera memoria del proceso
        if (freed == 0) Mostrar("[WARNING] No se encontraron bloques para PID=%d\n", pid);
        else Mostrar("[INFO] Liberados %d bloque(s) para PID=%d\n", freed, pid);
    }
    else if (strcasecmp(cmd, "MostrarMapaMemoria") == 0) {
        mem_map(); // Muestra estado de la memoria
    }

     // =============================
    //  Bloque de Archivos (VFS)
    // =============================
    
    else if (strcasecmp(cmd, "CrearArchivo") == 0) {
        char *name = strtok(NULL, " ");
        if (!name) { Mostrar("Uso: CrearArchivo <name>\n"); return 0; }
        if (fs_mkfile(name) == -1) Mostrar("[WARNING] No se pudo crear (ya existe o espacio lleno)\n");
        else Mostrar("[OK] Archivo creado: %s\n", name);
    }
    else if (strcasecmp(cmd, "ListarArchivos") == 0) {
        fs_ls();
    }
    else if (strcasecmp(cmd, "MostrarContenido") == 0) {
        char *name = strtok(NULL, " ");
        if (!name) { Mostrar("Uso: MostrarContenido <name>\n"); return 0; }
        char buf[MAX_CONTENT];
        if (fs_read(name, buf, sizeof(buf)) == -1) Mostrar("[WARNING] Archivo no encontrado\n");
        else Mostrar("Contenido de %s:\n%s\n", name, buf);
    }
    else if (strcasecmp(cmd, "EscribirArchivo") == 0) {
        char *name = strtok(NULL, " ");
        char *rest = strtok(NULL, "");   // toma TODO lo que sigue como contenido
        if (!name) { Mostrar("Uso: EscribirArchivo <name> [contenido]\n"); return 0; }

        if (fs_find(name) == -1) { // Si no existe, crearlo
            if (fs_mkfile(name) == -1) { Mostrar("[WARNING] No se pudo crear archivo\n"); return 0; }
        }
        if (!rest) { Mostrar("[INFO] Escriba el contenido en la misma linea. Ej:\n");
                     Mostrar("       EscribirArchivo notas Hola mundo\n"); return 0; }

        fs_write(name, rest);
        Mostrar("[OK] Contenido escrito en %s\n", name);
    }
    else if (strcasecmp(cmd, "EliminarArchivo") == 0) {
        char *name = strtok(NULL, " ");
        if (!name) { Mostrar("Uso: EliminarArchivo <name>\n"); return 0; }
        if (fs_rmfile(name) == -1) Mostrar("[WARNING] Archivo no encontrado\n");
        else Mostrar("[OK] Archivo eliminado: %s\n", name);
    }
    else if (strcasecmp(cmd, "GuardarFS") == 0) {
        if (fs_save(VFS_FILE) == 0) Mostrar("[OK] VFS guardado en %s\n", VFS_FILE);
        else Mostrar("[ERROR] Error guardando VFS\n");
    }
    else if (strcasecmp(cmd, "CargarFS") == 0) {
        if (fs_load(VFS_FILE) == 0) Mostrar("[OK] VFS cargado desde %s\n", VFS_FILE);
        else Mostrar("[WARNING] Error cargando VFS (existe %s?)\n", VFS_FILE);
    }

    // ===== Sistema =====
    else if (strcasecmp(cmd, "Salir") == 0) {
        Mostrar("[INFO] Saliendo...\n");
        return 1;
    }
    else {
        Mostrar("[WARNING] Comando desconocido. Escribe 'Ayuda'\n");
    }
    return 0;
}
