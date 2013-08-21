CC      = gcc
COMMON  = -fPIC -D_POSIX_C_SOURCE=200803L -lpthread -rdynamic -g -std=c99 -Werror -pedantic
GFLAGS  = -pie
CFLAGS  = $(COMMON) $(GFLAGS) $(MAC)

# Final binaries
PROG  = gpio i2c
# Module categories
THIS = $(addprefix obj/, io pinmaps print)
SUBS = $(addprefix tools/obj/, tokeniser)
MODS = $(addsuffix .o, $(THIS) $(SUBS))

all: $(addprefix bin/, $(PROG))

clean:
	rm -rf $(MODS) $(addprefix bin/, $(addsuffix *, $(PROG)))

bin/%: src/%.c $(MODS)
	$(CC) $(CFLAGS) -o $@ $^

tools/obj/%.o:
	cd tools && make $@
	cd ..

obj/%.o: src/%.c src/%.h
	$(CC) $(CFLAGS) -c -o $@ $<
