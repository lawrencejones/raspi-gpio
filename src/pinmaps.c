///////////////////////////////////////////////////////////////////////////////
// Raspberry Pi GPIO Interface
// ¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯
// File: pinmaps.c
// PA Consulting - Lawrence Jones
///////////////////////////////////////////////////////////////////////////////

// Contains mapping arrays to convert from physical
// on chip pin indexes (1-26) to the memory address indexes
// and from gpio label numbers to the memory address indexes

#include "pinmaps.h"

// Takes the pin memory address index and returns
// it's gpio label
int memToGpioLabel(int p)
{
	for (int i = 0; i < 17; i++)
		if (gpioIndexes[i] == p) return i;
	return -1;
}

int chipPinToMem(int p) { return memIndexes[p]; }
