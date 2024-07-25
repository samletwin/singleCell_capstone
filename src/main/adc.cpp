#include "driver/adc.h"
#include "esp32-hal-adc.h"
#include "custom_types.h"
#include "adc.h"
#include "mcp320x.h"
#include "global_types.h"
#include "timer/timer.h"
#include <vector>
#include "bms/soc.h"
#include <cmath>

/* ------------------------------------------------------------------------------------------------
  DEFINES
------------------------------------------------------------------------------------------------ */
#define ADC_BATT_CURRENT_IN   MCP3202::Channel::SINGLE_0    
#define ADC_BATT_VOLTAGE_IN   MCP3202::Channel::SINGLE_1

#define ADC_TASK_STACK_SIZE 4096

#define R1 9998
#define R2 20130
#define ADC_BATT_VOLTAGE_SCALE (R1+R2)/R2
#define ADC_CURRENT_SCALE (1000/400) /* 400mV/A scaling - divide by 400 to get A, multiply by 1000 to get mA*/

#define ADC_DEFAULT_SAMPLE_RATE_HZ 1

#define SPI_CS    	12 		   // SPI slave select
#define SPI_CLK     21
#define SPI_MOSI    22
#define SPI_MISO    19
#define ADC_VREF    5073     // 5V Vref
#define ADC_CLK     1000000  // SPI clock 1.6MHz
/* ------------------------------------------------------------------------------------------------
  GLOBALS
------------------------------------------------------------------------------------------------ */
std::vector<float32> soh_voltageMeasurements_mV_f32 = std::vector<float32>();
std::vector<float32> soh_currentMeasurements_mA_f32 = std::vector<float32>();

/* ------------------------------------------------------------------------------------------------
  EXTERNAL VARIABLES
------------------------------------------------------------------------------------------------ */
extern webpageGlobalData_Type globalWebpageData_s;
extern adcGlobalData_Type adcGlobalData_s;

/* ------------------------------------------------------------------------------------------------
  LOCAL VARIABLES
------------------------------------------------------------------------------------------------ */
static MCP3202 adc(ADC_VREF, SPI_CS);
static TaskHandle_t adcTaskHandle = NULL;
static esp_timer_handle_t adcTimerHandle = NULL;
static bool batteryDetected_b = false;

/* ------------------------------------------------------------------------------------------------
  FUNCTION PROTOTYPES
------------------------------------------------------------------------------------------------ */
static void adc_task(void *pvParameters);


// ISR callback to notify the task
void IRAM_ATTR onTimer(void* arg) {
  BaseType_t xHigherPriorityTaskWoken = pdFALSE;
  vTaskNotifyGiveFromISR(adcTaskHandle, &xHigherPriorityTaskWoken);
  portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
}

void adc_setup() {
  // Set the interval to 1 second (1000000 microseconds) for a 1Hz sampling rate
  adcTimerHandle = create_timer(0, 1000000/ADC_DEFAULT_SAMPLE_RATE_HZ, onTimer, true);

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
    float32 voltage_mV = adc_readBatteryVoltage();
    float32 current_mA = adc_readCurrent();

    // Error handling
    if ((voltage_mV < 500.0f || voltage_mV > 4500.0f) && fabs(current_mA) > 150.0f) {
      voltage_mV = 0.0f;
      current_mA = 0.0f;
    }
    else if (voltage_mV < 50.0f) {
      current_mA = 0.0f;
    }

    globalWebpageData_s.voltageReading_mv_f32 = voltage_mV;
    globalWebpageData_s.currentReading_mA_f32 = current_mA;

    if (voltage_mV >= 2.0f && !batteryDetected_b) {
      batteryDetected_b = true;
      float soc_init = soc_initialize(current_mA / 1000.0f, voltage_mV / 1000.0f);
      globalWebpageData_s.socResult_perc_f32 = soc_init * 100.0f;
    }
    else if (voltage_mV != 0.0f) {
      float soc_updated = soc_update(current_mA / 1000.0f);
      globalWebpageData_s.socResult_perc_f32 = soc_updated * 100.0f;
    }
    else {
      batteryDetected_b = false;
      soc_init(); // Reinitialize SOC when battery is disconnected - also clear any existing results
      globalWebpageData_s.socResult_perc_f32 = 0.0f;
      globalWebpageData_s.internalResistanceResult_mOhms_f32 = 0.0f;
    }
    globalWebpageData_s.batteryDetected_b = batteryDetected_b;

    if (adcGlobalData_s.storeAdcReadingsFlag_b) {
      /* Append most recent measurement to end of vector */
      soh_voltageMeasurements_mV_f32.push_back(voltage_mV); 
      soh_currentMeasurements_mA_f32.push_back(current_mA); 
    }

    // Check if sample rate has changed and update timer if necessary
    if (globalWebpageData_s.sampleRateChanged_b && globalWebpageData_s.sampleRate_Hz_ui16 > 0) {
      globalWebpageData_s.sampleRateChanged_b = false;
      delete_timer(adcTimerHandle);
      adcTimerHandle = create_timer(1, (1000000/globalWebpageData_s.sampleRate_Hz_ui16), onTimer, true); 
    }

    #ifdef DEBUG_ADC_LOG
    Serial.print(voltage_mV);
    Serial.print(",");
    Serial.println(current_mA);
    #endif
  }
}


