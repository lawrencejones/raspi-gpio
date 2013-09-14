///////////////////////////////////////////////////////////////////////////////
// Virtual Gyroscope.
// ¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯
// File: generate_signal.c
// PA Consulting - Pierre Sutter
///////////////////////////////////////////////////////////////////////////////

#include "generate_signal.h"

///////////////////////////////////////////////////////////////////////////////
// Iteration function.
///////////////////////////////////////////////////////////////////////////////

void                  iterate_devices(Iterate *iter)
{
  int axis, i;
	float result = 0;
  Params_Template* p;
  // For each axis.
  for(axis=0;axis<enum_axis_length;axis++)
  {
    // Do per axis initialization.
    iter->params->axis = axis;
		(*iter->initialize_per_axis)(iter->params, iter->scan);
    p = iter->params;
		// If iterate is set.
    if(p->iterate)
    {
			// For each values.
      for(i=0;i<p->fs->maps_max;i++)
      {
        // Updates the readings.
        read_fds(p->fs,p->readings,(size_t)i);
				result = (*iter->compute_result)(iter->params);
				fwrite(&result,sizeof(float),1,p->result_f);
      }
			printf("Result written to file: %s.\n", p->d->path);
    }
    // Frees allocated ressources for the axis iteration.
		(*iter->free_per_axis)(iter->params);
  }
}

///////////////////////////////////////////////////////////////////////////////
// Template initialization functions.
///////////////////////////////////////////////////////////////////////////////

void							   initialize_per_axis_template(Params_Template* p_, void* f)
{
  Files *(*scan)(Params_Template*) = f;
  // Create the resulting device and unlink its file in case it
  // already exists.
	p_->d = create_d(p_->result_type,p_->result_model,p_->axis,0);
  unlink(p_->d->path);
	// Scans the data folder.
	p_->fs = (*scan)(p_);
  p_->iterate = false;
	// If files were actually parsed:
	if(p_->fs->maps_num > 0)
	{
		// Sets iterate to true.
		p_->iterate = true;
		p_->result_f = fopen(p_->d->path,"wb+");
		// Frees result_d
		// Initializes readings.
		p_->readings = malloc(sizeof(float)*p_->fs->maps_num);
	}
}

///////////////////////////////////////////////////////////////////////////////
// Template functions for freeing up memory.
///////////////////////////////////////////////////////////////////////////////

Iterate*						generate_iter()
{
	// Allocates memory for new Iteration and Params_Template structures.
	Iterate *iter = malloc(sizeof(Iterate));
  Params_Template *params = malloc(sizeof(Params_Template));
	// Initializes iter's fields.
	iter->initialize_per_axis = &initialize_per_axis_template;
	iter->free_per_axis = &free_per_axis_template;
	iter->free_params = &free_params_template;
  iter->params = params;
  return iter;
}

void								free_iter(Iterate* iter)
{
	(*iter->free_params)(iter->params);
	free(iter);
}

void								free_params_template(Params_Template *p)
{
	free(p);
}

void							  free_per_axis_template(Params_Template *p_)
{
	// If iterate is set free readings and closes file.
	if(p_->iterate)
	{
		free(p_->readings);
		fclose(p_->result_f);
	}
  free_fds(p_->fs);
	free_d(p_->d);
  p_->iterate = false;
}
