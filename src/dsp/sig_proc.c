///////////////////////////////////////////////////////////////////////////////
// Virtual Gyroscope
// ¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯
// File: sig_proc.c
// PA Consulting  - Pierre Sutter
///////////////////////////////////////////////////////////////////////////////

#include <stdio.h>
#include "string.h"
#include "sig_proc.h"
#include "accur_anal.h"
#include "devices.h"
#include "conf/conf_vgyro.h"
#include "kalman_filter.h"
#include "generate/generate_signal.h"
#include "utils/interface_utils.h"
#include "parser.h"

#define FLAG(s,r) if(!strcmp(s,str)) { return r; }
#define CASE(e,f) case e: { (void)(*f)(); } break;
#define CASE_ARGS(e,f,argc,argv) case e: { (void)(*f)(argc,argv); } break;

///////////////////////////////////////////////////////////////////////////////
// Main function.
///////////////////////////////////////////////////////////////////////////////

void sig_drift()
{
  Conf *c = load_config_data();
  free_conf(c);
}

int main(int argc, char** argv)
{
	int enum_index = 0;
	// If there are arguments.
	if(argc>=2)
	{
		enum_index = string_to_enum(argv[1]);
		switch(enum_index)
		{
			CASE(VGYRO_COM,&sig_vgyro)
			CASE(SPEED_COM,&sig_speed)
			CASE(POSITION_COM,&sig_position)
			CASE(FUSION_COM,&sig_fusion)
			CASE(ARW,&sig_arw)
			CASE(DRIFT,&sig_drift)
      CASE_ARGS(RAW,&sig_raw,argc,argv)
			//CASE(RAW,&sig_raw)
			//CASE(DRIFT:
				/*
       struct directory_scanning* ds = get_names(1, 0);
			 struct data_handle* dh = data_handle(ds);
       update_handle(dh);
			 free_handle(dh);
       free_ds(ds);
			 */
			//CASE(HELP:
			CASE(IMPORT,&sig_import)
			CASE(VACCEL_COM,&sig_vaccel)
			CASE(ORIENTATION_COM,&sig_orientation)
			//CASE(INVALID,&sig_invalid)printf("Invalid Argument. Type 'sig_proc -help'");
		//		printf(" to have a list of the available commands.\n"); 
		}
	}
	return 0;
}


///////////////////////////////////////////////////////////////////////////////
// Enumeration.
///////////////////////////////////////////////////////////////////////////////

// Returns the int correspoding to the index of the command in enum
// COMMANDS. In the command is not found, returns the index of
// invalid.
int string_to_enum(char *str)
{
	FLAG("-speed", SPEED_COM)
	FLAG("-position", POSITION_COM)
	FLAG("-vgyro", VGYRO_COM)
	FLAG("-orientation", ORIENTATION_COM)
	FLAG("-fusion", FUSION_COM)
	FLAG("-arw", ARW)
	FLAG("-import", IMPORT)
	FLAG("-help", HELP)
	FLAG("-vaccel", VACCEL_COM)
	FLAG("-drift", DRIFT)
	FLAG("-raw", RAW)
	return (int) INVALID;
}

///////////////////////////////////////////////////////////////////////////////
// Wrapper Functions.
///////////////////////////////////////////////////////////////////////////////

// Generates virtual gyroscope readings.
void sig_vgyro()
{
	generate_vgyro();
}

// Performs Allan Variance analysis.
void sig_arw()
{
	compute_allan();
}

void sig_fusion()
{
  //sensor_fusion_data();
}

void sig_orientation()
{
	// Computes the norm of the gyroscope signal.
	generate_norm(GYROSCOPE,VIRTUAL);
	generate_orientation();
}

void sig_position()
{
	generate_position();
}

void sig_speed()
{
	generate_gravity();
}

void sig_import()
{
	matlab_parse();
}

void sig_vaccel()
{
  generate_vaccel();
}

// Ugly implementation of a last minute change.
void sig_raw(int argc, char **argv)
{
  if(argc == 6)
  {
    int type = str_to_typ(argv[3]);
    int mod = str_to_mod(argv[4]);
    // If arguments have been succesfully parsed and file succesfuly
    // opened.
    if(type!= -1 && mod != -1)
    {
      Device *d = create_d(type,mod,X,atoi(argv[5]));
      split_axis(argv[2],(enum DEVICE_TYPE) type,(enum MODEL) mod,atoi(argv[5]));
      free_d(d);
    }
  }
}


