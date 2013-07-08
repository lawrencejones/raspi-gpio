CC      = gcc
CFLAGS  = -fPIC -pie -g -std=c99 -Werror -pedantic

# list final targets
PROG=gpio
TESTS=readmem

all: $(PROG)

tests: $(TESTS)

clean:
	rm $(OBJS)
	rm $(BINS)

gpio: src/inputs.c src/inputs.h
	$(CC) $(CFLAGS) $< -o bin/$@

readmem: test/readmem.c
	$(CC) $(CFLAGS) $< -o ~/readmem
