///////////////////////////////////////////////////////////////////////////////
// Virtual-Gyroscope
// -----------------
// File :devices.c
// PA Consulting  - Pierre Sutter
///////////////////////////////////////////////////////////////////////////////

#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/mman.h>
#include "devices.h"
#include "utils/binary_loading.h"

///////////////////////////////////////////////////////////////////////////////
// Helper functions.
///////////////////////////////////////////////////////////////////////////////

void 		compute_path(Device*);
void 		dev_to_str(Device*);
void 		id_to_str(Device*);
void 		ax_to_str(Device*);
void 		mod_to_str(Device*);
void 		ex_to_str(Device*);

///////////////////////////////////////////////////////////////////////////////
// Create and free Device.
///////////////////////////////////////////////////////////////////////////////

Device* create_d(enum DEVICE_TYPE dt, enum MODEL mod,
  enum AXIS ax, int index)
{
	Device* d = malloc(sizeof(Device));
	d->dt = dt;
	d->mod = mod;
	d->ax = ax;
	d->index = index;
	d->path = malloc(sizeof(char)*40);
	compute_path(d);
  printf("%s.\n",d->path);
	return d;
}

void free_d(Device* d)
{
	free(d->path);
	free(d);
}

///////////////////////////////////////////////////////////////////////////////
// Create and free devices.
///////////////////////////////////////////////////////////////////////////////

Files* create_fds(Devices* ds)
{
	int i;
	int size;
	FILE *f;
	Files* fds = malloc(sizeof(Files));
	// Initializes the files_fds structure.
	fds->maps_num = 0;
	fds->maps_max = 0;
	fds->maps = malloc(sizeof(float*)*ds->num_ds);
	fds->maps_size = malloc(sizeof(int)*ds->num_ds);

	// For each devices try to open the corresponding file.
	for(i=0;i<ds->num_ds;i++)
	{
		f = fopen(ds->ds[i]->path,"rb");
		// If the file has been properly opened updates fds.
		if(f!=NULL)
		{
			// Compute the size of the file.
			size = get_size(f);
			// Create a new map for the file.
			fds->maps[fds->maps_num] = mmap(NULL,size,PROT_READ,MAP_SHARED,fileno(f),0);
			// Update the size array.
			fds->maps_size[fds->maps_num] = size;
			// Update the minimal size of our maps.
			if(fds->maps_max > size || fds->maps_num == 0)
			{
				fds->maps_max = size;
			}
			// Update the number of maps.
			fds->maps_num++;
			fclose(f);
		}
	}
	// Updates the value of maps_max
	fds->maps_max /=sizeof(float);
	return fds;
}

void free_ds(Devices* ds)
{
	int i;
	for(i=0;i<ds->num_ds;i++)
	{
		free_d(ds->ds[i]);
	}
	free(ds->ds);
	free(ds);
}

///////////////////////////////////////////////////////////////////////////////
// Free fds.
///////////////////////////////////////////////////////////////////////////////

void free_fds(Files* fds)
{
	int i;
	for(i=0;i<fds->maps_num;i++)
	{
		munmap(fds->maps[i],fds->maps_size[i]);
	}
	free(fds->maps_size);
	free(fds->maps);
	free(fds);
}

///////////////////////////////////////////////////////////////////////////////
// Function to compute the path of a device.
///////////////////////////////////////////////////////////////////////////////

void compute_path(Device *d)
{
	strcpy(d->path, "data/");
	dev_to_str(d);
	mod_to_str(d);
	id_to_str(d);
	ax_to_str(d);
	ex_to_str(d);
}

void dev_to_str(Device *d)
{
	switch(d->dt)
	{
		case ACCELEROMETER:
			strcat(d->path, "accels/a");
			break;
		case SPEED:
			strcat(d->path, "speed/s");
			break;
		case POSITION_DEVICE:
			strcat(d->path, "position/p");
			break;
		case GYROSCOPE:
			strcat(d->path, "gyros/g");
			break;
		case COMPASS:
			strcat(d->path, "mags/m");
			break;
		case ALLAN:
			strcat(d->path, "allan/a");
			break;
	}
}

void mod_to_str(Device *d)
{
	switch(d->mod)
	{
		case  ITG:
			strcat(d->path, "itg_");
			break;
		case  MMA:
			strcat(d->path, "mma_");
			break;
		case  MPU:
			strcat(d->path, "mpu_");
			break;
		case  L3G4:
			strcat(d->path, "l3g_");
			break;
		case CONVERTED:
			strcat(d->path, "conv_");
			break;
		case MATLAB:
			strcat(d->path, "_mat/g_math_");
			break;
		case MATH:
			strcat(d->path, "mat_");
			break;
		case VIRTUAL:
			strcat(d->path, "vir_");
			break;
		case FUSED:
			strcat(d->path, "fus_");
			break;
		case NORM_DEVICE:
			strcat(d->path, "norm_");
			break;
		case OTHER:
			break;
	}
}

void ax_to_str(Device *d)
{
	switch(d->ax)
	{
		case X:
			strcat(d->path, "x");
			break;
		case Y:
			strcat(d->path, "y");
			break;
		case Z:
			strcat(d->path, "z");
			break;
		case NONE:
			strcat(d->path, "n");
			break;
	}
}

void ex_to_str(Device *d)
{
	strcat(d->path,".dat");
}

void id_to_str(Device *d)
{
	char *temp = malloc(sizeof(char)*5);
	sprintf(temp,"%d_",d->index);
	strcat(d->path, temp);
	free(temp);
}

///////////////////////////////////////////////////////////////////////////////
// Utilitaries.
///////////////////////////////////////////////////////////////////////////////

// Write values held in files->maps at offset for each map in buff.
void read_fds(Files *fds, float *f, size_t offset)
{
	int i;
	for(i=0;i<fds->maps_num;i++)
	{
		f[i] = fds->maps[i][offset];
	}
}

// Iterate over each devices held by device with a function of type 
// void f(Device*);
void iterate_ds(Devices* ds, void (*f)(Device* d))
{
	int i;
	for(i=0;i<ds->num_ds;i++)
	{
		(*f)(ds->ds[i]);
	}
}
