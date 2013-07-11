CC      = gcc
COMMON  = -fPIC -rdynamic -g -ansi -std=c99 -Werror -pedantic
GFLAGS  = -pie
CFLAGS  = $(COMMON) $(GFLAGS) $(MAC)

# list final targets
PROG  = gpio display
TESTS = gpio-words
OS    = io print pinmaps
OBJ   = $(addsuffix .o, $(addprefix obj/, $(OS)))

all: $(PROG)

tests: $(TESTS)

clean:
	rm $(OBJ)
	rm $(addprefix bin/, $(PROG))

gpio:  src/gpio.c $(OBJ)
	$(CC) $(CFLAGS) -o bin/$@ src/gpio.c $(OBJ)

display: src/display.c $(OBJ)
	$(CC) $(CFLAGS) -o bin/$@ src/display.c $(OBJ) 

gpio-words: test/gpio-words.c
	sudo $(CC) $(CFLAGS) $< -o /usr/local/gpio-words

obj/%.o: src/%.c src/%.h
	$(CC) $(CFLAGS) -c -o $@ $<
