///////////////////////////////////////////////////////////////////////////////
// Raspberry Pi GPIO Interface
// ¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯
// File: imu_mpu_test.c
// PA Consulting - Lawrence Jones
///////////////////////////////////////////////////////////////////////////////

#include "i2c.h"
#include "imu.h"
#include "devs/mpu3300.h"
#include "devs/pca9548a.h"

///////////////////////////////////////////////////////////////////////////////
// MPU TESTING
///////////////////////////////////////////////////////////////////////////////

int imu_mpu_test(int bus, short addr, char *config_str)
{
  // Prime i2c access /////////////////////////////////////////////////////////
  // Initialise i2c protocol for bus 1
  // Assign i2c handle
  i2c_bus *i2c = i2c_init(1);
  
  // Prime the multiplexer with address 116 to channel 0 //////////////////////
  // Init the i2c multiplexer
  Mux *m = pca_init("MUX1", 116, i2c);
  // Select channel 0
  m->set_channel(m, PCA_C0);


  // Malloc and prime the mux with default settings ///////////////////////////
  Sensor *mpu = mpu_init("MPU", i2c, 0x69, config_str);

  // Attempt a read and print results /////////////////////////////////////////
  Axes *ax = mpu_read(mpu, HOST);
  printf("The axes readings are...\n\n");
  printf("  X : %d\n  Y : %d\n  Z : %d\n\n", 
    (short)ax->x, (short)ax->y, (int)ax->z);
  free(ax);

  // Free all used memory /////////////////////////////////////////////////////
  // Dealloc the mpu struct
  mpu_dealloc(&mpu);
  // Dealloc the mux struct
  pca_dealloc(&m);
  return 0;
}
