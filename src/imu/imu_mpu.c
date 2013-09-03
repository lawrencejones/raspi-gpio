///////////////////////////////////////////////////////////////////////////////
// Raspberry Pi GPIO Interface
// ¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯
// File: imu_mpu.c
// PA Consulting - Lawrence Jones
///////////////////////////////////////////////////////////////////////////////

#include <stdlib.h>
#include <stdio.h>
#include "imu_private.h"
#include "devs/mpu3300.h"
#include "devs/pca9548a.h"
#include "macros.h"

///////////////////////////////////////////////////////////////////////////////
// MPU READ AXES
///////////////////////////////////////////////////////////////////////////////

static int imu_mpu_read_axes(Sensor *mpu)
{
  // Attempt a read and print results
  Axes *ax = mpu->read(mpu, HOST);
  printf("The axes readings are...\n\n");
  printf("  X : %d\n  Y : %d\n  Z : %d\n\n", 
    (int)ax->x, (int)ax->y, (int)ax->z);
  free(ax);
  // Return success
  return 0;
}

///////////////////////////////////////////////////////////////////////////////
// MPU READ BURST
///////////////////////////////////////////////////////////////////////////////

static int imu_mpu_read_burst(Sensor *mpu)
{
  // Attempt a burst read and print results
  Axes *ax = mpu->read(mpu, FIFO);
  printf("The axes readings are...\n\n");
  printf  ("       |    X    |    Y    |    Z    |\n");
  int count = 0;
  while (ax)
  {
    printf(" %3d - | %7d | %7d | %7d |\n", 
      count++, (int)ax->x, (int)ax->y, (int)ax->z);
    // Increment ax
    ax = ax->next;
  }
  // Print final clearing line
  printf("\n");
  // Return success
  return 0;
}

///////////////////////////////////////////////////////////////////////////////
// MPU CONFIG
///////////////////////////////////////////////////////////////////////////////

static int imu_mpu_config(Sensor *mpu, char* config_str)
{
  // Attempt to configure the axis
  int res = mpu->config(mpu, config_str);
  // Print how many registers altered
  printf("Altered %d registers.\n\n", res);
  // Return success
  return 0;
}

///////////////////////////////////////////////////////////////////////////////
// HELPERS
///////////////////////////////////////////////////////////////////////////////

static int parse_path ( char *token, 
                        int  *mux_addr, 
                        int  *mux_chan, 
                        int  *mpu_addr )
{
  // Create a temporary var for a copied token
  char *_token = malloc(sizeof(char) * strlen(token) + 1);
  // Declare slash count variable for verification
  int slash_count = 0;
  // For all of tokens characters
  for (int i = 0; i < strlen(token); i++)
  {
    // Copy character into temporary token
    _token[i] = token[i];
    // If slash detected
    if (_token[i] == '/')
    {
      // Increment slash count
      slash_count++;
    }
  }
  _token[strlen(token)] = '\0';
  if (slash_count != 2)
  {
    // Print error
    ERR("%s%s", "Invalid format of path.",
        "\nCorrect usage: MUX/CHANNEL/DEV\n\n");
    // Return error
    return 1;
  }
  // Extract address values
  *mux_addr = strtoul(strtok(_token, "/"), NULL, 16);
  *mux_chan = atoi(strtok(NULL, "/"));
  *mpu_addr = strtoul(strtok(NULL, " "), NULL, 16);
  // Verify sensible values 
  if (*mux_addr > 127 || *mux_addr < 0)
  {
    // Print error
    ERR("The mux address `0x%02x` is out of the bus range.\n\n", *mux_addr);
    // Return for error
    return 1;
  }
  if (*mpu_addr > 127 || *mpu_addr < 0)
  {
    // Print error
    ERR("The mpu address `0x%02x` is out of the bus range.\n\n", *mpu_addr);
    // Return and error
    return 1;
  }
  if (*mux_chan < 0 || *mux_chan > 3)
  {
    // Print error
    ERR("The mux channel `%d` is out of range.\n\n", *mux_chan);
    // Return and error
    return 1;
  }
  // Free token
  free(_token);
  // Return success
  return 0;
}

///////////////////////////////////////////////////////////////////////////////
// MPU ROUTER
///////////////////////////////////////////////////////////////////////////////

int imu_mpu_route(i2c_bus *i2c, short addr, char **tokens, int argc)
{
  // Extract the mux address and the mpu address
  int mux_addr, mux_chan, mpu_addr, err;
  // Verify and set properties
  if ((err = parse_path(tokens[3], &mux_addr, &mux_chan, &mpu_addr)))
  {
    // Return failure
    return err;
  }
  // Init a pca mux
  Mux *pca = pca_init(i2c, mux_addr);
  // Init a mpu sensor
  Sensor *mpu = mpu_init(i2c, mpu_addr, pca, mux_chan, NULL);
  // Reset to default
  mpu->reset(mpu);
  // If reading axis
  if (!strcmp(tokens[4], "read_axes"))                        // READ_AXES
  {
    // Read and print axes
    imu_mpu_read_axes(mpu);
  }
  // Else if reading burst
  else if (!strcmp(tokens[4], "read_burst"))                  // READ_BURST
  {
    // Read and print fifo contents
    imu_mpu_read_burst(mpu);
  }
  // Else if config
  else if (!strcmp(tokens[4], "config"))                      // CONFIG
  {
    // Configure the mpu
    imu_mpu_config(mpu, tokens[5]);
  }
  // Else if reset
  else if (!strcmp(tokens[4], "reset"))                       // RESET
  {
    // Reset the mpu
    mpu->reset(mpu);
    // Print success
    printf("Gyro settings successfully reset.\n\n");
  }
  // Else if selftesting
  else if (!strcmp(tokens[4], "selftest"))                    // SELFTEST
  {
    // Run the selftest
    mpu->selftest(mpu);
  }
  // Else unsupported action
  else
  {
    // Print error
    ERR("Action `%s` is unsupported for mpu.\n\n", tokens[5]);
    // Return failure
    return 1;
  }

  // Deallocate the mpu
  mpu->dealloc(&mpu);
  // Deallocate the pca
  pca->dealloc(&pca);
  // Return success
  return 0;
}
