///////////////////////////////////////////////////////////////////////////////
// Raspberry Pi GPIO Interface
// ¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯
// File: mpu3300.c
// PA Consulting - Lawrence Jones
///////////////////////////////////////////////////////////////////////////////

#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>
#include "mpu3300.h"
#include "i2c.h"
#include "../../tools/src/macros.h"

static char* make_upper(char* str);

///////////////////////////////////////////////////////////////////////////////
// INITIALISATION
///////////////////////////////////////////////////////////////////////////////
/*
   The mpu3300 is a 3-axis gyro, with an onboard i2c master for interfacing
   with an auxiliary accelerometer (or other sensor).
   The implementation below assumes i2c as the connection interface
   and makes use of an i2c library from github.com/LawrenceJones/raspi-gpio.

   The process of using an mpu goes as follows...

     1. Enable the chip.
     2. Configure gyro settings.
     3. Configure auxiliary sensor settings (opt)
     4. Read data from sensor (repeat)
     5. Disable the chip.

   This process breaks down naturally into different actions. In
   this implementation, they are...

     Allocate and initialise mpu details such as name, addr, etc.
     ============================================================
     Toggle sensor power mode
     Configure specific gyro settings
     Configure auxiliary sensor settings
     Read data from sensor

   The allocation of memory and initialisation return a struct of type
   Sensor which has all the required function pointers for mpu config
   and data reading.
 */

// Given name and i2c address, allocates and initialises a mpu
// struct with the given config options. `config` represents
// an array of KeyVal pairs which correspond to mpu configuration
// options. Supported config options are listed in config array.
Sensor *mpu_init(char*   name, 
                 i2c_bus* i2c,
                 int     i2c_addr,
                 KeyVal* config)
{
  // malloc the sensor struct
  Sensor *s = malloc(sizeof(Sensor));
  // Verify malloc success
  if (!s)
  {
    // malloc has failed, print error and exit
    ERR("Failed to allocate memory (malloc) for Sensor *s.\n");
    exit(EXIT_FAILURE);
  }
  // Assign given sensor properties
  s->name = malloc(sizeof(char) * strlen(name) + 1);
  // Make copies of strings to avoid stack expiry
  strcpy(s->name, name);
  // Assign pointer to the i2c bus
  s->i2c = i2c;
  // Assign the i2c addr
  s->i2c_addr = i2c_addr;
  // Assign the enum for model
  s->model = MPU3300;
  // Assign enum for sensor type
  s->type = GYRO;
  // Assign the function pointer for an enable method
  s->enable = &mpu_enable;
  // Assign the read function
  s->read = &mpu_read;
  // Assign the prep function
  // s->prep = &prep_board;
  return NULL;
}

///////////////////////////////////////////////////////////////////////////////
// CONFIGURE GYRO SETTINGS
///////////////////////////////////////////////////////////////////////////////
/*
   Configuring settings is handled using KeyVal pairs, where these
   are structs containing a setting key and desired value.

   The `mpu_configure` function is the only entry point for setting
   configuration values. All the other functions are (and for future
   changes, should) be declared as static and therefore private to
   this file.
 */

#define FETCH_REG(reg) \
  i2c_read_reg(s->i2c, s->i2c_addr, reg)

#define SET_REG(reg, val) \
  i2c_write_reg(s->i2c, s->i2c_addr, reg, &val, 1)

//////////////////////////////////////////////////////////
// Configures the sample rate of the sensor //////////////
//    samplerate: Nhz
static uint8_t mpu_config_samplerate(Sensor *s, KeyVal *pairs)
{
  // Fetch the current reg value
  uint8_t byte = FETCH_REG(MPU_SMPLRT_DIV);
  // If the config is a samplerate key
  if (!strcmp(pairs->key, "samplerate"))
  {
    // Fetch the current digital low pass filter setting
    uint8_t dlpf = FETCH_REG(MPU_CONFIG) & 0x7u,
            sdiv = 0;
    int outr = 1000, samplerate;
    // If dlpf is disabled
    if ((dlpf == 0) || (dlpf == 7))
    {
      outr = 8000;
    }
    // Extract sample rate from config pair
    // atoi will stop processing the number at any non digit
    samplerate = atoi(pairs->val);
    // Calculate the new sdiv value
    sdiv = outr/samplerate - 1;
    // If the register is not already correct
    if (sdiv != byte)
    {
      // The set it's new value
      SET_REG(MPU_SMPLRT_DIV, sdiv);
    }
  }
  return 0;
}

