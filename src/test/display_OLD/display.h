#ifndef DISPLAY_H
#define DISPLAY_H

#include "Adafruit_FT5336.h"

/* API Functions */
void displaySetup();
void displayLoop();
void displayDrawTouch(TS_Point touch[]);

#endif 