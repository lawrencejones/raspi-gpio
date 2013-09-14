///////////////////////////////////////////////////////////////////////////////
// Virtual Gyroscope.
// ¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯
// File: gravity.c
// PA Consulting - Pierre Sutter
///////////////////////////////////////////////////////////////////////////////

#include "gravity.h"

/////////////////////////////////////////////////////////////////////////////////
// Functions for speed computing by gravity removal.
/////////////////////////////////////////////////////////////////////////////////

float               compute_result_gravity(Params_Template*);
Files*              scan_gravity(Params_Template*);
void								free_params_gravity(Params_Template*);
Params_Gravity*			initialize_gravity();
float								average_signal(Device *d);

/////////////////////////////////////////////////////////////////////////////////
// Number of readings used to calibrate gravity.
/////////////////////////////////////////////////////////////////////////////////

const int           average_gravity = 100;

/////////////////////////////////////////////////////////////////////////////////
// Speed computing by gravity removal.
/////////////////////////////////////////////////////////////////////////////////

void							  generate_gravity()
{
	// Allocates memory for new Iteration and Params_Template structures.
  Iterate* iter = generate_iter();
  iter->params->params = initialize_gravity();
	// Initializes iter's fields.
	iter->compute_result = &compute_result_gravity;
	iter->free_params = &free_params_gravity;
  iter->scan = &scan_gravity;
  iter->params->result_type = SPEED;
  iter->params->result_model = OTHER;
	// Iterates.
	iterate_devices(iter);
	// Frees allocated ressources.
	free_iter(iter);
}

Params_Gravity*			initialize_gravity()
{
	int i;
	Device *d;
	Params_Gravity *p_ = malloc(sizeof(Params_Gravity));
	p_->time_interval = 0.003;
	p_->previous = 0;
	// For all axis compute the average signal and assign it to the
	// corresponding axis of gravity.
	for(i=0;i<3;i++)
	{
		d = create_d(ACCELEROMETER,FUSED,X,0);
    // If the file exists and the file is big enough.
    if(file_exists(d->path) 
        && get_size_path(d->path)/sizeof(float) > average_gravity)
    {
		  p_->gravity[i] = average_signal(d);
    }
    else
    {
		  p_->gravity[i] = 0;
    }
		free_d(d);
	}
	return p_;
}

Files*              scan_gravity(Params_Template *p)
{
  Device *fused_accel = create_d(ACCELEROMETER,FUSED,p->axis,0);
  Devices *scan_ds = malloc(sizeof(Devices));
  // If both axis exists.
  if(file_exists(fused_accel->path))
  {
    scan_ds->num_ds = 1;
    scan_ds->ds = malloc(sizeof(Device*)*1);
    scan_ds->ds[0] = fused_accel;
  } else
  {
    printf("0 files.\n");
    scan_ds->num_ds = 0;
		scan_ds->ds = malloc(sizeof(Device*));
    free_d(fused_accel);
  }
  Files *fs = create_fds(scan_ds);
  free_ds(scan_ds);
  return fs;
}

void								free_params_gravity(Params_Template *p)
{
	Params_Gravity *p_ = (Params_Gravity*) p->params;
  free(p_);
	free_params_template(p);
}

float           	  compute_result_gravity(Params_Template *p)
{
  Params_Gravity *p_ = (Params_Gravity*) p->params;
  // Updates the value of the speed.
  p_->previous += p_->time_interval*p->readings[0] - p_->gravity[p->axis];
  // Returns weighted average of the two readings.
  return p_->previous;
}

// Compute the average signal of the 100 first values of the device d.
// If device d's file can't be opened then return 0.
float									average_signal(Device *d)
{
	FILE *f = fopen(d->path,"rb");
	int num_max,i;
	float sum,temp;
	sum = 0;
	// If file is not null and its size is big enough
	if(!f)
	{
		((get_size(f) / sizeof(float))> 100) ? (num_max = 100) : (num_max = 0);
		for(i=0;i<num_max;i++)
		{
			fread(&temp,sizeof(temp),1,f);
			sum += temp;
		}
		fclose(f);
		sum /= num_max;
	}
	return sum;
}
