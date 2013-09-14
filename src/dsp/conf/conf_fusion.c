///////////////////////////////////////////////////////////////////////////////
// Virtual-Gyroscope
// -----------------
// File :conf_fusion.c
// PA Consulting  - Pierre Sutter
///////////////////////////////////////////////////////////////////////////////

#include <unistd.h>
#include <stdio.h>
#include "conf_fusion.h"
#include "../utils/binary_loading.h"

///////////////////////////////////////////////////////////////////////////////
// Global variables.
///////////////////////////////////////////////////////////////////////////////

static const char* conf_fusion_path = "data/conf/conf_fusion.dat";
static float conf_fusion_wg = 20.0;

///////////////////////////////////////////////////////////////////////////////
// Read and write configurations.
///////////////////////////////////////////////////////////////////////////////

// Returns the read value of conf_fusion_path.
float							read_wg()
{
	FILE *f = fopen(conf_fusion_path,"rb");
	// If f is not null and has size of expected value return the value
	// otherwise write conf_fusion_wg.
	if(!f)
	{
		float result;
		if(get_size(f) == sizeof(conf_fusion_wg))
		{
			fread(&result,sizeof(result),1,f);
			fclose(f);
			return result;
		}
		fclose(f);
	}
	write_wg(conf_fusion_wg);
	return conf_fusion_wg;
}

// Writes the new given value to the configuration file for fusion.
void							write_wg(float value)
{
	// Unlinks previous configuration.
	unlink(conf_fusion_path);
	FILE *f = fopen(conf_fusion_path,"+wb");
	if(!f)
	{
		fwrite(&conf_fusion_wg,sizeof(conf_fusion_wg),1,f);
		fclose(f);
	}
}
