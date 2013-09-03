///////////////////////////////////////////////////////////////////////////////
// Raspberry Pi GPIO Interface
// ¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯
// File: mpu3300.h
// PA Consulting - Lawrence Jones
///////////////////////////////////////////////////////////////////////////////

#ifndef MPU3300_HEADER_INC
#define MPU3300_HEADER_INC

#include "devs/shared.h"
#include "devs/shared/dev_sensor.h"

///////////////////////////////////////////////////////////////////////////////
// INTERFACE - FUNCTION STUBS
///////////////////////////////////////////////////////////////////////////////

// Malloc and initialise an mpu struct, return the pointer
Sensor *mpu_init          (  i2c_bus*    i2c,                         // INIT
                             int         i2c_addr,
                             Mux*        mux,
                             int         mux_channel,
                             char*       config  );

// Set the mpu to the default config
void    mpu_reset         (  Sensor      *s  );                       // RESET
// Enables the given sensor with sensible defaults/given config
int     mpu_enable        (  Sensor*     s,                           // ENABLE
                             KeyVal*     settings  );
// Disables the given sensor
int     mpu_disable       (  Sensor*     s );                         // DISABLE
// Read from the given sensor with the given target
Axes    *mpu_read         (  Sensor*     s,                           // READ
                             target_t    t  );
// Configure the given mpu struct pointer using the keyval array
int     mpu_config        (  Sensor*     s,                           // CONFIG
                             char*       conf_str  );
// Runs a selftest on the gyro
int     mpu_selftest      (  Sensor*     mpu  );                      // SELFTEST
// Dealloc an mpu sensor struct. See .c for dependency behaviour
// and warnings.
void    mpu_dealloc       (  Sensor**    s  );                        // DEALLOC

#endif
