///////////////////////////////////////////////////////////////////////////////
// Virtual-Gyroscope
// -----------------
// File :parser.h
// PA Consulting  - Pierre Sutter
///////////////////////////////////////////////////////////////////////////////

#ifndef PARSER
#define PARSER
#include <stdbool.h>
#include "devices.h"

///////////////////////////////////////////////////////////////////////////////
// Importing functions.
///////////////////////////////////////////////////////////////////////////////

// Scans the content of the data folder for devices with specified device_type,
// models and axis. If model == enum_model_length scans the folder for all 
// devices.
Devices             *scan_folder(enum DEVICE_TYPE, enum MODEL, enum AXIS);

// Parses data from a .gyr file into a .dat file.
void                matlab_parse();

// Splits a binary file of float in the format x,y,z to three binary
// files representing each axis.
void                split_3(char *path, enum DEVICE_TYPE, enum MODEL, int axis);

///////////////////////////////////////////////////////////////////////////////
// Folder functions.
///////////////////////////////////////////////////////////////////////////////

bool                devices_exist(Devices*);

#endif
