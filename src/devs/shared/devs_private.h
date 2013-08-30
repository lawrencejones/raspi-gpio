///////////////////////////////////////////////////////////////////////////////
// Raspberry Pi GPIO Interface
// ¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯
// File: devs_private.h
// PA Consulting - Lawrence Jones
///////////////////////////////////////////////////////////////////////////////

#ifndef I2C_DEV_HEADER_INC
#define I2C_DEV_HEADER_INC

#include "devs.h"
#include "../../../tools/src/keyval.h"
#include "../../../tools/src/macros.h"

///////////////////////////////////////////////////////////////////////////////
// TYPEDEFS
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
// DEVS PRIVATE METHOD STUBS
///////////////////////////////////////////////////////////////////////////////

// Print to stdout a muxnetwork
void mux_network_print(Mux *m);
// Generate keyvals from a string
KeyVal *str_to_keyval(char *str);

#endif
