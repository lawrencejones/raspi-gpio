///////////////////////////////////////////////////////////////////////////////
// Raspberry Pi GPIO Interface
// ¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯
// File: inputs.h
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

///////////////////////////////////////////////////////////////////////////////
// Macros to prep pins for accessing
// INP_GPIO must be used prior to OUT and SET
///////////////////////////////////////////////////////////////////////////////
// Returns the shift required for a given pins control code
#define PIN_SHIFT(g) ((g-1) % 10)*3
// Using the gpio counter plus (pin number - 1)/10  locate 
// the correct word to represent the pins physical address
#define PIN_CONTROL_WORD(g) *(gpio + ((g - 1) / 10))
// Modify the pin control word to represent the given pin
// in input mode, 000. Pin specific bits found at input code 
// left shifted by the pin number modulo 10, multiplied by 3
#define INP_GPIO(g) PIN_CONTROL_WORD(g) &= ~(7 << PIN_SHIFT(g))
// Modify the specific pin word to set the out code for the
// given pin number, 001
#define OUT_GPIO(g) PIN_CONTROL_WORD(g) |=  (1 << PIN_SHIFT(g))
// Macros to set and clear individual pins
#define GPIO_SET(g) SET_WORD |= 1 << (g - 1)  // offset to set word
#define GPIO_CLR(g) CLR_WORD &= 1 << (g - 1)  // offset to clear word

// Define unsigned int for reading results
// 16 bits presumed to be sufficient
typedef uint16_t u16;
// Define type to represent all four pin states
typedef enum { OUTHIGH, OUTLOW, INHIGH, INLOW } state_t;
static char* stateLabels[] = {
  "OUT / HIGH", "OUT / LOW", "IN / HIGH", "IN / LOW"
};

// Define the map point for access
static void* gpioMap;
// Define the entry point for gpios
volatile unsigned *gpio;

///////////////////////////////////////////////////////////////////////////////
// FUNCTION STUBS
///////////////////////////////////////////////////////////////////////////////

// Standard sensor retrieval functions
u16 getTemperature();
u16 getLight();
u16 getHumidity();
u16 getPressure();
u16 getUV();
u16 getNO2();
u16 getCO();

// GPIO helpers
int initialiseGpios();
int pinWrite(int g);

// Given a pin number, will return it's current state
state_t pinStatus(int g);


#endif