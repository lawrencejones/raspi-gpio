///////////////////////////////////////////////////////////////////////////////
// Raspberry Pi GPIO Interface
// ¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯
// File: gpio_res.h
// PA Consulting - Lawrence Jones
///////////////////////////////////////////////////////////////////////////////

#ifndef GPIO_RES_HEADER_INC
#define GPIO_RES_HEADER_INC

#include "gpio/raspi_peri.h"

#define GPIO_BASE ( BCM2835_PERI_BASE + 0x00200000u )

#define NA 0

// Maps the Gpio label number to the meory address
// indexes.
static const int gpioIndexes[] = {
  /* Gpio 0  */ 17,
  /* Gpio 1  */ 18,
  /* Gpio 2  */ 27,
  /* Gpio 3  */ 22,
  /* Gpio 4  */ 23,
  /* Gpio 5  */ 24,
  /* Gpio 6  */ 25,
  /* Gpio 7  */ 4,
  /* Gpio 8  */ 2,
  /* Gpio 9  */ 3,
  /* Gpio 10 */ 8,
  /* Gpio 11 */ 7,
  /* Gpio 12 */ 10,
  /* Gpio 13 */ 9,
  /* Gpio 14 */ 11,
  /* Gpio 15 */ 14,
  /* Gpio 16 */ 15,
};

// Maps the physical ON CHIP pin indexes by means of an
// array to their memory address index counterparts
static const int memIndexes[] = {
  /* Not applicable */                NA,
  /* Pin 1  - 3.3v Power        */    0,
  /* Pin 2  - 5v Power          */    1,
  /* Pin 3  - SDA - Gpio 8      */    2,
  /* Pin 4  - 5v Power          */    NA,
  /* Pin 5  - SCL - Gpio 9      */    3,
  /* Pin 6  - 0v Ground         */    NA,
  /* Pin 7  - GPCLK0 - Gpio 7   */    4,
  /* Pin 8  - TXD - Gpio 15     */    14,
  /* Pin 9  - 0v Ground         */    NA,
  /* Pin 10 - RXD - Gpio 16     */    15,
  /* Pin 11 - Gpio 0            */    17,
  /* Pin 12 - PCM_CLK - Gpio 1  */    18,
  /* Pin 13 - Gpio 2            */    27,
  /* Pin 14 - 0v Ground         */    NA,
  /* Pin 15 - Gpio 3            */    22,
  /* Pin 16 - Gpio 4            */    23,
  /* Pin 17 - 3.3v Power        */    NA,
  /* Pin 18 - Gpio 5            */    24,
  /* Pin 19 - MOSI - Gpio 12    */    10,
  /* Pin 20 - 0v Ground         */    NA,
  /* Pin 21 - MISO - Gpio 13    */    9,
  /* Pin 22 - Gpio 6            */    25,
  /* Pin 23 - SCLK - Gpio 14    */    11,
  /* Pin 24 - CE0 - Gpio 10     */    8,
  /* Pin 25 - 0v Ground         */    NA,
  /* Pin 26 - CE1 - Gpio 11     */    7,
};

#endif
