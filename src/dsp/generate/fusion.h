///////////////////////////////////////////////////////////////////////////////
// Virtual Gyroscope.
// ¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯
// File: fusion.h
// PA Consulting - Pierre Sutter
///////////////////////////////////////////////////////////////////////////////

#ifndef FUSION
#define FUSION
#include "generate_signal.h"

/////////////////////////////////////////////////////////////////////////////////
// Param Structures.
/////////////////////////////////////////////////////////////////////////////////

typedef struct
{
  float             wg; 
} Params_Fusion;

/////////////////////////////////////////////////////////////////////////////////
// Generate Signal Function.
/////////////////////////////////////////////////////////////////////////////////

void							  generate_fusion();

#endif
