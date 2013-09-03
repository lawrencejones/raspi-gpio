///////////////////////////////////////////////////////////////////////////////
// Raspberry Pi GPIO Interface
// ¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯
// File: shared.h
// PA Consulting - Lawrence Jones
///////////////////////////////////////////////////////////////////////////////

#ifndef DEV_SHARED_HEADER_INC
#define DEV_SHARED_HEADER_INC

#include <stdint.h>
#include "dev.h"
#include "keyval.h"

///////////////////////////////////////////////////////////////////////////////
// TYPEDEF
///////////////////////////////////////////////////////////////////////////////

// Function type for configuration helpers
typedef uint8_t       (*Configs)(Sensor *s, KeyVal *pairs);

///////////////////////////////////////////////////////////////////////////////
// STRUCTS
///////////////////////////////////////////////////////////////////////////////

// Define a struct to map config keys to functions
struct ConfigFunctionMap {
  // String of keys
  char* keys;
  // Function pointer to a helper
  Configs helper;
};

///////////////////////////////////////////////////////////////////////////////
// FUNCTION STUBS
///////////////////////////////////////////////////////////////////////////////

// Generate keyvals from a string
KeyVal *str_to_keyval(char *str);
// General configuration function for sensors
int dev_config(Sensor *s, char *conf_str, ConfigFunctionMap *map);

#endif