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
#include "macros.h"

///////////////////////////////////////////////////////////////////////////////
// SELFTEST VERIFICATION
///////////////////////////////////////////////////////////////////////////////

char selftest_conf[] = \
" fsrange:225\
, selftest:x|y|z";


#define CALC_FT(test_reg) (25 * 145.6 * pow(1.046, test_reg - 1));

// Run the build in hardware selftest, return a number representing
// the health of each sensor in binary. So the int will be ZYX when
// in binary, where 1's will represent sensor error beyond normal range.
int mpu_selftest(Sensor *mpu, int print)
{
  // Activate selftest
  mpu->config(mpu, selftest_conf);
  // Wait for the selftest to kick in
  nanosleep((struct timespec[]) {{0, 500000}}, NULL);

  // Take readings
  Axes *selftest_axes = mpu->read(mpu, FIFO);
  // Calculate average for the selftest results
  Axes *selftest_average = axes_average(selftest_axes);
  // Dealloc the selftest_axes
  axes_dealloc(&selftest_axes);

  // Deactive selftest
  mpu->config(mpu, "selftest:off");
  // Wait for the selftest to shut off
  nanosleep((struct timespec[]) {{0, 500000}}, NULL);

  // Take readings
  Axes *standard_axes = mpu->read(mpu, FIFO);
  // Calculate average for each axis
  Axes *standard_average = axes_average(standard_axes);
  // Dealloc the standard_axes
  axes_dealloc(&standard_axes);
  // Find selftest_response = (selftest - standard)
  selftest_average->x -= standard_average->x;
  selftest_average->y -= standard_average->y;
  selftest_average->z -= standard_average->z;
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
  double scores[3] = {
    (selftest_average->x - ft_x) / ft_x,
    (selftest_average->y - ft_y) / ft_y,
    (selftest_average->z - ft_z) / ft_z,
  };
  // If print is yes
  if (print)
  {
    printf("Selftest results are...\n\n");
    printf("  Axes X: %.4f%%\n",   scores[0]);
    printf("  Axes Y: %.4f%%\n",   scores[1]);
    printf("  Axes Z: %.4f%%\n\n", scores[2]);
  }
  // Initialise value that will represent what sensors are ok
  int health = 0;
  // Check in good range
  for (int i = 0; i < 3; i++)
  {
    if (scores[i] > 14 || scores[i] < -14)
    {
      health |= (1 << i);
    }
  }
  // If failed and print
  if (health && print)
  {
    ERR("Selftest has failed. -14 < tval < 14 is acceptable range.\n\n");
  }
  // Free used memory
  axes_dealloc(&standard_average);
  axes_dealloc(&selftest_average);
  // Return health
  return health;
}
