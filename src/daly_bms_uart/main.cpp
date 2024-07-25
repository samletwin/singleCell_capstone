#include "Arduino.h"
#include "bms/daly-bms-uart.h"
#include <HardwareSerial.h>

static HardwareSerial mySerial = HardwareSerial(1); 
static Daly_BMS_UART bms(mySerial);

#define DALY_RX 5
#define DALY_TX 6

void setup() {
  /* Begin serial communication */
  mySerial.begin(9600, SERIAL_8N1, DALY_RX, DALY_TX);
  Serial.begin(115200);  // Initialize the default serial port for debugging
  Serial.println("Serial port 2 initialized with RX on pin 16 and TX on pin 17");
  delay(1000);
  
  /* Init structs */
  Serial.println("Attemtping to init daly");
  bool val = bms.Init();
  if (true != val)
    Serial.println("Error initializing daly BMS");
  else
    Serial.println("initialized daly BMS");
}

void loop() {
  #ifdef USE_WEBSERVER
  webpage_MainFunc();
  #endif
  delay(5000);
  bms.update();
  Serial.println("Basic BMS Data:              " + (String)bms.get.packVoltage + "V " + (String)bms.get.packCurrent + "I " + (String)bms.get.packSOC + "\% ");
  Serial.println("Package Temperature (C):     " + (String)bms.get.tempAverage);
  Serial.println("Highest Cell Voltage:        #" + (String)bms.get.maxCellVNum + " with voltage " + (String)(bms.get.maxCellmV / 1000));
  Serial.println("Lowest Cell Voltage:         #" + (String)bms.get.minCellVNum + " with voltage " + (String)(bms.get.minCellmV / 1000));
  Serial.println("Number of Cells:             " + (String)bms.get.numberOfCells);
}