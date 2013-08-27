CC      = gcc
COMMON  = -fPIC -D_POSIX_C_SOURCE=200803L -rdynamic -g -std=c99 -Werror -pedantic
GFLAGS  = -pie
CFLAGS  = $(COMMON) $(GFLAGS) $(MAC)

# Final binaries
PROG = gpio i2c board
ISRC = src/i2c.c $(addprefix src/i2c/i2c_, io.h io.c res.h)
GSRC = src/gpio.c $(addprefix src/gpio/gpio_, io.h io.c res.h)

all: $(PROG)

clean:
	find . -name "*.o" -exec rm -f {} \;
	rm -rf bin/*

i2c: $(ISRC)
  $(CC) $(CFLAGS) -o bin/i2c $<

gpio: $(GSRC)
	$(CC) $(CFLAGS) -o bin/gpio $<

