///////////////////////////////////////////////////////////////////////////////
// Virtual Gyroscope.
// ¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯
// File: vgyro.h
// PA Consulting - Pierre Sutter
///////////////////////////////////////////////////////////////////////////////

#ifndef VGYRO
#define VGYRO
#include "generate_signal.h"

/////////////////////////////////////////////////////////////////////////////////
// Param Structures.
/////////////////////////////////////////////////////////////////////////////////

typedef struct
{
  float             curr_value;
  float             next_value;
  Kalman*           kf;
} Params_Vgyro;

/////////////////////////////////////////////////////////////////////////////////
// Generate Signal Function.
/////////////////////////////////////////////////////////////////////////////////

void							generate_vgyro();

#endif
