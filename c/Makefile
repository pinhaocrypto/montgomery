CC = gcc
CFLAGS = -Wall -Wextra -O2
TARGET = montgomery_reduce
SRC = montgomery_reduce.c
OBJ = $(SRC:.c=.o)

all: clean $(TARGET) run

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

$(TARGET): $(OBJ)
	$(CC) $(CFLAGS) -o $@ $^

run: $(TARGET)
	./$(TARGET)

clean:
	rm -f $(OBJ) $(TARGET)

.PHONY: all clean run