#define USE_DISPLAY
#ifdef USE_DISPLAY

// #include <Adafruit_LvGL_Glue.h>
#include "UI/lv_helper.h"
#include <lvgl.h>
#include "Adafruit_HX8357.h"
#include "Adafruit_FT5336.h"
#include "Adafruit_GFX.h"
#include "custom_types.h"
#include "display.h"

/* ------------------------------------------------------------------------------------------------
  DEFINES
------------------------------------------------------------------------------------------------ */

#define SPI_MOSI_PIN          10
#define SPI_MISO_PIN          -1    // Not used  
#define SPI_CLK_PIN           8
#define SPI_CS_PIN            -1    // Test using - dont want to use but see if necessary
#define SPI_DC_PIN            9


#define PIN_I2C_SCL           7
#define PIN_I2C_SDA           6
#define PIN_TOUCH_INTERRUPT   0

#define FT5336_MAXTOUCHES  5

/* Diplay misc. */
// #define CONTROL_BACKLITE
#ifdef CONTROL_BACKLITE
#define PIN_DISP_BACKLITE     -1
#endif


/* ------------------------------------------------------------------------------------------------
  LOCAL VARIABLES
------------------------------------------------------------------------------------------------ */
/* Setup HX8357 - Display control */
Adafruit_HX8357 display_c = Adafruit_HX8357(SPI_CS_PIN, SPI_DC_PIN, SPI_MOSI_PIN, SPI_CLK_PIN,-1 ,-1 );
// Adafruit_LvGL_Glue lvgl_adafruit_c;
adafruit_lv_helper lvgl_adafruit_c;
Adafruit_FT5336 touchscreen_c;

/* ------------------------------------------------------------------------------------------------
  FUNCTION PROTOTYPES
------------------------------------------------------------------------------------------------ */
static void lvgl_setupDefaultUI(void);
static void lvgl_setupSettingsUI(void);
static void lvgl_settingsBtn_event_callback(lv_event_t *e);  
static void event_handler(lv_event_t * e);

void displayDiagnostics(Adafruit_HX8357* tft);
uint8_t cnt;


static void event_handler(lv_event_t * e) {
    lv_event_code_t code = lv_event_get_code(e);
    lv_obj_t * btn = (lv_obj_t*)lv_event_get_target(e);
    if(code == LV_EVENT_CLICKED) {
        cnt++;
        /*Get the first child of the button which is the label and change its text*/
        lv_obj_t * label = lv_obj_get_child(btn, 0);
        lv_label_set_text_fmt(label, "Button: %d", cnt);
    }
    if(code == LV_EVENT_CLICKED) {
        LV_LOG_USER("Clicked");
    }
    else if(code == LV_EVENT_VALUE_CHANGED) {
        LV_LOG_USER("Toggled");
    }
}

static void lvgl_setupDefaultUI(void) {
    Serial.println("Starting to setup default UI..");
    /* Setup Default UI */
    lv_obj_t * label;

    lv_obj_t * btn1 = lv_btn_create(lv_scr_act());
    lv_obj_add_event_cb(btn1, event_handler, LV_EVENT_ALL, NULL);
    lv_obj_align(btn1, LV_ALIGN_CENTER, 0, -40);
    lv_obj_clear_flag(btn1, LV_OBJ_FLAG_PRESS_LOCK);

    label = lv_label_create(btn1);
    lv_label_set_text(label, "Button");
    lv_obj_center(label);
}

static void lvgl_setupSettingsUI(void) {
    /* Locks LVGL resource to prevent memory corruption on ESP32 */
    /* When using WiFi on esp32, this MUST be called prior to LVGL function calls*/
    lvgl_adafruit_c.lvgl_acquire();

    /* Setup UI */

    /* Unlocks LVGL resource to prevent memory corrupton on ESP32 */
    /* NOTE: This function MUST be called AFTER lvgl_acquire */
    lvgl_adafruit_c.lvgl_release();
}


void TFT_Init() {
    /* Setup Display - SPI */
    display_c = Adafruit_HX8357(SPI_CS_PIN, SPI_DC_PIN, SPI_MOSI_PIN, SPI_CLK_PIN,-1 ,-1 );
    display_c.begin(HX8357D);
    delay(200);
    displayDiagnostics(&display_c);

    /* Setup touchscreen - I2C */
    bool res;
    res = Wire.setPins(PIN_I2C_SDA, PIN_I2C_SCL);
    if (res == true)
        Serial.println("Succesfully set I2C pins");
    else
        Serial.println("Failed to set I2C pins");
    touchscreen_c = Adafruit_FT5336();
    res = touchscreen_c.begin();
    if (res == true)
        Serial.println("Successfully set TS");
    else
        Serial.println("Failed to set TS");

    /* Init lvgl after TFT */
    LvGLStatus status = lvgl_adafruit_c.lv_helper_init(&display_c, &touchscreen_c, true);
    if(status == LVGL_OK) 
        Serial.println("Init LVGL Glue Successfully");
    else
        Serial.printf("\nGlue error %d\r\n", (int)status);

    // lvgl_adafruit_c.lvgl_acquire();
    lvgl_setupDefaultUI();
    // lvgl_adafruit_c.lvgl_release();
}

void TFT_Loop() {
    lv_task_handler(); /* let the GUI do its work */
    delay(5); /* let this time pass */
}

void displayDiagnostics(Adafruit_HX8357* tft) {
    // read diagnostics (optional but can help debug problems)
    uint8_t x = 0xF;
    x = tft->readcommand8(HX8357_RDPOWMODE);
    Serial.print("Display Power Mode: 0x"); Serial.println(x, HEX);
    x = tft->readcommand8(HX8357_RDMADCTL);
    Serial.print("MADCTL Mode: 0x"); Serial.println(x, HEX);
    x = tft->readcommand8(HX8357_RDCOLMOD);
    Serial.print("Pixel Format: 0x"); Serial.println(x, HEX);
    x = tft->readcommand8(HX8357_RDDIM);
    Serial.print("Image Format: 0x"); Serial.println(x, HEX);
    x = tft->readcommand8(HX8357_RDDSDR);
    Serial.print("Self Diagnostic: 0x"); Serial.println(x, HEX); 
}

#endif