///////////////////////////////////////////////////////////////////////////////
// Raspberry Pi GPIO Interface
// ¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯
// File: itg3050.h
// PA Consulting - Lawrence Jones
///////////////////////////////////////////////////////////////////////////////

#ifndef ITG3050_HEADER_INC
#define ITG3050_HEADER_INC

#include "dev/shared.h"
#include "dev/shared/dev_sensor.h"

///////////////////////////////////////////////////////////////////////////////
// INTERFACE - FUNCTION STUBS
///////////////////////////////////////////////////////////////////////////////

// Malloc and initialise an mpu struct, return the pointer
Sensor *itg_init          (  i2c_bus*    i2c,                         // INIT
                             int         i2c_addr,
                             Mux*        mux,
                             int         mux_channel,
                             char*       config  );

#endif
