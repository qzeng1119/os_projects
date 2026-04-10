CC = gcc
CFLAGS = -Wall -Wextra -std=c11
TARGET = banker
SRC = banker.c

all: $(TARGET)

$(TARGET): $(SRC)
	$(CC) $(CFLAGS) -o $(TARGET) $(SRC)

.PHONY: clean
clean:
	rm -f $(TARGET)
