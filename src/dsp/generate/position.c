///////////////////////////////////////////////////////////////////////////////
// Virtual Gyroscope.
// ¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯
// File: position.c
// PA Consulting - Pierre Sutter
///////////////////////////////////////////////////////////////////////////////

#include "position.h"

/////////////////////////////////////////////////////////////////////////////////
// Position calculation.
/////////////////////////////////////////////////////////////////////////////////

float               compute_result_position(Params_Template*);
Files*              scan_position(Params_Template*);
void								free_params_position(Params_Template*);

/////////////////////////////////////////////////////////////////////////////////
// Position calculation.
/////////////////////////////////////////////////////////////////////////////////

void							  generate_position()
{
	// Allocates memory for new Iteration and Params_Template structures.
	Iterate* iter = generate_iter();
  iter->params->params = malloc(sizeof(Params_Position));
	// Initializes iter's fields.
	iter->compute_result = &compute_result_position;
	iter->free_params = &free_params_position;
  iter->scan = &scan_position;
  iter->params->result_type = POSITION_DEVICE;
  iter->params->result_model = OTHER;
	// Iterates.
	iterate_devices(iter);
	// Frees allocated ressources.
	free_iter(iter);
}

Files*            scan_position(Params_Template *p)
{
  Device *position = create_d(SPEED,OTHER,p->axis,0);
  Devices *scan_ds = malloc(sizeof(Devices));
  // If both axis exists.
  if(file_exists(position->path))
  {
    scan_ds->num_ds = 1;
    scan_ds->ds = malloc(sizeof(Device*)*1);
    scan_ds->ds[0] = position;
  } else
  {
    scan_ds->num_ds = 0;
    scan_ds->ds = malloc(sizeof(Device*));
		free_d(position);
  }
  Files *fs = create_fds(scan_ds);
  free_ds(scan_ds);
  return fs;
}

void								free_params_position(Params_Template *p)
{
	Params_Position *p_ = (Params_Position*) p->params;
  free(p_);
	free_params_template(p);
}

float           	  compute_result_position(Params_Template *p)
{
  Params_Position *p_ = (Params_Position*) p->params;
  // Updates the value of the speed.
  p_->previous += p_->time_interval*p->readings[p->axis];
  // Returns weighted average of the two readings.
  return p_->previous;
}
