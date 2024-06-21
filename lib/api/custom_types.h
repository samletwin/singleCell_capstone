#ifndef CUSTOM_TYPES_H
#define CUSTOM_TYPES_H

/* ------------------------------------------------------------------------------------------------
  TYPES
------------------------------------------------------------------------------------------------ */
typedef unsigned char         uint8;
typedef unsigned int          uint16;
typedef unsigned long         uint32;
typedef unsigned long long    uint64;

typedef signed char           sint8;
typedef signed int            sint16;
typedef signed long           sint32;
typedef signed long long      sint64;

typedef float                 float32;
typedef double                float64;


typedef struct {
  float32 batteryVoltage_mV_f32; 
  uint32 batteryVoltage_rawAdc_ui32;
  float32 batteryCurrent_mA_f32; 
  uint32 batteryCurrent_rawAdc_ui32;
  uint16 dischargePeriod_ms_ui16;
  uint8 numDischarges_ui8;
  bool measureCurrent_b;
} signalData_s;

#endif