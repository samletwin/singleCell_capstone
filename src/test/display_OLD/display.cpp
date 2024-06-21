#define USE_DISPLAY
#ifdef USE_DISPLAY

#include "SPI.h"
#include "Adafruit_HX8357.h"
#include "Adafruit_GFX.h"
#include "custom_types.h"
#include "display.h"

/* ------------------------------------------------------------------------------------------------
  DEFINES
------------------------------------------------------------------------------------------------ */
/* SPI */
// #define SPI_MOSI_PIN          23
// #define SPI_MISO_PIN          -1    // Not used  
// #define SPI_CLK_PIN           19
// #define SPI_CS_PIN            -1    // Test using - dont want to use but see if necessary
// #define SPI_DC_PIN            18

#define SPI_MOSI_PIN          10
#define SPI_MISO_PIN          -1    // Not used  
#define SPI_CLK_PIN           8
#define SPI_CS_PIN            -1    // Test using - dont want to use but see if necessary
#define SPI_DC_PIN            9


/* Diplay misc. */
// #define CONTROL_BACKLITE
#ifdef CONTROL_BACKLITE
#define PIN_DISP_BACKLITE     -1
#endif

// Size of the color selection boxes and the paintbrush size
#define BOXSIZE 40
#define PENRADIUS 3

/* ------------------------------------------------------------------------------------------------
  LOCAL VARIABLES
------------------------------------------------------------------------------------------------ */
/* Setup HX8357 - Display control */
SPIClass * spiComm_pc;
Adafruit_HX8357 tft = Adafruit_HX8357(SPI_CS_PIN, SPI_DC_PIN, SPI_MOSI_PIN, SPI_CLK_PIN,-1 ,-1 );
int oldcolor, currentcolor;

/* ------------------------------------------------------------------------------------------------
  FUNCTION PROTOTYPES
------------------------------------------------------------------------------------------------ */

void displayDiagnostics(Adafruit_HX8357* tft);

void displaySetupTouchWindow();
unsigned long testLines(uint16_t color);
unsigned long testText(Adafruit_HX8357* tft);

void displaySetup() {
    #ifdef CONTROL_BACKLITE
    pinMode(PIN_DISP_BACKLITE, OUTPUT);
    #endif
    tft = Adafruit_HX8357(SPI_CS_PIN, SPI_DC_PIN, SPI_MOSI_PIN, SPI_CLK_PIN,-1 ,-1 );
    tft.begin(HX8357D);
    delay(200);
    displayDiagnostics(&tft);
    // testText(&tft);
    displaySetupTouchWindow();
}

void displayLoop() {
    #ifdef CONTROL_BACKLITE
    digitalWrite(PIN_DISP_BACKLITE, HIGH);
    #endif
}

void displaySetupTouchWindow() {
    tft.fillScreen(HX8357_BLACK);

    // make the color selection boxes
    tft.fillRect(0, 0, BOXSIZE, BOXSIZE, HX8357_RED);
    tft.fillRect(BOXSIZE, 0, BOXSIZE, BOXSIZE, HX8357_YELLOW);
    tft.fillRect(BOXSIZE*2, 0, BOXSIZE, BOXSIZE, HX8357_GREEN);
    tft.fillRect(BOXSIZE*3, 0, BOXSIZE, BOXSIZE, HX8357_CYAN);
    tft.fillRect(BOXSIZE*4, 0, BOXSIZE, BOXSIZE, HX8357_BLUE);
    tft.fillRect(BOXSIZE*5, 0, BOXSIZE, BOXSIZE, HX8357_MAGENTA);
    tft.fillRect(BOXSIZE*6, 0, BOXSIZE, BOXSIZE, HX8357_WHITE);
    tft.fillRect(BOXSIZE*7, 0, BOXSIZE, BOXSIZE, HX8357_BLACK);

    // select the current color 'red'
    tft.drawRect(0, 0, BOXSIZE, BOXSIZE, HX8357_WHITE);
    currentcolor = HX8357_RED;
}

