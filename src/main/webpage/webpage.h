#ifndef WEBPAGE_H
#define WEBPAGE_H

#include "custom_types.h"

#define USE_INTRANET

typedef struct {
    double voltageReading_mv_f32;
    double currentReading_mA_f32;
    float32 ocvResult_V_f32;
    float32 internalResistanceResult_Ohms_f32;
    float32 socResult_perc_f32;
    uint64 ttsResult_S_ui32;;
    uint16 dischargePeriod_ms_ui16;
    uint16 chargePeriod_ms_ui16;
    uint16 sampleRate_Hz_ui16;
    uint8 numDischarges_ui8;
    uint8 numCharges_ui8;
    bool sampleRateChanged_b;
    bool dischargeBatterySwitch_b;
    bool chargeBatterySwitch_b;
    bool measureSohSwitch_b;
} webpageGlobalData_s; 

/* API Functions */
void webpage_Setup();
void webpage_MainFunc();

#endif 