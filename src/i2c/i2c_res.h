///////////////////////////////////////////////////////////////////////////////
// Raspberry Pi GPIO Interface
// ¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯
// File: i2c_res.h
// PA Consulting - Lawrence Jones
///////////////////////////////////////////////////////////////////////////////

#ifndef I2C_RES_HEADER_INC
#define I2C_RES_HEADER_INC

#include "../shared/io.h"

///////////////////////////////////////////////////////////////////////////////
// Macros defining I2C Data Protocol
///////////////////////////////////////////////////////////////////////////////
// The base addresses of the Broadcom Serial Buses
#define BSC0_BASE  (BCM2835_PERI_BASE + 0x00205000u)
#define BSC1_BASE  (BCM2835_PERI_BASE + 0x00804000u)
// Note that BSC2 is linked to HDMI, inadvisable to mess with
#define BSC2_BASE  (BCM2835_PERI_BASE + 0x00805000u)
// Provide access to different bases
#define I2C_BASE(bus) \
  bus?(BSC1_BASE):(BSC0_BASE)

// Define the base independant offsets
// Hex is actual offset (in byte addressable), but will
// return offset in words

///////////////////////////////////////////////////////////////////////////////
// I2C Control Register    ////////////////////////////////////////////////////
// 10  -> INTR interrupt on RX, 1/0                                       INTR
// 9   -> INTT interrupt on TX, 1/0                                       INTT
// 8   -> INTD interrupt on DONE, 1/0                                     INTD
// 7   -> Start transfer, write 1                                           ST
// 5:4 -> Clear FIFO, 00 nothing, 01 clear                               CLEAR
// 0   -> Read or Write 1/0                                               READ
// 31:16, 14:11, 6, 3:1 -> *RESERVED*
#define BSC_C              *(i2c + (0x00u >> 2))
#define BSC_C_I2CEN         (1 << 15)
#define BSC_C_INTR          (1 << 10)
#define BSC_C_INTT          (1 << 9)
#define BSC_C_INTD          (1 << 8)
#define BSC_C_ST            (1 << 7)
#define BSC_C_CLEAR         (1 << 4)
#define BSC_C_READ          (1 << 0)
///////////////////////////////////////////////////////////////////////////////
// I2C Status Register    /////////////////////////////////////////////////////
// 9 -> Clock stretch timeout, 1 slave error                              CLKT
// 8 -> Error acknowledgement, 0 clean, 1 slave not acked                  ERR
// 7 -> FIFO capacity, full?  1/0                                          RXF
// 6 -> FIFO capacity, empty? 1/0                                          TXE
// 5 -> FIFO >1 byte / empty? 1/0                                          RXD
// 4 -> FIFO 1 means can accept at least 1 byte                            TXD
// 3 -> FIFO is less than full and read underway                           RXR
// 2 -> FIFO requires writing, 1 write underway                            TXW
// 1 -> Transfer done?   1/0                                              DONE
// 0 -> Transfer active? 1/0                                                TA
// 31:10 -> *RESERVED*
#define BSC_S              *(i2c + (0x04u >> 2))
#define BSC_S_CLKT          (1 << 9)
#define BSC_S_ERR           (1 << 8)
#define BSC_S_RXF           (1 << 7)
#define BSC_S_TXE           (1 << 6)
#define BSC_S_RXD           (1 << 5)
#define BSC_S_TXD           (1 << 4)
#define BSC_S_RXR           (1 << 3)
#define BSC_S_TXW           (1 << 2)
#define BSC_S_DONE          (1 << 1)
#define BSC_S_TA            (1 << 0)
///////////////////////////////////////////////////////////////////////////////
// I2C Data Length Register  //////////////////////////////////////////////////
// 15:0 -> Specifies no of bytes to write/receive                         DLEN
// 31:16 -> *RESERVED*
#define BSC_DATA_LEN       *(i2c + (0x08u >> 2))
///////////////////////////////////////////////////////////////////////////////
// I2C Slave Address Register  ////////////////////////////////////////////////
// 6:0 -> 7 bit slave address                                             ADDR
// 31:7 -> *RESERVED*
#define BSC_SLAVE_ADDR     *(i2c + (0x0cu >> 2))
///////////////////////////////////////////////////////////////////////////////
// I2C FIFO Register  /////////////////////////////////////////////////////////
// 7:0 -> Writes to FIFO, reads from FIFO                                 DATA
// 31:8 -> *RESERVED*
#define BSC_FIFO           *(i2c + (0x10u >> 2))
///////////////////////////////////////////////////////////////////////////////
// I2C Clock Divider Register  ////////////////////////////////////////////////
// 15:0 -> Default 0 = div(32768)                                         CDIV
// 31:16 -> *RESERVED*
#define BSC_CLOCK_DIV      *(i2c + (0x14u >> 2))
///////////////////////////////////////////////////////////////////////////////
// I2C Data Delay Register  ///////////////////////////////////////////////////
// 31:16 -> Falling edge delay, no of clk cycles                          FEDL
// 15:0  -> Rising edge delay, no of clk cycles                           REDL
#define BSC_DATA_DELAY     *(i2c + (0x18u >> 2))
///////////////////////////////////////////////////////////////////////////////
// I2C Clock Stretch Timeout  /////////////////////////////////////////////////
// 15:0 -> Clock stretch timeout value in cycles                          TOUT
// 31:16 -> *RESERVED*
#define BSC_CLOCK_STRETCH  *(i2c + (0x1cu >> 2))
///////////////////////////////////////////////////////////////////////////////

// Start I2C read macro, with enable, start transfer, 
// clear FIFO, read behaviour
#define START_READ    BSC_C_I2CEN|BSC_C_ST|BSC_C_CLEAR|BSC_C_READ
// Start I2C write macro with enable and start bits
#define START_WRITE   BSC_C_I2CEN|BSC_C_ST
// Define a 'clear' status, where timeout has been exceeded,
// error has occurred, bus reads 'DONE'
#define CLEAR_STATUS  BSC_S_CLKT|BSC_S_ERR|BSC_S_DONE

#endif