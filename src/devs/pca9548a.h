///////////////////////////////////////////////////////////////////////////////
// Raspberry Pi GPIO Interface
// ¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯
// File: pca9548a.h
// PA Consulting - Lawrence Jones
///////////////////////////////////////////////////////////////////////////////

#ifndef PCA9548A_HEADER_INC
#define PCA9548A_HEADER_INC

#include "shared/devs_mux.h"

///////////////////////////////////////////////////////////////////////////////
// DEFINE CHANNEL CODES
///////////////////////////////////////////////////////////////////////////////

// Defines the channel enable codes
// To generate a channel code, or the channels you desire
// to become active. The pca supports multiple channels
// simulataneously.
// Ex - { Mux *m->set_channel(m, PCA_C0|PCA_C2) } for channel 0+2
// PCA_CD is ChannelDisabled
#define PCA_CD 0x00
#define PCA_C0 0x01
#define PCA_C1 0x02
#define PCA_C2 0x04
#define PCA_C3 0x08

///////////////////////////////////////////////////////////////////////////////
// INTERFACE - FUNCTION STUBS
///////////////////////////////////////////////////////////////////////////////

// Malloc and initialise a mux struct, return the pointer
Mux         *pca_init            (  char*     name,
                                    short     i2c_addr,
                                    i2c_bus   *i2c  );
// Fetches the current channel code (raw reg val) and updates
// the channel field inside the mux struct
uint8_t     pca_fetch_channel    (  Mux *m  );
// Sets the mux channel value with a given short
// Also runs a write test to verify success (updates channel field)
void        pca_set_channel      (  Mux     *m, 
                                    uint8_t c  );
// Generates a mux network from all the devices visible on the buses
MuxNetwork  *pca_get_devs        (  Mux *m  );
// Dealloc an pca mux struct, watch for any memory leaks
void        pca_dealloc          (  Mux** m  );

#endif
