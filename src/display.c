///////////////////////////////////////////////////////////////////////////////
// Raspberry Pi GPIO Interface
// ¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯
// File: display.c
// PA Consulting - Lawrence Jones
///////////////////////////////////////////////////////////////////////////////

#include "io.h"
#include <unistd.h>
#include <time.h>
#include <stdio.h>
#include <stdlib.h>

#define B0 26
#define B1 24
#define B2 23
#define B3 22

#define D0 16
#define D1 18
#define D2 19
#define D3 21

int muls[]    = { 1, 10, 100, 1000 };
int binPins[] = { B0, B1, B2, B3 };
int decPins[] = { D0, D1, D2, D3 };
int value;

static inline void selectDecoder(int index)
{
  for (int j = 0; j < 4; j++)
	  setPin(decPins[j], 0);
	setPin(decPins[index], 1);
}

// Assumes pins are already prepped for output
// and writing
void outputToDisplay()
{
  int v = 0;  
start:
  for (int j = 0; j < 4; j++)
  {
    selectDecoder(j);
    v = (value / muls[j]) % 10;
    // Run from LSB -> MSB
    for (int i = 0; i < 4; i++)
    {
      setPin(binPins[i], (0x1u & v));
      v >>= 1;
    } for(int k=0; k<0xfffu; k++);
  }
  goto start;
}

void setDisplayPinsOut()
{
  for (int i = 0; i < 4; i++)
  {
    setPinState(decPins[i], 1);
    setPinState(binPins[i], 1);
  }
}

void setValue(int val)
{ value = val; }

int main(int argv, char** argc)
{
  if (argv)
    value = atoi(argc[1]);
  else value = (int)argc;
  initialiseGpioAccess();
  initialiseChip();
  setDisplayPinsOut();
	outputToDisplay();
  deallocChip();
  return 0;
}
