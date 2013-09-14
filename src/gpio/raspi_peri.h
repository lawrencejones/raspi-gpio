///////////////////////////////////////////////////////////////////////////////
// Raspberry Pi GPIO Interface
// ¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯
// File: io.h
// PA Consulting - Lawrence Jones
///////////////////////////////////////////////////////////////////////////////

#ifndef RASPI_PERI_HEADER_INC
#define RASPI_PERI_HEADER_INC

///////////////////////////////////////////////////////////////////////////////
// PLATFORM VALUES
///////////////////////////////////////////////////////////////////////////////

// Define the 'base' mem addr for the start of gpio access
#define BCM2708_PERI_BASE  0x20000000u
#define BCM2835_PERI_BASE  0x20000000u

// Define macro values for the paging and block size
#define PAGE_SIZE  (4*1024)
#define BLOCK_SIZE (4*1024)
#define NA 0 
#define NO_OF_PINS 26
#define WRITE 0
#define READ  1

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
// FUNCTION STUBS
///////////////////////////////////////////////////////////////////////////////

// Configure the memory access required to alter
// the GPIO pins. Will request memory access from the system
volatile unsigned* get_mmap(int base);                             // get_mmap

#endif
