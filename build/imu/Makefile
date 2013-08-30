BUILDROOT := ../..

APPNAME := imu

APP_SRC := imu.c

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

MPU := \
	mpu_conf.c \
	mpu_conf_fifo.c \
	mpu_conf_gyro.c \
	mpu_conf_i2c_mst.c \
	mpu_conf_i2c_slv.c \
	mpu_conf_int.c \
	mpu_conf_power.c \
	mpu_conf_samplerate.c \
	mpu_conf_user_ctrl.c \
	mpu_dealloc.c \
	mpu_helpers.c \
	mpu_init.c \
	mpu_read.c

MPU := $(addprefix devs/mpu3300/, $(MPU))

PCA := \
	pca_conf.c \
	pca_dealloc.c \
	pca_init.c \
	pca_muxnet.c

PCA := $(addprefix devs/pca9548a/, $(PCA))

MODULES := \
  imu/imu_mux_test.c \
  gpio/gpio_init.c \
	shared/io.c \
  ../tools/src/tokeniser.c \
	$(I2C) $(PCA) $(MPU)

include $(BUILDROOT)/build/App.Makefile
