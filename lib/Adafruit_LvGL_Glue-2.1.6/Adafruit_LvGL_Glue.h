// #ifndef _ADAFRUIT_LVGL_GLUE_H_
// #define _ADAFRUIT_LVGL_GLUE_H_

// #include <Adafruit_FT5336.h>
// #include <Adafruit_SPITFT.h>   // GFX lib for SPI and parallel displays
// #include <lvgl.h>              // LittlevGL core lib
// #if defined(ARDUINO_ARCH_SAMD)
// #include <Adafruit_ZeroTimer.h> // SAMD-specific timer lib
// #elif defined(ESP32)
// #include <Ticker.h> // ESP32-specific timer lib
// #endif

// #define TOUCH_PRESSURE_THRESHOLD 10u

// typedef enum {
//   LVGL_OK,
//   LVGL_ERR_ALLOC,
//   LVGL_ERR_TIMER,
//   LVGL_ERR_MUTEX,
//   LVGL_ERR_TASK
// } LvGLStatus;

// /**
//  * @brief Class to act as a "glue" layer between the LvGL graphics library and
//  * most of Adafruit's TFT displays
//  *
//  */
// class Adafruit_LvGL_Glue {
// public:
//   Adafruit_LvGL_Glue(void);
//   ~Adafruit_LvGL_Glue(void);
//   // Different begin() funcs for STMPE610, ADC or no touch
//   LvGLStatus begin(Adafruit_SPITFT *tft, Adafruit_FT5336 *touch,
//                    bool debug = false);
//   // These items need to be public for some internal callbacks,
//   // but should be avoided by user code please!
//   Adafruit_SPITFT *display; ///< Pointer to the SPITFT display instance
//   Adafruit_FT5336 *touchscreen;        ///< Pointer to the touchscreen object to use
//   bool is_adc_touch; ///< determines if the touchscreen controlelr is ADC based
//   bool first_frame;  ///< Tracks if a call to `lv_flush_callback` needs to wait
//                      ///< for DMA transfer to complete

// #ifdef ESP32
//   void lvgl_acquire(); ///< Acquires the lock around the lvgl object
//   void lvgl_release(); ///< Releases the lock around the lvgl object
// #endif

// private:
//   static lv_disp_drv_t lv_disp_drv;
//   static lv_disp_draw_buf_t lv_disp_draw_buf;
//   static lv_color_t *lv_pixel_buf;
//   static lv_indev_drv_t lv_indev_drv;
//   lv_indev_t *lv_input_dev_ptr;
// #if defined(ARDUINO_ARCH_SAMD)
//   Adafruit_ZeroTimer *zerotimer;
// #elif defined(ESP32)
//   Ticker tick;
// #elif defined(NRF52_SERIES)
// #endif
// };

// #endif // _ADAFRUIT_LVGL_GLUE_H_
