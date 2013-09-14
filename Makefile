# Current gsl-1.15 location
MIRROR := http://www.mirrorservice.org/sites/ftp.gnu.org/gnu/gsl/gsl-
GVER   := 1.15

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
	ifeq($@, dsp)
		$(MAKE) gsl
	endif
	@-$(MAKE) -C ./build/$@

# General Scientific Library (GSL)

gsl: gsl-
	@-echo "Removing previous gsl libraries..."
	@-rm -rf gsl-*/* && rm -f gsl-*.*.*
	@-echo "Downloading gsl..."
	@-wget --progress=bar:force $(MIRROR)$(GVER).tar.gz 2>&1 | tail -f -n +6
	@-echo "Untarring gsl..."
	@-tar xzf gsl-$(GVER).tar.gz 2>&1
	@-echo "Removing gsl-$(GVER) tar..."
	@-rm gsl-$(GVER).tar.gz*

gsl-:
	@-mkdir $@$(GVER)