//////////////////////////////////////////////////////////
// Configures gyro settings //////////////////////////////
//    selftest: (x|y|z|off)
//    fs_range:(225:450)
static uint8_t mpu_config_gyro(Sensor *s, KeyVal * pairs)
{
  // Fetch the current reg value
  uint8_t byte = FETCH_REG(MPU_GYRO_CONFIG);
  // If configuring the selftest properties
  if (!strcmp(pairs->key, "selftest"))
  {
    // Make all uppercase to avoid case errors
    char *val = make_upper(pairs->val);
    // Find if the value contains 
    short off = (strstr(val, "off") != NULL),
          x = (strchr(val, 'X') != NULL) & !off,
          y = (strchr(val, 'Y') != NULL) & !off,
          z = (strchr(val, 'Z') != NULL) & !off;
    // Make new reg value from initial byte
    uint8_t reg_val = byte;
    // Assign new x y z values
    reg_val |= (x << 7) | (y << 6) | (z << 5);
    // If reg value has changed, update it
    if (reg_val != byte)
    {
      SET_REG(MPU_GYRO_CONFIG, reg_val);
    }
    // Free the malloced string
    free(val);
  }
  // If configuring full scale range
  if (!strcmp(pairs->key, "fs_range"))
  {
    // Make a copy of the initial byte
    uint8_t reg_val = byte;
    // Extract new fs_range value
    int fs = atoi(pairs->val);
    // Verify one of allowed options
    if (fs == 225)
    {
      // Make bits 3 & 4 = 0
      reg_val &= 0xe7u;
    }
    else if (fs == 450)
    {
      // Wipe bits 3 & 4
      reg_val &= 0xe7u;
      // Set bit 3 to 1
      reg_val |= 0x08u;
    }
    // If the byte has changed then update it
    if (byte != reg_val)
    {
      SET_REG(MPU_GYRO_CONFIG, reg_val);
    }
  }
  return 0;
}

//////////////////////////////////////////////////////////
// Configures the fifo settings //////////////////////////
//    fifo_selection: (tmp|xg|yg|zg|xa|ya|za)
static uint8_t mpu_config_fifo(Sensor *s, KeyVal * pairs)
{
  // Fetch the current register value
  uint8_t byte = FETCH_REG(MPU_FIFO_EN);
  // If the fifo_selection key
  if (!strcmp(pairs->key, "fifo_selection"))
  {
    // Setup default fifo selection of none
    uint8_t reg_val = 0;
    // Make upper case to avoid case sensitivity
    char *val = make_upper(pairs->val);
    // Find if the config value contains
    short tmp = (strstr(val, "TMP") != NULL) << 7,
          xg  = (strstr(val, "XG")  != NULL) << 6,
          yg  = (strstr(val, "YG")  != NULL) << 5,
          zg  = (strstr(val, "ZG")  != NULL) << 4,
          xa  = (strstr(val, "XA")  != NULL) << 2,
          ya  = (strstr(val, "YA")  != NULL) << 1,
          za  = (strstr(val, "ZA")  != NULL) << 0;
    // Generate the resulting byte
    reg_val = tmp | xg | yg | zg | xa | ya | za;
    // If the new reg val is different from the current
    if (reg_val != byte)
    {
      // Set the register value to reg_val
      SET_REG(MPU_FIFO_EN, reg_val);
    }
    // Free the malloced string
    free(val);
  }
  return 0;
}

//////////////////////////////////////////////////////////
// Configures the user settings //////////////////////////
//    i2c_mst_en: (yes:no)
//    fifo: (on:off)
static uint8_t mpu_config_user_ctrl(Sensor *s, KeyVal * pairs)
{
  // Fetch the current register value
  uint8_t byte = FETCH_REG(MPU_USER_CTRL);
  // If the key is i2c_mst_en
  if (!strcmp(pairs->key, "i2c_mst_en"))
  {
    // Make a copy of the original byte
    uint8_t reg_val = byte;
    // If the value is a yes
    if (strstr(pairs->val, "yes") != NULL)
    {
      // Make bit 5 equal 1
      reg_val |= (1 << 5);
    }
    // Else if the value is a no
    else if (strstr(pairs->val, "no") != NULL)
    {
      // Make bit 5 equal to 0
      reg_val &= 0xdf;
    }
    // If the register value differs
    if (reg_val != byte)
    {
      // Then set the register to new value
      SET_REG(MPU_USER_CTRL, reg_val);
    }
  }
  // If the key is fifo
  if (!strcmp(pairs->key, "fifo"))
  {
    // Make a copy of the original reg value
    uint8_t reg_val = byte;
    // If the value is on
    if (strstr(pairs->val, "on") != NULL)
    {
      // Make bit 6 equal 1
      reg_val |= (1 << 6);
    }
    // Else if the value is off
    else if (strstr(pairs->val, "off") != NULL)
    {
      // Make bit 6 equal 0
      reg_val &= 0xbf;
    }
    // If reg value differs
    if (reg_val != byte)
    {
      // Set reg value
      SET_REG(MPU_USER_CTRL, reg_val);
    }
  }
  return 0;
}

