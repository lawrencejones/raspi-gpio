BUILDROOT := ../..

APPNAME := i2c

APP_SRC := i2c.c

MODULES := \
  i2c/i2c_dealloc.c \
  i2c/i2c_dev_mgmt.c \
  i2c/i2c_init.c \
  i2c/i2c_malloc.c \
  i2c/i2c_read.c \
  i2c/i2c_system.c \
  i2c/i2c_write.c \
  i2c/i2c_display.c \
  i2c_cli/i2c_cli_display.c \
  i2c_cli/i2c_cli_helpers.c \
  i2c_cli/i2c_cli_interpreter.c \
  i2c_cli/i2c_cli_run.c \
  gpio/gpio_init.c \
  ../tools/src/tokeniser.c \
	shared/io.c

include $(BUILDROOT)/build/App.Makefile
