///////////////////////////////////////////////////////////////////////////////
// Raspberry Pi GPIO Interface
// ¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯
// File: inputs.h
// PA Consulting - Lawrence Jones
///////////////////////////////////////////////////////////////////////////////

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
#define BCM2708_PERI_BASE               0x20000000u
// Calculate the base dependent on platform
#define GPIO_BASE ( BCM2708_PERI_BASE + 0x00200000u )
// Define macro values for the paging and block size
#define PAGE_SIZE  (4*1024)
#define BLOCK_SIZE (4*1024)

// Macros to prep pin for accessing
// INP_GPIO must be used prior to OUT and SET
#define INP_GPIO(g) *(gpio+((g-1)/10)) &= ~(7<<(((g-1)%10)*3))
#define OUT_GPIO(g) *(gpio+((g-1)/10)) |=  (1<<(((g-1)%10)*3))
#define SET_GPIO_ALT(g,a) *(gpio+(((g-1)/10))) |= \
                    (((a)<=3?(a)+4:(a)==4?3:2)<<(((g-1)%10)*3))

// Macros to set and clear individual pins
#define GPIO_SET(p) *(gpio +  7) |= 1 << (p -1)  // offset to set word
#define GPIO_CLR(p) *(gpio + 10) &= ~(1 << (p - 1))  // offset to clear word

// Define unsigned int for reading results
// 16 bits presumed to be sufficient
typedef uint16_t u16;

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

#endif