//////////////////////////////////////////////////////////
// Configures the mpu power management ///////////////////
//    sleep: (on:off)
//    temp_en: (yes:no)
//    gyro_sleep: (x|y|z|none)
static uint8_t mpu_config_power(Sensor *s, KeyVal * pairs)
{
  // If the key is sleep
  if (!strcmp(pairs->key, "sleep"))
  {
    // Get copy of current register state
    uint8_t crrt = FETCH_REG(MPU_POWER_MGMT_1),
            reg_val = crrt;
    // If value is on
    if (strstr(pairs->val, "on") != NULL)
    {
      // Set bit 6 to 1
      reg_val |= (1 << 6);
    }
    // Else if value is off
    else if (strstr(pairs->val, "off") != NULL)
    {
      // Set bit 6 to 0
      reg_val &= 0xbf;
    }
    // If the reg val has changed
    if (reg_val != crrt)
    {
      // The set the register
      SET_REG(MPU_POWER_MGMT_1, reg_val);
    }
  }
  // If key is temp_em
  if (!strcmp(pairs->key, "temp_en"))
  {
    // Get copy of the register
    uint8_t crrt = FETCH_REG(MPU_POWER_MGMT_1),
            reg_val = crrt;
    // If the value is yes
    if (strstr(pairs->val, "yes") != NULL)
    {
      // Set bit 3 to 0
      reg_val &= 0xf7;
    }
    // Else if the value is no
    else if (strstr(pairs->val, "no") != NULL)
    {
      // Set bit 3 to 1
      reg_val |= (1 << 3);
    }
    // If the register value has changed
    if (reg_val != crrt)
    {
      // Set the new value
      SET_REG(MPU_POWER_MGMT_1, reg_val);
    }
  }
  // If the key is gyro_sleep
  if (!strcmp(pairs->key, "gyro_sleep"))
  {
    // Get copy of current reg
    uint8_t crrt = FETCH_REG(MPU_POWER_MGMT_2),
            reg_val = 0;
    // Make val all uppercase
    char *val = make_upper(pairs->val);
    short none = (strstr(val, "NONE") != NULL),
          x = (strchr(val, 'X')) & !none,
          y = (strchr(val, 'Y')) & !none,
          z = (strchr(val, 'Z')) & !none;
    // Compute new reg value
    reg_val = (x << 2) | (y << 1) | (z << 0);
    // If the new reg value is different
    if (reg_val != crrt)
    {
      // Set the new value
      SET_REG(MPU_POWER_MGMT_2, reg_val);
    }
  }
  return 0;
}

///////////////////////////////////////////////////////////////////////////////
// CONFIGURE AUX SENSOR SETTINGS
///////////////////////////////////////////////////////////////////////////////
/*
   All i2c slave related configuration functionality should appear
   below. All should be static and only accessed through the mpu_configure
   function.
*/

//////////////////////////////////////////////////////////
// Configures the i2c master /////////////////////////////
//    multi_mst_en: (yes:no)
//    wait_for_aux: (yes:no)
//    i2c_mst_clk: (0..15)
static uint8_t mpu_config_i2c_mst(Sensor *s, KeyVal * pairs)
{
  
  return 0;
}

//////////////////////////////////////////////////////////
// Configures the i2c slave (0) //////////////////////////
//    i2c_slave_rw: (read|write) [r|w] optional
//    i2c_slave_addr: (hex)
//    i2c_slave_reg: (hex)
//    i2c_slave_len: (hex) 4-bit
//    i2c_slave_en: (yes:no)
//    i2c_slave_byte_swap: (yes:no)
//    i2c_slave_reg_dis: (yes:no)
//    i2c_slave_grp: (evenodd|oddeven) [e|o] optional
static uint8_t mpu_config_i2c_slv(Sensor *s, KeyVal * pairs)
{
  return 0;
}

//////////////////////////////////////////////////////////
// Configure the interupt pin and i2c bus ////////////////
//    i2c_bypass: (on:off)
static uint8_t mpu_config_int_pin(Sensor *s, KeyVal * pairs)
{
  return 0;
}


///////////////////////////////////////////////////////////////////////////////
// ENTRY FUNCTION FOR CONFIG
///////////////////////////////////////////////////////////////////////////////

