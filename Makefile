CC      = gcc
COMMON  = -fPIC -rdynamic -g -std=c99 -Werror -pedantic
GFLAGS  = -pie
CFLAGS  = $(COMMON) $(GFLAGS) $(MAC)

# list final targets
PROG  = gpio display
TESTS = gpio-words
OBJ   = $(addprefix 'obj/', $(io))

all: $(PROG)

tests: $(TESTS)

clean:
	rm $(OBJS)
	rm $(BINS)

gpio: obj/io.o src/gpio.c $(OBJ)
	$(CC) $(CFLAGS) $(OBJ) -o bin/$@ src/gpio.c

display: obj/io.o src/display.c src/display.h
	$(CC) $(CFLAGS) $< -o bin/$@

gpio-words: test/gpio-words.c
	sudo $(CC) $(CFLAGS) $< -o /usr/local/gpio-words

obj/%.o: src/%.c src/%.h
	$(CC) $(CFLAGS) -c -o $@ $<
