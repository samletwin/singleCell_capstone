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
#define GPIO_SWITCH_OUT       14    // GPIO_NUM_14


/* ------------------------------------------------------------------------------------------------
  CONSTANTS
------------------------------------------------------------------------------------------------ */

/* ------------------------------------------------------------------------------------------------
  EXTERNALS
------------------------------------------------------------------------------------------------ */
signalData_s globalSignalData_s;

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

  /* Set pin modes */
  // pinMode(GPIO_SWITCH_OUT, OUTPUT);
  
  adc_setup();

  /* Start ESP32 Webserver */
  #ifdef USE_WEBSERVER
  webpage_Setup();
  #endif

  /* Start Display */
  // TFT_Init();
}

void loop() {
  #ifdef USE_WEBSERVER
  webpage_MainFunc();
  #endif
  // PRINT_LN("Hey!");
  // adc_loop();
  adc_task();
  // if (globalWebpageData_s.measureSohSwitch_b == true && measureSohFlag_b == false) {
  //   measureSohFlag_b = true;
  //   Main_MeasureSOH(globalWebpageData_s.numDischarges_ui8, globalWebpageData_s.dischargePeriod_ms_ui16, globalWebpageData_s.sampleRate_Hz_ui16);
  // }
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
  webpage_SetSohButtonStatus(true);
}


