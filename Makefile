CC = gcc

CFLAGS = -Wall -Wextra -std=c11

TARGET = UnixShell

SRC = src/main.c \
      src/command.c \
      src/memory.c \
      src/storage.c \
      src/process.c \
      src/report.c \
      src/vector.c \
      src/parser.c \
      src/signal.c
all:
	$(CC) $(CFLAGS) $(SRC) -o $(TARGET)

clean:
	rm -f $(TARGET)
