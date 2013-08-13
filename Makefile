CC      = gcc
COMMON  = -fPIC -D_POSIX_C_SOURCE=200803L -lpthread -rdynamic -g -ansi -std=c99 -Werror -pedantic
GFLAGS  = -pie
CFLAGS  = $(COMMON) $(GFLAGS) $(MAC)

# list final targets
PROG  = gpio display
TESTS = gpio-words i2c-test
OS    = io print pinmaps
TOOLS = $(addsuffix .o, $(addprefix tools/obj/, tokeniser))
OBJ   = $(addsuffix .o, $(addprefix obj/, $(OS)))

all: $(PROG) $(TESTS)

tests: $(TESTS)

clean:
	rm -f $(OBJ) $(addprefix bin/, $(PROG)) $(TOOLS)

gpio:  src/gpio.c $(OBJ)
	$(CC) $(CFLAGS) -o bin/$@ $^

display: src/display.c $(OBJ)
	$(CC) $(CFLAGS) -o bin/$@ $^

gpio-words: test/gpio_words.c $(OBJ)
	sudo $(CC) $(CFLAGS) $< -o /usr/local/$@

i2c-test: test/i2c_test.c $(OBJ) $(TOOLS)
	$(CC) $(CFLAGS) -o bin/$@ $^

obj/%.o: src/%.c src/%.h
	$(CC) $(CFLAGS) -c -o $@ $<

tools/obj/%.o: tools/src/%.c tools/src/%.h
	$(CC) $(CFLAGS) -c -o $@ $<

