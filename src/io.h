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

#define SET_WORD *(gpio +  7)
#define CLR_WORD *(gpio + 10)
#define VAL_WORD *(gpio + (0x34 >> 2))
#define NA 0 
#define NO_OF_PINS 26

///////////////////////////////////////////////////////////////////////////////
// Macros to prep pins for accessing
// INP_GPIO must be used prior to OUT and SET
///////////////////////////////////////////////////////////////////////////////
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
void  setPin(int p, int v);
void  setPinState(int p, int v);
// Sinatra Add-ons   ////////////////////////
Chip  *getChip();

#endif
