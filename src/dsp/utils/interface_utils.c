///////////////////////////////////////////////////////////////////////////////
// Virtual-Gyroscope
// -----------------
// File :interface_utils.c
// PA Consulting  - Pierre Sutter
///////////////////////////////////////////////////////////////////////////////

#include <gsl/gsl_matrix.h>
#include <sys/mman.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include "binary_loading.h"
#include "../devices.h"
#include "../parser.h"
#include "../conf/conf_vgyro.h"
#include "interface_utils.h"


/////////////////////////////////////////////////////////////////////////////////
// Arw functions.
/////////////////////////////////////////////////////////////////////////////////

Arw           generate_arw(Device *d)
{
  int j;
  FILE *f = fopen(d->path,"rb");
  // Allocates memory for the arw structure.
  Arw result = malloc(sizeof(struct arw_));
  if(f!=NULL)
  {
    // Computes the number of values for the current file.
    result->num_values = (int) get_size(f)/(sizeof(int) + sizeof(float));
    result->indices = malloc(sizeof(int)*result->num_values);
    result->values = malloc(sizeof(float)*result->num_values);
    result->axis = d->ax;
    result->index = 0;

    // Parses every values from the allan file.
    for(j=0;j<result->num_values;j++)
    {
      fread(&(result->values[j]),sizeof(float),1,f);
      fread(&(result->indices[j]),sizeof(int),1,f);
    }

    fclose(f);
  }
  return result;
}


// Frees the ressources occupied by an arw file.
void free_arw(Arw arw)
{
  free(arw->indices);
  free(arw->values);
  free(arw);
}

///////////////////////////////////////////////////////////////////////////////
// Iterate over arw.
///////////////////////////////////////////////////////////////////////////////

int             length_arw(Arw arw)
{
  return arw->num_values;
}

int             index_arw(Arw arw)
{
  return arw->indices[arw->index];
}

float           values_arw(Arw arw)
{
  return arw->values[arw->index];
}

void            iterate_arw(Arw arw)
{
  (arw->index == arw->num_values-1) ? arw->index=0: arw->index++;
}

///////////////////////////////////////////////////////////////////////////////
// String to matrix function.
///////////////////////////////////////////////////////////////////////////////

// Takes a string representation of a matrixc and saves it to the
// given configuration.
void string_to_matrix(Conf* c, char* s)
{
  int i,j;
  int value_index;
  bool reading = false;

  // Holds the parsed values.
  float value[c->n*c->n];
  // Assumes the length of a float to be less than 15.
  char temp_reading[15];
  int s_length = strlen(s);


  j=0;
  value_index = 0;
  // Parses until all characters are parsed or the expected number of
  // values has been parsed.
  for(i=0;i<=s_length && value_index < (c->n*c->n);i++)
  {
    // Parses data while characters are digits or points and less than
    // 15 characters have been parsed so far.
    if(((s[i] <= '9' && s[i] >= '0') || s[i] == '.') && j<14)
    {
      reading = true;
      temp_reading[j] = s[i];
      j++;
    }
    else 
    {
      // Else if we were parsing characters.
      if(reading == true)
      {
        // Sets reading to false.
        reading = false;
        temp_reading[j] = '\0';
        value[value_index] = atof(temp_reading);
        value_index++;
        j=0;
      }
    }
  }

  // Checks that the number of value is the expected one.
  if(value_index == (c->n*c->n))
  {
    // If so modifies the values of the cross correlation matrix
    // accordingly.
    value_index = 0;
    for(i=0;i<c->n;i++)
      for(j=0;j<c->n;j++)
      {
        gsl_matrix_set(c->cross_corr,i,j,value[value_index]);
        value_index++;
      }
  }
}
///////////////////////////////////////////////////////
// Access functions.
////////////////////////////////////////////////////////

// Returns the float at index i in the array of floats f.
void            *access_pointer(void** p, int i)
{
  return p[i];
}

float           access_float(float *f, int i)
{
  return f[i];
}

int             access_int(int* in, int i)
{
  return in[i];
}

float           access_pointer_float(void** arr, int i, int j)
{
  return ((float*)arr[i])[j];
}

///////////////////////////////////////////////////////////////////////////////
// Gsl functions.
///////////////////////////////////////////////////////////////////////////////

// Access i,j-th element of matrix m.
float           get_gsl_matrix_get(gsl_matrix* m, int i , int j)
{
  return gsl_matrix_get(m,i,j);
}

int             get_size1(gsl_matrix* m)
{
  return m->size1;
}

int             get_size2(gsl_matrix* m)
{
  return m->size2;
}

///////////////////////////////////////////////////////////////////////////////
// Scan functions.
///////////////////////////////////////////////////////////////////////////////

// Helper functions.
int             str_to_mod(char *model);
int             str_to_typ(char *type);

// Scans for devices with the corresponding model and type.
Devices*        scan_devices(char *type, char *model, int index)
{
  int mod_ = str_to_mod(model);
  int typ_ = str_to_typ(type);
  Devices *ds = malloc(sizeof(Devices));
  ds->num_ds = 0;
  ds->ds = malloc(sizeof(Device*)*4);
  // Scans device types and models.
  if(mod_ != -1 && typ_ != -1)
  {
    // If valid model and valid type iterate over each axis.
    int axis,added;
    added = 0;
    Device *temp;
    // For each axis.
    for(axis=0;axis<enum_axis_length;axis++)
    {
      temp = create_d(typ_,mod_,axis,index);
      // If device actually exists, add it to ds.
      if(file_exists(temp->path))
      {
        // Increment the number of added devices.
        printf("Found device: %s.\n", temp->path);
        ds->ds[added] = temp;
        added++;
      }
      else
      {
        free_d(temp);
      }
    }
    ds->num_ds = added;
  }
  return ds;
}

///////////////////////////////////////////////////////////////////////////////
// Helper functions for scan functions.
///////////////////////////////////////////////////////////////////////////////

#define STR_ENUM(mod, _mod, result) if(!strcmp(mod,_mod)) { return result; }

int             str_to_mod(char *model)
{
  STR_ENUM(model,"itg",ITG)
  STR_ENUM(model,"mpu",MPU)
  STR_ENUM(model,"l3g4",L3G4)
  STR_ENUM(model,"mma",MMA)
  STR_ENUM(model,"vir",VIRTUAL)
  STR_ENUM(model,"fused",FUSED)
  return -1;
}

int             str_to_typ(char *type)
{
  STR_ENUM(type,"accel",ACCELEROMETER)
  STR_ENUM(type,"gyro",GYROSCOPE)
  STR_ENUM(type,"allan",ALLAN)
  return -1;
}
