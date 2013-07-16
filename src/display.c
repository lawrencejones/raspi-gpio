///////////////////////////////////////////////////////////////////////////////
// Raspberry Pi GPIO Interface
// ¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯
// File: display.c
// PA Consulting - Lawrence Jones
///////////////////////////////////////////////////////////////////////////////

#include "io.h"
#include <unistd.h>
#include <time.h>
#include <pthread.h>
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

static uint32_t clrWord;

int muls[]    = { 1, 10, 100, 1000 };
int binPins[] = { B0, B1, B2, B3 };
int decPins[] = { D0, D1, D2, D3 };
int *value; pthread_t clockThread; 

uint32_t generateClearWord()
{
  clrWord = 0;
  for (int i = 0; i < 4; i++)
  {
    clrWord |= 1 << chipPinToMem(binPins[i]);
    clrWord |= 1 << chipPinToMem(decPins[i]);
  } return clrWord;
}

// Assumes pins are already prepped for output
// and writing
void *outputToDisplay(void *arg)
{
  int v = 0; uint32_t setWord;
  generateClearWord();
  for (;;)
  {
    for (int j = 0; j < 4; j++)
    {
      setWord = 0;
      v = (*value / muls[j]) % 10;
      // Run from LSB -> MSB
      for (int i = 0; i < 4; i++)
      {
        setWord |= (0x1u & v) << chipPinToMem(binPins[i]);
        v >>= 1;
      } clr_with_word(clrWord & ~setWord);
      set_with_word(setWord | (1u << chipPinToMem(decPins[j])));
      nanosleep((struct timespec[]) {{0, 1000000}}, NULL);
    }
  }
}

void setDisplayPinsOut()
{
  for (int i = 0; i < 4; i++)
  {
    set_pin_state(decPins[i], 1);
    set_pin_state(binPins[i], 1);
  }
}

void stopClock(void)
{
  pthread_cancel(clockThread);
}

void setValue(int val)
{ 
  if (value)
  {
    free(value);
  }
  value = malloc(sizeof(int));
  *value = val; 
}

int *runme(int val)
{
  init_gpio_access();
  init_chip();
  setDisplayPinsOut();
  setValue(val);
  pthread_create(&clockThread, NULL, &outputToDisplay, NULL);
  pthread_detach(clockThread);
  return value;
}


int main(int argv, char** argc)
{
  int val;
  if (argv)
  {
    val = atoi(argc[1]);
  }
  else 
  {
    val = (int)argc;
  }
  runme(*value);
  dealloc_chip();
  return 0;
}
