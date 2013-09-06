///////////////////////////////////////////////////////////////////////////////
// Raspberry Pi GPIO Interface
// ¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯
// File: dev_config.h
// PA Consulting - Lawrence Jones
///////////////////////////////////////////////////////////////////////////////

#include <stdlib.h>
#include <string.h>
#include "devs/shared/dev_mux.h"
#include "devs/shared/dev_sensor.h"
#include "macros.h"

///////////////////////////////////////////////////////////////////////////////
// HELPERS
///////////////////////////////////////////////////////////////////////////////

// Takes a string, formatted as `key:val,key:val,...`
// and returns a processed keyval pointer.
KeyVal *str_to_keyval(char *_str)
{
  // Make copy of string to ensure mutable
  char *str = malloc(sizeof(char) * strlen(_str) + 1);
  // Copy contents
  strcpy(str, _str);
  // Add termination char
  str[strlen(_str)] = '\0';
  // Declare initial keyval pointer
  KeyVal *pairs = NULL, *tmp;
  // Use strtok to split the string for
  // the key and the tail
  char *key = strtok(str, ": "), *val;
  while (key != NULL)
  { 
    // Split once again for the val and the tail
    val = strtok(NULL, ", ");
    // If val is not null
    if (val != NULL)
    {
      // Create a new keyval
      tmp = keyval_malloc(key, val);
      // Make it's next point to the current head
      tmp->next = pairs;
      // Then make it the new head
      pairs = tmp;
    }
    // Split once again for the key
    key = strtok(NULL, ": ");
  }
  // Free the copied string
  free(str);
  // Return the pairs list
  return pairs;
}

///////////////////////////////////////////////////////////////////////////////
// YES NO CONVERSION
///////////////////////////////////////////////////////////////////////////////

void yn_toggle(uint8_t *reg, int bit, char *yn)
{
  // If the string is yes|on
  if (strstr("yes,on", yn) != NULL)
  {
    // Set the value of the byte to a 1 on the given bit
    *reg |= (1 << bit);
  }
  // Else if a no|off
  else if (strstr("no,off", yn) != NULL)
  {
    // Set the value of the byte to 0 on the given bit
    *reg &= ~(1 << bit);
  }
}

///////////////////////////////////////////////////////////////////////////////
// DEVICE VERIFICATION
///////////////////////////////////////////////////////////////////////////////

int dev_fails_to_respond(i2c_bus *i2c, int i2c_addr, Mux *mux, int mux_channel)
{
  // Verify the i2c bus handle
  if (!i2c)
  {
    // Print error
    ERR("Invalid I2C bus handle.\n\n");
    return DEV_INVALID_HANDLE;
  }
  // If mux exists, then configure for the address test
  if (mux)
  {
    mux->set_channel(mux, mux_channel);
  }
  // Verify the presense of the device on the bus
  if (!i2c_bus_addr_active(i2c, i2c_addr))
  {
    ERR("Device did not respond at address `0x%02x`\n\n", i2c_addr);
    // If device doesn't respond
    return DEV_NOT_RESPOND;
  }
  return 0;
}

///////////////////////////////////////////////////////////////////////////////
// ENTRY FUNCTION FOR CONFIG
///////////////////////////////////////////////////////////////////////////////

// Given an array of KeyVal structs, iterate through all pairs and
// if this setting is implemented, then call the appropriate helper.
// Sensor *s is a pointer to the dev that should be configured.
// Returns number of detected register changes made
int dev_config(Sensor *s, char *conf_str, ConfigFunctionMap *map)
{
  // If there is a multiplexer, configure for access
  if (s->mux)
  {
    // Set the channel
    s->mux->set_channel(s->mux, s->mux_channel);
  }
  // Generate the keyval
  KeyVal *k = str_to_keyval(conf_str),
         *_k = k;
  // Initialise counters
  int applied = 0;
  // While there is another keyval
  while (k) 
  {
    // If the current keyval has not already been applied
    if (!k->applied)
    {
      // Declare a config helper function pointer
      Configs handle = NULL;
      // Counter for the config map
      int j = 0;
      // Iterate through the map while there are valid keys
      // and a config helper has not been found/called
      do {
        // If the current key is present in the keys
        if (strstr(map[j].keys, k->key) != NULL)
        {
          // Assign the handle value
          handle = map[j].helper;
          // Call the handle function and add to applied
          applied += handle(s, k);
        }
      } while (!handle && map[++j].keys);
    }
    // Move to next k
    k = k->next;
  }
  // Dealloc the keyval
  keyval_dealloc(&_k);
  return applied;
}
