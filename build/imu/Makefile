BUILDROOT := ../..

APPNAME := imu

APP_SRC := imu.c

ifeq ($(shell arch), armv6l)
LDFLAGS += -lm
endif

# Required modules from the gpio folder
GPIO := \
	gpio_init.c

GPIO := $(addprefix gpio/, $(GPIO))

# Required modules from the i2c folder
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

# Required shared devs modules
DEV := \
  dev_config.c \
  dev_axes.c

DEV := $(addprefix devs/shared/, $(DEV))

# Required modules from the mpu folder
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
  mpu_selftest.c \
	mpu_helpers.c \
	mpu_init.c \
	mpu_read.c

MPU := $(addprefix devs/mpu3300/, $(MPU))

# Required modules from the itg folder
ITG := \
  itg_conf.c \
  itg_conf_aux.c \
  itg_conf_fifo.c \
  itg_conf_gyro_offset.c \
  itg_conf_i2c.c \
  itg_conf_int.c \
  itg_conf_sync.c \
  itg_conf_user_ctrl.c \
  itg_dealloc.c \
  itg_init.c \
  itg_read.c

ITG := $(addprefix devs/itg3050/, $(ITG))

# Required modules from the pca folder
PCA := \
	pca_conf.c \
	pca_dealloc.c \
	pca_init.c \
	pca_muxnet.c

PCA := $(addprefix devs/pca9548a/, $(PCA))

# Required modules from the imu folder
IMU := \
	imu_gyro.c \
	imu_pca.c \
	imu_routing.c

IMU := $(addprefix imu/, $(IMU))

# Required modules from the shared folder
SHARED := \
	io.c

SHARED := $(addprefix shared/, $(SHARED))

# Required modules from the git submodule
TOOLS := \
	keyval.c

TOOLS := $(addprefix ../tools/src/, $(TOOLS))

MODULES := \
	$(GPIO) $(I2C) $(DEV) $(MPU) $(ITG) $(PCA) $(IMU) $(SHARED) $(TOOLS)

include $(BUILDROOT)/build/App.Makefile
