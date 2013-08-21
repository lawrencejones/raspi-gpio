CC      = gcc
COMMON  = -fPIC -D_POSIX_C_SOURCE=200803L -lpthread -rdynamic -g -ansi -std=c99 -Werror -pedantic
GFLAGS  = -pie
CFLAGS  = $(COMMON) $(GFLAGS) $(MAC)

# list final targets
PROG  = gpio display i2c
TESTS = gpio-words
OS    = io print pinmaps
TOOLS = $(addsuffix .o, $(addprefix tools/obj/, tokeniser))
OBJ   = $(addsuffix .o, $(addprefix obj/, $(OS)))

all: $(PROG)

tests: $(TESTS)

clean:
	rm -f $(OBJ) $(addprefix bin/, $(PROG)) $(TOOLS)

gpio:  src/gpio.c $(OBJ)
	$(CC) $(CFLAGS) -o bin/$@ $^

display: src/display.c $(OBJ)
	$(CC) $(CFLAGS) -o bin/$@ $^

gpio-words: test/gpio_words.c $(OBJ)
	sudo $(CC) $(CFLAGS) $< -o /usr/local/$@

i2c: src/i2c.c $(OBJ) $(TOOLS)
	$(CC) $(CFLAGS) -o bin/$@ $^

obj/%.o: src/%.c src/%.h
	$(CC) $(CFLAGS) -c -o $@ $<

tools/obj/%.o: tools/src/%.c tools/src/%.h
	$(CC) $(CFLAGS) -c -o $@ $<

