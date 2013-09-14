///////////////////////////////////////////////////////////////////////////////
// Virtual Gyroscope.
// ¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯
// File: orientation.c
// PA Consulting - Pierre Sutter
///////////////////////////////////////////////////////////////////////////////

#include "orientation.h"

/////////////////////////////////////////////////////////////////////////////////
// Functions for orientation.
/////////////////////////////////////////////////////////////////////////////////

float               compute_result_orientation(Params_Template*);
Files*              scan_orientation(Params_Template*);
void                free_params_orientation(Params_Template*);
void                initialize_orientation(Params_Direction_Cosine*);
void                initialize_per_axis_orientation(Params_Template*,void*);
void                update_orientation(Params_Template *p);
void								set_zero(gsl_matrix*);

/////////////////////////////////////////////////////////////////////////////////
// Direction cosine matrix.
/////////////////////////////////////////////////////////////////////////////////

void							  generate_orientation()
{
	// Allocates memory for new Iteration and Params_Template structures.
  Iterate* iter = generate_iter();
  Params_Direction_Cosine *drc = malloc(sizeof(Params_Direction_Cosine));
  iter->params->params = drc;
  // Initializes the orientation matrix.
  initialize_orientation(drc);
	// Initializes iter's fields.
	iter->compute_result = &compute_result_orientation;
	iter->free_params = &free_params_orientation;
	iter->initialize_per_axis = &initialize_per_axis_orientation;
  iter->scan = &scan_orientation;
  iter->params->result_type = ACCELEROMETER;
  iter->params->result_model = FUSED;
	// Iterates.
	iterate_devices(iter);
	// Frees allocated ressources.
	free_iter(iter);
}

void              initialize_per_axis_orientation(Params_Template* p,void* f)
{
	// Scales all matrices to 0.
	Params_Direction_Cosine *p_ = (Params_Direction_Cosine*) p->params;
	set_zero(p_->orientation);
	set_zero(p_->sin_matrix);
	set_zero(p_->cos_matrix);
	initialize_per_axis_template(p,f);
}

void              initialize_orientation(Params_Direction_Cosine *drc)
{
  int value,i,j;
  // Assumes our object has orientation 0.
  drc->identity = gsl_matrix_alloc(3,3);
  drc->orientation = gsl_matrix_alloc(3,3);
	drc->sin_matrix = gsl_matrix_alloc(3,3);
	drc->cos_matrix = gsl_matrix_alloc(3,3);

  for(i=0;i<3;i++)
  {
    for(j=0;j<3;j++)
    {
      // We want to assign 1 to diagonal values and 0 otherwise.
      value = (int) (i==j);
      gsl_matrix_set(drc->identity,i,j,value);
    }
  }
}

Files*              scan_orientation(Params_Template *p)
{
  Devices *scan_ds = malloc(sizeof(Devices));
  Files *fs;
  // If axis is none, returns a new Files that holds readings for all
  // virtual gyroscope readings as well as the norm. This allows us 
  // to use the iterate devices function while performing only one 
  // operation for all axis.
  if(p->axis!=NONE)
  {
    scan_ds->num_ds = 7;
    scan_ds->ds = malloc(sizeof(Device*)*7);
    scan_ds->ds[0] = create_d(GYROSCOPE,VIRTUAL,X,0);
    scan_ds->ds[1] = create_d(GYROSCOPE,VIRTUAL,Y,0);
    scan_ds->ds[2] = create_d(GYROSCOPE,VIRTUAL,Z,0);
    scan_ds->ds[3] = create_d(GYROSCOPE,NORM_DEVICE,NONE,0);
    scan_ds->ds[4] = create_d(ACCELEROMETER,VIRTUAL,X,0);
    scan_ds->ds[5] = create_d(ACCELEROMETER,VIRTUAL,Y,0);
    scan_ds->ds[6] = create_d(ACCELEROMETER,VIRTUAL,Z,0);
  	fs = create_fds(scan_ds);
		free_ds(scan_ds);
		// If the number of opened files corresponds to the number of
		// required devices, return otherwise frees devices.
		if(fs->maps_num == 7)
		{
			return fs;
		}
    free_fds(fs);
  	scan_ds = malloc(sizeof(Devices));
  } 
  scan_ds->num_ds = 0;
  fs = create_fds(scan_ds);
  free(scan_ds);
  return fs;
}

void           	    update_orientation(Params_Template *p)
{
  float sin_coeff,cos_coeff,norm;
  Params_Direction_Cosine *p_= (Params_Direction_Cosine*) p->params;
  gsl_matrix *orientation,*sin_matrix,*cos_matrix,*identity;
  orientation = p_->orientation;
  sin_matrix = p_->sin_matrix;
  cos_matrix = p_->cos_matrix;
  identity = p_->identity;
  // Computes coefficients.
  norm = p->readings[3];
  sin_coeff = sinf(norm) / norm;
  cos_coeff = (1-cosf(norm)) / powf(norm,2);
  // Initializes coefficients matrices and computes them.
  set_zero(sin_matrix);
  set_zero(cos_matrix);
  gsl_matrix_add(sin_matrix,orientation);
  gsl_matrix_add(cos_matrix,orientation);
  gsl_matrix_scale(sin_matrix,sin_coeff);
  gsl_matrix_scale(cos_matrix,cos_coeff);
  // Computes new Orientation Matrix.
  set_zero(orientation);
  gsl_matrix_add(orientation,identity);
  gsl_matrix_add(orientation,sin_matrix);
  gsl_matrix_add(orientation,cos_matrix);
}

// Updates the orientation matrix and outputs the oriented
// accelerometer reading.
float           	  compute_result_orientation(Params_Template *p)
{
  int j;
  float result = 0;
  Params_Direction_Cosine *p_ = (Params_Direction_Cosine*) p->params;
  // Updates the state of the direction matrix.
  update_orientation(p);
  // Computes the oriented value of the accelerometer.
  for(j=0;j<3;j++)
  {
    result += p->readings[4+j] + gsl_matrix_get(p_->orientation,p->axis,j);
  }
  return result;
}

void								free_params_orientation(Params_Template *p)
{
	Params_Direction_Cosine *p_ = (Params_Direction_Cosine*) p->params;
  gsl_matrix_free(p_->orientation);
  gsl_matrix_free(p_->identity);
	gsl_matrix_free(p_->sin_matrix);
	gsl_matrix_free(p_->cos_matrix);
  free(p_);
	free_params_template(p);
}

void								set_zero(gsl_matrix* g)
{
	int i,j;
	for(i=0;i<g->size1;i++)
	{
		for(j=0;j<g->size2;j++)
		{
			gsl_matrix_set(g,i,j,0);
		}
	}
}


