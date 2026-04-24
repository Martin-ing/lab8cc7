CC = gcc
CFLAGS = -Wall -pthread

BIN_DIR = bin
TARGET = $(BIN_DIR)/scheduler

SRC = main.c algorithms.c queue.c utils.c

all: $(BIN_DIR) $(TARGET)

$(BIN_DIR):
	mkdir -p $(BIN_DIR)

$(TARGET): $(SRC)
	$(CC) $(SRC) -o $(TARGET) $(CFLAGS)

run: all
	./$(TARGET)

clean:
	rm -rf $(BIN_DIR)