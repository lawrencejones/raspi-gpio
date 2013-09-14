///////////////////////////////////////////////////////////////////////////////
// Raspberry Pi GPIO Interface
// ¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯
// File: itg_registers.h
// PA Consulting - Lawrence Jones
///////////////////////////////////////////////////////////////////////////////

#ifndef ITG3050_REGISTERS_INC
#define ITG3050_REGISTERS_INC

///////////////////////////////////////////////////////////////////////////////
// REGISTERS
///////////////////////////////////////////////////////////////////////////////

// Identity register
#define ITG_WHO_AM_I 0x00

// Gyro offset values
#define ITG_X_OFFS_H 0x0c
#define ITG_X_OFFS_L 0x0d
#define ITG_Y_OFFS_H 0x0e
#define ITG_Y_OFFS_L 0x0f
#define ITG_Z_OFFS_H 0x10
#define ITG_Z_OFFS_L 0x11

// Samplerate divider register
#define ITG_SMPLRT_DIV 0x15
#define ITG_SYNC_SET 0x16

// Temperature registers
#define ITG_TEMP_H 0x1b
#define ITG_TEMP_L 0x1c

// Gyro axes value registers
#define ITG_XOUT_H 0x1d
#define ITG_XOUT_L 0x1e
#define ITG_YOUT_H 0x1f
#define ITG_YOUT_L 0x20
#define ITG_ZOUT_H 0x21
#define ITG_ZOUT_L 0x22

// Auxiliary sensor data
#define ITG_AUX_XOUT_H 0x23
#define ITG_AUX_XOUT_L 0x24
#define ITG_AUX_YOUT_H 0x25
#define ITG_AUX_YOUT_L 0x26
#define ITG_AUX_ZOUT_H 0x27
#define ITG_AUX_ZOUT_L 0x28

// First in first out count
#define ITG_FIFO_COUNTH 0x3a
#define ITG_FIFO_COUNTL 0x3b
// Entry point for FIFO
#define ITG_FIFO_R 0x3c
// Address of fifo controls
#define ITG_FIFO_EN 0x12

// Interupt status
#define ITG_INT_STATUS 0x1a
// Interupt configuration
#define ITG_INT_CFG 0x17

// i2c slave address
#define ITG_AUX_SLV_ADDR 0x14
// i2c slave burst address
#define ITG_AUX_ADDR 0x18

// User control register
#define ITG_USER_CTRL 0x3d
// Power management configuration
#define ITG_POWER_MGMT 0x3e

#endif
