///////////////////////////////////////////////////////////////////////////////
// Virtual-Gyroscope
// -----------------
// File :parser.c
// PA Consulting  - Pierre Sutter
///////////////////////////////////////////////////////////////////////////////

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdbool.h>
#include "utils/binary_loading.h"
#include "parser.h"

///////////////////////////////////////////////////////////////////////////////
// Helper functions.
///////////////////////////////////////////////////////////////////////////////

int matlab_readline(FILE *, char*);
void iterate_matlab_parse(Device *);

///////////////////////////////////////////////////////////////////////////////
// Scanning functions.
///////////////////////////////////////////////////////////////////////////////

Devices *scan_folder(enum DEVICE_TYPE dt, enum MODEL m, enum AXIS ax)
{
	int index, mod, mod_init, mod_limit, failed;
	//Allocates memory for the devices.
	Device *d;
	Devices *ds = malloc(sizeof(Devices)); 
	ds->num_ds=0;
	// We assume the maximum number of devices is 20.
	ds->ds = malloc(sizeof(Device*)*20);
	// If model m == enum_model_length scans for all items, excluding
	// the matlab ones that are not binary files.
  if(m == enum_model_length)
	{
		mod_init = 1;
		mod_limit = m;
	}
  // Otherwise only looks for the specified item.
	else
	{
  	mod_init = m;
		mod_limit = m+1;
	}

	for(mod=mod_init;mod<mod_limit;mod++)
	{
		failed=0;
		index=0;
		//While we haven't failed to open enum_Axis_length files in a row.
		while(failed!=enum_axis_length)
		{
			d = create_d(dt,mod,ax,index);
			// If file exists add the devices to ds.
			if(file_exists(d->path))
			{
				ds->ds[ds->num_ds] = d;
				ds->num_ds++;
			}
			// If file couldn't be opened then free the ressources.
			else
			{
			  free_d(d);
				failed++;
			}
			index++;
		}
	}
	return ds;
}

// Parses n gyr files from the Matlab folder into .dat files. Axis is
// an array of char associated to axis and n_axis is the number of
// axis in the array.
void matlab_parse()
{
	int axis;
	Devices* ds;

	for(axis=0;axis<3;axis++)
	{
		ds = scan_folder(GYROSCOPE,MATLAB,axis);
		iterate_ds(ds,&iterate_matlab_parse);
		free_ds(ds);
	}
}

///////////////////////////////////////////////////////////////////////////////
// Helper functions.
///////////////////////////////////////////////////////////////////////////////

void iterate_matlab_parse(Device *d)
{
	int eof;
	float line_value;
	char *buff = malloc(sizeof(char)*13);
	Device *d_out;
	FILE *in,*out;

	// Creates device d_out.
	d_out = create_d(d->dt,MATH,d->ax,d->index);

	// Open files for the current device.
	unlink(d_out->path);
	in = fopen(d->path, "r");
	out = fopen(d_out->path, "wb+");

	// If both files are opened properly.
	if(in!=NULL && out!=NULL)
	{
		// Read all lines.
		eof = matlab_readline(in,buff);
		// While the end of the file has not been reached.
		while(eof == 1)
		{
			line_value = atof(buff);
			// Writes the fetched value to the result file.
			fwrite(&line_value,sizeof(float),1,out);
			// Reads new value from the input file.
			eof = matlab_readline(in, buff);
		}
    // Prints status message.
    printf("Importing data from %s to %s.\n",d->path,d_out->path);

	}

	// Close all files.
	if(in!=NULL)
	{
		fclose(in);
	}
	if(out!=NULL)
	{
		fclose(out);
	}

	// Free allocated ressources.
	free_d(d_out);
	free(buff);
}

int matlab_readline(FILE *f, char* buff)
{
	int i;
	const int max = 13;// The maximal number of elements in one line of code.
	for(i=0;i<max;i++)
	{
		// Reads next character.
		buff[i] = fgetc(f);

		// If the end of the file has been reached return 0.
		if(buff[i] == EOF)
			return 0;

		// If the i-th character is a new line, converts the line into a
		// float.
		if(buff[i] == '\n')
		{
			return 1;
		}
	}
	return 0;
}

///////////////////////////////////////////////////////////////////////////////
// Folder functions.
///////////////////////////////////////////////////////////////////////////////

bool            devices_exist(Devices* ds)
{
  int i;
  for(i=0;i<ds->num_ds;i++)
  {
    if(!file_exists(ds->ds[i]->path))
    {
      return false;
    }
  }
  return true;
}

////////////////////////////////////////////////////////////////////////////////
// Quick importation function.
////////////////////////////////////////////////////////////////////////////////

void            split_axis(char *path, enum DEVICE_TYPE type, enum MODEL model, int axis)
{
  FILE *f = fopen(path,"rb");
  if(f)
  {
    int i;
    float readings[3];
    // Creates devices.
    Device *d_x = create_d(type,model,X,axis);
    Device *d_y = create_d(type,model,Y,axis);
    Device *d_z = create_d(type,model,Z,axis);
    // Unlink previous results.
    unlink(d_x->path);
    unlink(d_y->path);
    unlink(d_z->path);
    // Opens new files.
    FILE *f_x = fopen(d_x->path,"+wb");
    FILE *f_y = fopen(d_y->path,"+wb");
    FILE *f_z = fopen(d_z->path,"+wb");
    // While i is less than the number of floats divided by 3.
    for(i=0;i<(get_size(f)/sizeof(float)*3);i++)
    {
      // Reads three values from f.
      fread(&readings,3,sizeof(float),f);
      // Writes retrieved values to files.
      fwrite(&readings[0],1,sizeof(float),f_x);
      fwrite(&readings[1],1,sizeof(float),f_y);
      fwrite(&readings[2],1,sizeof(float),f_z);
    }
    fclose(f_x);
    fclose(f_y);
    fclose(f_z);
    fclose(f);
  }
}
