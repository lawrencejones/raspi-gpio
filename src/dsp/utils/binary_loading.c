///////////////////////////////////////////////////////////////////////////////
// C Group Project - First Year
// ¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯
// File: binaryLoading.c
// Group: 21
// Members: amv12, lmj112, skd212
///////////////////////////////////////////////////////////////////////////////

#include <stdio.h>
#include "binary_loading.h"

int get_size(FILE *file)
{
  // use fseek to set relative pointer pos in file
  fseek(file, 0, SEEK_END);
  // get size from current relative position of the seek
  // inside the file
  long unsigned int size = ftell(file);
  // reset the offset to the beginning of the file
  fseek(file, 0, SEEK_SET);
  return size;
}

int get_size_path(char *path)
{
  FILE *file = fopen(path,"rb");
  // use fseek to set relative pointer pos in file
  fseek(file, 0, SEEK_END);
  // get size from current relative position of the seek
  // inside the file
  long unsigned int size = ftell(file);
  // reset the offset to the beginning of the file
  fseek(file, 0, SEEK_SET);
  fclose(file);
  return size;
}

bool file_exists(char *path)
{
	FILE *f = fopen(path,"r");
	if(f!=NULL)
	{
		fclose(f);
		return true;
	}
	return false;
}
