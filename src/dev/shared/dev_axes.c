///////////////////////////////////////////////////////////////////////////////
// Raspberry Pi GPIO Interface
// ¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯
// File: dev_axes.c
// PA Consulting - Lawrence Jones
///////////////////////////////////////////////////////////////////////////////

#include <stdlib.h>
#include "dev/shared/dev_sensor.h"
#include "macros.h"

///////////////////////////////////////////////////////////////////////////////
// MALLOC
///////////////////////////////////////////////////////////////////////////////

// Helper to malloc an Axes struct
Axes *axes_malloc(Axes *next)
{
  // Malloc new Axes struct
  Axes *ax = malloc(sizeof(Axes));
  // If not successfully malloced
  if (ax == NULL)
  {
    // Print error
    ERR("Error mallocing new axes. Unrecoverable.\n\n");
    // Exit with failure
    exit(EXIT_FAILURE);
  }
  // Assign next
  ax->next = next;
  // Return the pointer
  return ax;
}

///////////////////////////////////////////////////////////////////////////////
// TOOLS
///////////////////////////////////////////////////////////////////////////////

// Helper to produce an Axes struct containing averages
Axes *axes_average(Axes *ax)
{
  // If axes are already null
  if (ax == NULL)
  {
    // Return null
    return NULL;
  }
  // Produce result with initially 0 values
  Axes *res = malloc(sizeof(Axes));
  // Initialise variable to keep count
  int count = 0;
  // Initialise long values to keep totals
  long long int x = 0, y = 0, z = 0;
  // While we have an ax
  while (ax)
  {
    // Increment count
    count++;
    // Accumulate the axes
    x += ax->x;
    y += ax->y;
    z += ax->z;
    // Increment ax
    ax = ax->next;
  }
  // Set the averages into the res struct
  res->x = (short) (x / count);
  res->y = (short) (y / count);
  res->z = (short) (z / count);
  // Return the result
  return res;
}

///////////////////////////////////////////////////////////////////////////////
// DEALLOC
///////////////////////////////////////////////////////////////////////////////

void axes_dealloc(Axes **a)
{
  // Verify valid pointer
  if ((*a) == NULL)
  {
    ERR("The axes pointer is not valid.\n\n");
  }
  // Keep pointer to front of axes
  Axes *b;
  // While we have a next node
  do {
    // Save next node
    b = (*a)->next;
    // Free current
    free(*a);
  } while ((*a = b));
  // Set the callers pointer to null
  *a = NULL;
}

