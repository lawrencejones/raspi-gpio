///////////////////////////////////////////////////////////////////////////////
// Virtual Gyroscope.
// ¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯
// File: norm.c
// PA Consulting - Pierre Sutter
///////////////////////////////////////////////////////////////////////////////

#include "norm.h"

/////////////////////////////////////////////////////////////////////////////////
// Functions for norm generation.
/////////////////////////////////////////////////////////////////////////////////

float               compute_result_norm(Params_Template*);
Files*              scan_norm(Params_Template*);
void								free_params_norm(Params_Template*);

/////////////////////////////////////////////////////////////////////////////////
// Norm generation.
/////////////////////////////////////////////////////////////////////////////////

void				generate_norm(enum DEVICE_TYPE type, enum MODEL model)
{
	// Allocates memory for new Iteration and Params_Template structures.
  Iterate* iter = generate_iter();
  Params_Norm *norm = malloc(sizeof(Params_Norm));
  norm->scan_type = type;
  norm->scan_model = model;
  iter->params->params = norm;
	// Initializes iter's fields.
	iter->compute_result = &compute_result_norm;
	iter->free_params = &free_params_norm;
  iter->scan = &scan_norm;
  iter->params->result_type = type;
  iter->params->result_model = NORM_DEVICE;
	// Iterates.
	iterate_devices(iter);
	// Frees allocated ressources.
	free_iter(iter);
}

Files*              scan_norm(Params_Template *p)
{
  Devices *scan_ds = malloc(sizeof(Devices));
  Files *fs;
  Params_Norm *p_ = (Params_Norm*) p->params;
  // If axis is none, returns a new Files that holds readings for all
  // virtual accelerometer axis. This allows us to use the iterate
  // devices function while performing only one operation for all
  // axis.
  if(p->axis==NONE)
  {
    scan_ds->num_ds = 3;
    scan_ds->ds = malloc(sizeof(Device*)*3);
    scan_ds->ds[0] = create_d(p_->scan_type,p_->scan_model,X,0);
    scan_ds->ds[1] = create_d(p_->scan_type,p_->scan_model,Y,0);
    scan_ds->ds[2] = create_d(p_->scan_type,p_->scan_model,Z,0);
  	fs = create_fds(scan_ds);
		free_ds(scan_ds);
		if(fs->maps_num == 3)
		{
			return fs;
		}
  	scan_ds = malloc(sizeof(Devices));
  } 
	scan_ds->num_ds = 0;
  fs = create_fds(scan_ds);
  free(scan_ds);
  return fs;
}
 
// Computes the mean of an array of floats. Assumption: the function
// is only called with arrays of length > 0.
float           	  compute_result_norm(Params_Template *p)
{
  int i;
  float temp = 0;
  // Recursvie assignment.
  for(i=0;i<p->fs->maps_num;i++)
  {
      temp += powf(p->readings[i],2);
  }
  return sqrt(temp);
}

void								free_params_norm(Params_Template *p)
{
	Params_Norm *p_ = (Params_Norm*) p->params;
  free(p_);
	free_params_template(p);
}
