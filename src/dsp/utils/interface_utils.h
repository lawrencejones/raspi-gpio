///////////////////////////////////////////////////////////////////////////////
// Virtual-Gyroscope
// -----------------
// File :interface_utils.h
// PA Consulting  - Pierre Sutter
///////////////////////////////////////////////////////////////////////////////

#ifndef INTERFACE_UTILS 
#define INTERFACE_UTILS
#include <stdbool.h>

////////////////////////////////////////////////////////////////////////////////
// Struct arw.
////////////////////////////////////////////////////////////////////////////////

typedef struct arw_* Arw;

///////////////////////////////////////////////////////////////////////////////
// Scan functions.
///////////////////////////////////////////////////////////////////////////////

// Scans for devices with the corresponding model and type.
Devices*        scan_devices(char *type, char *model, int index);

///////////////////////////////////////////////////////////////////////////////
// Generate and free arw.
///////////////////////////////////////////////////////////////////////////////

// Returns true if an allan file with specified model and axis exists.
Arw             generate_arw(Device *d);
void            free_arw(Arw);

///////////////////////////////////////////////////////////////////////////////
// Iterate over arw.
///////////////////////////////////////////////////////////////////////////////

int             length_arw(Arw);
int             index_arw(Arw);
float           values_arw(Arw);
void            iterate_arw(Arw);

///////////////////////////////////////////////////////////////////////////////
// String to matrix function.
///////////////////////////////////////////////////////////////////////////////

void            string_to_matrix(Conf*, char*);

///////////////////////////////////////////////////////////////////////////////
// Gsl functions.
///////////////////////////////////////////////////////////////////////////////

// Access i,j-th element of matrix m.
float           get_gsl_matrix_get(gsl_matrix* m, int i , int j);
int             get_size1(gsl_matrix* m);
int             get_size2(gsl_matrix* m);

///////////////////////////////////////////////////////////////////////////////
// Access functions.
///////////////////////////////////////////////////////////////////////////////

// Access i-th element of array array.
void            *access_pointer(void** arr, int i);
// Access i-th element of array array.
float           access_float(float* arr, int i);
// Access i-th element of array array.
int             access_int(int* arr, int i);
// Access j-th element of the i-th element of an array of pointers to
// floats.
float           access_pointer_float(void** arr, int i, int j);

///////////////////////////////////////////////////////////////////////////////
// Conversion Functions.
///////////////////////////////////////////////////////////////////////////////

int             str_to_mod(char *model);
int             str_to_typ(char *type);

////////////////////////////////////////////////////////////////////////////////
// Struct arw.
////////////////////////////////////////////////////////////////////////////////

struct arw_
{
  enum AXIS   axis;
  int         num_values;
  int         index;
  int         *indices;
  float       *values;
};

#endif
