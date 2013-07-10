CC      = gcc
COMMON  = -fPIC -rdynamic -g -std=c99 -Werror -pedantic 
GFLAGS  = -pie
CFLAGS  = $(COMMON) $(GFLAGS) $(MAC)

# list final targets
PROG=gpio
TESTS=gpio-words

all: $(PROG)

tests: $(TESTS)

clean:
	rm $(OBJS)
	rm $(BINS)

gpio: src/inputs.c src/inputs.h
	$(CC) $(CFLAGS) $< -o bin/$@

gpio-words: test/gpio-words.c
	sudo $(CC) $(CFLAGS) $< -o /usr/local/gpio-words
