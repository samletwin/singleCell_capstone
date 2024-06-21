#include "Arduino.h"
#include "driver/adc.h"
#include "esp32-hal-adc.h"
#include "custom_types.h"
#include "adc.h"

/* ------------------------------------------------------------------------------------------------
  DEFINES
------------------------------------------------------------------------------------------------ */
#define ADC_BATT_CURRENT_IN   2    // ADC1_CHANNEL_0
#define ADC_BATT_VOLTAGE_IN   3    // ADC1_CHANNEL_1

#define R1 9970
#define R2 19890
#define ADC_BATT_VOLTAGE_SCALE (R1+R2)/R2
#define ADC_CURRENT_SCALE (1000/400) /* 400mV/A scaling - divide by 400 to get A, multiply by 1000 to get mA*/

#define ADC_LOOP_PERIOD_DEFAULT_MS 5
/* ------------------------------------------------------------------------------------------------
  GLOBALS
------------------------------------------------------------------------------------------------ */

/* ------------------------------------------------------------------------------------------------
  EXTERNAL VARIABLES
------------------------------------------------------------------------------------------------ */
extern signalData_s globalSignalData_s;

/* ------------------------------------------------------------------------------------------------
  LOCAL VARIABLES
------------------------------------------------------------------------------------------------ */
uint64_t runningTimer_ms_ui64 = 0;
uint64_t currentTimer_ms_ui64 = 0;

/* ------------------------------------------------------------------------------------------------
  FUNCTION PROTOTYPES
------------------------------------------------------------------------------------------------ */
void setupTimer(uint8_t timerNumber, uint32_t intervalMicros, void (*callback)());

// Function to initialize a hardware timer
void setupTimer(uint8_t timerNumber, uint32_t intervalMicros, void (*callback)()) {
    hw_timer_t* timer = timerBegin(timerNumber, 80, true); // Use prescaler 80 for microsecond resolution
    timerAttachInterrupt(timer, callback, true); // Attach the callback function
    timerAlarmWrite(timer, intervalMicros, true); // Set the timer to fire every 'intervalMicros' microseconds
    timerAlarmEnable(timer); // Enable the timer
}

// Callback function that gets called by the hardware timer interrupt
void IRAM_ATTR onTimer() {
    // Your code here, e.g., read ADC, toggle a pin, etc.
    Serial.println("Timer Interrupt Triggered");
}

void adc_setup() {
  /* Set pin modes */
  pinMode(ADC_BATT_CURRENT_IN, INPUT);
  pinMode(ADC_BATT_VOLTAGE_IN, INPUT);

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


void adc_task() {
  runningTimer_ms_ui64 = millis();
  globalSignalData_s.batteryCurrent_rawAdc_ui32 = analogReadMilliVolts(ADC_BATT_CURRENT_IN);
  globalSignalData_s.batteryVoltage_rawAdc_ui32 = analogReadMilliVolts(ADC_BATT_VOLTAGE_IN);

  globalSignalData_s.batteryCurrent_mA_f32 = globalSignalData_s.batteryCurrent_rawAdc_ui32 * ADC_CURRENT_SCALE; /* TI Sensor has 400mV/A scaling */
  globalSignalData_s.batteryVoltage_mV_f32 = globalSignalData_s.batteryVoltage_rawAdc_ui32 * ADC_BATT_VOLTAGE_SCALE;

  #ifdef DEBUG_ADC
  if (runningTimer_ms_ui64 >= currentTimer_ms_ui64 + 2000)
  {
    currentTimer_ms_ui64 = runningTimer_ms_ui64;
    Serial.print("Current voltage scaled reading: ");
    Serial.println(globalSignalData_s.batteryVoltage_mV_f32);
    Serial.print("Raw voltage divider reading: ");
    Serial.println(globalSignalData_s.batteryVoltage_rawAdc_ui32);

    Serial.print("Current current scaled reading: ");
    Serial.println(globalSignalData_s.batteryCurrent_mA_f32);
    Serial.print("Raw current reading: ");
    Serial.println(globalSignalData_s.batteryCurrent_rawAdc_ui32);
    Serial.print("\n");
  }
  #endif
}


