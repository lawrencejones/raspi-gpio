///////////////////////////////////////////////////////////////////////////////
// Raspberry Pi GPIO Interface
// ¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯
// File: pca_muxnet.c
// PA Consulting - Lawrence Jones
///////////////////////////////////////////////////////////////////////////////

#include <stdlib.h>
#include "pca_private.h"
#include "macros.h"

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
  printf("MuxNetwork map for mux at addr 0x%02x\n\n", m->i2c_addr);
  // While there is a next net and that net has devices
  while ((net = net->next)) {
    int count = 0;
    // If there are devices
    if (net->dev)
    {
      PRINTC(GREEN, "Devices on channel %d...\n\n", net->channel);
      // Print the device while one exists
      do {
        printf("  Dev %d  -  Addr 0x%02x\n", count++, net->dev->addr);
      } while ((net->dev = net->dev->next));
      // Clear a line
      PRINTC(GREEN, "\n...done.\n\n");
    }
  };
}

int i2c_devs_contains(i2c_dev *list, i2c_dev *node)
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

void i2c_dev_filter(i2c_dev **as, i2c_dev *bs)
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
    else
    {
      // Increment a1
      a1 = a2;
    }
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
    m->set_channel(m, i);
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
