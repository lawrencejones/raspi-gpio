///////////////////////////////////////////////////////////////////////////////
// Raspberry Pi GPIO Interface
// ¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯
// File: mpu3300.h
// PA Consulting - Lawrence Jones
///////////////////////////////////////////////////////////////////////////////

#ifndef MPU3300_HEADER_INC
#define MPU3300_HEADER_INC

#include "shared/devs_sensor.h"

///////////////////////////////////////////////////////////////////////////////
// INTERFACE - FUNCTION STUBS
///////////////////////////////////////////////////////////////////////////////

// Malloc and initialise an mpu struct, return the pointer
Sensor *mpu_init          (  char*       name, 
                             i2c_bus*    i2c,
                             int         i2c_addr,
                             char*       config  );
// Set the mpu to the default config
void    mpu_set_defaults  (  Sensor      *s  );
// Enables the given sensor with sensible defaults/given config
int     mpu_enable        (  Sensor*     s,
                             KeyVal*     settings  );
// Disables the given sensor
int     mpu_disable       (  Sensor*     s );
// Configure the given mpu struct pointer using the keyval array
int     mpu_configure     (  Sensor*     s, 
                             char*       conf_str  );
// Read from the given sensor with the given target
Axes    *mpu_read         (  Sensor*     s, 
                             target_t    t  );
// Dealloc an mpu sensor struct. See .c for dependency behaviour
// and warnings.
void    mpu_dealloc       (  Sensor**    s  );

#endif
