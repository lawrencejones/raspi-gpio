///////////////////////////////////////////////////////////////////////////////
// Raspberry Pi GPIO Interface
// ¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯
// File: imu_gyro.c
// PA Consulting - Lawrence Jones
///////////////////////////////////////////////////////////////////////////////

#include <stdlib.h>
#include <stdio.h>
#include "imu_private.h"
#include "devs/mpu3300.h"
#include "devs/itg3050.h"
#include "devs/pca9548a.h"
#include "macros.h"

///////////////////////////////////////////////////////////////////////////////
// GYRO READ AXES
///////////////////////////////////////////////////////////////////////////////

static int imu_gyro_read_axes(Sensor *gyro)
{
  // Attempt a read and print results
  Axes *ax = gyro->read(gyro, HOST);
  printf("The axes readings are...\n\n");
  printf("  X : %d\n  Y : %d\n  Z : %d\n\n", 
    (int)ax->x, (int)ax->y, (int)ax->z);
  // Free the memory
  axes_dealloc(&ax);
  // Return success
  return 0;
}

///////////////////////////////////////////////////////////////////////////////
// GYRO READ BURST
///////////////////////////////////////////////////////////////////////////////

static int imu_gyro_read_burst(Sensor *gyro)
{
  // Attempt a burst read and print results
  Axes *head = gyro->read(gyro, FIFO),
       *ax = head;
  printf("The axes readings are...\n\n");
  printf  ("             +---------+---------+---------+\n");
  printf  ("             |    X    |    Y    |    Z    |\n");
  printf  ("             +---------+---------+---------+\n");
  int count = 0;
  while (ax)
  {
    printf(" %3d -       | %7d | %7d | %7d |\n", 
      count++, (short)ax->x, (short)ax->y, (short)ax->z);
    ax = ax->next;
  }
  // Print final clearing line
  printf("\n");
  // Deallocate the returned axes
  axes_dealloc(&head);
  // Return success
  return 0;
}

///////////////////////////////////////////////////////////////////////////////
// GYRO CONFIG
///////////////////////////////////////////////////////////////////////////////

static int imu_gyro_config(Sensor *gyro, char* config_str)
{
  // Attempt to configure the axis
  int res = gyro->config(gyro, config_str);
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
                        int  *gyro_addr )
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
  *gyro_addr = strtoul(strtok(NULL, " "), NULL, 16);
  // Verify sensible values 
  if (*mux_addr > 127 || *mux_addr < 0)
  {
    // Print error
    ERR("The mux address `0x%02x` is out of the bus range.\n\n", *mux_addr);
    // Return for error
    return 1;
  }
  if (*gyro_addr > 127 || *gyro_addr < 0)
  {
    // Print error
    ERR("The gyro address `0x%02x` is out of the bus range.\n\n", *gyro_addr);
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
// GYRO ROUTER
///////////////////////////////////////////////////////////////////////////////

int imu_gyro_route(i2c_bus *i2c, short addr, char **tokens, int argc)
{
  // Extract the mux address and the gyro address
  int mux_addr, mux_chan, gyro_addr, err;
  // Verify and set properties
  if ((err = parse_path(tokens[3], &mux_addr, &mux_chan, &gyro_addr)))
  {
    // Return failure
    return err;
  }
  // Init a pca mux
  Mux *pca = pca_init(i2c, mux_addr);
  model_t model = NA;
  Sensor *gyro = NULL;
  if (!strcmp(tokens[1], "mpu"))
  {
    // Init a gyro sensor
    gyro = mpu_init(i2c, gyro_addr, pca, mux_chan, NULL);
    model = MPU3300;
  }
  else if (!strcmp(tokens[1], "itg"))
  {
    // Init an itg sensor
    gyro = itg_init(i2c, gyro_addr, pca, mux_chan, NULL);
    model = ITG3050;
  }
  else
  {
    ERR("Gyro `%s` is unsupported.\n\n", tokens[1]);
  }
  int supported = 0;
  switch (model)
  {
    case MPU3300:
      // Else if selftesting
      if (!strcmp(tokens[4], "selftest"))                       // SELFTEST
      {
        // Run the selftest with print on
        gyro->selftest(gyro, 1);
        supported = 1;
      }
    case ITG3050:
      // If reading axis
      if (!strcmp(tokens[4], "read_axes"))                        // READ_AXES
      {
        // Read and print axes
        imu_gyro_read_axes(gyro);
        supported = 1;
      }
      // Else if reading burst
      else if (!strcmp(tokens[4], "read_burst"))                  // READ_BURST
      {
        // Read and print fifo contents
        imu_gyro_read_burst(gyro);
        supported = 1;
      }
      // Else if config
      else if (!strcmp(tokens[4], "config"))                      // CONFIG
      {
        // Configure the gyro
        imu_gyro_config(gyro, tokens[5]);
        supported = 1;
      }
      // Else if reset
      else if (!strcmp(tokens[4], "reset"))                       // RESET
      {
        // Reset the gyro
        gyro->reset(gyro);
        // Print success
        printf("Gyro settings successfully reset.\n\n");
        supported = 1;
      }
    default:
      // Else if unsupported action
      if (!supported)
      {
        // Print error
        ERR("Action `%s` is unsupported for gyro.\n\n", tokens[5]);
      }
  }

  // Deallocate the gyro
  gyro->dealloc(&gyro);
  // Deallocate the pca
  pca->dealloc(&pca);
  // Return supported
  return !supported;
}
