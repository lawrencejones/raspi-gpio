///////////////////////////////////////////////////////////////////////////////
// Virtual Gyroscope
// ¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯
// File: kalman_filter.c
// PA Consulting  - Pierre Sutter
///////////////////////////////////////////////////////////////////////////////

#include <gsl/gsl_matrix.h>
#include <gsl/gsl_linalg.h>
#include <math.h>
#include "kalman_filter.h"

///////////////////////////////////////////////////////////////////////////////
// Helper Function.
///////////////////////////////////////////////////////////////////////////////

void inverse(gsl_matrix*, int, gsl_matrix*);

///////////////////////////////////////////////////////////////////////////////
// Creation, deletion.
///////////////////////////////////////////////////////////////////////////////

Kalman* build_kalman(Conf* conf)
{
	float f = 1 / conf->tau;
	int i, j;

	// Computation of the covariance matrix.
	gsl_matrix *r = gsl_matrix_alloc (conf->n, conf->n);
	for(i = 0; i<conf->n; i++)
		for(j = 0; j<conf->n; j++)
			gsl_matrix_set(r,i,j,gsl_matrix_get(conf->cross_corr,i,j)*conf->sigma);

	// Computes the value of the inverse of r.
	gsl_matrix *invr = gsl_matrix_alloc (conf->n, conf->n);
	inverse(r, conf->n, invr); 

	// Computes d
	gsl_matrix *d = gsl_matrix_alloc (1, conf->n);
	float column = 0;
	for(i = 0; i<conf->n; i++)
	{
		column = 0;
		for(j = 0; j<conf->n; j++)
		{
			column += gsl_matrix_get(invr,i,j);
		}
		gsl_matrix_set(d,0,i,column);
	}

	float d_result = 0;
	for(i = 0; i<conf->n; i++)
	{
		d_result += gsl_matrix_get(d,0,i);
	}

	// Computes the Optimal Kalman Gain
	float coeff = (1/d_result) * (-f + sqrt( f*f + conf->qomega*d_result));
	gsl_matrix *k = gsl_matrix_alloc (1, conf->n);
	for(i = 0; i<conf->n; i++)
	{
		column = 0;
		for(j = 0; j<conf->n; j++)
		{
			column += gsl_matrix_get(invr,i,j);
		}

		gsl_matrix_set(k,0,i,column*coeff);
	}

	// Computes a
	float a = 0;
	column = 0;
	for(i = 0; i<conf->n; i++)
	{
		column += gsl_matrix_get(k,0,i);
	}
	a = -(f + column);

	
	// Computes EAT
	float eat = exp(a*conf->samplingp);

	// Computes inv_a 
	float inv_a = 1/a;

	// Updates the Kalman filter structure.
	Kalman *kf = malloc(sizeof(Kalman));
	kf->eat = eat;
	kf->k = k;
	kf->inv_a = inv_a;
	kf->num_elems = conf->n;


	// Frees the memory used by the matrices computed throughout the
	// function.
	gsl_matrix_free(d);
	gsl_matrix_free(r);
	gsl_matrix_free(invr);

	return kf;
}

// Frees all ressources allocated to a Kalman filter.
void free_kalman(Kalman* kf)
{
	gsl_matrix_free(kf->k);
	free(kf);
}

///////////////////////////////////////////////////////////////////////////////
// Helper Function.
///////////////////////////////////////////////////////////////////////////////

// Computes the inverse of gsl_matrix.
void inverse(gsl_matrix *m, int n, gsl_matrix *inv)
{
	gsl_permutation *p = gsl_permutation_alloc(n);
	int s;
	gsl_linalg_LU_decomp(m, p, &s);
	gsl_linalg_LU_invert(m, p, inv);
	gsl_permutation_free(p);
}

///////////////////////////////////////////////////////////////////////////////
// Display Function.
///////////////////////////////////////////////////////////////////////////////

void display_kf(Kalman *kf)
{
	int j;
	// Prints final configuration.
	printf("Computed value of eat is %f.\n", kf->eat);
	printf("Computed value of inv_a is %f.\n", kf->inv_a);
	printf("Computed value of the Kalman Optimal Gain is:\n");
	for(j = 0; j<kf->num_elems; j++)
	{
		printf("%f ", gsl_matrix_get(kf->k,0,j));
	}
	printf("\n");
}
