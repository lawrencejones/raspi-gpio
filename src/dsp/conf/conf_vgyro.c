///////////////////////////////////////////////////////////////////////////////
// Virtual-Gyroscope
// -----------------
// File :conf_vgyro.h
// PA Consulting  - Pierre Sutter
///////////////////////////////////////////////////////////////////////////////

#include <unistd.h>
#include <string.h>
#include "../utils/binary_loading.h"
#include "conf_vgyro.h"

static const char *conf_static_path = "data/confs/conf.dat";

///////////////////////////////////////////////////////////////////////////////
// Loading functions.
///////////////////////////////////////////////////////////////////////////////

// Returns the standard configuration ot the model.
Conf* load_config_static(void)
{
	// Allocates memory for the conf.
	Conf* c = malloc(sizeof(Conf));

	// Copies static values into the configuration.
	c->n = n_stat;
	c->qomega = qomega_stat;
	c->tau = tau_stat;
	c->sigma = sigma_stat;
	c->samplingp = samplingp_stat;

	// Copies the covariance matrix.
	int i, j;
	gsl_matrix *cross_corr = gsl_matrix_alloc(n_stat,n_stat);
	for(i=0; i<n_stat; i++)
		for(j=0; j<n_stat; j++)
		{
			gsl_matrix_set(cross_corr,i,j,cross_corr_stat[i][j]);
		}

	c->cross_corr = cross_corr;

	return c;
}

// Creates a custom configuration
Conf* load_config_custom(int n, float tau, float qomega, float samplingp, float sigma, float** cross_corr_arr)
{
	// Allocates memory for the conf.
	Conf* c = malloc(sizeof(Conf));

	// Copies static values into the configuration.
	c->n = n;
	c->qomega = qomega;
	c->tau = tau;
	c->sigma = sigma;
	c->samplingp = samplingp;

	// Copies the covariance matrix.
	int i, j;
	gsl_matrix *cross_corr = gsl_matrix_alloc(n,n);
	for(i=0; i<n; i++)
		for(j=0; j<n; j++)
			gsl_matrix_set(cross_corr,i,j,cross_corr_arr[i][j]);

	c->cross_corr = cross_corr;

	return c;
}

Conf* load_config_data()
{
	FILE *f = fopen(conf_static_path, "r");

	if(f!=NULL)
	{
		int read_int;
		float *read_floats;
		fread(&read_int, sizeof(int), 1 ,f);
		int num_floats = read_int*read_int + 4;

		// Checks the size of the config file.
		if(get_size(f)==(sizeof(int)+sizeof(float)*num_floats))
		{
			Conf* c = malloc(sizeof(Conf));

			read_floats = malloc(sizeof(float)*num_floats);

			fseek(f,sizeof(int),SEEK_SET);

			fread(read_floats, sizeof(float), num_floats, f);

			c->n = read_int;
			c->tau = read_floats[0];
			c->qomega = read_floats[1];
			c->samplingp = read_floats[2];
			c->sigma = read_floats[3];
			c->cross_corr = gsl_matrix_alloc(read_int, read_int);

			int i,j;
			for(i=0;i<read_int;i++)
				for(j=0;j<read_int;j++)
				{	
					gsl_matrix_set(c->cross_corr,i,j,read_floats[(4+i*read_int + j)]);
				}

      // Closes opened file.
      fclose(f);
      // Frees up array of floats readings.
      free(read_floats);

			return c;
		}
	}
	// If data could not be loaded properly from the file, returns the
	// static data. And writes it to the data folder.
	Conf *c;
	c = load_config_static();
	write_conf(c);
	return c;
}

///////////////////////////////////////////////////////////////////////////////
// Free.
///////////////////////////////////////////////////////////////////////////////

// Frees all ressources allocated by a conf.
void free_conf(Conf* c)
{
	gsl_matrix_free(c->cross_corr);
	free(c);
}

///////////////////////////////////////////////////////////////////////////////
// Actions.
///////////////////////////////////////////////////////////////////////////////

// Writes the content of the conf structure to the data folder
void write_conf(Conf* c)
{
	unlink(conf_static_path);
	FILE *f = fopen(conf_static_path, "w+");

	if(f!=NULL)
	{
		fwrite(&c->n,sizeof(int),1,f);
		fwrite(&c->tau,sizeof(float),1,f);
		fwrite(&c->qomega,sizeof(float),1,f);
		fwrite(&c->samplingp,sizeof(float),1,f);
		fwrite(&c->sigma,sizeof(float),1,f);

		// If the dimension of the matrix doesn't correspond to n then
		// allocate a new one with corresponding dimensions.
		if(!((c->n == c->cross_corr->size1) && (c->cross_corr->size1 == c->cross_corr->size2)))
		{
			// Frees up previous matrix.
			gsl_matrix_free(c->cross_corr);
			// Allocates new one.
			c->cross_corr = gsl_matrix_alloc(c->n,c->n);
			// Scales the matrix to 0.
			gsl_matrix_scale(c->cross_corr,0);
		}

		// Writes matrix to configuration file.
		int i,j;
		for(i=0;i<c->n;i++)
			for(j=0;j<c->n;j++)
			{
				float matrix_value = gsl_matrix_get(c->cross_corr,i,j);
				fwrite(&matrix_value,sizeof(float),1,f);
			}
		fclose(f);
	}
}

// Displays a configuration on the screen.
void display_conf(Conf* c)
{
	printf("Number of gyroscopes is %d.\n", c->n);
	printf("The value of tau is %f.\n", c->tau);
	printf("The value of qomega is %f.\n", c->qomega);
	printf("The value of samplingp is %f.\n", c->samplingp);
	printf("The value of sigma is %f.\n", c->sigma);
	printf("Printing the cross_correlation matrix:\n");
	int i,j;
	for(i=0;i<c->n;i++)
	{
		for(j=0;j<c->n;j++)
		{
			printf("%f ", gsl_matrix_get(c->cross_corr,i,j));
		}
		printf("\n");
	}
}
