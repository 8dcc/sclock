
CC=gcc
CFLAGS=-Wall -Wextra -Wpedantic
LDFLAGS=$(shell sdl2-config --cflags --libs)

OBJ_FILES=main.c.o
OBJS=$(addprefix obj/, $(OBJ_FILES))

INSTALL_DIR=/usr/local/bin
BIN=sclock

#-------------------------------------------------------------------------------

.PHONY: clean all

all: $(BIN)

clean:
	rm -f $(OBJS)
	rm -f $(BIN)

install: $(BIN)
	mkdir -p $(INSTALL_DIR)
	install -m 755 $(BIN) $(INSTALL_DIR)/$(BIN)

#-------------------------------------------------------------------------------

$(BIN): $(OBJS)
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

obj/%.c.o : src/%.c
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c -o $@ $<
