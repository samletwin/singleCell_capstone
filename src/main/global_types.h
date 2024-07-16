#ifndef GLOBAL_TYPES_H
#define GLOBAL_TYPES_H

/* Used for communication between webpage.cpp, main.cpp, and adc.cpp */
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
} webpageGlobalData_Type; 

/* Used for communication between adc.cpp and main.cpp */
typedef struct {
    bool storeAdcReadingsFlag_b;
} adcGlobalData_Type;

#endif /* GLOBAL_TYPES_H */