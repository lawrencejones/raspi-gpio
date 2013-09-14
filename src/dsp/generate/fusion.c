///////////////////////////////////////////////////////////////////////////////
// Virtual Gyroscope.
// ¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯
// File: fusion.c
// PA Consulting - Pierre Sutter
///////////////////////////////////////////////////////////////////////////////

#include "fusion.h"
#include "../conf/conf_fusion.h"

////////////////////////////////////////////////////////////////////////////////
// Functions for sensor fusion.
/////////////////////////////////////////////////////////////////////////////////

float               compute_result_fusion(Params_Template*);
Files*              scan_fusion(Params_Template*);
void								free_params_fusion(Params_Template*p);

/////////////////////////////////////////////////////////////////////////////////
// Sensor fusion.
/////////////////////////////////////////////////////////////////////////////////

void							  generate_fusion()
{
	// Allocates memory for new Iteration and Params_Template structures.
  Iterate* iter = generate_iter();
	// Initializes iter's fields.
	iter->compute_result = &compute_result_fusion;
	iter->free_params = &free_params_fusion;
  iter->scan = &scan_fusion;
  iter->params->result_type = ACCELEROMETER;
  iter->params->result_model = FUSED;
  // Load configuration.
	Params_Fusion* p_ = malloc(sizeof(p_));
	p_->wg = read_wg();
	iter->params->params = p_;
	// Iterates.
	iterate_devices(iter);
	// Frees allocated ressources.
	free_iter(iter);
}

Files*            scan_fusion(Params_Template *p)
{
  Device *vaccel = create_d(ACCELEROMETER,VIRTUAL,p->axis,0);
  Device *vgyro = create_d(ACCELEROMETER,CONVERTED,p->axis,0);
  Devices *scan_ds = malloc(sizeof(Devices));
  Files *fs = malloc(sizeof(Files));
  // If both axis exists.
  if(file_exists(vaccel->path)&&file_exists(vgyro->path))
  {
    scan_ds->num_ds = 2;
    scan_ds->ds = malloc(sizeof(Device*)*2);
    scan_ds->ds[0] = vaccel;
    scan_ds->ds[1] = vgyro;
  } else
  {
    scan_ds->num_ds = 0;
    scan_ds->ds = malloc(sizeof(Device*));
  }
  fs = create_fds(scan_ds);
  free_ds(scan_ds);
  return fs;
}

void								free_params_fusion(Params_Template *p)
{
	Params_Fusion *p_ = (Params_Fusion*) p->params;
  free(p_);
	free_params_template(p);
}

// Computes the mean of an array of floats. Assumption: the function
// is only called with arrays of length > 0.
float           	  compute_result_fusion(Params_Template *p)
{
  Params_Fusion *p_ = (Params_Fusion*) p->params;
  // Returns weighted average of the two readings.
  return (p->readings[0] + p->readings[1]*(p_->wg))/(1 + p_->wg);
}
