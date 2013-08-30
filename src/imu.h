///////////////////////////////////////////////////////////////////////////////
// Raspberry Pi GPIO Interface
// ¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯
// File: imu.h
// PA Consulting - Lawrence Jones
///////////////////////////////////////////////////////////////////////////////

#ifndef IMU_HEADER_INC
#define IMU_HEADER_INC

///////////////////////////////////////////////////////////////////////////////
// IMU MODULE METHOD STUBS
///////////////////////////////////////////////////////////////////////////////

// Run tests for the i2c multiplexer (pca)
int imu_pca_test(int bus, short addr);
// Run tests for the mpu
int imu_mpu_test(int bus, short addr, char* config_str);
// Routing for cli
int imu_route(char **tokens, int argc);

#endif
