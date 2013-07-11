///////////////////////////////////////////////////////////////////////////////
// Raspberry Pi GPIO Interface
// ¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯
// File: print.h
// PA Consulting - Lawrence Jones
///////////////////////////////////////////////////////////////////////////////

#ifndef PRINT
#define PRINT

#include "io.h"

static const char* yn[] = {"\x1b[32mINP\x1b[0m", "OUT"};
static const char* vals[] = {"0", "\x1b[91m1\x1b[0m"};

void printHeader();
void printPin(Pin *pin);
void printAll();

#endif
