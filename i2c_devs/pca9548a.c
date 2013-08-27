///////////////////////////////////////////////////////////////////////////////
// Raspberry Pi GPIO Interface
// ¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯
// File: pca9548a.c
// PA Consulting - Lawrence Jones
///////////////////////////////////////////////////////////////////////////////

#include <stdlib.h>
#include <stdio.h>
#include "pca9548a.h"
#include "../src/io.h"

///////////////////////////////////////////////////////////////////////////////
// INITIALISATION
///////////////////////////////////////////////////////////////////////////////
/*
   The PCA9548A is an I2C multiplexer, with the ability to split
   a single I2C bus four ways.

   The IMU board currently utilises two of these multiplexers
   on either side of the board. The devices are distributed
   between the multiplexers channels like so...

      ====================================================
      |  Channel   |   Sensors                           |
      ====================================================
      |   0x01     |   MPU  (0x69)   ITG  (0x68)         |
      |   0x02     |   ITG  (0x69)   L3G4 (0x68)         |
      |   0x04     |   L3G4 (0x69)   MAG  (0x0e)         |
      ====================================================
   
   ...whilst there are, as stated, 4 different channels, only 
   three are in use. As of present, no MAG's are attached 
   either.

   The mux works very simply. The channels are labelled by the
   value the sensors require written to them in order to enable 
   that specific channel.

   The only complex part of this device is preventing any
   confusion between devices of the same model with identical
   addresses. As such, reading the current value of the mux
   easily is required by the device to correlate against 
   other device drivers and prevent such mixups.

   These pca's operate with an i2c slave address of 0x74.
 */

// Given name, i2c addr and i2c handle, allocates & initialises
// a new Mux struct.
Mux *pca_init(char* name,
              short i2c_addr,
              i2c_bus *i2c  )
{
  // malloc the sensor struct
  Mux *m = malloc(sizeof(Mux));
  // Verify malloc success
  if (!m)
  {
    // malloc has failed, print error and exit
    ERR("Failed to allocate memory (malloc) for Mux *m.\n");
    exit(EXIT_FAILURE);
  }
  // Assign given sensor properties
  m->name = malloc(sizeof(char) * strlen(name) + 1);
  // Make copies of strings to avoid stack expiry
  strcpy(m->name, name);
  // Assign the i2c addr
  m->i2c_addr = i2c_addr;
  // Assign the i2c handle
  m->i2c = i2c;
  // Assign the enum for model
  m->model = PCA9548A;
  // Set the channel fetch function
  m->fetch_channel = &pca_fetch_channel;
  // Assign the channel set function
  m->set_channel = &pca_set_channel;
  // Assign the mux networking function
  //m->get_devs = &pca_get_devs;
  // Set the channel to 0 initially
  m->set_channel(m, 0);
  // Return the struct
  return m;
}

///////////////////////////////////////////////////////////////////////////////
// CONFIGURING CHANNEL
///////////////////////////////////////////////////////////////////////////////
/*
   The mux is only ever used directly to access it's current
   channel register or to write to it.
   As such, there are only two functions below to achieve this.
 */

// Helper to avoid duplicating validity checks
static void verify_mux(Mux *m)
{
  // Verify that we have a valid mux pointer
  if (m == NULL)
  {
    // Print error message
    ERR("Invalid pointer for mux struct (m).\n\n");
    // Exit with failure
    exit(EXIT_FAILURE);
  }
  // Verify that we have a valid i2c handle
  if (m->i2c == NULL)
  {
    // Print error message
    ERR("Invalid i2c handle attached to mux `%s`.\
         Cannot continue.\n\n", m->name);
    // Exit with failure
    exit(EXIT_FAILURE);
  }
}

// Fetches the current channel code from the given mux,
// then updates the field inside the given struct and
// returns the code as the value
uint8_t pca_fetch_channel(Mux *m)
{
  // Verify that the pointer is healthy
  verify_mux(m);
  // Once verified, read the channel code from the mux 
  // into the channel field
  m->channel = i2c_read_byte(m->i2c, m->i2c_addr);
  // Return the value read
  return m->channel;
}

// Sets the pca control channel and verifies write
// success
void pca_set_channel(Mux *m, uint8_t c)
{
  // Verify arg health
  verify_mux(m);
  // Once verified, transfer the channel code to
  // the device
  i2c_write_byte(m->i2c, m->i2c_addr, (uint8_t)c);
  // Once written, verify that write has been successful
  m->fetch_channel(m);
  // After fetching the channel, write has been successful
  // iff the new channel field is equal to the desired parameter
  if (c != m->channel)
  {
    // If not equal then write has failed
    ERR("Write to mux named `%s` has failed.\n\n", m->name);
    // Exit with error
    exit(EXIT_FAILURE);
  }
}

