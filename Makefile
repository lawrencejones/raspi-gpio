CC      := gcc
COMMON  := -fPIC -D_POSIX_C_SOURCE:=200803L -rdynamic -g -std:=c99 -Werror -pedantic
GFLAGS  := -pie
CFLAGS  := $(COMMON) $(GFLAGS) $(MAC)

# Final binaries
PROG := gpio i2c board

MODS := $(addprefix obj/, io.o i2c.o gpio.o)
ISRC := $(addprefix src/i2c/i2c_, io res)
GSRC := $(addprefix src/gpio/gpio_, io.c io.h res.h)

all: $(PROG)

clean:
	find . -name "*.o" -exec rm -f {} \;
	rm -rf bin/*

obj/%.o: 

# Make for any .c file in the src directory
src/**/%.c:
	$(CC) $(CFLAGS) -c -o obj/$*.o $@
