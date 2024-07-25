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
  CONSTANTS
------------------------------------------------------------------------------------------------ */

/* ------------------------------------------------------------------------------------------------
  GLOBALS
------------------------------------------------------------------------------------------------ */
webpageGlobalData_Type globalWebpageData_s;
adcGlobalData_Type adcGlobalData_s;
FuelGauge batteryClass(4.2f, 0.001f, 3.0f, 0, 1); 

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
    // Main_MeasureSOH();
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

  uint32 time = globalWebpageData_s.dischargePeriod_ms_ui16 * 2 * globalWebpageData_s.numDischarges_ui8;
  delay(time);
  PRINT_LN("Finished SOH Measurement");
  measureSohFlag_b = false;
  globalWebpageData_s.measureSohSwitch_b = false;
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


