#include "Arduino.h"
#include "driver/gpio.h"
#include "driver/adc.h"
#include "esp32-hal-adc.h"
#include "adc.h"
#include "custom_types.h"
#include "bms/soh.h"
#include "bms/soc.h"
#include <vector>
#include "webpage/webpage.h"
#include "global_types.h"
#include "main_cfg.h"
/* ESP32 Arduino ADC Reference : https://github.com/espressif/arduino-esp32/blob/master/docs/en/api/adc.rst */
/* SPI Docs: https://espressif-docs.readthedocs-hosted.com/projects/arduino-esp32/en/latest/api/spi.html *
/
/* ------------------------------------------------------------------------------------------------
  DEFINES
------------------------------------------------------------------------------------------------ */
#define GPIO_ENABLE_DISCHARGE      15  
#define GPIO_ENABLE_CHARGE         13


/* ------------------------------------------------------------------------------------------------
  EXTERNALS
------------------------------------------------------------------------------------------------ */
extern std::vector<float32> soh_voltageMeasurements_mV_f32;
extern std::vector<float32> soh_currentMeasurements_mA_f32;

/* ------------------------------------------------------------------------------------------------
  GLOBALS
------------------------------------------------------------------------------------------------ */
webpageGlobalData_Type globalWebpageData_s;
adcGlobalData_Type adcGlobalData_s;

/* ------------------------------------------------------------------------------------------------
  LOCAL VARIABLES
------------------------------------------------------------------------------------------------ */
static bool measureSohFlag_b = false;

/* ------------------------------------------------------------------------------------------------
  FUNCTION PROTOTYPES
------------------------------------------------------------------------------------------------ */
void TestSOH();
void Main_MeasureSOH();
void Main_SOHTest(uint8 numIter, uint32 period_ms);

void setup() {
  /* Begin serial communication */
  Serial.begin(115200);
  delay(1000);
  
  /* Init structs */
  globalWebpageData_s = (webpageGlobalData_Type){0};
  adcGlobalData_s = (adcGlobalData_Type){0};

  /* Set pin modes */
  pinMode(GPIO_ENABLE_CHARGE, OUTPUT);
  pinMode(GPIO_ENABLE_DISCHARGE, OUTPUT);

  digitalWrite(GPIO_ENABLE_CHARGE, LOW);
  digitalWrite(GPIO_ENABLE_DISCHARGE, LOW);
  
  /* Start ESP32 Webserver */
  #ifdef USE_WEBSERVER
  webpage_Setup();
  #endif

  adc_setup();
//   Serial.println("Waiting 20sec..");
//   delay(20000);
//   Main_SOHTest(5, 10000);
}

void loop() {
  #ifdef USE_WEBSERVER
  webpage_MainFunc();
  #endif
  // PRINT_LN("Hey!");
  // delay(1000);
  // TestSOH();
  digitalWrite(GPIO_ENABLE_DISCHARGE, globalWebpageData_s.dischargeBatterySwitch_b);
  digitalWrite(GPIO_ENABLE_CHARGE, globalWebpageData_s.chargeBatterySwitch_b);
  if (globalWebpageData_s.measureSohSwitch_b == true && measureSohFlag_b == false) {
    measureSohFlag_b = true;
    Main_MeasureSOH();
  }
}

void TestSOH() {
  std::vector<float32> volt = {4002, 4302, 4103}; 
  std::vector<float32> current = {-1015, -34, -875}; 
  soh_result res = soh_LeastSquares(current, volt);
  PRINT("Res soh:");
  PRINT_LN(res.internalResistance_f32);
}

void Main_MeasureSOH() {
  PRINT_LN("Starting SOH Measurement");
  adcGlobalData_s.storeAdcReadingsFlag_b = true;
  delay(10000); 
  for (uint8 i = 0; i < globalWebpageData_s.numCycles_ui8; i++) {
    digitalWrite(GPIO_ENABLE_CHARGE, HIGH);
    delay(globalWebpageData_s.chargePeriod_ms_ui16);
    digitalWrite(GPIO_ENABLE_CHARGE, LOW);
    delay(100); /* Wait a bit b4 discharging */
    digitalWrite(GPIO_ENABLE_DISCHARGE, HIGH);
    delay(globalWebpageData_s.dischargePeriod_ms_ui16);
    digitalWrite(GPIO_ENABLE_DISCHARGE, LOW);
    delay(100);
  }
  delay(10000); 
  adcGlobalData_s.storeAdcReadingsFlag_b = false;
  PRINT_LN("Finished SOH Measurement");
  
  /* now that we have the data, compute the least squares */
  soh_result res = soh_LeastSquares(soh_currentMeasurements_mA_f32, soh_voltageMeasurements_mV_f32);
  PRINT("Calculated internal resistance from SOH Measurement: ");
  PRINT_LN(res.internalResistance_f32);

   PRINT("Calculated SOH from SOH Measurement: ");
  PRINT_LN(res.internalResistance_f32);

  globalWebpageData_s.internalResistanceResult_mOhms_f32 = res.internalResistance_f32 * 1000;

  /* clear the vectors */
  soh_currentMeasurements_mA_f32.clear();
  soh_voltageMeasurements_mV_f32.clear();

  globalWebpageData_s.measureSohSwitch_b = false;
  measureSohFlag_b = false;
}

void Main_SOHTest(uint8 numIter, uint32 period_ms) {
  for (uint8 i = 0; i < numIter; i++) {
    digitalWrite(GPIO_ENABLE_CHARGE, HIGH);
    delay(period_ms);
    digitalWrite(GPIO_ENABLE_CHARGE, LOW);
    delay(100); /* Wait a bit b4 discharging */
    digitalWrite(GPIO_ENABLE_DISCHARGE, HIGH);
    delay(period_ms);
    digitalWrite(GPIO_ENABLE_DISCHARGE, LOW);
    delay(100);
  }
}


