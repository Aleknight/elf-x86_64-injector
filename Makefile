CC=gcc

CFLAGS= -I./inc
LDFLAGS=

SOURCES = $(wildcard src/*.c)
HEADERS = $(wildcard inc/*.h)
OBJECTS = $(patsubst src/%.c,obj/%.o,$(SOURCES))

all: infector

infector: $(OBJECTS)
	$(CC) -o $@ $^ $(LDFLAGS)

obj/%.o: src/%.c
	$(CC) -c $< -o $@ $(CFLAGS)

clean:
	rm -f obj/*.o infector
