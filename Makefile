CC = gcc
CFLAGS = -Wall -O2

# Usa pkgconf si pkg-config no existe
PKG ?= pkg-config

SRC_CORE  = src/process.c src/memory.c src/fs.c src/log.c
SRC_SHELL = src/shell.c

# CLI
SRC_CLI   = src/main_cli.c
CIN_CLI   = CinnamStrawbOS_cli.exe

# GUI (GTK3)
SRC_GUI   = src/ui_gtk.c
CIN_GUI   = CinnamStrawbOS_gui.exe
GTK_CFLAGS := $(shell $(PKG) --cflags gtk+-3.0)
GTK_LIBS   := $(shell $(PKG) --libs gtk+-3.0)

all: $(CIN_CLI) $(CIN_GUI)

$(CIN_CLI): $(SRC_CORE) $(SRC_SHELL) $(SRC_CLI)
	$(CC) $(CFLAGS) -o $@ $^

# Elige UNA de estas dos líneas:
# (1) con consola visible (útil para depurar)
#	$(CC) $(CFLAGS) $(GTK_CFLAGS) -o $@ $^ $(GTK_LIBS)
# (2) sin consola (app “solo GUI”)  ← recomendada
$(CIN_GUI): $(SRC_CORE) $(SRC_SHELL) $(SRC_GUI)
	$(CC) $(CFLAGS) -mwindows $(GTK_CFLAGS) -o $@ $^ $(GTK_LIBS)

clean:
	rm -f *.o *.exe

