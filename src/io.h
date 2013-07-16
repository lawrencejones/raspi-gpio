///////////////////////////////////////////////////////////////////////////////
// Raspberry Pi GPIO Interface
// ¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯
// File: io.h
// PA Consulting - Lawrence Jones
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
// INSTRUCTIONS
///////////////////////////////////////////////////////////////////////////////
/*
  The process to go from x to pin(g) on, is shown below...
    
    INP_GPIO(g)  =>  OUT_GPIO(g)  =>  CLR_PIN(g)  =>  SET_PIN(g)

  ...which first sets the pin to input mode to clear the current
  setting. This is entirely optional and is only present to highlight
  the possible entry at step 2, regardless of previous states.
  The clear pin is required to prep the pi for changing the said pins
  status. Once cleared, any value may be sent to the SET_PIN macro.
*/

#ifndef INPUTS
#define INPUTS

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <unistd.h>
#include "pinmaps.h"

///////////////////////////////////////////////////////////////////////////////
// DEFINE MACROS
///////////////////////////////////////////////////////////////////////////////

// Define the 'base' mem addr for the start of gpio access
#define BCM2708_PERI_BASE  0x20000000u
#define BCM2835_PERI_BASE  0x20000000u
// Calculate the base dependent on platform
#define GPIO_BASE ( BCM2835_PERI_BASE + 0x00200000u )
// Define macro values for the paging and block size
#define PAGE_SIZE  (4*1024)
#define BLOCK_SIZE (4*1024)
#define NA 0 
#define NO_OF_PINS 26

///////////////////////////////////////////////////////////////////////////////
// Macros to prep pins for accessing
// INP_GPIO must be used prior to OUT and SET
///////////////////////////////////////////////////////////////////////////////
// General macros to supply the set clear and value words
#define SET_WORD    *(gpio +  7)
#define CLR_WORD    *(gpio + 10)
#define VAL_WORD    *(gpio + (0x34 >> 2))
// Given g, the physical memory index of a pin, return the
// shift required to bring the pins control word to
// the pin control value being it's three least significant bits
#define PIN_SHIFT(g) 3*(g%10)
// Given g, a pins memory index, find the control word value
// for that specific pin.
#define PIN_CONTROL_WORD(g) *(gpio + (g/10))
// Modify the pin control word to represent the given pin
// in input mode, 000. Pin specific bits found at input code 
// left shifted by the pin number modulo 10, multiplied by 3
#define INP_GPIO(g) PIN_CONTROL_WORD(g) &= ~(7 << PIN_SHIFT(g))
// Modify the specific pin word to set the out code for the
// given pin number, 001
#define OUT_GPIO(g) PIN_CONTROL_WORD(g) |=  (1 << PIN_SHIFT(g))
// Macros to set and clear individual pins
#define GPIO_SET(g,v) SET_WORD = v << g  // offset to set word
#define GPIO_CLR(g) CLR_WORD   = 1 << g  // offset to clear word
// Macro to turn pin to alternate purpose
// if a (mode) is <= to 3, then return 0b1(a)
// if a == 4 then return 011
// else return 010
// Then shift the mode by the relevant pin
#define GPIO_MODE(m) ((m) <= 3? (m + 4):((m) == 4? (3):(2)))
#define SET_GPIO_ALT(g,a) PIN_CONTROL_WORD(g) |= (GPIO_MODE(a) << PIN_SHIFT(g))

///////////////////////////////////////////////////////////////////////////////
// Macros defining I2C Data Protocol
///////////////////////////////////////////////////////////////////////////////
// The base addresses of the Broadcom Serial Buses
#define BSC0_BASE  (BCM2835_PERI_BASE + 0x00205000u)
#define BSC1_BASE  (BCM2835_PERI_BASE + 0x00804000u)
// Note that BSC2 is linked to HDMI, inadvisable to mess with
#define BSC2_BASE  (BCM2835_PERI_BASE + 0x00805000u)
// Repeat the BSC0 base for ease of access
#define I2C_BASE   (BSC0_BASE)

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

///////////////////////////////////////////////////////////////////////////////
// TYPEDEFS / STRUCTS
///////////////////////////////////////////////////////////////////////////////

// Define unsigned int for reading results
// 16 bits presumed to be sufficient
typedef uint16_t u16;
// Define type to represent all four pin states
typedef enum { INPUT, OUTPUT } state_t;
static char* stateLabels[] = {
  "INPUT", "OUTPUT"
};

// Define the Pin struct
typedef struct {
  int chipIndex;
  int memIndex;
  state_t state;
  int value;
} Pin;

// Define the Chip struct
typedef struct {
  Pin* pins[NO_OF_PINS];
} Chip;

// Define the map point for access
static void* gpioMap;
// Define the entry point for gpios
volatile unsigned *gpio;
// Define the entry point for I2C interface
volatile unsigned *i2c;
// Define the chip state
static Chip* chip;

///////////////////////////////////////////////////////////////////////////////
// FUNCTION STUBS
///////////////////////////////////////////////////////////////////////////////

// Initialisation    ////////////////////////
volatile unsigned* initialiseGpioAccess();
// Malloc / Dealloc   ///////////////////////
Chip  *initialiseChip();
Pin   *mallocPin(int p);
void  deallocChip();
int   chipIndexToMem(int p);
// Gpio Read    /////////////////////////////
Pin   *pinStatus(int p);
Pin   *updatePinStatus(Pin* pin);
Pin   **updateAllPins();
// Gpio Write    ////////////////////////////
void  setWithWord(uint32_t w);
void  clrWithWord(uint32_t w);
void  setPin(int p, int v);
void  setPinState(int p, int v);
// Sinatra Add-ons   ////////////////////////
Chip  *getChip();

#endif
