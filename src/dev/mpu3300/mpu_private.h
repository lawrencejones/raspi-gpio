///////////////////////////////////////////////////////////////////////////////
// Raspberry Pi GPIO Interface
// ¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯
// File: mpu_private.h
// PA Consulting - Lawrence Jones
///////////////////////////////////////////////////////////////////////////////

#ifndef MPU3300_PRIVATE_HEADER_INC
#define MPU3300_PRIVATE_HEADER_INC

#include "dev/mpu3300.h"
#include "dev/shared/dev_sensor.h"

///////////////////////////////////////////////////////////////////////////////
// MACROS
///////////////////////////////////////////////////////////////////////////////

#define FETCH_REG(reg) \
  i2c_read_reg(s->i2c, s->i2c_addr, reg)

#define SET_REG(reg, val) \
  i2c_write_reg(s->i2c, s->i2c_addr, reg, &val, 1)

///////////////////////////////////////////////////////////////////////////////
// PRIVATE INTERFACE
///////////////////////////////////////////////////////////////////////////////

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
int     mpu_selftest      (  Sensor*     mpu,                         // SELFTEST
                             int         print_output );
// Dealloc an mpu sensor struct. See .c for dependency behaviour
// and warnings.
void    mpu_dealloc       (  Sensor**    s  );                        // DEALLOC 

///////////////////////////////////////////////////////////////////////////////
// CONFIG INTERFACE
///////////////////////////////////////////////////////////////////////////////

// Entry for the gyro configuration helpers
uint8_t mpu_config_fifo(Sensor *s, KeyVal * pairs);
uint8_t mpu_config_gyro(Sensor *s, KeyVal * pairs);
uint8_t mpu_config_i2c_mst(Sensor *s, KeyVal * pairs);
uint8_t mpu_config_i2c_slv(Sensor *s, KeyVal * pairs);
uint8_t mpu_config_int_pin(Sensor *s, KeyVal * pairs);
uint8_t mpu_config_power(Sensor *s, KeyVal * pairs);
uint8_t mpu_config_samplerate(Sensor *s, KeyVal *pairs);
uint8_t mpu_config_user_ctrl(Sensor *s, KeyVal * pairs);
void yn_toggle(uint8_t *reg, int bit, char *yn);

#endif