///////////////////////////////////////////////////////////////////////////////
// BUILD MUX NETWORK
///////////////////////////////////////////////////////////////////////////////
/*
   The `get_devs` function pointer should, when provided with a
   mux struct pointer, provide a MuxNetwork showing all the devices
   that are responsive on it's buses.
   TODO - Requires a very neat print function.
*/

// Prints to stdout the mux network
void mux_network_print(Mux *m)
{
  // Generate mux network
  MuxNetwork *net = pca_get_devs(m);
  // If net is null
  if (net == NULL)
  {
    // Then exit function
    return;
  }
  printf("MuxNetwork map for `%s`\n\n", m->name);
  while ((net = net->next)) {
    int count = 0;
    PRINTC(GREEN, "Devices on channel %d...\n\n", net->channel);
    // Print the device while one exists
    do {
      printf("  Dev %d  -  Addr 0x%02x\n", count++, net->dev->addr);
    } while ((net->dev = net->dev->next));
    // Clear a line
    PRINTC(GREEN, "\n...done.\n\n");
  };
}

static int i2c_devs_contains(i2c_dev *list, i2c_dev *node)
{
  // If the node is null
  if (!node)
  {
    // Then return false
    return 0;
  }
  // Create a previous start node
  list = &((i2c_dev){-1, list});
  // While there is a valid next node
  while ((list = list->next))
  {
    // If the addresses are equal
    if (node->addr == list->addr)
    {
      // Then return true
      return 1;
    }
  }
  // If not found a match then return false
  return 0;
}

static void i2c_dev_filter(i2c_dev **as, i2c_dev *bs)
{
  // If as or bs are null
  if (!as || !bs)
  {
    // Return as nothing need be done
    return;
  }
  // Make an i2c_dev for the start of the chain
  // and create a pointer for the next node
  *as = &((i2c_dev){-1,*as});
  i2c_dev *a1 = *as,
          *a2 = *as;
  // While a2 is not null
  while ((a2 = a1->next))
  {
    // If the node a2 is included in bs
    if (i2c_devs_contains(bs, a2))
    {
      // Then remove a2 from the chain
      a1->next = a2->next;
      // Free the a2 pointer
      free(a2);
    }
    // Increment a1
    a1 = a2;
  }
  // Move the as pointer to the next node
  *as = (*as)->next;
}


// Scans all the available buses and generates a MuxNetwork
// Does not include other devices present on the network
MuxNetwork *pca_get_devs(Mux *m)
{
  // Initially disable all channels
  pca_set_channel(m, PCA_CD);
  // Gather all devices on network not on mux
  i2c_dev *extdevs = i2c_dev_detect(m->i2c),
          *muxdevs = NULL;
  // Declare and init a MuxNetwork
  MuxNetwork *net = malloc(sizeof(MuxNetwork)),
             *crrt = net;
  // Set the net channel to -1 (external devices) initially
  net->channel = -1;
  // Assign the first devs as externals
  net->dev = extdevs;
  // For channel 0 to 3
  for (int i = 0; i < 4; i++)
  {
    // Set the channel
    m->set_channel(m, 1 << i);
    // Scan bus for devices
    muxdevs = i2c_dev_detect(m->i2c);
    // TODO - Implement filter
    // Remove any non-muxed devs from the scan
    i2c_dev_filter(&muxdevs, extdevs);
    // Add to current network by malloc...
    crrt->next = malloc(sizeof(MuxNetwork));
    // Increment crrt
    crrt = crrt->next;
    // ...assign the channel value...
    crrt->channel = i;
    // ...assign the muxdevs with externals removed
    crrt->dev = muxdevs;
  }
  // Return the origin of the MuxNet
  return net;
}

///////////////////////////////////////////////////////////////////////////////
// DEALLOC PCA
///////////////////////////////////////////////////////////////////////////////
/*
   Deallocing the pca is much less significant than any sensors
   due to the passive nature of the device. Any pointers to
   i2c devices (which may or may not be sensors) have the potential
   to be leaked if not dealt with carefully though.
*/

void pca_dealloc(Mux **m)
{
  // If either the pointer to pointer of the pointer to the
  // mux is null then...
  if (!(m && *m))
  {
    // Output error message
    ERR("Not a valid Mux pointer. Possibly already free?\n\n");
    // Exit with error
    exit(EXIT_FAILURE);
  }
  // m is now a valid pointer, so proceed to free fields
  // Free the mux name string
  free((*m)->name);
  // Free the struct
  free(*m);
}
