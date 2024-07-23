#ifndef ADC_H
#define ADC_H

/* Config */
// #define DEBUG_ADC
// #define DEBUG_ADC_LOG

/* API Functions */
void adc_setup();
void adc_loop();
void IRAM_ATTR adc_task();

#endif 