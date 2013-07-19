CC      = gcc
COMMON  = -fPIC -D_POSIX_C_SOURCE=200803L -lpthread -rdynamic -g -std=c99 -Werror -pedantic
GFLAGS  = -pie
CFLAGS  = $(COMMON) $(GFLAGS) $(MAC)

# list final targets
PROG  = gpio display
TESTS = gpio-words i2c-test
OS    = io print pinmaps
OBJ   = $(addsuffix .o, $(addprefix obj/, $(OS)))
TOOLS = $(addsuffix .o, $(addprefix tools/obj/, tokeniser))

all: $(PROG) $(TESTS)

tests: $(TESTS)

clean:
	rm $(OBJ)
	rm $(addprefix bin/, $(PROG))

gpio:  src/gpio.c $(OBJ) $(TOOLS)
	$(CC) $(CFLAGS) -o bin/$@ src/gpio.c $(OBJ)

display: src/display.c $(OBJ) $(TOOLS)
	$(CC) $(CFLAGS) -o bin/$@ src/display.c $(OBJ) 

gpio-words: test/gpio_words.c $(OBJ) $(TOOLS)
	sudo $(CC) $(CFLAGS) $< -o /usr/local/gpio_words

i2c-test: test/i2c_test.c $(OBJ) $(TOOLS)
	$(CC) $(CFLAGS) -o bin/$@ $^

obj/%.o: src/%.c src/%.h
	$(CC) $(CFLAGS) -c -o $@ $<

tools/obj/%.o: tools/src/%.c tools/src/%.h
	$(CC) $(CFLAGS) -c -o $@ $<

