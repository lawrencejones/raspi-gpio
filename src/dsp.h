///////////////////////////////////////////////////////////////////////////////
// Virtual Gyroscope
// ¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯
// File: dsp.h
// PA Consulting - Pierre Sutter
///////////////////////////////////////////////////////////////////////////////

#ifndef DSP_HEADER_INC
#define DSP_HEADER_INC

///////////////////////////////////////////////////////////////////////////////
// TYPEDEFS / STRUCTS
///////////////////////////////////////////////////////////////////////////////

// Struct to represent the result of the Allan Variance
// Will be an array of indexes that correspond directly
// to an array of values, to be plotted as X-Y
typedef struct {
  int *keys;
  float *values;
  int length;
} AllanVar;

typedef struct {
  const char** paths;
  float *arws;
  int no_of_signals;
} GyroSignals;

typedef struct {
  const char** paths;
  int no_of_signals;
} AccelSignals;

///////////////////////////////////////////////////////////////////////////////
// DSP INTERFACE
///////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////
// Allan Variance  //////////////////////////////////////////
// Computes the allan variance on the given array of values
// - The array of values is a **single axes** array
// - The second parameter will be length of array
AllanVar      *dsp_allan_compute  (  float           *axis,
                                     int             length  );

/////////////////////////////////////////////////////////////
// Sensor Processing  ///////////////////////////////////////
// Reduce white noise on signals and fuse them
//   - gyros   - String to fifo path for a gyro
//             - Sensor specific angular random walk value
//   - accels  - Path to accelerometer fifo
//   - freq    - Sample rate for all the signals
//   - ccm     - Pointer to a GSL matrix struct that contains
//               information on how the signals are related
//               To be configured on a per board basis
//   - path    - A path to the desired fifo output file for
//               the now processed signal
KalmanFilter  *dsp_apply_live     (  GyroSignals     *gyros,
                                     AccelSignals    *accels,
                                     int             freq,
                                     gsl_matrix      *ccm,
                                     const char      *path  );

#endif

