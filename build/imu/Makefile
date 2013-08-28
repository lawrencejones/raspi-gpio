BUILDROOT := ../..

APPNAME := imu

APP_SRC := imu.c

MODULES := \
  imu/imu_mux_test.c \
  i2c/i2c_dealloc.c \
  i2c/i2c_dev_mgmt.c \
  i2c/i2c_init.c \
  i2c/i2c_malloc.c \
  i2c/i2c_read.c \
  i2c/i2c_system.c \
  i2c/i2c_write.c \
  i2c/i2c_display.c \
  gpio/gpio_init.c \
  devs/mpu3300.c \
  devs/pca9548a.c \
  ../tools/src/tokeniser.c \
	shared/io.c

include $(BUILDROOT)/build/App.Makefile
