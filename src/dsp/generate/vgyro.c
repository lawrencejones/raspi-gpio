///////////////////////////////////////////////////////////////////////////////
// Virtual Gyroscope.
// ¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯
// File: vgyro.c
// PA Consulting - Pierre Sutter
///////////////////////////////////////////////////////////////////////////////

#include "vgyro.h"

/////////////////////////////////////////////////////////////////////////////////
// Generate Virtual Gyroscopes functions.
/////////////////////////////////////////////////////////////////////////////////

float           	compute_result_vgyro(Params_Template*);
void							initialize_per_axis_vgyro(Params_Template*,void*);
void							free_params_vgyro(Params_Template*);
Files*            scan_vgyro(Params_Template*);

/////////////////////////////////////////////////////////////////////////////////
// Generate Virtual Gyroscope.
/////////////////////////////////////////////////////////////////////////////////

void							generate_vgyro()
{
	// Allocates memory for new Iteration and Params_Vaccel structures.
	Iterate *iter = generate_iter();
  Params_Vgyro *params_ = malloc(sizeof(Params_Vgyro));
  // Create a kalman filter using the data configuration for it.
  Conf *c = load_config_data();
  params_->kf = build_kalman(c);
  free_conf(c);
	// Initializes iter's fields.
	iter->compute_result = &compute_result_vgyro;
	iter->initialize_per_axis = &initialize_per_axis_vgyro;
	iter->free_params = &free_params_vgyro;
  iter->scan = &scan_vgyro;
  iter->params->params = params_;
  iter->params->result_type = GYROSCOPE;
  iter->params->result_model = VIRTUAL;
	// Iterates.
	iterate_devices(iter);
	// Frees allocated ressources.
	free_iter(iter);
}

Files*            scan_vgyro(Params_Template *p)
{
  Devices *ds;
  if(p->axis!=NONE)
  {
    ds = scan_folder(GYROSCOPE,enum_model_length,p->axis);
  }
  else
  {
    ds = malloc(sizeof(Devices));
    ds->num_ds = 0;
    ds->ds = malloc(sizeof(Device*));
  }
  Files* fs = create_fds(ds);
  free_ds(ds);
  return fs;
}

void							initialize_per_axis_vgyro(Params_Template* p, void* f)
{
  // Calls intiailize_per_axis_template.
  initialize_per_axis_template(p,f);
  // If iterate is set, computes the value of next_value.
  Params_Vgyro *p_ = (Params_Vgyro*) p->params;
  if(p->iterate)
  {
    p_->next_value = 0;
  }
}

// If the number of found files is accounted for by the configured
// Kalman filter, uses the configure Kalman filter to compute the next
// value of the signal, otherwise, performs a simple arithmetic mean.
float           	  compute_result_vgyro(Params_Template *p)
{
  int i;
  float temp = 0, result = 0;
	Params_Vgyro *p_ = (Params_Vgyro*) p->params;
  // If right number of values for Kalman:
  if(p_->kf->num_elems == p->fs->maps_num)
  {
    // Recursive assignment.
    p_->curr_value = p_->next_value;
    for(i=0;i<p->fs->maps_num;i++)
    {
        temp += (p->readings[i])*gsl_matrix_get(p_->kf->k,0,i);
    }
    result = p_->kf->eat*(p_->curr_value) + p_->kf->inv_a*(p_->kf->eat -1)*temp;
  }
  else
  {
    for(i=0;i<p->fs->maps_num;i++)
    {
      result += p->readings[i];
    }
    result /= p->fs->maps_num;
  }
  return result;
}

void								free_params_vgyro(Params_Template *p)
{
	Params_Vgyro *p_ = (Params_Vgyro*) p->params;
  free_kalman(p_->kf);
  free(p_);
	free_params_template(p);
}
