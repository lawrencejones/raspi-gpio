///////////////////////////////////////////////////////////////////////////////
// Virtual Gyroscope.
// ¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯
// File: generate_signal.h
// PA Consulting - Pierre Sutter
///////////////////////////////////////////////////////////////////////////////

#ifndef GENERATE_SIGNAL
#define GENERATE_SIGNAL
#include <stdlib.h>
#include <unistd.h>
#include <math.h>
#include <gsl/gsl_math.h>
#include <gsl/gsl_matrix.h>
#include "../utils/binary_loading.h"
#include "../devices.h"
#include "../parser.h"
#include "../conf/conf_vgyro.h"
#include "../kalman_filter.h"

///////////////////////////////////////////////////////////////////////////////
// Include all headers for signal generation.
///////////////////////////////////////////////////////////////////////////////

#include "norm.h"
#include "fusion.h"
#include "vgyro.h"
#include "position.h"
#include "gravity.h"
#include "orientation.h"
#include "vaccel.h"

///////////////////////////////////////////////////////////////////////////////
// Define BASE_PARAMETER and parameters structs.
///////////////////////////////////////////////////////////////////////////////

typedef struct
{
  int               axis;
	bool 				      iterate; 					
	float				      *readings;				
	FILE				      *result_f;				
	Device						*d;
  enum DEVICE_TYPE  result_type;      
  enum MODEL        result_model;     
	Files				      *fs;							
  void              *params;
} Params_Template;

///////////////////////////////////////////////////////////////////////////////
// Iterate structure.
///////////////////////////////////////////////////////////////////////////////

typedef struct
{
	float               (*compute_result)(Params_Template*);
	void                (*initialize_per_axis)(Params_Template*, void*);
  Files               *(*scan)(Params_Template*);
	void                (*free_per_axis)(Params_Template*);
	void                (*free_params)(Params_Template*);
  void                (*write_result)(Params_Template*);
	Params_Template     *params;
} Iterate;

///////////////////////////////////////////////////////////////////////////////
// Iterate function.
///////////////////////////////////////////////////////////////////////////////

void 									iterate_devices(Iterate*);

///////////////////////////////////////////////////////////////////////////////
// Template functions.
///////////////////////////////////////////////////////////////////////////////

void							  	free_iter(Iterate*);
void							  	free_params_template(Params_Template*);
void                  free_per_axis_template(Params_Template*);
void							    initialize_per_axis_template(Params_Template*,void*);
Iterate*						  generate_iter(void);
void                  write_result_template(Params_Template*);

#endif
