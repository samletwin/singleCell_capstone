

/**
 * @file lv_helper.cpp
 * @author Sam Letwin (letwins@uwindsor.ca)
 * @brief Code for interfacing with LVGL library
 * @version 0.1
 * @date 2024-06-09
 * 
 */
#include <lvgl.h>
#include "lv_helper.h"
#include "Adafruit_FT5336.h"

#define TFT_HOR_RES   480
#define TFT_VER_RES   320
#define LV_TICK_INTERVAL_MS 10

/*LVGL draw into this buffer, 1/10 screen size usually works well. The size is in bytes*/
#define DRAW_BUF_SIZE (TFT_HOR_RES * TFT_VER_RES / 10 * (LV_COLOR_DEPTH / 8))
uint16_t draw_buf[TFT_HOR_RES*10];
static SemaphoreHandle_t xGuiSemaphore = NULL;
static TaskHandle_t g_lvgl_task_handle;

typedef struct {
  Adafruit_FT5336* touchPtr_cp;
  uint8_t screenRotation_ui8;
} lv_touch_data_s;

typedef struct {
  Adafruit_SPITFT* displayPtr_cp;
  bool first_frame;
} lv_disp_data_s;


// The following preprocessor code segments are based around the LVGL example
// project for ESP32:
// https://github.com/lvgl/lv_port_esp32/blob/master/main/main.c

// Semaphore to handle concurrent calls to LVGL
// If you wish to call *any* lvgl function from other threads/tasks
// on ESP32, wrap the lvgl function calls inside of lvgl_acquire() and
// lvgl_release()

// Periodic timer handler
// NOTE: We use the IRAM_ATTR here to place this code into RAM rather than flash
static void IRAM_ATTR lv_tick_handler(void *arg) {
  (void)arg;
  lv_tick_inc(LV_TICK_INTERVAL_MS);
}

// Pinned task used to update the GUI, called by FreeRTOS
static void gui_task(void *args) {
  while (1) {
    // Delay 1 tick (follows lv_tick_interval_ms)
    vTaskDelay(pdMS_TO_TICKS(LV_TICK_INTERVAL_MS));

    // Try to take the semaphore, call lvgl task handler function on success
    if (pdTRUE == xSemaphoreTake(xGuiSemaphore, portMAX_DELAY)) {
      lv_task_handler();
      xSemaphoreGive(xGuiSemaphore);
    }
  }
}

/**
 * @brief Locks LVGL resource to prevent memory corrupton on ESP32.
 * NOTE: This function MUST be called PRIOR to a LVGL function (`lv_`) call.
 */
void adafruit_lv_helper::lvgl_acquire(void) {
  TaskHandle_t task = xTaskGetCurrentTaskHandle();
  if (g_lvgl_task_handle != task) {
    xSemaphoreTake(xGuiSemaphore, portMAX_DELAY);
  }
}

/**
 * @brief Unlocks LVGL resource to prevent memory corrupton on ESP32.
 * NOTE: This function MUST be called in application code AFTER lvgl_acquire()
 */
void adafruit_lv_helper::lvgl_release(void) {
  TaskHandle_t task = xTaskGetCurrentTaskHandle();
  if (g_lvgl_task_handle != task) {
    xSemaphoreGive(xGuiSemaphore);
  }
}

void lv_disp_flush(lv_display_t *disp, const lv_area_t *area, uint8_t * px_map) {
  // Get pointer to glue object from indev user data
  Serial.println("Started disp flush");
  lv_disp_data_s *lv_disp_data_flush = (lv_disp_data_s*)lv_display_get_user_data(disp);
  Serial.println("got disp flush");

  // if (!lv_disp_data_flush->first_frame) {
  //     lv_disp_data_flush->displayPtr_cp->dmaWait();  // Wait for prior DMA transfer to complete
  //     lv_disp_data_flush->displayPtr_cp->endWrite(); // End transaction from any prior call
  // } else {
  //     lv_disp_data_flush->first_frame = false;
  // }

  uint16_t width = (area->x2 - area->x1 + 1);
  uint16_t height = (area->y2 - area->y1 + 1);
  Serial.println("start writing");
  uint16_t * buf16 = (uint16_t *)px_map; /*Let's say it's a 16 bit (RGB565) display*/
  int32_t x, y;
  for(y = area->y1; y <= area->y2; y++) {
        for(x = area->x1; x <= area->x2; x++) {
            lv_disp_data_flush->displayPtr_cp->drawPixel(x, y, *buf16);
            Serial.println("drew p");  
            buf16++;
        }
    }
  // lv_disp_data_flush->displayPtr_cp->drawRGBBitmap(area->x1, area->y1, (uint16_t*)px_map, width, height);
  lv_disp_flush_ready(disp);
  Serial.println("Finished disp flush");
    
}

