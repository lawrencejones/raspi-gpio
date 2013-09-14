///////////////////////////////////////////////////////////////////////////////
// Virtual-Gyroscope
// -----------------
// File :conf_vgyro.h
// PA Consulting  - Pierre Sutter
///////////////////////////////////////////////////////////////////////////////

#ifndef CONFVGYRO
#define CONFVGYRO
#include <gsl/gsl_matrix.h>

///////////////////////////////////////////////////////////////////////////////
// Configuration.
///////////////////////////////////////////////////////////////////////////////

// Struct for the configuration of the Virtual Gyroscope.
typedef struct
{
	int 	 				n;          // Number of gyroscopes.
	float  				tau;       	// Process time constant.
	float  				qomega;    	// Variance of the system.
	float  				samplingp; 	// Sampling Period of the system.
	float  				sigma;     	// The ARW variance of the component gyroscopes.
	gsl_matrix 		*cross_corr;// Our cross-correlation matrix; 
} Conf;

///////////////////////////////////////////////////////////////////////////////
// Static configuration.
///////////////////////////////////////////////////////////////////////////////

// Values for standard model.
static const int 		n_stat = 4;
static const float 	sigma_stat = 6.17;
static const float 	qomega_stat = 0.07;
static const float 	samplingp_stat = 0.02;
static const float 	tau_stat = 500;
static const float 	cross_corr_stat[4][4] = 
{
	{ 1, 0.3, 0.3, 0.3},
	{ 0.3, 1, 0.3, 0.3},
	{ 0.3, 0.3, 1, 0.3},
	{ 0.3, 0.3, 0.3, 1}
};

///////////////////////////////////////////////////////////////////////////////
// Loading functions.
///////////////////////////////////////////////////////////////////////////////

// Loads the standard hardcoded configuration for the project.
Conf* 						load_config_static(void);
Conf* 						load_config_data(void);
// Creates a custom configuration for the model.
Conf* 						load_config_custom(int n, float tau, float qomega, float samplingp, float sigma, float** cross_corr_arr);

///////////////////////////////////////////////////////////////////////////////
// Actions.
///////////////////////////////////////////////////////////////////////////////

// Frees all ressources allocated by a conf.
void 							free_conf(Conf*);
void 							write_conf(Conf*);
void 							display_conf(Conf*);

#endif
