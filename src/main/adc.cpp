#include "Arduino.h"
#include "driver/adc.h"
#include "esp32-hal-adc.h"
#include "custom_types.h"
#include "adc.h"
#include "webpage/webpage.h"

/* ------------------------------------------------------------------------------------------------
  DEFINES
------------------------------------------------------------------------------------------------ */
#define ADC_BATT_CURRENT_IN   ADC1_CHANNEL_2    // ADC1_CHANNEL_0
#define ADC_BATT_VOLTAGE_IN   ADC1_CHANNEL_3    // ADC1_CHANNEL_1

#define R1 9970
#define R2 19890
#define ADC_BATT_VOLTAGE_SCALE (R1+R2)/R2
#define ADC_CURRENT_SCALE (1000/400) /* 400mV/A scaling - divide by 400 to get A, multiply by 1000 to get mA*/

#define ADC_DEFAULT_SAMPLE_RATE_HZ 100
/* ------------------------------------------------------------------------------------------------
  GLOBALS
------------------------------------------------------------------------------------------------ */

/* ------------------------------------------------------------------------------------------------
  EXTERNAL VARIABLES
------------------------------------------------------------------------------------------------ */
extern webpageGlobalData_s globalWebpageData_s;

/* ------------------------------------------------------------------------------------------------
  LOCAL VARIABLES
------------------------------------------------------------------------------------------------ */
uint64_t runningTimer_ms_ui64 = 0;
uint64_t currentTimer_ms_ui64 = 0;

/* ------------------------------------------------------------------------------------------------
  FUNCTION PROTOTYPES
------------------------------------------------------------------------------------------------ */

// Function to initialize a hardware timer
void setupTimer(uint8_t timerNumber, uint32_t intervalMicros, void (*callback)()) {
    hw_timer_t* timerAdc = timerBegin(timerNumber, 80, true); // Use prescaler 80 for microsecond resolution
    timerAttachInterrupt(timerAdc, callback, true); // Attach the callback function
    timerAlarmWrite(timerAdc, intervalMicros, true); // Set the timer to fire every 'intervalMicros' microseconds
    timerAlarmEnable(timerAdc); // Enable the timer
}

// Callback function that gets called by the hardware timer interrupt
void IRAM_ATTR onTimer() {
    // Your code here, e.g., read ADC, toggle a pin, etc.
    adc_task();
    Serial.println("Timer Interrupt Triggered");
}

void adc_setup() {
  /* Set pin modes */
  pinMode(ADC_BATT_CURRENT_IN, INPUT);
  pinMode(ADC_BATT_VOLTAGE_IN, INPUT);

  setupTimer(1, ADC_DEFAULT_SAMPLE_RATE_HZ * 1000, adc_task);

  /* Setup ADC */
  analogSetAttenuation(ADC_11db); 
  adc1_config_width(ADC_WIDTH_BIT_12);
}

float32 adc_readCurrent() {
  return analogReadMilliVolts(ADC_BATT_CURRENT_IN) * ADC_CURRENT_SCALE;
}

float32 adc_readBatteryVoltage() {
  return analogReadMilliVolts(ADC_BATT_VOLTAGE_IN) * ADC_BATT_VOLTAGE_SCALE;
}


void IRAM_ATTR adc_task() {
  runningTimer_ms_ui64 = millis();
  uint32 volt_rawAdc_ui32;
  sint32 current_rawAdc_ui32;
  volt_rawAdc_ui32 = analogReadMilliVolts(ADC_BATT_VOLTAGE_IN);
  current_rawAdc_ui32 = analogReadMilliVolts(ADC_BATT_CURRENT_IN) - 2500;


  globalWebpageData_s.voltageReading_mv_f32 = ((double)volt_rawAdc_ui32) * ADC_BATT_VOLTAGE_SCALE; 
  globalWebpageData_s.currentReading_mA_f32 = ((double)current_rawAdc_ui32) * 2.5; /* TI Sensor has 400mV/A scaling */


  if (globalWebpageData_s.sampleRateChanged_b == true && globalWebpageData_s.sampleRate_Hz_ui16 > 0) {
    globalWebpageData_s.sampleRateChanged_b = false;
  
    setupTimer(1, (globalWebpageData_s.sampleRate_Hz_ui16*1000), adc_task);
  }
  
  #ifdef DEBUG_ADC_LOG
  Serial.print("Battery_Voltage:");
  Serial.print(globalWebpageData_s.voltageReading_mv_f32);
  Serial.print(",Battery_Current:");
  Serial.println(globalWebpageData_s.currentReading_mA_f32);
  #ifdef DEBUG_ADC
  // if (runningTimer_ms_ui64 >= currentTimer_ms_ui64 + 2000)
  // {
    currentTimer_ms_ui64 = runningTimer_ms_ui64;
    Serial.print("Current voltage scaled reading: ");
    Serial.println(globalWebpageData_s.voltageReading_mv_f32);
    Serial.print("Raw voltage divider reading: ");
    Serial.println(volt_rawAdc_ui32);

    Serial.print("Current current scaled reading: ");
    Serial.println(globalWebpageData_s.currentReading_mA_f32);
    Serial.print("Raw current reading: ");
    Serial.println(current_rawAdc_ui32);
    Serial.print("\n");
  // }
  #endif
  #endif
}


