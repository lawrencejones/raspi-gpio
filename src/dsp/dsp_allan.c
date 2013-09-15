///////////////////////////////////////////////////////////////////////////////
// Virtual Gyroscope
// ¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯
// File: dsp_allan.c
// PA Consulting  - Pierre Sutter
///////////////////////////////////////////////////////////////////////////////

#include <stdio.h>
#include <math.h>
#include <unistd.h>
#include "dsp.h"

///////////////////////////////////////////////////////////////////////////////
// COMPUTE
///////////////////////////////////////////////////////////////////////////////

// For the given array of floats representing a single sensors
// axes, return a struct representing the Allan Variance
AllanVariance *dsp_allan_compute(float *axis, int length)
{

}
// Computes the Allan Variance for each of the gyroscopes in the data
// directory.
void compute_allan()
{
	int axis;
	Devices *ds;
	for(axis=0;axis<enum_axis_length;axis++)
	{
		ds = scan_folder(GYROSCOPE,enum_model_length,axis);
	  iterate_ds(ds,*iterate_allan);
	  free_ds(ds);
	}
}

///////////////////////////////////////////////////////////////////////////////
// Helper functions.
///////////////////////////////////////////////////////////////////////////////

// Iterates through the devices_axis of a devices_type andcalls
// compute_allan_da for each of them.
void iterate_allan(Device* d)
{
	// Creates the corresponding allan file.
	Device* d_all = create_d(ALLAN,d->mod,d->ax,d->index);
	
	// Unlinks the previous Allan file.
	unlink(d_all->path);
	FILE *in = fopen(d->path,"rb");
	FILE *out = fopen(d_all->path,"wb+");
	if (in != NULL && out != NULL)
  {
	 printf("Analyzing file at location %s.\n",d->path);
	 compute_allan_(in,out);
   printf(" result written at location %s.\n",d_all->path);
  }
	if (in != NULL)
	 fclose(in);
	if (out != NULL)
		fclose(out);
	free_d(d_all);
}

// Compute the Allan Variance for the given file.
void compute_allan_(FILE *in, FILE *out)
{
	int max_i;		 					// The maximum number of iterations
	int tau;								// The integration time constant
	int no_of_clusters;			// The number of clusters to be created
	float sum_sd;						// Sum of the squared differences
	float sum_cluster;			// Holds the sum of clustered values

	// Computes the maximum number of iterations
	max_i = log(length/2.0)/log(2.0);
  
	for(i=0; i < max_i; i++)
	{
		// Computes tau, our time interval.
		tau = pow(2,i+1);
		// Computes the number of clusters
		no_of_clusters = length/tau;
		sum_sd = 0;
		// Iterates over the file, parsing in into clusters.
		int clusts_index, j;
		// Temporary variables that hold
		float value_, value_r;
		// the mean of the summed up values over one cluster.
		float allan_var;

		for(clusts_index=0;clusts_index<clusts;clusts_index++)
		{
			sum_cluster = 0;
			// Computes the sum of the values over the one cluster of
			// readings.
			for(j=0; j<tau; j++)
			{
				sum_cluster += in_values[clusts_index*tau + j];
			}
			// Averages the signals from the clusters.
			value_ = sum_cluster/tau; 
			// If this is not the first reading, computes the sum of the
			// squared differences of value_ and value_r.
			if(clusts_index)
			{
				sum_sd += pow((value_r - value_),2);
			}
			// Updates the values of value_ for recursion,
			value_r = value_;
		}
		// Computes the actual value of the allan variance.
		allan_var = pow(0.5*(sum_sd/clusts),0.5);
		// Writes the value of the allan variance as well as the tau
		// constant.
		fwrite(&allan_var,sizeof(float),1,out);
		fwrite(&tau,sizeof(int),1,out);
	}
	munmap(in_values,get_size(in));
}
