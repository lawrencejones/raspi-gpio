///////////////////////////////////////////////////////////////////////////////
// Virtual-Gyroscope
// -----------------
// File :sig_proc.h
// PA Consulting  - Pierre Sutter
///////////////////////////////////////////////////////////////////////////////

#ifndef SIG_PROC
#define SIG_PROC

///////////////////////////////////////////////////////////////////////////////
// Enumeration.
///////////////////////////////////////////////////////////////////////////////

enum commands
{ 
	VGYRO_COM,
  VACCEL_COM,
	FUSION_COM,
	ORIENTATION_COM,
	SPEED_COM,
	POSITION_COM,
	ARW,
	DRIFT,
	HELP,
	IMPORT,
	INVALID,
  RAW
};

// Returns the int correspoding to the index of the command in enum
// COMMANDS. In the command is not found, returns the index of
// invalid.
int 		string_to_enum(char *str);

///////////////////////////////////////////////////////////////////////////////
// Wrapper functions.
///////////////////////////////////////////////////////////////////////////////

void 		sig_import(void);
void 		sig_arw(void);
void 		sig_vgyro(void);
void    sig_fusion(void);
void 		sig_vaccel(void);
void 		sig_orientation(void);
void		sig_position(void);
void 		sig_speed(void);
void 		sig_drift(void);
void    sig_raw(int, char**);
#endif
