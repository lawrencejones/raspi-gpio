///////////////////////////////////////////////////////////////////////////////
// Virtual Gyroscope.
// ¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯
// File: vaccel.c
// PA Consulting - Pierre Sutter
///////////////////////////////////////////////////////////////////////////////

#include "vaccel.h"

/////////////////////////////////////////////////////////////////////////////////
// Generate Virtual Accelerometer Functions.
/////////////////////////////////////////////////////////////////////////////////

float           	compute_result_vaccel(Params_Template*);
Files*            scan_vaccel(Params_Template*);

/////////////////////////////////////////////////////////////////////////////////
// Generate Virtual Accelerometer.
/////////////////////////////////////////////////////////////////////////////////

void							generate_vaccel()
{
	// Allocates memory for new Iteration and Params_Vaccel structures.
	Iterate *iter = generate_iter();
	// Initializes iter's fields.
	iter->compute_result = &compute_result_vaccel;
	iter->free_params = &free_params_template;
  iter->scan = &scan_vaccel;
  iter->params->result_type = ACCELEROMETER;
  iter->params->result_model = VIRTUAL;
	// Iterates.
	iterate_devices(iter);
	// Frees allocated ressources.
	free_iter(iter);
}

Files*            scan_vaccel(Params_Template* p)
{
  Devices* ds = scan_folder(ACCELEROMETER,OTHER,p->axis);
  Files* fs = create_fds(ds);
  free_ds(ds);
  return fs;
}

// Computes the mean of an array of floats. Assumption: the function
// is only called with arrays of length > 0.
float           	compute_result_vaccel(Params_Template *p_)
{
  int i;
  float result = 0;
  for(i=0;i<p_->fs->maps_num;i++)
  {
    result += p_->readings[i];
  }
	result /= p_->fs->maps_num;
  return result;
}
