///////////////////////////////////////////////////////////////////////////////
// Raspberry Pi GPIO Interface
// ¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯
// File: itg_conf_sync.c
// PA Consulting - Lawrence Jones
///////////////////////////////////////////////////////////////////////////////

#include <stdlib.h>
#include <string.h>
#include "itg_private.h"
#include "itg_registers.h"

///////////////////////////////////////////////////////////////////////////////
// SUB-CONFIGS
///////////////////////////////////////////////////////////////////////////////

static void process_samplerate_key(uint8_t *reg_val, uint8_t dlpf, char *val)
{
  // Initialise required vars
  int outr = 1000, samplerate = 0;
  // If dlpf is disabled
  if ((dlpf == 0) || (dlpf == 7))
  {
    outr = 8000;
  }
  // Extract sample rate from config pair
  // atoi will stop processing the number at any non digit
  samplerate = atoi(val);
  // Calculate the new sdiv (reg) value
  *reg_val = outr/samplerate - 1;
}

static void process_fs_sel_key(uint8_t *reg_val, char *val)
{
  // Extract the number from the value
  int fs = atoi(val), setting = 0;
  // Switch on the case
  switch (fs)
  {
    case 2000:
      setting++;
    case 1000:
      setting++;
    case 500:
      setting++;
  }
  *reg_val &= 0xe7;
  *reg_val |= ((0x03 & setting) << 3);
}

static void process_dlpf_config_key(uint8_t *reg_val, char *val)
{
  // Extract val of dlpf
  int dlpf = atoi(val);
  // 0 previous dlpf
  *reg_val &= 0xf8;
  // Set new
  *reg_val |= (0x07 & dlpf);
}

///////////////////////////////////////////////////////////////////////////////
// SAMPLERATE CONFIGURATION
///////////////////////////////////////////////////////////////////////////////

// Configures the sample rate of the sensor //////////////
//    samplerate: Nhz
//    fs_sel: (250|500|1000|2000)
//    dlpf_config: N
uint8_t itg_config_sync(Sensor *s, KeyVal *pairs)
{
  // Fetch the current reg value
  uint8_t smplrt_div = FETCH_REG(ITG_SMPLRT_DIV),
          _smplrt_div = smplrt_div,
          sync_reg = FETCH_REG(ITG_SYNC_SET),
          _sync_reg = sync_reg;
  do {
    // If not already applied
    if (!pairs->applied)
    {
      // Set applied to true initially
      pairs->applied = 1;
      // If the config is a samplerate key
      if (!strcmp(pairs->key, "samplerate"))
      {
        // Fetch the current digital low pass filter setting
        uint8_t dlpf = FETCH_REG(ITG_SYNC_SET) & 0x7u;
        // Configure for sample rate
        process_samplerate_key(&_smplrt_div, dlpf, pairs->val);
      }
      // Else if fs_sel
      else if (!strcmp(pairs->key, "fs_sel"))
      {
        // Configure for full scale range
        process_fs_sel_key(&_sync_reg, pairs->val);
      }
      // Else if dlpf_config
      else if (!strcmp(pairs->key, "dlpf_config"))
      {
        // Configure for the digital low pass filter
        process_dlpf_config_key(&_sync_reg, pairs->val);
      }
      // Else not supported key
      else
      {
        // Set applied to false
        pairs->applied = 0;
      }
    }
  } while ((pairs = pairs->next));
  int applied = 0;
  // If the register is not already correct
  if (smplrt_div != _smplrt_div)
  {
    // The set it's new value
    SET_REG(ITG_SMPLRT_DIV, _smplrt_div);
    // Increment applied
    applied++;
  }
  // If reg changed
  if (sync_reg != _sync_reg)
  {
    // Set new value
    SET_REG(ITG_SYNC_SET, _sync_reg);
    // Increment applied
    applied++;
  }
  // Return applied
  return applied;
}
