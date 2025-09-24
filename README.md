# 📘 Prototipo de Sistema Operativo **CinnamStrawbOS**  

✨ Versión **0.1 - 2025**  
👩‍💻 **Autores:** Maria Alejandra Toro Ortiz & Kevin Guzman Acevedo  

---

## 📖 Descripción  

Este proyecto es un **prototipo académico de un sistema operativo básico**, desarrollado en **C** como parte del **Taller de Sistemas Operativos**.  

El objetivo es **simular y comprender los conceptos fundamentales** de un sistema operativo real, integrando:  

- ⚡ **Gestión de Procesos** → creación, listado, ejecución y finalización.  
- 💾 **Gestión de Memoria** → asignación, liberación y visualización de bloques.  
- 📂 **Sistema de Archivos Virtual (VFS)** → crear, listar, escribir, leer, eliminar, guardar y cargar archivos.  
- 🖥️ **Interfaz de Comandos** → shell interactiva con soporte de ayuda.  

Este prototipo está diseñado con un enfoque **pedagógico y modular**, para facilitar la comprensión y la experimentación.  

---

## ⚙️ Características principales  

### 🔹 Gestión de Procesos  
- Crear nuevos procesos con ráfaga de CPU.  
- Listar procesos activos.  
- Ejecutar el planificador **Round-Robin** con quantum configurable.  
- Terminar procesos específicos por ID.  

### 🔹 Gestión de Memoria  
- Asignar bloques de memoria a procesos.  
- Liberar memoria ocupada por procesos.  
- Visualizar el mapa de memoria.  

### 🔹 Sistema de Archivos Virtual (VFS)  
- Crear y eliminar archivos.  
- Escribir y mostrar contenido en archivos.  
- Listar archivos disponibles.  
- Guardar y cargar el estado del sistema de archivos en **vfs.dat**.  

### 🔹 Interfaz  
- Interacción mediante **consola de comandos**.  
- Menú de ayuda **elegante y categorizado**.  
- Comandos **insensibles a mayúsculas/minúsculas**.  

---

## 🖥️ Requisitos  

- 🔧 **Compilador C** (ejemplo: `gcc` de MinGW o Linux GCC).  
- 💻 **Windows PowerShell / CMD** o terminal en **Linux/MacOS**.  

---

## 🚀 Instalación y Ejecución  

1. **Clonar o descargar** el repositorio:  
   ```bash
      git clone <https://github.com/KnNgZmN/PrototipoCinnamStrawbOS.git>
      cd cinnamstrawbos


Compilar el proyecto:
mingw32-make
Esto generará el ejecutable CinnamStrawbOS.exe (Windows) o ./CinnamStrawbOS (Linux/Mac).

Ejecutar el sistema:
./CinnamStrawbOS.exe

⌨️ Comandos Disponibles

🧑‍💻 Procesos

NuevoProceso <nombre> <rafaga> → Crear proceso (ráfaga en unidades).

ListarProcesos → Listar procesos activos.

Ejecutar [quantum] → Ejecutar planificador Round-Robin.

TerminarProceso <id_proceso> → Terminar un proceso específico.

💾 Memoria

AsignarMemoria <id_proceso> <tamano_bytes> → Asignar memoria al proceso.

LiberarMemoria <id_proceso> → Liberar memoria del proceso.

MostrarMapaMemoria → Mostrar mapa de memoria.

📂 Archivos (VFS)

CrearArchivo <nombre> → Crear archivo en el sistema.

ListarArchivos → Listar archivos del VFS.

MostrarContenido <nombre> → Mostrar contenido de un archivo.

EscribirArchivo <nombre> → Escribir contenido en un archivo.

EliminarArchivo <nombre> → Eliminar archivo del VFS.

GuardarFS → Guardar el estado del VFS en disco (vfs.dat).

CargarFS → Cargar el VFS desde disco (vfs.dat).

⚙️ Sistema

Ayuda → Mostrar menú de ayuda.

Salir → Cerrar el sistema.

prototipo-so/
│── src/                 # Código fuente en C
│   ├── main.c           # Interfaz de comandos
│   ├── process.c/.h     # Gestión de procesos
│   ├── memory.c/.h      # Gestión de memoria
│   ├── fs.c/.h          # Sistema de archivos virtual
│
│── vfs.dat              # Archivo de persistencia del VFS
│── Makefile             # Script de compilación
│── README.md            # Documentación del proyecto

🎯 Objetivos Académicos

✔️ Comprender el ciclo de vida de un proceso en un sistema operativo.
✔️ Simular la planificación de procesos mediante algoritmos básicos.
✔️ Explorar estrategias de administración de memoria.
✔️ Modelar un sistema de archivos simplificado.
✔️ Familiarizarse con el diseño modular en C.


📌 Notas

📎 Este proyecto es didáctico, no un sistema operativo real.
✍️ Los comandos y parámetros están en español para mayor claridad.
