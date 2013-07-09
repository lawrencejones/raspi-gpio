CC      = gcc
CFLAGS  = -fPIC -g -std=c99 -Werror -pedantic

# list final targets
PROG=gpio

all: $(PROG)

clean:
	rm $(OBJS)
	rm $(BINS)

gpio: src/inputs.c src/inputs.h
	$(CC) $(CFLAGS) $< -o bin/$@
