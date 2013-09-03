///////////////////////////////////////////////////////////////////////////////
// Raspberry Pi GPIO Interface
// ¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯
// File: dev_config.h
// PA Consulting - Lawrence Jones
///////////////////////////////////////////////////////////////////////////////

#include <stdlib.h>
#include <string.h>
#include "devs/shared.h"

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
// ENTRY FUNCTION FOR CONFIG
///////////////////////////////////////////////////////////////////////////////

// Given an array of KeyVal structs, iterate through all pairs and
// if this setting is implemented, then call the appropriate helper.
// Sensor *s is a pointer to the dev that should be configured.
// Returns number of detected register changes made
int dev_config(Sensor *s, char *conf_str, ConfigFunctionMap *map)
{
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
