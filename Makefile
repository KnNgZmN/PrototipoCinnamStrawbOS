CC = gcc
CFLAGS = -std=c99 -Wall -O2 -fexec-charset=UTF-8 -finput-charset=UTF-8
SRC = src/main.c src/process.c src/memory.c src/fs.c
OBJ = $(SRC:.c=.o)
TARGET = CinnamStrawbOS

all: $(TARGET)

$(TARGET): $(SRC)
	$(CC) $(CFLAGS) -o $(TARGET) $(SRC)

clean:
	rm -f $(TARGET) $(OBJ)
