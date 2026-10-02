CC = gcc
CFLAGS = -Wall -Wextra

SRC = $(wildcard *.c)
OBJ = $(SRC:%.c=build/%.o)

.PHONY: all debug clean

all: ./bin/relf

./bin/relf: $(OBJ)
	@mkdir -p bin
	$(CC) $(CFLAGS) -o $@ $^

debug: CFLAGS += -g -O0
debug: ./bin/relf_debug

./bin/relf_debug: $(OBJ)
	@mkdir -p bin
	$(CC) $(CFLAGS) -o $@ $^

build/%.o: %.c
	@mkdir -p bin
	@mkdir -p build
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -rf $(OBJ) ./bin/relf ./bin/relf_debug
