///////////////////////////////////////////////////////////////////////////////
// Raspberry Pi GPIO Interface
// ¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯
// File: devs_config.h
// PA Consulting - Lawrence Jones
///////////////////////////////////////////////////////////////////////////////

#include "devs_private.h"

///////////////////////////////////////////////////////////////////////////////
// HELPERS
///////////////////////////////////////////////////////////////////////////////

// Takes a string, formatted as `key:val,key:val,...`
// and returns a processed keyval pointer.
KeyVal *str_to_keyval(char *str)
{
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
  // Return the pairs list
  return pairs;
}
