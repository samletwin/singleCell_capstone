/**
 * @file lv_helper.h
 * @author Sam Letwin (letwins@uwindsor.ca)
 * @brief 
 * @version 0.1
 * @date 2024-06-09
 * 
 */

#ifndef LV_HELPER_H
#define LV_HELPER_H

#include <Adafruit_HX8357.h>   // GFX lib for SPI and parallel displays
#include "Adafruit_FT5336.h"
#include <lvgl.h>              // LittlevGL core lib
#if defined(ARDUINO_ARCH_SAMD)
#include <Adafruit_ZeroTimer.h> // SAMD-specific timer lib
#elif defined(ESP32)
#include <Ticker.h> // ESP32-specific timer lib
#endif

typedef enum {
  LVGL_OK,
  LVGL_ERR_ALLOC,
  LVGL_ERR_TIMER,
  LVGL_ERR_MUTEX,
  LVGL_ERR_TASK
} LvGLStatus;

/**
 * @brief Class to act as a "glue" layer between the LvGL graphics library and
 * Adafruit TFT Display - modified specifically to update to LvGL 9.0 and for use with 
 * HX8357 chip
 *
 */
class adafruit_lv_helper {
public:
    adafruit_lv_helper(void);
    ~adafruit_lv_helper(void);
    LvGLStatus lv_helper_init(Adafruit_HX8357 *tft, Adafruit_FT5336 *touch, bool debug);
    // These items need to be public for some internal callbacks,
    // but should be avoided by user code please!
    Adafruit_SPITFT *display; ///< Pointer to the SPITFT display instance
    Adafruit_FT5336 *touchscreen;        ///< Pointer to the touchscreen object to use
    bool first_frame;  ///< Tracks if a call to `lv_flush_callback` needs to wait
                        ///< for DMA transfer to complete

#ifdef ESP32
    void lvgl_acquire(); ///< Acquires the lock around the lvgl object
    void lvgl_release(); ///< Releases the lock around the lvgl object
#endif

private:
    Ticker tick;
};

#endif /* LV_HELPER_H */