#include "Arduino.h"
#include "driver/adc.h"
#include "esp32-hal-adc.h"
#include "custom_types.h"
#include "adc.h"
#include "mcp320x.h"
#include "webpage/webpage.h"

/* ------------------------------------------------------------------------------------------------
  DEFINES
------------------------------------------------------------------------------------------------ */
#define ADC_BATT_CURRENT_IN   MCP3202::Channel::SINGLE_0    
#define ADC_BATT_VOLTAGE_IN   MCP3202::Channel::SINGLE_1

#define R1 10000
#define R2 20000
#define ADC_BATT_VOLTAGE_SCALE (R1+R2)/R2
#define ADC_CURRENT_SCALE (1000/400) /* 400mV/A scaling - divide by 400 to get A, multiply by 1000 to get mA*/

#define ADC_DEFAULT_SAMPLE_RATE_HZ 10000

#define SPI_CS    	13 		   // SPI slave select
#define SPI_CLK     19
#define SPI_MOSI    22
#define SPI_MISO    21
#define ADC_VREF    5000     // 5V Vref
#define ADC_CLK     1600000  // SPI clock 1.6MHz
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
static MCP3202 adc(ADC_VREF, SPI_CS);

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

void IRAM_ATTR onTimer() {
  adc_task();
}

void adc_setup() {

  setupTimer(1, ADC_DEFAULT_SAMPLE_RATE_HZ * 1000, adc_task);

  /* Setup SPI for MCP3202 */
  pinMode(SPI_CS, OUTPUT);
  digitalWrite(SPI_CS, HIGH);

  SPISettings settings(ADC_CLK, MSBFIRST, SPI_MODE0);
  SPI.begin(SPI_CLK, SPI_MISO, SPI_MOSI, SPI_CS);
  SPI.beginTransaction(settings);
}

float32 adc_readCurrent() {
  uint16_t digital = adc.read(ADC_BATT_CURRENT_IN);
  uint16_t raw = adc.toAnalog(digital);
  float32 current_mA = static_cast<float32>((raw - 2500) *ADC_CURRENT_SCALE);
  return current_mA;
}

float32 adc_readBatteryVoltage() {
  uint16_t digital = adc.read(ADC_BATT_VOLTAGE_IN);
  uint16_t raw = adc.toAnalog(digital);
  float32 voltage_mV = static_cast<float32>(raw *ADC_BATT_VOLTAGE_SCALE);
  return voltage_mV;
}


void IRAM_ATTR adc_task() {
  globalWebpageData_s.voltageReading_mv_f32 = adc_readBatteryVoltage();
  globalWebpageData_s.currentReading_mA_f32 = adc_readCurrent();


  if (globalWebpageData_s.sampleRateChanged_b == true && globalWebpageData_s.sampleRate_Hz_ui16 > 0) {
    globalWebpageData_s.sampleRateChanged_b = false;
  
    setupTimer(1, (globalWebpageData_s.sampleRate_Hz_ui16*1000), adc_task);
  }
  
  #ifdef DEBUG_ADC_LOG
  Serial.print(globalWebpageData_s.voltageReading_mv_f32);
  Serial.print(",");
  Serial.println(globalWebpageData_s.currentReading_mA_f32);
  #endif
}


