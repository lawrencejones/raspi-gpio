# Current gsl-1.15 location
MIRROR := http://www.mirrorservice.org/sites/ftp.gnu.org/gnu/gsl/gsl-
GVER   := 1.15

PROG = i2c gpio imu

all: $(PROG)

rebuild: clean
	@-clear
	@-$(MAKE) all

clean:
	@-$(MAKE) -C ./build/i2c clean
	@-$(MAKE) -C ./build/gpio clean
	@-$(MAKE) -C ./build/imu clean

$(PROG):
	@-$(MAKE) -C ./build/$@

# General Scientific Library (GSL)
gsl:
	@-echo "Removing previous gsl libraries..."
	@-rm -rf gsl-*/* && rm -f gsl-*.*.*
	@-echo "Downloading gsl..."
	@-wget --progress=bar:force $(MIRROR)$(GVER).tar.gz 2>&1 | tail -f -n +6
	@-echo "Untarring gsl..."
	@-tar xzf gsl-$(GVER).tar.gz 2>&1
	@-echo "Removing gsl-$(GVER) tar..."
	@-rm gsl-$(GVER).tar.gz*
	@-echo "Making the gsl library..."
	@-echo "  Configuring gsl..."
	@-cd gsl-$(GVER) && ./configure > /dev/null
	@-echo "  Compiling gsl shared library..."
	@-cd gsl-$(GVER) && make > /dev/null
	@-echo "Done!"

