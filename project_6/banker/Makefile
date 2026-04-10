CC = gcc
CFLAGS = -Wall -Wextra -std=c11
TARGET = banker
SRC = banker.c

all: $(TARGET)

$(TARGET): $(SRC)
	$(CC) $(CFLAGS) -o $(TARGET) $(SRC)

clean:
	del /Q $(TARGET).exe 2>NUL || exit 0