// `map` defines the link between keys and their config helper
// functions.
static ConfigFunctionMap map[] = {
  {
    "samplerate", 
    &mpu_config_samplerate
  },
  {
    "selftest,fsrange",
    &mpu_config_gyro
  },
  {
    "fifo_selection",
    &mpu_config_fifo
  },
  {
    "i2c_mst_en,fifo",
    &mpu_config_user_ctrl
  },
  {
    "sleep,temp_en,gyro_sleep",
    &mpu_config_power
  },
  {
    "multi_mst_en,wait_for_aux,i2c_mst_clk",
    &mpu_config_i2c_mst
  },
  {
    "i2c_slave_rw,i2c_slave_addr,i2c_slave_reg,i2c_slave_len, \
     i2c_slave_en,i2c_slave_byte_swap,i2c_slave_reg_dis,i2c_slave_grp",
    &mpu_config_i2c_slv
  },
  {
    "i2c_bypass",
    &mpu_config_int_pin
  },
  { // Termination element
    NULL, NULL
  }
};

// Given an array of KeyVal structs, iterate through all pairs and
// if this setting is implemented, then modify the mpu settings.
// Sensor *s is a pointer to the mpu that should be configured.
// Returns the number of valid config pairs
int mpu_configure(Sensor *s, KeyVal *settings)
{
  // Initialise counters
  int i = 0, applied = 0;
  // Create entry keyval pointer
  KeyVal *k = &((KeyVal){NULL,NULL,settings});
  // While there is another keyval
  while ((k = k->next))
  { 
    i = 0;
    // Iterate through the map
    do {
      // If the current key is present in the keys
      if (strstr(map[i].keys, k->key))
      {
        // Then invoke the helper function
        map[i].helper(s, k);
        // And increment number of applied settings
        applied++;
      } // Repeat this while there are valid keys
    } while (map[++i].keys);
  }
  return applied;
}

///////////////////////////////////////////////////////////////////////////////
// MPU ON/OFF TOGGLE
///////////////////////////////////////////////////////////////////////////////
/*
   For ease of use, two functions are provided to enable and disable
   the mpu sensor using sensible defaults and/or the configuration
   provided in KeyVal pairs
*/

// Turns the mpu on and deals with sensible defaults
int mpu_enable(Sensor *s, KeyVal *settings)
{
  return 0;
}

// Disables the mpu sensor
int mpu_disable(Sensor *s)
{
  return 0;
}

///////////////////////////////////////////////////////////////////////////////
// READ SENSOR DATA
///////////////////////////////////////////////////////////////////////////////
/*
   Only one entry point for the reading of data. The function
   `mpu_read` conforms to function type `ReadAxes` and takes the
   mpu sensor pointer along with a target for reading.

   Currently possible targets are `HOST` or `AUX`, standing
   for the native gyro axes and aux sensor respectively.
*/

// Retrieves only the mpu's gyro axes data, then returns
// the data as an Axes struct pointer.
Axes *read_gyro(Sensor *s)
{
  return NULL;
}

// Retrieves only the aux sensor's axes data, then returns
// the data as an Axes struct pointer.
Axes *read_aux(Sensor *s)
{
  return NULL;
}

// Entry point for all reads. Takes a Sensor pointer (the selected mpu)
// and a target. This is currently either `HOST` or `AUX`, as explained
// above.
Axes *mpu_read(Sensor *s, target_t t)
{
  return NULL;
}

///////////////////////////////////////////////////////////////////////////////
// DEALLOC MPU
///////////////////////////////////////////////////////////////////////////////
/*
   After consideration, the current implementation will both reset
   and disable an mpu when the struct memory is released.
   All memory releases should be achieved through the below `mpu_dealloc`
   function to avoid the user missing other dependencies.
  
   IMPORTANT - Any auxiliary sensors connected to the mpu will
               also have their respective dealloc function called
               should the i2c_bypass setting not be in use.
               Once again, to avoid user errors where aux sensors
               are left enabled but unaccessable.
*/

void mpu_dealloc(Sensor **s)
{
}

///////////////////////////////////////////////////////////////////////////////
// HELPERS
///////////////////////////////////////////////////////////////////////////////

// Helper function to uppercase a string
static char* make_upper(char* str)
{
  // malloc result
  char *res = malloc(sizeof(char) * strlen(str) + 1);
  // Copy the string
  strcpy(res, str);
  // Uppercase it
  int i = 0;
  // While not end of string
  while (val[i])
  {
    // Uppercase the string
    val[i] = toupper(val[i]);
    i++;
  }
  // Return the uppercased result
  return res;
}

