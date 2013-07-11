///////////////////////////////////////////////////////////////////////////////
// Raspberry Pi GPIO Interface
// ¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯
// File: display.c
// PA Consulting - Lawrence Jones
///////////////////////////////////////////////////////////////////////////////

#include "io.h"
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>

#define PIN1 3
#define PIN2 5
#define PIN3 7
#define PIN4 8

#define PAUSE 1000000
int muls[] = { 1000, 100, 10, 0 };

static int *value;

void setValue(int v)
{ *value = v; }

// Assumes pins are already prepped for output
// and writing
void *initiateDisplay(int _value)
{
  value = malloc(sizeof(int));
  setValue((int)_value);
  int v;
start:
  for (int i = 0; i < 4; i++)
  {
    v = (*value / muls[i]) % muls[3-i];
    setPin(PIN1, v & 0x1u);
    setPin(PIN2, v & 0x2u);
    setPin(PIN3, v & 0x4u);
    setPin(PIN4, v & 0x8u);
    sleep(1);
  }
  goto start;
}

int main(int argv, char** argc)
{
	initiateDisplay(atoi(argc[1]));
  return 0;
}
