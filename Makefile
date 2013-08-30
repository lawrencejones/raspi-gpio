PROG = i2c gpio imu

all: $(PROG)

rebuild:
	@-$(MAKE) clean
	@-clear
	@-$(MAKE) all

clean:
	@-$(MAKE) -C ./build/i2c clean
	@-$(MAKE) -C ./build/gpio clean
	@-$(MAKE) -C ./build/imu clean

$(PROG):
	@-$(MAKE) -C ./build/$@
