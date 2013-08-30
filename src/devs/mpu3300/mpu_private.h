///////////////////////////////////////////////////////////////////////////////
// Raspberry Pi GPIO Interface
// ¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯
// File: mpu_private.h
// PA Consulting - Lawrence Jones
///////////////////////////////////////////////////////////////////////////////

#ifndef MPU3300_PRIVATE_HEADER_INC
#define MPU3300_PRIVATE_HEADER_INC

#include "devs/mpu3300.h"
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

uint8_t mpu_config_fifo(Sensor *s, KeyVal * pairs);
uint8_t mpu_config_gyro(Sensor *s, KeyVal * pairs);
uint8_t mpu_config_i2c_mst(Sensor *s, KeyVal * pairs);
uint8_t mpu_config_i2c_slv(Sensor *s, KeyVal * pairs);
uint8_t mpu_config_int_pin(Sensor *s, KeyVal * pairs);
uint8_t mpu_config_power(Sensor *s, KeyVal * pairs);
uint8_t mpu_config_samplerate(Sensor *s, KeyVal *pairs);
uint8_t mpu_config_user_ctrl(Sensor *s, KeyVal * pairs);
void yn_toggle(uint8_t *reg, int bit, char *yn);
char* make_upper(char* str);

#endif
