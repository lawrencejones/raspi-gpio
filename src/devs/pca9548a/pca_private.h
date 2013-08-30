///////////////////////////////////////////////////////////////////////////////
// Raspberry Pi GPIO Interface
// ¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯
// File: pca_private.c
// PA Consulting - Lawrence Jones
///////////////////////////////////////////////////////////////////////////////

#ifndef PCA9548A_PRIVATE_HEADER_INC
#define PCA9548A_PRIVATE_HEADER_INC

#include "../pca9548a.h"

///////////////////////////////////////////////////////////////////////////////
// PCA PRIVATE INTERFACE
///////////////////////////////////////////////////////////////////////////////

// Verifies valid pointer and i2c handle
void verify_mux(Mux *m);
// Filter all devs in bs from as
void i2c_dev_filter(i2c_dev **as, i2c_dev *bs);
// Returns 1 if node is present in list
int i2c_devs_contains(i2c_dev *list, i2c_dev *node);
// Prints to stdout the mux network
void mux_network_print(Mux *m);

#endif
