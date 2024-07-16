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

#define ADC_TASK_STACK_SIZE 4096

#define R1 10000
#define R2 20000
#define ADC_BATT_VOLTAGE_SCALE (R1+R2)/R2
#define ADC_CURRENT_SCALE (1000/400) /* 400mV/A scaling - divide by 400 to get A, multiply by 1000 to get mA*/

#define ADC_DEFAULT_SAMPLE_RATE_HZ 10

#define SPI_CS    	12 		   // SPI slave select
#define SPI_CLK     21
#define SPI_MOSI    22
#define SPI_MISO    19
#define ADC_VREF    5000     // 5V Vref
#define ADC_CLK     1000000  // SPI clock 1.6MHz
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
static TaskHandle_t adcTaskHandle = NULL;

/* ------------------------------------------------------------------------------------------------
  FUNCTION PROTOTYPES
------------------------------------------------------------------------------------------------ */
static void adc_task(void *pvParameters);


// Function to initialize a hardware timer
void setupTimer(uint8_t timerNumber, uint32_t intervalMicros, void (*callback)()) {
  hw_timer_t* timer = timerBegin(timerNumber, 80, true); // Use prescaler 80 for microsecond resolution
  timerAttachInterrupt(timer, callback, true); // Attach the callback function
  timerAlarmWrite(timer, intervalMicros, true); // Set the timer to fire every 'intervalMicros' microseconds
  timerAlarmEnable(timer); // Enable the timer
}

// ISR callback to notify the task
void IRAM_ATTR onTimer() {
  BaseType_t xHigherPriorityTaskWoken = pdFALSE;
  vTaskNotifyGiveFromISR(adcTaskHandle, &xHigherPriorityTaskWoken);
  portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
}

void adc_setup() {
  // Set the interval to 1 second (1000000 microseconds) for a 1Hz sampling rate
  setupTimer(1, 1000000/ADC_DEFAULT_SAMPLE_RATE_HZ, onTimer);

  // Create the ADC task
  xTaskCreate(adc_task, "ADC Task", ADC_TASK_STACK_SIZE, NULL, 1, &adcTaskHandle);

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
  float32 current_mA = static_cast<float32>((raw - 2500) * ADC_CURRENT_SCALE);
  /* invert current becasue I messed up the connections o_O */
  current_mA = -current_mA;
  return current_mA;
}

float32 adc_readBatteryVoltage() {
  uint16_t digital = adc.read(ADC_BATT_VOLTAGE_IN);
  uint16_t raw = adc.toAnalog(digital);
  float32 voltage_mV = static_cast<float32>(raw * ADC_BATT_VOLTAGE_SCALE);
  return voltage_mV;
}

void adc_task(void *pvParameters) {
  for (;;) {
    // Wait for the notification from the ISR
    ulTaskNotifyTake(pdTRUE, portMAX_DELAY);

    // Perform the necessary ADC readings
    globalWebpageData_s.voltageReading_mv_f32 = adc_readBatteryVoltage();
    globalWebpageData_s.currentReading_mA_f32 = adc_readCurrent();

    // Check if sample rate has changed and update timer if necessary
    // if (globalWebpageData_s.sampleRateChanged_b == true && globalWebpageData_s.sampleRate_Hz_ui16 > 0) {
    //     globalWebpageData_s.sampleRateChanged_b = false;
    //     setupTimer(1, (globalWebpageData_s.sampleRate_Hz_ui16 * 1000), onTimer); // Adjusted to correct timer interval
    // }

    #ifdef DEBUG_ADC_LOG
    Serial.print(globalWebpageData_s.voltageReading_mv_f32);
    Serial.print(",");
    Serial.println(globalWebpageData_s.currentReading_mA_f32);
    #endif
      
  }
}


