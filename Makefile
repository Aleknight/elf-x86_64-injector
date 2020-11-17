CC=gcc

CFLAGS=
LDFLAGS=

HEADERS = $(wildcard inc/*.h)

all: infector

infector: obj/%.o
	$(CC) -o $@ $^ $(LDFLAGS)

obj/%.o: src/%.c
