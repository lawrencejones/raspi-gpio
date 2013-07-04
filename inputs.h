///////////////////////////////////////////////////////////////////////////////
// Air Quality Sensor
// ¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯
// File: inputs.h
// PA Consulting - Joao Fermoselle
///////////////////////////////////////////////////////////////////////////////

#ifndef INPUTS
#define INPUTS

// Define unsigned int for reading results
// 16 bits presumed to be sufficient
typedef uint16_t u16;

// Standard sensor retrieval functions
u16 getTemperature();
u16 getLight();
u16 getHumidity();
u16 getPressure();
u16 getUV();
u16 getNO2();
u16 getCO();

#endif