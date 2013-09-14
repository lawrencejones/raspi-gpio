///////////////////////////////////////////////////////////////////////////////
// Raspberry Pi GPIO Interface
// ¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯
// File: mpu3300.h
// PA Consulting - Lawrence Jones
///////////////////////////////////////////////////////////////////////////////

#ifndef MPU3300_HEADER_INC
#define MPU3300_HEADER_INC

#include "dev/shared.h"
#include "dev/shared/dev_sensor.h"

///////////////////////////////////////////////////////////////////////////////
// INTERFACE - FUNCTION STUBS
///////////////////////////////////////////////////////////////////////////////

// Malloc and initialise an mpu struct, return the pointer
Sensor *mpu_init          (  i2c_bus*    i2c,                         // INIT
                             int         i2c_addr,
                             Mux*        mux,
                             int         mux_channel,
                             char*       config  );

#endif
