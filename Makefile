CC      = gcc
COMMON  = -fPIC -D_POSIX_C_SOURCE=200803L -rdynamic -g -std=c99 -Werror -pedantic
GFLAGS  = -pie
CFLAGS  = $(COMMON) $(GFLAGS) $(MAC)

# Final binaries
PROG  = gpio i2c
# Module categories
THIS = $(addprefix obj/, io pinmaps print)
SUBS = $(addprefix tools/obj/, tokeniser)
MODS = $(addsuffix .o, $(THIS) $(SUBS))
# Driver binaries
DRIV = $(addsuffix .o, $(addprefix i2c_devs/, pca9548a))

all: $(addprefix bin/, $(PROG))

clean:
	rm -rf $(MODS) $(addprefix bin/, $(addsuffix *, $(PROG)))

bin/%: src/%.c $(MODS)
	$(CC) $(CFLAGS) -o $@ $^

bin/board: i2c_devs/board.c $(DRIV) $(MODS)
	$(CC) $(CFLAGS) -o $@ $^

tools/obj/%.o:
	cd tools && make $@
	cd ..

obj/%.o: src/%.c src/%.h
	$(CC) $(CFLAGS) -c -o $@ $<

%:
	make bin/$@
