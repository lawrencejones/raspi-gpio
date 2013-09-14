///////////////////////////////////////////////////////////////////////////////
// Virtual-Gyroscope
// -----------------
// File :devices.h
// PA Consulting  - Pierre Sutter
///////////////////////////////////////////////////////////////////////////////

#ifndef DEVICES
#define DEVICES
#include <stdbool.h>

///////////////////////////////////////////////////////////////////////////////
// Enumerations of devices, axis, models.
///////////////////////////////////////////////////////////////////////////////

// An enumeration of the different possible types of devices.
static const int enum_device_type_length = 6;
enum DEVICE_TYPE
{
	ACCELEROMETER=0, GYROSCOPE, COMPASS, ALLAN, SPEED, POSITION_DEVICE
};

// An enumeration of the different possible axis for a device.
static const int enum_axis_length = 4;
enum AXIS
{
	X=0, Y, Z, NONE
};

static const int enum_model_length = 10;
enum MODEL
{
	MATLAB=0, ITG, MPU, L3G4, MATH, CONVERTED, VIRTUAL, FUSED, OTHER, NORM_DEVICE, MMA
};


///////////////////////////////////////////////////////////////////////////////
// Struct device, devices.
///////////////////////////////////////////////////////////////////////////////

typedef struct
{
	enum DEVICE_TYPE    dt;   	// Type of the device.
	enum MODEL          mod;    // Model of the device.
	enum AXIS           ax;     // Axis of the device.
	int                 index;  // Index of the device.
	char*               path;   // The path to the current device is a 40 characters string.
} Device;

typedef struct
{
	int                num_ds; // Number of devices in the struct.
	Device**           ds;    	// Array of devices.
} Devices;

///////////////////////////////////////////////////////////////////////////////
// Struct file handle.
///////////////////////////////////////////////////////////////////////////////

typedef struct
{
	int 							maps_max;   // The maximum number of floats to be read.
	int 							maps_num;   // The number of maps held by the struct.
	int 							*maps_size; // Holds the sizes of all the files.
	float							**maps;     // Arrays of memory maps.
} Files;

///////////////////////////////////////////////////////////////////////////////
// Create, free structures.
///////////////////////////////////////////////////////////////////////////////

Device* 					create_d(enum DEVICE_TYPE, enum MODEL, enum AXIS, int);
Files* 						create_fds(Devices* ds);
void 							free_ds(Devices*);
void 							free_d(Device*);
void 							free_fds(Files*);

///////////////////////////////////////////////////////////////////////////////
// Utilitaries.
///////////////////////////////////////////////////////////////////////////////

// Iterate over each devices held by device with a function of type 
// void f(Device*)
void 								iterate_ds(Devices *,void (*)(Device*));
// Write values held in files->maps at offset for each map in buff.
void 								read_fds(Files *, float *buff, size_t offset);

#endif 
