///////////////////////////////////////////////////////////////////////////////
// C Group Project - First Year
// ¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯
// File: binaryLoading.h
// Group: 21
// Members: amv12, lmj112, skd212
///////////////////////////////////////////////////////////////////////////////

#ifndef BIN_LOAD_H
#define BIN_LOAD_H
#include <stdbool.h>
#include <stdio.h>

bool file_exists(char *path);
int get_size(FILE *file);
int get_size_path(char *path);

#endif
