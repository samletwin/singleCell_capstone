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
  EXTERNALS
------------------------------------------------------------------------------------------------ */
webpageGlobalData_s globalWebpageData_s;

/* ------------------------------------------------------------------------------------------------
  LOCAL VARIABLES
------------------------------------------------------------------------------------------------ */
bool measureSohFlag_b = false;


/* ------------------------------------------------------------------------------------------------
  FUNCTION PROTOTYPES
------------------------------------------------------------------------------------------------ */
void TestSOH();
void Main_MeasureSOH(uint8 numDischarges_ui8, uint16 dischargePeriod_ms_ui16, uint16 sampleRate_Hz_ui16);

void setup() {
  /* Begin serial communication */
  Serial.begin(115200);
  delay(5000);


  /* Init struct */
  globalWebpageData_s = (webpageGlobalData_s){0};
  globalWebpageData_s.currentReading_mA_f32 = 0.0;
  globalWebpageData_s.chargeBatterySwitch_b = false;

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
}

void loop() {
  #ifdef USE_WEBSERVER
  webpage_MainFunc();
  #endif
  // PRINT_LN("Hey!");
  // delay(1000);

  digitalWrite(GPIO_ENABLE_DISCHARGE, globalWebpageData_s.dischargeBatterySwitch_b);
  digitalWrite(GPIO_ENABLE_CHARGE, globalWebpageData_s.chargeBatterySwitch_b);
  if (globalWebpageData_s.measureSohSwitch_b == true && measureSohFlag_b == false) {
    measureSohFlag_b = true;
    Main_MeasureSOH(globalWebpageData_s.numDischarges_ui8, globalWebpageData_s.dischargePeriod_ms_ui16, globalWebpageData_s.sampleRate_Hz_ui16);
  }
}

void TestSOH() {
  std::vector<float32> volt = {4002, 4302, 4103}; 
  std::vector<float32> current = {1015, 934, 875}; 
  soh_result res = soh_LeastSquares(current, volt);
  PRINT("Res soh:");
  PRINT_LN(res.internalResistance_f32);
}

void Main_MeasureSOH(uint8 numDischarges_ui8, uint16 dischargePeriod_ms_ui16, uint16 sampleRate_Hz_ui16) {
  PRINT_LN("Starting SOH Measurement");
  uint32 time = dischargePeriod_ms_ui16 * 2 * numDischarges_ui8;
  delay(time);
  PRINT_LN("Finished SOH Measurement");
  measureSohFlag_b = false;
  globalWebpageData_s.measureSohSwitch_b = false;
}


