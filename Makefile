
CC=gcc
CFLAGS=-Wall -Wextra -Wpedantic
LDFLAGS=$(shell sdl2-config --cflags --libs)

# TODO: Add object files and rename
OBJ_FILES=main.c.o
OBJS=$(addprefix obj/, $(OBJ_FILES))

BIN=output.out

#-------------------------------------------------------------------------------

.PHONY: clean all

all: $(BIN)

clean:
	rm -f $(OBJS)
	rm -f $(BIN)

#-------------------------------------------------------------------------------

$(BIN): $(OBJS)
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

obj/%.c.o : src/%.c
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c -o $@ $<