void lv_touch_read(lv_indev_t * indev, lv_indev_data_t * data) {
    static lv_coord_t last_x = 0, last_y = 0;
    Serial.println("Started touch read");

    // Get pointer to glue object from indev user data
    lv_touch_data_s *lv_touch_data_read = (lv_touch_data_s*)lv_indev_get_user_data(indev);
    TS_Point p = lv_touch_data_read->touchPtr_cp->getPoint();
    
    /* Not pressed */
    if (p.z == 0) { 
        data->state = LV_INDEV_STATE_RELEASED;
    } else {
        data->state = LV_INDEV_STATE_PRESSED; // Is PRESSED
        // TODO: Figure out if with current setup need to remap coordinates based on rotation
        // switch (lv_touch_data_read->screenRotation_ui8) {
        // case 0: // 0 degrees
        //     last_x = p.x;
        //     last_y = p.y;
        //     break;
        // case 1: // 90 degrees
        //     last_x = p.y;
        //     last_y = 480 - p.x;
        //     break;
        // case 2: // 180 degrees
        //     last_x = 480 - p.x;
        //     last_y = 320 - p.y;
        //     break;
        // case 3: // 270 degrees
        //     last_x = 320 - p.y;
        //     last_y = p.x;
        //     break;
        // default:
        //     last_x = p.x;
        //     last_y = p.y;
        //     break;
        // }
    }
    data->point.x = p.x; // Last-pressed coordinates
    data->point.y = p.y;
    data->continue_reading = false; // No buffering of ADC touch data
    Serial.println("ended touch read");
}

// Constructor
/**
 * @brief Construct a new adafruit_lv_helper::adafruit_lv_helper object,
 * initializing minimal variables
 *
 */
adafruit_lv_helper::adafruit_lv_helper(void) : first_frame(true) {

}

// Destructor
/**
 * @brief Destroy the adafruit_lv_helper::adafruit_lv_helper object, freeing any
 * memory previously allocated within this library.
 *
 */
adafruit_lv_helper::~adafruit_lv_helper(void) {
  // Probably other stuff that could be deallocated here
}

static void lv_debug(lv_log_level_t level, const char *buf) { 
    LV_UNUSED(level);
    Serial.println(buf);
    Serial.flush();
}

LvGLStatus adafruit_lv_helper::lv_helper_init(Adafruit_HX8357 *tft, Adafruit_FT5336 *touch, bool debug) {
    this->display= tft;
    this->touchscreen = touch;
    this->first_frame = true;
    tft->fillScreen(0);
    lv_init();
    if (debug) {
        lv_log_register_print_cb(lv_debug); // Register debug print function
    }

    /* Timer Setup - ESP Processor specific */
    // Create a periodic timer to call `lv_tick_handler`
    // const esp_timer_create_args_t periodic_timer_args = {
    //     .callback = &lv_tick_handler, 
    //     .name = "lv_tick_handler"
    // };
    // esp_timer_handle_t periodic_timer;
    // ESP_ERROR_CHECK(esp_timer_create(&periodic_timer_args, &periodic_timer));

    // // Create a new mutex
    // xGuiSemaphore = xSemaphoreCreateMutex();
    // if (xGuiSemaphore == NULL) {
    //     return LVGL_ERR_MUTEX; // failure
    // }

    // #ifdef CONFIG_IDF_TARGET_ESP32C3
    // // For unicore ESP32-x, pin GUI task to core 0
    // if (xTaskCreatePinnedToCore(gui_task, "lvgl_gui", 1024 * 8, NULL, 5,
    //                             &g_lvgl_task_handle, 0) != pdPASS)
    //     return LVGL_ERR_TASK; // failure
    // #else
    // // For multicore ESP32-x, pin GUI task to core 1 to allow WiFi on core 0
    // if (xTaskCreatePinnedToCore(gui_task, "lvgl_gui", 1024 * 8, NULL, 5,
    //                             &g_lvgl_task_handle, 1) != pdPASS)
    //     return LVGL_ERR_TASK; // failure
    // #endif

    // // Start timer
    // ESP_ERROR_CHECK(esp_timer_start_periodic(periodic_timer, LV_TICK_INTERVAL_MS * 1000));
    lv_tick_set_cb(millis);

    /* Setup Display */
    lv_display_t * lv_display_pt = lv_display_create(TFT_HOR_RES, TFT_VER_RES);
    lv_display_set_flush_cb(lv_display_pt, lv_disp_flush);
    lv_display_set_buffers(lv_display_pt, draw_buf, NULL, sizeof(draw_buf), LV_DISPLAY_RENDER_MODE_PARTIAL);
    
    lv_disp_data_s lv_disp_data_flush =  {
      .displayPtr_cp = tft,
      .first_frame = true
    };
    lv_display_set_user_data(lv_display_pt, (void*)&lv_disp_data_flush);

    /* Setup input device driver */
    lv_indev_t * index_pt = lv_indev_create();
    lv_indev_set_type(index_pt, LV_INDEV_TYPE_POINTER); /*Touchpad should have POINTER type*/
    lv_indev_set_read_cb(index_pt, lv_touch_read);

    lv_touch_data_s lv_touch_data_read = {
      .touchPtr_cp = touch,
      .screenRotation_ui8 = tft->getRotation()
    };
    lv_indev_set_user_data(index_pt, (void*)&lv_touch_data_read);

    /* Successfly init */
    return LVGL_OK;
}