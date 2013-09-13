BUILDROOT := ../..

APPNAME := i2c

APP_SRC := i2c.c

I2C := \
  i2c_dealloc.c \
  i2c_dev_mgmt.c \
  i2c_init.c \
  i2c_malloc.c \
  i2c_read.c \
  i2c_system.c \
  i2c_write.c \
  i2c_display.c

I2C := $(addprefix i2c/, $(I2C))

I2C_CLI := \
  i2c_cli_display.c \
  i2c_cli_helpers.c \
  i2c_cli_interpreter.c \
  i2c_cli_run.c 

I2C_CLI := $(addprefix i2c_cli/, $(I2C_CLI))

OTHERS := \
  gpio/gpio_init.c \
  ../tools/src/tokeniser.c \
	shared/io.c

MODULES := $(I2C) $(I2C_CLI) $(OTHERS)

include $(BUILDROOT)/build/App.Makefile
