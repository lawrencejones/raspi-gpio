///////////////////////////////////////////////////////////////////////////////
// Raspberry Pi GPIO Interface
// ¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯
// File: mpu_selftest.c
// PA Consulting - Lawrence Jones
///////////////////////////////////////////////////////////////////////////////

#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <time.h>
#include "mpu_private.h"
#include "mpu_registers.h"

///////////////////////////////////////////////////////////////////////////////
// SELFTEST VERIFICATION
///////////////////////////////////////////////////////////////////////////////

char selftest_conf[] = \
" fsrange:225\
, selftest:x|y|z";


#define CALC_FT(test_reg) (25 * 145.6 * pow(1.046, test_reg - 1));

// Run the build in hardware selftest
int mpu_selftest(Sensor *mpu)
{
  // Activate selftest
  mpu->config(mpu, selftest_conf);
  // Wait for the selftest to kick in
  nanosleep((struct timespec[]) {{0, 500000}}, NULL);
  // Take readings
  Axes *selftest_axes = mpu->read(mpu, HOST);
  // Deactive selftest
  mpu->config(mpu, "selftest:off");
  // Wait for the selftest to shut off
  nanosleep((struct timespec[]) {{0, 500000}}, NULL);  
  // Take readings
  Axes *standard_axes = mpu->read(mpu, HOST);
  // Find selftest_response = (selftest - standard)
  selftest_axes->x -= standard_axes->x;
  selftest_axes->y -= standard_axes->y;
  selftest_axes->z -= standard_axes->z;
  // Calculate change from factory trim
  double ft_x = CALC_FT(i2c_read_reg( mpu->i2c,
                                      mpu->i2c_addr,
                                      MPU_SELF_TEST_X ));
  double ft_y = CALC_FT(i2c_read_reg( mpu->i2c,
                                      mpu->i2c_addr,
                                      MPU_SELF_TEST_Y ));
  double ft_z = CALC_FT(i2c_read_reg( mpu->i2c,
                                      mpu->i2c_addr,
                                      MPU_SELF_TEST_Z ));
  // (selftest_response - ft) / ft
  double x_score = (selftest_axes->x - ft_x) / ft_x;
  double y_score = (selftest_axes->y - ft_y) / ft_y;
  double z_score = (selftest_axes->z - ft_z) / ft_z;
  // Print the scores
  printf("Selftest results are...\n\n");
  printf("%s%.2f\n%s%.2f\n%s%.2f\n\n",
         "  Gyro X: ", x_score,
         "  Gyro Y: ", y_score,
         "  Gyro Z: ", z_score );
  // Free used memory
  free(standard_axes);
  free(selftest_axes);
  // Return success
  return 0;
}
