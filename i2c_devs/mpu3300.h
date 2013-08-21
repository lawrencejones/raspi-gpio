///////////////////////////////////////////////////////////////////////////////
// Raspberry Pi GPIO Interface
// ¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯
// File: mpu3300.h
// PA Consulting - Lawrence Jones
///////////////////////////////////////////////////////////////////////////////

#ifndef MPU3300
#define MPU3300

#include "board.h"

///////////////////////////////////////////////////////////////////////////////
// REGISTERS
///////////////////////////////////////////////////////////////////////////////

// Self test registers
#define MPU_SELF_TEST_X 0x0d
#define MPU_SELF_TEST_Y 0x0e
#define MPU_SELF_TEST_Z 0x0f

// Sample rate divider
#define MPU_SMPLRT_DIV 0x19

// Basic config register
#define MPU_CONFIG 0x1a
// Gyro config register
#define MPU_GYRO_CONFIG 0x1b

// First in first out enable register
#define MPU_FIFO_EN 0x23

// i2c master control register
#define MPU_I2C_MST_CTRL 0x24
// i2c slave address register
#define MPU_I2C_SLV0_ADDR 0x25
// i2c slave register for fifo
#define MPU_I2C_SLV0_REG 0x26
// i2c slave control register
#define MPU_I2C_SLV0_CTRL 0x27
// i2c master status
#define MPU_I2C_MST_STATUS 0x36

// Interupt pin configuration
#define MPU_INT_PIN_CFG 0x37
// Interupt enable config
#define MPU_INT_ENABLE 0x38
// Interupt status register
#define MPU_INT_STATUS 0x3a

// Temperature registers
#define MPU_TEMP_H 0x41
#define MPU_TEMP_L 0x42

// Gyro axes value registers
#define MPU_XOUT_H 0x43
#define MPU_XOUT_L 0x44
#define MPU_YOUT_H 0x45
#define MPU_YOUT_L 0x46
#define MPU_ZOUT_H 0x47
#define MPU_ZOUT_L 0x48

// Entry registers for external sensor readings
#define MPU_EXT_DATA(n) 0x49 + n

// i2c master delay control
#define MPU_I2C_MST_DELAY_CTRL 0x67
// Signal path reset register
#define MPU_SIGNAL_PATH_RESET 0x68

// User control register
#define MPU_USER_CTRL 0x6a

// Power management configuration
#define MPU_POWER_MGMT_1 0x6b
#define MPU_POWER_MGMT_2 0x6c

// First in first out count
#define MPU_FIFO_COUNTH 0x72
#define MPU_FIFO_COUNTL 0x73
// Entry point for FIFO
#define MPU_FIFO_R_W 0x74

// Identity register
#define MPU_WHO_AM_I 0x75

///////////////////////////////////////////////////////////////////////////////
// INTERFACE - FUNCTION STUBS
///////////////////////////////////////////////////////////////////////////////

// Malloc and initialise an mpu struct, return the pointer
Sensor  *mpu_init     (  char*       name, 
                         int         i2c_addr,
                         KeyVal*     config  );
// Configure the given mpu struct pointer using the keyval array
int     mpu_configure (  Sensor*     s, 
                         KeyVal*     settings  );
// Read from the given sensor with the given target
Axes    *mpu_read     (  Sensor*     s, 
                         enum Target t  );
// Dealloc an mpu sensor struct. See .c for dependency behaviour
// and warnings.
void    mpu_dealloc   (  Sensor**    s  );

#endif
