///////////////////////////////////////////////////////////////////////////////
// Raspberry Pi GPIO Interface
// ¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯
// File: mpu_conf_i2c_slv.c
// PA Consulting - Lawrence Jones
///////////////////////////////////////////////////////////////////////////////

#include "mpu_private.h"
#include "../../../tools/src/macros.h"

///////////////////////////////////////////////////////////////////////////////
// SUB-CONFIGS
///////////////////////////////////////////////////////////////////////////////

static void process_rw_key (uint8_t *reg_val, char *val)
{
  // If the val is read
  if (!strcmp(val, "read"))
  {
    // Set bit 7 to 1
    *reg_val &= (1 << 7);
  }
  // Else if write
  else if (!strcmp(val, "write"))
  {
    // Set bit 7 to 0
    *reg_val &= !(1 << 7);
  }
}

static void process_addr_key (uint8_t *reg_val, char *val)
{
  // Process val to get a number
  int addr = DEX_TO_INT(val);
  // Verify sensible address
  if ((addr < 128) && (addr > 0))
  {
    // Wipe bits 0:6 of reg
    *reg_val &= (1 << 7);
    // Assign value to reg
    *reg_val |= (0x7f & addr);
  }
}

static void process_reg_key (uint8_t *reg_val, char *val)
{
  // Process the val
  int reg = DEX_TO_INT(val);
  // Verify sensible register
  if ((reg < 256) && (reg > 0))
  {
    // Update reg value
    *reg_val = 0xff & reg;
  }
}

static void process_len_key (uint8_t *reg_val, char *val)
{
  // Process the val
  int len = DEX_TO_INT(val);
  // Verify sensible length
  if ((len < 16) && (len > 0))
  {
    // Set bottom bits of reg
    *reg_val = (0xf0 & *reg_val) | (0x0f & len);
  }
}

static void process_en_key (uint8_t *reg_val, char *val)
{
  // Toggle for yes/no
  yn_toggle(reg_val, 7, val);
}

static void process_byte_swap_key (uint8_t *reg_val, char *val)
{
  // Toggle for yes/no
  yn_toggle(reg_val, 6, val);
}

static void process_reg_dis_key (uint8_t *reg_val, char *val)
{
  // Toggle for yes/no
  yn_toggle(reg_val, 5, val);
}

static void process_grp_key (uint8_t *reg_val, char *val)
{
  // Toggle for yes/no
  yn_toggle(reg_val, 4, val);
}

///////////////////////////////////////////////////////////////////////////////
// I2C SLAVE CONFIGURATION
///////////////////////////////////////////////////////////////////////////////

// Configures the i2c slave (0)
//    i2c_slave_rw: (read|write) [r|w] optional
//    i2c_slave_addr: (hex)
//    i2c_slave_reg: (hex)
//    i2c_slave_len: (hex) 4-bit
//    i2c_slave_en: (yes:no)
//    i2c_slave_byte_swap: (yes:no)
//    i2c_slave_reg_dis: (yes:no)
//    i2c_slave_grp: (evenodd|oddeven) [e|o] optional
uint8_t mpu_config_i2c_slv(Sensor *s, KeyVal * pairs)
{
  // Fetch current register value
  uint8_t addr = FETCH_REG(MPU_I2C_SLV0_ADDR),
          _addr = addr,
          reg = FETCH_REG(MPU_I2C_SLV0_REG),
          _reg = reg,
          ctrl = FETCH_REG(MPU_I2C_SLV0_CTRL),
          _ctrl = ctrl;
  // While there are keys left
  do {
    // If not yet applied
    if (!pairs->applied)
    {
      // Initially set applied as true
      pairs->applied = 1;
      // If i2c_slave_rw key
      if (!strcmp(pairs->key, "i2c_slave_rw"))
      {
        // Configure for i2c read and write
        process_rw_key(&_addr, pairs->val);
      }
      // If i2c_slave_addr key
      else if (!strcmp(pairs->key, "i2c_slave_addr"))
      {
        // Configure for the slave address
        process_addr_key(&_addr, pairs->val);
      }
      // If i2c_slave_reg key
      else if (!strcmp(pairs->key, "i2c_slave_reg"))
      {
        // Configure for the slave register
        process_reg_key(&_reg, pairs->val);
      }
      // If i2c_slave_len key
      else if (!strcmp(pairs->key, "i2c_slave_len"))
      {
        // Configure for the number of bytes to transfer
        process_len_key(&_ctrl, pairs->val);
      }
      // If i2c_slave_en key
      else if (!strcmp(pairs->key, "i2c_slave_en"))
      {
        // Configure for i2c slave enable
        process_en_key(&_ctrl, pairs->val);
      }
      // If i2c_slave_byte_swap key
      else if (!strcmp(pairs->key, "i2c_slave_byte_swap"))
      {
        // Configure for byte swapping
        process_byte_swap_key(&_ctrl, pairs->val);
      }
      // If i2c_slave_reg_dis key
      else if (!strcmp(pairs->key, "i2c_slave_reg_dis"))
      {
        // Configure for read or write only
        process_reg_dis_key(&_ctrl, pairs->val);
      }
      // If i2c_slave_grp key
      else if (!strcmp(pairs->key, "i2c_slave_grp"))
      {
        // Configure for grouping order of word pairs
        process_grp_key(&_ctrl, pairs->val);
      }
      // Else unsupported key
      else
      {
        // So set applied to false
        pairs->applied = 0;
      }
    }
  } while ((pairs = pairs->next));
  // Initialise change_made to keep track of change in regs
  int change_made = 0;
  // If the reg values have changed
  if (addr != _addr)
  {
    // Then set register
    SET_REG(MPU_I2C_SLV0_ADDR, _addr);
    // Increment change_made
    change_made++;
  }
  if (reg != _reg)
  {
    // Then set register
    SET_REG(MPU_I2C_SLV0_REG, _reg);
    // Increment change_made
    change_made++;
  }
  if (ctrl != _ctrl)
  {
    // Then set register
    SET_REG(MPU_I2C_SLV0_CTRL, _ctrl);
    // Increment change_made
    change_made++;
  }
  // Return change made value
  return change_made;
}
