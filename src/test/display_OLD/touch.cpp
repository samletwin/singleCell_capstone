6#define USE_TOUCH
#ifdef USE_TOUCH
#include "Adafruit_FT5336.h"
#include "display.h"

/* ------------------------------------------------------------------------------------------------
  DEFINES
------------------------------------------------------------------------------------------------ */
// #define PIN_I2C_SCL           21
// #define PIN_I2C_SDA           22
// #define PIN_TOUCH_INTERRUPT   0

#define PIN_I2C_SCL           7
#define PIN_I2C_SDA           6
#define PIN_TOUCH_INTERRUPT   0

#define FT5336_MAXTOUCHES  5

// Size of the color selection boxes and the paintbrush size
#define BOXSIZE 40
#define PENRADIUS 3P

#define TOUCH_ADDR 0x38

/* ------------------------------------------------------------------------------------------------
  LOCAL VARIABLES
------------------------------------------------------------------------------------------------ */
Adafruit_FT5336 ctp;

/* ------------------------------------------------------------------------------------------------
  FUNCTION PROTOTYPES
------------------------------------------------------------------------------------------------ */
void touch_setup();
void touch_loop();

void touch_setup() {
    // /* Setup I2C */
    bool res;
    res = Wire.setPins(PIN_I2C_SDA, PIN_I2C_SCL);
    if (res == true)
        Serial.println("Succesfully set I2C pins");
    else
        Serial.println("Failed to set I2C pins");
    // res = Wire.begin(TOUCH_ADDR);
    // if (res == true)
    //     Serial.println("Succesfully set I2C pins");
    // else
    //     Serial.println("Failed to set I2C pins");
    ctp = Adafruit_FT5336();
    res = ctp.begin();
    if (res == true)
        Serial.println("Successfully set TS");
    else
        Serial.println("Failed to set TS");

}

void touch_loop() {
    uint8_t touches = ctp.touched();
    // Wait for a touch
    if (! touches) {
        return; 
    }

    // Retrieve the points, up to 5!
    TS_Point ps[FT5336_MAXTOUCHES];
    ctp.getPoints(ps, FT5336_MAXTOUCHES);
    displayDrawTouch(ps);
    // for (int i=0; i<FT5336_MAXTOUCHES; i++) {
    //     // Check if z (pressure) is zero, skip if so
    //     if (ps[i].z == 0) continue;
    //     /*
    //     ps[i].x = map(ps[i].x, 0, 320, 0, 320);
    //     ps[i].y = map(ps[i].y, 0, 480, 0, 480);
    //     */
    //     // Print out the remapped/rotated coordinates
    //     Serial.print("("); Serial.print(ps[i].x);
    //     Serial.print(", "); Serial.print(ps[i].y);
    //     Serial.print(")\t");  
    // }
    // Serial.println();

}
#endif