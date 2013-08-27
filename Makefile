PROG = i2c gpio board

all: $(PROG)

rebuild:
	$(MAKE) clean
	$(MAKE) all

clean:
	$(MAKE) -C ./build/i2c clean
	$(MAKE) -C ./build/gpio clean
	$(MAKE) -C ./build/board clean

%:
	$(MAKE) -C ./build/$@
