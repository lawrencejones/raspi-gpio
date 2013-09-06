///////////////////////////////////////////////////////////////////////////////
// Raspberry Pi GPIO Interface
// ¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯
// File: itg_private.h
// PA Consulting - Lawrence Jones
///////////////////////////////////////////////////////////////////////////////

#ifndef ITG3050_PRIVATE_HEADER_INC
#define ITG3050_PRIVATE_HEADER_INC

#include "devs/itg3050.h"
#include "devs/shared/dev_sensor.h"

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

// Set the itg to the default config
void    itg_reset         (  Sensor      *s  );                       // RESET
// Read from the given sensor with the given target
Axes    *itg_read         (  Sensor*     s,                           // READ
                             target_t    t  );
// Configure the given itg struct pointer using the keyval array
int     itg_config        (  Sensor*     s,                           // CONFIG
                             char*       conf_str  );
// Dealloc an itg sensor struct. See .c for dependency behaviour
// and warnings.
void    itg_dealloc       (  Sensor**    s  );                        // DEALLOC 

///////////////////////////////////////////////////////////////////////////////
// CONFIG INTERFACE
///////////////////////////////////////////////////////////////////////////////

// Entry for the gyro configuration helpers
uint8_t itg_config_aux(Sensor *s, KeyVal * pairs);
uint8_t itg_config_fifo(Sensor *s, KeyVal * pairs);
uint8_t itg_config_gyro_offset(Sensor *s, KeyVal * pairs);
uint8_t itg_config_i2c(Sensor *s, KeyVal * pairs);
uint8_t itg_config_int_pin(Sensor *s, KeyVal *pairs);
uint8_t itg_config_sync(Sensor *s, KeyVal *pairs);
uint8_t itg_config_user_ctrl(Sensor *s, KeyVal *pairs);
void yn_toggle(uint8_t *reg, int bit, char *yn);

#endif
