CC      = gcc
COMMON  = -fPIC -D_POSIX_C_SOURCE=200803L -lpthread -rdynamic -g -ansi -std=c99 -Werror -pedantic
GFLAGS  = -pie
CFLAGS  = $(COMMON) $(GFLAGS) $(MAC)

# list final targets
PROG  = gpio display
TESTS = gpio-words i2c-test
OS    = io print pinmaps
OBJ   = $(addsuffix .o, $(addprefix obj/, $(OS)))

all: $(PROG) $(TESTS)

tests: $(TESTS)

clean:
	rm $(OBJ)
	rm $(addprefix bin/, $(PROG))

gpio:  src/gpio.c $(OBJ)
	$(CC) $(CFLAGS) -o bin/$@ src/gpio.c $(OBJ)

display: src/display.c $(OBJ)
	$(CC) $(CFLAGS) -o bin/$@ src/display.c $(OBJ) 

gpio-words: test/gpio_words.c $(OBJ)
	sudo $(CC) $(CFLAGS) $< -o /usr/local/gpio_words

i2c-test: test/i2c_test.c $(OBJ)
	$(CC) $(CFLAGS) -o bin/$@ $< $(OBJ)

obj/%.o: src/%.c src/%.h
	$(CC) $(CFLAGS) -c -o $@ $<