void displayDrawTouch(TS_Point touch[]) {

    if (touch[0].y < BOXSIZE) {
        oldcolor = currentcolor;

        if (touch[0].x < BOXSIZE) { 
        currentcolor = HX8357_RED; 
        tft.drawRect(0, 0, BOXSIZE, BOXSIZE, HX8357_WHITE);
        } else if (touch[0].x < BOXSIZE*2) {
        currentcolor = HX8357_YELLOW;
        tft.drawRect(BOXSIZE, 0, BOXSIZE, BOXSIZE, HX8357_WHITE);
        } else if (touch[0].x < BOXSIZE*3) {
        currentcolor = HX8357_GREEN;
        tft.drawRect(BOXSIZE*2, 0, BOXSIZE, BOXSIZE, HX8357_WHITE);
        } else if (touch[0].x < BOXSIZE*4) {
        currentcolor = HX8357_CYAN;
        tft.drawRect(BOXSIZE*3, 0, BOXSIZE, BOXSIZE, HX8357_WHITE);
        } else if (touch[0].x < BOXSIZE*5) {
        currentcolor = HX8357_BLUE;
        tft.drawRect(BOXSIZE*4, 0, BOXSIZE, BOXSIZE, HX8357_WHITE);
        } else if (touch[0].x <= BOXSIZE*6) {
        currentcolor = HX8357_MAGENTA;
        tft.drawRect(BOXSIZE*5, 0, BOXSIZE, BOXSIZE, HX8357_WHITE);
        } else if (touch[0].x <= BOXSIZE*7) {
        currentcolor = HX8357_WHITE;
        tft.drawRect(BOXSIZE*5, 0, BOXSIZE, BOXSIZE, HX8357_RED);
        } else {
        // erase
        tft.fillScreen(HX8357_BLACK);
        
        // make the color selection boxes
        tft.fillRect(0, 0, BOXSIZE, BOXSIZE, HX8357_RED);
        tft.fillRect(BOXSIZE, 0, BOXSIZE, BOXSIZE, HX8357_YELLOW);
        tft.fillRect(BOXSIZE*2, 0, BOXSIZE, BOXSIZE, HX8357_GREEN);
        tft.fillRect(BOXSIZE*3, 0, BOXSIZE, BOXSIZE, HX8357_CYAN);
        tft.fillRect(BOXSIZE*4, 0, BOXSIZE, BOXSIZE, HX8357_BLUE);
        tft.fillRect(BOXSIZE*5, 0, BOXSIZE, BOXSIZE, HX8357_MAGENTA);
        tft.fillRect(BOXSIZE*6, 0, BOXSIZE, BOXSIZE, HX8357_WHITE);
        }

        if (oldcolor != currentcolor) {
        if (oldcolor == HX8357_RED) 
            tft.fillRect(0, 0, BOXSIZE, BOXSIZE, HX8357_RED);
        if (oldcolor == HX8357_YELLOW) 
            tft.fillRect(BOXSIZE, 0, BOXSIZE, BOXSIZE, HX8357_YELLOW);
        if (oldcolor == HX8357_GREEN) 
            tft.fillRect(BOXSIZE*2, 0, BOXSIZE, BOXSIZE, HX8357_GREEN);
        if (oldcolor == HX8357_CYAN) 
            tft.fillRect(BOXSIZE*3, 0, BOXSIZE, BOXSIZE, HX8357_CYAN);
        if (oldcolor == HX8357_BLUE) 
            tft.fillRect(BOXSIZE*4, 0, BOXSIZE, BOXSIZE, HX8357_BLUE);
        if (oldcolor == HX8357_MAGENTA) 
            tft.fillRect(BOXSIZE*5, 0, BOXSIZE, BOXSIZE, HX8357_MAGENTA);
        if (oldcolor == HX8357_WHITE) 
            tft.fillRect(BOXSIZE*6, 0, BOXSIZE, BOXSIZE, HX8357_WHITE);
        }
    }
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

/* Draws lines on the tft. Returns time it took to do so in microseconds */
unsigned long testLines(Adafruit_HX8357* tft, uint16_t color) {
    unsigned long start;
    int x1, y1, x2, y2,
    w = tft->width(),
    h = tft->height();

    tft->fillScreen(HX8357_BLACK);

    x1 = y1 = 0;
    y2 = h - 1;
    start = micros();
    for(x2=0; x2<w; x2+=6) 
    {
        tft->drawLine(x1, y1, x2, y2, color);
    }
    x2 = w - 1;
    for(y2=0; y2<h; y2+=6) 
    {
        tft->drawLine(x1, y1, x2, y2, color);
    }
    return micros() - start;
}

/* Writes text to the tft. Returns time in microseconds it took to do so. */
unsigned long testText(Adafruit_HX8357* tft) {
    tft->fillScreen(HX8357_BLACK);
    unsigned long start = micros();
    tft->setCursor(0, 0);
    tft->setTextColor(HX8357_WHITE);  tft->setTextSize(1);
    tft->println("Hello World!");
    tft->setTextColor(HX8357_YELLOW); tft->setTextSize(2);
    tft->println(1234.56);
    tft->setTextColor(HX8357_RED);    tft->setTextSize(3);
    tft->println(0xDEADBEEF, HEX);
    tft->println();
    tft->setTextColor(HX8357_GREEN);
    tft->setTextSize(5);
    tft->println("Groop");
    tft->setTextSize(2);
    tft->println("I implore thee,");
    tft->setTextSize(1);
    tft->println("my foonting turlingdromes.");
    tft->println("And hooptiously drangle me");
    tft->println("with crinkly bindlewurdles,");
    tft->println("Or I will rend thee");
    tft->println("in the gobberwarts");
    tft->println("with my blurglecruncheon,");
    tft->println("see if I don't!");

    tft->setTextColor(HX8357_WHITE);
    tft->println(F("Alice was beginning to get very tired of sitting by her sister on the bank, and of having nothing to do: once or twice she had peeped into the book her sister was reading, but it had no pictures or conversations in it, 'and what is the use of a book,' thought Alice 'without pictures or conversations?'"));
    tft->println(F("So she was considering in her own mind (as well as she could, for the hot day made her feel very sleepy and stupid), whether the pleasure of making a daisy-chain would be worth the trouble of getting up and picking the daisies, when suddenly a White Rabbit with pink eyes ran close by her."));
    tft->println(F("There was nothing so very remarkable in that; nor did Alice think it so very much out of the way to hear the Rabbit say to itself, 'Oh dear! Oh dear! I shall be late!' (when she thought it over afterwards, it occurred to her that she ought to have wondered at this, but at the time it all seemed quite natural); but when the Rabbit actually took a watch out of its waistcoat-pocket, and looked at it, and then hurried on, Alice started to her feet, for it flashed across her mind that she had never before seen a rabbit with either a waistcoat-pocket, or a watch to take out of it, and burning with curiosity, she ran across the field after it, and fortunately was just in time to see it pop down a large rabbit-hole under the hedge."));
    tft->println(F("In another moment down went Alice after it, never once considering how in the world she was to get out again."));
    tft->println(F("The rabbit-hole went straight on like a tunnel for some way, and then dipped suddenly down, so suddenly that Alice had not a moment to think about stopping herself before she found herself falling down a very deep well."));
    tft->println(F("Either the well was very deep, or she fell very slowly, for she had plenty of time as she went down to look about her and to wonder what was going to happen next. First, she tried to look down and make out what she was coming to, but it was too dark to see anything; then she looked at the sides of the well, and noticed that they were filled with cupboards and book-shelves; here and there she saw matouch and pictures hung upon pegs. She took down a jar from one of the shelves as she passed; it was labelled 'ORANGE MARMALADE', but to her great disappointment it was empty: she did not like to drop the jar for fear of killing somebody, so managed to put it into one of the cupboards as she fell past it."));

    return micros() - start;
}
#endif